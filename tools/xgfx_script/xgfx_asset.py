"""XGFX single-file image asset generator.

Copy this file into the image directory. Run it once to create
xgfx_assets.json, edit that file, then run this script again to build assets.
"""

from __future__ import annotations

__version__ = "1.0.0"

# ---- model ----

from dataclasses import dataclass, field
from pathlib import Path
from typing import Callable


@dataclass(frozen=True)
class Diagnostic:
    severity: str
    code: str
    message: str
    asset: str | None = None


@dataclass(frozen=True)
class AssetConfig:
    name: str
    source: Path
    image_type: str
    asset_id: str = ""
    storage: str = "MCU"
    color_format: str = "RGB565"
    byte_order: str = "little"
    bpp: int = 0
    alpha_bpp: int = 0
    palette: tuple[str, ...] = ()
    auto_quantize: bool = False
    flash_address: int | None = None


@dataclass(frozen=True)
class EncodedAsset:
    config: AssetConfig
    width: int
    height: int
    color_data: bytes
    alpha_data: bytes
    indices: tuple[int, ...] = ()
    palette_values: tuple[int, ...] = ()
    device_pixels: tuple[int, ...] = ()

    @property
    def total_size(self) -> int:
        return len(self.color_data) + len(self.alpha_data)


@dataclass
class BuildResult:
    assets: list[EncodedAsset] = field(default_factory=list)
    diagnostics: list[Diagnostic] = field(default_factory=list)
    written_files: list[Path] = field(default_factory=list)
    cancelled: bool = False

    @property
    def succeeded(self) -> bool:
        return not self.cancelled and not any(d.severity == "error" for d in self.diagnostics)


ProgressCallback = Callable[[str, int, int], None]
CancelCallback = Callable[[], bool]


# ---- codec ----

from collections.abc import Iterable, Sequence


COLOR_BYTES = {"RGB332": 1, "RGB565": 2, "RGB888": 4}


def color_to_device(rgb: tuple[int, int, int], color_format: str) -> int:
    r, g, b = rgb
    if color_format == "RGB332":
        return (r & 0xE0) | ((g & 0xE0) >> 3) | (b >> 6)
    if color_format == "RGB565":
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
    if color_format == "RGB888":
        return (r << 16) | (g << 8) | b
    raise ValueError(f"unsupported color format: {color_format}")


def device_to_rgb(value: int, color_format: str) -> tuple[int, int, int]:
    if color_format == "RGB332":
        r, g, b = (value >> 5) & 7, (value >> 2) & 7, value & 3
        return round(r * 255 / 7), round(g * 255 / 7), round(b * 255 / 3)
    if color_format == "RGB565":
        r, g, b = (value >> 11) & 31, (value >> 5) & 63, value & 31
        return round(r * 255 / 31), round(g * 255 / 63), round(b * 255 / 31)
    if color_format == "RGB888":
        return (value >> 16) & 255, (value >> 8) & 255, value & 255
    raise ValueError(f"unsupported color format: {color_format}")


def serialize_colors(values: Iterable[int], color_format: str, byte_order: str) -> bytes:
    width = COLOR_BYTES[color_format]
    return b"".join(int(v).to_bytes(width, byte_order, signed=False) for v in values)


def deserialize_colors(data: bytes, color_format: str, byte_order: str) -> tuple[int, ...]:
    width = COLOR_BYTES[color_format]
    if len(data) % width:
        raise ValueError("native color data has an incomplete pixel")
    return tuple(int.from_bytes(data[i:i + width], byte_order) for i in range(0, len(data), width))


def pack_rows(values: Sequence[int], width: int, height: int, bpp: int) -> bytes:
    if not 1 <= bpp <= 5:
        raise ValueError("BPP must be between 1 and 5")
    if len(values) != width * height:
        raise ValueError("pixel count does not match width and height")
    row_bytes = (width * bpp + 7) // 8
    output = bytearray(row_bytes * height)
    mask = (1 << bpp) - 1
    for y in range(height):
        for x in range(width):
            value = values[y * width + x]
            if value < 0 or value > mask:
                raise ValueError(f"value {value} does not fit in {bpp} BPP")
            bit_position = x * bpp
            for bit in range(bpp):
                if (value >> bit) & 1:
                    absolute = y * row_bytes * 8 + bit_position + bit
                    output[absolute // 8] |= 1 << (absolute % 8)
    return bytes(output)


def unpack_rows(data: bytes, width: int, height: int, bpp: int) -> tuple[int, ...]:
    row_bytes = (width * bpp + 7) // 8
    if len(data) != row_bytes * height:
        raise ValueError("packed data length does not match dimensions")
    values: list[int] = []
    for y in range(height):
        for x in range(width):
            bit_position = x * bpp
            value = 0
            for bit in range(bpp):
                absolute = y * row_bytes * 8 + bit_position + bit
                value |= ((data[absolute // 8] >> (absolute % 8)) & 1) << bit
            values.append(value)
    return tuple(values)


def quantize_alpha(values: Sequence[int], bpp: int) -> tuple[int, ...]:
    maximum = (1 << bpp) - 1
    return tuple((v * maximum + 127) // 255 for v in values)


def parse_rgb(text: str) -> tuple[int, int, int]:
    value = text.strip().removeprefix("#")
    if len(value) != 6:
        raise ValueError(f"invalid RGB color: {text}")
    number = int(value, 16)
    return (number >> 16) & 255, (number >> 8) & 255, number & 255


def nearest_palette_index(rgb: tuple[int, int, int], palette: Sequence[tuple[int, int, int]]) -> int:
    candidates = range(len(palette))
    # Achromatic antialiasing must not acquire a saturated palette hue simply
    # because that hue is closer in RGB distance. Allow small device rounding
    # differences (e.g. RGB565 gray) when identifying neutral colors.
    if max(rgb) - min(rgb) <= 12:
        neutral = [i for i, color in enumerate(palette) if max(color) - min(color) <= 12]
        if neutral:
            candidates = neutral
    return min(candidates, key=lambda i: sum((rgb[c] - palette[i][c]) ** 2 for c in range(3)))


# ---- emit ----

import json
from pathlib import Path

from PIL import Image



def _atomic_bytes(path: Path, data: bytes) -> None:
    temporary = path.with_name(path.name + ".tmp")
    try:
        temporary.write_bytes(data)
        temporary.replace(path)
    finally:
        if temporary.exists():
            temporary.unlink()


def _atomic_text(path: Path, text: str) -> None:
    _atomic_bytes(path, text.encode("utf-8"))


def _bytes(name: str, data: bytes) -> str:
    lines = []
    for start in range(0, len(data), 12):
        lines.append("    " + ", ".join(f"0x{value:02X}" for value in data[start:start + 12]) + ",")
    return f"GFX_COLOR_ALIGNAS static const uint8_t {name}[] = {{\n" + "\n".join(lines) + "\n};\n"


def _descriptor(asset: EncodedAsset) -> str:
    c = asset.config
    color_type = f"GFX_COLOR_TYPE_{c.image_type}"
    save_way = "GFX_Save_Way_MCU" if c.storage == "MCU" else "GFX_Save_Way_Flash"
    if c.storage == "MCU":
        color_ref = f".C_Array = (uint8_t *){c.name}_Color"
        alpha_ref = f".C_Array = (uint8_t *){c.name}_Alpha" if asset.alpha_data else ".C_Array = NULL"
    else:
        color_ref = f".Flash_Addr = 0x{c.flash_address:08X}u"
        alpha_address = c.flash_address + len(asset.color_data)
        alpha_ref = f".Flash_Addr = 0x{alpha_address:08X}u" if asset.alpha_data else ".Flash_Addr = 0u"
    return f"""const GFX_Img_t {c.name} = {{
    .W = {asset.width}, .H = {asset.height},
    .Color_Type = {color_type},
    .Color_Save_Way = {save_way},
    .Color_Bits_Per_Pix = {c.bpp},
    .Alpha_Enable = {1 if asset.alpha_data else 0},
    .Alpha_Save_Way = {save_way},
    .Alpha_Bits_Per_Pix = {c.alpha_bpp},
    .Color_Save_Info = {{ {color_ref} }},
    .Alpha_Save_Info = {{ {alpha_ref} }}
}};
"""


def _preview(asset: EncodedAsset, path: Path) -> None:
    c = asset.config
    if c.image_type == "MIRROR":
        rgb = [device_to_rgb(value, c.color_format) for value in asset.device_pixels]
    elif c.image_type == "BITMAP":
        maximum = (1 << c.bpp) - 1
        rgb = [(round(v * 255 / maximum),) * 3 for v in asset.indices]
    else:
        palette = [device_to_rgb(value, c.color_format) for value in asset.palette_values]
        rgb = [palette[index] for index in asset.indices]
    image = Image.new("RGBA", (asset.width, asset.height))
    alpha = [255] * (asset.width * asset.height)
    if c.alpha_bpp:
        packed = unpack_rows(asset.alpha_data, asset.width, asset.height, c.alpha_bpp)
        maximum = (1 << c.alpha_bpp) - 1
        alpha = [round(value * 255 / maximum) for value in packed]
    image.putdata([(r, g, b, a) for (r, g, b), a in zip(rgb, alpha)])
    temporary = path.with_name(path.stem + ".tmp" + path.suffix)
    try:
        image.save(temporary)
        temporary.replace(path)
    finally:
        if temporary.exists():
            temporary.unlink()


def emit_project(manifest_path: Path, raw: dict, assets: list[EncodedAsset]) -> list[Path]:
    output = raw.get("output", {})
    directory = (manifest_path.parent / output.get("directory", "generated")).resolve()
    directory.mkdir(parents=True, exist_ok=True)
    stem = output.get("name", "gfx_assets")
    write_individual_bins = bool(output.get("write_individual_bins", True))
    write_previews = bool(output.get("write_previews", True))
    code_directory = (manifest_path.parent / output.get("code_directory", output.get("directory", "generated"))).resolve()
    code_directory.mkdir(parents=True, exist_ok=True)
    header_path, source_path = code_directory / f"{stem}.h", code_directory / f"{stem}.c"
    guard = f"_{stem.upper()}_H_"
    header = [f"#ifndef {guard}", f"#define {guard}", "", '#include "GFX.h"', ""]
    source = [f'#include "{header_path.name}"', ""]
    report_assets = []
    written: list[Path] = []
    flash_assets = [asset for asset in assets if asset.config.storage == "FLASH"]
    flash_info = None
    if flash_assets:
        flash = raw.get("flash", {})
        base = int(str(flash.get("base_address", 0)), 0)
        fill_byte = int(flash.get("fill_byte", 255))
        if not 0 <= fill_byte <= 255:
            raise ValueError("flash fill_byte must be between 0 and 255")
        end = max(asset.config.flash_address + asset.total_size for asset in flash_assets)
        if end < base:
            raise ValueError("Flash resource address is below flash base_address")
        bundle = bytearray([fill_byte]) * (end - base)
        for asset in flash_assets:
            offset = asset.config.flash_address - base
            if offset < 0:
                raise ValueError(f"{asset.config.name} is below flash base_address")
            bundle[offset:offset + asset.total_size] = asset.color_data + asset.alpha_data
        flash_path = directory / output.get("flash_file", f"{stem}.bin")
        _atomic_bytes(flash_path, bytes(bundle))
        written.append(flash_path)
        flash_info = {"file": flash_path.name, "base_address": base, "size": len(bundle)}
    for asset in assets:
        c = asset.config
        header.extend([f"GFX_IMG_DECLEARE( {c.name} )", f"#define {c.name.upper()}_COLOR_SIZE ({len(asset.color_data)}u)"])
        if asset.alpha_data:
            header.append(f"#define {c.name.upper()}_ALPHA_SIZE ({len(asset.alpha_data)}u)")
        if c.storage == "MCU":
            source.append(_bytes(f"{c.name}_Color", asset.color_data))
            if asset.alpha_data: source.append(_bytes(f"{c.name}_Alpha", asset.alpha_data))
        if write_individual_bins:
            binary_path = directory / f"{c.name}.bin"
            _atomic_bytes(binary_path, asset.color_data + asset.alpha_data)
            written.append(binary_path)
        if write_previews:
            preview_path = directory / f"{c.name}_preview.png"
            _preview(asset, preview_path)
            written.append(preview_path)
        source.append(_descriptor(asset))
        report_assets.append({
            "id": c.asset_id or c.name, "name": c.name, "source": str(c.source),
            "type": c.image_type, "storage": c.storage,
            "width": asset.width, "height": asset.height, "bpp": c.bpp, "alpha_bpp": c.alpha_bpp,
            "color_size": len(asset.color_data), "alpha_size": len(asset.alpha_data),
            "total_size": asset.total_size, "flash_address": c.flash_address,
        })
    header.extend(["", f"#endif /* {guard} */", ""])
    _atomic_text(header_path, "\n".join(header))
    _atomic_text(source_path, "\n".join(source))
    report_path = directory / f"{stem}_report.json"
    _atomic_text(report_path, json.dumps({"generator": "xgfx_asset/0.2", "flash": flash_info,
                                         "assets": report_assets},
                                         ensure_ascii=False, indent=2) + "\n")
    return [header_path, source_path, report_path, *written]


# ---- pipeline ----

import json
import os
import re
from dataclasses import replace
from pathlib import Path
from typing import Any

from PIL import Image



_C_NAME = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")
_IMAGE_TYPES = {"MIRROR", "BITMAP", "BITMAP_WITH_PALETTE"}


def _integer(value: Any) -> int:
    if isinstance(value, str):
        return int(value, 0)
    return int(value)


def effective_image_settings(raw: dict, entry: dict) -> dict:
    defaults = raw.get("defaults", {})
    item = {**defaults, **entry}
    if raw.get("color_mode", "free") == "fixed":
        item["color_format"] = defaults.get("color_format", "RGB565")
    image_type = item.get("type", "MIRROR")
    if image_type == "MIRROR":
        item["bpp"] = 0
    elif "bpp" not in entry and not int(item.get("bpp", 0)):
        item["bpp"] = 1
    if image_type == "BITMAP":
        # Explicit invalid overrides still reach validation in hand-written manifests.
        if "alpha_bpp" not in entry: item["alpha_bpp"] = 0
    return item


def load_project(path: str | Path) -> tuple[dict[str, Any], list[AssetConfig]]:
    manifest_path = Path(path).resolve()
    raw = json.loads(manifest_path.read_text(encoding="utf-8"))
    schema_version = int(raw.get("schema_version", 1))
    if schema_version != 1:
        raise ValueError(f"unsupported manifest schema_version: {schema_version}")
    defaults = raw.get("defaults", {})
    assets: list[AssetConfig] = []
    for entry in raw.get("assets", []):
        item = effective_image_settings(raw, entry)
        source = (manifest_path.parent / item["source"]).resolve()
        assets.append(AssetConfig(
            name=item["name"], source=source,
            image_type=item["type"].upper(), asset_id=str(item.get("id", item["name"])),
            storage=item.get("storage", "MCU").upper(),
            color_format=item.get("color_format", "RGB565").upper(),
            byte_order=item.get("byte_order", "little").lower(),
            bpp=_integer(item.get("bpp", 0)), alpha_bpp=_integer(item.get("alpha_bpp", 0)),
            palette=tuple(item.get("palette", [])), auto_quantize=bool(item.get("auto_quantize", False)),
            flash_address=_integer(item["flash_address"]) if item.get("flash_address") is not None else None,
        ))
    return raw, assets


def validate_config(config: AssetConfig, allow_auto_flash: bool = False) -> list[Diagnostic]:
    errors: list[Diagnostic] = []
    def error(code: str, message: str) -> None:
        errors.append(Diagnostic("error", code, message, config.name))
    if not _C_NAME.fullmatch(config.name): error("name", "name must be a valid C identifier")
    if config.image_type not in _IMAGE_TYPES: error("type", f"unsupported image type: {config.image_type}")
    if config.storage not in {"MCU", "FLASH"}: error("storage", "storage must be MCU or FLASH")
    if config.color_format not in COLOR_BYTES: error("color_format", "color format must be RGB332, RGB565 or RGB888")
    if config.byte_order not in {"little", "big"}: error("byte_order", "byte order must be little or big")
    if config.image_type != "MIRROR" and not 1 <= config.bpp <= 5: error("bpp", "indexed images require BPP from 1 to 5")
    if config.image_type == "MIRROR" and config.bpp != 0: error("bpp", "MIRROR BPP must be 0")
    if config.image_type == "BITMAP" and config.alpha_bpp: error("alpha", "BITMAP cannot contain independent Alpha")
    if config.alpha_bpp and not 1 <= config.alpha_bpp <= 5: error("alpha_bpp", "Alpha BPP must be from 1 to 5")
    if config.image_type != "BITMAP_WITH_PALETTE" and config.palette: error("palette", "only BITMAP_WITH_PALETTE accepts a palette")
    if config.palette and len(config.palette) > (1 << config.bpp): error("palette", "palette has more entries than BPP permits")
    if config.storage == "FLASH" and config.flash_address is None and not allow_auto_flash:
        error("flash_address", "FLASH storage requires flash_address or project flash auto-layout")
    if not config.source.is_file(): error("source", f"source file does not exist: {config.source}")
    return errors


def _dominant_palette(pixels, maximum, weights=None):
    """Choose actual source colors, preserving solid UI colors instead of averaging edges."""
    from collections import Counter
    histogram = Counter()
    for i, color in enumerate(pixels):
        histogram[color] += weights[i] if weights is not None else 1
    ranked = sorted(((c, n) for c, n in histogram.items() if n > 0), key=lambda x: (-x[1], x[0]))
    if not ranked: return [(0, 0, 0)]
    if len(ranked) <= maximum: return [c for c, _ in ranked]
    # Bound the candidate set for photographs; keep all meaningful flat UI colors.
    candidates = ranked[:4096]
    palette = [candidates[0][0]]
    distance = lambda a, b: sum((a[j]-b[j])**2 for j in range(3))
    nearest = [distance(c, palette[0]) for c, _ in candidates]
    for _ in range(1, maximum):
        index = max(range(len(candidates)), key=lambda i: candidates[i][1] * nearest[i])
        if nearest[index] == 0: break
        color = candidates[index][0]
        palette.append(color)
        nearest = [min(d, distance(c, color)) for d, (c, _) in zip(nearest, candidates)]
    # Refine each cluster to its most frequent ORIGINAL color, never a blended centroid.
    for _ in range(3):
        groups = [[] for _ in palette]
        for color, count in candidates:
            groups[nearest_palette_index(color, palette)].append((color, count))
        refined = [max(group, key=lambda x: x[1])[0] if group else palette[i]
                   for i, group in enumerate(groups)]
        if refined == palette: break
        palette = refined
    return palette


def _palette_indices(rgb_pixels: list[tuple[int, int, int]], config: AssetConfig,
                     alpha=None) -> tuple[tuple[int, ...], tuple[int, ...]]:
    maximum = 1 << config.bpp
    if config.palette:
        rgb_palette = [parse_rgb(item) for item in config.palette]
    else:
        rgb_palette = _dominant_palette(rgb_pixels, maximum, alpha if config.alpha_bpp else None)
    # Compare against representable device colors, including RGB565 rounding.
    device_palette = list(dict.fromkeys(color_to_device(rgb, config.color_format) for rgb in rgb_palette))
    rendered_palette = [device_to_rgb(value, config.color_format) for value in device_palette]
    mapping = {rgb: nearest_palette_index(rgb, rendered_palette) for rgb in set(rgb_pixels)}
    indices = tuple(mapping[rgb] for rgb in rgb_pixels)
    device_palette.extend([device_palette[0]] * (maximum - len(device_palette)))
    return indices, tuple(device_palette)


def encode_asset(config: AssetConfig) -> EncodedAsset:
    with Image.open(config.source) as opened:
        image = opened.convert("RGBA")
    width, height = image.size
    if width < 1 or height < 1 or width > 1023 or height > 1023:
        raise ValueError("image dimensions must be between 1 and 1023")
    rgba = list(image.get_flattened_data())
    rgb = [(r, g, b) for r, g, b, _ in rgba]
    alpha = [a for _, _, _, a in rgba]
    alpha_data = b""
    if config.alpha_bpp:
        alpha_data = pack_rows(quantize_alpha(alpha, config.alpha_bpp), width, height, config.alpha_bpp)

    if config.image_type == "MIRROR":
        pixels = tuple(color_to_device(value, config.color_format) for value in rgb)
        return EncodedAsset(config, width, height,
                            serialize_colors(pixels, config.color_format, config.byte_order), alpha_data,
                            device_pixels=pixels)
    if config.image_type == "BITMAP":
        maximum = (1 << config.bpp) - 1
        indices = tuple((round((r * 299 + g * 587 + b * 114) * maximum / 255000)) for r, g, b in rgb)
        return EncodedAsset(config, width, height, pack_rows(indices, width, height, config.bpp), b"", indices=indices)

    indices, palette = _palette_indices(rgb, config, alpha)
    packed = pack_rows(indices, width, height, config.bpp)
    color_data = packed + serialize_colors(palette, config.color_format, config.byte_order)
    return EncodedAsset(config, width, height, color_data, alpha_data, indices=indices, palette_values=palette)


def verify_asset(asset: EncodedAsset) -> None:
    config = asset.config
    if config.image_type == "MIRROR":
        if deserialize_colors(asset.color_data, config.color_format, config.byte_order) != asset.device_pixels:
            raise ValueError("MIRROR round-trip verification failed")
    else:
        row_size = ((asset.width * config.bpp + 7) // 8) * asset.height
        if unpack_rows(asset.color_data[:row_size], asset.width, asset.height, config.bpp) != asset.indices:
            raise ValueError("index round-trip verification failed")
        if config.image_type == "BITMAP_WITH_PALETTE":
            decoded = deserialize_colors(asset.color_data[row_size:], config.color_format, config.byte_order)
            if decoded != asset.palette_values:
                raise ValueError("palette round-trip verification failed")
    if config.alpha_bpp:
        unpack_rows(asset.alpha_data, asset.width, asset.height, config.alpha_bpp)


class AssetBuildService:
    """UI-neutral build service. CLI and future GUI call this same API."""

    def convert(self, config: AssetConfig, allow_auto_flash: bool = False) -> BuildResult:
        """Convert and verify one asset without writing files; intended for GUI live preview."""
        result = BuildResult()
        result.diagnostics.extend(validate_config(config, allow_auto_flash=allow_auto_flash))
        if not result.succeeded:
            return result
        try:
            asset = encode_asset(config)
            verify_asset(asset)
            result.assets.append(asset)
        except Exception as exc:
            result.diagnostics.append(Diagnostic("error", "convert", str(exc), config.name))
        return result

    def build(self, manifest_path: str | Path, progress: ProgressCallback | None = None,
              is_cancelled: CancelCallback | None = None) -> BuildResult:
        result = BuildResult()
        try:
            raw, configs = load_project(manifest_path)
        except Exception as exc:
            result.diagnostics.append(Diagnostic("error", "manifest", str(exc)))
            return result
        names: set[str] = set()
        allow_auto_flash = "flash" in raw
        total = len(configs)
        for index, config in enumerate(configs, 1):
            if is_cancelled and is_cancelled():
                result.cancelled = True
                return result
            if progress: progress(config.name, index - 1, total)
            diagnostics = validate_config(config, allow_auto_flash=allow_auto_flash)
            if config.name in names:
                diagnostics.append(Diagnostic("error", "duplicate_name", "duplicate asset name", config.name))
            names.add(config.name)
            result.diagnostics.extend(diagnostics)
            if any(item.severity == "error" for item in diagnostics):
                continue
            try:
                asset = encode_asset(config)
                verify_asset(asset)
                result.assets.append(asset)
            except Exception as exc:
                result.diagnostics.append(Diagnostic("error", "convert", str(exc), config.name))
        if result.succeeded and allow_auto_flash:
            try:
                flash = raw.get("flash", {})
                cursor = _integer(flash.get("base_address", 0))
                alignment = _integer(flash.get("alignment", 1))
                if alignment < 1 or alignment & (alignment - 1):
                    raise ValueError("flash alignment must be a positive power of two")
                ranges: list[tuple[int, int, str]] = []
                laid_out: list[EncodedAsset] = []
                for asset in result.assets:
                    if asset.config.storage != "FLASH":
                        laid_out.append(asset)
                        continue
                    address = asset.config.flash_address
                    if address is None:
                        address = (cursor + alignment - 1) & ~(alignment - 1)
                    end = address + asset.total_size
                    for used_start, used_end, used_name in ranges:
                        if address < used_end and used_start < end:
                            raise ValueError(f"Flash resources overlap: {asset.config.name} and {used_name}")
                    ranges.append((address, end, asset.config.name))
                    cursor = max(cursor, end)
                    laid_out.append(replace(asset, config=replace(asset.config, flash_address=address)))
                result.assets = laid_out
            except Exception as exc:
                result.diagnostics.append(Diagnostic("error", "flash_layout", str(exc)))
        if result.succeeded:
            try:
                result.written_files.extend(emit_project(Path(manifest_path).resolve(), raw, result.assets))
            except Exception as exc:
                result.diagnostics.append(Diagnostic("error", "output", str(exc)))
        if progress: progress("done", total, total)
        return result


# ---- initialize ----

import json
import re
from pathlib import Path


IMAGE_EXTENSIONS = {".png", ".jpg", ".jpeg", ".bmp", ".gif", ".tif", ".tiff", ".webp"}


def _identifier(relative_path: Path) -> str:
    parts = list(relative_path.with_suffix("").parts)
    cleaned: list[str] = []
    for part in parts:
        value = re.sub(r"[^A-Za-z0-9_]+", "_", part).strip("_")
        value = re.sub(r"_+", "_", value)
        if value:
            cleaned.append(value)
    name = "_".join(cleaned) or "Image"
    if name[0].isdigit():
        name = "Image_" + name
    return "Img_" + name


def initialize_project(directory: str | Path, manifest_name: str = "xgfx_assets.json", *,
                       storage: str = "FLASH", color_format: str = "RGB565",
                       byte_order: str = "little", output_directory: str = "generated",
                       code_directory: str | None = None,
                       force: bool = False) -> tuple[Path, int]:
    root = Path(directory).resolve()
    if not root.is_dir():
        raise ValueError(f"image directory does not exist: {root}")
    manifest = Path(manifest_name)
    if not manifest.is_absolute():
        manifest = root / manifest
    manifest = manifest.resolve()
    if manifest.exists() and not force:
        raise FileExistsError(f"manifest already exists: {manifest}; use --force to replace it")
    excluded = (manifest.parent / output_directory).resolve()
    images = []
    for path in root.rglob("*"):
        if not path.is_file() or path.suffix.lower() not in IMAGE_EXTENSIONS:
            continue
        resolved = path.resolve()
        if resolved == excluded or excluded in resolved.parents:
            continue
        relative = path.relative_to(root)
        source = Path(os.path.relpath(path, manifest.parent)).as_posix()
        images.append((source, _identifier(relative)))
    images.sort(key=lambda item: item[0].casefold())
    used: dict[str, int] = {}
    entries = []
    for source, base_name in images:
        count = used.get(base_name, 0)
        used[base_name] = count + 1
        name = base_name if count == 0 else f"{base_name}_{count}"
        entries.append({"id": source, "name": name, "source": source})
    project = {
        "schema_version": 1,
        "color_mode": "free",
        "defaults": {
            "type": "MIRROR", "storage": storage.upper(),
            "color_format": color_format.upper(), "byte_order": byte_order.lower(),
            "bpp": 0, "alpha_bpp": 0,
        },
        "flash": {"base_address": "0x0", "alignment": 1, "fill_byte": 255},
        "output": {
            "directory": output_directory, "code_directory": code_directory or output_directory,
            "name": "gfx_assets", "flash_file": "gfx_assets.bin",
            "write_individual_bins": False, "write_previews": False,
        },
        "assets": entries,
    }
    temporary = manifest.with_name(manifest.name + ".tmp")
    try:
        temporary.write_text(json.dumps(project, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
        temporary.replace(manifest)
    finally:
        if temporary.exists(): temporary.unlink()
    return manifest, len(entries)



# Shared project rules for the standalone script, CLI and desktop GUI.
def discover_configs(root):
    folder = Path(root).resolve() / '.xgfx'
    configs = []
    for path in sorted(folder.glob('*.json')):
        try:
            data = json.loads(path.read_text(encoding='utf-8-sig'))
            if isinstance(data, dict) and isinstance(data.get('assets'), list) and data.get('schema_version') == 1:
                configs.append(path)
        except (ValueError, OSError):
            pass
    return configs


def select_config(root, explicit=None):
    root = Path(root).resolve()
    if explicit:
        path = Path(explicit).resolve()
        if path.parent != root / '.xgfx':
            raise ValueError('配置必须位于当前项目直属的 .xgfx 文件夹中。')
        if not path.is_file(): raise ValueError('配置文件不存在。')
        return path
    configs = discover_configs(root)
    if len(configs) == 1: return configs[0]
    if not configs: raise ValueError('未发现配置，请先运行 xgfx init。')
    raise ValueError('存在多个配置，请用 --asset-json 指定：' + ', '.join(p.name for p in configs))


def _copy_asset_script(folder):
    """把安装目录（或脚本自身）的 xgfx_asset.py 复制到项目 .xgfx 里，
    方便用户双击该文件快速构建资产。"""
    import sys as _sys
    if getattr(_sys, 'frozen', False):
        src = Path(_sys.executable).resolve().parent / 'xgfx_asset.py'
    else:
        src = Path(__file__).resolve()
    if src.is_file():
        try:
            import shutil
            shutil.copy2(src, folder / 'xgfx_asset.py')
        except OSError:
            pass  # 复制失败不阻塞 init 主流程


def init_project(root):
    root = Path(root).resolve()
    folder = root / '.xgfx'
    folder.mkdir(exist_ok=True)
    target = folder / 'xgfx_assets.json'
    # init 同时负责把安装目录中的最新版独立脚本同步到项目。
    _copy_asset_script(folder)
    if target.exists(): return target
    initialize_project(root, manifest_name=target, output_directory='.', code_directory='.')
    return target


def scan_project(root, data, config_folder=None):
    root = Path(root).resolve()
    folder = Path(config_folder).resolve() if config_folder else root / '.xgfx'
    excluded = {folder}
    for key in ('directory', 'code_directory'):
        excluded.add((folder / data.get('output', {}).get(key, '.')).resolve())
    existing = {a['source']: a for a in data.get('assets', [])}
    result, used = [], {a.get('name') for a in existing.values()}
    for path in sorted(root.rglob('*')):
        if not path.is_file() or path.suffix.lower() not in IMAGE_EXTENSIONS: continue
        if any(ex == path.parent or ex in path.parents for ex in excluded): continue
        source = Path(os.path.relpath(path, folder)).as_posix()
        if source in existing: result.append(existing[source]); continue
        base = _identifier(path.relative_to(root)); name = base; suffix = 1
        while name in used:
            name = f'{base}_{suffix}'; suffix += 1
        used.add(name)
        result.append({'id': source, 'source': source, 'name': name})
    return result


def save_project(root, data):
    manifest = Path(root).resolve() / '.xgfx' / 'xgfx_assets.json'
    if not manifest.is_file():
        raise ValueError('未发现配置，请先运行 xgfx init。')
    _atomic_text(manifest, json.dumps(data, ensure_ascii=False, indent=2) + '\n')
    return manifest


def reset_project(root, data):
    assets = [
        {key: asset[key] for key in ('id', 'source', 'name') if key in asset}
        for asset in data.get('assets', [])
    ]
    reset = {
        'schema_version': int(data.get('schema_version', 1)),
        'color_mode': 'free',
        'defaults': {
            'type': 'MIRROR', 'storage': 'FLASH', 'color_format': 'RGB565',
            'bpp': 0, 'alpha_bpp': 0, 'byte_order': 'little',
        },
        'flash': {'base_address': '0x0', 'alignment': 1, 'fill_byte': 255},
        'output': {
            'directory': '.', 'code_directory': '.', 'name': 'gfx_assets',
            'flash_file': 'gfx_assets.bin', 'write_individual_bins': False,
            'write_previews': False,
        },
        'assets': assets,
    }
    save_project(root, reset)
    return reset


def cli(argv=None):
    import argparse
    import sys
    parser = argparse.ArgumentParser(prog='xgfx')
    parser.add_argument('-v', '--version', action='version', version=f'xgfx {__version__}')
    parser.add_argument('command', choices=['init', 'scan', 'build', 'reset', 'version'], nargs='?')
    args = parser.parse_args(argv)
    root = Path.cwd()
    script_dir = Path(__file__).resolve().parent
    if script_dir.name == '.xgfx':
        root = script_dir.parent
    try:
        if args.command is None and not getattr(sys, 'frozen', False):
            # 独立脚本固定使用：脚本目录存配置，其上一级目录存图片。
            root = script_dir.parent
            manifest = script_dir / 'xgfx_assets.json'
            if not manifest.is_file():
                initialize_project(root, manifest_name=manifest,
                                   output_directory='.', code_directory='.')
                print(f'Created {manifest}')
            data = json.loads(manifest.read_text(encoding='utf-8-sig'))
            before = {asset.get('source') for asset in data.get('assets', [])}
            data['assets'] = scan_project(root, data, config_folder=script_dir)
            after = {asset.get('source') for asset in data['assets']}
            _atomic_text(manifest, json.dumps(data, ensure_ascii=False, indent=2) + '\n')
            print(f'Scanned {len(after)} images; removed {len(before-after)} missing entries.')
            result = AssetBuildService().build(manifest)
            for diagnostic in result.diagnostics:
                print(f'{diagnostic.severity}: {diagnostic.asset or "project"}: {diagnostic.message}')
            if result.succeeded:
                print(f'Built {len(result.assets)} images, wrote {len(result.written_files)} files.')
            return 0 if result.succeeded else 1
        if args.command == 'init':
            print(init_project(root)); return 0
        if args.command == 'version':
            print(f'xgfx {__version__}'); return 0
        if args.command in ('scan', 'reset'):
            manifest = select_config(root)
            data = json.loads(manifest.read_text(encoding='utf-8-sig'))
            if args.command == 'scan':
                before = {asset.get('source') for asset in data.get('assets', [])}
                data['assets'] = scan_project(root, data)
                save_project(root, data)
                after = {asset.get('source') for asset in data['assets']}
                print(f'Scanned {len(after)} images; removed {len(before-after)} missing entries.')
            else:
                reset_project(root, data)
                print(f'Reset configuration for {len(data.get("assets", []))} images.')
            return 0
        if args.command == 'build':
            result = AssetBuildService().build(select_config(root))
            for d in result.diagnostics: print(f'{d.severity}: {d.asset or "project"}: {d.message}')
            if result.succeeded: print(f'Built {len(result.assets)} images, wrote {len(result.written_files)} files.')
            return 0 if result.succeeded else 1
        from desktop import launch
        return launch(root)
    except Exception as exc:
        print(str(exc), file=sys.stderr); return 1


if __name__ == '__main__':
    raise SystemExit(cli())

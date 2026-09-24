from __future__ import annotations

import json
import importlib.util
import sys
import tempfile
import unittest
from pathlib import Path

from PIL import Image

ROOT = Path(__file__).resolve().parents[1]
MODULE_PATH = ROOT / "tools" / "xgfx_script" / "xgfx_asset.py"
SPEC = importlib.util.spec_from_file_location("xgfx_asset_single", MODULE_PATH)
assert SPEC is not None and SPEC.loader is not None
xgfx_asset = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = xgfx_asset
SPEC.loader.exec_module(xgfx_asset)
AssetBuildService = xgfx_asset.AssetBuildService
initialize_project = xgfx_asset.initialize_project
pack_rows = xgfx_asset.pack_rows
unpack_rows = xgfx_asset.unpack_rows


class CodecTests(unittest.TestCase):
    def test_known_two_bpp_byte(self) -> None:
        self.assertEqual(pack_rows((0, 1, 2, 3), 4, 1, 2), bytes((0xE4,)))

    def test_every_bpp_round_trip_with_row_padding(self) -> None:
        for bpp in range(1, 6):
            maximum = (1 << bpp) - 1
            values = tuple((i * 3) & maximum for i in range(15))
            packed = pack_rows(values, 5, 3, bpp)
            self.assertEqual(unpack_rows(packed, 5, 3, bpp), values)


class BuildTests(unittest.TestCase):
    def test_gray_antialiasing_does_not_map_to_blue(self) -> None:
        palette = [(0, 121, 255), (0, 0, 0), (255, 255, 255), (247, 255, 0)]
        for level in range(256):
            self.assertIn(xgfx_asset.nearest_palette_index((level,) * 3, palette), (1, 2))
        self.assertEqual(xgfx_asset.nearest_palette_index(palette[0], palette), 0)
        self.assertEqual(xgfx_asset.nearest_palette_index(palette[3], palette), 3)
        self.assertIn(xgfx_asset.nearest_palette_index((128,) * 3, [palette[0], palette[3]]), (0, 1))

    def test_quantization_preserves_four_dominant_colors_without_opt_in(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / "ui.png"
            main = [(255, 0, 0), (0, 255, 0), (0, 0, 255), (255, 255, 255)]
            pixels = [c for c in main for _ in range(100)]
            pixels += [(i, i, i) for i in range(1, 201)]
            image = Image.new("RGB", (30, 20))
            image.putdata(pixels)
            image.save(path)
            config = xgfx_asset.AssetConfig(name="UI", source=path, image_type="BITMAP_WITH_PALETTE",
                                           color_format="RGB888", bpp=2, auto_quantize=False)
            result = AssetBuildService().convert(config)
            self.assertTrue(result.succeeded, result.diagnostics)
            self.assertEqual(set(result.assets[0].palette_values),
                             {xgfx_asset.color_to_device(c, "RGB888") for c in main})

    def test_fixed_color_overrides_per_image_only_when_enabled(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            Image.new("RGB", (2, 1), (255, 80, 0)).save(root / "input.png")
            raw = {"defaults": {"type": "MIRROR", "color_format": "RGB565"},
                   "output": {"directory": "out"},
                   "assets": [{"name": "Icon", "source": "input.png", "color_format": "RGB332"}]}
            manifest = root / "xgfx_assets.json"
            for mode, expected_size in (("free", 2), ("fixed", 4)):
                raw["color_mode"] = mode
                manifest.write_text(json.dumps(raw), encoding="utf-8")
                result = AssetBuildService().build(manifest)
                self.assertTrue(result.succeeded, result.diagnostics)
                self.assertEqual(len(result.assets[0].color_data), expected_size)
            self.assertEqual(raw["assets"][0]["color_format"], "RGB332")

    def test_preview_conversion_allows_project_flash_auto_layout(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            image_path = Path(temporary) / "icon.png"
            Image.new("RGB", (1, 1), (255, 0, 0)).save(image_path)
            config = xgfx_asset.AssetConfig(name="Icon", source=image_path, image_type="MIRROR",
                                            storage="FLASH", color_format="RGB565")
            rejected = AssetBuildService().convert(config)
            accepted = AssetBuildService().convert(config, allow_auto_flash=True)
            self.assertFalse(rejected.succeeded)
            self.assertTrue(accepted.succeeded, accepted.diagnostics)

    def test_tool_folder_manifest_and_outputs_with_parent_images(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            tool = root / "xgfx_asset"
            tool.mkdir()
            Image.new("RGB", (1, 1), (255, 255, 0)).save(root / "icon.png")
            manifest, count = initialize_project(root, manifest_name=tool / "xgfx_assets.json",
                                                 output_directory=".", code_directory=".")
            self.assertEqual(count, 1)
            config = json.loads(manifest.read_text(encoding="utf-8"))
            self.assertEqual(config["assets"][0]["source"], "../icon.png")
            result = AssetBuildService().build(manifest)
            self.assertTrue(result.succeeded, result.diagnostics)
            self.assertTrue((tool / "gfx_assets.c").is_file())
            self.assertTrue((tool / "gfx_assets.bin").is_file())

    def test_manifest_builds_three_formats_and_alpha(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            image = Image.new("RGBA", (3, 2))
            image.putdata([
                (255, 0, 0, 255), (0, 255, 0, 128), (0, 0, 255, 0),
                (255, 0, 0, 255), (0, 255, 0, 128), (0, 0, 255, 0),
            ])
            image.save(root / "input.png")
            manifest = {
                "schema_version": 1,
                "defaults": {"color_format": "RGB565", "byte_order": "little"},
                "output": {"directory": "generated", "name": "assets"},
                "assets": [
                    {"name": "Mirror", "source": "input.png", "type": "MIRROR", "storage": "MCU", "alpha_bpp": 2},
                    {"name": "Bitmap", "source": "input.png", "type": "BITMAP", "storage": "MCU", "bpp": 3},
                    {"name": "Palette", "source": "input.png", "type": "BITMAP_WITH_PALETTE", "storage": "MCU", "bpp": 2,
                     "palette": ["#FF0000", "#00FF00", "#0000FF"], "alpha_bpp": 2},
                ],
            }
            manifest_path = root / "assets.json"
            manifest_path.write_text(json.dumps(manifest), encoding="utf-8")
            result = AssetBuildService().build(manifest_path)
            self.assertTrue(result.succeeded, result.diagnostics)
            self.assertEqual(len(result.assets), 3)
            source = (root / "generated" / "assets.c").read_text(encoding="utf-8")
            self.assertIn("GFX_COLOR_ALIGNAS static const uint8_t Mirror_Color[]", source)
            self.assertIn("GFX_COLOR_TYPE_BITMAP_WITH_PALETTE", source)
            self.assertEqual(len(result.assets[2].palette_values), 4)

    def test_invalid_bitmap_alpha_is_reported_without_outputs(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            Image.new("RGBA", (1, 1), (0, 0, 0, 255)).save(root / "input.png")
            manifest = {"assets": [{"name": "Bad", "source": "input.png", "type": "BITMAP",
                                     "storage": "MCU", "bpp": 1, "alpha_bpp": 1}]}
            path = root / "assets.json"
            path.write_text(json.dumps(manifest), encoding="utf-8")
            result = AssetBuildService().build(path)
            self.assertFalse(result.succeeded)
            self.assertTrue(any(item.code == "alpha" for item in result.diagnostics))
            self.assertFalse((root / "generated").exists())

    def test_jpeg_rejects_alpha_channel(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / "photo.jpg"
            Image.new("RGB", (1, 1), (200, 100, 50)).save(path)
            config = xgfx_asset.AssetConfig(name="Photo", source=path, image_type="MIRROR",
                                            color_format="RGB565", alpha_bpp=2)
            result = AssetBuildService().convert(config)
            self.assertFalse(result.succeeded)
            self.assertTrue(any(item.code == "alpha" for item in result.diagnostics))

    def test_transparency_without_alpha_flattens_to_background_color(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / "transparent.png"
            image = Image.new("RGBA", (2, 1))
            image.putdata([(255, 0, 0, 0), (255, 0, 0, 128)])
            image.save(path)
            config = xgfx_asset.AssetConfig(name="Flat", source=path, image_type="MIRROR",
                                            color_format="RGB888", background_color="#0000FF")
            result = AssetBuildService().convert(config)
            self.assertTrue(result.succeeded, result.diagnostics)
            colors = xgfx_asset.deserialize_colors(result.assets[0].color_data, "RGB888", "little")
            self.assertEqual(colors[0], 0x0000FF)
            self.assertEqual(colors[1], 0x80007F)

    def test_transparency_without_selected_background_keeps_source_rgb(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / "transparent.png"
            image = Image.new("RGBA", (2, 1))
            image.putdata([(12, 34, 56, 0), (78, 90, 123, 128)])
            image.save(path)
            config = xgfx_asset.AssetConfig(name="RawRgb", source=path, image_type="MIRROR",
                                            color_format="RGB888", background_color=None)
            result = AssetBuildService().convert(config)
            self.assertTrue(result.succeeded, result.diagnostics)
            colors = xgfx_asset.deserialize_colors(result.assets[0].color_data, "RGB888", "little")
            self.assertEqual(colors, (0x0C2238, 0x4E5A7B))

    def test_flash_binary_uses_contiguous_alpha_address(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            Image.new("RGBA", (2, 1), (10, 20, 30, 128)).save(root / "input.png")
            manifest = {
                "schema_version": 1,
                "output": {"directory": "generated", "name": "assets"},
                "assets": [{"name": "FlashIcon", "source": "input.png", "type": "MIRROR",
                            "storage": "FLASH", "color_format": "RGB565", "alpha_bpp": 2,
                            "flash_address": "0x1000"}],
            }
            path = root / "assets.json"
            path.write_text(json.dumps(manifest), encoding="utf-8")
            result = AssetBuildService().build(path)
            self.assertTrue(result.succeeded, result.diagnostics)
            source = (root / "generated" / "assets.c").read_text(encoding="utf-8")
            self.assertIn(".Flash_Addr = 0x00001000u", source)
            self.assertIn(".Flash_Addr = 0x00001004u", source)
            self.assertEqual((root / "generated" / "FlashIcon.bin").stat().st_size, 5)

    def test_unknown_schema_version_is_rejected(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            path = Path(temporary) / "assets.json"
            path.write_text(json.dumps({"schema_version": 99, "assets": []}), encoding="utf-8")
            result = AssetBuildService().build(path)
            self.assertFalse(result.succeeded)
            self.assertTrue(any(item.code == "manifest" for item in result.diagnostics))

    def test_code_directory_can_overwrite_source_files(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            (root / "source").mkdir()
            Image.new("RGB", (1, 1), (255, 0, 0)).save(root / "input.png")
            (root / "source" / "ui_assets.c").write_text("old c", encoding="utf-8")
            (root / "source" / "ui_assets.h").write_text("old h", encoding="utf-8")
            manifest = {
                "schema_version": 1,
                "output": {"directory": "generated", "code_directory": "source", "name": "ui_assets"},
                "assets": [{"name": "Icon", "source": "input.png", "type": "MIRROR",
                            "storage": "MCU", "color_format": "RGB565"}],
            }
            path = root / "assets.json"
            path.write_text(json.dumps(manifest), encoding="utf-8")
            result = AssetBuildService().build(path)
            self.assertTrue(result.succeeded, result.diagnostics)
            self.assertIn("const GFX_Img_t Icon", (root / "source" / "ui_assets.c").read_text(encoding="utf-8"))
            self.assertIn("GFX_IMG_DECLEARE", (root / "source" / "ui_assets.h").read_text(encoding="utf-8"))
            self.assertFalse((root / "generated" / "ui_assets.c").exists())

    def test_absolute_source_and_output_paths(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            image_path = root / "input.png"
            output_path = root / "absolute-output"
            code_path = root / "absolute-source"
            Image.new("RGB", (1, 1), (0, 0, 255)).save(image_path)
            manifest = {
                "schema_version": 1,
                "output": {"directory": str(output_path), "code_directory": str(code_path), "name": "absolute_assets"},
                "assets": [{"name": "AbsoluteIcon", "source": str(image_path), "type": "MIRROR",
                            "storage": "MCU", "color_format": "RGB565"}],
            }
            path = root / "manifest.json"
            path.write_text(json.dumps(manifest), encoding="utf-8")
            result = AssetBuildService().build(path)
            self.assertTrue(result.succeeded, result.diagnostics)
            self.assertTrue((code_path / "absolute_assets.c").is_file())
            self.assertTrue((output_path / "absolute_assets_report.json").is_file())

    def test_auto_quantize_is_deterministic_and_leaves_no_temp_files(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            image = Image.new("RGB", (8, 1))
            image.putdata([(i * 31, 255 - i * 29, i * 17) for i in range(8)])
            image.save(root / "input.png")
            manifest = {
                "schema_version": 1,
                "output": {"directory": "generated", "name": "assets"},
                "assets": [{"id": "stable-palette", "name": "Quantized", "source": "input.png",
                            "type": "BITMAP_WITH_PALETTE", "storage": "MCU", "color_format": "RGB565",
                            "bpp": 2, "auto_quantize": True}],
            }
            path = root / "assets.json"
            path.write_text(json.dumps(manifest), encoding="utf-8")
            first = AssetBuildService().build(path)
            second = AssetBuildService().build(path)
            self.assertTrue(first.succeeded and second.succeeded)
            self.assertEqual(first.assets[0].color_data, second.assets[0].color_data)
            self.assertEqual(first.assets[0].config.asset_id, "stable-palette")
            self.assertFalse(list((root / "generated").glob("*.tmp*")))

    def test_init_scans_folders_and_builds_auto_layout_flash_bundle(self) -> None:
        with tempfile.TemporaryDirectory() as temporary:
            root = Path(temporary)
            (root / "menu" / "center").mkdir(parents=True)
            Image.new("RGB", (2, 1), (255, 0, 0)).save(root / "menu" / "center" / "area.png")
            Image.new("RGB", (1, 1), (0, 255, 0)).save(root / "0.png")
            manifest_path, count = initialize_project(root, byte_order="big")
            self.assertEqual(count, 2)
            manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
            self.assertEqual(manifest["defaults"]["byte_order"], "big")
            self.assertFalse(manifest["output"]["write_individual_bins"])
            self.assertFalse(manifest["output"]["write_previews"])
            self.assertEqual([item["name"] for item in manifest["assets"]],
                             ["Img_Image_0", "Img_menu_center_area"])
            result = AssetBuildService().build(manifest_path)
            self.assertTrue(result.succeeded, result.diagnostics)
            self.assertEqual([asset.config.flash_address for asset in result.assets], [0, 2])
            self.assertEqual((root / "generated" / "gfx_assets.bin").stat().st_size, 6)
            self.assertFalse(list((root / "generated").glob("*_preview.png")))


if __name__ == "__main__":
    unittest.main()

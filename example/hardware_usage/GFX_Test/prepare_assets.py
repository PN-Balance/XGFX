from pathlib import Path
from PIL import Image, ImageDraw
import json
import importlib.util
import sys

ROOT = Path(__file__).resolve().parent
SRC = ROOT / "source_images"
SELECTED = SRC / "selected"
OUT = ROOT / "generated"
OUT.mkdir(exist_ok=True)

# Remove stale files so the output always describes this manifest exactly.
for stale in OUT.iterdir():
    if stale.is_file():
        stale.unlink()


def fit(name: str, source: str, size: tuple[int, int], rgba: bool = False) -> None:
    with Image.open(SELECTED / source) as opened:
        image = opened.convert("RGBA" if rgba else "RGB")
        image.thumbnail(size, Image.Resampling.LANCZOS)
        mode = "RGBA" if rgba else "RGB"
        canvas = Image.new(mode, size, (0, 0, 0, 0) if rgba else (10, 16, 28))
        canvas.paste(image, ((size[0] - image.width) // 2, (size[1] - image.height) // 2), image if rgba else None)
        canvas.save(SRC / name)


# Human-friendly photographs and characters selected for the hardware gallery.
fit("mountain_160x90.png", "mountain_lake.jpg", (160, 90))
fit("great_wall_160x90.png", "great_wall.jpg", (160, 90))
fit("santorini_160x90.png", "santorini.jpg", (160, 90))
fit("kitten_96.png", "kitten.jpg", (96, 96))
fit("dog_96.png", "golden_retriever.jpg", (96, 96))
fit("cat_alpha_80x112.png", "transparent_cat.png", (80, 112), True)
fit("dog_alpha_80x112.png", "transparent_dog.png", (80, 112), True)
fit("anime_alpha_80x136.png", "wikipe_tan.png", (80, 136), True)
fit("ghibli_160x90.png", "spirited_away.jpg", (160, 90))

# Small MCU-resident variants; these prove every storage path without consuming the MCU Flash.
fit("mcu_mountain_48x27.png", "mountain_lake.jpg", (48, 27))
fit("mcu_kitten_40.png", "kitten.jpg", (40, 40))
fit("mcu_cat_alpha_32x40.png", "transparent_cat.png", (32, 40), True)

# Friendly, obvious bit-pattern resource. Odd width/height deliberately exercise row padding.
mask = Image.new("L", (33, 25), 0)
d = ImageDraw.Draw(mask)
d.rounded_rectangle((1, 1, 31, 23), radius=6, fill=255)
d.ellipse((7, 6, 11, 10), fill=0)
d.ellipse((21, 6, 25, 10), fill=0)
d.arc((8, 7, 25, 20), 15, 165, fill=96, width=3)
d.line((0, 12, 32, 12), fill=160, width=1)
mask.save(SRC / "friendly_mask_33x25.png")
mask.resize((31, 17), Image.Resampling.LANCZOS).save(SRC / "friendly_mask_31x17.png")

assets = [
    {"id":"mountain-mirror", "name":"GFX_Test_Mountain_Mirror", "source":"source_images/mountain_160x90.png"},
    {"id":"great-wall-p4", "name":"GFX_Test_GreatWall_P4", "source":"source_images/great_wall_160x90.png", "type":"BITMAP_WITH_PALETTE", "bpp":4, "quantization":"MEDIAN_CUT"},
    {"id":"santorini-p3", "name":"GFX_Test_Santorini_P3", "source":"source_images/santorini_160x90.png", "type":"BITMAP_WITH_PALETTE", "bpp":3, "quantization":"MAX_COVERAGE"},
    {"id":"kitten-p5", "name":"GFX_Test_Kitten_P5", "source":"source_images/kitten_96.png", "type":"BITMAP_WITH_PALETTE", "bpp":5, "quantization":"DOMINANT"},
    {"id":"dog-mirror", "name":"GFX_Test_Dog_Mirror", "source":"source_images/dog_96.png"},
    {"id":"cat-mirror-a4", "name":"GFX_Test_Cat_Mirror_A4", "source":"source_images/cat_alpha_80x112.png", "alpha_bpp":4},
    {"id":"dog-p2-a2", "name":"GFX_Test_Dog_P2_A2", "source":"source_images/dog_alpha_80x112.png", "type":"BITMAP_WITH_PALETTE", "bpp":2, "alpha_bpp":2, "quantization":"MAX_COVERAGE"},
    {"id":"anime-p4-a5", "name":"GFX_Test_Anime_P4_A5", "source":"source_images/anime_alpha_80x136.png", "type":"BITMAP_WITH_PALETTE", "bpp":4, "alpha_bpp":5, "quantization":"DOMINANT"},
    {"id":"ghibli-p4", "name":"GFX_Test_Ghibli_P4", "source":"source_images/ghibli_160x90.png", "type":"BITMAP_WITH_PALETTE", "bpp":4, "quantization":"MEDIAN_CUT"},
]

for bpp in range(1, 6):
    assets.append({"id":f"bitmap-b{bpp}", "name":f"GFX_Test_Bitmap_B{bpp}",
                   "source":"source_images/friendly_mask_33x25.png", "type":"BITMAP", "bpp":bpp})
for alpha_bpp in range(1, 6):
    assets.append({"id":f"cat-p4-a{alpha_bpp}", "name":f"GFX_Test_Cat_P4_A{alpha_bpp}",
                   "source":"source_images/cat_alpha_80x112.png", "type":"BITMAP_WITH_PALETTE",
                   "bpp":4, "alpha_bpp":alpha_bpp, "quantization":"DOMINANT"})

assets += [
    {"id":"mcu-mountain", "name":"GFX_Test_MCU_Mountain", "source":"source_images/mcu_mountain_48x27.png", "storage":"MCU"},
    {"id":"mcu-kitten-p3", "name":"GFX_Test_MCU_Kitten_P3", "source":"source_images/mcu_kitten_40.png", "storage":"MCU", "type":"BITMAP_WITH_PALETTE", "bpp":3, "quantization":"MEDIAN_CUT"},
    {"id":"mcu-cat-p2-a2", "name":"GFX_Test_MCU_Cat_P2_A2", "source":"source_images/mcu_cat_alpha_32x40.png", "storage":"MCU", "type":"BITMAP_WITH_PALETTE", "bpp":2, "alpha_bpp":2, "quantization":"DOMINANT"},
    {"id":"mcu-bitmap-b1", "name":"GFX_Test_MCU_Bitmap_B1", "source":"source_images/friendly_mask_31x17.png", "storage":"MCU", "type":"BITMAP", "bpp":1},
    {"id":"mcu-bitmap-b5", "name":"GFX_Test_MCU_Bitmap_B5", "source":"source_images/friendly_mask_31x17.png", "storage":"MCU", "type":"BITMAP", "bpp":5},
]

manifest = {
    "schema_version": 1,
    "color_mode": "fixed",
    "defaults": {"type":"MIRROR", "storage":"FLASH", "color_format":"RGB565",
                 "byte_order":"little", "bpp":0, "alpha_bpp":0,
                 "quantization":"DOMINANT", "background_color":"#10151f"},
    "flash": {"base_address":"0x0", "alignment":4, "fill_byte":255},
    "output": {"directory":"generated", "code_directory":"generated",
               "name":"gfx_test_assets", "flash_file":"gfx_test_assets.bin",
               "write_individual_bins":False, "write_previews":True},
    "assets": assets,
}
(ROOT / "xgfx_assets.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")

# Use the repository's single-file generator directly. No package installation is needed.
tool_path = ROOT.parents[2] / "tools" / "xgfx_script" / "xgfx_asset.py"
spec = importlib.util.spec_from_file_location("xgfx_asset", tool_path)
module = importlib.util.module_from_spec(spec)
sys.modules["xgfx_asset"] = module
spec.loader.exec_module(module)
result = module.AssetBuildService().build(ROOT / "xgfx_assets.json")
for diagnostic in result.diagnostics:
    print(f"{diagnostic.severity}: {diagnostic.asset or 'project'}: {diagnostic.message}")
if not result.succeeded:
    raise SystemExit(1)
print(f"Prepared and built {len(result.assets)} assets; wrote {len(result.written_files)} files")

from pathlib import Path
from PIL import Image, ImageDraw
import json

ROOT = Path(__file__).resolve().parent
SRC = ROOT / "source_images"
OUT = ROOT / "generated"
OUT.mkdir(exist_ok=True)

with Image.open(SRC / "landscape.jpg") as image:
    image = image.convert("RGB")
    image.thumbnail((172, 112), Image.Resampling.LANCZOS)
    canvas = Image.new("RGB", (172, 112), "black")
    canvas.paste(image, ((172 - image.width) // 2, (112 - image.height) // 2))
    canvas.save(SRC / "landscape_172x112.png")

with Image.open(SRC / "gear.png") as image:
    image = image.convert("RGBA").resize((96, 96), Image.Resampling.LANCZOS)
    image.save(SRC / "gear_96.png")
    image.resize((40, 40), Image.Resampling.LANCZOS).save(SRC / "gear_40.png")

with Image.open(SRC / "landscape.jpg") as image:
    image.convert("RGB").resize((64, 40), Image.Resampling.LANCZOS).save(SRC / "landscape_64x40.png")

mask = Image.new("L", (80, 80), 0)
d = ImageDraw.Draw(mask)
d.ellipse((3, 3, 76, 76), fill=255)
d.ellipse((21, 21, 58, 58), fill=0)
d.line((10, 68, 68, 10), fill=160, width=7)
mask.save(SRC / "mono_mask.png")
mask.resize((32, 32), Image.Resampling.LANCZOS).save(SRC / "mono_mask_32.png")

manifest = {
    "schema_version": 1,
    "color_mode": "fixed",
    "defaults": {
        "type": "MIRROR", "storage": "FLASH", "color_format": "RGB565",
        "byte_order": "little", "bpp": 0, "alpha_bpp": 0,
        "quantization": "DOMINANT", "background_color": "#10151f"
    },
    "flash": {"base_address": "0x0", "alignment": 4, "fill_byte": 255},
    "output": {
        "directory": "generated", "code_directory": "generated",
        "name": "gfx_test_assets", "flash_file": "gfx_test_assets.bin",
        "write_individual_bins": False, "write_previews": True
    },
    "assets": [
        {"id": "landscape-mirror", "name": "GFX_Test_Landscape_Mirror",
         "source": "source_images/landscape_172x112.png"},
        {"id": "landscape-palette", "name": "GFX_Test_Landscape_Palette4",
         "source": "source_images/landscape_172x112.png", "type": "BITMAP_WITH_PALETTE",
         "bpp": 4, "quantization": "MEDIAN_CUT"},
        {"id": "gear-alpha", "name": "GFX_Test_Gear_Alpha4",
         "source": "source_images/gear_96.png", "alpha_bpp": 4},
        {"id": "gear-palette", "name": "GFX_Test_Gear_Palette2_Alpha2",
         "source": "source_images/gear_96.png", "type": "BITMAP_WITH_PALETTE",
         "bpp": 2, "alpha_bpp": 2, "quantization": "MAX_COVERAGE"},
        {"id": "mono-bitmap", "name": "GFX_Test_Mono_Bitmap1",
         "source": "source_images/mono_mask.png", "type": "BITMAP", "bpp": 1}
        ,
        {"id": "mcu-landscape", "name": "GFX_Test_MCU_Landscape_Mirror",
         "source": "source_images/landscape_64x40.png", "storage": "MCU"},
        {"id": "mcu-gear-alpha", "name": "GFX_Test_MCU_Gear_Palette2_Alpha2",
         "source": "source_images/gear_40.png", "storage": "MCU",
         "type": "BITMAP_WITH_PALETTE", "bpp": 2, "alpha_bpp": 2,
         "quantization": "MAX_COVERAGE"},
        {"id": "mcu-mono-bitmap", "name": "GFX_Test_MCU_Mono_Bitmap1",
         "source": "source_images/mono_mask_32.png", "storage": "MCU",
         "type": "BITMAP", "bpp": 1}
    ]
}
(ROOT / "xgfx_assets.json").write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")

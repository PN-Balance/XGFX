"""生成 BITMAP_WITH_PALETTE 测试图的放大预览 PNG
12x12 四象限：左上红/右上绿/左下蓝/右下黑，放大20倍到240x240"""
import struct
from PIL import Image

W, H, BPP = 12, 12, 2
SCALE = 20

def rgb565_to_rgb888(c):
    r = (c >> 11) & 0x1F
    g = (c >> 5) & 0x3F
    b = c & 0x1F
    return (r << 3 | r >> 2, g << 2 | g >> 4, b << 3 | b >> 2)

palette_565 = [0x0000, 0xF800, 0x07E0, 0x001F]  # 黑红绿蓝
palette_rgb = [rgb565_to_rgb888(c) for c in palette_565]
print("调色板 RGB888:", palette_rgb)

# 像素索引图
pixels = []
for y in range(H):
    row = []
    for x in range(W):
        if y < 6:
            row.append(1 if x < 6 else 2)   # 左上红 / 右上绿
        else:
            row.append(3 if x < 6 else 0)   # 左下蓝 / 右下黑
    pixels.append(row)

# 生成放大 PNG
img = Image.new("RGB", (W * SCALE, H * SCALE), (128, 128, 128))
for y in range(H):
    for x in range(W):
        color = palette_rgb[pixels[y][x]]
        for dy in range(SCALE):
            for dx in range(SCALE):
                img.putpixel((x * SCALE + dx, y * SCALE + dy), color)

# 画象限分割线（白色细线，便于看清边界）
from PIL import ImageDraw
draw = ImageDraw.Draw(img)
# 垂直分割线（x=6）
for y in range(H * SCALE):
    img.putpixel((6 * SCALE - 1, y), (255, 255, 255))
    img.putpixel((6 * SCALE, y), (255, 255, 255))
# 水平分割线（y=6）
for x in range(W * SCALE):
    img.putpixel((x, 6 * SCALE - 1), (255, 255, 255))
    img.putpixel((x, 6 * SCALE), (255, 255, 255))

out = r"f:\DeskTop\WorkTemp\LCD\my GFX Lib\sim\_palette_preview.png"
img.save(out)
print(f"已保存: {out} ({W*SCALE}x{H*SCALE})")

"""
重新生成 BITMAP_WITH_PALETTE 测试图（12x12, BPP=2, 4色）
右下角改为紫色，布局：[像素bit流 36字节][调色板 8字节]
图案：左上红(1)/右上绿(2)/左下蓝(3)/右下紫(0)
"""
import struct, os

W, H, BPP = 12, 12, 2

def rgb565(r, g, b):
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

# 调色板：索引0=紫, 1=红, 2=绿, 3=蓝 (RGB565)
palette = [
    rgb565(128, 0, 128),    # 0 紫
    rgb565(255, 0, 0),      # 1 红
    rgb565(0, 255, 0),      # 2 绿
    rgb565(0, 0, 255),      # 3 蓝
]

# 像素索引图：左上红(1)/右上绿(2)/左下蓝(3)/右下紫(0)
pixels = []
for y in range(H):
    row = []
    for x in range(W):
        if y < 6:
            row.append(1 if x < 6 else 2)
        else:
            row.append(3 if x < 6 else 0)
    pixels.append(row)

# 打包 bit 流
bit_stream = bytearray()
for y in range(H):
    bit_pos = 0
    row_bytes = bytearray((W * BPP + 7) // 8)
    for x in range(W):
        val = pixels[y][x]
        for b in range(BPP):
            bit = (val >> b) & 1
            byte_idx = bit_pos // 8
            bit_in_byte = bit_pos % 8
            if bit:
                row_bytes[byte_idx] |= (1 << bit_in_byte)
            bit_pos += 1
    bit_stream += row_bytes

print(f"像素bit流: {len(bit_stream)} 字节")
print("字节:", " ".join(f"{b:02X}" for b in bit_stream))

# 调色板
palette_bytes = bytearray()
for c in palette:
    palette_bytes += struct.pack("<H", c)
print(f"调色板: {len(palette_bytes)} 字节")
print("调色板字节:", " ".join(f"{b:02X}" for b in palette_bytes))

# 写 bin
sim = r"f:\DeskTop\WorkTemp\LCD\my GFX Lib\sim"
with open(os.path.join(sim, "_palette.bin"), "wb") as f:
    f.write(bit_stream + palette_bytes)
print(f"_palette.bin 已更新 ({len(bit_stream)+len(palette_bytes)} 字节)")

# 合并 _flash.bin
with open(os.path.join(sim, "_mirror.bin"), "rb") as f:
    mirror = f.read()
flash = mirror + bit_stream + palette_bytes
with open(os.path.join(sim, "_flash.bin"), "wb") as f:
    f.write(flash)
print(f"_flash.bin 已更新 ({len(flash)} 字节)")

# 输出 C 数组
print("\n=== C 数组 ===")
print("GFX_COLOR_ALIGNAS static const uint8_t s_Palette_Img_Data[] = {")
line = "    "
for i, b in enumerate(bit_stream + palette_bytes):
    line += f"0x{b:02X}, "
    if (i + 1) % 12 == 0:
        print(line)
        line = "    "
if line.strip():
    print(line)
print("} ;")
print("OK")

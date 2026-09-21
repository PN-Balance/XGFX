"""
生成 BITMAP 测试图（12x12, BPP=1, 灰度调色板由库自动生成）
图案：黑底白色 X（对角线）
bit流：逐行扫描，低位在前，阳码(1=前景=白)，每行补齐整字节
无调色板数据（BITMAP 类型由 GFX_Create_Gray_Palette 自动生成 0=黑/1=白）
"""
import os

W, H, BPP = 12, 12, 1

# 像素图：黑底白色 X（两条对角线）
pixels = []
for y in range(H):
    row = []
    for x in range(W):
        # 主对角线 + 副对角线 = X
        if x == y or x == (W - 1 - y):
            row.append(1)  # 白
        else:
            row.append(0)  # 黑
    pixels.append(row)

# 打包 bit 流
bit_stream = bytearray()
for y in range(H):
    bit_pos = 0
    row_bytes = bytearray((W * BPP + 7) // 8)  # 2 字节/行
    for x in range(W):
        val = pixels[y][x]
        byte_idx = bit_pos // 8
        bit_in_byte = bit_pos % 8
        if val:
            row_bytes[byte_idx] |= (1 << bit_in_byte)
        bit_pos += BPP
    bit_stream += row_bytes

print(f"像素bit流: {len(bit_stream)} 字节 (每行 {(W*BPP+7)//8} 字节)")
print("字节:", " ".join(f"{b:02X}" for b in bit_stream))

# BITMAP 无调色板，bin 文件只有 bit 流
sim = r"f:\DeskTop\WorkTemp\LCD\my GFX Lib\sim"
with open(os.path.join(sim, "_bitmap.bin"), "wb") as f:
    f.write(bit_stream)
print(f"_bitmap.bin 已生成 ({len(bit_stream)} 字节)")

# 输出 C 数组
print("\n=== C 数组 ===")
print("GFX_COLOR_ALIGNAS static const uint8_t s_Bitmap_Img_Data[] = {")
line = "    "
for i, b in enumerate(bit_stream):
    line += f"0x{b:02X}, "
    if (i + 1) % 12 == 0:
        print(line)
        line = "    "
if line.strip():
    print(line)
print("} ;")
print("OK")

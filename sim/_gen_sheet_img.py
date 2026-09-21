"""
生成 Cut_Self 图集测试图：12x36 BPP=1 BITMAP，纵向排列 3 帧，每帧 12x12
第 0 份: 字母 H
第 1 份: 字母 E
第 2 份: 字母 L
bit流：逐行扫描，低位在前，阳码(1=前景=白)，每行补齐整字节
无调色板（BITMAP 类型）
"""
import os

W, H, BPP = 12, 36, 1

# 12x12 字母点阵（1=白，0=灰背景）
# H
H_pat = [
    "110000001100",
    "110000001100",
    "110000001100",
    "110000001100",
    "110000001100",
    "111111111100",
    "111111111100",
    "110000001100",
    "110000001100",
    "110000001100",
    "110000001100",
    "110000001100",
]
# E
E_pat = [
    "111111111100",
    "111111111100",
    "110000000000",
    "110000000000",
    "110000000000",
    "111111110000",
    "111111110000",
    "110000000000",
    "110000000000",
    "110000000000",
    "111111111100",
    "111111111100",
]
# L
L_pat = [
    "110000000000",
    "110000000000",
    "110000000000",
    "110000000000",
    "110000000000",
    "110000000000",
    "110000000000",
    "110000000000",
    "110000000000",
    "110000001100",
    "111111111100",
    "111111111100",
]

patterns = [H_pat, E_pat, L_pat]

# 打包 bit 流
bit_stream = bytearray()
bytes_per_row = (W * BPP + 7) // 8  # 2 字节/行
for pat in patterns:
    for row_str in pat:
        row_bytes = bytearray(bytes_per_row)
        for x in range(W):
            val = 1 if row_str[x] == '1' else 0
            bit_pos = x * BPP
            byte_idx = bit_pos // 8
            bit_in_byte = bit_pos % 8
            if val:
                row_bytes[byte_idx] |= (1 << bit_in_byte)
        bit_stream += row_bytes

print(f"像素bit流: {len(bit_stream)} 字节 (每行 {bytes_per_row} 字节, 共 {H} 行)")
print("字节:", " ".join(f"{b:02X}" for b in bit_stream))

sim = os.path.dirname(os.path.abspath(__file__))
with open(os.path.join(sim, "_sheet.bin"), "wb") as f:
    f.write(bit_stream)
print(f"_sheet.bin 已生成 ({len(bit_stream)} 字节)")

# 输出 C 数组
print("\n=== C 数组 ===")
print("GFX_COLOR_ALIGNAS static const uint8_t s_Sheet_Img_Data[] = {")
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

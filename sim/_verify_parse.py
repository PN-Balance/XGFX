"""
验证 PALETTE 类型的 bit 解析逻辑
复现 C 代码 prv_Fill_Img_Bitmap_With_Palette 的核心算法
"""
W, H, BPP = 12, 12, 2

# 像素 bit 流（与 _palette.bin 一致）
bit_stream = bytes([
    0x55, 0xA5, 0xAA, 0x55, 0xA5, 0xAA, 0x55, 0xA5, 0xAA, 0x55, 0xA5, 0xAA,
    0x55, 0xA5, 0xAA, 0x55, 0xA5, 0xAA, 0xFF, 0x0F, 0x00, 0xFF, 0x0F, 0x00,
    0xFF, 0x0F, 0x00, 0xFF, 0x0F, 0x00, 0xFF, 0x0F, 0x00, 0xFF, 0x0F, 0x00,
])
palette = [0x8010, 0xF800, 0x07E0, 0x001F]  # 紫 红 绿 蓝

def parse_row_c_logic(row, off_x, com_w, bpp, data):
    """复现 C 代码的解析逻辑"""
    raw_bytes_per_row = (W * bpp + 7) // 8  # 3
    # C: start_bit = (off_y * W + off_x) * bpp
    # 这里 off_y=0, 只看行内
    start_bit = (0 * W + off_x) * bpp
    bit_offset = start_bit % 8
    read_bytes = (bit_offset + com_w * bpp + 7) // 8
    start_byte = start_bit // 8

    row_start_byte = row * raw_bytes_per_row + start_byte
    bmp_buf = data[row_start_byte : row_start_byte + read_bytes]

    result = []
    bit_pos = bit_offset
    for p in range(com_w):
        val = 0
        for b in range(bpp):
            cur = bit_pos + b
            bit = (bmp_buf[cur // 8] >> (cur % 8)) & 1
            val |= (bit << b)
        result.append(val)
        bit_pos += bpp
    return result

print("=== 正常绘制 off_x=0, Com.W=12 ===")
for row in range(H):
    idx = parse_row_c_logic(row, 0, 12, BPP, bit_stream)
    colors = [palette[i] for i in idx]
    print(f"行{row:2d}: idx={idx}  colors={['%04X'%c for c in colors]}")

print("\n=== 裁剪 off_x=5, Com.W=7 (对应 -5 位置) ===")
for row in range(H):
    idx = parse_row_c_logic(row, 5, 7, BPP, bit_stream)
    colors = [palette[i] for i in idx]
    print(f"行{row:2d}: idx={idx}  colors={['%04X'%c for c in colors]}")

# 验证：正常绘制时，行0-5前6列应为红(1), 后6列绿(2)
# 行6-11前6列应为蓝(3), 后6列紫(0)
print("\n=== 验证 ===")
ok = True
for row in range(H):
    idx = parse_row_c_logic(row, 0, 12, BPP, bit_stream)
    for col in range(12):
        expected = None
        if row < 6:
            expected = 1 if col < 6 else 2  # 红/绿
        else:
            expected = 3 if col < 6 else 0  # 蓝/紫
        if idx[col] != expected:
            print(f"  错误: 行{row} 列{col} 期望{expected} 实际{idx[col]}")
            ok = False
if ok:
    print("  正常绘制: 所有像素索引正确 ✓")
else:
    print("  正常绘制: 有错误 ✗")

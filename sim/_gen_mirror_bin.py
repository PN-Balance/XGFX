"""生成 32x24 渐变 MIRROR 图的 RGB565 bin 文件（模拟外部 Flash 内容）"""
W, H = 32, 24

def rgb565(r, g, b):
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)

data = bytearray()
for y in range(H):
    for x in range(W):
        r = x * 255 // (W - 1)
        g = y * 255 // (H - 1)
        b = 128
        c = rgb565(r, g, b)
        data += bytes([c & 0xFF, (c >> 8) & 0xFF])  # 小端序，与 MCU 内存一致

with open(r"f:\DeskTop\WorkTemp\LCD\my GFX Lib\sim\_mirror.bin", "wb") as f:
    f.write(data)
print("OK", len(data), "bytes")

"""生成 32x24 渐变 MIRROR 原图（纯标准库，输出 PNG）"""
import struct, zlib

W, H = 32, 24

# 生成 RGB 像素
raw = bytearray()
for y in range(H):
    raw.append(0)  # PNG 每行开头需 0 过滤字节
    for x in range(W):
        r = x * 255 // (W - 1)
        g = y * 255 // (H - 1)
        b = 128
        raw += bytes([r, g, b])

def chunk(tag, data):
    c = tag + data
    return struct.pack(">I", len(data)) + c + struct.pack(">I", zlib.crc32(c) & 0xffffffff)

sig = b'\x89PNG\r\n\x1a\n'
ihdr = struct.pack(">IIBBBBB", W, H, 8, 2, 0, 0, 0)  # 8bit, truecolor
idat = zlib.compress(bytes(raw))

png = sig + chunk(b'IHDR', ihdr) + chunk(b'IDAT', idat) + chunk(b'IEND', b'')

with open(r"f:\DeskTop\WorkTemp\LCD\my GFX Lib\sim\_mirror_raw.png", "wb") as f:
    f.write(png)
print("OK")

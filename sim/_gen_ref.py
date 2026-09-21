"""生成 MIRROR 测试参考图，与 sim/main.c 中 Test_Mirror_Img 逻辑一致"""
from PIL import Image

W, H = 240, 320
img = Image.new("RGB", (W, H), (0, 0, 0))

MIRROR_W, MIRROR_H = 32, 24

def make_mirror():
    """生成 32x24 渐变 MIRROR 图（RGB888）"""
    m = Image.new("RGB", (MIRROR_W, MIRROR_H))
    for y in range(MIRROR_H):
        for x in range(MIRROR_W):
            r = x * 255 // (MIRROR_W - 1)
            g = y * 255 // (MIRROR_H - 1)
            b = 128
            m.putpixel((x, y), (r, g, b))
    return m

mirror = make_mirror()

def paste_clip(mirror_img, sx, sy):
    """模拟 GFX_Fill_Img 的裁剪逻辑：将 mirror_img 贴到 (sx,sy)，超出屏幕部分裁掉"""
    for y in range(MIRROR_H):
        for x in range(MIRROR_W):
            px = sx + x
            py = sy + y
            if 0 <= px < W and 0 <= py < H:
                img.putpixel((px, py), mirror_img.getpixel((x, y)))

# 1. 左上角正常
paste_clip(mirror, 5, 5)
# 2. 超出右下角
paste_clip(mirror, 220, 300)
# 3. 超出左上角（负坐标）
paste_clip(mirror, -10, -10)
# 4. 屏幕中间
paste_clip(mirror, 100, 140)

img.save(r"f:\DeskTop\WorkTemp\LCD\my GFX Lib\sim\_mirror_ref.png")
print("OK")

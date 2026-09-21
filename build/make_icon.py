"""把 xgfx_icon.png 转成带圆角的多尺寸 xgfx.ico。

圆角半径按尺寸比例给定：小图标比例略小、大图标略大，视觉上更接近系统应用图标。
遮罩用 4 倍超采样绘制，保证边缘平滑。运行：

    python build/make_icon.py
"""

from __future__ import annotations

from pathlib import Path
from PIL import Image, ImageDraw

ROOT = Path(__file__).resolve().parent
SRC = ROOT / 'gui_src' / 'xgfx_icon.png'     # 512x512 方形母版
ICO = ROOT / 'gui_src' / 'xgfx.ico'          # PyInstaller / Inno Setup 使用的成品

SIZES = [16, 24, 32, 48, 64, 128, 256]
SUPERSAMPLE = 4

# 圆角半径占边长的比例：16px 用 20%，256px 用 23%，中间按尺寸线性插值。
RADIUS_MIN = 0.20
RADIUS_MAX = 0.23


def radius_ratio(size: int) -> float:
    if size <= SIZES[0]:
        return RADIUS_MIN
    if size >= SIZES[-1]:
        return RADIUS_MAX
    span = SIZES[-1] - SIZES[0]
    return RADIUS_MIN + (RADIUS_MAX - RADIUS_MIN) * (size - SIZES[0]) / span


def rounded_mask(size: int) -> Image.Image:
    """生成 size×size 的圆角遮罩：圆角内为 255，四角外侧为 0。"""
    scale = size * SUPERSAMPLE
    mask = Image.new('L', (scale, scale), 0)
    draw = ImageDraw.Draw(mask)
    radius = max(1, round(radius_ratio(size) * scale))
    draw.rounded_rectangle((0, 0, scale - 1, scale - 1), radius=radius, fill=255)
    return mask.resize((size, size), Image.LANCZOS)


def rounded_icon(master: Image.Image, size: int) -> Image.Image:
    """把母版缩放到目标尺寸并套上圆角遮罩。"""
    tile = master.resize((size, size), Image.LANCZOS).convert('RGBA')
    tile.putalpha(rounded_mask(size))
    return tile


def main() -> None:
    master = Image.open(SRC).convert('RGBA')
    frames = [rounded_icon(master, s) for s in SIZES]
    # PIL 用 sizes 参数写入多尺寸 ICO；256 及以上自动使用 PNG 压缩存储。
    frames[-1].save(ICO, format='ICO', sizes=[(s, s) for s in SIZES], append_images=frames[:-1])
    # 同步更新母版，后续任何用法都拿到圆角版本。
    rounded_icon(master, master.width).save(SRC, format='PNG')
    print(f'wrote {ICO} ({", ".join(f"{s}x{s}" for s in SIZES)})')
    print(f'updated {SRC} ({master.width}x{master.height})')


if __name__ == '__main__':
    main()

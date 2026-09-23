from pathlib import Path
from PIL import Image, ImageDraw, ImageFont

ROOT = Path(__file__).resolve().parent
SRC = ROOT / "source_images"
OUT = ROOT / "expected_views"
OUT.mkdir(exist_ok=True)

CASES = [
    ("01", "边界图元", None, "四边越界图形被屏幕边缘正确裁掉"),
    ("02", "FLASH MIRROR", "mountain_160x90.png", "山景与金毛色彩自然、位置完整"),
    ("03", "FLASH PALETTE 4/3 BPP", "great_wall_160x90.png", "长城与圣托里尼以低色深显示"),
    ("04", "FLASH PALETTE 5/4 BPP", "kitten_96.png", "小猫与动画场景正常显示"),
    ("05", "BITMAP 1-5 BPP", "friendly_mask_33x25.png", "五种位深的笑脸逐渐平滑"),
    ("06", "双重裁剪", "mountain_160x90.png", "仅自身裁剪与屏幕裁剪交集可见"),
    ("07", "九点锚点", "friendly_mask_31x17.png", "九个图案分别围绕红色定位点"),
    ("08", "Alpha 与缓冲背景", "cat_alpha_80x112.png", "透明猫叠在蓝红条纹上"),
    ("09", "Alpha 与固定背景", "cat_alpha_80x112.png", "透明猫只与固定深灰色混合"),
    ("10", "2 BPP 色板 + 2 BPP Alpha", "dog_alpha_80x112.png", "透明狗轮廓完整且分级明显"),
    ("11", "5 BPP Alpha + 裁剪", "anime_alpha_80x136.png", "角色被两层裁剪且边缘平滑"),
    ("12", "Alpha 1-5 与缓冲背景", "cat_alpha_80x112.png", "五只猫的透明阶梯逐级细腻"),
    ("13", "Alpha 1-5 与固定背景", "cat_alpha_80x112.png", "五只猫均与各自固定色混合"),
    ("14", "Buffer 坐标与边界", None, "矩形和圆只出现在缓冲区域内"),
    ("15", "Buffer 双重裁剪", "great_wall_160x90.png", "长城只出现在两个裁剪区交集"),
    ("16", "MCU MIRROR / PALETTE", "mcu_mountain_48x27.png", "两张小山景和一张小猫来自 MCU"),
    ("17", "MCU Alpha / BITMAP", "mcu_cat_alpha_32x40.png", "透明猫和 1/5 BPP 图案来自 MCU"),
    ("18", "非法输入安全返回", None, "绿条与黄条出现，系统继续响应按键"),
]

font = ImageFont.load_default()
cards = []
for no, title, source, expected in CASES:
    card = Image.new("RGB", (440, 250), "#10151f")
    d = ImageDraw.Draw(card)
    d.rounded_rectangle((4, 4, 435, 245), 16, fill="#172130", outline="#4aa8ff", width=2)
    d.text((20, 18), f"CASE {no}  {title}", font=font, fill="#ffffff")
    d.text((20, 218), expected, font=font, fill="#b8c7dc")
    if source:
        im = Image.open(SRC / source).convert("RGBA")
        im.thumbnail((390, 165), Image.Resampling.LANCZOS)
        card.paste(im, ((440-im.width)//2, 44+(165-im.height)//2), im)
    else:
        for x, color in ((40,"#246bfe"),(145,"#35d07f"),(250,"#ffb84a")):
            d.rounded_rectangle((x,75,x+105,170),18,fill=color)
    card.save(OUT / f"case_{no}.png")
    cards.append(card)

sheet = Image.new("RGB", (900, 5*270), "#0b1018")
for i, card in enumerate(cards):
    sheet.paste(card, (10+(i%2)*450, 10+(i//2)*270))
sheet.save(OUT / "all_cases.png")
print(f"Wrote {len(cards)} expected views and contact sheet")

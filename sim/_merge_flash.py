"""合并 _mirror.bin + _palette.bin + _bitmap.bin + _sheet.bin = _flash.bin
mirror   @ 0     (1536B)
palette  @ 1536  (44B)
bitmap   @ 1580  (24B)
sheet    @ 1604  (72B)
total    = 1676B"""
import os
sim = r"f:\DeskTop\WorkTemp\LCD\my GFX Lib\sim"

parts = []
for name, off in [("_mirror.bin",0), ("_palette.bin",1536), ("_bitmap.bin",1580), ("_sheet.bin",1604)]:
    with open(os.path.join(sim, name), "rb") as f:
        d = f.read()
    parts.append((name, off, d))
    print(f"{name}: {len(d)} bytes @ {off}")

flash = b""
for name, off, d in parts:
    if len(flash) < off:
        flash += b"\x00" * (off - len(flash))
    flash += d

with open(os.path.join(sim, "_flash.bin"), "wb") as f:
    f.write(flash)
print(f"\n_flash.bin 已生成: {len(flash)} 字节")
print("OK")

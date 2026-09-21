# -*- mode: python ; coding: utf-8 -*-
# PyInstaller spec for the xgfx console command (like `git init`).
# Build with:
#   pyinstaller --noconfirm --distpath build\dist\xgfx_cli_dist build\xgfx_cli.spec
# Output: build\dist\xgfx_cli_dist\xgfx\xgfx.exe
# Install: copy xgfx.exe (or whole folder) somewhere on PATH,
#           or use the Inno Setup installer (build/xgfx.iss) which adds PATH.

from pathlib import Path

ROOT = Path(SPECPATH).resolve().parent  # project root (spec lives in build/)
SCRIPT_DIR = ROOT / 'tools' / 'xgfx_script'  # xgfx_asset.py 单一真源
APP_DIR = ROOT / 'build' / 'gui_src'          # desktop.py + index.html + logo.svg

# xgfx_asset.py 的 cli() 支持: init / scan / build / reset / version；无参数启动 GUI。
# 无参数模式会 `from desktop import launch`，所以 desktop.py 及其资源（index.html、logo.svg）
# 必须一起打进包里，且 pathex 要包含 APP_DIR 让 PyInstaller 能找到 desktop 模块。
datas = [
    (str(APP_DIR / 'index.html'), '.'),
    (str(APP_DIR / 'logo.svg'), '.'),
]

hiddenimports = [
    'webview',
    'webview.platforms.edgechromium',
    'PIL',
]

a = Analysis(
    [str(SCRIPT_DIR / 'xgfx_asset.py')],
    pathex=[str(SCRIPT_DIR), str(APP_DIR), str(ROOT / 'tools')],
    binaries=[],
    datas=datas,
    hiddenimports=hiddenimports,
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=['PyQt5', 'PyQt6', 'PySide2', 'PySide6', 'tkinter'],
    noarchive=False,
    optimize=0,
)

pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    [],
    exclude_binaries=True,
    name='xgfx',
    icon=str(APP_DIR / 'xgfx.ico'),
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=False,
    console=True,  # console app: xgfx init / scan / build / reset / version
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)

coll = COLLECT(
    exe,
    a.binaries,
    a.datas,
    strip=False,
    upx=False,
    upx_exclude=[],
    name='xgfx',
)

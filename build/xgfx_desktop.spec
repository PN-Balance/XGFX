# -*- mode: python ; coding: utf-8 -*-
# PyInstaller spec for the web-based XGFX desktop GUI.
# Build with:
#   pyinstaller --noconfirm --distpath build\dist\xgfx_desktop_dist build\xgfx_desktop.spec
# Output lands in build\dist\xgfx_desktop_dist\XGFXAssets\XGFXAssets.exe

from pathlib import Path

ROOT = Path(SPECPATH).resolve().parent  # project root (spec lives in build/)
APP_DIR = ROOT / 'build' / 'gui_src'          # desktop.py + index.html + logo.svg
SCRIPT_DIR = ROOT / 'tools' / 'xgfx_script'  # xgfx_asset.py 单一真源

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
    [str(APP_DIR / 'desktop.py')],
    pathex=[str(APP_DIR), str(SCRIPT_DIR)],
    binaries=[],
    datas=datas,
    hiddenimports=hiddenimports,
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=['PyQt5', 'PyQt6', 'PySide2', 'PySide6'],
    noarchive=False,
    optimize=0,
)

pyz = PYZ(a.pure)

exe = EXE(
    pyz,
    a.scripts,
    [],
    exclude_binaries=True,
    name='XGFXAssets',
    icon=str(APP_DIR / 'xgfx.ico'),
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=False,
    console=False,
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
    name='XGFXAssets',
)

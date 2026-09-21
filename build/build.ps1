# XGFX 一键打包脚本
# 用法: 在项目根目录运行  .\build\build.ps1
# 产物:
#   build\dist\xgfx_cli_dist\xgfx\xgfx.exe          命令行
#   build\dist\xgfx_desktop_dist\XGFXAssets\...     网页 GUI
#   build\dist\installer\XGFX-Setup.exe              Inno Setup 安装包

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root

# 版本号单一真源：tools\xgfx_script\xgfx_asset.py 里的 __version__
# 自动同步到 xgfx.iss 的 XGFXVersion
$versionLine = Select-String -Path "tools\xgfx_script\xgfx_asset.py" -Pattern '^__version__\s*=\s*"([^"]+)"' | Select-Object -First 1
if ($versionLine) {
    $version = $versionLine.Matches[0].Groups[1].Value
    $issPath = "build\xgfx.iss"
    $issContent = Get-Content $issPath -Raw -Encoding UTF8
    $issContent = $issContent -replace '#define XGFXVersion "[^"]+"', "#define XGFXVersion `"$version`""
    Set-Content $issPath -Value $issContent -Encoding UTF8 -NoNewline
    Write-Host "版本号已同步: XGFXVersion = $version" -ForegroundColor Cyan
}

Write-Host "=== 打包 xgfx.exe (console) ===" -ForegroundColor Cyan
pyinstaller --noconfirm --distpath build\dist\xgfx_cli_dist --workpath build\work build\xgfx_cli.spec

Write-Host "`n=== 打包 XGFXAssets.exe (GUI) ===" -ForegroundColor Cyan
pyinstaller --noconfirm --distpath build\dist\xgfx_desktop_dist --workpath build\work build\xgfx_desktop.spec

Write-Host "`n=== 编译安装包 XGFX-Setup.exe ===" -ForegroundColor Cyan
$iscc = "E:\Inno Setup 6\ISCC.exe"
if (Test-Path $iscc) {
    & $iscc build\xgfx.iss
} else {
    Write-Host "未找到 ISCC.exe ($iscc)，跳过安装包编译" -ForegroundColor Yellow
}

# 清理 PyInstaller 中间工作目录
Remove-Item -Recurse -Force build\work -ErrorAction SilentlyContinue

# 把成品安装包复制到 tools/xgfx_app/（tools 放成品工具）
$setupSrc = "build\dist\installer\XGFX-Setup.exe"
$setupDst = "tools\xgfx_app\XGFX-Setup.exe"
if (Test-Path $setupSrc) {
    Copy-Item $setupSrc $setupDst -Force
    Write-Host "`n已复制安装包到 tools\xgfx_app\XGFX-Setup.exe"
}

Write-Host "`n=== 打包完成 ===" -ForegroundColor Green
Write-Host "命令行:  build\dist\xgfx_cli_dist\xgfx\xgfx.exe"
Write-Host "GUI:     build\dist\xgfx_desktop_dist\XGFXAssets\XGFXAssets.exe"
Write-Host "安装包:  build\dist\installer\XGFX-Setup.exe"
Write-Host "成品:    tools\xgfx_app\XGFX-Setup.exe"

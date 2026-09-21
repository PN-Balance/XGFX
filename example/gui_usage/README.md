# GUI 与命令行用法示例

安装 `XGFX-Setup.exe` 后，像 git 一样用 `xgfx` 命令开发。

## 安装

运行 `tools/xgfx_app/XGFX-Setup.exe`，安装时勾选"把 xgfx 加入系统 PATH"。

## 命令行用法

进入图片目录，初始化配置：

```powershell
cd images
xgfx init
```

编辑 `.xgfx/xgfx_assets.json` 后生成资源：

```powershell
xgfx build
```

## 图形界面

```powershell
xgfx gui
```

启动桌面工作台，可在界面中浏览图片、调整参数、预览编码效果并生成资源。

## 快速重建

`xgfx init` 会把 `xgfx_asset.py` 复制到 `.xgfx/` 目录。开发过程中双击该文件即可在当前图片目录直接重新构建，无需打开终端。

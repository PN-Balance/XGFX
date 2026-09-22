# XGFX

面向 MCU 的轻量级 C 图形库，内置图片资源生成器。当前版本：**v1.1.0**。

## 功能特性

**图形库（`src/`）**
- 三种图片格式：原生彩色（MIRROR）、位图（BITMAP）、索引色板（BITMAP_WITH_PALETTE）
- 三种设备色：RGB332 / RGB565 / RGB888
- 1～5 BPP 色深与 Alpha 通道
- 直接写屏（Fill）与 Buffer 双绘制路径
- 区域裁剪、Alpha 混合、色板量化
- MCU 内置 Flash 与外部 Flash 存储

**资源生成器（`tools/`）**
- 扫描图片目录生成配置清单
- 输出 C 数组 / H 头文件 / BIN 二进制
- 两种使用方式：纯 Python 脚本 / 安装版 GUI + 命令行
- 支持 Flash 自动排址、实时预览、资源体积比较与报告
- 支持透明图片背景合成和多种调色板量化算法

## 目录结构

```
GFX Lib/
├── src/              # 图形库核心源码（C）
├── sim/              # Windows 模拟器 + 测试资源生成脚本
├── tests/            # 单元测试（Python + C）
├── tools/
│   ├── xgfx_script/  # 纯 Python 单文件脚本
│   └── xgfx_app/     # 安装包（XGFX-Setup.exe）
├── doc/              # 使用说明
├── example/
│   ├── python_usage/ # Python 脚本用法示例
│   ├── gui_usage/    # GUI 与命令行用法示例
│   └── hardware_usage/ # PY32F031 + ST7789W3 + BY25Q80 真机测试
└── build/            # 打包配置与产物
```

## 快速开始

### 方式一：纯 Python 脚本

```powershell
pip install pillow
cd example/python_usage/images
python ../../../tools/xgfx_script/xgfx_asset.py
```

首次运行创建 `.xgfx/xgfx_assets.json`，编辑配置后再次运行同一命令生成资源。

### 方式二：安装版（GUI + 命令行）

运行 `tools/xgfx_app/XGFX-Setup.exe` 安装，然后：

```powershell
cd example/gui_usage/images
xgfx init      # 初始化配置
xgfx build     # 生成资源
xgfx gui       # 打开图形界面
```

## 构建与测试

```powershell
.\sim\build.ps1 -Configuration Debug -Test
```

GCC 可用路径：`E:/mingw64/bin/gcc.exe`

## 文档

完整的环境搭建、图片存储格式、接口说明、裁剪与 Alpha、移植及限制见 [使用说明](doc/GFX_Usage.md)。版本变化见 [CHANGELOG](CHANGELOG.md)，可直接烧录的真机测试见 [hardware_usage](example/hardware_usage/DEPLOY/README.md)。

## License

MIT © 2026 PNBalance

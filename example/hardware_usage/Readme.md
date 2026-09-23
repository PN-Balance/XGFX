# XGFX 真机测试工程

本目录是面向现有硬件的上电即用测试工程：

- MCU：PY32F031x8
- 屏幕：172×320 ST7789W3
- 外部 Flash：BY25Q80ES，容量 1 MiB
- 开发环境：Keil MDK / ARMCC 5.06 update 5

打开 `Project.uvprojx` 即可编译。原产品 UI 已从 Keil 目标中移除，程序启动后直接显示 XGFX 测试界面。测试代码位于 `GFX_Test`，屏幕和外部 Flash 适配位于 `GFX/GFX_Port.c` 与 `4 - Hardware`。

测试同时使用 MCU 内部 Flash 和 BY25Q80ES 中的图片，覆盖三种图片格式、1–5 BPP、1–5 Alpha BPP、直接绘图、缓冲绘图、裁剪、九点锚点及两种 Alpha 混合背景。完整的 18 个案例、按键操作和理想画面见 [TEST_GUIDE.md](TEST_GUIDE.md)，烧录文件见 [DEPLOY/README.md](DEPLOY/README.md)，图片来源见 [GFX_Test/SOURCES.md](GFX_Test/SOURCES.md)。

`DEPLOY` 已包含可直接烧录的 MCU 固件和外部 Flash 镜像；无需安装 Keil 也可以完成硬件验证。

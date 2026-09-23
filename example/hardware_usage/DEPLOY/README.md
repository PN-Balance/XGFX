# XGFX 硬件测试烧录包

目标硬件：PY32F031x8（72 MHz）、172×320 ST7789W3、BY25Q80ES 1 MiB SPI NOR Flash。

1. 将 `XGFX_Hardware_Test.hex` 烧入 MCU。
2. 用 G120F 编程器选择 `25 FLASH / BYT / BY25Q80ES`，把 `XGFX_External_Flash_1MiB.bin` 从地址 `0x000000` 写入外部 Flash，并执行校验。
3. 上电后按 [硬件回归测试说明](../TEST_GUIDE.md) 操作。

`XGFX_External_Flash.bin` 是有效资源区；1 MiB 文件是在尾部填充 `0xFF` 的整片烧录版本。资源总有效大小为 153,096 字节。屏幕和外部 Flash 的 SPI 实际时钟均为 36 MHz。

如果 BY25Q80 JEDEC ID 不是 `68 40 14`，依赖外部 Flash 的案例会显示红色与黄色故障条；MCU 内置图片案例仍可运行。

当前基准固件 ARMCC 体积：Code 19,384 B，RO-data 5,024 B，RW-data 24 B，ZI-data 7,712 B。测试完成后主分支文档会追加优化后体积，以便比较。

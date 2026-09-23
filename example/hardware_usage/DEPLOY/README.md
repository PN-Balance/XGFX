# XGFX 硬件测试烧录包

目标硬件：PY32F031x8（72 MHz）、172×320 ST7789W3、BY25Q80ES 1 MiB SPI NOR Flash。

1. 将 `XGFX_Hardware_Test.hex` 烧入 MCU。
2. 用 G120F 编程器选择 `25 FLASH / BYT / BY25Q80ES`，把 `XGFX_External_Flash_1MiB.bin` 从地址 `0x000000` 写入外部 Flash，并执行校验。
3. 上电后按 [硬件回归测试说明](../TEST_GUIDE.md) 操作。

`XGFX_External_Flash.bin` 是有效资源区；1 MiB 文件是在尾部填充 `0xFF` 的整片烧录版本。资源总有效大小为 153,096 字节。屏幕和外部 Flash 的 SPI 实际时钟均为 36 MHz。

如果 BY25Q80 JEDEC ID 不是 `68 40 14`，依赖外部 Flash 的案例会显示红色与黄色故障条；MCU 内置图片案例仍可运行。

ARMCC 体积对比（相同 18 案例）：

| 版本 | Code | RO-data | RW-data | ZI-data |
|---|---:|---:|---:|---:|
| 优化前分支 `codex/pre-optimization-benchmark` | 19,384 B | 5,024 B | 24 B | 7,712 B |
| 优化后主分支 | 18,664 B | 5,048 B | 24 B | 7,712 B |

优化后的核心代码少 720 B；固定背景查表使用调用者已有工作区，不增加静态 RAM。真实速度需在同一块硬件上读取每个案例顶部的毫秒数。

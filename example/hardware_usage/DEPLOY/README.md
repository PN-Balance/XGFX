# XGFX hardware test image

This package targets the existing PY32F031x8 + 172x320 ST7789W3 board and a BY25Q80ES 1 MiB SPI NOR Flash.

## Program the board

1. Program `XGFX_Hardware_Test.hex` into the PY32F031x8 internal Flash at `0x08000000` with Keil/J-Link/CMSIS-DAP.
2. Program `XGFX_External_Flash_1MiB.bin` into the BY25Q80ES starting at address `0x000000` with the G120F programmer. Select `25 FLASH`, manufacturer `BYT`, model `BY25Q80ES`, load the file, then use the programmer's automatic write/verify action.
3. Fit the external Flash back on the board and power on.

`XGFX_External_Flash.bin` is the compact 76,648-byte equivalent. The 1 MiB file contains the same payload followed by `0xFF`, and is the safest choice when doing a full-chip write and verify.

## Buttons

- Button 1 short press: stop automatic playback and show the next case.
- Button 1 long press: toggle automatic playback of every case at 1.8-second intervals.
- Button 2 short press: stop automatic playback and return to case 1.
- Button 3 long press: turn off the backlight and release the board power-hold signal.

## Cases shown on screen

The large digit at the top is the case number. The green horizontal indicator means automatic playback is active.

1. Direct rectangles and circles, including negative coordinates and edge clipping.
2. RGB565 MIRROR from external Flash at two screen positions.
3. 4-bpp BITMAP_WITH_PALETTE quantization.
4. Independent `Cut_Self` and `Cut_Screen`, with the anchor based on `Cut_Self`.
5. RGB565 MIRROR with 4-bpp Alpha blended against existing buffer pixels.
6. 2-bpp palette image with 2-bpp Alpha blended against a fixed background color.
7. 1-bpp BITMAP with runtime foreground/background colors.
8. Buffer absolute coordinates and buffer-edge clipping for rectangles and circles.
9. Three placements of a 64x40 RGB565 MIRROR stored in MCU internal Flash.
10. A 40x40 MCU-resident 2-bpp palette image with 2-bpp Alpha, plus a 32x32 MCU-resident 1-bpp BITMAP.

If the BY25Q80 JEDEC ID is not `68 40 14`, cases 2-7 show red and amber fault bars instead of reading arbitrary Flash data. Case 1 remains available for checking the screen driver.

## Build result

- ARMCC 5.06 update 5: 0 errors, 0 warnings.
- Internal Flash: 18,104 bytes code + 6,516 bytes read-only data + 24 bytes initialized data (24,644 bytes total).
- MCU-resident test images: 6,056 bytes, covering MIRROR, palette + Alpha, and BITMAP storage.
- RAM: 6,944 bytes zero-initialized data + 24 bytes initialized data.
- External asset payload: 76,648 bytes of 1,048,576 bytes (7.31%).

SHA-256:

- `XGFX_Hardware_Test.hex`: `633C0E3BF6EC2DD3467F9C282E774FCC0192EC01FAB2519263B18B0AFC302AF9`
- `XGFX_Hardware_Test.bin`: `CA5783A41D3798AF3215F795EA924C2056CD54C5FD97925382BFFDD7F0B6A9A2`
- `XGFX_External_Flash.bin`: `85B7545706317D0D63C1A236CF495832698516F3664F2992053E109A23767337`
- `XGFX_External_Flash_1MiB.bin`: `6866236E61B3D89BD859979E3B8467FEEBB20A63AEACCA8CE975BDD292AB9594`

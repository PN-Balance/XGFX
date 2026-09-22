# XGFX 使用说明

本文件是 XGFX 图形库与资源生成器的完整使用教程。阅读顺序即学习顺序：先弄懂"是什么"，再从最简单的画点开始，逐步学会图片、裁剪、Alpha、资源生成，最后掌握硬件移植。

---

## 目录

1. [这是什么](#1-这是什么)
2. [目录结构](#2-目录结构)
3. [第一步：选择屏幕颜色类型](#3-第一步选择屏幕颜色类型)
4. [初始化与基本概念](#4-初始化与基本概念)
   - [缓冲区字节对齐（必读）](#43-缓冲区字节对齐必读)
5. [直接写屏绘图（Fill 系列）](#5-直接写屏绘图fill-系列)
6. [缓冲区绘图（Buff 系列）](#6-缓冲区绘图buff-系列)
7. [图片资源描述符 GFX_Img_t](#7-图片资源描述符-gfx_img_t)
8. [三种图片格式详解](#8-三种图片格式详解)
9. [高级绘图参数详解](#9-高级绘图参数详解)
10. [颜色处理函数](#10-颜色处理函数)
11. [区域 Area 与锚点 Anchor](#11-区域-area-与锚点-anchor)
12. [缓冲区切块与管理器](#12-缓冲区切块与管理器)
13. [硬件移植接口 GFX_Port](#13-硬件移植接口-gfx_port)
14. [资源生成器 xgfx_asset.py](#14-资源生成器-xgfx_assetpy)
15. [编译与模拟器测试](#15-编译与模拟器测试)
16. [常见问题与注意事项](#16-常见问题与注意事项)
17. [真机测试工程](#17-真机测试工程)

---

## 1. 这是什么

**XGFX** 是一个面向 MCU（单片机）的轻量级 C 语言图形库，外加一个图片资源生成器工具。

它解决两个问题：

1. **怎么在屏幕上画图** —— 提供画点、画矩形、画圆、画图片等函数，支持直接写屏和缓冲区两种方式。
2. **怎么把图片变成单片机能用的数组** —— 提供一个 Python 脚本，把 PNG/JPG 等图片转成 C 语言数组或二进制文件，库就能直接拿来画。

**核心特性：**

- 三种屏幕颜色格式：RGB332（1 字节）、RGB565（2 字节，默认）、RGB888（4 字节）
- 三种图片存储格式：MIRROR（原生彩色）、BITMAP（灰度位图）、BITMAP_WITH_PALETTE（索引色板）
- 位深 BPP 支持 1～5 bit
- 支持独立 Alpha 透明通道
- 支持 MCU 内置 Flash 数组和外部 Flash 两种存储
- 支持区域裁剪、九点锚点定位、固定背景 Alpha 混合

---

## 2. 目录结构

```
GFX Lib/
├── src/                    # 图形库核心源码（你要用到的 .h / .c）
│   ├── GFX.h               # 公共 API 声明
│   ├── GFX.c               # 核心绘制实现
│   ├── GFX_Define.h        # 类型定义、颜色配置、图片描述符
│   ├── GFX_Port.h          # 硬件适配接口声明（7 个函数）
│   ├── GFX_Port.c          # 硬件适配模板（你需要实现）
│   ├── Area.h / Area.c     # 区域矩形与锚点工具
│   └── General_Type.h      # 通用类型
├── sim/                    # Windows 模拟器 + 演示
├── tests/                  # 回归测试
├── tools/
│   ├── xgfx_script/        # 纯 Python 资源生成器（单文件）
│   └── xgfx_app/           # 安装版（XGFX-Setup.exe）
├── doc/                    # 使用说明（本文）
└── example/                # 用法示例
```

**你需要加入工程的文件：** `src/GFX.c`、`src/Area.c`、以及你自己实现的端口层（参考 `src/GFX_Port.c`）。不要同时链接模板端口和模拟端口。

---

## 3. 第一步：选择屏幕颜色类型

打开 [GFX_Define.h](../src/GFX_Define.h)，找到：

```c
#define GFX_COLOR_TYPE  ( GFX_COLOR_TYPE_RGB565 )
```

根据你的屏幕实际颜色格式改为其中之一：

| 宏 | GFX_Color_t 类型 | 每像素字节数 | 适用屏幕 |
| --- | --- | --- | --- |
| `GFX_COLOR_TYPE_RGB565` | `uint16_t` | 2 字节 | 大部分 SPI/MCU 屏（默认） |
| `GFX_COLOR_TYPE_RGB888` | `uint32_t` | 4 字节（低 24 位存 RGB） | 高端 RGB 接口屏 |
| `GFX_COLOR_TYPE_RGB332` | `uint8_t` | 1 字节 | 极低色深屏 |

> 注意：改了 `GFX_COLOR_TYPE` 后，必须重新用资源生成器生成图片资源，因为资源的每像素字节数会变。

---

## 4. 初始化与基本概念

### 4.1 初始化函数

```c
void GFX_Init( GFX_Init_t * Config );
```

**作用：** 初始化图形库和底层硬件。

**参数：**

| 参数 | 类型 | 说明 |
| --- | --- | --- |
| `Config` | `GFX_Init_t*` | 初始化配置，传 `NULL` 表示暂不提供工作缓冲区 |

`GFX_Init_t` 结构：

```c
typedef struct {
    void * Buffer;     // 工作缓冲区首地址
    uint32_t Size;     // 工作缓冲区大小，单位：字节
} GFX_Init_t;
```

**工作缓冲区是什么？** 画图时需要一块临时内存来存放解码出的像素、临时色板等。你提供一块 RAM 给库用。

### 4.2 示例

```c
#include "GFX.h"

// 工作缓冲区，大小按需要给，建议至少 2048 字节
static GFX_Color_t scratch[2048];

void App_Init(void)
{
    GFX_Init_t init = {
        .Buffer = scratch,
        .Size = sizeof(scratch)
    };
    GFX_Init(&init);              // 初始化库和硬件
    GFX_Set_Brightness(255);      // 设置亮度 0~255
    GFX_Fill_Bg(GFX_Color_Convert(0x202020));  // 填充背景色
}
```

如果暂时没有缓冲区：

```c
GFX_Init(NULL);   // 初始化硬件，不设工作区
// 之后需要时再设置：
GFX_Set_Buffer(scratch, sizeof(scratch));
```

### 4.3 缓冲区字节对齐（必读）

XGFX 内部经常用 `GFX_Color_t`（`uint16_t`/`uint32_t`）直接读写像素。如果缓冲区首地址没有按 `GFX_Color_t` 对齐，某些 MCU 会触发 **HardFault**，或者读出错误的像素值。这是最常见的崩溃原因之一。

**对齐要求：** 所有被库当作像素数组访问的内存，首地址必须满足 `_Alignof(GFX_Color_t)`。
- RGB565（`uint16_t`）→ 2 字节对齐
- RGB888（`uint32_t`）→ 4 字节对齐
- RGB332（`uint8_t`）→ 1 字节对齐（无要求）

**库帮你处理对齐的地方（放心用）：**

| 场景 | 谁负责对齐 |
| --- | --- |
| 工作缓冲区 `GFX_Init` / `GFX_Set_Buffer` | 库自动向上对齐，容量按像素数检查 |
| 像素缓冲区 `GFX_Buffer_Init` | 库自动向上对齐 `Memory`，并检查剩余容量 |
| 资源生成器输出的 MCU 数组 | 带 `GFX_COLOR_ALIGNAS` 声明（见 14.6） |

**必须自己保证对齐的地方（容易踩坑）：**

1. **手工填写 `GFX_Buffer_t`** —— 如果不用 `GFX_Buffer_Init`，而是自己给 `Buff.Buffer` 赋值，必须保证该地址已对齐。**强烈建议始终用 `GFX_Buffer_Init`。**

2. **MCU MIRROR 图片的 `Color_Save_Info.C_Array`** —— 直接 `Fill` 为了零拷贝，会按 `GFX_Color_t*` 直接访问该地址。手工写资源时必须保证对齐，否则：
   - 检查宏 `GFX_RESOURCE_ALIGNMENT_CHECK_ENABLE=1` 时 → 绘制被拒绝
   - 检查宏关闭时 → 可能 HardFault 或花屏
   - 用资源生成器输出的数组（带 `GFX_COLOR_ALIGNAS`）则无此问题

3. **外部 Flash 资源** —— 通过 `GFX_Port_Flash_Read` 搬到 RAM 后再访问，对齐由你的读取缓冲决定，建议缓冲也对齐。

**安全做法：** 用 `GFX_Color_t` 类型声明数组，编译器自然保证对齐：

```c
static GFX_Color_t scratch[2048];          // ✅ 天然对齐
static GFX_Color_t pixels[80 * 40];        // ✅ 天然对齐

// ❌ 不要这样：uint8_t 数组对齐无保证
static uint8_t bad_buffer[160];
GFX_Buffer_t buff = { .Buffer = (GFX_Color_t *)bad_buffer, ... };
```

### 4.4 基本控制函数

| 函数 | 作用 | 参数说明 |
| --- | --- | --- |
| `GFX_Set_Brightness(uint8_t b)` | 设置背光亮度 | `b`：0（最暗）～ 255（最亮） |
| `GFX_Set_Draw_Direction(Orth_Dir_t d)` | 设置绘制方向 | `Orth_Dir_0 / 90 / 180 / 270` |
| `GFX_Get_Draw_Direction()` | 获取当前方向 | 返回 `Orth_Dir_t` |
| `GFX_Get_Width()` | 获取屏幕逻辑宽度 | 90°/270° 时宽高会交换 |
| `GFX_Get_Height()` | 获取屏幕逻辑高度 | 同上 |
| `GFX_Get_X_Max()` | 获取 X 最大值 | = Width - 1 |
| `GFX_Get_Y_Max()` | 获取 Y 最大值 | = Height - 1 |

---

## 5. 直接写屏绘图（Fill 系列）

直接写屏就是调用函数后，像素立刻被送到屏幕上。适合简单图元和全屏刷新。

### 5.1 填充背景

```c
void GFX_Fill_Bg( GFX_Color_t Color_Bg );
```

**参数：** `Color_Bg` —— 屏幕颜色，用 `GFX_Color_Convert(0xRRGGBB)` 转换。

```c
GFX_Fill_Bg(GFX_Color_Convert(0x000000));  // 黑屏
```

### 5.2 画点

```c
void GFX_Fill_Point( int16_t X, int16_t Y, GFX_Color_t Color );
```

| 参数 | 说明 |
| --- | --- |
| `X`, `Y` | 点的坐标，左上角为 (0,0) |
| `Color` | 点的颜色 |

### 5.3 画实心矩形

```c
void GFX_Fill_Rect( GFX_Color_t Color, int16_t X, int16_t Y, uint16_t W, uint16_t H );
```

在 `(X, Y)` 处画一个 `W×H` 的实心矩形。坐标超出屏幕会自动裁剪。

### 5.4 画圆

```c
void GFX_Fill_Circle( GFX_Color_t Color, int16_t X, int16_t Y, uint16_t R, uint16_t Width );
```

| 参数 | 说明 |
| --- | --- |
| `X`, `Y` | 圆心坐标 |
| `R` | 半径（像素），`R=0` 不画 |
| `Width` | 圆环宽度，`Width=0` 画实心圆；`Width>0` 画圆环（外半径 R，内半径 R-Width） |

### 5.5 画图片

```c
void GFX_Fill_Img( const GFX_Img_t * Img, int16_t X, int16_t Y );
```

把整张图片画到屏幕 `(X, Y)` 位置。`Img` 是图片描述符（见第 7 章）。

如果需要裁剪、锚点、色板等高级功能，用 `GFX_Fill_Img_Adv`（见第 9 章）。

---

## 6. 缓冲区绘图（Buff 系列）

缓冲区绘图是先把像素画到一块内存（Buffer）里，最后再一次性 `Flush` 到屏幕。适合需要多个图层叠加、Alpha 混合的场景。

### 6.1 构建缓冲区

```c
bool GFX_Buffer_Init( GFX_Buffer_t * Buff, const Area_t * Area,
                      void * Memory, uint32_t Memory_Size );
```

| 参数 | 说明 |
| --- | --- |
| `Buff` | 输出的缓冲区对象 |
| `Area` | 缓冲区对应的屏幕区域（绝对坐标） |
| `Memory` | 像素内存首地址（任意字节地址都可以） |
| `Memory_Size` | 内存大小，单位：字节 |

**返回值：** `true` 成功，`false` 容量不足（会清空 Buff）。

> `Memory` 可以是任意字节地址，`GFX_Buffer_Init` 会自动向上对齐到 `GFX_Color_t` 边界，并扣除对齐浪费的容量后检查是否足够容纳 `W×H` 个像素。**务必用这个函数构建 Buffer，不要手工填 `GFX_Buffer_t.Buffer`**——手工填的地址若未对齐，`GFX_Buff_*` 会直接拒绝绘制，严重时触发 HardFault（详见 4.3 节对齐说明）。

`GFX_Buffer_t` 结构：

```c
typedef struct {
    Area_t Area;           // 缓冲区覆盖的屏幕区域
    GFX_Color_t * Buffer;  // 像素数组首地址（已对齐）
} GFX_Buffer_t;
```

### 6.2 缓冲区绘图函数

| 函数 | 作用 |
| --- | --- |
| `GFX_Buff_Bg(Buff, Color)` | 填充整个 Buff.Area 区域 |
| `GFX_Buff_Rect(Buff, X, Y, W, H, Color)` | 在 Buff 内画实心矩形（绝对坐标） |
| `GFX_Buff_Circle(Buff, X, Y, R, Color)` | 在 Buff 内画实心圆（绝对坐标） |
| `GFX_Buff_Img(Buff, Img, X, Y)` | 把图片画到 Buff 内（绝对坐标） |

> Buff 系列的坐标都是**屏幕绝对坐标**，不是相对缓冲区的坐标。函数会自动裁剪到 `Buff.Area` 范围内。

### 6.3 刷新到屏幕

```c
void GFX_Flush( GFX_Color_t * Buffer, int16_t X, int16_t Y, uint16_t W, uint16_t H );
```

| 参数 | 说明 |
| --- | --- |
| `Buffer` | 像素数组（连续 `W×H` 个像素） |
| `X`, `Y` | 屏幕目标区域左上角 |
| `W`, `H` | 区域宽高 |

### 6.4 完整示例

```c
static GFX_Color_t pixels[80 * 40];   // 80×40 的像素内存

void Draw_Buffer_Example(void)
{
    Area_t area = {20, 30, 80, 40};   // 屏幕上 (20,30) 开始的 80×40 区域
    GFX_Buffer_t buff;
    if (!GFX_Buffer_Init(&buff, &area, pixels, sizeof(pixels)))
        return;

    GFX_Buff_Bg(&buff, GFX_Color_Convert(0x202020));          // 填背景
    GFX_Buff_Rect(&buff, 25, 35, 10, 10,
                  GFX_Color_Convert(0xFF0000));               // 画红色方块
    GFX_Buff_Circle(&buff, 60, 50, 8,
                    GFX_Color_Convert(0x00FF00));             // 画绿色圆

    GFX_Flush(buff.Buffer, buff.Area.X, buff.Area.Y,
              buff.Area.W, buff.Area.H);                      // 刷新到屏幕
}
```

---

## 7. 图片资源描述符 GFX_Img_t

所有图片都用 `GFX_Img_t` 描述。它告诉库：图片多大、什么格式、数据存在哪、有没有 Alpha。

### 7.1 结构字段

```c
typedef struct {
    uint32_t W : 10;                  // 宽度 1~1023
    uint32_t H : 10;                  // 高度 1~1023
    uint32_t Color_Type : 2;          // 图片类型：0=MIRROR, 1=BITMAP, 2=BITMAP_WITH_PALETTE, 3=NONE
    uint32_t Color_Save_Way : 1;      // 颜色数据存储方式：0=外部Flash, 1=MCU内存
    uint32_t Color_Bits_Per_Pix : 3;  // 位图BPP：1~5（MIRROR填0）
    uint32_t Alpha_Enable : 1;        // 是否启用独立Alpha：0/1
    uint32_t Alpha_Save_Way : 2;      // Alpha存储方式：0=Flash, 1=MCU
    uint32_t Alpha_Bits_Per_Pix : 3;  // Alpha位深：1~5
    GFX_Save_Info_t Color_Save_Info;  // 颜色数据引用
    GFX_Save_Info_t Alpha_Save_Info;  // Alpha数据引用（未启用时忽略）
} GFX_Img_t;
```

`GFX_Save_Info_t` 联合体：

```c
typedef union {
    uint32_t Flash_Addr;   // 外部Flash字节地址（Color_Save_Way=0时用）
    uint8_t * C_Array;     // MCU内存数组指针（Color_Save_Way=1时用）
} GFX_Save_Info_t;
```

### 7.2 快速理解每个字段

| 字段 | 怎么填 |
| --- | --- |
| `W` / `H` | 图片的宽高像素数，范围 1～1023 |
| `Color_Type` | `GFX_COLOR_TYPE_MIRROR` / `GFX_COLOR_TYPE_BITMAP` / `GFX_COLOR_TYPE_BITMAP_WITH_PALETTE` |
| `Color_Save_Way` | 数据在 MCU 内存里填 `GFX_Save_Way_MCU`；在外部 Flash 填 `GFX_Save_Way_Flash` |
| `Color_Bits_Per_Pix` | 位图类型填 1～5；MIRROR 填 0 |
| `Alpha_Enable` | 有独立 Alpha 填 1，否则填 0（BITMAP 必须为 0） |
| `Alpha_Save_Way` / `Alpha_Bits_Per_Pix` | Alpha 的存储方式和位深，规则同颜色 |
| `Color_Save_Info` | 颜色数据的地址（Flash 地址或数组指针） |
| `Alpha_Save_Info` | Alpha 数据的地址（仅 Alpha_Enable=1 时有效） |

> 一般情况下你不需要手写 `GFX_Img_t`，用资源生成器自动生成即可。了解结构是为了理解生成出来的代码。

---

## 8. 三种图片格式详解

### 8.1 MIRROR —— 原生彩色图片

**是什么：** 每个像素直接存一个 `GFX_Color_t`，不压缩、不调色板。最直观、最占空间。

**适用场景：** 全彩图标、照片、UI 底图。

**数据布局：** 从左到右、从上到下逐行排列，每像素 `sizeof(GFX_Color_t)` 字节，无行填充。

**颜色大小：** `W × H × sizeof(GFX_Color_t)` 字节。

**示例（RGB565，2×1 像素）：**

```c
static const GFX_Color_t native_pixels[2] = {0xF800, 0x07E0};  // 红、绿

static const GFX_Img_t native_img = {
    .W = 2, .H = 1,
    .Color_Type = GFX_COLOR_TYPE_MIRROR,
    .Color_Save_Way = GFX_Save_Way_MCU,
    .Color_Save_Info = {.C_Array = (uint8_t *)native_pixels}
};
```

**使用：**

```c
GFX_Fill_Img(&native_img, 10, 10);   // 画到屏幕 (10,10)
```

> ⚠️ **MIRROR + MCU 的地址对齐要求最严格。** 直接 `Fill` 为了零拷贝，会把 `Color_Save_Info.C_Array` 当作 `GFX_Color_t*` 直接访问。如果该地址未按 `GFX_Color_t` 对齐：
> - 开启 `GFX_RESOURCE_ALIGNMENT_CHECK_ENABLE=1` → 绘制被跳过
> - 关闭检查宏 → 在不支持非对齐访问的 MCU 上直接 **HardFault**
>
> 安全做法：用 `GFX_Color_t` 数组存数据（如上面示例的 `native_pixels`），或保留资源生成器输出的 `GFX_COLOR_ALIGNAS` 声明。详见 4.3 节。

**Alpha 支持：** 可以带独立 Alpha 平面（见 8.4），但直接 `Fill` 会忽略 Alpha，只有 `Buff` 路径会混合。

---

### 8.2 BITMAP —— 灰度/索引位图

**是什么：** 每个像素存 1～5 bit 的值，表示灰度等级或索引。不附带调色板。

**适用场景：** 黑白图标、灰度图、需要运行时配色的图形。

**数据布局：** 逐行打包，**低位在前**，每行末尾补齐到整字节。

**每行字节数：** `row_bytes = (W × BPP + 7) / 8`
**总大小：** `row_bytes × H` 字节

**示例（5×2，1bit）：**

```c
static const uint8_t bitmap_pixels[] = {0x15, 0x0A};

static const GFX_Img_t bitmap_img = {
    .W = 5, .H = 2,
    .Color_Type = GFX_COLOR_TYPE_BITMAP,
    .Color_Save_Way = GFX_Save_Way_MCU,
    .Color_Bits_Per_Pix = 1,
    .Color_Save_Info = {.C_Array = (uint8_t *)bitmap_pixels}
};
```

**绘制效果：**
- 普通绘制：值 0 = 黑，最大值 = 白，中间线性灰度。
- 高级绘制：可以指定前景色/背景色做渐变，或传入自定义色板。

> BITMAP **不支持独立 Alpha**（`Alpha_Enable` 必须为 0）。BITMAP 的值 0 也不是透明像素。

---

### 8.3 BITMAP_WITH_PALETTE —— 索引彩色图片

**是什么：** 像素存的是调色板索引，每个索引对应一个 `GFX_Color_t`。比 MIRROR 省空间，比 BITMAP 多色。

**适用场景：** 低色彩图标、字体、仪表盘元素。最多 32 色（BPP=5 时）。

**数据布局：**

```
[索引位流：row_bytes × H 字节]   ← Color_Save_Info 指向这里
[调色板：(1<<BPP) 个 GFX_Color_t]  ← 紧跟在索引后面，无填充
```

**调色板偏移：** 严格等于 `row_bytes × H`，中间不能有额外字节。

**总大小：** `row_bytes × H + (1<<BPP) × sizeof(GFX_Color_t)` 字节

**示例（4×2，2bit，RGB565）：**

```c
typedef struct {
    uint8_t indices[2];              // 4×2 / 4bit/px = 2 字节索引
    GFX_Color_t palette[4];          // 2bit → 4 色调色板
} Palette_Asset;

static const Palette_Asset asset = {
    {0xE4, 0x1B},
    {0x0000, 0xF800, 0x07E0, 0x001F}  // 黑、红、绿、蓝
};

static const GFX_Img_t palette_img = {
    .W = 4, .H = 2,
    .Color_Type = GFX_COLOR_TYPE_BITMAP_WITH_PALETTE,
    .Color_Save_Way = GFX_Save_Way_MCU,
    .Color_Bits_Per_Pix = 2,
    .Color_Save_Info = {.C_Array = (uint8_t *)&asset}
};
```

**使用：**

```c
GFX_Fill_Img(&palette_img, 0, 0);   // 用图内默认色板绘制
```

> ⚠️ **色板起点可能不对齐。** 色板位于 `row_bytes × H` 偏移处，当这个偏移不是 `sizeof(GFX_Color_t)` 的整数倍时，色板首地址就不满足 `GFX_Color_t` 对齐。MCU 图内色板目前按类型指针读取，中间加填充会破坏"色板紧跟在位流后"的约定，所以**不要手工往索引和色板之间插 `padding` 去凑对齐**。
>
> 稳妥做法：
> - 需要严格控制的场合，用高级接口传入自己对齐的 `Palette` 覆盖图内色板；
> - 外部 Flash 色板会先被搬到工作区临时色板，不受此影响。
>
> 对齐的基础概念见 [4.3 节](#43-缓冲区字节对齐必读)。

**Alpha 支持：** 可以带独立 Alpha 平面（见 8.4）。

---

### 8.4 独立 Alpha 通道

MIRROR 和 BITMAP_WITH_PALETTE 可以附带独立的 Alpha 平面，实现透明效果。

**Alpha 平面布局：** 完整的 `W×H` 平面，逐行打包（同 BITMAP 的位流规则），`alpha_bytes = ((W × Alpha_BPP + 7) / 8) × H`。

**Alpha 值含义：**
- `0` = 完全透明
- `(1 << Alpha_BPP) - 1` = 完全不透明
- 中间值 = 按比例混合

**使用条件：** 只有 **Buff 系列**函数会处理 Alpha，**Fill 系列**会忽略 Alpha（直接覆盖）。

**混合公式：** `结果 = (源色 × Alpha + 背景色 × (max - Alpha)) / max`

背景色有两种来源：
- 默认：Buffer 里原有的像素
- 固定背景：设置 `Blend_Back_Color_Enable = true`，用 `Back_Color` 作为背景

---

## 9. 高级绘图参数详解

当普通 `Fill_Img` / `Buff_Img` 不够用时（需要裁剪、锚点、自定义色板、Alpha 混合等），用高级接口。

### 9.1 两个高级结构体

```c
GFX_Fill_Img_Adv_Para_t   // 直接写屏的高级参数
GFX_Buff_Img_Adv_Para_t   // 缓冲区的高级参数（多了 Buff 和 Blend_Back_Color_Enable）
```

### 9.2 公共字段（两种类型都有）

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `Img` | `const GFX_Img_t*` | 原图描述符（始终指向完整原图） |
| `X` | `int16_t` | 锚点在屏幕上的目标 X 坐标 |
| `Y` | `int16_t` | 锚点在屏幕上的目标 Y 坐标 |
| `Anchor` | `Anchor_t` | 九点锚点，决定 X/Y 对应图片的哪个位置 |
| `Cut_Screen_Enable` | `bool` | 是否启用屏幕裁剪 |
| `Cut_Screen` | `Area_t` | 屏幕裁剪矩形（只画这个范围内的像素） |
| `Cut_Self_Enable` | `bool` | 是否启用原图局部裁剪 |
| `Cut_Self` | `Area_t` | 原图裁剪矩形（只取原图这部分内容） |
| `Anchor_Use_Cut_Self` | `bool` | `true`=按 Cut_Self 区域锚定；`false`=按整图锚定 |
| `Palette` | `GFX_Color_t*` | 覆盖色板（NULL 时 BITMAP 用渐变、PALETTE 用图内色板） |
| `Enable_Map` | `uint32_t` | 位图索引使能位掩码，bit N=1 显示索引 N，=0 用 Back_Color |
| `Fore_Color` | `uint32_t` | RGB888 渐变终点（仅 BITMAP 且 Palette=NULL） |
| `Back_Color` | `uint32_t` | RGB888 渐变起点/屏蔽替代色/固定背景 |

**Buff 专属字段：**

| 字段 | 类型 | 说明 |
| --- | --- | --- |
| `Buff` | `const GFX_Buffer_t*` | 目标缓冲区 |
| `Blend_Back_Color_Enable` | `bool` | `true`=用 Back_Color 做 Alpha 固定背景；`false`=用 Buffer 原像素 |

### 9.3 调用函数

```c
void GFX_Fill_Img_Adv( GFX_Fill_Img_Adv_Para_t * Para );
void GFX_Buff_Img_Adv( GFX_Buff_Img_Adv_Para_t * Para );
```

### 9.4 示例 1：从图集中取一帧

```c
void Draw_Frame(const GFX_Img_t *sheet)
{
    GFX_Fill_Img_Adv_Para_t p = {0};
    p.Img = sheet;
    p.X = 100;
    p.Y = 60;
    p.Anchor = Anchor_LT;                    // 锚点用左上角
    p.Cut_Self_Enable = true;                // 只取原图局部
    p.Cut_Self = (Area_t){16, 32, 16, 16};   // 取原图 (16,32) 开始的 16×16
    p.Anchor_Use_Cut_Self = true;             // 锚定到 Cut_Self 区域
    GFX_Fill_Img_Adv(&p);
}
```

### 9.5 示例 2：BITMAP 自定义前后景渐变

```c
void Draw_Bitmap_Gradient(const GFX_Buffer_t *buff, const GFX_Img_t *img)
{
    GFX_Buff_Img_Adv_Para_t p = {0};
    p.Buff = buff;
    p.Img = img;
    p.X = 0; p.Y = 0;
    p.Anchor = Anchor_LT;
    p.Enable_Map = 0xFFFFFFFFu;   // 所有索引都显示
    p.Fore_Color = 0x0000FF;       // 前景蓝（RGB888）
    p.Back_Color = 0x000000;       // 背景黑
    GFX_Buff_Img_Adv(&p);
}
```

### 9.6 示例 3：PALETTE 图片 Alpha 混合到固定背景

```c
void Draw_Alpha_Palette(const GFX_Buffer_t *buff, const GFX_Img_t *img)
{
    GFX_Buff_Img_Adv_Para_t p = {0};
    p.Buff = buff;
    p.Img = img;                  // 带独立 Alpha 的 PALETTE 图
    p.X = buff->Area.X;
    p.Y = buff->Area.Y;
    p.Anchor = Anchor_LT;
    p.Enable_Map = 0xFFFFFFFFu;
    p.Back_Color = 0x202020;
    p.Blend_Back_Color_Enable = true;  // 用 Back_Color 做固定背景
    GFX_Buff_Img_Adv(&p);
}
```

### 9.7 字段在不同图片类型中的作用

| 字段 | MIRROR | BITMAP | BITMAP_WITH_PALETTE |
| --- | --- | --- | --- |
| `Palette` | 忽略 | 覆盖渐变表 | 覆盖图内色板 |
| `Enable_Map` | 忽略 | bit N=1 显示 | bit N=1 显示 |
| `Fore_Color` | 忽略 | 渐变终点 | 忽略 |
| `Back_Color` | Fill忽略；Buff可做固定背景 | 渐变起点+屏蔽色 | 屏蔽色+Buff固定背景 |
| `Blend_Back_Color_Enable` | 仅Buff+Alpha | 忽略 | 仅Buff+Alpha |

> **重要：** `{0}` 初始化会让 `Enable_Map=0`，这意味着位图的所有索引都被屏蔽！使用位图高级接口时必须显式设置 `Enable_Map = 0xFFFFFFFFu`。BITMAP 还需设置 `Fore_Color`，否则零值下是黑到黑的渐变。

---

## 10. 颜色处理函数

### 10.1 RGB888 转设备色

```c
GFX_Color_t GFX_Color_Convert( uint32_t RGB888 );
```

输入 `0xRRGGBB`，返回当前 `GFX_COLOR_TYPE` 对应的设备颜色值。

```c
GFX_Color_t red = GFX_Color_Convert(0xFF0000);
```

### 10.2 设备色分解为 RGB

```c
void GFX_Color_Get_RGB( GFX_Color_t color, uint8_t *r, uint8_t *g, uint8_t *b );
```

把设备色拆成 R/G/B 三个分量（0～255）。

### 10.3 颜色插值

```c
uint32_t GFX_Color_Transiton_RGB888( uint32_t RGB888_Active, uint32_t RGB888_Target, double Ratio );
```

在 `RGB888_Active` 和 `RGB888_Target` 之间按 `Ratio`（0.0～1.0）插值，返回 RGB888。

### 10.4 Alpha 混合

```c
GFX_Color_t GFX_Color_AlphaBlend( GFX_Color_t bg, GFX_Color_t fg, uint8_t alpha, uint8_t max_alpha );
```

混合前景色 `fg` 和背景色 `bg`，`alpha` 是不透明度，`max_alpha` 是最大值。

### 10.5 生成调色板

```c
void GFX_Create_Gray_Palette( uint8_t bpp, GFX_Color_t * palette );
```

生成 `1<<bpp` 项的黑白灰调色板（0=黑，最大=白）。

```c
void GFX_Create_Color_Palette( uint8_t BPP, GFX_Color_t *Palette,
                               uint32_t Front_Color, uint32_t Back_Color );
```

生成从 `Back_Color` 到 `Front_Color` 的 `1<<BPP` 项渐变色板（颜色为 RGB888）。

### 10.6 复制图内调色板

```c
void GFX_Img_Copy_Palette( const GFX_Img_t * Img, GFX_Color_t * Dest );
```

从 BITMAP_WITH_PALETTE 图片中复制其内置调色板到 `Dest`（至少 `1<<BPP` 项）。

---

## 11. 区域 Area 与锚点 Anchor

### 11.1 Area_t

```c
typedef struct {
    int16_t X;      // 左上角 X
    int16_t Y;      // 左上角 Y
    uint16_t W;     // 宽度
    uint16_t H;     // 高度
} Area_t;
```

表示矩形区域 `[X, X+W) × [Y, Y+H)`，W 或 H 为 0 表示无效区域。

### 11.2 Anchor_t 九点锚点

```
Anchor_LT  Anchor_CT  Anchor_RT
Anchor_LM  Anchor_CM  Anchor_RM
Anchor_LB  Anchor_CB  Anchor_RB
```

L=左，C=中，R=右；T=上，M=中，B=下。

### 11.3 区域关系函数

| 函数 | 返回值 | 作用 |
| --- | --- | --- |
| `Area_Com(A1, A2, Com)` | `bool` | 求交集，结果存入 `Com`，返回是否非空 |
| `Area_In(Active, Target)` | `bool` | `Active` 是否完全包含在 `Target` 内 |
| `Area_Over(Active, Target)` | `bool` | 两区域是否相交 |
| `Area_Contain(Active, Target)` | `bool` | 同 `Area_In` |
| `Area_Equal(A1, A2)` | `bool` | 四字段是否全部相等 |
| `Area_Is_Valid(A)` | `bool` | 区域是否有效（非空、尺寸 > 0） |

### 11.4 区域操作函数

| 函数 | 作用 |
| --- | --- |
| `Area_Move(A, Anchor, X, Y)` | 把 `A` 的指定锚点移动到 `(X, Y)` |
| `Area_Move_Offset(A, X_Off, Y_Off)` | 平移区域 |
| `Area_Get_Anchor_X(A, Anchor)` | 获取区域指定锚点的 X 坐标 |
| `Area_Get_Anchor_Y(A, Anchor)` | 获取区域指定锚点的 Y 坐标 |
| `Area_Inflate(A, Offset, Anchor)` | 围绕锚点把区域扩大/缩小 `Offset` |

---

## 12. 缓冲区切块与管理器

当屏幕太大、内存装不下整帧缓冲时，需要把屏幕分成小块逐块绘制。

### 12.1 切块器 Cutter

```c
void GFX_Buffer_Cutter_Init( Area_t * Area_Src, uint32_t Buffer_Size );
bool GFX_Buffer_Cutter_Cut( Area_t * Area_Buff );
```

**用法：**

1. `GFX_Buffer_Cutter_Init(&region, pixel_count)` —— 设定要切的区域和每块最多像素数。
2. 循环调用 `GFX_Buffer_Cutter_Cut(&part)` —— 每次返回一个子区域，返回 `false` 表示切完。

```c
void Render_Tiles(void)
{
    static GFX_Color_t tile[256];
    Area_t region = {0, 0, 80, 40};
    Area_t part;

    GFX_Buffer_Cutter_Init(&region, 256);   // 每块最多 256 像素
    while (GFX_Buffer_Cutter_Cut(&part)) {
        GFX_Buffer_t b;
        if (!GFX_Buffer_Init(&b, &part, tile, sizeof(tile)))
            return;
        GFX_Buff_Bg(&b, GFX_Color_Convert(0x202020));
        // 在此用绝对坐标重画与 part 相交的全部图层
        GFX_Flush(tile, part.X, part.Y, part.W, part.H);
    }
}
```

### 12.2 缓冲区管理器 Manager

从一块大内存里顺序分配子缓冲，类似简单的内存池。

```c
void GFX_Buffer_Manager_Init( GFX_Buffer_Manager_t * Manager, void * Buffer,
                              uint16_t Bytes_Per_Pix, uint32_t Max );
void * GFX_Buffer_Manager_Alloc( GFX_Buffer_Manager_t * Manager, uint32_t Size );
void GFX_Buffer_Manager_Clear( GFX_Buffer_Manager_t * Manager );
```

| 参数 | 说明 |
| --- | --- |
| `Buffer` | 大内存首地址 |
| `Bytes_Per_Pix` | 每个元素的字节数 |
| `Max` | 最大元素数 |
| `Size`（Alloc） | 要分配的元素数 |

`Clear` 只把分配指针归零，不清像素。没有单独的 free，用完整块 Clear 即可。

---

## 13. 硬件移植接口 GFX_Port

要让 XGFX 在你的硬件上工作，需要实现以下 7 个函数（参考 [GFX_Port.c](../src/GFX_Port.c) 模板）。

### 13.1 七个端口函数

| 函数 | 你要做什么 |
| --- | --- |
| `GFX_Port_Init(GFX_t *dev)` | 初始化 LCD 和总线，填充 `dev->W`、`dev->H`、`dev->Draw_Direction`、`dev->Brightness`、`dev->Scr_Area` |
| `GFX_Port_Fill(Color, X, Y, W, H)` | 用单色填充屏幕矩形区域 |
| `GFX_Port_Flush(Buffer, X, Y, W, H)` | 把连续 `W×H` 像素写到屏幕 `(X,Y)` 区域 |
| `GFX_Port_Flash_To_GFX(Addr, X, Y, W, H)` | 把外部 Flash 中 `Addr` 处的原生颜色数据直接搬到屏幕（可 DMA） |
| `GFX_Port_Flash_Read(Addr, Buffer, Size)` | 从外部 Flash `Addr` 读 `Size` 字节到 `Buffer` |
| `GFX_Port_Set_Brightness(b)` | 设置背光 PWM 亮度 |
| `GFX_Port_Set_Draw_Direction(dir)` | 设置屏幕硬件扫描方向（0/90/180/270） |

### 13.2 注意事项

- 核心直接调用这 7 个函数，不通过函数指针表分发。
- 先实现同步输出，确认正常后再扩展 DMA 异步。
- `GFX_Port_Flush` 需要正确处理裁剪和总线格式。
- ⚠️ **`GFX_Port_Flush` 收到的 `Buffer` 是 `GFX_Color_t*`**，如果你的总线/DMA 要求特定地址对齐（如 4 字节 burst 传输），请在这里做转换或二次缓冲；不要假设传入地址天然满足总线对齐要求。
- ⚠️ **`GFX_Port_Flash_Read` 的目标 `Buffer` 由调用方提供**，库内部会按 `GFX_Color_t` 访问，所以你分配这个读取缓冲时也要对齐（详见 4.3 节）。
- 模拟器的行为不等于真实硬件已验证，请在实机上测试。

---

## 14. 资源生成器 xgfx_asset.py

资源生成器把图片转换成 XGFX 能用的 C 数组、头文件和二进制文件。

### 14.1 两种使用方式

**方式一：纯 Python 脚本**（无需安装）

```powershell
pip install pillow
cd 你的图片目录
python 路径\tools\xgfx_script\xgfx_asset.py
```

**方式二：安装版**（GUI + 命令行）

运行 `tools\xgfx_app\XGFX-Setup.exe` 安装后，在任意目录使用：

```
xgfx init      # 初始化项目，创建 .xgfx/xgfx_assets.json
xgfx scan      # 扫描新图片，更新配置
xgfx build     # 生成资源
xgfx reset     # 重置配置到默认值
xgfx           # 打开网页版 GUI
```

### 14.2 工作流程

1. **初始化** —— 在图片目录运行脚本，自动扫描所有图片，生成 `.xgfx/xgfx_assets.json` 配置文件，并把脚本本身复制到 `.xgfx/`。
2. **编辑配置** —— 打开 `.xgfx/xgfx_assets.json`，为每张图片设置类型、色深、存储方式等。
3. **构建** —— 再次运行脚本，生成 C/H/BIN 等文件。

### 14.3 配置文件结构

```json
{
  "schema_version": 1,
  "color_mode": "free",
  "defaults": {
    "type": "MIRROR",
    "storage": "FLASH",
    "color_format": "RGB565",
    "byte_order": "little",
    "bpp": 0,
    "alpha_bpp": 0
  },
  "flash": {
    "base_address": "0x0",
    "alignment": 1,
    "fill_byte": 255
  },
  "output": {
    "directory": ".",
    "code_directory": ".",
    "name": "gfx_assets",
    "flash_file": "gfx_assets.bin",
    "write_individual_bins": false,
    "write_previews": false
  },
  "assets": [
    {
      "id": "icon.png",
      "name": "Img_icon",
      "source": "icon.png",
      "type": "BITMAP_WITH_PALETTE",
      "bpp": 4,
      "storage": "MCU"
    }
  ]
}
```

### 14.4 配置字段说明

**defaults（全局默认）：**

| 字段 | 取值 | 说明 |
| --- | --- | --- |
| `type` | `MIRROR` / `BITMAP` / `BITMAP_WITH_PALETTE` | 图片类型 |
| `storage` | `MCU` / `FLASH` | 存储方式 |
| `color_format` | `RGB332` / `RGB565` / `RGB888` | 设备颜色格式 |
| `byte_order` | `little` / `big` | 字节序 |
| `bpp` | `0`（MIRROR）或 `1~5`（位图） | 颜色位深 |
| `alpha_bpp` | `0` 或 `1~5` | Alpha 位深（BITMAP 必须 0） |
| `quantization` | `DOMINANT` / `MEDIAN_CUT` / `MAX_COVERAGE` / `FAST_OCTREE` | 调色板图片的默认量化算法 |
| `background_color` | `#RRGGBB` | 关闭 Alpha 通道时，透明像素合成使用的背景色 |

**flash（外部 Flash 布局）：**

| 字段 | 说明 |
| --- | --- |
| `base_address` | Flash 起始地址 |
| `alignment` | 对齐字节数 |
| `fill_byte` | 填充字节值 |

**output（输出设置）：**

| 字段 | 说明 |
| --- | --- |
| `directory` | 普通文件输出目录（相对 `.xgfx`） |
| `code_directory` | C/H 输出目录 |
| `name` | 生成文件名前缀 |
| `flash_file` | Flash 总包 BIN 文件名 |
| `write_individual_bins` | 是否输出每个资源的单独 BIN |
| `write_previews` | 是否输出预览 PNG |

**assets（每张图片的配置，可覆盖 defaults）：**

| 字段 | 说明 |
| --- | --- |
| `id` | 唯一标识 |
| `name` | 生成的 C 变量名 |
| `source` | 图片路径（相对 `.xgfx`） |
| `type` | 图片类型（覆盖 defaults） |
| `storage` | 存储方式 |
| `bpp` | 色深 |
| `alpha_bpp` | Alpha 位深 |
| `palette` | 自定义色板（仅 BITMAP_WITH_PALETTE） |
| `auto_quantize` | 是否自动量化到调色板 |
| `quantization` | 自动量化算法，可覆盖全局默认值 |
| `background_color` | 不生成 Alpha 通道时的透明图片合成背景色 |
| `flash_address` | 外部 Flash 地址（storage=FLASH 时需要） |

JPEG 本身没有 Alpha 通道，因此 JPEG 配置 `alpha_bpp > 0` 会直接报错。PNG 等带透明度的图片如果设置 `alpha_bpp = 0`，生成器会先按 `background_color` 合成成不透明图片，再进行颜色转换和量化；透明区域不会被随意替换成黑色或其他颜色。

`DOMINANT` 适合图标和少量主色的 UI 素材；另外三种算法来自 Pillow，可根据照片、渐变或复杂插画的实际预览效果选择。显式填写 `palette` 时始终优先使用手工色板，不再执行自动量化。

### 14.5 生成产物

构建后会生成：

- `gfx_assets.h` —— 所有图片描述符的 `extern` 声明
- `gfx_assets.c` —— 图片数据数组和描述符定义
- `gfx_assets.bin` —— 外部 Flash 总包（如有 FLASH 资源）
- `gfx_assets.json` —— 资源报告
- `*.bin` —— 单个资源 BIN（开启 `write_individual_bins`）
- `*_preview.png` —— 预览图（开启 `write_previews`）

### 14.6 MCU 资源对齐约定

生成器输出的每个 MCU 数组声明都带 `GFX_COLOR_ALIGNAS`：

```c
GFX_COLOR_ALIGNAS static const uint8_t icon_data[] = { /* ... */ };
```

**不要删除或改写这个声明。** 它由库按当前 `GFX_Color_t` 和编译器展开，保证数组首地址满足对齐要求。即使当前资源是按字节解码的 BITMAP，也请保留——资源类型变更或后续直接像素访问时它就是安全保障。

如果手写资源或从生成结果里复制数组却漏掉了 `GFX_COLOR_ALIGNAS`，在不支持非对齐访问的 MCU 上会直接 **HardFault**（详见 [4.3 节](#43-缓冲区字节对齐必读)）。

**外部 Flash 资源不受此约束**：它们通过 `GFX_Port_Flash_Read` 搬到 RAM 后才被访问，对齐由你的读取缓冲决定。

---

## 15. 编译与模拟器测试

### 15.1 Windows 模拟器

环境：PowerShell + MinGW-w64 GCC。

```powershell
# 编译并运行测试
.\sim\build.ps1 -Configuration Debug -Test

# 编译并运行演示窗口
.\sim\build.ps1 -Configuration Release -Run
```

输出在 `sim/build/`。模拟器默认 240×320、RGB565。

### 15.2 资源生成器测试

```powershell
python -m unittest tests\test_xgfx_asset.py -v
```

### 15.3 嵌入到你的工程

1. 把 `src/GFX.c`、`src/Area.c` 和你实现的 `GFX_Port.c` 加入编译。
2. 确保 `GFX_COLOR_TYPE` 与你的屏幕一致。
3. 调用 `GFX_Init` 开始使用。

---

## 16. 常见问题与注意事项

### 16.0 ⚠️ 字节对齐（最高优先级）

缓冲区和图片资源的首地址**必须按 `GFX_Color_t` 对齐**，否则在不支持非对齐访问的 MCU 上会直接 **HardFault**。这是本库最容易踩的坑，没有之一。

- ✅ **工作缓冲区 / 像素缓冲区**：用 `GFX_Color_t` 数组声明，或用 `GFX_Buffer_Init` 构建（自动对齐）。
- ✅ **图片资源数组**：保留资源生成器输出的 `GFX_COLOR_ALIGNAS` 声明，不要删。
- ✅ **手工写 MIRROR 资源**：`Color_Save_Info.C_Array` 必须对齐，直接 `Fill` 会按 `GFX_Color_t*` 访问。
- ❌ 不要用 `uint8_t` 数组强转成 `GFX_Color_t*` 当像素缓冲。
- 调试期可开 `GFX_RESOURCE_ALIGNMENT_CHECK_ENABLE=1` 让库帮你检查 MCU MIRROR 资源地址。

详见 [4.3 节](#43-缓冲区字节对齐必读)。

### 16.1 资源相关

- **改了 `GFX_COLOR_TYPE` 必须重新生成资源。** 颜色字节数变了，旧资源不能用。
- **BITMAP 不支持独立 Alpha。** 如果配置了，生成器和绘图函数都会报错/跳过。
- **Enable_Map 默认为 0。** 用高级接口画位图时必须设 `0xFFFFFFFFu`，否则什么都不显示。
- **BITMAP 的值 0 不是透明。** 它就是黑色（或渐变的起点）。
- **描述符不保存资源长度。** 库不会验证数组或 Flash 分区大小，调用方需保证资源正确。

### 16.2 内存相关

- **工作缓冲区和像素缓冲区不能重叠。** 解码时临时空间会覆盖源数据。
- **Buffer 像素内存必须用 `GFX_Buffer_Init` 构建。** 它会处理对齐和容量检查。
- **不要在绘制期间切换工作区。** 全局状态共享，不支持并发或重入。
- **工作区不足时某些路径会跳过绘制。** 直接填色、MCU MIRROR 等不依赖工作区的功能仍可用。

### 16.3 坐标相关

- **所有 Buff 系列坐标都是屏幕绝对坐标。** 不是相对缓冲区的坐标。
- **Area.W 就是行跨度。** 没有独立 stride，不能把全屏缓冲的子矩形直接当紧凑 Buffer 用。
- **坐标范围是 int16_t。** 极端大坐标要注意溢出。

### 16.4 编译相关

- 按 C11 编译，建议开启 `-Wall -Wextra`。
- `GFX_FILL_BITMAP_BY_BUFF` 宏目前不控制有效绘图分支，无需设置。
- `GFX_RESOURCE_ALIGNMENT_CHECK_ENABLE` 调试阶段可设为 1 检查资源对齐，发布版可关闭。

---

## 17. 真机测试工程

`example/hardware_usage` 是可直接编译和烧录的完整测试工程，硬件为 PY32F031x8、172×320 ST7789W3 屏幕和 BY25Q80ES 1 MiB SPI NOR Flash。工程已经移除原产品 UI 的编译入口，启动后直接进入 XGFX 测试。

测试包含 10 个案例，覆盖矩形与圆、越界裁剪、MIRROR、BITMAP、BITMAP_WITH_PALETTE、Alpha 混合、固定背景混合、`Cut_Self`、`Cut_Screen`、锚点、缓冲区绝对坐标，以及 MCU 内部 Flash 和外部 Flash 两种资源来源。由于目标芯片只有 8 KiB RAM，图片通过窄条缓冲逐段解码和刷新，避免申请整屏缓冲。

按键操作：

- 按键 1 短按：下一个案例；长按：开始或停止每 1.8 秒自动轮播。
- 按键 2 短按：返回第一个案例。
- 按键 3 长按：关闭背光并释放电源保持信号。

`example/hardware_usage/DEPLOY` 中提供 MCU 的 HEX/BIN、外部 Flash 紧凑镜像和填充到 1 MiB 的完整镜像。具体烧录地址、文件校验值和资源占用见该目录的 `README.md`。测试图片的来源和许可见 `example/hardware_usage/GFX_Test/SOURCES.md`。

---

## 附录：所有 API 速查表

### 初始化与控制

| 函数 | 作用 |
| --- | --- |
| `GFX_Init(Config)` | 初始化库和硬件 |
| `GFX_Set_Buffer(ptr, size)` | 设置工作缓冲区 |
| `GFX_Set_Brightness(b)` | 设置亮度 |
| `GFX_Set_Draw_Direction(dir)` | 设置绘制方向 |
| `GFX_Get_Draw_Direction()` | 获取方向 |
| `GFX_Get_Width() / Height()` | 获取屏幕宽高 |
| `GFX_Get_X_Max() / Y_Max()` | 获取坐标最大值 |
| `GFX_Flush(buf, x, y, w, h)` | 刷新缓冲到屏幕 |

### 直接绘图

| 函数 | 作用 |
| --- | --- |
| `GFX_Fill_Bg(color)` | 填背景 |
| `GFX_Fill_Point(x, y, color)` | 画点 |
| `GFX_Fill_Rect(color, x, y, w, h)` | 画实心矩形 |
| `GFX_Fill_Circle(color, x, y, r, width)` | 画圆/圆环 |
| `GFX_Fill_Img(img, x, y)` | 画图片 |
| `GFX_Fill_Img_Adv(para)` | 高级画图片 |

### 缓冲区绘图

| 函数 | 作用 |
| --- | --- |
| `GFX_Buffer_Init(buff, area, mem, size)` | 构建缓冲区 |
| `GFX_Buff_Bg(buff, color)` | 填缓冲区背景 |
| `GFX_Buff_Rect(buff, x, y, w, h, color)` | 画矩形 |
| `GFX_Buff_Circle(buff, x, y, r, color)` | 画圆 |
| `GFX_Buff_Img(buff, img, x, y)` | 画图片 |
| `GFX_Buff_Img_Adv(para)` | 高级画图片 |

### 颜色

| 函数 | 作用 |
| --- | --- |
| `GFX_Color_Convert(rgb888)` | RGB888 → 设备色 |
| `GFX_Color_Get_RGB(color, &r, &g, &b)` | 设备色 → RGB |
| `GFX_Color_Transiton_RGB888(a, b, ratio)` | 颜色插值 |
| `GFX_Color_AlphaBlend(bg, fg, a, max)` | Alpha 混合 |
| `GFX_Create_Gray_Palette(bpp, pal)` | 生成灰度板 |
| `GFX_Create_Color_Palette(bpp, pal, front, back)` | 生成渐变色板 |
| `GFX_Img_Copy_Palette(img, dest)` | 复制图内色板 |

### 区域与缓冲区管理

| 函数 | 作用 |
| --- | --- |
| `Area_Com / In / Over / Contain / Equal / Is_Valid` | 区域关系判断 |
| `Area_Move / Move_Offset / Get_Anchor_X/Y / Inflate` | 区域操作 |
| `GFX_Buffer_Cutter_Init / Cut` | 屏幕切块 |
| `GFX_Buffer_Manager_Init / Alloc / Clear` | 缓冲内存池 |

/**
 * @file GFX_Define.h
 * @author my_GFX
 * @brief GFX 模块基础类型定义与配置
 * @version 0.0.0.1
 * @date 2026-08-30
 * 
 * 
 */
#ifndef _GFX_DEFINE_H_
#define _GFX_DEFINE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 - 必须 模块所必须依赖的头文件 */
/* -------------------------------------------------------------------------------------------------------------------------- */
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Area.h"
#include "General_Type.h"

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 - 配置 通过宏定义之类的参数对模块进行配置的头文件 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 - debug 为调试而存在的头文件 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* ========================================================================================================================== */
/* 宏定义 - 可配置（CONFIG） —— 用户可按项目修改；改变此区会改变驱动工作方式                                     */
/* ========================================================================================================================== */

/* --- 测试 / 功能开关 ------------------------------------------------------------------------------------------------------ */

// GFX 测试初始化
#define GFX_TEST_ENABLE ( 0 )
#if GFX_TEST_ENABLE
    // GFX 测试驱动
    #define GFX_PORT_TEST_ENABLE    ( false )
#endif

// 是否通过缓冲区填充位图
#define GFX_FILL_BITMAP_BY_BUFF ( false )

/* MCU 图片资源地址合法性检查。生成器资源已保证对齐，发布版可关闭以减小开销。
 * 手工制作或修改图片资源的调试阶段建议设为 1。Buffer 与工作区检查不受此开关影响。 */
#ifndef GFX_RESOURCE_ALIGNMENT_CHECK_ENABLE
    #define GFX_RESOURCE_ALIGNMENT_CHECK_ENABLE ( 0 )
#endif

/* --- 屏幕颜色类型选择（三选一，决定 GFX_Color_t 的位宽） ------------------------------------------------------------------ */

// 可供选择的颜色类型
#define GFX_COLOR_TYPE_RGB888   ( 0 )
#define GFX_COLOR_TYPE_RGB565   ( 1 )
#define GFX_COLOR_TYPE_RGB332   ( 2 )

// 屏幕的指定颜色类型
#define GFX_COLOR_TYPE  ( GFX_COLOR_TYPE_RGB565 )

/* --- 位图 BPP 取值（离散枚举值 + 范围阈值，供 Color_Bits_Per_Pix / Alpha_Bits_Per_Pix 与脚本生成资产共用） ---------- */

// BPP 枚举取值（连续 1 ~ 5，和原最大位深度一致，每个像素最多 32 种灰度）
#define GFX_BPP_1                           ( 1 )
#define GFX_BPP_2                           ( 2 )
#define GFX_BPP_3                           ( 3 )
#define GFX_BPP_4                           ( 4 )
#define GFX_BPP_5                           ( 5 )

// BPP 范围阈值
#define GFX_BPP_MIN                         ( GFX_BPP_1 )
#define GFX_BPP_MAX                         ( GFX_BPP_5 )

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 宏定义 - 私有快速访问（PRIVATE SAVE HELPERS） —— 跨模块快速访问/断言类宏，使用时再填充                                  */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 宏定义 - 通用辅助（GENERAL HELPERS） —— MIN/MAX/CLAMP/PACKED/UNUSED 等通用宏，使用时再填充                                   */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 宏定义 - 固定硬件阈值（FIXED HW CONSTANTS） —— 与硬件规格绑定，不随项目变化                                        */
/* -------------------------------------------------------------------------------------------------------------------------- */

// GFX 亮度值 最大 最小
#define GFX_BRIGHTNESS_MAX  ( 255 )
#define GFX_BRIGHTNESS_MIN  ( 0 )

/* ========================================================================================================================== */
/* 宏定义 - 固定常量（FIXED CONSTANTS） —— 类型枚举/描述子字段的合法取值集合，请勿修改                               */
/* ========================================================================================================================== */

/* --- 存储方式（Save_Way 位域与 GFX_Save_Info_t 的 switch 使用） ------------------------------------------------- */

#define GFX_Save_Way_Flash ( 0 )
#define GFX_Save_Way_MCU   ( 1 )

/* --- 图片色彩类型（GFX_Img_t.Color_Type : 2 位域使用，有效值 0~2；Value=3 保留） --------------------------------------------- */

/**
 * @brief 图片色彩类型统一枚举（3 个有效值连续，脚本生成资产和绘制函数共同遵守）。
 *
 * @details 下列 3 个宏直接对应 GFX_Img_t.Color_Type 位域 0 / 1 / 2 的三个有效值，集中解释如下：
 *
 *  - @b Value = 0  @b GFX_COLOR_TYPE_MIRROR：直接镜像（Mirror）
 *      · 颜色数据是"按屏幕像素排列顺序、每像素固定字节数连续存储"的原始镜像，不支持调色板，也不支持按位压缩。
 *      · 每像素字节数由工程级 GFX_COLOR_TYPE 配置决定（= sizeof(GFX_Color_t)），图片自身不需要记录。
 *      · Color_Bits_Per_Pix 字段对 MIRROR 类型无意义（它是位图类型用于 bit 流寻址的），脚本生成时填 0 即可，绘制端不读。
 *      · Color_Save_Info 的读取方式由 Color_Save_Way 决定：
 *        - GFX_Save_Way_Flash → Color_Save_Info.Flash_Addr 指向 Flash 中原始镜像的起始地址（可直接 DMA / 寄存器窗口搬运到 GFX GRAM）。
 *        - GFX_Save_Way_MCU   → Color_Save_Info.C_Array    指向 RAM 中原始镜像的像素数组（可直接 memcpy 到帧缓冲）。
 *      · 典型应用：全彩 UI 底图 / 图标 / 照片类资源；解析函数可直接按位宽做线性搬运，无需逐像素解 BPP。
 *
 *  - @b Value = 1  @b GFX_COLOR_TYPE_BITMAP：位图（Bitmap，不带外部调色板）
 *      · 颜色数据按 BPP 压缩打包为 bit 流，每个像素从 bit 流中解出索引，但索引"不带外部调色板"，索引直接等于颜色值或灰度等级。
 *      · Color_Bits_Per_Pix：合法取值 = GFX_BPP_1 ~ GFX_BPP_5（1 ~ 5，推荐 1 / 2 / 4，对应单色 / 4 色 / 16 色）。
 *      · Color_Save_Info.Flash_Addr / C_Array 指向**位图 bit 流首地址**。bit 流格式：逐行扫描，每字节低位在前，阳码（1=有效像素/前景，0=无效/背景），像素按从左到右、从上到下排列，每行末尾不足一字节时补齐到整字节（例：宽 15px、BPP=1 → 每行 15bit，补齐为 2 字节）。
 *      · 典型应用：黑白 / 灰度图 + 上层用 Palette 配色；是 Buff_Bitmap / Fill_Bitmap 逻辑的基础输入格式。
 *      · 不允许独立 Alpha 通道；Alpha_Enable 必须为 0，所有 Alpha 字段和 Alpha_Save_Info 均忽略。
 *
 *  - @b Value = 2  @b GFX_COLOR_TYPE_BITMAP_WITH_PALETTE：位图 + 外部调色板（Bitmap With Palette）
 *      · 颜色数据同上按 BPP 压缩为 bit 流，但解出的索引必须到外部调色板 @c Palette[] 中查 GFX_Color_t 才是最终颜色。
 *      · Color_Bits_Per_Pix：合法取值 = GFX_BPP_1 ~ GFX_BPP_5（1 ~ 5，索引宽度决定调色板大小 = 1 << BPP，最多 32 色）。
 *      · Color_Save_Info 指向**位图 bit 流首地址**，bit 流格式与 GFX_COLOR_TYPE_BITMAP 完全相同。
 *      · 默认调色板紧跟完整索引位流，偏移为 ceil(W*BPP/8)*H，包含 1<<BPP 个 GFX_Color_t，中间无填充。
 *      · 高级接口可传入 Palette 覆盖默认表；NULL 时读取上述图内表。描述符不额外保存色板指针。
 *      · 典型应用：低色彩深度的图标 / 字体渲染 / 多色仪表盘元素。
 *      · Alpha 通道同样独立存储于 Alpha_Save_Info（仅当 GFX_Img_t.Alpha_Enable = 1 时有效）。
 *
 * @note
 *  - 以上 3 条约束在脚本生成端（资源转码）与绘制端（GFX_Fill_Img / GFX_Buff_Img 等）两侧必须同时遵守。
 *  - GFX_COLOR_TYPE_NONE(3) 为无效类型标记，用于未初始化的图片描述子，各处理函数应屏蔽。
 */
#define GFX_COLOR_TYPE_MIRROR                 ( 0 )
#define GFX_COLOR_TYPE_BITMAP                 ( 1 )
#define GFX_COLOR_TYPE_BITMAP_WITH_PALETTE    ( 2 )
#define GFX_COLOR_TYPE_NONE                   ( 3 )   /* 无效类型：未初始化的图片描述子 */

/* ========================================================================================================================== */
/* 类型定义 - 基础通用（BASIC TYPES） —— 坐标/颜色/枚举/存储信息/简单位图，不依赖脚本生成资产                              */
/* ========================================================================================================================== */

// 屏幕颜色类型
#if (GFX_COLOR_TYPE == GFX_COLOR_TYPE_RGB888)
    typedef uint32_t GFX_Color_t ;
#elif (GFX_COLOR_TYPE == GFX_COLOR_TYPE_RGB565)
    typedef uint16_t GFX_Color_t ;
#elif (GFX_COLOR_TYPE == GFX_COLOR_TYPE_RGB332)
    typedef uint8_t GFX_Color_t ;
#endif

/* 资源生成器统一放在数组声明前，保证 MCU 资源首地址可作为 GFX_Color_t 访问。 */
#if defined(__cplusplus)
    #define GFX_COLOR_ALIGNAS alignas( GFX_Color_t )
#elif defined(__STDC_VERSION__) && ( __STDC_VERSION__ >= 201112L )
    #define GFX_COLOR_ALIGNAS _Alignas( GFX_Color_t )
#elif defined(__GNUC__) || defined(__clang__)
    #define GFX_COLOR_ALIGNAS __attribute__(( aligned( sizeof( GFX_Color_t ) ) ))
#elif defined(__ICCARM__)
    #define GFX_COLOR_ALIGNAS _Pragma("data_alignment=4")
#else
    #error "Define GFX_COLOR_ALIGNAS for this compiler"
#endif

// 垂直位置
typedef enum 
{
    Vertical_Pos_Up ,
    Vertical_Pos_Middle ,
    Vertical_Pos_Down
} Vertical_Pos_t ;

// 水平位置
typedef enum
{
    Horizontal_Pos_Left ,
    Horizontal_Pos_Middle ,
    Horizontal_Pos_Right
} Hor_Pos_t;

// 正交方向
typedef enum {
    Orth_Dir_0 = 0 ,
    Orth_Dir_90 = 1 ,
    Orth_Dir_180 = 2 ,
    Orth_Dir_270 = 3 ,
    Orth_Dir_Max = 4 
}Orth_Dir_t ;

// 坐标点类型
typedef struct {
    int16_t X ;
    int16_t Y ;
} GFX_Coord_t ;

typedef union {
    uint32_t Flash_Addr ;
    uint8_t * C_Array ;
} GFX_Save_Info_t ;

/* ========================================================================================================================== */
/* 类型定义 - 脚本生成资产（ASSET TYPES） —— 图片资源，由资源转换脚本生成并作为 const 数组存储                                      */
/* ========================================================================================================================== */
typedef struct {
    /* 公共区域：所有图片类型有效。 */
    uint32_t W : 10 ;
    uint32_t H : 10 ;
    uint32_t Color_Type : 2 ;

    /* 颜色区域：Save_Way 所有类型有效；Bits_Per_Pix 仅 BITMAP 两种类型有效。 */
    uint32_t Color_Save_Way : 1 ;
    uint32_t Color_Bits_Per_Pix : 3 ;

    /* Alpha 区域：仅 MIRROR 和 BITMAP_WITH_PALETTE 支持；BITMAP 必须全部置 0。 */
    uint32_t Alpha_Enable : 1 ;
    uint32_t Alpha_Save_Way : 2 ;
    uint32_t Alpha_Bits_Per_Pix : 3 ;

    /* 数据引用：Alpha_Save_Info 仅在受支持类型且 Alpha_Enable=1 时有效。 */
    GFX_Save_Info_t Color_Save_Info ;
    GFX_Save_Info_t Alpha_Save_Info ;
} GFX_Img_t ;

/* ========================================================================================================================== */
/* 宏定义 - 资产声明辅助（ASSET DECLARE HELPERS） —— 配合脚本生成的 const 图片资产做声明与取地址                                   */
/* ========================================================================================================================== */

// 声明图片（资产 extern 声明；业务侧只需要 const GFX_Img_t* 引用，不需要额外的 GET 宏）
#define GFX_IMG_DECLEARE( IMG ) extern const GFX_Img_t IMG ;

/* ========================================================================================================================== */
/* 类型定义 - 运行时对象（RUNTIME TYPES） —— 运行期分配/引用，GFX.c 核心使用                                    */
/* ========================================================================================================================== */

typedef struct {
    Area_t Area ;    // 缓冲区区域
    GFX_Color_t * Buffer ;  // 像素缓冲区；推荐通过 GFX_Buffer_Init 构建以保证地址对齐和容量充足
} GFX_Buffer_t ;

typedef struct {
    void * Buffer ;
    uint16_t Bytes_Per_Pix ;
    uint32_t Index ;
    uint32_t Max ;
} GFX_Buffer_Manager_t ;

// GFX 初始化配置（后续可扩展其他成员）
typedef struct {
    void * Buffer ;       // 缓冲区首地址
    uint32_t Size ;       // 缓冲区大小（字节）
} GFX_Init_t ;

// GFX 数据对象类型
typedef struct 
{
    uint16_t W ;
    uint16_t H ;
    Orth_Dir_t Draw_Direction ;
    uint8_t Brightness ;
    Area_t Scr_Area ; // 屏幕区域
} GFX_t ;

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 外部变量引用声明 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 函数声明 - 功能 对上层模块提供的功能API*/
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 函数声明 - 接口 对下层依赖所需的接口 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 函数声明 - 回调 对下层的依赖而言，需被调用的回调函数 */
/* -------------------------------------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif

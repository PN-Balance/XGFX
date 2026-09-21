/**
 * @file main.c
 * @brief XGFX 模拟器入口 —— 调用 GFX API 绘制测试画面
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "GFX.h"
#include "GFX_Sim.h"

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 测试缓冲区 */
/* -------------------------------------------------------------------------------------------------------------------------- */

static uint8_t s_Buffer[ 4096 ] ;

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 测试场景：绘制基本图形验证 Fill_Bg / Fill_Point / Fill_Rect */
/* -------------------------------------------------------------------------------------------------------------------------- */

__attribute__((unused)) static void Test_Basic_Draw( void )
{
    /* 黑底 */
    GFX_Fill_Bg( GFX_Color_Convert( 0x000000 ) ) ;

    /* 红色矩形：左上角 */
    GFX_Fill_Rect( GFX_Color_Convert( 0xFF0000 ) , 10 , 10 , 80 , 60 ) ;

    /* 绿色矩形：右下角 */
    GFX_Fill_Rect( GFX_Color_Convert( 0x00FF00 ) , 150 , 250 , 80 , 60 ) ;

    /* 蓝色矩形：超出屏幕边界（测试裁剪） */
    GFX_Fill_Rect( GFX_Color_Convert( 0x0000FF ) , 200 , 200 , 80 , 60 ) ;

    /* 白色点：中心 */
    GFX_Fill_Point( 120 , 160 , GFX_Color_Convert( 0xFFFFFF ) ) ;

    /* 黄色点群：对角线 */
    for( int16_t i = 0 ; i < 240 ; i += 10 )
    {
        GFX_Fill_Point( i , i * 320 / 240 , GFX_Color_Convert( 0xFFFF00 ) ) ;
    }

    /* 越界点测试（不应崩溃，不应绘制） */
    GFX_Fill_Point( -1 , -1 , GFX_Color_Convert( 0xFF0000 ) ) ;
    GFX_Fill_Point( 240 , 320 , GFX_Color_Convert( 0xFF0000 ) ) ;

    /* 青色实心圆：左中 */
    GFX_Fill_Circle( GFX_Color_Convert( 0x00FFFF ) , 60 , 160 , 40 , 0 ) ;

    /* 紫色圆环：右中，线宽 5 */
    GFX_Fill_Circle( GFX_Color_Convert( 0xFF00FF ) , 180 , 160 , 40 , 5 ) ;

    /* 橙色小实心圆：右上 */
    GFX_Fill_Circle( GFX_Color_Convert( 0xFF8800 ) , 200 , 40 , 15 , 0 ) ;

    /* 白色细圆环：左下，线宽 2 */
    GFX_Fill_Circle( GFX_Color_Convert( 0xFFFFFF ) , 60 , 270 , 30 , 2 ) ;

    /* === 缓冲区测试 === */
    /* 分配一个 100×60 的缓冲区 */
    #define BASIC_BUFF_W  100
    #define BASIC_BUFF_H  60
    static GFX_Color_t buff[ BASIC_BUFF_W * BASIC_BUFF_H ] ;
    GFX_Buffer_t gbuff ;
    gbuff.Buffer = buff ;
    gbuff.Area.X = 0 ; gbuff.Area.Y = 0 ;
    gbuff.Area.W = BASIC_BUFF_W ; gbuff.Area.H = BASIC_BUFF_H ;

    /* 缓冲区：深蓝底 */
    GFX_Buff_Bg( &gbuff , GFX_Color_Convert( 0x000080 ) ) ;

    /* 缓冲区：红色矩形 */
    GFX_Buff_Rect( &gbuff , 5 , 5 , 30 , 20 , GFX_Color_Convert( 0xFF0000 ) ) ;

    /* 缓冲区：绿色矩形（部分超出缓冲区，测试裁剪） */
    GFX_Buff_Rect( &gbuff , 80 , 40 , 30 , 30 , GFX_Color_Convert( 0x00FF00 ) ) ;

    /* 缓冲区：黄色实心圆 */
    GFX_Buff_Circle( &gbuff , 50 , 30 , 15 , GFX_Color_Convert( 0xFFFF00 ) ) ;

    /* 将缓冲区刷到屏幕右下角 */
    GFX_Flush( buff , 130 , 220 , BASIC_BUFF_W , BASIC_BUFF_H ) ;

    GFX_Sim_Refresh() ;
}

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 测试场景：MIRROR 图片绘制 + 裁剪验证 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* 造一张 32×24 的彩色渐变 MIRROR 图片 */
#define MIRROR_W  32
#define MIRROR_H  24
/* 运行时在 Test_Mirror_Img 中填充渐变数据（避免 const 数组写入崩溃） */
static GFX_Color_t s_Mirror_Data[ MIRROR_W * MIRROR_H ] ;

static GFX_Img_t s_Mirror_Img = {
    .W = MIRROR_W ,
    .H = MIRROR_H ,
    .Color_Type = GFX_COLOR_TYPE_MIRROR ,
    .Color_Save_Way = GFX_Save_Way_MCU ,
    .Color_Bits_Per_Pix = 0 ,
    .Alpha_Enable = 0 ,
    .Alpha_Save_Way = 0 ,
    .Alpha_Bits_Per_Pix = 0 ,
    .Color_Save_Info = { .C_Array = (uint8_t *)s_Mirror_Data },
    .Alpha_Save_Info = { 0 },
} ;

/* Flash 存储的同一张渐变图（内容来自 _mirror.bin，与 MCU 版完全一致） */
static GFX_Img_t s_Mirror_Img_Flash = {
    .W = MIRROR_W ,
    .H = MIRROR_H ,
    .Color_Type = GFX_COLOR_TYPE_MIRROR ,
    .Color_Save_Way = GFX_Save_Way_Flash ,
    .Color_Bits_Per_Pix = 0 ,
    .Alpha_Enable = 0 ,
    .Alpha_Save_Way = 0 ,
    .Alpha_Bits_Per_Pix = 0 ,
    .Color_Save_Info = { .Flash_Addr = 0 },
    .Alpha_Save_Info = { 0 },
} ;

__attribute__((unused)) static void Test_Mirror_Img( void )
{
    /* 运行时填充渐变数据（const 数组初始为 0，这里写入） */
    GFX_Color_t * data = (GFX_Color_t *)s_Mirror_Data ;
    for( uint16_t y = 0 ; y < MIRROR_H ; y++ )
    {
        for( uint16_t x = 0 ; x < MIRROR_W ; x++ )
        {
            uint8_t r = (uint8_t)( x * 255 / ( MIRROR_W - 1 ) ) ;
            uint8_t g = (uint8_t)( y * 255 / ( MIRROR_H - 1 ) ) ;
            uint8_t b = 128 ;
            data[ y * MIRROR_W + x ] = GFX_Color_Convert( ( r << 16 ) | ( g << 8 ) | b ) ;
        }
    }

    /* 黑底 */
    GFX_Fill_Bg( GFX_Color_Convert( 0x000000 ) ) ;

    /* === MCU 路径（左半屏） === */
    GFX_Fill_Img( &s_Mirror_Img , 5 , 5 ) ;           /* 正常绘制 */
    GFX_Fill_Img( &s_Mirror_Img , 60 , 5 ) ;          /* 正常绘制，并排 */
    GFX_Fill_Img( &s_Mirror_Img , -10 , 40 ) ;        /* 超出左上角 */
    GFX_Fill_Img( &s_Mirror_Img , 200 , 40 ) ;        /* 超出右边界 */

    /* === Flash 路径（右半屏 / 下半屏，与 MCU 同图对比） === */
    GFX_Fill_Img( &s_Mirror_Img_Flash , 5 , 140 ) ;       /* 正常绘制 */
    GFX_Fill_Img( &s_Mirror_Img_Flash , 60 , 140 ) ;      /* 正常绘制，并排 */
    GFX_Fill_Img( &s_Mirror_Img_Flash , -10 , 180 ) ;     /* 超出左上角 */
    GFX_Fill_Img( &s_Mirror_Img_Flash , 200 , 180 ) ;     /* 超出右边界 */

    GFX_Sim_Refresh() ;
}

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 测试场景：BITMAP_WITH_PALETTE 图片绘制 + 裁剪验证 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* 12x12 BPP=2 四色位图（左上红/右上绿/左下蓝/右下紫），调色板紧跟像素数据 */
#define PAL_W  12
#define PAL_H  12

GFX_COLOR_ALIGNAS static const uint8_t s_Palette_Img_Data[] = {
    0x55, 0xA5, 0xAA, 0x55, 0xA5, 0xAA, 0x55, 0xA5, 0xAA, 0x55, 0xA5, 0xAA,
    0x55, 0xA5, 0xAA, 0x55, 0xA5, 0xAA, 0xFF, 0x0F, 0x00, 0xFF, 0x0F, 0x00,
    0xFF, 0x0F, 0x00, 0xFF, 0x0F, 0x00, 0xFF, 0x0F, 0x00, 0xFF, 0x0F, 0x00,
    0x10, 0x80, 0x00, 0xF8, 0xE0, 0x07, 0x1F, 0x00,
} ;

/* MCU 路径：像素+调色板都在 s_Palette_Img_Data 数组中 */
static GFX_Img_t s_Palette_Img_MCU = {
    .W = PAL_W ,
    .H = PAL_H ,
    .Color_Type = GFX_COLOR_TYPE_BITMAP_WITH_PALETTE ,
    .Color_Save_Way = GFX_Save_Way_MCU ,
    .Color_Bits_Per_Pix = GFX_BPP_2 ,
    .Alpha_Enable = 0 ,
    .Alpha_Save_Way = 0 ,
    .Alpha_Bits_Per_Pix = 0 ,
    .Color_Save_Info = { .C_Array = (uint8_t *)s_Palette_Img_Data },
    .Alpha_Save_Info = { 0 },
} ;

/* Flash 路径：调色板图在 _flash.bin 偏移 1536 处（紧跟 _mirror.bin 之后） */
static GFX_Img_t s_Palette_Img_Flash = {
    .W = PAL_W ,
    .H = PAL_H ,
    .Color_Type = GFX_COLOR_TYPE_BITMAP_WITH_PALETTE ,
    .Color_Save_Way = GFX_Save_Way_Flash ,
    .Color_Bits_Per_Pix = GFX_BPP_2 ,
    .Alpha_Enable = 0 ,
    .Alpha_Save_Way = 0 ,
    .Alpha_Bits_Per_Pix = 0 ,
    .Color_Save_Info = { .Flash_Addr = 1536 },
    .Alpha_Save_Info = { 0 },
} ;

__attribute__((unused)) static void Test_Palette_Img( void )
{
    /* 黑底 */
    GFX_Fill_Bg( GFX_Color_Convert( 0x000000 ) ) ;

    /* === MCU 路径（上半屏） === */
    GFX_Fill_Img( &s_Palette_Img_MCU , 5 , 5 ) ;            /* 正常绘制：左上红/右上绿/左下蓝/右下紫 */
    GFX_Fill_Img( &s_Palette_Img_MCU , 25 , 5 ) ;           /* 并排第二张 */
    GFX_Fill_Img( &s_Palette_Img_MCU , -5 , 40 ) ;          /* 超出左上角（裁掉左侧+上侧） */
    GFX_Fill_Img( &s_Palette_Img_MCU , 230 , 40 ) ;         /* 超出右边界（裁掉右侧） */

    /* === Flash 路径（下半屏，与 MCU 同图对比） === */
    GFX_Fill_Img( &s_Palette_Img_Flash , 5 , 140 ) ;        /* 正常绘制 */
    GFX_Fill_Img( &s_Palette_Img_Flash , 25 , 140 ) ;       /* 并排第二张 */
    GFX_Fill_Img( &s_Palette_Img_Flash , -5 , 180 ) ;       /* 超出左上角 */
    GFX_Fill_Img( &s_Palette_Img_Flash , 230 , 180 ) ;      /* 超出右边界 */

    GFX_Sim_Refresh() ;
}

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 测试场景：BITMAP 图片绘制（灰度调色板自动生成） + 裁剪验证 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* 12x12 BPP=1 黑底白色 X 图案（无调色板，由库自动生成 0=黑/1=白） */
#define BMP_W  12
#define BMP_H  12

GFX_COLOR_ALIGNAS static const uint8_t s_Bitmap_Img_Data[] = {
    0x01, 0x08, 0x02, 0x04, 0x04, 0x02, 0x08, 0x01, 0x90, 0x00, 0x60, 0x00,
    0x60, 0x00, 0x90, 0x00, 0x08, 0x01, 0x04, 0x02, 0x02, 0x04, 0x01, 0x08,
} ;

static GFX_Img_t s_Bitmap_Img_MCU = {
    .W = BMP_W ,
    .H = BMP_H ,
    .Color_Type = GFX_COLOR_TYPE_BITMAP ,
    .Color_Save_Way = GFX_Save_Way_MCU ,
    .Color_Bits_Per_Pix = GFX_BPP_1 ,
    .Alpha_Enable = 0 ,
    .Alpha_Save_Way = 0 ,
    .Alpha_Bits_Per_Pix = 0 ,
    .Color_Save_Info = { .C_Array = (uint8_t *)s_Bitmap_Img_Data },
    .Alpha_Save_Info = { 0 },
} ;

/* Flash 路径：BITMAP 图在 _flash.bin 偏移 1580 处（紧跟 palette 图之后） */
static GFX_Img_t s_Bitmap_Img_Flash = {
    .W = BMP_W ,
    .H = BMP_H ,
    .Color_Type = GFX_COLOR_TYPE_BITMAP ,
    .Color_Save_Way = GFX_Save_Way_Flash ,
    .Color_Bits_Per_Pix = GFX_BPP_1 ,
    .Alpha_Enable = 0 ,
    .Alpha_Save_Way = 0 ,
    .Alpha_Bits_Per_Pix = 0 ,
    .Color_Save_Info = { .Flash_Addr = 1580 },
    .Alpha_Save_Info = { 0 },
} ;

__attribute__((unused)) static void Test_Bitmap_Img( void )
{
    /* 灰底（与调色板背景色 0x808080 一致） */
    GFX_Fill_Bg( GFX_Color_Convert( 0x808080 ) ) ;

    /* === MCU 路径（上半屏）=== */
    GFX_Fill_Img( &s_Bitmap_Img_MCU , 5 , 5 ) ;             /* 正常绘制：黑底白X */
    GFX_Fill_Img( &s_Bitmap_Img_MCU , 25 , 5 ) ;            /* 并排第二张 */
    GFX_Fill_Img( &s_Bitmap_Img_MCU , -5 , 40 ) ;           /* 超出左上角 */
    GFX_Fill_Img( &s_Bitmap_Img_MCU , 230 , 40 ) ;          /* 超出右边界 */

    /* === Flash 路径（下半屏，与 MCU 同图对比）=== */
    GFX_Fill_Img( &s_Bitmap_Img_Flash , 5 , 140 ) ;         /* 正常绘制 */
    GFX_Fill_Img( &s_Bitmap_Img_Flash , 25 , 140 ) ;        /* 并排第二张 */
    GFX_Fill_Img( &s_Bitmap_Img_Flash , -5 , 180 ) ;        /* 超出左上角 */
    GFX_Fill_Img( &s_Bitmap_Img_Flash , 230 , 180 ) ;       /* 超出右边界 */

    GFX_Sim_Refresh() ;
}

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 测试场景：Cut_Self 从纵向图集中选择 H/E/L 三个字模 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* 12x36 BPP=1 BITMAP，3 等分（H/E/L），无调色板 */
#define SHEET_W  12
#define SHEET_H  36
#define SHEET_FRAME_H 12

GFX_COLOR_ALIGNAS static const uint8_t s_Sheet_Img_Data[] = {
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0xFF, 0x03,
    0xFF, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
    0xFF, 0x03, 0xFF, 0x03, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0xFF, 0x00,
    0xFF, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0xFF, 0x03, 0xFF, 0x03,
    0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x00,
    0x03, 0x00, 0x03, 0x00, 0x03, 0x00, 0x03, 0x03, 0xFF, 0x03, 0xFF, 0x03,
} ;

static GFX_Img_t s_Sheet_Img_MCU = {
    .W = SHEET_W ,
    .H = SHEET_H ,
    .Color_Type = GFX_COLOR_TYPE_BITMAP ,
    .Color_Save_Way = GFX_Save_Way_MCU ,
    .Color_Bits_Per_Pix = GFX_BPP_1 ,
    .Alpha_Enable = 0 ,
    .Alpha_Save_Way = 0 ,
    .Alpha_Bits_Per_Pix = 0 ,
    .Color_Save_Info = { .C_Array = (uint8_t *)s_Sheet_Img_Data },
    .Alpha_Save_Info = { 0 },
} ;

/* Flash 路径：图集在 _flash.bin 偏移 1604 处（紧跟 bitmap 之后） */
static GFX_Img_t s_Sheet_Img_Flash = {
    .W = SHEET_W ,
    .H = SHEET_H ,
    .Color_Type = GFX_COLOR_TYPE_BITMAP ,
    .Color_Save_Way = GFX_Save_Way_Flash ,
    .Color_Bits_Per_Pix = GFX_BPP_1 ,
    .Alpha_Enable = 0 ,
    .Alpha_Save_Way = 0 ,
    .Alpha_Bits_Per_Pix = 0 ,
    .Color_Save_Info = { .Flash_Addr = 1604 },
    .Alpha_Save_Info = { 0 },
} ;

__attribute__((unused)) static void Test_Cut_Self_Frames( void )
{
    /* 灰底 */
    GFX_Fill_Bg( GFX_Color_Convert( 0x808080 ) ) ;

    GFX_Fill_Img_Adv_Para_t para = {
        .Img = &s_Sheet_Img_MCU,
        .Anchor = Anchor_LT,
        .Anchor_Use_Cut_Self = true,
        .Cut_Self_Enable = true,
        .Cut_Self = { 0, 0, SHEET_W, SHEET_FRAME_H },
        .Enable_Map = 0xFFFFFFFFu,
        .Fore_Color = 0xFFFFFFu,
        .Back_Color = 0x808080u
    } ;

    /* === MCU 路径（上半屏）：H E L 横排 === */
    for( uint16_t i = 0 ; i < 3 ; i++ )
    {
        para.X = (int16_t)( 30 + i * 40 ) ;
        para.Y = 40 ;
        para.Cut_Self.Y = (int16_t)( i * SHEET_FRAME_H ) ;
        GFX_Fill_Img_Adv( &para ) ;
    }

    /* 整图也画一次（参考） */
    GFX_Fill_Img( &s_Sheet_Img_MCU , 160 , 20 ) ;

    /* === Flash 路径（下半屏）：H E L 横排 === */
    para.Img = &s_Sheet_Img_Flash ;
    for( uint16_t i = 0 ; i < 3 ; i++ )
    {
        para.X = (int16_t)( 30 + i * 40 ) ;
        para.Y = 180 ;
        para.Cut_Self.Y = (int16_t)( i * SHEET_FRAME_H ) ;
        GFX_Fill_Img_Adv( &para ) ;
    }

    /* 整图也画一次（参考） */
    GFX_Fill_Img( &s_Sheet_Img_Flash , 160 , 160 ) ;

    GFX_Sim_Refresh() ;
}

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 测试场景：GFX_Fill_Img_Adv 高级贴图（锚点 + 调色板覆盖 + Enable_Map 掩码） */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* 自定义覆盖调色板：原 紫黑红蓝 → 改为 白黄青洋红 */
static GFX_Color_t s_Custom_Palette[4] ;

__attribute__((unused)) static void Test_Fill_Img_Adv( void )
{
    /* 灰底 */
    GFX_Fill_Bg( GFX_Color_Convert( 0x808080 ) ) ;

    /* 准备自定义覆盖调色板（覆盖原图调色板） */
    s_Custom_Palette[0] = GFX_Color_Convert( 0xFFFFFF ) ;  /* 白 */
    s_Custom_Palette[1] = GFX_Color_Convert( 0xFFFF00 ) ;  /* 黄 */
    s_Custom_Palette[2] = GFX_Color_Convert( 0x00FFFF ) ;  /* 青 */
    s_Custom_Palette[3] = GFX_Color_Convert( 0xFF00FF ) ;  /* 洋红 */

    GFX_Fill_Img_Adv_Para_t para = { 0 } ;
    para.Img = &s_Palette_Img_MCU ;

    /* === 第 1 组：锚点对齐测试（上半屏）=== */
    /* 左上角对齐：原图绘制 */
    para.Palette    = NULL ;             /* 用原图自带调色板 */
    para.Enable_Map = 0xFFFFFFFF ;       /* 全显示 */
    para.Back_Color = 0x808080 ;         /* 背景灰（RGB888） */
    para.Fore_Color = 0xFFFFFF ;         /* 前景白（RGB888，BITMAP 用） */
    para.Anchor     = Anchor_LT ;
    para.X = 10 ; para.Y = 10 ;
    GFX_Fill_Img_Adv( &para ) ;

    /* 中心对齐：同一坐标，图中心落在 (60,16) */
    para.Anchor = Anchor_CM ;
    para.X = 60 ; para.Y = 16 ;
    GFX_Fill_Img_Adv( &para ) ;

    /* 右下对齐：图的右下角落在 (120,28) */
    para.Anchor = Anchor_RB ;
    para.X = 120 ; para.Y = 28 ;
    GFX_Fill_Img_Adv( &para ) ;

    /* === 第 2 组：调色板覆盖测试（上半屏右侧）=== */
    /* 用自定义调色板，四象限颜色变化：紫黑红蓝 → 白黄青洋红 */
    para.Palette = s_Custom_Palette ;
    para.Enable_Map = 0xFFFFFFFF ;
    para.Anchor = Anchor_LT ;
    para.X = 140 ; para.Y = 10 ;
    GFX_Fill_Img_Adv( &para ) ;

    /* === 第 3 组：Enable_Map 透明掩码测试（下半屏）=== */
    /* 只显示索引 1（原图红色），其余显示 Back_Color（灰） */
    para.Palette    = NULL ;             /* 用原图调色板 */
    para.Enable_Map = 0x00000002 ;       /* bit1=1，只显示索引 1 */
    para.Anchor     = Anchor_LT ;
    para.X = 10 ; para.Y = 60 ;
    GFX_Fill_Img_Adv( &para ) ;

    /* 只显示索引 3（原图蓝色），其余显示 Back_Color（灰） */
    para.Enable_Map = 0x00000008 ;       /* bit3=1，只显示索引 3 */
    para.X = 40 ; para.Y = 60 ;
    GFX_Fill_Img_Adv( &para ) ;

    /* 全显示参考 */
    para.Palette    = NULL ;
    para.Enable_Map = 0xFFFFFFFF ;
    para.Anchor     = Anchor_LT ;
    para.X = 80 ; para.Y = 60 ;
    GFX_Fill_Img_Adv( &para ) ;

    /* === 第 4 组：BITMAP 类型 + 自定义前/背景色测试（下半屏右侧）=== */
    /* 黑底白 X 图，前景白背景黑（区别于灰底背景） */
    para.Img        = &s_Bitmap_Img_MCU ;
    para.Palette    = NULL ;             /* BITMAP 自动用 Fore/Back 生成渐变色板 */
    para.Enable_Map = 0xFFFFFFFF ;
    para.Back_Color = 0x000000 ;         /* 背景黑（RGB888） */
    para.Fore_Color = 0xFFFFFF ;         /* 前景白（RGB888） */
    para.Anchor     = Anchor_LT ;
    para.X = 110 ; para.Y = 60 ;
    GFX_Fill_Img_Adv( &para ) ;

    /* BITMAP + Enable_Map：只显示索引 1（前景），其余显示 Back_Color（黑） */
    para.Enable_Map = 0x00000002 ;       /* BPP=1，bit1 越界无效，实际只有 bit0 */
    para.X = 130 ; para.Y = 60 ;
    GFX_Fill_Img_Adv( &para ) ;

    /* BITMAP + Enable_Map：只显示索引 0（背景） */
    para.Enable_Map = 0x00000001 ;
    para.X = 150 ; para.Y = 60 ;
    GFX_Fill_Img_Adv( &para ) ;

    GFX_Sim_Refresh() ;
}

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 测试场景：GFX_Buff_Img 缓冲区贴图（三种类型 + 裁剪） */
/* -------------------------------------------------------------------------------------------------------------------------- */

static void Test_Buff_Img( void )
{
    /* 黑底（屏幕） */
    GFX_Fill_Bg( GFX_Color_Convert( 0x000000 ) ) ;

    /* 填充渐变 MIRROR 数据 */
    GFX_Color_t * data = s_Mirror_Data ;
    for( uint16_t y = 0 ; y < MIRROR_H ; y++ )
    {
        for( uint16_t x = 0 ; x < MIRROR_W ; x++ )
        {
            uint8_t r = (uint8_t)( x * 255 / ( MIRROR_W - 1 ) ) ;
            uint8_t g = (uint8_t)( y * 255 / ( MIRROR_H - 1 ) ) ;
            uint8_t b = 128 ;
            data[ y * MIRROR_W + x ] = GFX_Color_Convert( ( r << 16 ) | ( g << 8 ) | b ) ;
        }
    }

    /* 建一个 80×60 的缓冲区（Area 起点设为 (0,0)，实际可改） */
    #define IMG_BUFF_W  80
    #define IMG_BUFF_H  60
    static GFX_Color_t buff[ IMG_BUFF_W * IMG_BUFF_H ] ;
    GFX_Buffer_t gbuff ;
    gbuff.Buffer = buff ;
    gbuff.Area.X = 0 ; gbuff.Area.Y = 0 ;
    gbuff.Area.W = IMG_BUFF_W ; gbuff.Area.H = IMG_BUFF_H ;

    /* 缓冲区清成深蓝底（便于区分屏幕黑底） */
    GFX_Buff_Bg( &gbuff , GFX_Color_Convert( 0x000080 ) ) ;

    /* 1. MIRROR 贴图到缓冲区左上角（完整 32×24） */
    GFX_Buff_Img( &gbuff , &s_Mirror_Img , 0 , 0 ) ;

    /* 2. MIRROR 贴图超出缓冲区右下角（测试裁剪：只贴左上角部分） */
    GFX_Buff_Img( &gbuff , &s_Mirror_Img , 60 , 45 ) ;

    /* 3. BITMAP 黑底白 X 贴到缓冲区中上（完整 12×12） */
    GFX_Buff_Img( &gbuff , &s_Bitmap_Img_MCU , 35 , 5 ) ;

    /* 4. PALETTE 四象限图贴到缓冲区中下（完整 12×12） */
    GFX_Buff_Img( &gbuff , &s_Palette_Img_MCU , 35 , 40 ) ;

    /* 5. BITMAP 贴图超出缓冲区左上角（负坐标，测试裁剪） */
    GFX_Buff_Img( &gbuff , &s_Bitmap_Img_MCU , -5 , -5 ) ;

    /* 将缓冲区 Flush 到屏幕 (80,100) 位置 */
    GFX_Flush( buff , 80 , 100 , IMG_BUFF_W , IMG_BUFF_H ) ;

    GFX_Sim_Refresh() ;
}

/* -------------------------------------------------------------------------------------------------------------------------- */
/* WinMain */
/* -------------------------------------------------------------------------------------------------------------------------- */

int WINAPI WinMain( HINSTANCE hInstance , HINSTANCE hPrev , LPSTR cmd , int show )
{
    (void)hPrev ; (void)cmd ; (void)show ;

    /* 1. 初始化模拟器窗口（注册窗口类） */
    GFX_Sim_Init( (void *)hInstance ) ;

    /* 2. 初始化 GFX */
    GFX_Init_t init = { .Buffer = s_Buffer , .Size = sizeof( s_Buffer ) } ;
    GFX_Init( &init ) ;

    /* 3. 绘制测试画面 */
    Test_Buff_Img() ;

    /* 4. 显示窗口 + 消息循环（阻塞直到关闭） */
    GFX_Sim_Run() ;

    return 0 ;
}

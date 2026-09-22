/**
 * @file GFX.h
 * @author my_GFX
 * @brief GFX 模块对外 API 头文件
 * @version 0.0.0.1
 * @date 2026-08-30
 * 
 * 
 */
#ifndef _GFX_H_
#define _GFX_H_

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 - 必须 模块所必须依赖的头文件 */
/* -------------------------------------------------------------------------------------------------------------------------- */
#include "GFX_Define.h"
#include "GFX_Port.h"
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 - 配置 通过宏定义之类的参数对模块进行配置的头文件 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 - debug 为调试而存在的头文件 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 私有宏定义 - save 为快速的跨文件变量访问存在的宏定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 宏定义 - 辅助 用于辅助设计代码的宏定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 宏定义 - 配置 通过修改宏定义参数从而实现对模块的配置的宏定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 类型定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */
typedef struct {
    /* 公共区域：MIRROR / BITMAP / BITMAP_WITH_PALETTE 均支持。 */
    const GFX_Img_t * Img ;       /* 原图描述符，裁剪不修改资源。 */
    int16_t X ;                  /* 显示器绝对坐标：所选 Anchor 的目标 X。 */
    int16_t Y ;                  /* 显示器绝对坐标：所选 Anchor 的目标 Y。 */
    Anchor_t Anchor ;            /* 整图或有效 Cut_Self 的九点锚点。 */
    bool Cut_Screen_Enable ;     /* 启用显示器坐标裁剪。 */
    bool Anchor_Use_Cut_Self ;    /* true=有效 Cut_Self 锚点；要求启用且存在有效 Cut_Self。 */
    Area_t Cut_Screen ;          /* 允许写入的显示器坐标矩形。 */
    bool Cut_Self_Enable ;       /* 启用原图局部坐标裁剪。 */
    Area_t Cut_Self ;            /* 原图左上角为 (0,0)，先与原图范围求交。 */

    /* 类型专用区域：下列字段仅对注释列出的类型生效。 */
    GFX_Color_t * Palette ;     /* 覆盖色板，至少 1<<BPP 项；支持 BITMAP / BITMAP_WITH_PALETTE。
                                  NULL：BITMAP 生成前后景渐变，PALETTE 读取图内色板。 */
    uint32_t Enable_Map ;        /* bit N=1 取色板，=0 取 Back_Color；支持 BITMAP / BITMAP_WITH_PALETTE。
                                  全显示必须设置 0xFFFFFFFFu，屏蔽不等于透明跳过。 */
    uint32_t Fore_Color ;        /* RGB888 渐变终点；仅支持 BITMAP 且 Palette=NULL。 */
    uint32_t Back_Color ;        /* RGB888：渐变起点（BITMAP），屏蔽索引替代色（BITMAP / BITMAP_WITH_PALETTE）。
                                  MIRROR 忽略；直接 Fill 不执行独立 Alpha 混合。 */
} GFX_Fill_Img_Adv_Para_t ;

typedef struct {
    const GFX_Buffer_t * Buff ;  /* 公共目标：所有图片类型，Area 使用显示器绝对坐标。 */
    /* 公共区域：MIRROR / BITMAP / BITMAP_WITH_PALETTE 均支持。 */
    const GFX_Img_t * Img ;       /* 原图描述符，裁剪不修改资源。 */
    int16_t X ;                  /* 显示器绝对坐标：所选 Anchor 的目标 X。 */
    int16_t Y ;                  /* 显示器绝对坐标：所选 Anchor 的目标 Y。 */
    Anchor_t Anchor ;            /* 整图或有效 Cut_Self 的九点锚点。 */
    bool Cut_Screen_Enable ;     /* 启用显示器坐标裁剪。 */
    bool Anchor_Use_Cut_Self ;    /* true=有效 Cut_Self 锚点；要求启用且存在有效 Cut_Self。 */
    Area_t Cut_Screen ;          /* 允许写入的显示器坐标矩形。 */
    bool Cut_Self_Enable ;       /* 启用原图局部坐标裁剪。 */
    Area_t Cut_Self ;            /* 原图左上角为 (0,0)，先与原图范围求交。 */

    /* 类型专用区域：下列字段仅对注释列出的类型生效。 */
    GFX_Color_t * Palette ;     /* 覆盖色板，至少 1<<BPP 项；支持 BITMAP / BITMAP_WITH_PALETTE。
                                  NULL：BITMAP 生成前后景渐变，PALETTE 读取图内色板。 */
    uint32_t Enable_Map ;        /* bit N=1 取色板，=0 取 Back_Color；支持 BITMAP / BITMAP_WITH_PALETTE。
                                  全显示必须设置 0xFFFFFFFFu，屏蔽不等于透明跳过。 */
    uint32_t Fore_Color ;        /* RGB888 渐变终点；仅支持 BITMAP 且 Palette=NULL。 */
    uint32_t Back_Color ;        /* RGB888：渐变起点（BITMAP），屏蔽索引替代色（两种 BITMAP）；
                                  固定混合背景（MIRROR / BITMAP_WITH_PALETTE 且启用下方开关和独立 Alpha）。 */
    bool Blend_Back_Color_Enable ; /* 独立 Alpha 背景：false=Buffer 原像素，true=Back_Color。
                                      支持 MIRROR / BITMAP_WITH_PALETTE；true 时 Alpha=0 也写 Back_Color。 */
} GFX_Buff_Img_Adv_Para_t ;
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 外部变量引用声明 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 函数声明 - 功能 对上层模块提供的功能API */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* ========================= 初始化与基础控制 ========================= */
void GFX_Init( GFX_Init_t * Config );

void GFX_Set_Buffer( void * Buffer , uint32_t Size );

void GFX_Set_Brightness( uint8_t Brightness );

void GFX_Set_Draw_Direction( Orth_Dir_t Direction );
Orth_Dir_t GFX_Get_Draw_Direction( void );

void GFX_Flush( GFX_Color_t * Buffer , int16_t X , int16_t Y , uint16_t W , uint16_t H );

int16_t GFX_Get_Width( void );
int16_t GFX_Get_Height( void );
int16_t GFX_Get_X_Max( void );
int16_t GFX_Get_Y_Max( void );

/* ========================= 缓冲区切割器 / 缓冲区管理器 ========================= */
bool GFX_Buffer_Init( GFX_Buffer_t * Buff , const Area_t * Area , void * Memory , uint32_t Memory_Size );

void GFX_Buffer_Cutter_Init( Area_t * Area_Src , uint32_t Buffer_Size );
bool GFX_Buffer_Cutter_Cut( Area_t *  Area_Buff );

void GFX_Buffer_Manager_Init( GFX_Buffer_Manager_t * Manager , void * Buffer , uint16_t Bytes_Per_Pix , uint32_t Max );
void GFX_Buffer_Manager_Clear( GFX_Buffer_Manager_t * Manager );
void * GFX_Buffer_Manager_Alloc( GFX_Buffer_Manager_t * Manager , uint32_t Size );

/* ========================= 颜色处理 ========================= */
GFX_Color_t GFX_Color_Convert( uint32_t RGB888 );
uint32_t GFX_Color_Transiton_RGB888( uint32_t RGB888_Active , uint32_t RGB888_Target , double Ratio  );
void GFX_Color_Get_RGB( GFX_Color_t color, uint8_t *r , uint8_t *g , uint8_t *b );
GFX_Color_t GFX_Color_AlphaBlend( GFX_Color_t bg , GFX_Color_t fg , uint8_t alpha, uint8_t max_alpha );
void GFX_Create_Gray_Palette( uint8_t bpp, GFX_Color_t * palette );
void GFX_Create_Color_Palette( uint8_t BPP, GFX_Color_t *Palette, uint32_t Front_Color, uint32_t Back_Color );

/* ========================= 图片数据访问 ========================= */
void GFX_Img_Copy_Palette( const GFX_Img_t * Img , GFX_Color_t * Dest ) ;

/* ========================= 直接绘图 API（直接调用底层驱动） ========================= */
void GFX_Fill_Bg( GFX_Color_t Color_Bg );
void GFX_Fill_Point( int16_t X , int16_t Y , GFX_Color_t Color );
void GFX_Fill_Img( const GFX_Img_t * Img , int16_t X , int16_t Y );
void GFX_Fill_Img_Adv( GFX_Fill_Img_Adv_Para_t * Para );
void GFX_Fill_Circle( GFX_Color_t Color , int16_t X , int16_t Y , uint16_t R , uint16_t Width );
void GFX_Fill_Rect( GFX_Color_t Color , int16_t X , int16_t Y , uint16_t W , uint16_t H );


/* ========================= 缓冲区绘图 API（先写入缓冲区，再通过 Flush 刷新） ========================= */
void GFX_Buff_Bg( const GFX_Buffer_t * Buff , GFX_Color_t Color );
/* Buff 系列坐标统一使用显示器绝对坐标，并自动裁剪到 Buff.Area。 */
void GFX_Buff_Img( const GFX_Buffer_t * Buff , const GFX_Img_t * Img , int16_t Image_X , int16_t Image_Y );
void GFX_Buff_Img_Adv( GFX_Buff_Img_Adv_Para_t * Para );
void GFX_Buff_Circle( const GFX_Buffer_t * Buff , int16_t X , int16_t Y , uint16_t R , GFX_Color_t Color );
void GFX_Buff_Rect( const GFX_Buffer_t * Buff , int16_t X , int16_t Y , uint16_t W , uint16_t H , GFX_Color_t Color );

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 函数声明 - 接口 对下层依赖所需的接口（已在 GFX_Port.h 中声明） */
/* -------------------------------------------------------------------------------------------------------------------------- */

#if GFX_TEST_ENABLE
    /* 测试入口：开启 GFX_TEST_ENABLE 后可调用 */
    void example_use_bitmap(void);

    #if GFX_PORT_TEST_ENABLE
        void GFX_Drive_Test( void );
    #endif
#endif

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 函数声明 - 回调 对下层的依赖而言，需被调用的回调函数 */
/* -------------------------------------------------------------------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif

/**
 * @file GFX.c
 * @author my_GFX
 * @brief GFX 模块核心功能实现
 * @version 0.0.0.1
 * @date 2026-08-30
 * 
 * 
 */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 - 必须 模块所必须依赖的头文件 */
/* -------------------------------------------------------------------------------------------------------------------------- */
#include "GFX.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 - 配置 通过宏定义之类的参数对模块进行配置的头文件 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 - debug 为调试而存在的头文件 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 - save 为快速的跨文件变量访问存在的头文件 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 私有类型定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */

// 缓存区切割器
typedef struct {
    Area_t Raw_Area ;      // 原始区域
    uint32_t Buffer_Size ; // 缓冲区大小 (pix)
    uint16_t Current_X ;   // 当前切割位置X
    uint16_t Current_Y ;   // 当前切割位置Y
    uint16_t Block_W ;     // 计算出的块宽度
    uint16_t Block_H ;     // 计算出的块高度
} GFX_Buffer_Cutter_t ;

// GFX 管理器
typedef struct {
    GFX_t * Current ;              // 当前活跃的GFX
    GFX_Buffer_Manager_t Manager ; // 缓冲区管理器（绘图函数内部使用的内存池）
    GFX_Buffer_Cutter_t Cutter ;   // 缓冲区切割器
    GFX_Color_t Palette[ ( 1 << GFX_BPP_MAX ) ] ; // 固定调色板缓冲（最多 32 色，绘制开始时载入）
} GFX_Manager_t ;

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 外部变量引用声明 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 文件作用域对象 */
/* -------------------------------------------------------------------------------------------------------------------------- */
GFX_Manager_t The_GFX ;

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 内部函数声明 */
/* -------------------------------------------------------------------------------------------------------------------------- */
void prv_GFX_Area_Calculate( GFX_t * GFX );
static bool prv_GFX_Is_Ready( void ) ;
static bool prv_GFX_Buffer_Is_Valid( const GFX_Buffer_t * Buff ) ;
static uint8_t * prv_GFX_Align_Color_Ptr( uint8_t * Ptr ) ;
static void prv_GFX_Fill_HLine_Clipped( GFX_Color_t Color , int32_t X0 , int32_t X1 , int32_t Y ) ;
static void prv_GFX_Buff_HLine_Clipped( const GFX_Buffer_t * Buff , GFX_Color_t Color ,
                                        int32_t X0 , int32_t X1 , int32_t Y ) ;

/* 按色彩类型拆分 Fill_Img 实现，避免 GFX_Fill_Img 主体过长 */
static void prv_Fill_Img_Mirror              ( const GFX_Img_t * Img , int16_t X , int16_t Y , const Area_t * Com ) ;
static void prv_Fill_Img_Bitmap              ( const GFX_Img_t * Img , int16_t X , int16_t Y , const Area_t * Com ) ;
static void prv_Fill_Img_Bitmap_With_Palette ( const GFX_Img_t * Img , int16_t X , int16_t Y , const Area_t * Com ) ;
static bool prv_Img_Adv_Get_Position( const GFX_Img_t * Img , Anchor_t Anchor , int16_t X , int16_t Y ,
                                      bool Anchor_Use_Cut_Self , bool Cut_Self_Enable ,
                                      const Area_t * Cut_Self , int16_t * Image_X , int16_t * Image_Y ) ;
static bool prv_Img_Adv_Get_Draw_Area( const GFX_Img_t * Img , int16_t Image_X , int16_t Image_Y ,
                                       const Area_t * Target_Area , bool Cut_Screen_Enable ,
                                       const Area_t * Cut_Screen , bool Cut_Self_Enable ,
                                       const Area_t * Cut_Self , Area_t * Draw_Area ) ;

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 函数定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* ========================= 内部辅助函数 ========================= */

/**
 * @brief 根据当前绘制方向计算屏幕有效区域
 * @param GFX GFX 设备对象
 */
void prv_GFX_Area_Calculate( GFX_t * GFX )
{
    if( GFX == NULL ) return ;
    GFX->Scr_Area.X = 0 ;
    GFX->Scr_Area.Y = 0 ;
    GFX->Scr_Area.W = ( Orth_Dir_0 == GFX->Draw_Direction || Orth_Dir_180 == GFX->Draw_Direction ) ? GFX->W : GFX->H ;
    GFX->Scr_Area.H = ( Orth_Dir_90 == GFX->Draw_Direction ||  Orth_Dir_270 == GFX->Draw_Direction ) ? GFX->W : GFX->H ;
}

/**
 * @brief 检查 GFX 是否已初始化且屏幕区域有效
 */
static bool prv_GFX_Is_Ready( void )
{
    return The_GFX.Current != NULL && Area_Is_Valid( &The_GFX.Current->Scr_Area ) ;
}

/**
 * @brief 校验缓冲区对象的合法性（非空、地址对齐、区域有效）
 */
static bool prv_GFX_Buffer_Is_Valid( const GFX_Buffer_t * Buff )
{
    return Buff != NULL && Buff->Buffer != NULL &&
           prv_GFX_Align_Color_Ptr( (uint8_t *)Buff->Buffer ) == (uint8_t *)Buff->Buffer &&
           Area_Is_Valid( &Buff->Area ) ;
}

/* 将任意字节地址向上对齐到 GFX_Color_t 的对齐边界。 */
static uint8_t * prv_GFX_Align_Color_Ptr( uint8_t * Ptr )
{
    const uintptr_t alignment = (uintptr_t)_Alignof( GFX_Color_t ) ;
    
    
    uintptr_t address = (uintptr_t)Ptr ;
    address = ( address + alignment - 1u ) & ~( alignment - 1u ) ;
    return (uint8_t *)address ;
}

/* 核心层裁剪水平线，保证 Port 层只收到屏幕范围内的坐标。 */
static void prv_GFX_Fill_HLine_Clipped( GFX_Color_t Color , int32_t X0 , int32_t X1 , int32_t Y )
{
    if( !prv_GFX_Is_Ready() || X0 > X1 ) return ;

    const Area_t * scr = &The_GFX.Current->Scr_Area ;
    int32_t left = scr->X ;
    int32_t right = (int32_t)scr->X + scr->W - 1 ;
    int32_t top = scr->Y ;
    int32_t bottom = (int32_t)scr->Y + scr->H - 1 ;

    if( Y < top || Y > bottom || X1 < left || X0 > right ) return ;
    if( X0 < left ) X0 = left ;
    if( X1 > right ) X1 = right ;

    GFX_Port_Fill( Color , (int16_t)X0 , (int16_t)Y , (uint16_t)( X1 - X0 + 1 ) , 1 ) ;
}

/* Buff 系列统一使用显示器绝对坐标，写入时再换算为缓冲区下标。 */
static void prv_GFX_Buff_HLine_Clipped( const GFX_Buffer_t * Buff , GFX_Color_t Color ,
                                        int32_t X0 , int32_t X1 , int32_t Y )
{
    if( !prv_GFX_Buffer_Is_Valid( Buff ) || X0 > X1 ) return ;

    int32_t left = Buff->Area.X ;
    int32_t right = (int32_t)Buff->Area.X + Buff->Area.W - 1 ;
    int32_t top = Buff->Area.Y ;
    int32_t bottom = (int32_t)Buff->Area.Y + Buff->Area.H - 1 ;

    if( Y < top || Y > bottom || X1 < left || X0 > right ) return ;
    if( X0 < left ) X0 = left ;
    if( X1 > right ) X1 = right ;

    uint32_t row = (uint32_t)( Y - Buff->Area.Y ) * Buff->Area.W ;
    uint32_t start = (uint32_t)( X0 - Buff->Area.X ) ;
    uint32_t end = (uint32_t)( X1 - Buff->Area.X ) ;
    for( uint32_t x = start ; x <= end ; x++ ) Buff->Buffer[ row + x ] = Color ;
}

/**
 * @brief 按整图或有效 Cut_Self 区域解释 Anchor，并返回整张图片的实际左上角。
 *
 * @note Anchor_Use_Cut_Self=true 时，先将 Cut_Self 裁剪到图片范围，再以该有效区域的
 *       九点锚点对齐 (X,Y)，最后反推原图左上角。没有启用或不存在有效 Cut_Self 时失败。
 */
static bool prv_Img_Adv_Get_Position( const GFX_Img_t * Img , Anchor_t Anchor , int16_t X , int16_t Y ,
                                      bool Anchor_Use_Cut_Self , bool Cut_Self_Enable ,
                                      const Area_t * Cut_Self , int16_t * Image_X , int16_t * Image_Y )
{
    if( Img == NULL || Image_X == NULL || Image_Y == NULL ) return false ;

    Area_t anchor_area = { 0 , 0 , (uint16_t)Img->W , (uint16_t)Img->H } ;
    int32_t source_x = 0 ;
    int32_t source_y = 0 ;

    if( Anchor_Use_Cut_Self )
    {
        if( !Cut_Self_Enable || Cut_Self == NULL ) return false ;

        Area_t self_bounds = { 0 , 0 , (uint16_t)Img->W , (uint16_t)Img->H } ;
        Area_t valid_self ;
        if( !Area_Com( &self_bounds , Cut_Self , &valid_self ) ) return false ;

        anchor_area.W = valid_self.W ;
        anchor_area.H = valid_self.H ;
        source_x = valid_self.X ;
        source_y = valid_self.Y ;
    }

    Area_Move( &anchor_area , Anchor , X , Y ) ;

    int32_t image_x = (int32_t)anchor_area.X - source_x ;
    int32_t image_y = (int32_t)anchor_area.Y - source_y ;
    if( image_x < INT16_MIN || image_x > INT16_MAX ||
        image_y < INT16_MIN || image_y > INT16_MAX ) return false ;

    *Image_X = (int16_t)image_x ;
    *Image_Y = (int16_t)image_y ;
    return true ;
}

/**
 * @brief 计算高级贴图最终可绘制区域。
 *
 * @note Cut_Screen 使用显示器绝对坐标；Cut_Self 使用图片左上角为原点的局部坐标。
 *       两个裁剪独立生效，同时使能时最终区域为两者与目标区域的交集。
 */
static bool prv_Img_Adv_Get_Draw_Area( const GFX_Img_t * Img , int16_t Image_X , int16_t Image_Y ,
                                       const Area_t * Target_Area , bool Cut_Screen_Enable ,
                                       const Area_t * Cut_Screen , bool Cut_Self_Enable ,
                                       const Area_t * Cut_Self , Area_t * Draw_Area )
{
    Area_t image_area = { Image_X , Image_Y , (uint16_t)Img->W , (uint16_t)Img->H } ;
    Area_t target_area = *Target_Area ;
    Area_t visible ;

    if( !Area_Com( &image_area , &target_area , &visible ) ) return false ;

    if( Cut_Screen_Enable )
    {
        Area_t cut_screen = *Cut_Screen ;
        Area_t clipped ;
        if( !Area_Com( &visible , &cut_screen , &clipped ) ) return false ;
        visible = clipped ;
    }

    if( Cut_Self_Enable )
    {
        Area_t self_bounds = { 0 , 0 , (uint16_t)Img->W , (uint16_t)Img->H } ;
        Area_t cut_self = *Cut_Self ;
        Area_t valid_self ;
        Area_t mapped_self ;
        Area_t clipped ;

        if( !Area_Com( &self_bounds , &cut_self , &valid_self ) ) return false ;

        mapped_self.X = Image_X + valid_self.X ;
        mapped_self.Y = Image_Y + valid_self.Y ;
        mapped_self.W = valid_self.W ;
        mapped_self.H = valid_self.H ;

        if( !Area_Com( &visible , &mapped_self , &clipped ) ) return false ;
        visible = clipped ;
    }

    *Draw_Area = visible ;
    return true ;
}

/* ========================= 初始化与基础控制 ========================= */

/**
 * @brief 初始化GFX
 * @param Init  初始化配置。传 NULL 则不初始化缓冲区管理器，后续可通过 GFX_Set_Buffer 设置。
 */
void GFX_Init( GFX_Init_t * Init )
{
    static GFX_t GFX_Obj ;

    memset( &The_GFX , 0 , sizeof( The_GFX ) ) ;
    memset( &GFX_Obj , 0 , sizeof( GFX_Obj ) ) ;
    The_GFX.Current = &GFX_Obj ;

    GFX_Port_Init( The_GFX.Current );
    prv_GFX_Area_Calculate( The_GFX.Current );

    if( Init != NULL )
    {
        GFX_Set_Buffer( Init->Buffer , Init->Size );
    }
}

/**
 * @brief 设置/切换内部缓冲区
 * @param Buffer  缓冲区首地址
 * @param Size    缓冲区大小（字节）
 * @note  每次绘图函数调用周期内，缓冲区可任意使用；函数开始时会 Clear 一次。
 *        若 UI 层有闲置缓冲区，可随时调用此函数切换为 UI 的缓冲区以节省内存。
 */
void GFX_Set_Buffer( void * Buffer , uint32_t Size )
{
    if( Buffer == NULL || Size < sizeof( GFX_Color_t ) )
    {
        GFX_Buffer_Manager_Init( &The_GFX.Manager , NULL , sizeof( GFX_Color_t ) , 0 ) ;
        return ;
    }

    uint8_t * raw = (uint8_t *)Buffer ;
    uint8_t * aligned = prv_GFX_Align_Color_Ptr( raw ) ;
    uint32_t padding = (uint32_t)( aligned - raw ) ;
    if( padding > Size || Size - padding < sizeof( GFX_Color_t ) )
    {
        GFX_Buffer_Manager_Init( &The_GFX.Manager , NULL , sizeof( GFX_Color_t ) , 0 ) ;
        return ;
    }

    GFX_Buffer_Manager_Init( &The_GFX.Manager , aligned , sizeof( GFX_Color_t ) ,
                             ( Size - padding ) / sizeof( GFX_Color_t ) );
}

/**
 * @brief 使用一段原始内存构建绘图缓冲区，并保证像素首地址满足 GFX_Color_t 对齐要求。
 * @param Buff        输出缓冲区对象；失败时被清空。
 * @param Area        缓冲区对应的显示器绝对坐标区域。
 * @param Memory      原始内存首地址，可以未对齐。
 * @param Memory_Size 原始内存总字节数；对齐填充也计入其中。
 * @return true 构建成功；false 参数、区域或容量无效。
 */
bool GFX_Buffer_Init( GFX_Buffer_t * Buff , const Area_t * Area , void * Memory , uint32_t Memory_Size )
{
    if( Buff == NULL ) return false ;

    memset( Buff , 0 , sizeof( *Buff ) ) ;
    if( Area == NULL || Memory == NULL || !Area_Is_Valid( Area ) ) return false ;

    uint8_t * raw = (uint8_t *)Memory ;
    uint8_t * aligned = prv_GFX_Align_Color_Ptr( raw ) ;
    uint32_t padding = (uint32_t)( aligned - raw ) ;
    if( padding > Memory_Size ) return false ;

    uint32_t available_bytes = Memory_Size - padding ;
    uint32_t pixel_count = (uint32_t)Area->W * Area->H ;
    if( pixel_count > available_bytes / sizeof( GFX_Color_t ) ) return false ;

    Buff->Area = *Area ;
    Buff->Buffer = (GFX_Color_t *)aligned ;
    return true ;
}

/**
 * @brief 设置当前GFX的亮度
 * 
 * @param Brightness     亮度值 （ 0 - 255 ）
 */
void GFX_Set_Brightness( uint8_t Brightness )
{
    if( The_GFX.Current == NULL )
        return ;

    if( The_GFX.Current->Brightness == Brightness )
        return ;

    The_GFX.Current->Brightness = Brightness ;
    GFX_Port_Set_Brightness( Brightness );
}

/**
 * @brief 设置当前屏幕的绘制方向
 * 
 * @param Direction      绘制方向
 */
void GFX_Set_Draw_Direction( Orth_Dir_t Direction )
{
    if( The_GFX.Current == NULL )
        return ;

    if( The_GFX.Current->Draw_Direction == Direction )
        return ;

    The_GFX.Current->Draw_Direction = Direction ;
    GFX_Port_Set_Draw_Direction( Direction );
    prv_GFX_Area_Calculate( The_GFX.Current );
}

/**
 * @brief 获取当前屏幕的绘制方向
 */
Orth_Dir_t GFX_Get_Draw_Direction( void )
{
    if( The_GFX.Current == NULL )
        return Orth_Dir_0 ;
    return The_GFX.Current->Draw_Direction ;
}

/**
 * @brief 将缓冲区的内容显示到当前屏幕上
 */
void GFX_Flush( GFX_Color_t * Buffer , int16_t X , int16_t Y , uint16_t W , uint16_t H )
{
    if( !prv_GFX_Is_Ready() || Buffer == NULL || W == 0 || H == 0 ) return ;

    GFX_Port_Flush( Buffer , X , Y , W , H );
}

int16_t GFX_Get_Width( void )
{
    if( The_GFX.Current == NULL ) return 0 ;
    return (int16_t)The_GFX.Current->Scr_Area.W ;
}

int16_t GFX_Get_Height( void )
{
    if( The_GFX.Current == NULL ) return 0 ;
    return (int16_t)The_GFX.Current->Scr_Area.H ;
}

int16_t GFX_Get_X_Max( void )
{
    return GFX_Get_Width() - 1 ;
}

int16_t GFX_Get_Y_Max( void )
{
    return GFX_Get_Height() - 1 ;
}

/* ========================= 缓冲区切割器 / 缓冲区管理器 ========================= */

void GFX_Buffer_Cutter_Init( Area_t * Area_Src , uint32_t Buffer_Size )
{
    GFX_Buffer_Cutter_t * Cutter = &The_GFX.Cutter ;

    memset( Cutter , 0 , sizeof( *Cutter ) ) ;

    if( Area_Src == NULL || !Area_Is_Valid( Area_Src ) || Buffer_Size == 0 )
        return ;

    Cutter->Raw_Area = *Area_Src ;
    Cutter->Buffer_Size = Buffer_Size ;
    Cutter->Current_X = 0 ;
    Cutter->Current_Y = 0 ;

    // 计算块大小：以 Buffer_Size 为像素容量上限
    // 优先按整行划分
    if( Area_Src->W <= Buffer_Size )
    {
        Cutter->Block_W = Area_Src->W ;
        Cutter->Block_H = (uint16_t)( Buffer_Size / Area_Src->W ) ;
        if( Cutter->Block_H == 0 ) Cutter->Block_H = 1 ;
        if( Cutter->Block_H > Area_Src->H ) Cutter->Block_H = Area_Src->H ;
    }
    else
    {
        // 缓冲区连一行都放不下，按块宽切
        Cutter->Block_W = (uint16_t)Buffer_Size ;
        Cutter->Block_H = 1 ;
    }
}

bool GFX_Buffer_Cutter_Cut( Area_t * Area_Buff )
{
    GFX_Buffer_Cutter_t * Cutter = &The_GFX.Cutter ;

    if( Area_Buff == NULL || Cutter->Block_W == 0 || Cutter->Block_H == 0 )
        return false ;

    if( Cutter->Current_Y >= Cutter->Raw_Area.H )
        return false ;

    uint16_t x_start = Cutter->Current_X ;
    uint16_t y_start = Cutter->Current_Y ;
    uint16_t w = Cutter->Block_W ;
    uint16_t h = Cutter->Block_H ;

    if( x_start + w > Cutter->Raw_Area.W )
        w = (uint16_t)( Cutter->Raw_Area.W - x_start ) ;
    if( y_start + h > Cutter->Raw_Area.H )
        h = (uint16_t)( Cutter->Raw_Area.H - y_start ) ;

    Area_Buff->X = Cutter->Raw_Area.X + x_start ;
    Area_Buff->Y = Cutter->Raw_Area.Y + y_start ;
    Area_Buff->W = w ;
    Area_Buff->H = h ;

    // 前进到下一块
    Cutter->Current_X += Cutter->Block_W ;
    if( Cutter->Current_X >= Cutter->Raw_Area.W )
    {
        Cutter->Current_X = 0 ;
        Cutter->Current_Y += Cutter->Block_H ;
    }
    return true ;
}

void GFX_Buffer_Manager_Init( GFX_Buffer_Manager_t * Manager , void * Buffer , uint16_t Bytes_Per_Pix , uint32_t Max )
{
    if( NULL == Manager )
        return ;

    Manager->Buffer = Buffer ;
    Manager->Bytes_Per_Pix = Bytes_Per_Pix ;
    Manager->Max = Max ;
    Manager->Index = 0 ;
}

void GFX_Buffer_Manager_Clear( GFX_Buffer_Manager_t * Manager )
{
    if( NULL == Manager )
        return ;

    Manager->Index = 0 ;
}

void * GFX_Buffer_Manager_Alloc( GFX_Buffer_Manager_t * Manager , uint32_t Size_Of_Pix )
{
    if( NULL == Manager )
        return NULL ;

    if( NULL == Manager->Buffer )
        return NULL ;

    if( Manager->Bytes_Per_Pix == 0 || Manager->Index > Manager->Max ||
        Size_Of_Pix > Manager->Max - Manager->Index )
        return NULL ;

    uint8_t * Temp = (( uint8_t * )Manager->Buffer ) +
                     (uint32_t)Manager->Index * Manager->Bytes_Per_Pix ;

    Manager->Index += Size_Of_Pix ;

    return ( void * ) Temp ;
}

/* ========================= 颜色处理 ========================= */

/**
 * @brief 将 RGB888 转换为当前配置的 GFX_Color_t (RGB565 / RGB888 / RGB332)
 */
GFX_Color_t GFX_Color_Convert( uint32_t RGB888 )
{
    uint8_t r = (uint8_t)( ( RGB888 >> 16 ) & 0xFF ) ;
    uint8_t g = (uint8_t)( ( RGB888 >> 8  ) & 0xFF ) ;
    uint8_t b = (uint8_t)(   RGB888         & 0xFF ) ;

#if ( GFX_COLOR_TYPE == GFX_COLOR_TYPE_RGB888 )
    return RGB888 ;
#elif ( GFX_COLOR_TYPE == GFX_COLOR_TYPE_RGB565 )
    return ( GFX_Color_t )( ( ( r & 0xF8 ) << 8 ) | ( ( g & 0xFC ) << 3 ) | ( b >> 3 ) ) ;
#elif ( GFX_COLOR_TYPE == GFX_COLOR_TYPE_RGB332 )
    return ( GFX_Color_t )( ( ( r & 0xE0 ) ) | ( ( g & 0xE0 ) >> 3 ) | ( b >> 6 ) ) ;
#endif
}

/**
 * @brief 在两个 RGB888 颜色之间按比例插值
 */
uint32_t GFX_Color_Transiton_RGB888( uint32_t RGB888_Active , uint32_t RGB888_Target , double Ratio )
{
    if( Ratio <= 0.0 ) return RGB888_Active ;
    if( Ratio >= 1.0 ) return RGB888_Target ;

    uint8_t ar = (uint8_t)( ( RGB888_Active >> 16 ) & 0xFF ) ;
    uint8_t ag = (uint8_t)( ( RGB888_Active >> 8  ) & 0xFF ) ;
    uint8_t ab = (uint8_t)(   RGB888_Active         & 0xFF ) ;

    uint8_t tr = (uint8_t)( ( RGB888_Target >> 16 ) & 0xFF ) ;
    uint8_t tg = (uint8_t)( ( RGB888_Target >> 8  ) & 0xFF ) ;
    uint8_t tb = (uint8_t)(   RGB888_Target         & 0xFF ) ;

    uint8_t r = (uint8_t)( ar + (int16_t)( ( tr - ar ) * Ratio ) ) ;
    uint8_t g = (uint8_t)( ag + (int16_t)( ( tg - ag ) * Ratio ) ) ;
    uint8_t b = (uint8_t)( ab + (int16_t)( ( tb - ab ) * Ratio ) ) ;

    return ( (uint32_t)r << 16 ) | ( (uint32_t)g << 8 ) | b ;
}

/**
 * @brief 从 GFX_Color_t 中拆分出 R G B 分量（归一化到 0-255）
 */
void GFX_Color_Get_RGB( GFX_Color_t color, uint8_t *r , uint8_t *g , uint8_t *b )
{
    if( r == NULL || g == NULL || b == NULL ) return ;

#if ( GFX_COLOR_TYPE == GFX_COLOR_TYPE_RGB888 )
    *r = (uint8_t)( ( color >> 16 ) & 0xFF ) ;
    *g = (uint8_t)( ( color >> 8  ) & 0xFF ) ;
    *b = (uint8_t)(   color         & 0xFF ) ;
#elif ( GFX_COLOR_TYPE == GFX_COLOR_TYPE_RGB565 )
    uint8_t r5 = (uint8_t)( ( color >> 11 ) & 0x1F ) ;
    uint8_t g6 = (uint8_t)( ( color >> 5  ) & 0x3F ) ;
    uint8_t b5 = (uint8_t)(   color         & 0x1F ) ;
    *r = (uint8_t)( ( r5 << 3 ) | ( r5 >> 2 ) ) ;     // 5->8
    *g = (uint8_t)( ( g6 << 2 ) | ( g6 >> 4 ) ) ;     // 6->8
    *b = (uint8_t)( ( b5 << 3 ) | ( b5 >> 2 ) ) ;
#elif ( GFX_COLOR_TYPE == GFX_COLOR_TYPE_RGB332 )
    uint8_t r3 = (uint8_t)( ( color >> 5 ) & 0x07 ) ;
    uint8_t g3 = (uint8_t)( ( color >> 2 ) & 0x07 ) ;
    uint8_t b2 = (uint8_t)(   color       & 0x03 ) ;
    *r = (uint8_t)( ( r3 << 5 ) | ( r3 << 2 ) | ( r3 >> 1 ) ) ;
    *g = (uint8_t)( ( g3 << 5 ) | ( g3 << 2 ) | ( g3 >> 1 ) ) ;
    *b = (uint8_t)( ( b2 << 6 ) | ( b2 << 4 ) | ( b2 << 2 ) | b2 ) ;
#endif
}

/**
 * @brief Alpha 混合：结果 = (fg * alpha + bg * (max_alpha - alpha)) / max_alpha
 */
GFX_Color_t GFX_Color_AlphaBlend( GFX_Color_t bg , GFX_Color_t fg , uint8_t alpha, uint8_t max_alpha )
{
    if( max_alpha == 0 ) return bg ;
    if( alpha >= max_alpha ) return fg ;
    if( alpha == 0 ) return bg ;

    uint8_t br, bg_c, bb ;
    uint8_t fr, fg_c, fb ;
    GFX_Color_Get_RGB( bg, &br, &bg_c, &bb );
    GFX_Color_Get_RGB( fg, &fr, &fg_c, &fb );

    uint16_t a = alpha ;
    uint16_t ia = (uint16_t)( max_alpha - alpha ) ;

    uint8_t r = (uint8_t)( ( fr * a + br * ia ) / max_alpha ) ;
    uint8_t g = (uint8_t)( ( fg_c * a + bg_c * ia ) / max_alpha ) ;
    uint8_t b = (uint8_t)( ( fb * a + bb * ia ) / max_alpha ) ;

    return GFX_Color_Convert( ( (uint32_t)r << 16 ) | ( (uint32_t)g << 8 ) | b ) ;
}

/**
 * @brief 创建灰度调色板
 * 
 * @param bpp       每个像素的位数（1~5）
 * @param palette   输出调色板（长度至少 2^bpp）
 */
void GFX_Create_Gray_Palette( uint8_t bpp, GFX_Color_t * palette )
{
    if( palette == NULL ) return ;
    if( bpp < GFX_BPP_MIN ) bpp = GFX_BPP_MIN ;
    if( bpp > GFX_BPP_MAX ) bpp = GFX_BPP_MAX ;

    uint16_t count = (uint16_t)( 1 << bpp ) ;
    uint16_t max_val = count - 1 ;

    for( uint16_t i = 0 ; i < count ; i++ )
    {
        /* 纯黑(0) → 纯白(255) 线性渐变 */
        uint8_t gray = (uint8_t)( ( i * 255 ) / max_val ) ;
        uint32_t rgb888 = ( (uint32_t)gray << 16 ) | ( (uint32_t)gray << 8 ) | gray ;
        palette[ i ] = GFX_Color_Convert( rgb888 ) ;
    }
}

/**
 * @brief 创建前后双色过渡调色板
 * 
 * @param BPP           每像素位数
 * @param Palette       输出
 * @param Front_Color   前景 RGB888
 * @param Back_Color    背景 RGB888
 */
void GFX_Create_Color_Palette( uint8_t BPP, GFX_Color_t *Palette, uint32_t Front_Color, uint32_t Back_Color )
{
    if( Palette == NULL ) return ;
    if( BPP < GFX_BPP_MIN ) BPP = GFX_BPP_MIN ;
    if( BPP > GFX_BPP_MAX ) BPP = GFX_BPP_MAX ;

    uint16_t count = (uint16_t)( 1 << BPP ) ;

    // 约定：索引 0 = Back_Color, 索引 max = Front_Color；与 Bitmap 的一般用法一致
    for( uint16_t i = 0 ; i < count ; i++ )
    {
        double ratio = (double)i / (double)( count - 1 ) ;
        uint32_t c = GFX_Color_Transiton_RGB888( Back_Color, Front_Color, ratio );
        Palette[ i ] = GFX_Color_Convert( c ) ;
    }
}

/* ========================= 直接绘图 API（直接调用底层驱动） ========================= */

/**
 * @brief 整块GFX填充背景颜色
 */
void GFX_Fill_Bg( GFX_Color_t Color_Bg )
{
    if( !prv_GFX_Is_Ready() ) return ;
    Area_t * scr = &The_GFX.Current->Scr_Area ;
    GFX_Port_Fill( Color_Bg , scr->X , scr->Y , scr->W , scr->H ) ;
}

/**
 * @brief 在指定位置填充点
 */
void GFX_Fill_Point( int16_t X , int16_t Y , GFX_Color_t Color )
{
    if( !prv_GFX_Is_Ready() ) return ;
    Area_t * scr = &The_GFX.Current->Scr_Area ;

    /* 越界检查 */
    if( X < scr->X || X >= scr->X + scr->W || Y < scr->Y || Y >= scr->Y + scr->H )
        return ;

    GFX_Port_Fill( Color , X , Y , 1 , 1 ) ;
}

/**
 * @brief MIRROR 类型图片填充实现（从 GFX_Fill_Img 拆出）
 *
 * @param Img  图片描述子
 * @param X    屏幕目标左上角 X（原始传入值，用于计算图片内偏移）
 * @param Y    屏幕目标左上角 Y
 * @param Com  已求好的图片与屏幕交集区域（绝对屏幕坐标）
 *
 * @note MIRROR 数据连续存储，无裁剪时整块一次刷新即可；有裁剪时逐行搬运可见行。
 */
static void prv_Fill_Img_Mirror( const GFX_Img_t * Img , int16_t X , int16_t Y , const Area_t * Com )
{
#if GFX_RESOURCE_ALIGNMENT_CHECK_ENABLE
    if( Img->Color_Save_Way == GFX_Save_Way_MCU &&
        prv_GFX_Align_Color_Ptr( Img->Color_Save_Info.C_Array ) != Img->Color_Save_Info.C_Array ) return ;
#endif

    uint8_t  bpp_bytes    = sizeof( GFX_Color_t ) ;
    uint16_t bytes_per_row = (uint16_t)Img->W * bpp_bytes ;
    bool     no_clip       = ( Com->X == X && Com->Y == Y && Com->W == Img->W && Com->H == Img->H ) ;

    if( Img->Color_Save_Way == GFX_Save_Way_MCU )
    {
        const uint8_t * src = Img->Color_Save_Info.C_Array ;

        if( no_clip )
        {
            /* 无裁剪：整张图数据连续，一次 Flush 完成 */
            GFX_Port_Flush( (GFX_Color_t *)src , X , Y , Img->W , Img->H ) ;
        }
        else
        {
            /* 有裁剪：逐行搬运可见行 */
            int16_t off_x = Com->X - X ;
            int16_t off_y = Com->Y - Y ;
            for( uint16_t i = 0 ; i < Com->H ; i++ )
            {
                const GFX_Color_t * src_row = (const GFX_Color_t *)(
                    src + (uint32_t)( off_y + i ) * bytes_per_row + off_x * bpp_bytes ) ;
                GFX_Port_Flush( (GFX_Color_t *)src_row , Com->X , Com->Y + i , Com->W , 1 ) ;
            }
        }
    }
    else    /* GFX_Save_Way_Flash */
    {
        uint32_t base_addr = Img->Color_Save_Info.Flash_Addr ;

        if( no_clip )
        {
            /* 无裁剪：整张图数据连续，一次搬运完成 */
            GFX_Port_Flash_To_GFX( base_addr , X , Y , Img->W , Img->H ) ;
        }
        else
        {
            /* 有裁剪：逐行搬运可见行 */
            int16_t off_x = Com->X - X ;
            int16_t off_y = Com->Y - Y ;
            for( uint16_t i = 0 ; i < Com->H ; i++ )
            {
                uint32_t row_addr = base_addr + (uint32_t)( off_y + i ) * bytes_per_row + off_x * bpp_bytes ;
                GFX_Port_Flash_To_GFX( row_addr , Com->X , Com->Y + i , Com->W , 1 ) ;
            }
        }
    }
}

/**
 * @brief BITMAP_WITH_PALETTE 类型图片填充实现（从 GFX_Fill_Img 拆出）
 *
 * @param Img  图片描述子
 * @param X    屏幕目标左上角 X（原始传入值，用于计算图片内偏移）
 * @param Y    屏幕目标左上角 Y
 * @param Com  已求好的图片与屏幕交集区域（绝对屏幕坐标）
 *
 * @note 内存布局（共用 GFX_Buffer_Manager 的缓冲区）：
 *       [位图读取缓冲(1行)][显示缓冲(Max_Rows行)]
 *       - 位图缓冲只留一行，逐行读取解析后累加到显示缓冲
 *       - 显示缓冲装满 Max_Rows 行后 Flush，循环直至整图完成
 *       - 调色板在 GFX_Manager.Palette 固定数组中（不占 buffer）
 *       - 调色板由调用方负责载入（BITMAP 用灰度默认色板，PALETTE 从图片数据复制）
 */

/* BITMAP / BITMAP_WITH_PALETTE 共用的位图绘制核心
 * @param Palette    调色板首地址（调用方负责载入）
 * @param Enable_Map 显隐掩码，bit N=1 显示索引 N，=0 显示 Back_Color；0xFFFFFFFF 表示全显示（整行 Flush 优化）
 * @param Back_Color Enable_Map 屏蔽像素的填充色 */
static void prv_Fill_Img_Bitmap_Core( const GFX_Img_t * Img , int16_t X , int16_t Y , const Area_t * Com ,
                                      const GFX_Color_t * Palette , uint32_t Enable_Map , GFX_Color_t Back_Color )
{
    uint8_t  bpp          = (uint8_t)Img->Color_Bits_Per_Pix ;
    int16_t  off_x        = Com->X - X ;
    int16_t  off_y        = Com->Y - Y ;

    /* 算参数：bit 偏移与每行读取字节数 */
    uint32_t start_bit          = (uint32_t)off_x * bpp ;
    uint8_t  bit_offset         = (uint8_t)( start_bit % 8 ) ;
    uint16_t read_bytes_per_row = (uint16_t)( ( bit_offset + Com->W * bpp + 7 ) / 8 ) ;

    /* buffer 指针划分：[位图读取缓冲(1行)][对齐填充][显示缓冲] */
    uint16_t disp_bytes_per_row = (uint16_t)Com->W * (uint16_t)sizeof( GFX_Color_t ) ;
    uint32_t buffer_bytes       = The_GFX.Manager.Max * The_GFX.Manager.Bytes_Per_Pix ;
    uint8_t      * bmp_buf  = (uint8_t *)The_GFX.Manager.Buffer ;
    if( bmp_buf == NULL ) return ;

    uint8_t * disp_bytes = prv_GFX_Align_Color_Ptr( bmp_buf + read_bytes_per_row ) ;
    uint32_t prefix_bytes = (uint32_t)( disp_bytes - bmp_buf ) ;

    /* 预检查：位流、对齐填充和一行显示像素必须全部装得下。 */
    if( prefix_bytes > buffer_bytes ||
        (uint32_t)disp_bytes_per_row > buffer_bytes - prefix_bytes ) return ;

    GFX_Color_t * disp_buf = (GFX_Color_t *)disp_bytes ;

    /* 原图每行字节数（用于 Flash/MCU 寻址） */
    uint16_t raw_bytes_per_row = (uint16_t)( ( Img->W * bpp + 7 ) / 8 ) ;
    uint32_t start_byte = start_bit / 8 ;

    /* 全显示模式：整行解析 + 整批 Flush（优化路径） */
    if( Enable_Map == 0xFFFFFFFF )
    {
        uint16_t max_rows = (uint16_t)( ( buffer_bytes - prefix_bytes ) / disp_bytes_per_row ) ;
        if( max_rows < 1 ) return ;
        if( max_rows > Com->H ) max_rows = Com->H ;

        uint16_t cur_row = 0 ;
        while( cur_row < Com->H )
        {
            uint16_t batch_rows = ( ( Com->H - cur_row ) < max_rows ) ? ( Com->H - cur_row ) : max_rows ;

            for( uint16_t i = 0 ; i < batch_rows ; i++ )
            {
                uint16_t src_row = off_y + cur_row + i ;

                if( Img->Color_Save_Way == GFX_Save_Way_MCU )
                {
                    const uint8_t * src = Img->Color_Save_Info.C_Array ;
                    uint32_t row_start_byte = (uint32_t)src_row * raw_bytes_per_row + start_byte ;
                    memcpy( bmp_buf , src + row_start_byte , read_bytes_per_row ) ;
                }
                else
                {
                    uint32_t base_addr = Img->Color_Save_Info.Flash_Addr ;
                    uint32_t row_addr = base_addr + (uint32_t)src_row * raw_bytes_per_row + start_byte ;
                    GFX_Port_Flash_Read( row_addr , bmp_buf , read_bytes_per_row ) ;
                }

                GFX_Color_t * disp_row = disp_buf + (uint32_t)i * Com->W ;
                uint16_t bit_pos = bit_offset ;
                for( uint16_t p = 0 ; p < Com->W ; p++ )
                {
                    uint16_t val = 0 ;
                    for( uint16_t b = 0 ; b < bpp ; b++ )
                    {
                        uint16_t cur = bit_pos + b ;
                        uint8_t  bit = ( bmp_buf[ cur / 8 ] >> ( cur % 8 ) ) & 0x01 ;
                        val |= (uint16_t)( bit << b ) ;
                    }
                    disp_row[ p ] = Palette[ val ] ;
                    bit_pos += bpp ;
                }
            }

            GFX_Port_Flush( disp_buf , Com->X , Com->Y + cur_row , Com->W , batch_rows ) ;
            cur_row += batch_rows ;
        }
    }
    else
    {
        /* 掩码模式：Enable_Map 屏蔽的索引显示 Palette[0] 颜色（背景色填充），整行解析 + 整批 Flush */
        uint16_t max_rows = (uint16_t)( ( buffer_bytes - prefix_bytes ) / disp_bytes_per_row ) ;
        if( max_rows < 1 ) return ;
        if( max_rows > Com->H ) max_rows = Com->H ;

        uint16_t cur_row = 0 ;
        while( cur_row < Com->H )
        {
            uint16_t batch_rows = ( ( Com->H - cur_row ) < max_rows ) ? ( Com->H - cur_row ) : max_rows ;

            for( uint16_t i = 0 ; i < batch_rows ; i++ )
            {
                uint16_t src_row = off_y + cur_row + i ;

                if( Img->Color_Save_Way == GFX_Save_Way_MCU )
                {
                    const uint8_t * src = Img->Color_Save_Info.C_Array ;
                    uint32_t row_start_byte = (uint32_t)src_row * raw_bytes_per_row + start_byte ;
                    memcpy( bmp_buf , src + row_start_byte , read_bytes_per_row ) ;
                }
                else
                {
                    uint32_t base_addr = Img->Color_Save_Info.Flash_Addr ;
                    uint32_t row_addr = base_addr + (uint32_t)src_row * raw_bytes_per_row + start_byte ;
                    GFX_Port_Flash_Read( row_addr , bmp_buf , read_bytes_per_row ) ;
                }

                GFX_Color_t * disp_row = disp_buf + (uint32_t)i * Com->W ;
                uint16_t bit_pos = bit_offset ;
                for( uint16_t p = 0 ; p < Com->W ; p++ )
                {
                    uint16_t val = 0 ;
                    for( uint16_t b = 0 ; b < bpp ; b++ )
                    {
                        uint16_t cur = bit_pos + b ;
                        uint8_t  bit = ( bmp_buf[ cur / 8 ] >> ( cur % 8 ) ) & 0x01 ;
                        val |= (uint16_t)( bit << b ) ;
                    }
                    /* 被屏蔽的索引显示 Back_Color 背景色，其余显示对应调色板颜色 */
                    disp_row[ p ] = ( ( Enable_Map >> val ) & 1u ) ? Palette[ val ] : Back_Color ;
                    bit_pos += bpp ;
                }
            }

            GFX_Port_Flush( disp_buf , Com->X , Com->Y + cur_row , Com->W , batch_rows ) ;
            cur_row += batch_rows ;
        }
    }
}

/**
 * @brief BITMAP 类型绘制：位图无调色板，使用默认灰度调色板（0=黑→max=白）
 */
static void prv_Fill_Img_Bitmap( const GFX_Img_t * Img , int16_t X , int16_t Y , const Area_t * Com )
{
    GFX_Create_Gray_Palette( (uint8_t)Img->Color_Bits_Per_Pix , The_GFX.Palette ) ;
    prv_Fill_Img_Bitmap_Core( Img , X , Y , Com , The_GFX.Palette , 0xFFFFFFFF , The_GFX.Palette[0] ) ;
}

/**
 * @brief BITMAP_WITH_PALETTE 类型绘制：位图 + 外部调色板
 */
static void prv_Fill_Img_Bitmap_With_Palette( const GFX_Img_t * Img , int16_t X , int16_t Y , const Area_t * Com )
{
    GFX_Img_Copy_Palette( Img , The_GFX.Palette ) ;
    prv_Fill_Img_Bitmap_Core( Img , X , Y , Com , The_GFX.Palette , 0xFFFFFFFF , The_GFX.Palette[0] ) ;
}

/**
 * @brief 在指定位置填充图片（简单版：左上角对齐，默认配色）
 *
 * @param Img  图片描述子
 * @param X    屏幕目标左上角 X
 * @param Y    屏幕目标左上角 Y
 *
 * @note MIRROR 由 Port 搬运原生像素；两种 BITMAP 解码后输出。直接 Fill 不混合独立 Alpha。
 */
void GFX_Fill_Img( const GFX_Img_t * Img , int16_t X , int16_t Y )
{
    if( !prv_GFX_Is_Ready() ) return ;
    if( !Img || Img->W == 0 || Img->H == 0 || Img->Color_Type == GFX_COLOR_TYPE_NONE ) return ;
    if( Img->Color_Save_Way != GFX_Save_Way_MCU && Img->Color_Save_Way != GFX_Save_Way_Flash ) return ;
    if( Img->Color_Save_Way == GFX_Save_Way_MCU && Img->Color_Save_Info.C_Array == NULL ) return ;
    if( Img->Color_Type == GFX_COLOR_TYPE_BITMAP && Img->Alpha_Enable ) return ;
    if( Img->Color_Type != GFX_COLOR_TYPE_MIRROR &&
        ( Img->Color_Bits_Per_Pix < GFX_BPP_MIN || Img->Color_Bits_Per_Pix > GFX_BPP_MAX ) ) return ;

    Area_t * scr = &The_GFX.Current->Scr_Area ;
    Area_t img_area = { X , Y , (uint16_t)Img->W , (uint16_t)Img->H } ;
    Area_t com ;

    /* 求图片与屏幕的交集 */
    if( !Area_Com( &img_area , scr , &com ) )
        return ;

    switch( Img->Color_Type )
    {
        case GFX_COLOR_TYPE_MIRROR :
            prv_Fill_Img_Mirror( Img , X , Y , &com ) ;
            break ;

        case GFX_COLOR_TYPE_BITMAP :
            prv_Fill_Img_Bitmap( Img , X , Y , &com ) ;
            break ;

        case GFX_COLOR_TYPE_BITMAP_WITH_PALETTE :
            prv_Fill_Img_Bitmap_With_Palette( Img , X , Y , &com ) ;
            break ;
    }
}

/**
 * @brief 在指定位置绘制圆圈
 * 
 * @param Color      颜色
 * @param X          中心X
 * @param Y          中心Y
 * @param R          半径
 * @param Width      线宽（0 表示实心圆）
 *
 * @note Width 为 0 时绘制实心圆，否则绘制指定线宽的圆环。
 */
void GFX_Fill_Circle( GFX_Color_t Color , int16_t X , int16_t Y , uint16_t R , uint16_t Width )
{
    if( !prv_GFX_Is_Ready() || R == 0 ) return ;

    /* 两侧居中：中心线半径 = R，外径 = R + Width/2，内径 = R - Width/2 */
    uint32_t outer_r ;
    uint32_t inner_r ;
    bool     ring = ( Width > 0 ) ;

    if( ring )
    {
        outer_r = R + Width / 2 ;
        inner_r = ( R > Width / 2 ) ? ( R - Width / 2 ) : 0 ;
        if( inner_r >= outer_r ) ring = false ;   /* 线宽太大，退化成实心 */
    }
    else
    {
        outer_r = R ;
        inner_r = 0 ;
    }

    /* 坐标 API 为 int16_t，拒绝无法安全参与坐标运算的半径。 */
    if( outer_r > INT16_MAX ) return ;

    int32_t ox = (int32_t)outer_r ;
    int32_t ix = (int32_t)inner_r ;

    for( int32_t dy = 0 ; dy < (int32_t)outer_r ; dy++ )
    {
        /* 收缩外圈：找到最大的 ox 使 ox² + dy² < outer_r²（严格内部，去掉正交突起） */
        while( ox > 0 && (int64_t)ox * ox + (int64_t)dy * dy >= (int64_t)outer_r * outer_r )
            ox-- ;

        /* 收缩内圈 */
        bool inner_valid = false ;
        if( ring && dy < (int32_t)inner_r )
        {
            while( ix > 0 && (int64_t)ix * ix + (int64_t)dy * dy >= (int64_t)inner_r * inner_r )
                ix-- ;
            inner_valid = ( ix > 0 ) ;
        }

        /* 画 +dy 和 -dy 两行 */
        for( uint8_t s = 0 ; s < 2 ; s++ )
        {
            if( s == 1 && dy == 0 ) continue ;
            int32_t py = ( s == 0 ) ? (int32_t)Y + dy : (int32_t)Y - dy ;

            if( ring && inner_valid )
            {
                if( ox > ix )
                {
                    prv_GFX_Fill_HLine_Clipped( Color , (int32_t)X - ox , (int32_t)X - ix - 1 , py ) ;
                    prv_GFX_Fill_HLine_Clipped( Color , (int32_t)X + ix + 1 , (int32_t)X + ox , py ) ;
                }
            }
            else
            {
                prv_GFX_Fill_HLine_Clipped( Color , (int32_t)X - ox , (int32_t)X + ox , py ) ;
            }
        }
    }
}

/**
 * @brief 在指定位置填充矩形（自动裁剪）
 */
void GFX_Fill_Rect( GFX_Color_t Color , int16_t X , int16_t Y , uint16_t W , uint16_t H )
{
    if( !prv_GFX_Is_Ready() || W == 0 || H == 0 ) return ;
    Area_t * scr = &The_GFX.Current->Scr_Area ;
    Area_t rect = { X , Y , W , H } ;
    Area_t com ;

    /* 求与屏幕的交集，无交集则跳过 */
    if( !Area_Com( &rect , scr , &com ) )
        return ;

    GFX_Port_Fill( Color , com.X , com.Y , com.W , com.H ) ;
}

/**
 * @brief 高级图像填充：支持整图/Cut_Self 锚点、屏幕/自身裁剪、自定义调色板和位图显隐掩码。
 *
 * @note MIRROR：忽略 Palette 和 Enable_Map，按原图直接绘制
 *       BITMAP：Palette=NULL 时用 Fore_Color/Back_Color 渐变，否则用传入 Palette
 *       BITMAP_WITH_PALETTE：Palette=NULL 时用图内色板，否则用传入 Palette
 *       Enable_Map：bit N=1 取色板，=0 取 Back_Color；0xFFFFFFFF 全显示，屏蔽不透明跳过
 *       直接 Fill 不执行独立 Alpha 混合。
 */
void GFX_Fill_Img_Adv( GFX_Fill_Img_Adv_Para_t * Para )
{
    if( !prv_GFX_Is_Ready() ) return ;
    if( !Para || !Para->Img ) return ;
    const GFX_Img_t * img = Para->Img ;
    if( img->W == 0 || img->H == 0 || img->Color_Type == GFX_COLOR_TYPE_NONE ) return ;
    if( img->Color_Save_Way != GFX_Save_Way_MCU && img->Color_Save_Way != GFX_Save_Way_Flash ) return ;
    if( img->Color_Save_Way == GFX_Save_Way_MCU && img->Color_Save_Info.C_Array == NULL ) return ;
    if( img->Color_Type == GFX_COLOR_TYPE_BITMAP && img->Alpha_Enable ) return ;
    if( img->Color_Type != GFX_COLOR_TYPE_MIRROR &&
        ( img->Color_Bits_Per_Pix < GFX_BPP_MIN || img->Color_Bits_Per_Pix > GFX_BPP_MAX ) ) return ;

    int16_t real_x ;
    int16_t real_y ;
    if( !prv_Img_Adv_Get_Position( img , Para->Anchor , Para->X , Para->Y ,
                                   Para->Anchor_Use_Cut_Self , Para->Cut_Self_Enable ,
                                   &Para->Cut_Self , &real_x , &real_y ) ) return ;

    /* 求图片、屏幕、可选屏幕裁剪和可选自身裁剪的共同可绘制区域 */
    Area_t * scr = &The_GFX.Current->Scr_Area ;
    Area_t com ;
    if( !prv_Img_Adv_Get_Draw_Area( img , real_x , real_y , scr ,
                                    Para->Cut_Screen_Enable , &Para->Cut_Screen ,
                                    Para->Cut_Self_Enable , &Para->Cut_Self , &com ) ) return ;

    switch( Para->Img->Color_Type )
    {
        case GFX_COLOR_TYPE_MIRROR :
            prv_Fill_Img_Mirror( Para->Img , real_x , real_y , &com ) ;
            break ;

        case GFX_COLOR_TYPE_BITMAP :
        {
            /* Palette=NULL → 用 Fore_Color/Back_Color 生成渐变色板；否则用传入 Palette 覆盖 */
            if( Para->Palette == NULL )
                GFX_Create_Color_Palette( (uint8_t)Para->Img->Color_Bits_Per_Pix , The_GFX.Palette ,
                                          Para->Fore_Color , Para->Back_Color ) ;
            prv_Fill_Img_Bitmap_Core( Para->Img , real_x , real_y , &com ,
                                      Para->Palette ? Para->Palette : The_GFX.Palette ,
                                      Para->Enable_Map , GFX_Color_Convert( Para->Back_Color ) ) ;
            break ;
        }
        case GFX_COLOR_TYPE_BITMAP_WITH_PALETTE :
        {
            /* Palette=NULL → 用图片自带调色板；否则用传入 Palette 覆盖 */
            if( Para->Palette == NULL )
                GFX_Img_Copy_Palette( Para->Img , The_GFX.Palette ) ;
            prv_Fill_Img_Bitmap_Core( Para->Img , real_x , real_y , &com ,
                                      Para->Palette ? Para->Palette : The_GFX.Palette ,
                                      Para->Enable_Map , GFX_Color_Convert( Para->Back_Color ) ) ;
            break ;
        }
    }
}

/* ========================= 图片数据访问 ========================= */

/**
 * @brief 复制图片的调色板到外部 RAM 缓冲
 *
 * @param Img  图片描述子（调色板紧跟在像素数据后面，存储位置与像素数据一致）
 * @param Dest 目标缓冲区（调用方分配，大小 >= sizeof(GFX_Color_t) * (1 << BPP)）
 *
 * @note 无论调色板存在 MCU Flash 还是外部 Flash，都通过此函数复制到 RAM。
 *       外部 Flash 不能直接寻址，所以不提供 Get 指针函数。
 */
void GFX_Img_Copy_Palette( const GFX_Img_t * Img , GFX_Color_t * Dest )
{
    if( !prv_GFX_Is_Ready() || !Dest || !Img || Img->Color_Type == GFX_COLOR_TYPE_NONE ) return ;

    uint16_t palette_count = 1 << Img->Color_Bits_Per_Pix ;
    uint32_t palette_bytes = (uint32_t)palette_count * sizeof( GFX_Color_t ) ;

    /* 计算像素数据大小（调色板从下一个完整字节开始，方便裁剪定位） */
    uint16_t bytes_per_row = ( (uint16_t)Img->W * Img->Color_Bits_Per_Pix + 7 ) / 8 ;
    uint32_t data_size = (uint32_t)bytes_per_row * Img->H ;

    if( Img->Color_Save_Way == GFX_Save_Way_MCU )
    {
        /* 图内色板紧跟位流，起点不保证满足 GFX_Color_t 对齐要求。 */
        const uint8_t * src = Img->Color_Save_Info.C_Array + data_size ;
        memcpy( Dest , src , palette_bytes ) ;
    }
    else
    {
        /* 外部 Flash：通过 Port 层读取 */
        GFX_Port_Flash_Read( Img->Color_Save_Info.Flash_Addr + data_size , Dest , palette_bytes ) ;
    }
}

/* ========================= 缓冲区绘图 API（先写入缓冲区，再通过 Flush 刷新） ========================= */

/**
 * @brief 用同一种颜色填满整个缓冲区（Buffer + Area 由 GFX_Buffer_t 统一描述）
 *
 * @note Buff 的有效性由调用方保证。
 */
void GFX_Buff_Bg( const GFX_Buffer_t * Buff , GFX_Color_t Color )
{
    if( !prv_GFX_Buffer_Is_Valid( Buff ) ) return ;
    uint32_t total = (uint32_t)Buff->Area.W * Buff->Area.H ;
    for( uint32_t i = 0 ; i < total ; i++ )
        Buff->Buffer[ i ] = Color ;
}

/**
 * @brief 将图片绘制到缓冲区（不刷屏）。支持 MIRROR / BITMAP / PALETTE 三种类型；独立 Alpha 仅支持 MIRROR / PALETTE。
 *        图片超出缓冲区区域时自动裁剪。简单版不支持透明掩码（用 Adv 版）。
 *
 * @param Buff     目标缓冲区（Area + Buffer）
 * @param Img      图片描述子
 * @param Image_X  图片在缓冲区坐标系中的左上角 X
 * @param Image_Y  图片在缓冲区坐标系中的左上角 Y
 *
 * @note BITMAP 类型用灰度默认色板，PALETTE 类型用图片自带调色板。
 *       Flash 存储的 BITMAP/PALETTE 借用 Manager.Buffer 临时读一行 bit 流。
 *       MIRROR / BITMAP_WITH_PALETTE 的 Alpha_Enable=1 时，按 Alpha_Bits_Per_Pix 混合。
 *       BITMAP 不允许独立 Alpha，违规描述子会被拒绝。
 */
static bool prv_Buff_Img_Is_Valid( const GFX_Buffer_t * Buff , const GFX_Img_t * Img )
{
    if( !prv_GFX_Is_Ready() || !prv_GFX_Buffer_Is_Valid( Buff ) ) return false ;
    if( Img == NULL || Img->W == 0 || Img->H == 0 || Img->Color_Type == GFX_COLOR_TYPE_NONE ) return false ;
    if( Img->Color_Save_Way != GFX_Save_Way_MCU && Img->Color_Save_Way != GFX_Save_Way_Flash ) return false ;
    if( Img->Color_Save_Way == GFX_Save_Way_MCU && Img->Color_Save_Info.C_Array == NULL ) return false ;
    if( Img->Color_Type != GFX_COLOR_TYPE_MIRROR &&
        ( Img->Color_Bits_Per_Pix < GFX_BPP_MIN || Img->Color_Bits_Per_Pix > GFX_BPP_MAX ) ) return false ;
    if( Img->Color_Type == GFX_COLOR_TYPE_BITMAP && Img->Alpha_Enable ) return false ;
    if( Img->Alpha_Enable )
    {
        if( Img->Alpha_Bits_Per_Pix < GFX_BPP_MIN || Img->Alpha_Bits_Per_Pix > GFX_BPP_MAX ) return false ;
        if( Img->Alpha_Save_Way != GFX_Save_Way_MCU && Img->Alpha_Save_Way != GFX_Save_Way_Flash ) return false ;
        if( Img->Alpha_Save_Way == GFX_Save_Way_MCU && Img->Alpha_Save_Info.C_Array == NULL ) return false ;
    }
    return true ;
}

static uint8_t prv_Buff_Read_Packed_Value( const uint8_t * Row , uint32_t Bit_Position , uint8_t BPP )
{
    uint8_t value = 0 ;
    for( uint8_t bit_index = 0 ; bit_index < BPP ; bit_index++ )
    {
        uint32_t current_bit = Bit_Position + bit_index ;
        uint8_t bit = ( Row[ current_bit / 8 ] >> ( current_bit % 8 ) ) & 0x01u ;
        value |= (uint8_t)( bit << bit_index ) ;
    }
    return value ;
}

static void prv_Buff_Img_Core( const GFX_Buffer_t * Buff , const GFX_Img_t * Img ,
                               int16_t Image_X , int16_t Image_Y , const GFX_Color_t * Palette ,
                               uint32_t Enable_Map , GFX_Color_t Back_Color , bool Blend_Back_Color_Enable ,
                               const Area_t * Cut_Area )
{
    if( !prv_Buff_Img_Is_Valid( Buff , Img ) ) return ;

    Area_t img_area = { Image_X , Image_Y , (uint16_t)Img->W , (uint16_t)Img->H } ;
    Area_t buf_area = Buff->Area ;
    Area_t com ;
    if( !Area_Com( &img_area , &buf_area , &com ) ) return ;
    if( Cut_Area != NULL )
    {
        Area_t cut_area = *Cut_Area ;
        Area_t clipped ;
        if( !Area_Com( &com , &cut_area , &clipped ) ) return ;
        com = clipped ;
    }

    int16_t off_x = com.X - Image_X ;
    int16_t off_y = com.Y - Image_Y ;
    int16_t buf_off_x = com.X - Buff->Area.X ;
    int16_t buf_off_y = com.Y - Buff->Area.Y ;

    uint8_t alpha_bpp = (uint8_t)Img->Alpha_Bits_Per_Pix ;
    uint16_t alpha_raw_bytes_per_row = Img->Alpha_Enable ?
        (uint16_t)( ( Img->W * alpha_bpp + 7 ) / 8 ) : 0 ;
    uint32_t alpha_row_start_bit = (uint32_t)off_x * alpha_bpp ;
    uint8_t alpha_bit_offset = (uint8_t)( alpha_row_start_bit % 8 ) ;
    uint16_t alpha_read_bytes_per_row = Img->Alpha_Enable ?
        (uint16_t)( ( alpha_bit_offset + com.W * alpha_bpp + 7 ) / 8 ) : 0 ;
    uint32_t alpha_start_byte = alpha_row_start_bit / 8 ;
    uint8_t alpha_max = Img->Alpha_Enable ? (uint8_t)( ( 1u << alpha_bpp ) - 1u ) : 0 ;
    uint32_t manager_bytes = The_GFX.Manager.Max * The_GFX.Manager.Bytes_Per_Pix ;

    if( Img->Color_Type == GFX_COLOR_TYPE_MIRROR )
    {
        uint8_t pixel_bytes = sizeof( GFX_Color_t ) ;
        uint16_t bytes_per_row = (uint16_t)Img->W * pixel_bytes ;
        uint16_t color_read_bytes = (uint16_t)( com.W * pixel_bytes ) ;
        uint32_t scratch_bytes = Img->Color_Save_Way == GFX_Save_Way_Flash ? color_read_bytes : 0 ;
        uint8_t * color_scratch = (uint8_t *)The_GFX.Manager.Buffer ;
        uint8_t * alpha_scratch = color_scratch == NULL ? NULL : color_scratch + scratch_bytes ;

        if( Img->Alpha_Enable && Img->Alpha_Save_Way == GFX_Save_Way_Flash )
            scratch_bytes += alpha_read_bytes_per_row ;
        if( scratch_bytes > 0 && ( The_GFX.Manager.Buffer == NULL || scratch_bytes > manager_bytes ) ) return ;

        for( uint16_t i = 0 ; i < com.H ; i++ )
        {
            uint32_t source_offset = (uint32_t)( off_y + i ) * bytes_per_row +
                                     (uint32_t)off_x * pixel_bytes ;
            GFX_Color_t * dst_row = Buff->Buffer +
                (uint32_t)( buf_off_y + i ) * Buff->Area.W + buf_off_x ;
            const uint8_t * src_row ;
            if( Img->Color_Save_Way == GFX_Save_Way_MCU )
                src_row = Img->Color_Save_Info.C_Array + source_offset ;
            else
            {
                GFX_Port_Flash_Read( Img->Color_Save_Info.Flash_Addr + source_offset ,
                                     color_scratch , color_read_bytes ) ;
                src_row = color_scratch ;
            }

            const uint8_t * alpha_row = NULL ;
            if( Img->Alpha_Enable )
            {
                uint32_t alpha_row_offset = (uint32_t)( off_y + i ) * alpha_raw_bytes_per_row + alpha_start_byte ;
                if( Img->Alpha_Save_Way == GFX_Save_Way_MCU )
                    alpha_row = Img->Alpha_Save_Info.C_Array + alpha_row_offset ;
                else
                {
                    GFX_Port_Flash_Read( Img->Alpha_Save_Info.Flash_Addr + alpha_row_offset ,
                                         alpha_scratch , alpha_read_bytes_per_row ) ;
                    alpha_row = alpha_scratch ;
                }
            }

            for( uint16_t p = 0 ; p < com.W ; p++ )
            {
                GFX_Color_t source_color ;
                memcpy( &source_color , src_row + (uint32_t)p * pixel_bytes , sizeof( source_color ) ) ;
                if( Img->Alpha_Enable )
                {
                    uint8_t alpha = prv_Buff_Read_Packed_Value(
                        alpha_row , alpha_bit_offset + (uint32_t)p * alpha_bpp , alpha_bpp ) ;
                    GFX_Color_t blend_bg = Blend_Back_Color_Enable ? Back_Color : dst_row[ p ] ;
                    dst_row[ p ] = GFX_Color_AlphaBlend( blend_bg , source_color , alpha , alpha_max ) ;
                }
                else
                    dst_row[ p ] = source_color ;
            }
        }
        return ;
    }

    if( Palette == NULL ) return ;

    uint8_t bpp = (uint8_t)Img->Color_Bits_Per_Pix ;
    uint16_t raw_bytes_per_row = (uint16_t)( ( Img->W * bpp + 7 ) / 8 ) ;
    uint32_t row_start_bit = (uint32_t)off_x * bpp ;
    uint8_t bit_offset = (uint8_t)( row_start_bit % 8 ) ;
    uint16_t read_bytes_per_row = (uint16_t)( ( bit_offset + com.W * bpp + 7 ) / 8 ) ;
    uint32_t start_byte = row_start_bit / 8 ;
    uint8_t * bmp_buf = NULL ;
    uint32_t scratch_bytes = 0 ;

    if( Img->Color_Save_Way == GFX_Save_Way_Flash )
    {
        bmp_buf = (uint8_t *)The_GFX.Manager.Buffer ;
        scratch_bytes = read_bytes_per_row ;
    }
    uint8_t * scratch_base = (uint8_t *)The_GFX.Manager.Buffer ;
    uint8_t * alpha_scratch = scratch_base == NULL ? NULL : scratch_base + scratch_bytes ;
    if( Img->Alpha_Enable && Img->Alpha_Save_Way == GFX_Save_Way_Flash )
        scratch_bytes += alpha_read_bytes_per_row ;
    if( scratch_bytes > 0 && ( The_GFX.Manager.Buffer == NULL || scratch_bytes > manager_bytes ) ) return ;

    for( uint16_t i = 0 ; i < com.H ; i++ )
    {
        uint16_t src_row = off_y + i ;
        const uint8_t * row_src ;
        if( Img->Color_Save_Way == GFX_Save_Way_MCU )
        {
            row_src = Img->Color_Save_Info.C_Array +
                      (uint32_t)src_row * raw_bytes_per_row + start_byte ;
        }
        else
        {
            uint32_t row_addr = Img->Color_Save_Info.Flash_Addr +
                                (uint32_t)src_row * raw_bytes_per_row + start_byte ;
            GFX_Port_Flash_Read( row_addr , bmp_buf , read_bytes_per_row ) ;
            row_src = bmp_buf ;
        }

        GFX_Color_t * dst_row = Buff->Buffer +
            (uint32_t)( buf_off_y + i ) * Buff->Area.W + buf_off_x ;
        const uint8_t * alpha_row = NULL ;
        if( Img->Alpha_Enable )
        {
            uint32_t alpha_row_offset = (uint32_t)src_row * alpha_raw_bytes_per_row + alpha_start_byte ;
            if( Img->Alpha_Save_Way == GFX_Save_Way_MCU )
                alpha_row = Img->Alpha_Save_Info.C_Array + alpha_row_offset ;
            else
            {
                GFX_Port_Flash_Read( Img->Alpha_Save_Info.Flash_Addr + alpha_row_offset ,
                                     alpha_scratch , alpha_read_bytes_per_row ) ;
                alpha_row = alpha_scratch ;
            }
        }
        uint16_t bit_pos = bit_offset ;
        for( uint16_t p = 0 ; p < com.W ; p++ )
        {
            uint8_t val = prv_Buff_Read_Packed_Value( row_src , bit_pos , bpp ) ;
            GFX_Color_t source_color = ( ( Enable_Map >> val ) & 1u ) ? Palette[ val ] : Back_Color ;
            if( Img->Alpha_Enable )
            {
                uint8_t alpha = prv_Buff_Read_Packed_Value(
                    alpha_row , alpha_bit_offset + (uint32_t)p * alpha_bpp , alpha_bpp ) ;
                GFX_Color_t blend_bg = Blend_Back_Color_Enable ? Back_Color : dst_row[ p ] ;
                dst_row[ p ] = GFX_Color_AlphaBlend( blend_bg , source_color , alpha , alpha_max ) ;
            }
            else
                dst_row[ p ] = source_color ;
            bit_pos += bpp ;
        }
    }
}

void GFX_Buff_Img( const GFX_Buffer_t * Buff , const GFX_Img_t * Img , int16_t Image_X , int16_t Image_Y )
{
    if( !prv_Buff_Img_Is_Valid( Buff , Img ) ) return ;

    if( Img->Color_Type == GFX_COLOR_TYPE_MIRROR )
    {
        prv_Buff_Img_Core( Buff , Img , Image_X , Image_Y , NULL , 0xFFFFFFFFu , 0 , false , NULL ) ;
    }
    else
    {
        if( Img->Color_Type == GFX_COLOR_TYPE_BITMAP )
            GFX_Create_Gray_Palette( (uint8_t)Img->Color_Bits_Per_Pix , The_GFX.Palette ) ;
        else
            GFX_Img_Copy_Palette( Img , The_GFX.Palette ) ;
        prv_Buff_Img_Core( Buff , Img , Image_X , Image_Y , The_GFX.Palette ,
                           0xFFFFFFFFu , The_GFX.Palette[ 0 ] , false , NULL ) ;
    }
}

/**
 * @brief 缓冲区绘制图片（扩展参数版），支持整图/Cut_Self 锚点、屏幕/自身裁剪、调色板、显隐掩码和独立 Alpha 通道。
 *
 * @note 参数语义与 GFX_Fill_Img_Adv 一致，目标改为 Buff。
 */
void GFX_Buff_Img_Adv( GFX_Buff_Img_Adv_Para_t * Para )
{
    if( Para == NULL || !prv_Buff_Img_Is_Valid( Para->Buff , Para->Img ) ) return ;

    int16_t image_x ;
    int16_t image_y ;
    if( !prv_Img_Adv_Get_Position( Para->Img , Para->Anchor , Para->X , Para->Y ,
                                   Para->Anchor_Use_Cut_Self , Para->Cut_Self_Enable ,
                                   &Para->Cut_Self , &image_x , &image_y ) ) return ;

    Area_t draw_area ;
    if( !prv_Img_Adv_Get_Draw_Area( Para->Img , image_x , image_y , &Para->Buff->Area ,
                                    Para->Cut_Screen_Enable , &Para->Cut_Screen ,
                                    Para->Cut_Self_Enable , &Para->Cut_Self , &draw_area ) ) return ;

    if( Para->Img->Color_Type == GFX_COLOR_TYPE_MIRROR )
    {
        prv_Buff_Img_Core( Para->Buff , Para->Img , image_x , image_y ,
                           NULL , 0xFFFFFFFFu , GFX_Color_Convert( Para->Back_Color ) ,
                           Para->Blend_Back_Color_Enable , &draw_area ) ;
        return ;
    }

    const GFX_Color_t * palette = Para->Palette ;
    if( palette == NULL )
    {
        if( Para->Img->Color_Type == GFX_COLOR_TYPE_BITMAP )
            GFX_Create_Color_Palette( (uint8_t)Para->Img->Color_Bits_Per_Pix , The_GFX.Palette ,
                                      Para->Fore_Color , Para->Back_Color ) ;
        else
            GFX_Img_Copy_Palette( Para->Img , The_GFX.Palette ) ;
        palette = The_GFX.Palette ;
    }

    prv_Buff_Img_Core( Para->Buff , Para->Img , image_x , image_y , palette ,
                       Para->Enable_Map , GFX_Color_Convert( Para->Back_Color ) ,
                       Para->Blend_Back_Color_Enable , &draw_area ) ;
}

/**
 * @brief 在缓冲区中绘制实心圆（覆盖）
 *
 * @param Buff 目标缓冲区
 * @param X 圆心 X 坐标（显示器绝对坐标）
 * @param Y 圆心 Y 坐标（显示器绝对坐标）
 * @param R 半径
 * @param Color 填充颜色
 *
 * @note 坐标使用显示器绝对坐标，并自动裁剪到 Buff.Area。
 */
void GFX_Buff_Circle( const GFX_Buffer_t * Buff , int16_t X , int16_t Y , uint16_t R , GFX_Color_t Color )
{
    if( !prv_GFX_Buffer_Is_Valid( Buff ) || R == 0 || R > INT16_MAX ) return ;

    int32_t ox = (int32_t)R ;

    for( int32_t dy = 0 ; dy < (int32_t)R ; dy++ )
    {
        while( ox > 0 && (int64_t)ox * ox + (int64_t)dy * dy >= (int64_t)R * R )
            ox-- ;

        for( uint8_t s = 0 ; s < 2 ; s++ )
        {
            if( s == 1 && dy == 0 ) continue ;
            int32_t py = ( s == 0 ) ? (int32_t)Y + dy : (int32_t)Y - dy ;
            prv_GFX_Buff_HLine_Clipped( Buff , Color , (int32_t)X - ox , (int32_t)X + ox , py ) ;
        }
    }
}

/**
 * @brief 在缓冲区中覆盖填充矩形
 *
 * @note 坐标使用显示器绝对坐标，并自动裁剪到 Buff.Area。
 */
void GFX_Buff_Rect( const GFX_Buffer_t * Buff , int16_t X , int16_t Y , uint16_t W , uint16_t H , GFX_Color_t Color )
{
    if( !prv_GFX_Buffer_Is_Valid( Buff ) || W == 0 || H == 0 ) return ;

    Area_t rect = { X , Y , W , H } ;
    Area_t clipped ;
    if( !Area_Com( &rect , &Buff->Area , &clipped ) ) return ;

    for( uint16_t row = 0 ; row < clipped.H ; row++ )
    {
        int32_t y = (int32_t)clipped.Y + row ;
        prv_GFX_Buff_HLine_Clipped( Buff , Color , clipped.X ,
                                    (int32_t)clipped.X + clipped.W - 1 , y ) ;
    }
}

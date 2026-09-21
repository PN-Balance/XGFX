/**
 * @file GFX_Port_Sim.c
 * @brief GFX Port 层模拟实现 —— 用内存帧缓冲替代真实硬件
 *
 * @note Flash 模拟：GFX_Port_Init 时读取 _mirror.bin 到 s_Flash_Buffer，
 *       模拟单片机外部 Flash 芯片内容。
 *       - GFX_Port_Flash_To_GFX：从 s_Flash_Buffer 读 RGB565 像素刷到 framebuffer
 *       - GFX_Port_Flash_Read  ：从 s_Flash_Buffer memcpy 到目标缓冲
 *       bin 文件为 RGB565 小端序，与 GFX_Color_t(uint16_t) 内存布局一致。
 */

#include "GFX_Port.h"
#include "GFX.h"
#include "GFX_Sim.h"
#include <string.h>
#include <stdio.h>

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 文件作用域对象 */
/* -------------------------------------------------------------------------------------------------------------------------- */

#define SIM_SCREEN_W    240
#define SIM_SCREEN_H    320

/* 模拟外部 Flash 容量 */
#define SIM_FLASH_SIZE  ( 64 * 1024 )

/* bin 文件名（运行时相对路径读取，需在 sim 目录下执行）
 * _flash.bin = _mirror.bin @ 0 + _palette.bin @ 1536 */
#define SIM_FLASH_BIN   "_flash.bin"

static GFX_Color_t s_Framebuffer[ SIM_SCREEN_W * SIM_SCREEN_H ] ;
static uint8_t     s_Flash_Buffer[ SIM_FLASH_SIZE ] ;
static uint32_t    s_Flash_Loaded ;   /* 已从 bin 载入的字节数 */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 模拟器对外接口（供 GFX_Sim.c 窗口绘制使用） */
/* -------------------------------------------------------------------------------------------------------------------------- */

GFX_Color_t * GFX_Sim_Get_Framebuffer( void ) { return s_Framebuffer ; }
uint16_t      GFX_Sim_Get_Width( void )       { return SIM_SCREEN_W ; }
uint16_t      GFX_Sim_Get_Height( void )      { return SIM_SCREEN_H ; }

bool GFX_Sim_Flash_Write( uint32_t Address , const void * Data , uint32_t Size )
{
    if( Data == NULL || Address > SIM_FLASH_SIZE || Size > SIM_FLASH_SIZE - Address )
        return false ;

    memcpy( s_Flash_Buffer + Address , Data , Size ) ;
    if( Address + Size > s_Flash_Loaded )
        s_Flash_Loaded = Address + Size ;
    return true ;
}

/* -------------------------------------------------------------------------------------------------------------------------- */
/* Port 层实现 */
/* -------------------------------------------------------------------------------------------------------------------------- */

void GFX_Port_Init( GFX_t * GFX_Device )
{
    GFX_Device->W = SIM_SCREEN_W ;
    GFX_Device->H = SIM_SCREEN_H ;
    GFX_Device->Brightness = 0 ;
    GFX_Device->Draw_Direction = Orth_Dir_0 ;

    /* framebuffer 初始填黑色 */
    memset( s_Framebuffer , 0 , sizeof( s_Framebuffer ) ) ;

    /* 载入模拟 Flash 内容（bin 文件） */
    memset( s_Flash_Buffer , 0 , sizeof( s_Flash_Buffer ) ) ;
    s_Flash_Loaded = 0 ;
    FILE * fp = fopen( SIM_FLASH_BIN , "rb" ) ;
    if( fp != NULL )
    {
        s_Flash_Loaded = (uint32_t)fread( s_Flash_Buffer , 1 , SIM_FLASH_SIZE , fp ) ;
        fclose( fp ) ;
    }
}

void GFX_Port_Fill( GFX_Color_t Color , int16_t X , int16_t Y , uint16_t W , uint16_t H )
{
    for( int16_t y = Y ; y < Y + (int16_t)H ; y++ )
    {
        if( y < 0 || y >= SIM_SCREEN_H ) continue ;
        for( int16_t x = X ; x < X + (int16_t)W ; x++ )
        {
            if( x < 0 || x >= SIM_SCREEN_W ) continue ;
            s_Framebuffer[ y * SIM_SCREEN_W + x ] = Color ;
        }
    }
}

void GFX_Port_Flush( GFX_Color_t * Buffer , int16_t X , int16_t Y , uint16_t W , uint16_t H )
{
    if( Buffer == NULL ) return ;

    for( uint16_t row = 0 ; row < H ; row++ )
    {
        int16_t sy = Y + (int16_t)row ;
        if( sy < 0 || sy >= SIM_SCREEN_H ) continue ;

        for( uint16_t col = 0 ; col < W ; col++ )
        {
            int16_t sx = X + (int16_t)col ;
            if( sx < 0 || sx >= SIM_SCREEN_W ) continue ;
            s_Framebuffer[ sy * SIM_SCREEN_W + sx ] = Buffer[ row * W + col ] ;
        }
    }
}

void GFX_Port_Flash_To_GFX( uint32_t Addr , int16_t X , int16_t Y , uint16_t W , uint16_t H )
{
    /* Addr 为字节地址；bin 以 RGB565 小端序存储，与 GFX_Color_t 内存布局一致 */
    if( ( Addr + (uint32_t)W * H * sizeof( GFX_Color_t ) ) > s_Flash_Loaded )
        return ;

    const GFX_Color_t * src = (const GFX_Color_t *)( s_Flash_Buffer + Addr ) ;

    for( uint16_t row = 0 ; row < H ; row++ )
    {
        int16_t sy = Y + (int16_t)row ;
        if( sy < 0 || sy >= SIM_SCREEN_H ) continue ;

        for( uint16_t col = 0 ; col < W ; col++ )
        {
            int16_t sx = X + (int16_t)col ;
            if( sx < 0 || sx >= SIM_SCREEN_W ) continue ;
            s_Framebuffer[ sy * SIM_SCREEN_W + sx ] = src[ row * W + col ] ;
        }
    }
}

void GFX_Port_Flash_Read( uint32_t Addr , void * Buffer , uint32_t Size )
{
    if( Buffer == NULL || Size == 0 ) return ;
    if( ( Addr + Size ) > s_Flash_Loaded ) return ;

    memcpy( Buffer , s_Flash_Buffer + Addr , Size ) ;
}

void GFX_Port_Set_Brightness( uint8_t Brightness )
{
    (void)Brightness ;
}

void GFX_Port_Set_Draw_Direction( Orth_Dir_t Direction )
{
    (void)Direction ;
}

/**
 * @file GFX_Sim.c
 * @brief Win32 GDI 窗口模拟器 —— 把帧缓冲显示到窗口
 */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "GFX_Sim.h"
#include "GFX_Define.h"

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 配置 */
/* -------------------------------------------------------------------------------------------------------------------------- */

#define PIXEL_SCALE   4   /* 每个屏幕像素放大 4 倍显示 */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 文件作用域对象 */
/* -------------------------------------------------------------------------------------------------------------------------- */

static HWND   s_hWnd    = NULL ;
static void * s_hInst   = NULL ;
static uint32_t * s_BmpBuf = NULL ;   /* 32bit BGR 位图缓冲 */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 内部函数 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* RGB565 → 32bit DIB 像素（内存序 B-G-R-X，小端 uint32_t = R<<16 | G<<8 | B） */
static uint32_t prv_RGB565_To_DIB( uint16_t color )
{
    uint8_t r = (uint8_t)( ( ( color >> 11 ) & 0x1F ) * 255 / 31 ) ;
    uint8_t g = (uint8_t)( ( ( color >> 5  ) & 0x3F ) * 255 / 63 ) ;
    uint8_t b = (uint8_t)( (   color         & 0x1F ) * 255 / 31 ) ;
    return ( (uint32_t)r << 16 ) | ( (uint32_t)g << 8 ) | b ;
}

static LRESULT CALLBACK prv_WndProc( HWND hwnd , UINT msg , WPARAM wParam , LPARAM lParam )
{
    switch( msg )
    {
    case WM_PAINT :
    {
        PAINTSTRUCT ps ;
        HDC hdc = BeginPaint( hwnd , &ps ) ;

        uint16_t w = GFX_Sim_Get_Width() ;
        uint16_t h = GFX_Sim_Get_Height() ;
        GFX_Color_t * fb = GFX_Sim_Get_Framebuffer() ;

        if( s_BmpBuf == NULL )
            s_BmpBuf = (uint32_t *)malloc( (size_t)w * h * 4 ) ;

        /* 帧缓冲 → 32bit DIB（小端 uint32_t = R<<16 | G<<8 | B） */
        for( uint32_t i = 0 ; i < (uint32_t)w * h ; i++ )
        {
#if ( GFX_COLOR_TYPE == GFX_COLOR_TYPE_RGB565 )
            s_BmpBuf[ i ] = prv_RGB565_To_DIB( fb[ i ] ) ;
#elif ( GFX_COLOR_TYPE == GFX_COLOR_TYPE_RGB888 )
            uint32_t c = fb[ i ] ;
            s_BmpBuf[ i ] = ( ( c >> 16 ) & 0xFF ) << 16 | ( c & 0xFF00 ) | ( c & 0xFF ) ;
#elif ( GFX_COLOR_TYPE == GFX_COLOR_TYPE_RGB332 )
            uint8_t c = (uint8_t)fb[ i ] ;
            uint8_t r = ( ( c >> 5 ) & 0x07 ) * 255 / 7 ;
            uint8_t g = ( ( c >> 2 ) & 0x07 ) * 255 / 7 ;
            uint8_t b = (   c & 0x03 ) * 255 / 3 ;
            s_BmpBuf[ i ] = ( (uint32_t)r << 16 ) | ( (uint32_t)g << 8 ) | b ;
#endif
        }

        BITMAPINFO bi ;
        memset( &bi , 0 , sizeof( bi ) ) ;
        bi.bmiHeader.biSize        = sizeof( BITMAPINFOHEADER ) ;
        bi.bmiHeader.biWidth       = w ;
        bi.bmiHeader.biHeight      = -(int32_t)h ;   /* top-down */
        bi.bmiHeader.biPlanes      = 1 ;
        bi.bmiHeader.biBitCount    = 32 ;
        bi.bmiHeader.biCompression = BI_RGB ;

        StretchDIBits( hdc ,
            0 , 0 , w * PIXEL_SCALE , h * PIXEL_SCALE ,
            0 , 0 , w , h ,
            s_BmpBuf , &bi , DIB_RGB_COLORS , SRCCOPY ) ;

        EndPaint( hwnd , &ps ) ;
        break ;
    }
    case WM_DESTROY :
        if( s_BmpBuf ) { free( s_BmpBuf ) ; s_BmpBuf = NULL ; }
        PostQuitMessage( 0 ) ;
        break ;
    default :
        return DefWindowProcA( hwnd , msg , wParam , lParam ) ;
    }
    return 0 ;
}

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 对外接口 */
/* -------------------------------------------------------------------------------------------------------------------------- */

void GFX_Sim_Init( void * hInstance )
{
    s_hInst = hInstance ;

    WNDCLASSA wc ;
    memset( &wc , 0 , sizeof( wc ) ) ;
    wc.lpfnWndProc   = prv_WndProc ;
    wc.hInstance     = (HINSTANCE)hInstance ;
    wc.hCursor       = LoadCursor( NULL , IDC_ARROW ) ;
    wc.lpszClassName = "XGFX_Sim" ;

    RegisterClassA( &wc ) ;
}

void GFX_Sim_Run( void )
{
    uint16_t w = GFX_Sim_Get_Width() ;
    uint16_t h = GFX_Sim_Get_Height() ;

    RECT rc = { 0 , 0 , w * PIXEL_SCALE , h * PIXEL_SCALE } ;
    AdjustWindowRect( &rc , WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU , FALSE ) ;

    s_hWnd = CreateWindowA( "XGFX_Sim" , "XGFX Simulator" ,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU ,
        CW_USEDEFAULT , CW_USEDEFAULT ,
        rc.right - rc.left , rc.bottom - rc.top ,
        NULL , NULL , (HINSTANCE)s_hInst , NULL ) ;

    ShowWindow( s_hWnd , SW_SHOW ) ;
    UpdateWindow( s_hWnd ) ;

    MSG msg ;
    while( GetMessageA( &msg , NULL , 0 , 0 ) )
    {
        TranslateMessage( &msg ) ;
        DispatchMessageA( &msg ) ;
    }
}

void GFX_Sim_Refresh( void )
{
    /* 保存 BMP 截图（原始分辨率，可无限放大看像素） */
    {
        uint16_t w = GFX_Sim_Get_Width() ;
        uint16_t h = GFX_Sim_Get_Height() ;
        GFX_Color_t * fb = GFX_Sim_Get_Framebuffer() ;

        FILE * fp = fopen( "screenshot.bmp" , "wb" ) ;
        if( fp )
        {
            uint32_t row_bytes = (uint32_t)w * 3 ;
            uint32_t pad = ( 4 - row_bytes % 4 ) % 4 ;
            uint32_t img_size = ( row_bytes + pad ) * h ;
            uint32_t file_size = 54 + img_size ;

            /* BMP 文件头 + 信息头 */
            uint8_t hdr[54] = {0} ;
            hdr[0] = 'B' ; hdr[1] = 'M' ;
            *(uint32_t*)(hdr+2) = file_size ;
            *(uint32_t*)(hdr+10) = 54 ;
            *(uint32_t*)(hdr+14) = 40 ;
            *(int32_t*)(hdr+18) = w ;
            *(int32_t*)(hdr+22) = h ;       /* 正数 = bottom-up */
            *(uint16_t*)(hdr+26) = 1 ;
            *(uint16_t*)(hdr+28) = 24 ;
            *(uint32_t*)(hdr+34) = img_size ;
            fwrite( hdr , 1 , 54 , fp ) ;

            /* 像素数据（BMP 是 bottom-up，BGR 顺序） */
            uint8_t pad_bytes[4] = {0} ;
            for( int32_t y = h - 1 ; y >= 0 ; y-- )
            {
                for( uint16_t x = 0 ; x < w ; x++ )
                {
                    GFX_Color_t c = fb[ y * w + x ] ;
                    uint8_t r , g , b ;
                    /* RGB565 → RGB888 */
                    r = ( ( c >> 11 ) & 0x1F ) << 3 ;
                    g = ( ( c >> 5  ) & 0x3F ) << 2 ;
                    b = (   c         & 0x1F ) << 3 ;
                    uint8_t px[3] = { b , g , r } ;   /* BGR */
                    fwrite( px , 1 , 3 , fp ) ;
                }
                if( pad ) fwrite( pad_bytes , 1 , pad , fp ) ;
            }
            fclose( fp ) ;
        }
    }

    if( s_hWnd )
        InvalidateRect( s_hWnd , NULL , FALSE ) ;
}

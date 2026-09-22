#include "GFX_Hardware_Test.h"
#include "GFX.h"
#include "gfx_test_assets.h"
#include "BY25Q80.h"
#include "Key.h"
#include "Power.h"
#include "Tick.h"
#include "Delay.h"
#include "MCU.h"
#include <string.h>

#define TEST_CASE_COUNT       10u
#define TEST_AUTO_PERIOD_MS   1800u
#define TEST_LONG_PRESS_MS    1200u
#define STRIPE_HEIGHT         24u

static uint8_t s_gfx_scratch[ 512u ];
static uint8_t s_stripe_memory[ 96u * STRIPE_HEIGHT * sizeof( GFX_Color_t ) + sizeof( GFX_Color_t ) ];
static uint8_t s_case;
static bool s_auto_run;
static bool s_flash_ok;
static Tick_t s_auto_tick;

static GFX_Color_t C( uint32_t rgb ) { return GFX_Color_Convert( rgb ); }

static void draw_case_marker( uint8_t value, uint32_t rgb )
{
    static const uint8_t seg[10] = {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
    const uint8_t map = seg[value % 10u];
    const GFX_Color_t fg = C( rgb );
    const GFX_Color_t bg = C( 0x10151f );
    GFX_Fill_Rect( bg, 0, 0, 172, 30 );
    if( map & 0x01 ) GFX_Fill_Rect( fg, 7, 3, 18, 3 );
    if( map & 0x02 ) GFX_Fill_Rect( fg, 23, 5, 3, 8 );
    if( map & 0x04 ) GFX_Fill_Rect( fg, 23, 16, 3, 8 );
    if( map & 0x08 ) GFX_Fill_Rect( fg, 7, 23, 18, 3 );
    if( map & 0x10 ) GFX_Fill_Rect( fg, 5, 16, 3, 8 );
    if( map & 0x20 ) GFX_Fill_Rect( fg, 5, 5, 3, 8 );
    if( map & 0x40 ) GFX_Fill_Rect( fg, 7, 13, 18, 3 );
    GFX_Fill_Rect( C( s_auto_run ? 0x35d07f : 0x3a485c ), 38, 11, s_auto_run ? 116 : 36, 8 );
}

static void draw_striped_alpha( const GFX_Img_t * image, bool fixed_background )
{
    uint16_t y;
    for( y = 48; y < 48 + image->H; y += STRIPE_HEIGHT )
    {
        const uint16_t h = (uint16_t)(((48 + image->H - y) < STRIPE_HEIGHT) ?
                           (48 + image->H - y) : STRIPE_HEIGHT);
        Area_t area = { 38, (int16_t)y, 96, h };
        GFX_Buffer_t buffer;
        GFX_Buff_Img_Adv_Para_t p;
        if( !GFX_Buffer_Init( &buffer, &area, s_stripe_memory, sizeof(s_stripe_memory) ) ) return;
        GFX_Buff_Bg( &buffer, C( (y / STRIPE_HEIGHT) & 1u ? 0x16406a : 0xd06a35 ) );
        memset( &p, 0, sizeof(p) );
        p.Buff = &buffer;
        p.Img = image;
        p.X = 38;
        p.Y = 48;
        p.Anchor = Anchor_LT;
        p.Enable_Map = 0xffffffffu;
        p.Back_Color = 0x203040u;
        p.Blend_Back_Color_Enable = fixed_background;
        GFX_Buff_Img_Adv( &p );
        GFX_Flush( buffer.Buffer, area.X, area.Y, area.W, area.H );
    }
}

static void draw_case( uint8_t number )
{
    GFX_Fill_Bg( C( 0x10151f ) );
    draw_case_marker( number + 1u, s_flash_ok ? 0x4aa8ff : 0xff4055 );
    if( !s_flash_ok && number >= 1u && number <= 6u )
    {
        GFX_Fill_Rect( C(0xff4055), 16, 70, 140, 12 );
        GFX_Fill_Rect( C(0xffc14a), 30, 96, 112, 12 );
        return;
    }

    switch( number )
    {
        case 0: /* direct primitives and core clipping */
            GFX_Fill_Rect( C(0x1b6cff), -18, 42, 92, 48 );
            GFX_Fill_Rect( C(0x35d07f), 95, 54, 100, 36 );
            GFX_Fill_Circle( C(0xffc14a), 50, 150, 42, 7 );
            GFX_Fill_Circle( C(0xe04fff), 150, 305, 48, 0 );
            break;
        case 1: /* native RGB565 mirror */
            GFX_Fill_Img( &GFX_Test_Landscape_Mirror, 0, 55 );
            GFX_Fill_Img( &GFX_Test_Landscape_Mirror, 0, 183 );
            break;
        case 2: /* 4-bpp indexed palette */
            GFX_Fill_Img( &GFX_Test_Landscape_Palette4, 0, 55 );
            GFX_Fill_Rect( C(0x334155), 0, 184, 172, 112 );
            GFX_Fill_Img( &GFX_Test_Landscape_Palette4, 0, 184 );
            break;
        case 3: /* Cut_Self + Cut_Screen + anchor on Cut_Self */
        {
            GFX_Fill_Img_Adv_Para_t p;
            memset( &p, 0, sizeof(p) );
            p.Img = &GFX_Test_Landscape_Mirror;
            p.X = 86; p.Y = 155; p.Anchor = Anchor_CM;
            p.Cut_Self_Enable = true; p.Cut_Self = (Area_t){35, 18, 102, 76};
            p.Anchor_Use_Cut_Self = true;
            p.Cut_Screen_Enable = true; p.Cut_Screen = (Area_t){24, 74, 124, 164};
            GFX_Fill_Rect( C(0x263247), 24, 74, 124, 164 );
            GFX_Fill_Img_Adv( &p );
            break;
        }
        case 4: /* 4-bpp Alpha blended with current buffer pixels */
            draw_striped_alpha( &GFX_Test_Gear_Alpha4, false );
            break;
        case 5: /* 2-bpp palette + 2-bpp Alpha + fixed background */
            draw_striped_alpha( &GFX_Test_Gear_Palette2_Alpha2, true );
            break;
        case 6: /* 1-bpp BITMAP and generated foreground/background palette */
        {
            GFX_Fill_Img_Adv_Para_t p;
            memset( &p, 0, sizeof(p) );
            p.Img = &GFX_Test_Mono_Bitmap1;
            p.X = 86; p.Y = 152; p.Anchor = Anchor_CM;
            p.Enable_Map = 0xffffffffu; p.Fore_Color = 0x4aa8ff; p.Back_Color = 0x10151f;
            GFX_Fill_Img_Adv( &p );
            break;
        }
        case 7: /* buffer absolute coordinates and buffer-edge clipping */
        {
            Area_t area = { 38, 72, 96, STRIPE_HEIGHT };
            GFX_Buffer_t buffer;
            if( GFX_Buffer_Init( &buffer, &area, s_stripe_memory, sizeof(s_stripe_memory) ) )
            {
                GFX_Buff_Bg( &buffer, C(0x182235) );
                GFX_Buff_Rect( &buffer, 0, 74, 90, 18, C(0x35d07f) );
                GFX_Buff_Circle( &buffer, 128, 84, 20, C(0xffc14a) );
                GFX_Flush( buffer.Buffer, area.X, area.Y, area.W, area.H );
            }
            GFX_Fill_Rect( C(0x4aa8ff), 10, 120, 152, 2 );
            GFX_Fill_Rect( C(0x4aa8ff), 10, 214, 152, 2 );
            break;
        }
        case 8: /* MCU internal Flash RGB565 MIRROR */
            GFX_Fill_Img( &GFX_Test_MCU_Landscape_Mirror, 54, 78 );
            GFX_Fill_Img( &GFX_Test_MCU_Landscape_Mirror, 18, 150 );
            GFX_Fill_Img( &GFX_Test_MCU_Landscape_Mirror, 90, 222 );
            break;
        default: /* MCU palette + Alpha, plus MCU 1-bpp BITMAP */
        {
            Area_t area = { 66, 62, 40, STRIPE_HEIGHT };
            uint16_t y;
            for( y = 62; y < 102; y = (uint16_t)(y + STRIPE_HEIGHT) )
            {
                uint16_t h = (uint16_t)(((102u - y) < STRIPE_HEIGHT) ? (102u - y) : STRIPE_HEIGHT);
                GFX_Buffer_t buffer;
                GFX_Buff_Img_Adv_Para_t p;
                area.Y = (int16_t)y; area.H = h;
                if( !GFX_Buffer_Init( &buffer, &area, s_stripe_memory, sizeof(s_stripe_memory) ) ) break;
                GFX_Buff_Bg( &buffer, C(0x25507a) );
                memset( &p, 0, sizeof(p) );
                p.Buff = &buffer; p.Img = &GFX_Test_MCU_Gear_Palette2_Alpha2;
                p.X = 66; p.Y = 62; p.Anchor = Anchor_LT; p.Enable_Map = 0xffffffffu;
                GFX_Buff_Img_Adv( &p );
                GFX_Flush( buffer.Buffer, area.X, area.Y, area.W, area.H );
            }
            {
                GFX_Fill_Img_Adv_Para_t p;
                memset( &p, 0, sizeof(p) );
                p.Img = &GFX_Test_MCU_Mono_Bitmap1;
                p.X = 86; p.Y = 180; p.Anchor = Anchor_CM;
                p.Enable_Map = 0xffffffffu; p.Fore_Color = 0xffc14a; p.Back_Color = 0x10151f;
                GFX_Fill_Img_Adv( &p );
            }
            break;
        }
    }
}

static void keys_init( void )
{
    uint8_t i;
    Key_Set_t setting;
    Key_Init();
    Key_Set_Struct_Init( &setting );
    setting.Shake = 30;
    setting.Long_Enable = true;
    setting.Long = TEST_LONG_PRESS_MS;
    setting.Continue_Enable = false;
    setting.Interval_Enable = false;
    for( i = 0; i < 3u; ++i ) Key_Set( i, &setting );
    Key_Start();
}

static void process_keys( void )
{
    Key_Event_Node_t events[6];
    uint8_t i, count = Key_Buffer_Read( 6u, events );
    for( i = 0; i < count; ++i )
    {
        if( events[i].ID == KEY_ID_0 && events[i].Event == Key_Event_Short_Press )
        {
            s_auto_run = false;
            s_case = (uint8_t)((s_case + 1u) % TEST_CASE_COUNT);
            draw_case( s_case );
        }
        else if( events[i].ID == KEY_ID_0 && events[i].Event == Key_Event_Long_Press )
        {
            s_auto_run = !s_auto_run;
            s_auto_tick = Tick_Get_Tick();
            draw_case( s_case );
        }
        else if( events[i].ID == KEY_ID_1 && events[i].Event == Key_Event_Short_Press )
        {
            s_auto_run = false;
            s_case = 0;
            draw_case( s_case );
        }
        else if( events[i].ID == KEY_ID_2 && events[i].Event == Key_Event_Long_Press )
        {
            GFX_Fill_Bg( C(0x000000) );
            GFX_Set_Brightness( 0 );
            Delay_ms( 80 );
            Power_OFF;
        }
    }
}

void GFX_Hardware_Test_Run( void )
{
    GFX_Init_t init = { s_gfx_scratch, sizeof(s_gfx_scratch) };
    GFX_Init( &init );
    GFX_Set_Brightness( 255 );
    keys_init();
    s_flash_ok = BY25Q80_Is_Present();
    s_case = 0;
    s_auto_run = false;
    draw_case( s_case );
    for( ;; )
    {
        MCU_IDWG_Feed();
        process_keys();
        if( s_auto_run && Tick_Elaspe( s_auto_tick, TEST_AUTO_PERIOD_MS ) )
        {
            s_auto_tick = Tick_Get_Tick();
            s_case = (uint8_t)((s_case + 1u) % TEST_CASE_COUNT);
            draw_case( s_case );
        }
    }
}

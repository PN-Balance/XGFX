#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "GFX.h"
#include "GFX_Sim.h"

#define TEST_BUFFER_PIXELS 8192u

static GFX_Color_t s_WorkBuffer[ TEST_BUFFER_PIXELS ] ;
static unsigned int s_TestsRun ;
static unsigned int s_TestsFailed ;

static void reset_gfx( void )
{
    GFX_Init_t init = {
        .Buffer = s_WorkBuffer,
        .Size = sizeof( s_WorkBuffer )
    } ;
    memset( s_WorkBuffer, 0, sizeof( s_WorkBuffer ) ) ;
    GFX_Init( &init ) ;
}

static GFX_Color_t pixel_at( uint16_t x, uint16_t y )
{
    return GFX_Sim_Get_Framebuffer()[ (uint32_t)y * GFX_Sim_Get_Width() + x ] ;
}

static int expect_color( const char * label, uint16_t x, uint16_t y, GFX_Color_t expected )
{
    GFX_Color_t actual = pixel_at( x, y ) ;
    if( actual == expected ) return 1 ;

    fprintf( stderr, "    %s at (%u,%u): expected 0x%X, got 0x%X\n",
             label, x, y, (unsigned int)expected, (unsigned int)actual ) ;
    return 0 ;
}

static int expect_buffer_color( const char * label, const GFX_Color_t * buffer,
                                uint16_t width, uint16_t x, uint16_t y,
                                GFX_Color_t expected )
{
    GFX_Color_t actual = buffer[ (uint32_t)y * width + x ] ;
    if( actual == expected ) return 1 ;

    fprintf( stderr, "    %s at buffer (%u,%u): expected 0x%X, got 0x%X\n",
             label, x, y, (unsigned int)expected, (unsigned int)actual ) ;
    return 0 ;
}

#define RUN_TEST(test_fn) do {                                      \
    int passed ;                                                     \
    s_TestsRun++ ;                                                   \
    printf( "[ RUN      ] %s\n", #test_fn ) ;                       \
    passed = test_fn() ;                                             \
    if( passed ) {                                                   \
        printf( "[       OK ] %s\n", #test_fn ) ;                   \
    } else {                                                         \
        s_TestsFailed++ ;                                            \
        printf( "[  FAILED  ] %s\n", #test_fn ) ;                   \
    }                                                                \
} while( 0 )

static int test_uninitialized_and_null_calls_are_safe( void )
{
    GFX_Buffer_t invalid_buffer = { 0 } ;

    GFX_Fill_Bg( 0 ) ;
    GFX_Fill_Point( 0, 0, 0 ) ;
    GFX_Fill_Rect( 0, 0, 0, 1, 1 ) ;
    GFX_Fill_Circle( 0, 0, 0, 1, 0 ) ;
    GFX_Fill_Img( NULL, 0, 0 ) ;
    GFX_Fill_Img_Adv( NULL ) ;
    GFX_Flush( NULL, 0, 0, 1, 1 ) ;

    GFX_Buff_Bg( NULL, 0 ) ;
    GFX_Buff_Bg( &invalid_buffer, 0 ) ;
    GFX_Buff_Rect( NULL, 0, 0, 1, 1, 0 ) ;
    GFX_Buff_Circle( NULL, 0, 0, 1, 0 ) ;
    GFX_Buff_Img( NULL, NULL, 0, 0 ) ;
    GFX_Buff_Img_Adv( NULL ) ;

    return GFX_Get_Width() == 0 && GFX_Get_Height() == 0 ;
}

static int test_area_safety_and_large_edges( void )
{
    Area_t negative = { -5, -5, 2, 2 } ;
    Area_t container = { -10, -10, 20, 20 } ;
    Area_t edge_a = { 32760, 0, 20, 10 } ;
    Area_t edge_b = { 32765, 0, 10, 10 } ;
    Area_t invalid = { 0, 0, 0, 1 } ;
    Area_t intersection ;

    Area_Move( NULL, Anchor_LT, 0, 0 ) ;
    Area_Move_Offset( NULL, 1, 1 ) ;
    Area_Inflate( NULL, 1, Anchor_CM ) ;

    if( Area_Is_Valid( NULL ) || Area_Over( &invalid, &container ) ) return 0 ;
    if( !Area_In( &negative, &container ) || !Area_Contain( &negative, &container ) ) return 0 ;
    if( Area_Com( NULL, &container, &intersection ) || Area_Com( &negative, &container, NULL ) ) return 0 ;
    if( !Area_Com( &edge_a, &edge_b, &intersection ) ) return 0 ;

    return intersection.X == 32765 && intersection.Y == 0 &&
           intersection.W == 10 && intersection.H == 10 &&
           Area_Equal( &negative, &negative ) ;
}

static int test_initialization_and_color_conversion( void )
{
    reset_gfx() ;

    if( GFX_Get_Width() != 240 || GFX_Get_Height() != 320 ) {
        fprintf( stderr, "    unexpected dimensions: %d x %d\n",
                 GFX_Get_Width(), GFX_Get_Height() ) ;
        return 0 ;
    }
    if( GFX_Color_Convert( 0xFF0000u ) != (GFX_Color_t)0xF800u ) return 0 ;
    if( GFX_Color_Convert( 0x00FF00u ) != (GFX_Color_t)0x07E0u ) return 0 ;
    if( GFX_Color_Convert( 0x0000FFu ) != (GFX_Color_t)0x001Fu ) return 0 ;
    return 1 ;
}

static int test_fill_primitives_and_clipping( void )
{
    const GFX_Color_t black = GFX_Color_Convert( 0x000000u ) ;
    const GFX_Color_t red = GFX_Color_Convert( 0xFF0000u ) ;
    const GFX_Color_t green = GFX_Color_Convert( 0x00FF00u ) ;
    const GFX_Color_t blue = GFX_Color_Convert( 0x0000FFu ) ;

    reset_gfx() ;
    GFX_Fill_Bg( black ) ;
    GFX_Fill_Point( 5, 7, red ) ;
    GFX_Fill_Point( -1, 7, green ) ;
    GFX_Fill_Rect( green, -2, -1, 5, 4 ) ;
    GFX_Fill_Rect( blue, 238, 318, 5, 5 ) ;
    GFX_Fill_Circle( red, -1, -1, 3, 0 ) ;

    return expect_color( "core-clipped top-left circle", 0, 0, red ) &&
           expect_color( "circle outside", 2, 0, green ) &&
           expect_color( "clipped top-left rectangle edge", 2, 2, green ) &&
           expect_color( "rectangle outside", 3, 2, black ) &&
           expect_color( "point", 5, 7, red ) &&
           expect_color( "clipped bottom-right rectangle", 239, 319, blue ) ;
}

static int test_flush_clipping_preserves_source_stride( void )
{
    const GFX_Color_t black = GFX_Color_Convert( 0x000000u ) ;
    const GFX_Color_t a = GFX_Color_Convert( 0x110000u ) ;
    const GFX_Color_t b = GFX_Color_Convert( 0x220000u ) ;
    const GFX_Color_t c = GFX_Color_Convert( 0x330000u ) ;
    const GFX_Color_t d = GFX_Color_Convert( 0x440000u ) ;
    GFX_Color_t source[ 4 ] = { a, b, c, d } ;

    reset_gfx() ;
    GFX_Fill_Bg( black ) ;
    GFX_Flush( source, -1, -1, 2, 2 ) ;

    return expect_color( "flush clipped source", 0, 0, d ) &&
           expect_color( "flush neighbor unchanged", 1, 0, black ) ;
}

static int test_buffer_primitives( void )
{
    const GFX_Color_t base = GFX_Color_Convert( 0x101010u ) ;
    const GFX_Color_t red = GFX_Color_Convert( 0xFF0000u ) ;
    const GFX_Color_t yellow = GFX_Color_Convert( 0xFFFF00u ) ;
    GFX_Color_t pixels[ 8 * 6 ] ;
    GFX_Buffer_t buffer = {
        .Area = { 10, 20, 8, 6 },
        .Buffer = pixels
    } ;

    reset_gfx() ;
    GFX_Buff_Bg( &buffer, base ) ;
    GFX_Buff_Rect( &buffer, 8, 21, 4, 3, red ) ;
    GFX_Buff_Circle( &buffer, 15, 23, 2, yellow ) ;

    return expect_buffer_color( "buffer background", pixels, 8, 7, 0, base ) &&
           expect_buffer_color( "buffer clipped rectangle", pixels, 8, 0, 1, red ) &&
           expect_buffer_color( "buffer rectangle edge", pixels, 8, 1, 3, red ) &&
           expect_buffer_color( "buffer circle center", pixels, 8, 5, 3, yellow ) &&
           expect_buffer_color( "buffer circle exterior", pixels, 8, 7, 5, base ) ;
}

static int test_buffer_constructor_alignment_and_capacity( void )
{
    enum { WIDTH = 3, HEIGHT = 2 } ;
    uint8_t storage[ WIDTH * HEIGHT * sizeof( GFX_Color_t ) + _Alignof( GFX_Color_t ) ] ;
    uint8_t rejected_storage[ sizeof( GFX_Color_t ) * 2u + _Alignof( GFX_Color_t ) ] ;
    uint8_t * raw = storage ;
    uint8_t * rejected_raw = rejected_storage ;
    Area_t area = { 7, 9, WIDTH, HEIGHT } ;
    GFX_Buffer_t buffer = { 0 } ;
    GFX_Buffer_t rejected = { 0 } ;
    const GFX_Color_t color = GFX_Color_Convert( 0x336699u ) ;

    if( _Alignof( GFX_Color_t ) > 1u )
    {
        while( ( (uintptr_t)raw % _Alignof( GFX_Color_t ) ) == 0u ) raw++ ;
        while( ( (uintptr_t)rejected_raw % _Alignof( GFX_Color_t ) ) == 0u ) rejected_raw++ ;
    }

    memset( rejected_storage, 0xA5, sizeof( rejected_storage ) ) ;
    rejected.Area = (Area_t){ 0, 0, 1, 1 } ;
    rejected.Buffer = (GFX_Color_t *)rejected_raw ;
    reset_gfx() ;
    GFX_Buff_Bg( &rejected, color ) ;
    if( _Alignof( GFX_Color_t ) > 1u && rejected_raw[ 0 ] != 0xA5u ) return 0 ;

    uint32_t raw_offset = (uint32_t)( raw - storage ) ;
    if( !GFX_Buffer_Init( &buffer, &area, raw,
                          (uint32_t)sizeof( storage ) - raw_offset ) ) return 0 ;
    if( (uintptr_t)buffer.Buffer % _Alignof( GFX_Color_t ) != 0u ) return 0 ;
    if( !Area_Equal( &buffer.Area, &area ) ) return 0 ;

    GFX_Buff_Bg( &buffer, color ) ;
    for( uint16_t i = 0 ; i < WIDTH * HEIGHT ; i++ )
        if( buffer.Buffer[ i ] != color ) return 0 ;

    if( GFX_Buffer_Init( &buffer, &area, raw, sizeof( GFX_Color_t ) ) ) return 0 ;
    return buffer.Buffer == NULL && !Area_Is_Valid( &buffer.Area ) ;
}

static int test_mirror_image_and_clipping( void )
{
    const GFX_Color_t black = GFX_Color_Convert( 0x000000u ) ;
    GFX_Color_t image_pixels[ 6 ] = {
        GFX_Color_Convert( 0xFF0000u ), GFX_Color_Convert( 0x00FF00u ), GFX_Color_Convert( 0x0000FFu ),
        GFX_Color_Convert( 0xFFFF00u ), GFX_Color_Convert( 0x00FFFFu ), GFX_Color_Convert( 0xFF00FFu )
    } ;
    GFX_Img_t image = {
        .W = 3, .H = 2,
        .Color_Type = GFX_COLOR_TYPE_MIRROR,
        .Color_Save_Way = GFX_Save_Way_MCU,
        .Color_Bits_Per_Pix = 0,
        .Color_Save_Info = { .C_Array = (uint8_t *)image_pixels }
    } ;

    reset_gfx() ;
    GFX_Fill_Bg( black ) ;
    GFX_Fill_Img( &image, -1, 3 ) ;

    return expect_color( "mirror clipped row 0 col 1", 0, 3, image_pixels[ 1 ] ) &&
           expect_color( "mirror clipped row 0 col 2", 1, 3, image_pixels[ 2 ] ) &&
           expect_color( "mirror clipped row 1 col 1", 0, 4, image_pixels[ 4 ] ) &&
           expect_color( "mirror outside", 2, 3, black ) ;
}

static int test_bitmap_image_row_alignment( void )
{
    const GFX_Color_t black = GFX_Color_Convert( 0x000000u ) ;
    const GFX_Color_t white = GFX_Color_Convert( 0xFFFFFFu ) ;
    static const uint8_t bitmap[] = {
        0x15u, /* 1 0 1 0 1 */
        0x0Au  /* 0 1 0 1 0 */
    } ;
    GFX_Img_t image = {
        .W = 5, .H = 2,
        .Color_Type = GFX_COLOR_TYPE_BITMAP,
        .Color_Save_Way = GFX_Save_Way_MCU,
        .Color_Bits_Per_Pix = GFX_BPP_1,
        .Color_Save_Info = { .C_Array = (uint8_t *)bitmap }
    } ;

    reset_gfx() ;
    GFX_Fill_Bg( black ) ;
    GFX_Fill_Img( &image, 10, 10 ) ;

    if( !expect_color( "bitmap row 0 bit 0", 10, 10, white ) ||
        !expect_color( "bitmap row 0 bit 1", 11, 10, black ) ||
        !expect_color( "bitmap row 0 bit 4", 14, 10, white ) ||
        !expect_color( "bitmap row 1 bit 0", 10, 11, black ) ||
        !expect_color( "bitmap row 1 bit 1", 11, 11, white ) ) return 0 ;

    GFX_Fill_Bg( black ) ;
    GFX_Fill_Img( &image, 10, -1 ) ;
    return expect_color( "bitmap top clipping uses second source row", 10, 0, black ) &&
           expect_color( "bitmap top clipping bit 1", 11, 0, white ) ;
}

typedef struct {
    uint8_t pixels[ 2 ] ;
    GFX_Color_t palette[ 4 ] ;
} PaletteImageData ;

static int test_palette_image( void )
{
    const GFX_Color_t black = GFX_Color_Convert( 0x000000u ) ;
    PaletteImageData data = {
        .pixels = { 0xE4u, 0x1Bu }, /* rows: 0,1,2,3 and 3,2,1,0 */
        .palette = {
            GFX_Color_Convert( 0x100000u ),
            GFX_Color_Convert( 0x001000u ),
            GFX_Color_Convert( 0x000010u ),
            GFX_Color_Convert( 0xFFFFFFu )
        }
    } ;
    GFX_Img_t image = {
        .W = 4, .H = 2,
        .Color_Type = GFX_COLOR_TYPE_BITMAP_WITH_PALETTE,
        .Color_Save_Way = GFX_Save_Way_MCU,
        .Color_Bits_Per_Pix = GFX_BPP_2,
        .Color_Save_Info = { .C_Array = data.pixels }
    } ;

    reset_gfx() ;
    GFX_Fill_Bg( black ) ;
    GFX_Fill_Img( &image, 20, 20 ) ;

    return expect_color( "palette index 0", 20, 20, data.palette[ 0 ] ) &&
           expect_color( "palette index 3", 23, 20, data.palette[ 3 ] ) &&
           expect_color( "palette reverse index 3", 20, 21, data.palette[ 3 ] ) &&
           expect_color( "palette reverse index 0", 23, 21, data.palette[ 0 ] ) ;
}

static int test_unaligned_bitmap_scratch_and_embedded_palette( void )
{
    enum { INDEX_BYTES = 3, PALETTE_COUNT = 2 } ;
    uint8_t storage[ INDEX_BYTES + PALETTE_COUNT * sizeof( GFX_Color_t ) +
                     _Alignof( GFX_Color_t ) ] ;
    uint8_t * image_data = storage ;
    const GFX_Color_t dark = GFX_Color_Convert( 0x102030u ) ;
    const GFX_Color_t light = GFX_Color_Convert( 0xE0D0C0u ) ;
    const GFX_Color_t palette[ PALETTE_COUNT ] = { dark, light } ;
    GFX_Color_t copied[ PALETTE_COUNT ] = { 0 } ;
    uint8_t work_storage[ 128 + _Alignof( GFX_Color_t ) ] ;
    uint8_t * work = work_storage ;

    /* 强制让紧跟 3 字节索引流的默认色板落在非对齐地址。 */
    if( _Alignof( GFX_Color_t ) > 1u )
    {
        while( ( (uintptr_t)( image_data + INDEX_BYTES ) % _Alignof( GFX_Color_t ) ) == 0u )
            image_data++ ;
        while( ( (uintptr_t)work % _Alignof( GFX_Color_t ) ) == 0u ) work++ ;
    }

    memset( image_data, 0, INDEX_BYTES ) ;
    image_data[ 0 ] = 0x01u ; /* 第一个像素索引 1，其余 16 个像素索引 0。 */
    memcpy( image_data + INDEX_BYTES, palette, sizeof( palette ) ) ;

    GFX_Img_t image = {
        .W = 17, .H = 1,
        .Color_Type = GFX_COLOR_TYPE_BITMAP_WITH_PALETTE,
        .Color_Save_Way = GFX_Save_Way_MCU,
        .Color_Bits_Per_Pix = GFX_BPP_1,
        .Color_Save_Info = { .C_Array = image_data }
    } ;

    reset_gfx() ;
    GFX_Set_Buffer( work, (uint32_t)sizeof( work_storage ) - (uint32_t)( work - work_storage ) ) ;
    GFX_Img_Copy_Palette( &image, copied ) ;
    GFX_Fill_Img( &image, 30, 30 ) ;

    return copied[ 0 ] == dark && copied[ 1 ] == light &&
           expect_color( "unaligned palette first index", 30, 30, light ) &&
           expect_color( "aligned scratch final index", 46, 30, dark ) ;
}

static int test_buffer_image_and_flush( void )
{
    const GFX_Color_t black = GFX_Color_Convert( 0x000000u ) ;
    const GFX_Color_t blue = GFX_Color_Convert( 0x0000FFu ) ;
    const GFX_Color_t red = GFX_Color_Convert( 0xFF0000u ) ;
    GFX_Color_t source[ 4 ] = { red, blue, blue, red } ;
    GFX_Color_t pixels[ 4 * 4 ] ;
    GFX_Buffer_t buffer = {
        .Area = { 0, 0, 4, 4 },
        .Buffer = pixels
    } ;
    GFX_Img_t image = {
        .W = 2, .H = 2,
        .Color_Type = GFX_COLOR_TYPE_MIRROR,
        .Color_Save_Way = GFX_Save_Way_MCU,
        .Color_Save_Info = { .C_Array = (uint8_t *)source }
    } ;

    reset_gfx() ;
    GFX_Fill_Bg( black ) ;
    GFX_Buff_Bg( &buffer, black ) ;
    GFX_Buff_Img( &buffer, &image, 3, 3 ) ;
    GFX_Flush( pixels, 30, 30, 4, 4 ) ;

    return expect_color( "buffer image clipped pixel", 33, 33, red ) &&
           expect_color( "buffer image untouched pixel", 32, 33, black ) ;
}

static int test_buffer_manager_boundaries( void )
{
    uint8_t storage[ 12 ] = { 0 } ;
    GFX_Buffer_Manager_t manager ;

    GFX_Buffer_Manager_Init( &manager, storage, 2, 6 ) ;
    if( GFX_Buffer_Manager_Alloc( &manager, 2 ) != storage ) return 0 ;
    if( GFX_Buffer_Manager_Alloc( &manager, 3 ) != storage + 4 ) return 0 ;
    if( GFX_Buffer_Manager_Alloc( &manager, 1 ) != storage + 10 ) return 0 ;
    if( GFX_Buffer_Manager_Alloc( &manager, 1 ) != NULL ) return 0 ;

    GFX_Buffer_Manager_Clear( &manager ) ;
    return GFX_Buffer_Manager_Alloc( &manager, 6 ) == storage &&
           GFX_Buffer_Manager_Alloc( &manager, 1 ) == NULL ;
}

static int test_buffer_cutter_coverage( void )
{
    Area_t source = { 10, 20, 5, 3 } ;
    Area_t part ;
    const Area_t expected[ 3 ] = {
        { 10, 20, 5, 1 },
        { 10, 21, 5, 1 },
        { 10, 22, 5, 1 }
    } ;

    GFX_Buffer_Cutter_Init( &source, 6 ) ;
    for( unsigned int i = 0 ; i < 3 ; i++ )
    {
        if( !GFX_Buffer_Cutter_Cut( &part ) ||
            part.X != expected[i].X || part.Y != expected[i].Y ||
            part.W != expected[i].W || part.H != expected[i].H ) return 0 ;
    }
    if( GFX_Buffer_Cutter_Cut( &part ) ) return 0 ;

    GFX_Buffer_Cutter_Init( &source, 0 ) ;
    if( GFX_Buffer_Cutter_Cut( &part ) ) return 0 ;

    GFX_Buffer_Cutter_Init( &source, 2 ) ;
    if( !GFX_Buffer_Cutter_Cut( &part ) || part.X != 10 || part.Y != 20 || part.W != 2 || part.H != 1 ) return 0 ;
    if( !GFX_Buffer_Cutter_Cut( &part ) || part.X != 12 || part.Y != 20 || part.W != 2 || part.H != 1 ) return 0 ;
    if( !GFX_Buffer_Cutter_Cut( &part ) || part.X != 14 || part.Y != 20 || part.W != 1 || part.H != 1 ) return 0 ;
    return 1 ;
}

static int test_cut_self_anchor_reference( void )
{
    const GFX_Color_t black = GFX_Color_Convert( 0x000000u ) ;
    GFX_Color_t source[ 4 * 4 ] ;
    GFX_Color_t pixels[ 4 * 4 ] ;
    for( uint16_t i = 0 ; i < 16 ; i++ ) source[ i ] = (GFX_Color_t)( i + 1 ) ;

    GFX_Img_t image = {
        .W = 4, .H = 4,
        .Color_Type = GFX_COLOR_TYPE_MIRROR,
        .Color_Save_Way = GFX_Save_Way_MCU,
        .Color_Save_Info = { .C_Array = (uint8_t *)source }
    } ;
    GFX_Fill_Img_Adv_Para_t fill_parameters = {
        .Img = &image,
        .X = 10, .Y = 10, .Anchor = Anchor_LT,
        .Anchor_Use_Cut_Self = true,
        .Cut_Self_Enable = true,
        .Cut_Self = { 1, 1, 2, 2 },
        .Enable_Map = 0xFFFFFFFFu
    } ;

    reset_gfx() ;
    GFX_Fill_Bg( black ) ;
    GFX_Fill_Img_Adv( &fill_parameters ) ;
    if( !expect_color( "cut LT anchor places cut origin", 10, 10, source[ 5 ] ) ||
        !expect_color( "cut LT anchor places cut end", 11, 11, source[ 10 ] ) ||
        !expect_color( "cut LT anchor does not expose image origin", 9, 9, black ) ) return 0 ;

    GFX_Fill_Bg( black ) ;
    fill_parameters.X = 20 ;
    fill_parameters.Y = 20 ;
    fill_parameters.Anchor = Anchor_RB ;
    GFX_Fill_Img_Adv( &fill_parameters ) ;
    if( !expect_color( "cut RB anchor places cut origin", 19, 19, source[ 5 ] ) ||
        !expect_color( "cut RB anchor places cut anchor", 20, 20, source[ 10 ] ) ) return 0 ;

    GFX_Buffer_t buffer = { .Area = { 30, 40, 4, 4 }, .Buffer = pixels } ;
    GFX_Buff_Img_Adv_Para_t buff_parameters = {
        .Buff = &buffer, .Img = &image,
        .X = 30, .Y = 40, .Anchor = Anchor_LT,
        .Anchor_Use_Cut_Self = true,
        .Cut_Self_Enable = true,
        .Cut_Self = { 1, 1, 2, 2 },
        .Enable_Map = 0xFFFFFFFFu
    } ;

    GFX_Buff_Bg( &buffer, black ) ;
    GFX_Buff_Img_Adv( &buff_parameters ) ;
    if( !expect_buffer_color( "buffer cut anchor places cut origin", pixels, 4, 0, 0, source[ 5 ] ) ||
        !expect_buffer_color( "buffer cut anchor places cut end", pixels, 4, 1, 1, source[ 10 ] ) ) return 0 ;

    GFX_Buff_Bg( &buffer, black ) ;
    buff_parameters.Cut_Self_Enable = false ;
    GFX_Buff_Img_Adv( &buff_parameters ) ;
    return expect_buffer_color( "cut anchor requires enabled cut", pixels, 4, 0, 0, black ) ;
}

static int test_flash_image_paths( void )
{
    const GFX_Color_t black = GFX_Color_Convert( 0x000000u ) ;
    GFX_Color_t mirror_data[ 4 ] = {
        GFX_Color_Convert( 0xFF0000u ), GFX_Color_Convert( 0x00FF00u ),
        GFX_Color_Convert( 0x0000FFu ), GFX_Color_Convert( 0xFFFFFFu )
    } ;
    GFX_Img_t mirror = {
        .W = 2, .H = 2,
        .Color_Type = GFX_COLOR_TYPE_MIRROR,
        .Color_Save_Way = GFX_Save_Way_Flash,
        .Color_Save_Info = { .Flash_Addr = 100 }
    } ;
    GFX_Color_t buffer_pixels[ 4 ] = { black, black, black, black } ;
    GFX_Buffer_t buffer = { .Area = { 0, 0, 2, 2 }, .Buffer = buffer_pixels } ;

    reset_gfx() ;
    if( !GFX_Sim_Flash_Write( 100, mirror_data, sizeof( mirror_data ) ) ) return 0 ;
    GFX_Fill_Bg( black ) ;
    GFX_Fill_Img( &mirror, 40, 40 ) ;
    GFX_Buff_Img( &buffer, &mirror, 0, 0 ) ;

    return expect_color( "flash mirror direct", 41, 41, mirror_data[ 3 ] ) &&
           expect_buffer_color( "flash mirror buffer", buffer_pixels, 2, 0, 1, mirror_data[ 2 ] ) ;
}

static int test_flash_palette_image_path( void )
{
    const uint8_t packed_pixels = 0xE4u ; /* indexes 0,1,2,3 */
    GFX_Color_t palette[ 4 ] = {
        GFX_Color_Convert( 0x100000u ), GFX_Color_Convert( 0x001000u ),
        GFX_Color_Convert( 0x000010u ), GFX_Color_Convert( 0xFFFFFFu )
    } ;
    GFX_Img_t image = {
        .W = 4, .H = 1,
        .Color_Type = GFX_COLOR_TYPE_BITMAP_WITH_PALETTE,
        .Color_Save_Way = GFX_Save_Way_Flash,
        .Color_Bits_Per_Pix = GFX_BPP_2,
        .Color_Save_Info = { .Flash_Addr = 200 }
    } ;

    reset_gfx() ;
    if( !GFX_Sim_Flash_Write( 200, &packed_pixels, sizeof( packed_pixels ) ) ) return 0 ;
    if( !GFX_Sim_Flash_Write( 201, palette, sizeof( palette ) ) ) return 0 ;
    GFX_Fill_Img( &image, 50, 50 ) ;

    return expect_color( "flash palette index 0", 50, 50, palette[ 0 ] ) &&
           expect_color( "flash palette index 2", 52, 50, palette[ 2 ] ) &&
           expect_color( "flash palette index 3", 53, 50, palette[ 3 ] ) ;
}

static int test_draw_direction_dimensions( void )
{
    reset_gfx() ;
    GFX_Set_Draw_Direction( Orth_Dir_90 ) ;
    if( GFX_Get_Draw_Direction() != Orth_Dir_90 || GFX_Get_Width() != 320 || GFX_Get_Height() != 240 ) return 0 ;
    GFX_Set_Draw_Direction( Orth_Dir_180 ) ;
    if( GFX_Get_Width() != 240 || GFX_Get_Height() != 320 ) return 0 ;
    GFX_Set_Draw_Direction( Orth_Dir_270 ) ;
    return GFX_Get_Width() == 320 && GFX_Get_Height() == 240 ;
}

static int test_fill_image_advanced( void )
{
    const GFX_Color_t black = GFX_Color_Convert( 0x000000u ) ;
    const GFX_Color_t green = GFX_Color_Convert( 0x00FF00u ) ;
    GFX_Color_t palette[ 2 ] = {
        GFX_Color_Convert( 0x0000FFu ),
        GFX_Color_Convert( 0xFF0000u )
    } ;
    uint8_t bits[ 1 ] = { 0x02u } ;
    GFX_Img_t image = {
        .W = 2, .H = 1,
        .Color_Type = GFX_COLOR_TYPE_BITMAP,
        .Color_Save_Way = GFX_Save_Way_MCU,
        .Color_Bits_Per_Pix = GFX_BPP_1,
        .Color_Save_Info = { .C_Array = bits }
    } ;
    GFX_Fill_Img_Adv_Para_t parameters = {
        .Img = &image, .X = 5, .Y = 5, .Anchor = Anchor_RB,
        .Palette = palette, .Enable_Map = 0x02u,
        .Fore_Color = 0xFFFFFFu, .Back_Color = 0x00FF00u
    } ;

    reset_gfx() ;
    GFX_Fill_Bg( black ) ;
    GFX_Fill_Img_Adv( &parameters ) ;

    return expect_color( "advanced fill masked background", 4, 5, green ) &&
           expect_color( "advanced fill enabled index", 5, 5, palette[ 1 ] ) ;
}

static int test_buffer_image_advanced( void )
{
    const GFX_Color_t base = GFX_Color_Convert( 0x101010u ) ;
    const GFX_Color_t green = GFX_Color_Convert( 0x00FF00u ) ;
    GFX_Color_t palette[ 2 ] = {
        GFX_Color_Convert( 0x0000FFu ),
        GFX_Color_Convert( 0xFF0000u )
    } ;
    uint8_t bits[ 1 ] = { 0x02u } ;
    GFX_Color_t pixels[ 6 * 4 ] ;
    GFX_Buffer_t buffer = { .Area = { 10, 20, 6, 4 }, .Buffer = pixels } ;
    GFX_Img_t image = {
        .W = 2, .H = 1,
        .Color_Type = GFX_COLOR_TYPE_BITMAP,
        .Color_Save_Way = GFX_Save_Way_MCU,
        .Color_Bits_Per_Pix = GFX_BPP_1,
        .Color_Save_Info = { .C_Array = bits }
    } ;
    GFX_Buff_Img_Adv_Para_t parameters = {
        .Buff = &buffer, .Img = &image,
        .X = 12, .Y = 21, .Anchor = Anchor_CM,
        .Palette = palette, .Enable_Map = 0x02u,
        .Fore_Color = 0xFFFFFFu, .Back_Color = 0x00FF00u
    } ;

    reset_gfx() ;
    GFX_Buff_Bg( &buffer, base ) ;
    GFX_Buff_Img_Adv( &parameters ) ;

    return expect_buffer_color( "advanced buffer masked background", pixels, 6, 1, 1, green ) &&
           expect_buffer_color( "advanced buffer enabled index", pixels, 6, 2, 1, palette[ 1 ] ) &&
           expect_buffer_color( "advanced buffer outside", pixels, 6, 0, 0, base ) ;
}

static int test_invalid_image_inputs_are_ignored( void )
{
    const GFX_Color_t base = GFX_Color_Convert( 0x123456u ) ;
    uint8_t bits[ 1 ] = { 0xFFu } ;
    GFX_Img_t invalid = {
        .W = 1, .H = 1,
        .Color_Type = GFX_COLOR_TYPE_BITMAP,
        .Color_Save_Way = GFX_Save_Way_MCU,
        .Color_Bits_Per_Pix = 0,
        .Color_Save_Info = { .C_Array = bits }
    } ;

    reset_gfx() ;
    GFX_Fill_Bg( base ) ;
    GFX_Fill_Img( &invalid, 0, 0 ) ;
    invalid.Color_Bits_Per_Pix = GFX_BPP_1 ;
    invalid.Color_Save_Info.C_Array = NULL ;
    GFX_Fill_Img( &invalid, 0, 0 ) ;
    GFX_Fill_Img( NULL, 0, 0 ) ;

    return expect_color( "invalid image leaves framebuffer unchanged", 0, 0, base ) ;
}

static int test_buffer_palette_alpha_mcu( void )
{
    const GFX_Color_t black = GFX_Color_Convert( 0x000000u ) ;
    const GFX_Color_t green = GFX_Color_Convert( 0x00FF00u ) ;
    const GFX_Color_t white = GFX_Color_Convert( 0xFFFFFFu ) ;
    GFX_Color_t palette[ 2 ] = { black, white } ;
    uint8_t color_bits[ 2 ] = { 0x0Fu, 0x0Fu } ;
    uint8_t alpha_bits[ 2 ] = {
        0xE4u, /* alpha values 0,1,2,3 */
        0x1Bu  /* alpha values 3,2,1,0 */
    } ;
    GFX_Color_t pixels[ 4 * 2 ] ;
    GFX_Buffer_t buffer = { .Area = { 0, 0, 4, 2 }, .Buffer = pixels } ;
    GFX_Img_t image = {
        .W = 4, .H = 2,
        .Color_Type = GFX_COLOR_TYPE_BITMAP_WITH_PALETTE,
        .Color_Save_Way = GFX_Save_Way_MCU,
        .Color_Bits_Per_Pix = GFX_BPP_1,
        .Alpha_Enable = 1,
        .Alpha_Save_Way = GFX_Save_Way_MCU,
        .Alpha_Bits_Per_Pix = GFX_BPP_2,
        .Color_Save_Info = { .C_Array = color_bits },
        .Alpha_Save_Info = { .C_Array = alpha_bits }
    } ;
    GFX_Buff_Img_Adv_Para_t parameters = {
        .Buff = &buffer, .Img = &image,
        .X = 0, .Y = 0, .Anchor = Anchor_LT,
        .Palette = palette, .Enable_Map = 0xFFFFFFFFu,
        .Fore_Color = 0xFFFFFFu, .Back_Color = 0x000000u
    } ;

    reset_gfx() ;
    GFX_Buff_Bg( &buffer, black ) ;
    GFX_Buff_Img_Adv( &parameters ) ;

    if( !expect_buffer_color( "alpha zero preserves destination", pixels, 4, 0, 0, black ) ||
        !expect_buffer_color( "alpha one blends", pixels, 4, 1, 0,
                              GFX_Color_AlphaBlend( black, white, 1, 3 ) ) ||
        !expect_buffer_color( "alpha two blends", pixels, 4, 2, 0,
                              GFX_Color_AlphaBlend( black, white, 2, 3 ) ) ||
        !expect_buffer_color( "alpha max replaces destination", pixels, 4, 3, 0, white ) ) return 0 ;

    GFX_Buff_Bg( &buffer, black ) ;
    parameters.X = -1 ;
    parameters.Y = -1 ;
    GFX_Buff_Img_Adv( &parameters ) ;
    if( !expect_buffer_color( "alpha clipping selects source row and column", pixels, 4, 0, 0,
                              GFX_Color_AlphaBlend( black, white, 2, 3 ) ) ||
        !expect_buffer_color( "alpha clipping transparent tail", pixels, 4, 2, 0, black ) ) return 0 ;

    /* Buffer 是绿色，但开启纯色背景混合后，结果必须只以 Back_Color 黑色为背景。 */
    GFX_Buff_Bg( &buffer, green ) ;
    parameters.X = 0 ;
    parameters.Y = 0 ;
    parameters.Blend_Back_Color_Enable = true ;
    GFX_Buff_Img_Adv( &parameters ) ;

    return expect_buffer_color( "solid blend alpha zero writes back color", pixels, 4, 0, 0, black ) &&
           expect_buffer_color( "solid blend ignores buffer pixel", pixels, 4, 1, 0,
                                GFX_Color_AlphaBlend( black, white, 1, 3 ) ) &&
           expect_buffer_color( "solid blend alpha max writes source", pixels, 4, 3, 0, white ) ;
}

static int test_bitmap_independent_alpha_is_rejected( void )
{
    const GFX_Color_t base = GFX_Color_Convert( 0x123456u ) ;
    uint8_t color_bits[ 1 ] = { 0x01u } ;
    uint8_t alpha_bits[ 1 ] = { 0x01u } ;
    GFX_Color_t pixels[ 1 ] = { base } ;
    GFX_Buffer_t buffer = { .Area = { 0, 0, 1, 1 }, .Buffer = pixels } ;
    GFX_Img_t image = {
        .W = 1, .H = 1,
        .Color_Type = GFX_COLOR_TYPE_BITMAP,
        .Color_Save_Way = GFX_Save_Way_MCU,
        .Color_Bits_Per_Pix = GFX_BPP_1,
        .Alpha_Enable = 1,
        .Alpha_Save_Way = GFX_Save_Way_MCU,
        .Alpha_Bits_Per_Pix = GFX_BPP_1,
        .Color_Save_Info = { .C_Array = color_bits },
        .Alpha_Save_Info = { .C_Array = alpha_bits }
    } ;
    GFX_Buff_Img_Adv_Para_t buff_parameters = {
        .Buff = &buffer, .Img = &image,
        .X = 0, .Y = 0, .Anchor = Anchor_LT,
        .Enable_Map = 0xFFFFFFFFu,
        .Fore_Color = 0xFFFFFFu, .Back_Color = 0x000000u
    } ;
    GFX_Fill_Img_Adv_Para_t fill_parameters = {
        .Img = &image, .X = 0, .Y = 0, .Anchor = Anchor_LT,
        .Enable_Map = 0xFFFFFFFFu,
        .Fore_Color = 0xFFFFFFu, .Back_Color = 0x000000u
    } ;

    reset_gfx() ;
    GFX_Fill_Bg( base ) ;
    GFX_Fill_Img( &image, 0, 0 ) ;
    GFX_Fill_Img_Adv( &fill_parameters ) ;
    GFX_Buff_Img( &buffer, &image, 0, 0 ) ;
    GFX_Buff_Img_Adv( &buff_parameters ) ;

    return expect_color( "bitmap alpha rejected by direct drawing", 0, 0, base ) &&
           expect_buffer_color( "bitmap alpha rejected by buffer drawing", pixels, 1, 0, 0, base ) ;
}

static int test_buffer_advanced_alpha_flash( void )
{
    const GFX_Color_t blue = GFX_Color_Convert( 0x0000FFu ) ;
    GFX_Color_t colors[ 3 ] = {
        GFX_Color_Convert( 0xFF0000u ),
        GFX_Color_Convert( 0x00FF00u ),
        GFX_Color_Convert( 0xFFFFFFu )
    } ;
    uint8_t alpha_bits = 0x34u ; /* 2-bit values 0,1,3 */
    GFX_Color_t pixels[ 3 ] ;
    GFX_Buffer_t buffer = { .Area = { 0, 0, 3, 1 }, .Buffer = pixels } ;
    GFX_Img_t image = {
        .W = 3, .H = 1,
        .Color_Type = GFX_COLOR_TYPE_MIRROR,
        .Color_Save_Way = GFX_Save_Way_Flash,
        .Alpha_Enable = 1,
        .Alpha_Save_Way = GFX_Save_Way_Flash,
        .Alpha_Bits_Per_Pix = GFX_BPP_2,
        .Color_Save_Info = { .Flash_Addr = 300 },
        .Alpha_Save_Info = { .Flash_Addr = 400 }
    } ;
    GFX_Buff_Img_Adv_Para_t parameters = {
        .Buff = &buffer, .Img = &image,
        .X = 0, .Y = 0, .Anchor = Anchor_LT,
        .Enable_Map = 0xFFFFFFFFu
    } ;

    reset_gfx() ;
    if( !GFX_Sim_Flash_Write( 300, colors, sizeof( colors ) ) ||
        !GFX_Sim_Flash_Write( 400, &alpha_bits, sizeof( alpha_bits ) ) ) return 0 ;
    GFX_Buff_Bg( &buffer, blue ) ;
    GFX_Buff_Img_Adv( &parameters ) ;

    if( !expect_buffer_color( "flash alpha zero", pixels, 3, 0, 0, blue ) ||
        !expect_buffer_color( "flash alpha partial", pixels, 3, 1, 0,
                              GFX_Color_AlphaBlend( blue, colors[ 1 ], 1, 3 ) ) ||
        !expect_buffer_color( "flash alpha max", pixels, 3, 2, 0, colors[ 2 ] ) ) return 0 ;

    parameters.Blend_Back_Color_Enable = true ;
    parameters.Back_Color = 0x000000u ;
    GFX_Buff_Bg( &buffer, blue ) ;
    GFX_Buff_Img_Adv( &parameters ) ;

    return expect_buffer_color( "flash solid blend alpha zero", pixels, 3, 0, 0,
                                GFX_Color_Convert( 0x000000u ) ) &&
           expect_buffer_color( "flash solid blend ignores buffer pixel", pixels, 3, 1, 0,
                                GFX_Color_AlphaBlend( GFX_Color_Convert( 0x000000u ), colors[ 1 ], 1, 3 ) ) &&
           expect_buffer_color( "flash solid blend alpha max", pixels, 3, 2, 0, colors[ 2 ] ) ;
}

static int test_fill_image_advanced_cut_areas( void )
{
    const GFX_Color_t black = GFX_Color_Convert( 0x000000u ) ;
    GFX_Color_t source[ 4 * 4 ] ;
    for( uint16_t i = 0 ; i < 16 ; i++ ) source[ i ] = (GFX_Color_t)( i + 1 ) ;

    GFX_Img_t image = {
        .W = 4, .H = 4,
        .Color_Type = GFX_COLOR_TYPE_MIRROR,
        .Color_Save_Way = GFX_Save_Way_MCU,
        .Color_Save_Info = { .C_Array = (uint8_t *)source }
    } ;
    GFX_Fill_Img_Adv_Para_t parameters = {
        .Img = &image,
        .X = 10, .Y = 10, .Anchor = Anchor_LT,
        .Cut_Screen_Enable = true,
        .Cut_Screen = { 11, 11, 2, 3 },
        .Cut_Self_Enable = true,
        .Cut_Self = { 1, 0, 3, 3 },
        .Enable_Map = 0xFFFFFFFFu
    } ;

    reset_gfx() ;
    GFX_Fill_Bg( black ) ;
    GFX_Fill_Img_Adv( &parameters ) ;
    if( !expect_color( "combined cuts first source pixel", 11, 11, source[ 5 ] ) ||
        !expect_color( "combined cuts last source pixel", 12, 12, source[ 10 ] ) ||
        !expect_color( "combined cuts screen exclusion", 13, 11, black ) ||
        !expect_color( "combined cuts self exclusion", 11, 13, black ) ) return 0 ;

    GFX_Fill_Bg( black ) ;
    parameters.Cut_Screen_Enable = false ;
    GFX_Fill_Img_Adv( &parameters ) ;
    if( !expect_color( "self cut keeps selected content", 13, 12, source[ 11 ] ) ||
        !expect_color( "self cut removes left source column", 10, 10, black ) ||
        !expect_color( "self cut removes bottom source row", 11, 13, black ) ) return 0 ;

    GFX_Fill_Bg( black ) ;
    parameters.Cut_Screen_Enable = true ;
    parameters.Cut_Self_Enable = false ;
    GFX_Fill_Img_Adv( &parameters ) ;
    return expect_color( "screen cut keeps absolute region", 11, 13, source[ 13 ] ) &&
           expect_color( "screen cut removes left side", 10, 11, black ) &&
           expect_color( "screen cut removes right side", 13, 11, black ) ;
}

static int test_buffer_image_advanced_cut_areas( void )
{
    const GFX_Color_t black = GFX_Color_Convert( 0x000000u ) ;
    GFX_Color_t source[ 4 * 4 ] ;
    GFX_Color_t pixels[ 4 * 4 ] ;
    for( uint16_t i = 0 ; i < 16 ; i++ ) source[ i ] = (GFX_Color_t)( i + 1 ) ;

    GFX_Img_t image = {
        .W = 4, .H = 4,
        .Color_Type = GFX_COLOR_TYPE_MIRROR,
        .Color_Save_Way = GFX_Save_Way_MCU,
        .Color_Save_Info = { .C_Array = (uint8_t *)source }
    } ;
    GFX_Buffer_t buffer = { .Area = { 20, 30, 4, 4 }, .Buffer = pixels } ;
    GFX_Buff_Img_Adv_Para_t parameters = {
        .Buff = &buffer, .Img = &image,
        .X = 20, .Y = 30, .Anchor = Anchor_LT,
        .Cut_Screen_Enable = true,
        .Cut_Screen = { 22, 30, 2, 3 },
        .Cut_Self_Enable = true,
        .Cut_Self = { 1, 1, 3, 2 },
        .Enable_Map = 0xFFFFFFFFu
    } ;

    reset_gfx() ;
    GFX_Buff_Bg( &buffer, black ) ;
    GFX_Buff_Img_Adv( &parameters ) ;

    return expect_buffer_color( "buffer cuts first source pixel", pixels, 4, 2, 1, source[ 6 ] ) &&
           expect_buffer_color( "buffer cuts last source pixel", pixels, 4, 3, 2, source[ 11 ] ) &&
           expect_buffer_color( "buffer cuts self exclusion", pixels, 4, 1, 1, black ) &&
           expect_buffer_color( "buffer cuts screen exclusion", pixels, 4, 2, 0, black ) &&
           expect_buffer_color( "buffer cuts lower exclusion", pixels, 4, 2, 3, black ) ;
}

static void pack_test_values( uint8_t * bytes, uint16_t count, uint8_t bpp )
{
    uint16_t i ;
    memset( bytes, 0, 16 ) ;
    for( i = 0 ; i < count ; ++i )
    {
        uint8_t value = (uint8_t)(i & ((1u << bpp) - 1u)) ;
        uint8_t bit ;
        for( bit = 0 ; bit < bpp ; ++bit )
            if( value & (1u << bit) ) bytes[((uint32_t)i*bpp+bit)>>3] |= (uint8_t)(1u << (((uint32_t)i*bpp+bit)&7u)) ;
    }
}

static int test_packed_decoder_all_bpp( void )
{
    uint8_t packed[16] ;
    GFX_Color_t pixels[17] ;
    GFX_Color_t palette[32] ;
    Area_t area = {0,0,17,1} ;
    GFX_Buffer_t buffer ;
    uint8_t bpp ;
    reset_gfx() ;
    if( !GFX_Buffer_Init(&buffer,&area,pixels,sizeof(pixels)) ) return 0 ;
    for( bpp=1 ; bpp<=5 ; ++bpp )
    {
        uint16_t i ;
        GFX_Img_t image ;
        memset(&image,0,sizeof(image));
        pack_test_values(packed,17,bpp);
        for(i=0;i<(1u<<bpp);++i) palette[i]=(GFX_Color_t)(i+1u);
        image.W=17; image.H=1; image.Color_Type=GFX_COLOR_TYPE_BITMAP_WITH_PALETTE;
        image.Color_Save_Way=GFX_Save_Way_MCU; image.Color_Bits_Per_Pix=bpp;
        image.Color_Save_Info.C_Array=packed;
        memset(pixels,0,sizeof(pixels));
        {
            GFX_Buff_Img_Adv_Para_t p;
            memset(&p,0,sizeof(p));p.Buff=&buffer;p.Img=&image;p.Anchor=Anchor_LT;
            p.Enable_Map=0xffffffffu;p.Palette=palette;GFX_Buff_Img_Adv(&p);
        }
        for(i=0;i<17;++i) if(pixels[i]!=palette[i&((1u<<bpp)-1u)]) return 0;
    }
    return 1;
}

int main( void )
{
    RUN_TEST( test_uninitialized_and_null_calls_are_safe ) ;
    RUN_TEST( test_area_safety_and_large_edges ) ;
    RUN_TEST( test_initialization_and_color_conversion ) ;
    RUN_TEST( test_fill_primitives_and_clipping ) ;
    RUN_TEST( test_flush_clipping_preserves_source_stride ) ;
    RUN_TEST( test_buffer_primitives ) ;
    RUN_TEST( test_buffer_constructor_alignment_and_capacity ) ;
    RUN_TEST( test_mirror_image_and_clipping ) ;
    RUN_TEST( test_bitmap_image_row_alignment ) ;
    RUN_TEST( test_palette_image ) ;
    RUN_TEST( test_unaligned_bitmap_scratch_and_embedded_palette ) ;
    RUN_TEST( test_buffer_image_and_flush ) ;
    RUN_TEST( test_buffer_manager_boundaries ) ;
    RUN_TEST( test_buffer_cutter_coverage ) ;
    RUN_TEST( test_cut_self_anchor_reference ) ;
    RUN_TEST( test_flash_image_paths ) ;
    RUN_TEST( test_flash_palette_image_path ) ;
    RUN_TEST( test_draw_direction_dimensions ) ;
    RUN_TEST( test_fill_image_advanced ) ;
    RUN_TEST( test_buffer_image_advanced ) ;
    RUN_TEST( test_invalid_image_inputs_are_ignored ) ;
    RUN_TEST( test_buffer_palette_alpha_mcu ) ;
    RUN_TEST( test_bitmap_independent_alpha_is_rejected ) ;
    RUN_TEST( test_buffer_advanced_alpha_flash ) ;
    RUN_TEST( test_fill_image_advanced_cut_areas ) ;
    RUN_TEST( test_buffer_image_advanced_cut_areas ) ;
    RUN_TEST( test_packed_decoder_all_bpp ) ;

    printf( "\n%u tests, %u failed\n", s_TestsRun, s_TestsFailed ) ;
    return s_TestsFailed == 0 ? 0 : 1 ;
}

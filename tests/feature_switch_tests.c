#include <stdint.h>
#include <string.h>
#include "GFX.h"
#include "GFX_Sim.h"

static GFX_Color_t work[128];

int main(void)
{
    static uint8_t bits[1] = { 0xffu };
    GFX_Color_t pixels[8];
    Area_t area = {0, 0, 8, 1};
    GFX_Buffer_t buffer;
    GFX_Init_t init = {work, sizeof(work)};
    GFX_Img_t bitmap;
    uint8_t i;

    memset(&bitmap, 0, sizeof(bitmap));
    bitmap.W = 8; bitmap.H = 1;
    bitmap.Color_Type = GFX_COLOR_TYPE_BITMAP;
    bitmap.Color_Save_Way = GFX_Save_Way_MCU;
    bitmap.Color_Bits_Per_Pix = 1;
    bitmap.Color_Save_Info.C_Array = bits;
    for(i=0;i<8;i++) pixels[i] = GFX_Color_Convert(0x123456u);

    GFX_Init(&init);
    if(!GFX_Buffer_Init(&buffer, &area, pixels, sizeof(pixels))) return 1;
    GFX_Buff_Img(&buffer, &bitmap, 0, 0);
    for(i=0;i<8;i++) if(pixels[i] != GFX_Color_Convert(0x123456u)) return 2;
    return 0;
}

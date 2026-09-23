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

#define CASE_COUNT 18u
#define AUTO_MS 2200u
#define LONG_MS 1200u
#define STRIPE_H 16u
static uint8_t scratch[768u];
static uint8_t stripe[160u*STRIPE_H*sizeof(GFX_Color_t)+sizeof(GFX_Color_t)];
static uint8_t current_case;
static bool auto_run, flash_ok;
static Tick_t auto_tick;
static GFX_Color_t C(uint32_t rgb){return GFX_Color_Convert(rgb);}

/* Tick_Init initializes SysTick. VAL gives sub-millisecond benchmark resolution. */
static uint32_t timer_us(void){Tick_t a,b;uint32_t v;do{a=Tick_Get_Tick();v=SysTick->VAL;b=Tick_Get_Tick();}while(a!=b);return a*1000u+((SysTick->LOAD-v)*1000u)/(SysTick->LOAD+1u);}

static void digit(uint8_t n,int16_t x,int16_t y,uint8_t s,GFX_Color_t c){
 static const uint8_t m[10]={0x3f,6,0x5b,0x4f,0x66,0x6d,0x7d,7,0x7f,0x6f};uint8_t q=m[n%10u],w=4u*s,h=5u*s;
 if(q&1)GFX_Fill_Rect(c,x+s,y,w,s);if(q&2)GFX_Fill_Rect(c,x+w+s,y+s,s,h);if(q&4)GFX_Fill_Rect(c,x+w+s,y+h+2*s,s,h);
 if(q&8)GFX_Fill_Rect(c,x+s,y+2*h+2*s,w,s);if(q&16)GFX_Fill_Rect(c,x,y+h+2*s,s,h);if(q&32)GFX_Fill_Rect(c,x,y+s,s,h);if(q&64)GFX_Fill_Rect(c,x+s,y+h+s,w,s);
}
static void number(uint32_t v,int16_t x,int16_t y,uint8_t count,uint8_t s,GFX_Color_t c){uint8_t i;uint32_t d=1;for(i=1;i<count;i++)d*=10;for(i=0;i<count;i++){digit((uint8_t)((v/d)%10),x,y,s,c);x+=(int16_t)(7*s);d/=10;}}
static void header(uint8_t n,uint32_t us){GFX_Fill_Rect(C(0x0b1018),0,0,172,36);number(n+1u,6,5,2,2,C(flash_ok?0x4aa8ff:0xff4055));GFX_Fill_Rect(C(auto_run?0x35d07f:0x344255),40,8,auto_run?18:8,18);number((us+500u)/1000u,76,7,4,1,C(0xffffff));GFX_Fill_Rect(C(0xffb84a),145,8,20,4);GFX_Fill_Rect(C(0xffb84a),145,18,20,4);}

static void fill_adv(const GFX_Img_t*i,int16_t x,int16_t y,Anchor_t a,bool cs,Area_t self,bool cd,Area_t display){GFX_Fill_Img_Adv_Para_t p;memset(&p,0,sizeof(p));p.Img=i;p.X=x;p.Y=y;p.Anchor=a;p.Cut_Self_Enable=cs;p.Cut_Self=self;p.Anchor_Use_Cut_Self=cs;p.Cut_Screen_Enable=cd;p.Cut_Screen=display;p.Enable_Map=0xffffffffu;p.Fore_Color=0x4aa8ff;p.Back_Color=0x10151f;GFX_Fill_Img_Adv(&p);}
static void buff_img(const GFX_Img_t*i,int16_t x,int16_t y,uint32_t bg,bool fixed,bool cs,Area_t self,bool cd,Area_t display){uint16_t top;for(top=40;top<316;top+=STRIPE_H){uint16_t h=(uint16_t)(((316u-top)<STRIPE_H)?316u-top:STRIPE_H);Area_t a={6,(int16_t)top,160,h};GFX_Buffer_t b;GFX_Buff_Img_Adv_Para_t p;if(!GFX_Buffer_Init(&b,&a,stripe,sizeof(stripe)))return;GFX_Buff_Bg(&b,C(((top/STRIPE_H)&1)?0x173d61:0x8b3d55));memset(&p,0,sizeof(p));p.Buff=&b;p.Img=i;p.X=x;p.Y=y;p.Anchor=Anchor_LT;p.Enable_Map=0xffffffffu;p.Back_Color=bg;p.Blend_Back_Color_Enable=fixed;p.Cut_Self_Enable=cs;p.Cut_Self=self;p.Anchor_Use_Cut_Self=cs;p.Cut_Screen_Enable=cd;p.Cut_Screen=display;GFX_Buff_Img_Adv(&p);GFX_Flush(b.Buffer,a.X,a.Y,a.W,a.H);}}
static void bitmaps(void){const GFX_Img_t*i[5]={&GFX_Test_Bitmap_B1,&GFX_Test_Bitmap_B2,&GFX_Test_Bitmap_B3,&GFX_Test_Bitmap_B4,&GFX_Test_Bitmap_B5};uint8_t n;for(n=0;n<5;n++){GFX_Fill_Img_Adv_Para_t p;memset(&p,0,sizeof(p));p.Img=i[n];p.X=(int16_t)(10+(n&1)*78);p.Y=(int16_t)(48+(n/2)*72);p.Anchor=Anchor_LT;p.Enable_Map=0xffffffffu;p.Fore_Color=0x4aa8ffu+(uint32_t)n*0x251500u;p.Back_Color=0x10151fu;GFX_Fill_Img_Adv(&p);}}
static void alphas(bool fixed){const GFX_Img_t*i[5]={&GFX_Test_Cat_P4_A1,&GFX_Test_Cat_P4_A2,&GFX_Test_Cat_P4_A3,&GFX_Test_Cat_P4_A4,&GFX_Test_Cat_P4_A5};uint8_t n;for(n=0;n<5;n++)buff_img(i[n],(int16_t)(6+(n%2)*82),(int16_t)(42+(n/2)*88),0x203040u+(uint32_t)n*0x120804u,fixed,true,(Area_t){0,(int16_t)(n*12),80,70},true,(Area_t){6,40,160,276});}

static void scene(uint8_t n){GFX_Fill_Bg(C(0x10151f));if(!flash_ok&&n>0&&n<15){GFX_Fill_Rect(C(0xff4055),12,70,148,18);GFX_Fill_Rect(C(0xffb84a),28,106,116,18);return;}switch(n){
 case 0:GFX_Fill_Rect(C(0x246bfe),-24,44,95,52);GFX_Fill_Rect(C(0x35d07f),112,70,90,46);GFX_Fill_Circle(C(0xffb84a),28,192,48,8);GFX_Fill_Circle(C(0xe45cff),162,305,42,0);break;
 case 1:GFX_Fill_Img(&GFX_Test_Mountain_Mirror,6,54);GFX_Fill_Img(&GFX_Test_Dog_Mirror,38,168);break;
 case 2:GFX_Fill_Img(&GFX_Test_GreatWall_P4,6,52);GFX_Fill_Img(&GFX_Test_Santorini_P3,6,164);break;
 case 3:GFX_Fill_Img(&GFX_Test_Kitten_P5,38,48);GFX_Fill_Img(&GFX_Test_Ghibli_P4,6,160);break;
 case 4:bitmaps();break;
 case 5:GFX_Fill_Rect(C(0x27364c),18,62,136,210);fill_adv(&GFX_Test_Mountain_Mirror,86,162,Anchor_CM,true,(Area_t){24,12,112,70},true,(Area_t){30,82,112,160});break;
 case 6:{uint8_t j;static const Anchor_t a[9]={Anchor_LT,Anchor_CT,Anchor_RT,Anchor_LM,Anchor_CM,Anchor_RM,Anchor_LB,Anchor_CB,Anchor_RB};for(j=0;j<9;j++){int16_t x=(int16_t)(18+(j%3)*68),y=(int16_t)(64+(j/3)*90);GFX_Fill_Rect(C(0xff4055),x-2,y-2,5,5);fill_adv(&GFX_Test_Bitmap_B1,x,y,a[j],true,(Area_t){0,0,31,17},false,(Area_t){0,0,0,0});}}break;
 case 7:buff_img(&GFX_Test_Cat_Mirror_A4,46,76,0x203040,false,false,(Area_t){0,0,0,0},false,(Area_t){0,0,0,0});break;
 case 8:buff_img(&GFX_Test_Cat_Mirror_A4,46,76,0x203040,true,false,(Area_t){0,0,0,0},false,(Area_t){0,0,0,0});break;
 case 9:buff_img(&GFX_Test_Dog_P2_A2,46,76,0x2e405c,false,false,(Area_t){0,0,0,0},false,(Area_t){0,0,0,0});break;
 case 10:buff_img(&GFX_Test_Anime_P4_A5,46,54,0x203040,true,true,(Area_t){5,8,70,118},true,(Area_t){20,60,132,230});break;
 case 11:alphas(false);break;case 12:alphas(true);break;
 case 13:{Area_t a={28,80,116,94};GFX_Buffer_t b;if(GFX_Buffer_Init(&b,&a,stripe,sizeof(stripe))){GFX_Buff_Bg(&b,C(0x26364d));GFX_Buff_Rect(&b,4,92,92,50,C(0x35d07f));GFX_Buff_Circle(&b,135,142,30,C(0xffb84a));GFX_Flush(b.Buffer,a.X,a.Y,a.W,a.H);}}break;
 case 14:buff_img(&GFX_Test_GreatWall_P4,6,90,0x10151f,false,true,(Area_t){20,12,110,62},true,(Area_t){32,106,108,100});break;
 case 15:GFX_Fill_Img(&GFX_Test_MCU_Mountain,8,54);GFX_Fill_Img(&GFX_Test_MCU_Mountain,96,54);GFX_Fill_Img(&GFX_Test_MCU_Kitten_P3,66,122);break;
 case 16:buff_img(&GFX_Test_MCU_Cat_P2_A2,70,52,0x284260,true,false,(Area_t){0,0,0,0},false,(Area_t){0,0,0,0});fill_adv(&GFX_Test_MCU_Bitmap_B1,18,210,Anchor_LT,false,(Area_t){0,0,0,0},false,(Area_t){0,0,0,0});fill_adv(&GFX_Test_MCU_Bitmap_B5,112,210,Anchor_LT,false,(Area_t){0,0,0,0},false,(Area_t){0,0,0,0});break;
 default:{GFX_Img_t bad;GFX_Buffer_t b;Area_t a={20,120,80,40};memset(&bad,0,sizeof(bad));bad.W=20;bad.H=20;bad.Color_Type=GFX_COLOR_TYPE_NONE;GFX_Fill_Img(&bad,20,60);GFX_Fill_Rect(C(0x35d07f),20,72,132,18);GFX_Fill_Rect(C(0x4aa8ff),-200,-200,10,10);if(!GFX_Buffer_Init(&b,&a,stripe+1,10))GFX_Fill_Rect(C(0xffb84a),20,124,132,18);}break;}}
static void draw_case(uint8_t n){uint32_t t=timer_us();scene(n);t=timer_us()-t;header(n,t);}
static void keys_init(void){uint8_t i;Key_Set_t s;Key_Init();Key_Set_Struct_Init(&s);s.Shake=30;s.Long_Enable=true;s.Long=LONG_MS;s.Continue_Enable=false;s.Interval_Enable=false;for(i=0;i<3;i++)Key_Set(i,&s);Key_Start();}
static void keys(void){Key_Event_Node_t e[6];uint8_t i,c=Key_Buffer_Read(6,e);for(i=0;i<c;i++){if(e[i].ID==KEY_ID_0&&e[i].Event==Key_Event_Short_Press){auto_run=false;current_case=(uint8_t)((current_case+1)%CASE_COUNT);draw_case(current_case);}else if(e[i].ID==KEY_ID_0&&e[i].Event==Key_Event_Long_Press){auto_run=!auto_run;auto_tick=Tick_Get_Tick();draw_case(current_case);}else if(e[i].ID==KEY_ID_1&&e[i].Event==Key_Event_Short_Press){auto_run=false;current_case=0;draw_case(0);}else if(e[i].ID==KEY_ID_2&&e[i].Event==Key_Event_Long_Press){GFX_Fill_Bg(C(0));GFX_Set_Brightness(0);Delay_ms(80);Power_OFF;}}}
void GFX_Hardware_Test_Run(void){GFX_Init_t i={scratch,sizeof(scratch)};GFX_Init(&i);GFX_Set_Brightness(255);keys_init();flash_ok=BY25Q80_Is_Present();current_case=0;auto_run=false;draw_case(0);for(;;){MCU_IDWG_Feed();keys();if(auto_run&&Tick_Elaspe(auto_tick,AUTO_MS)){auto_tick=Tick_Get_Tick();current_case=(uint8_t)((current_case+1)%CASE_COUNT);draw_case(current_case);}}}

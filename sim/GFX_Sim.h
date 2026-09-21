#ifndef _GFX_SIM_H_
#define _GFX_SIM_H_

#include <stdint.h>
#include <stdbool.h>
#include "GFX_Define.h"

/* 获取帧缓冲指针（供窗口绘制使用） */
GFX_Color_t * GFX_Sim_Get_Framebuffer( void ) ;
uint16_t      GFX_Sim_Get_Width( void ) ;
uint16_t      GFX_Sim_Get_Height( void ) ;

/* Test/support hook: copy bytes into the simulated external Flash. */
bool GFX_Sim_Flash_Write( uint32_t Address , const void * Data , uint32_t Size ) ;

/* 初始化模拟器窗口（注册窗口类，不显示） */
void GFX_Sim_Init( void * hInstance ) ;

/* 显示窗口并进入消息循环（阻塞，直到用户关闭窗口） */
void GFX_Sim_Run( void ) ;

/* 请求重绘窗口 */
void GFX_Sim_Refresh( void ) ;

#endif

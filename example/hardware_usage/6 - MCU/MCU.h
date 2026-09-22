/**
 * @file MCU.h 
 * @author Pop_Cat (xuzhicheng@longshuo-tech.com)
 * @brief  
 * @version 0.0.0.24.11.08
 * @date 2025年09月14日
 * 
 * 
 */
#ifndef _MCU_H_
#define _MCU_H_

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 */
/* -------------------------------------------------------------------------------------------------------------------------- */
/* MCU 库文件 */
#include "py32f031_ll_rcc.h"
#include "py32f031_ll_bus.h"
#include "py32f031_ll_system.h"
#include "py32f031_ll_cortex.h"
#include "py32f031_ll_utils.h"
#include "py32f031_ll_pwr.h"
#include "py32f031_ll_dma.h"
#include "py32f031_ll_gpio.h"
#include "py32f031_ll_flash.h"
#include "py32f031_ll_tim.h"
#include "py32f031_ll_spi.h"
#include "py32f031_ll_usart.h"
#include "py32f031_ll_exti.h"
#include "py32f031_ll_adc.h"
#include "py32f031_ll_iwdg.h"
#include "stdint.h"
#include "stdbool.h"

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 宏定义 - 配置 */
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 日志打印 */
#define LOG_PRINTF_ENABLE   ( 0 )
#if LOG_PRINTF_ENABLE
    #define PRINTF_LOG printf
    #include "stdio.h"
#else
    #define PRINTF_LOG( ... ) 
#endif

/* 调试保护 */
#define MCU_DEBUG_PROTECT   ( 0 )

/* 看门狗 */
#define MCU_IWDG_ENABLE   ( 1 )
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 宏定义 - 通用 */
/* -------------------------------------------------------------------------------------------------------------------------- */
/* mcu flash 编程字大小 */
#define MCU_FLASH_PROGRAM_SIZE  ( 128 )   /* byte */

/* mcu flash 地址结尾 */
#define MCU_FLASH_ADDR_END      ( 0x0800FFFF )

/* mcu flash 页大小 */
#define MCU_FLASH_PAGE_SIZE     ( 128 )    /* byte */

/* mcu flash 用来保存的页的数量 */
#define MCU_FLASH_SAVE_PAGE_NUMBER ( 20 )    

/* mcu flash 用来保存的开始地址 */
#define MCU_FLASH_ADDR_START    ( MCU_FLASH_ADDR_END - MCU_FLASH_SAVE_PAGE_NUMBER * MCU_FLASH_PAGE_SIZE - 1 )  

#if MCU_IWDG_ENABLE
    // 程序结束
    #define MCU_END_OF_PROGRAM  {while( 1 ){ MCU_IDWG_Feed( ); }}
#else
    #define MCU_END_OF_PROGRAM  {while( 1 );}
#endif


/* GPIO Port 重映射 */
#define MCU_GPIOA GPIOA
#define MCU_GPIOB GPIOB
#define MCU_GPIOF GPIOF  

/* GPIO 类型重映射 */
#define MCU_GPIO_TYPE GPIO_TypeDef *

/* GPIO Pin 重映射 */
#define MCU_GPIO_PIN_0	LL_GPIO_PIN_0
#define MCU_GPIO_PIN_1	LL_GPIO_PIN_1
#define MCU_GPIO_PIN_2	LL_GPIO_PIN_2
#define MCU_GPIO_PIN_3	LL_GPIO_PIN_3
#define MCU_GPIO_PIN_4	LL_GPIO_PIN_4
#define MCU_GPIO_PIN_5	LL_GPIO_PIN_5
#define MCU_GPIO_PIN_6	LL_GPIO_PIN_6
#define MCU_GPIO_PIN_7	LL_GPIO_PIN_7
#define MCU_GPIO_PIN_8	LL_GPIO_PIN_8
#define MCU_GPIO_PIN_9	LL_GPIO_PIN_9
#define MCU_GPIO_PIN_10	LL_GPIO_PIN_10
#define MCU_GPIO_PIN_11	LL_GPIO_PIN_11
#define MCU_GPIO_PIN_12	LL_GPIO_PIN_12
#define MCU_GPIO_PIN_13	LL_GPIO_PIN_13
#define MCU_GPIO_PIN_14	LL_GPIO_PIN_14
#define MCU_GPIO_PIN_15	LL_GPIO_PIN_15
#define MCU_GPIO_PIN_16	LL_GPIO_PIN_16

/* 设置GPIO输出的高低电平 */
#define MCU_PIN_Set( GPIO , PIN )       LL_GPIO_SetOutputPin( GPIO , PIN )
#define MCU_PIN_Reset( GPIO , PIN )     LL_GPIO_ResetOutputPin( GPIO , PIN )

/* 读取GPIO的输入电平 */
#define MCU_PIN_READ( GPIO , PIN )     LL_GPIO_IsInputPinSet( GPIO , PIN )


/* -------------------------------------------------------------------------------------------------------------------------- */
/* 类型定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 外部变量引用声明 */
/* -------------------------------------------------------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------------------------------------------------------- */
/* 静态对象定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------------------------------------------------------- */
/* 函数声明 - 功能 */
/* -------------------------------------------------------------------------------------------------------------------------- */
void MCU_Init( void );

void MCU_ADC_Init( void ( * DelayMs )( uint32_t ) );

void MCU_Download_Reset( void );

void MCU_Flash_Program( uint32_t Flash_Addr , void * Data_Addr , uint32_t Size );
void MCU_Flash_Erase( uint32_t Page_Addr , uint32_t Page_Number );

void MCU_GPIO_CLOCK_ENABLE( MCU_GPIO_TYPE GPIOx );
void MCU_Disable_All_IT( void );

#if MCU_IWDG_ENABLE
void MCU_IDWG_Feed( void );
#else
#define MCU_IDWG_Feed( ... )
#endif

void MCU_Reset( void );
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 函数声明 - 回调 */
/* -------------------------------------------------------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------------------------------------------------------- */
/* 函数声明 - 接口 */
/* -------------------------------------------------------------------------------------------------------------------------- */


#ifdef __cplusplus
}
#endif

#endif

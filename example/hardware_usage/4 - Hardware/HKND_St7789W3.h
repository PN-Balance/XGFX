/**
 * @file HKND_St7789W3.h
 * @author Pop_Cat (xuzhicheng@longshuo-tech.com)
 * @brief 
 * @version 0.0.0.24.11.08
 * @date 2025-03-20
 * 
 * 
 */
#ifndef _HKND_ST7789W3_H_
#define _HKND_ST7789W3_H_

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 */
/* -------------------------------------------------------------------------------------------------------------------------- */
#include "General_Type.h"
#include "MCU.h"
#include "User_Config.h"
#include "GFX_Define.h"
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 宏定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */
/* RS */
#define ST7789W3_PORT_RS        UC_LCD_RS_PORT
#define ST7789W3_PIN_RS         UC_LCD_RS_PIN

/* SCLK */
#define ST7789W3_PORT_SCLK      UC_LCD_SCL_PORT
#define ST7789W3_PIN_SCLK       UC_LCD_SCL_PIN

/* MOSI */
#define ST7789W3_PORT_MOSI      UC_LCD_SDA_PORT
#define ST7789W3_PIN_MOSI       UC_LCD_SDA_PIN

/* CS */
#define ST7789W3_PORT_CS        UC_LCD_CS_PORT
#define ST7789W3_PIN_CS         UC_LCD_CS_PIN

/* RESET */
#define ST7789W3_PORT_RESET        UC_LCD_RESET_PORT
#define ST7789W3_PIN_RESET         UC_LCD_RESET_PIN

/* LED */
#define ST7789W3_PORT_LED   UC_LCD_LED_PORT
#define ST7789W3_PIN_LED    UC_LCD_LED_PIN


/* 端口操作定义 */
#define ST7789W3_RESET_Low()  MCU_PIN_Reset( ST7789W3_PORT_RESET,ST7789W3_PIN_RESET)
#define ST7789W3_RESET_High()  MCU_PIN_Set(ST7789W3_PORT_RESET,ST7789W3_PIN_RESET)

#define ST7789W3_RS_Low()   MCU_PIN_Reset(ST7789W3_PORT_RS,ST7789W3_PIN_RS)
#define ST7789W3_RS_High()   MCU_PIN_Set(ST7789W3_PORT_RS,ST7789W3_PIN_RS)

#define ST7789W3_CS_Low()	MCU_PIN_Reset(ST7789W3_PORT_CS,ST7789W3_PIN_CS)
#define ST7789W3_CS_High()	MCU_PIN_Set(ST7789W3_PORT_CS,ST7789W3_PIN_CS)

#define ST7789W3_LED_High()     MCU_PIN_Set( ST7789W3_PORT_LED , UC_LCD_LED_PIN )
#define ST7789W3_LED_Low( )     MCU_PIN_Reset( ST7789W3_PORT_LED , UC_LCD_LED_PIN )

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
/* 函数声明 */
/* -------------------------------------------------------------------------------------------------------------------------- */
void ST7789W3_Init( Orth_Dir_t Direction_Init );

void ST7789W3_Write_Bus( uint8_t Byte );
void ST7789W3_Write_Data_8bit( uint8_t Data_8Bit );
void ST7789W3_Write_Data_16bit( uint16_t Data_16Bit );
void ST7789W3_Write_Address_Of_Data( uint8_t Address );
void ST7789W3_Window_Set( uint16_t X_Start , uint16_t Y_Start , uint16_t X_End , uint16_t Y_End );


void ST7789W3_Direction_Set( Orth_Dir_t Direction );
void ST7789W3_Fill( uint16_t Colour , int16_t X , int16_t Y , uint16_t Width ,  uint16_t Height  );
void ST7789W3_Flush( uint16_t * Buffer , int16_t X , int16_t Y , uint16_t Width ,  uint16_t Height );
void ST7789W3_Set_Brighness( uint8_t Brightness );
void ST7789W3_Point( int16_t X , int16_t Y , uint16_t Colour );
#ifdef __cplusplus
}
#endif

#endif

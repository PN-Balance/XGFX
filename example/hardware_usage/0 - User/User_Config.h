/**
 * @file User_Config.h 
 * @author Pop_Cat (xuzhicheng@longshuo-tech.com)
 * @brief  
 * @version 0.0.0.24.11.08
 * @date 2025年09月14日
 * 
 * 
 */
#ifndef _USER_CONFIG_H_
#define _USER_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 */
/* -------------------------------------------------------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------------------------------------------------------- */
/* 宏定义 - 通用 */
/* -------------------------------------------------------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------------------------------------------------------- */
/* 宏定义 - 配置 */
/* -------------------------------------------------------------------------------------------------------------------------- */
/******************/
/* 系统信息 */
/******************/
#define UC_SOFT_VERSION ( "1001v" )

/******************/
/* 节能设置 */
/******************/
/* 自动关机时间 */
#define UC_AUTO_SHUT_DOWN_IN_MS ( 2 * 60 * 1000 )   /* ms */

/* 自动激光关闭时间 */
#define UC_AUTO_LASER_OFF_IN_MS ( 30 * 1000 )   /* ms */

/* 背光自动关闭时间 */
#define UC_AUTO_BACKLIGHT_TIME    ( 30 * 1000)   /* ms */

/* ----------------- */
/* 硬件资源配置 */
/* ----------------- */
/* 电源保持脚 */
#define UC_POWER_HOLD_PORT    MCU_GPIOB
#define UC_POWER_HOLD_PIN     MCU_GPIO_PIN_3

/* 开机按键脚 = 按键 1 */
#define UC_KEY_POWER_ON_PORT    MCU_GPIOA
#define UC_KEY_POWER_ON_PIN     MCU_GPIO_PIN_5

/* 按键 2 */
#define UC_KEY_2_PORT    MCU_GPIOA
#define UC_KEY_2_PIN     MCU_GPIO_PIN_3

/* 按键 3 */
#define UC_KEY_3_PORT    MCU_GPIOA
#define UC_KEY_3_PIN     MCU_GPIO_PIN_4

/* 电池电压模拟输入脚 */
#define UC_BATTERY_ADC_VOLTAGE_PORT   MCU_GPIOA
#define UC_BATTERY_ADC_VOLTAGE_PIN    MCU_GPIO_PIN_1
#define UC_BATTERY_ADC_VOLTAGE_CHANNEL  LL_ADC_CHANNEL_1

/* 温度传感器模拟输入脚 */
#define UC_ENV_TEMP_PORT        MCU_GPIOA
#define UC_ENV_TEMP_PIN         MCU_GPIO_PIN_2
#define UC_ENV_TEMP_ADC_CHANNEL LL_ADC_CHANNEL_2    

/* 充电 检测输入 */
#define UC_CHARGE_DETECT_PORT   MCU_GPIOB
#define UC_CHARGE_DETECT_PIN    MCU_GPIO_PIN_2

/* 充电 输出检测 */
#define UC_CHARGE_OUTPUT_PORT   MCU_GPIOF
#define UC_CHARGE_OUTPUT_PIN    MCU_GPIO_PIN_1

/* 充电 错误检测*/
#define UC_CHARGE_ERROR_PORT   MCU_GPIOF
#define UC_CHARGE_ERROR_PIN    MCU_GPIO_PIN_0

/* 充电 主动控制 */
#define UC_CHARGE_CONTROL_PORT   MCU_GPIOF
#define UC_CHARGE_CONTROL_PIN    MCU_GPIO_PIN_3

/* 容栅数据脚 */
#define UC_RONG_SHAN_DATA_PORT    MCU_GPIOA
#define UC_RONG_SHAN_DATA_PIN     MCU_GPIO_PIN_6
#define UC_RONG_SHAN_DATA_POSTION   6

/* 容栅时钟脚 */
#define UC_RONG_SHAN_SCLK_PORT    MCU_GPIOA
#define UC_RONG_SHAN_SCLK_PIN     MCU_GPIO_PIN_7
#define UC_RONG_SHAN_SCLK_POSTION   7
#define UC_RONG_SHAN_EXTI_LINE   LL_EXTI_LINE_7
#define UC_RONG_SHAN_EXTI_Handler  EXTI4_15_IRQHandler
#define UC_RONG_SHAN_EXTI_IRQn   EXTI4_15_IRQn

/* LCD RS */
#define UC_LCD_RS_PORT   MCU_GPIOB
#define UC_LCD_RS_PIN    MCU_GPIO_PIN_6

/* LCD CS */
#define UC_LCD_CS_PORT   MCU_GPIOB
#define UC_LCD_CS_PIN    MCU_GPIO_PIN_4


/* LCD LED */
#define UC_LCD_LED_PORT   MCU_GPIOF
#define UC_LCD_LED_PIN    MCU_GPIO_PIN_7


/* LCD SCL */
#define UC_LCD_SCL_PORT   MCU_GPIOB
#define UC_LCD_SCL_PIN    MCU_GPIO_PIN_8
#define UC_LCD_SCL_ALTERNATE  LL_GPIO_AF_1

/* LCD SDA */
#define UC_LCD_SDA_PORT   MCU_GPIOA 
#define UC_LCD_SDA_PIN    MCU_GPIO_PIN_10

/* LCD RESET */
#define UC_LCD_RESET_PORT   MCU_GPIOB
#define UC_LCD_RESET_PIN    MCU_GPIO_PIN_7

/* 蜂鸣器 */
#define UC_BUZZER_PORT   MCU_GPIOB
#define UC_BUZZER_PIN    MCU_GPIO_PIN_1
#define UC_BUZZER_ALTERNATE  LL_GPIO_AF_1
#define UC_BUZZER_TIMER   TIM2
#define UC_BUZZER_TIMER_CHANNEL   LL_TIM_CHANNEL_CH4
#define UC_BUZZER_INT_TIMER   TIM17
#define UC_BUZZER_INT_TIMER_IRQn   TIM17_IRQn
#define UC_BUZZER_INT_TIMER_CLK_ENABLE( )   LL_APB1_GRP2_EnableClock( LL_APB1_GRP2_PERIPH_TIM17 )

/* Flash CS */
#define UC_FLASH_CS_PORT   MCU_GPIOA
#define UC_FLASH_CS_PIN    MCU_GPIO_PIN_8

/* Flash SCLK */
#define UC_FLASH_SCLK_PORT   MCU_GPIOA
#define UC_FLASH_SCLK_PIN    MCU_GPIO_PIN_9

/* Flash MOSI */
#define UC_FLASH_MOSI_PORT   MCU_GPIOA
#define UC_FLASH_MOSI_PIN    MCU_GPIO_PIN_12

/* Flash MISO */
#define UC_FLASH_MISO_PORT   MCU_GPIOA
#define UC_FLASH_MISO_PIN    MCU_GPIO_PIN_11

/* I2C0 SDA */
#define UC_I2C0_SDA_PORT   MCU_GPIOB
#define UC_I2C0_SDA_PIN    MCU_GPIO_PIN_0

/* I2C0 SCL */
#define UC_I2C0_SCL_PORT   MCU_GPIOB
#define UC_I2C0_SCL_PIN    MCU_GPIO_PIN_1


/* LCD 宽度和高度 */
#define UC_LCD_WIDTH    ( 172 )
#define UC_LCD_HEIGHT   ( 320 )

/* 背景颜色 rgb888 */
#define UC_LCD_BG_COLOUR    ( 0x000000 )

/* 主题色 */
#define UC_LCD_ITEM_COLOUR  ( 0xFF5A00 )

/* 机身长度 */
#define UC_DEVICE_LENGTH ( 0.159 )  /* M */

/* 机芯校准的基准和设备外壳基准的偏移量 */
#define UC_FAC_LD_DEFALUT_OFFSET    ( 44 )  /* mm */

/* 基准校准后的偏移量 */
#define UC_LD_DEFALUT_OFFSET    ( 0 ) /* mm*/

/* 默认的测距单位 */
#define UC_LD_DEFALUT_UNIT  Unit_Feet_Inch

/* 默认的测距基准 */
#define UC_LD_DEFALUT_REF   Ref_Back 

/* 量程 - 米 */
#define UC_LD_RANGE_MAX     ( 45.0 )
#define UC_LD_RANGE_MIN     ( 0.05 )



/* 中断优先级 */
#define UC_TICK_TIMER_UPDATA_IT_PRIOTY ( 3 )
#define UC_CAPGATE_EXTI_IT_PRIOTY  ( 0 )
#define UC_BUZZER_TIMER_UPDATA_IT_PRIOTY  ( 2 )
#define UC_KEY_TIMER_UPDATA_IT_PRIOTY  ( 1 )

/* 激光测距模式行数 */
#define UC_LMM_LINES ( 3 )

/* 激光测距行位图 宏定义 */
#define LMM_LINE_0   ( 0x01 << 0 )
#define LMM_LINE_1   ( 0x01 << 1 )
#define LMM_LINE_2   ( 0x01 << 2 )
#define LMM_LINE_ALL ( 0xFF )

/* 水平尺默认单位 */
#define UC_HOR_LEVEL_ANG_DEFALUT Unit_Deg

/* 用户默认模式 */
#define UC_MODE_DEFALUT Mode_Angle_Ruler

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

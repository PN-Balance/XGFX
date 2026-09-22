/**
 * @file Power_Port.c
 * @author Pop_Cat (xuzhicheng@longshuo-tech.com)
 * @brief 
 * @version 0.0.0.24.11.08
 * @date 2025年09月18日
 * 
 * 
 */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 */
/* -------------------------------------------------------------------------------------------------------------------------- */
#include "User_Config.h"
#include "Power.h"
#include "MCU.h"
#include "Key.h"
#include "math.h"
#include "Delay.h"
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 私有宏定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------------------------------------------------------- */
/* 内部函数声明 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 私有类型定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------------------------------------------------------- */
/* 外部变量引用声明 */
/* -------------------------------------------------------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------------------------------------------------------- */
/* 文件作用域对象 */
/* -------------------------------------------------------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------------------------------------------------------- */
/* 只读数据 */
/* -------------------------------------------------------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------------------------------------------------------- */
/* 函数定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */
/**
 * @brief 接口 初始化 电源控制脚 out（ 默认保持电源 ）、开机按键脚 in 、充电检测脚 in 、充电完成脚 in 、充电控制脚( 默认可充电 )  out
 * 刚上电 复位相关的内容
 */
void Power_Port_Power_Reset_Periph_Init( void )
{
    LL_GPIO_InitTypeDef GPIO_InitStruct;


    /* 时钟开启 */
    MCU_GPIO_CLOCK_ENABLE( UC_POWER_HOLD_PORT );
    MCU_GPIO_CLOCK_ENABLE( UC_CHARGE_CONTROL_PORT );
    MCU_GPIO_CLOCK_ENABLE( UC_KEY_POWER_ON_PORT );
    MCU_GPIO_CLOCK_ENABLE( UC_CHARGE_DETECT_PORT );
    MCU_GPIO_CLOCK_ENABLE( UC_CHARGE_OUTPUT_PORT );
    MCU_GPIO_CLOCK_ENABLE( UC_CHARGE_ERROR_PORT );


    /* 电源控制脚初始化为推挽输出并保持 */
    LL_GPIO_StructInit( &GPIO_InitStruct );
    GPIO_InitStruct.Pin = UC_POWER_HOLD_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    LL_GPIO_Init( UC_POWER_HOLD_PORT , &GPIO_InitStruct );
    Power_Port_Set_Power_Hold_State( true );

    /* 充电控制脚 */
    LL_GPIO_StructInit( &GPIO_InitStruct );
    GPIO_InitStruct.Pin = UC_CHARGE_CONTROL_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_OPENDRAIN ;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO ;
    LL_GPIO_Init( UC_CHARGE_CONTROL_PORT , &GPIO_InitStruct );

    /* 开机脚 初始化为上拉输入 */
    LL_GPIO_StructInit( &GPIO_InitStruct );
    GPIO_InitStruct.Pin = UC_KEY_POWER_ON_PIN;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init( UC_KEY_POWER_ON_PORT , &GPIO_InitStruct );

    /* 充电检测脚 */
    GPIO_InitStruct.Pin = UC_CHARGE_DETECT_PIN;
    LL_GPIO_Init( UC_CHARGE_DETECT_PORT , &GPIO_InitStruct );

    /* 充电输出检测脚 */
    GPIO_InitStruct.Pin = UC_CHARGE_OUTPUT_PIN;
    LL_GPIO_Init( UC_CHARGE_OUTPUT_PORT , &GPIO_InitStruct );

    /* 充电完成状态正常脚 */
    GPIO_InitStruct.Pin = UC_CHARGE_ERROR_PIN;
    LL_GPIO_Init( UC_CHARGE_ERROR_PORT , &GPIO_InitStruct );
}


/**
 * @brief 剩余的外设初始化 电池电压 和 环境温度的 ADC 
 * 如果初始化时间过长可能会出问题
 * 
 */
void Power_Port_Other_Peripheral_Init( void )
{
    MCU_ADC_Init( Delay_ms );
}




/**
 * @brief 控制是否进行充电
 * 
 * @param Enable 
 * @return true 是
 * @return false 不是
 */
void Power_Port_Set_Charge_Control_State( bool Enable )
{
    if( Enable )
    {
        MCU_PIN_Set( UC_CHARGE_CONTROL_PORT , UC_CHARGE_CONTROL_PIN );
    }
    else
    {
        MCU_PIN_Reset( UC_CHARGE_CONTROL_PORT , UC_CHARGE_CONTROL_PIN );
    }
}

/**
 * @brief 设置电源保持状态
 * 
 * @param State true 开启 false 关闭
 */
void Power_Port_Set_Power_Hold_State( bool State )
{
    if( true == State )
    {
        MCU_PIN_Set( UC_POWER_HOLD_PORT , UC_POWER_HOLD_PIN );
    }
    else
    {
        MCU_PIN_Reset( UC_POWER_HOLD_PORT , UC_POWER_HOLD_PIN );
    }
}

/**
 * @brief 
 * 
 * @return true 
 * @return false 
 */
bool Power_Port_Get_Power_On_Key_State( void )
{
    return ! MCU_PIN_READ( UC_KEY_POWER_ON_PORT , UC_KEY_POWER_ON_PIN );
}


/**
 * @brief 获取电池的电压 
 * 
 * @return float 伏特
 */
float Power_Port_Get_Bat_Voltage( void )
{
    /* 按键扫描中断的ADC值获取流程和 电池电压获取的流程互斥 */
    Key_Scan_Mux_Enter(  ); // 进入互斥区

    uint16_t ADC_Value ;

    LL_ADC_REG_StopConversion( ADC1 );
    LL_ADC_Disable( ADC1 );

    /* 通道选择 */
    /* Set channel 4 as conversion channel */
    LL_ADC_REG_SetSequencerRanks(ADC1,LL_ADC_REG_RANK_1, UC_BATTERY_ADC_VOLTAGE_CHANNEL);

    LL_ADC_Enable( ADC1 );
    for( uint8_t i = 0 ; i < 4 ; i++ )
    {
        __nop();
    }

    /* Start ADC Software Conversion */
    LL_ADC_REG_StartConversion(ADC1);
    
    /* Wait until ADC conversion is complete */
    while(LL_ADC_IsActiveFlag_EOC(ADC1)==0);
    LL_ADC_ClearFlag_EOC(ADC1);

    /* Read ADC conversion data */
    ADC_Value = LL_ADC_REG_ReadConversionData12( ADC1 );


    /* 按键扫描中断的ADC值获取流程和 电池电压获取的流程互斥 */
    Key_Scan_Mux_Exit(  ); // 退出互斥区

    return ( float ) ADC_Value / ( 4095.0f ) * 2.5 * 2 ;
}


// 定义常量
const float VCC = 3.32;          // 电源电压
const float VREF = 2.5;        // ADC参考电压
const float B = 3380.0;        // 热敏电阻B值
const float R0 = 10000.0;      // 热敏电阻在25°C时的阻值(假设为10kΩ，根据你的实际热敏电阻修改)
const float T0 = 298.15;       // 25°C开尔文温度(25+273.15)

// 假设定值电阻阻值(根据你的实际电路修改)
const float R_FIXED = 10000.0; // 假设定值电阻为10kΩ

// 从ADC值获取温度
float getTemperature(int adcValue) {
    // 1. 将ADC值转换为电压
    float voltage = (adcValue * VREF) / 4095.0;

    // 2. 计算热敏电阻阻值
    float Rt = R_FIXED * (VCC / voltage - 1.0);
    
    // 3. 使用B值公式计算温度(开尔文)
    float invT = 1.0 / T0 + (1.0 / B) * log(Rt / R0);
    float temperatureK = 1.0 / invT;
    
    // 4. 转换为摄氏度
    float temperatureC = temperatureK - 273.15;
    
    return temperatureC;
}

/**
 * @brief 获取环境温度
 * 
 * 
 * @return float 温度 摄氏度
 */
float Power_Port_Get_Env_Tempture( void )
{
    /* 按键扫描中断的ADC值获取流程和 电池电压获取的流程互斥 */
    Key_Scan_Mux_Enter(  ); // 进入互斥区

    uint16_t ADC_Value ;

    LL_ADC_REG_StopConversion( ADC1 );
    LL_ADC_Disable( ADC1 );

    /* 通道选择 */
    /* Set channel 4 as conversion channel */
    LL_ADC_REG_SetSequencerRanks(ADC1,LL_ADC_REG_RANK_1, UC_ENV_TEMP_ADC_CHANNEL);

    LL_ADC_Enable( ADC1 );
    for( uint8_t i = 0 ; i < 4 ; i++ )
    {
        __nop();
    }

    /* Start ADC Software Conversion */
    LL_ADC_REG_StartConversion(ADC1);
    
    /* Wait until ADC conversion is complete */
    while(LL_ADC_IsActiveFlag_EOC(ADC1)==0);
    LL_ADC_ClearFlag_EOC(ADC1);

    /* Read ADC conversion data */
    ADC_Value = LL_ADC_REG_ReadConversionData12( ADC1 );

    /* 按键扫描中断的ADC值获取流程和 电池电压获取的流程互斥 */
    Key_Scan_Mux_Exit(  ); // 退出互斥区
    
    return getTemperature( ADC_Value );
}


/**
 * @brief 获取充电线插入状态
 * 
 * @return true     插入
 * @return false    未插入
 */
bool Power_Port_Get_Charge_Insert( void )
	
{
    return ! MCU_PIN_READ( UC_CHARGE_DETECT_PORT , UC_CHARGE_DETECT_PIN );
}

/**
 * @brief 获取充电错误状态
 * 
 * @return true      错误
 * @return false     正常
 */
bool Power_Port_Get_Charge_Error( void )
{
    return MCU_PIN_READ( UC_CHARGE_ERROR_PORT , UC_CHARGE_ERROR_PIN );
}

/**
 * @brief 获取充电输出状态
 * 
 * @return true      输出
 * @return false     未输出
 */
bool Power_Port_Get_Charge_Output( void )
{
    return ! MCU_PIN_READ( UC_CHARGE_OUTPUT_PORT , UC_CHARGE_OUTPUT_PIN );
}

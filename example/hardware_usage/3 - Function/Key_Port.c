/**
 * @file Key_Port.c
 * @author Pop_Cat (xuzhicheng@longshuo-tech.com)
 * @brief 
 * @version 0.0.0.24.11.08
 * @date 2025年06月03日
 * 
 * 
 */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 */
/* -------------------------------------------------------------------------------------------------------------------------- */
#include "Key.h"
#include "MCU.h"

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 私有宏定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */
#define KEY_ADC_MAX	0xFFFF

#define KEY_ADC_VALUE_TOLARANCE	( 200 )

#define KEY_1_ADC_VALUE ( 2702 )
#define KEY_2_ADC_VALUE ( 1801 )


#define KEY_TIM TIM14
#define KEY_TIM_CLK_ENABLE() LL_APB1_GRP2_EnableClock( LL_APB1_GRP2_PERIPH_TIM14 )
#define KEY_TIM_IRQn TIM14_IRQn
#define KEY_TIM_IRQHandler TIM14_IRQHandler
#define KEY_TIM_IT_FLAG_CHECK   ( LL_TIM_IsActiveFlag_UPDATE( KEY_TIM ) && LL_TIM_IsEnabledIT_UPDATE( KEY_TIM ) )
#define KEY_TIM_IT_FLAG_CLEAR   LL_TIM_ClearFlag_UPDATE( KEY_TIM )


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
 * @brief 初始化按键输入相关的硬件资源 GPIO 、ADC
 * 
 * @param User 可能的用户数据
 * 
 */
void Key_Port_Input_Init( void )
{
    LL_GPIO_InitTypeDef GPIO_InitStruct;

    MCU_GPIO_CLOCK_ENABLE( UC_CHARGE_OUTPUT_PORT );
    MCU_GPIO_CLOCK_ENABLE( UC_CHARGE_ERROR_PORT );

    /* 开机脚 初始化为上拉输入 */
    LL_GPIO_StructInit( &GPIO_InitStruct );
    GPIO_InitStruct.Pin = UC_KEY_2_PIN ;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init( UC_KEY_2_PORT  , &GPIO_InitStruct );

    GPIO_InitStruct.Pin = UC_KEY_3_PIN ;
    LL_GPIO_Init( UC_KEY_3_PORT  , &GPIO_InitStruct );
}

/**
 * @brief 按键扫描定时器初始化 扫描间隔为 KEY_SCAN_TIME 
 * 
 * @param User 可能的用户数据
 * 
 */
void Key_Port_Timer_Init( void )
{
	/* Configure KEY_TIM */
	LL_TIM_InitTypeDef TIM1CountInit = {0};

	/* Enable KEY_TIM clock */
	KEY_TIM_CLK_ENABLE( );

	TIM1CountInit.ClockDivision       = LL_TIM_CLOCKDIVISION_DIV1;/* No clock division             */
	TIM1CountInit.CounterMode         = LL_TIM_COUNTERMODE_UP;    /* Up counting mode */
	TIM1CountInit.Prescaler           = 7200-1;                   /* Prescaler：8000   */
	TIM1CountInit.Autoreload          = 200-1;                   /* Autoreload value：1000 */
	TIM1CountInit.RepetitionCounter   = 0;                        /* RepetitionCounter value：0      */

	/* Initialize KEY_TIM */
	LL_TIM_Init( KEY_TIM , &TIM1CountInit);

	/* Clear update flag */
	LL_TIM_ClearFlag_UPDATE(KEY_TIM);

	/* Enable UPDATE interrupt */
	LL_TIM_EnableIT_UPDATE(KEY_TIM);

	/* Enable KEY_TIM */
	LL_TIM_EnableCounter(KEY_TIM);

	/* Enable UPDATE interrupt request */
	NVIC_DisableIRQ( KEY_TIM_IRQn );
	NVIC_SetPriority( KEY_TIM_IRQn , UC_KEY_TIMER_UPDATA_IT_PRIOTY );
}



/* 按键扫描中断句柄 */
void KEY_TIM_IRQHandler ( void )
{
    if( KEY_TIM_IT_FLAG_CHECK )
    {
        KEY_TIM_IT_FLAG_CLEAR;
        Key_Call_Back_Scan( );
    }
}

/**
 * @brief 控制扫描失能还是使能
 * 
 * @param New_State 失能还是使能
 */
void Key_Port_Scan( bool New_State )
{
	if( New_State )
		NVIC_EnableIRQ( KEY_TIM_IRQn );
	else
		NVIC_DisableIRQ( KEY_TIM_IRQn );	
}

void Key_Port_Read_State_All( volatile Key_t * Key )
{
    for( uint8_t i = 0 ; i < KEY_TOTAL_NUMB ; i++ )
        Key[ i ].Pressed = Key_Port_Is_Pressed( i ) ? Key_Pressed : Key_Release ;   
}

/**
 * @brief 指定按键是否被按下
 * 
 * @param ID 按键ID
 * 
 * @return true 按下
 * @return false 没按下
 */
bool Key_Port_Is_Pressed( uint8_t ID )
{
    switch( ID )
    {
        case 0 : return !MCU_PIN_READ( UC_KEY_POWER_ON_PORT , UC_KEY_POWER_ON_PIN ) ;
        case 1 : return !MCU_PIN_READ( UC_KEY_2_PORT , UC_KEY_2_PIN ) ;
        case 2 : return !MCU_PIN_READ( UC_KEY_3_PORT , UC_KEY_3_PIN ) ;
    }

	return false ;
}







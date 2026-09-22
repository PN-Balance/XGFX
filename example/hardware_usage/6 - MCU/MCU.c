/**
 * @file MCU.c
 * @author Pop_Cat (xuzhicheng@longshuo-tech.com)
 * @brief 
 * @version 0.0.0.24.11.08
 * @date 2025年09月14日
 * 
 * 
 */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 */
/* -------------------------------------------------------------------------------------------------------------------------- */
#include "MCU.h"
#include "User_Config.h"
#include <string.h>
#include "Cap7618B.h"
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 私有宏定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */
#define LOG_USART USART2
#define LOG_USART_CLOCK_ENABLE  LL_APB1_GRP2_EnableClock( LL_APB1_GRP1_PERIPH_USART2 ) ;

#define MCU_ADC_CHANNEL_NUM 3


/* -------------------------------------------------------------------------------------------------------------------------- */
/* 内部函数声明 */
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 基本初始化 */
void MCU_Base_Init( void );

/* Flash初始化 */
void MCU_Flash_Init( void );

/* ADC 校准 */
static void MCU_ADC_Calibrate( uint16_t Timeout , void ( * DelayMs )( uint32_t ) );

#if MCU_IWDG_ENABLE
/* IDWG 初始化 */
void MCU_IDWG_Init( void );
#endif

/* 串口初始化 */
#if LOG_PRINTF_ENABLE
void MCU_LOG_Init( void );
#include "stdio.h"
#endif

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 私有类型定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 外部变量引用声明 */
/* -------------------------------------------------------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------------------------------------------------------- */
/* 文件作用域对象 */
/* -------------------------------------------------------------------------------------------------------------------------- */
bool MCU_ADC_Init_Flag = false ; 


/* -------------------------------------------------------------------------------------------------------------------------- */
/* 只读数据 */
/* -------------------------------------------------------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------------------------------------------------------- */
/* 函数定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */
/**
 * @brief MCU 相关的初始化 
 * 
 */
void MCU_Init( void )
{

#if MCU_DEBUG_PROTECT
    /* 调试保护 */
    // 假设系统时钟为72MHz
    uint32_t cycles = 72000000 * 2; // 总延迟周期数 = 频率 × 时间
    
    // 使用简单的循环实现延迟
    for(uint32_t i = 0; i < cycles; i++) {
        __NOP(); // 执行空操作，防止编译器优化掉循环
    }
#endif

    /* 基本初始化 MCU */
    MCU_Base_Init(  );

    /* Flash 初始化 */
    MCU_Flash_Init(  );
    
#if LOG_PRINTF_ENABLE
    /* 日志初始化 */
    MCU_LOG_Init(  );
#endif

#if MCU_IWDG_ENABLE
    /* 看门狗初始化 */
    MCU_IDWG_Init(  );
#endif
    
    MCU_ADC_Init_Flag = false ;
}


/**
 * @brief MCU 下载复位
 * 
 * 
 */
void MCU_Download_Reset( void )
{

}

/**
 * @brief mcu flash 编程
 * 
 * @param Flash_Addr 写入的地址
 * @param Data_Addr 数据的地址
 * @param Size 数据的大小
 * 
 */
void MCU_Flash_Program( uint32_t Flash_Addr , void * Data_Addr , uint32_t Size )
{   
    if( Flash_Addr & 0x1F )  return  ;  /* 页地址必须是 32 字节对齐 */
    if( !Size )  return  ;  /* 数据大小必须大于 0 */

    uint32_t * src = (uint32_t *)Data_Addr ;  /* Pointer to the data to be written */
 
    // 拷贝残缺页
    uint8_t Part_Memory_Copy[ FLASH_PAGE_SIZE ] = { 0 } ;

    // 完整的页数
    uint16_t Total_Page = Size / FLASH_PAGE_SIZE ;

    // 残缺页的大小
    uint16_t Partial_Page_Size = Size - Total_Page * FLASH_PAGE_SIZE ; 

    // 解锁 flash
    LL_FLASH_Unlock(FLASH);

    // 完整页编程
    for( uint8_t i = 0 ; i < Total_Page ; i++ )
    {
        /* Wait Busy=0 */
        while(LL_FLASH_IsActiveFlag_BUSY(FLASH)==1);

        /* Enable EOP */
        LL_FLASH_EnableIT_EOP(FLASH);

        /* Enable Program */
        LL_FLASH_EnablePageProgram(FLASH);

        /* Page Program */
        LL_FLASH_PageProgram( FLASH , Flash_Addr , src );

        /* Wait Busy=0 */
        while(LL_FLASH_IsActiveFlag_BUSY(FLASH)==1);

        /* Wait EOP=1 */
        while(LL_FLASH_IsActiveFlag_EOP(FLASH)==0);

        /* Clear EOP */
        LL_FLASH_ClearFlag_EOP(FLASH);

        /* Disable EOP */
        LL_FLASH_DisableIT_EOP(FLASH);

        /* Disable Program */
        LL_FLASH_DisablePageProgram(FLASH);
        Flash_Addr += FLASH_PAGE_SIZE;                                           /* Point to the start address of the next page to be written */
        src += FLASH_PAGE_SIZE / 4;                                                       /* Point to the next data to be written */
    }

    // 无需编程的残缺页
    if( !Partial_Page_Size )
    {
        LL_FLASH_Lock (FLASH);
        return ;
    }

    // 拷贝残缺页
    memcpy( Part_Memory_Copy , src , Partial_Page_Size );

    // 残缺页编程
    {
        /* Wait Busy=0 */
        while(LL_FLASH_IsActiveFlag_BUSY(FLASH)==1);

        /* Enable EOP */
        LL_FLASH_EnableIT_EOP(FLASH);

        /* Enable Program */
        LL_FLASH_EnablePageProgram(FLASH);

        /* Page Program */
        LL_FLASH_PageProgram( FLASH , Flash_Addr , ( uint32_t * ) Part_Memory_Copy );

        /* Wait Busy=0 */
        while(LL_FLASH_IsActiveFlag_BUSY(FLASH)==1);

        /* Wait EOP=1 */
        while(LL_FLASH_IsActiveFlag_EOP(FLASH)==0);

        /* Clear EOP */
        LL_FLASH_ClearFlag_EOP(FLASH);

        /* Disable EOP */
        LL_FLASH_DisableIT_EOP(FLASH);

        /* Disable Program */
        LL_FLASH_DisablePageProgram(FLASH);

    }


    // 上锁 flash
    LL_FLASH_Lock(FLASH);

}

    

/**
 * @brief MCU Flash 擦除指定页
 * 
 * @param Page_Addr 页的地址
 * @param Page_Number 页数
 * 
 */
void MCU_Flash_Erase( uint32_t Page_Addr , uint32_t Page_Number )
{
    if( Page_Addr & 0x1F )  return ;

    uint32_t flash_program_start = Page_Addr ;              /* Start address of user erase page */
    uint32_t flash_program_end = ( Page_Addr + Page_Number * FLASH_PAGE_SIZE );  /* End address of user erase page */

    /* Unlock flash */
    LL_FLASH_Unlock(FLASH);
    
    /* Erase pages */
    while ( flash_program_start < flash_program_end )
    {
        /* Wait Busy=0 */
        while(LL_FLASH_IsActiveFlag_BUSY(FLASH)==1);

        /* Enable EOP */
        LL_FLASH_EnableIT_EOP(FLASH);

        /* Enable Page Erase */
        LL_FLASH_EnablePageErase(FLASH);

        /* Set Erase Address */
        LL_FLASH_SetEraseAddress(FLASH,flash_program_start);

        /* Wait Busy=0 */
        while(LL_FLASH_IsActiveFlag_BUSY(FLASH)==1);

        /* Wait EOP=1 */
        while(LL_FLASH_IsActiveFlag_EOP(FLASH)==0);

        /* Clear EOP */
        LL_FLASH_ClearFlag_EOP(FLASH);

        /* Disable EOP */
        LL_FLASH_DisableIT_EOP(FLASH);

        /* Disable Page Erase */
        LL_FLASH_DisablePageErase(FLASH);
        flash_program_start += FLASH_PAGE_SIZE;                                           /* Point to the start address of the next page to be erase */
    }


    /* Lock flash */
    LL_FLASH_Lock(FLASH);
}

/**
 * @brief mcu flash 初始化
 * 
 */
void MCU_Flash_Init( void )
{
    LL_FLASH_TIMMING_SEQUENCE_CONFIG_8M();
}

/**
 * @brief 初始化 MCU 的基本功能
 * 分配好各路时钟
 * 重映射GPIO等
 * 
 * 
 */
void MCU_Base_Init( void )
{
    /* Enable SYSCFG and PWR clock */
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
    
    /* Enable and initialize HSI */
    LL_RCC_HSI_Enable();
    LL_RCC_HSI_SetCalibFreq(LL_RCC_HSICALIBRATION_24MHz);
    while(LL_RCC_HSI_IsReady() != 1)
    {
    }
    /* Configure HSISYS as system clock */
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSISYS);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSISYS)
    {
    }
    /* PLL multiplication factor of 3 using HSI (16MHz) */
    LL_RCC_PLL_Disable();
    while(LL_RCC_PLL_IsReady() != 0)
    {
    }
    LL_RCC_PLL_SetMainSource(LL_RCC_PLLSOURCE_HSI);
    LL_RCC_PLL_SetMulFactor(LL_RCC_PLL_MUL_3);
    LL_RCC_PLL_Enable();
    while(LL_RCC_PLL_IsReady() != 1)
    {
    }
    
    /* Set flash latency */
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
    while(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_2)
    {
    }
    
    /* Configure AHB prescaler */
    LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

    /* Configure PLL as system clock and initialize */
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
    {
    }

    /* Configure APB1 prescaler and initialize */
    LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
    LL_Init1msTick(72000000);

    /* Update the SystemCoreClock global variable(which can be updated also through SystemCoreClockUpdate function) */
    LL_SetSystemCoreClock(72000000);  



}


/**
 * @brief 使能 GPIO 时钟
 * 
 * @param GPIOx 
 * 
 */
void MCU_GPIO_CLOCK_ENABLE( MCU_GPIO_TYPE GPIOx )
{
    if( GPIOx == MCU_GPIOA )
    {
        LL_IOP_GRP1_EnableClock( LL_IOP_GRP1_PERIPH_GPIOA );
    }
    else 
    if( GPIOx == MCU_GPIOB )
    {
        LL_IOP_GRP1_EnableClock( LL_IOP_GRP1_PERIPH_GPIOB );
    }
    else 
    if( GPIOx == MCU_GPIOF )
    {
        LL_IOP_GRP1_EnableClock( LL_IOP_GRP1_PERIPH_GPIOF );
    }

}


/**
 * @brief 初始化 MCU 的 ADC
 * 
 */
void MCU_ADC_Init( void ( * DelayMs )( uint32_t ) )
{
    if( MCU_ADC_Init_Flag )    
        return ;
    else
        MCU_ADC_Init_Flag = true ;


    /* Disable ADC1 */
    LL_ADC_Reset( ADC1 );

    /* Enable ADC1 clock */
    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1);
    MCU_GPIO_CLOCK_ENABLE( UC_BATTERY_ADC_VOLTAGE_PORT );
    MCU_GPIO_CLOCK_ENABLE( UC_ENV_TEMP_PORT );

    /* Configure PA4 pin in analog input mode */
    LL_GPIO_SetPinMode( UC_ENV_TEMP_PORT , UC_ENV_TEMP_PIN , LL_GPIO_MODE_ANALOG );
    LL_GPIO_SetPinMode( UC_BATTERY_ADC_VOLTAGE_PORT , UC_BATTERY_ADC_VOLTAGE_PIN , LL_GPIO_MODE_ANALOG );


    /* Set ADC clock to pclk/8 */
    LL_ADC_SetClock(ADC1, LL_ADC_CLOCK_SYNC_PCLK_DIV16);
    

    /* Set ADC resolution to 12 bit */
    LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);

    /* ADC conversion data alignment: right aligned */
    LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);

    /* No ADC low power mode activated */
    LL_ADC_SetLowPowerMode(ADC1, LL_ADC_LP_MODE_NONE);

    /* Sampling time 239.5 ADC clock cycles */
    LL_ADC_SetChannelSamplingTime(ADC1, UC_BATTERY_ADC_VOLTAGE_CHANNEL, LL_ADC_SAMPLINGTIME_239CYCLES_5);
    LL_ADC_SetChannelSamplingTime( ADC1 , UC_ENV_TEMP_ADC_CHANNEL , LL_ADC_SAMPLINGTIME_239CYCLES_5 );


    /* Set vrefbuffer voltage to 2.5V */
    LL_ADC_SetVrefBufferVoltage(ADC1, LL_ADC_VREFBUF_2P5V);
	
    /* Enable Vrefbuffer output */
    LL_ADC_EnableVrefBufferVoltage(ADC1);

    /* ADC regular group conversion trigger from internal: SW start */
    LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);

    /* Set ADC conversion mode to single mode: one conversion per trigger */
    LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);

    /* ADC regular group behavior in case of overrun: data overwritten */
    LL_ADC_REG_SetOverrun(ADC1, LL_ADC_REG_OVR_DATA_OVERWRITTEN);

    /* Disable ADC regular group sequencer discontinuous mode  */
    LL_ADC_REG_SetSequencerDiscont(ADC1, LL_ADC_REG_SEQ_DISCONT_DISABLE);

    /* Dose not enable internal conversion channel */
    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_PATH_INTERNAL_NONE ); 
    
    /* 校准ADC */
    MCU_ADC_Calibrate( 1000 , DelayMs );

    /* Enable ADC */
    LL_ADC_Enable(ADC1);

    /* The delay between ADC enablement and ADC stabilization is at least 8 ADC clocks */
    DelayMs( 1 );
}

/**
 * @brief 校准ADC
 * 
 * @param Timeout 
 */
static void MCU_ADC_Calibrate( uint16_t Timeout , void ( * DelayMs )( uint32_t ) )
{

    if (LL_ADC_IsEnabled(ADC1) == 0)
    {

        /* Enable ADC calibration */
        LL_ADC_StartCalibration(ADC1);

        while ( LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
        {

            /* Detects if the calibration has timed out */
            if (LL_SYSTICK_IsActiveCounterFlag())
            {
                if(Timeout-- == 0)
                {
                    break;
                }
            }   

            /* The delay between the end of ADC calibration and ADC enablement is at least 4 ADC clocks */
            DelayMs( 1 );
        }
    }
}


 
#if LOG_PRINTF_ENABLE
/**
 * @brief MCU 日志初始化
 * 
 * 
 */
void MCU_LOG_Init(  )
{
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    LL_USART_InitTypeDef USART_InitStruct = {0};

    /* Enable GPIOA clock */
    MCU_GPIO_CLOCK_ENABLE( UC_LD_RX_PORT );
    MCU_GPIO_CLOCK_ENABLE( UC_LD_TX_PORT );
    
    /* Enable USART2 clock */
    LL_APB1_GRP1_EnableClock( LL_APB1_GRP1_PERIPH_USART2 ) ;

    /* GPIOA configuration */

    /* Select pin 2 */
    GPIO_InitStruct.Pin = UC_LD_RX_PIN;
    /* Select alternate function mode */
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    /* Set output speed */
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    /* Set output type to push pull */
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    /* Enable pull up */
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    /* Set alternate function to USART2 function  */
    GPIO_InitStruct.Alternate = UC_LD_RX_ALTERNATE;
    /* Initialize GPIOA */
    LL_GPIO_Init( UC_LD_RX_PORT , &GPIO_InitStruct );

    /* Select pin 3 */
    GPIO_InitStruct.Pin = UC_LD_TX_PIN   ;
    /* Set alternate function to USART2 function  */
    GPIO_InitStruct.Alternate = UC_LD_TX_ALTERNATE;
    /* Initialize GPIOA */
    // LL_GPIO_Init( UC_LD_TX_PORT , &GPIO_InitStruct );

    /* Set USART feature */

    /* Set baud rate */
    USART_InitStruct.BaudRate = 57600 ;
    /* set word length to 8 bits: Start bit, 8 data bits, n stop bits */
    USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
    /* 1 stop bit */
    USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
    /* Parity control disabled  */
    USART_InitStruct.Parity = LL_USART_PARITY_NONE;
    USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX ;
    USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;

    /* Initialize USART */
    LL_USART_Init( UC_LD_USART , &USART_InitStruct);

    /* Set mode as full-duplex asynchronous mode */
    LL_USART_ConfigAsyncMode(UC_LD_USART);

    /*Enable USART */
    LL_USART_Enable(UC_LD_USART);

}


/**
 * @brief 重定向printf函数
 * 
 * @param ch 
 * @param f 
 * @return int 
 */
int fputc( int ch, FILE *f )
{
    LL_USART_TransmitData8( LOG_USART , ch );
    while( !LL_USART_IsActiveFlag_TXE( LOG_USART ) );

    return ch ;
}
#endif


/**
 * @brief 关闭所有中断
 * 
 * 
 */
void MCU_Disable_All_IT( void )
{

}

/**
 * @brief MCU 复位 
 * 
 * 
 */
void MCU_Reset( void )
{
    NVIC_SystemReset();
}   

#if MCU_IWDG_ENABLE
/**
 * @brief 看门狗 初始化
 * 
 */
void MCU_IDWG_Init( void )
{
    /* Enable LSI */
    LL_RCC_LSI_Enable();
    while (LL_RCC_LSI_IsReady() == 0U) {;}

    /* Enable IWDG */
    LL_IWDG_Enable(IWDG);

    /* Enable write access */
    LL_IWDG_EnableWriteAccess(IWDG);

    /* Set IWDG prescaler */    /* 32768 / 128 = 256 Hz */
    LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_128 );

    /* Set watchdog reload counter */
    LL_IWDG_SetReloadCounter(IWDG, 2560 ); /* T*1024=1s */

    /* IWDG initialization*/
    while (LL_IWDG_IsReady(IWDG) == 0U) {;}

    /* Feed the watchdog */
    LL_IWDG_ReloadCounter(IWDG);

}


/**
 * @brief 看门狗 喂狗
 * 
 */
void MCU_IDWG_Feed( void )
{
    LL_IWDG_ReloadCounter(IWDG);
}

#endif

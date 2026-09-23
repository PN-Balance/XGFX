/**
 * @file BY25Q80.c
 * @author Pop_Cat (xuzhicheng@longshuo-tech.com)
 * @brief BY25Q80 8-Mbit SPI NOR Flash read driver
 * @version 0.0.0.24.11.08
 * @date 2025-03-31
 * 
 * 
 */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 */
/* -------------------------------------------------------------------------------------------------------------------------- */
#include "BY25Q80.h"
#include "MCU.h"
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 私有宏定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */
#define FLASH_SPI       SPI1
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 外部变量引用声明 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 文件作用域对象 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 内部函数声明 */
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 静态内部函数 ---------------------------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------------------------------------------------------- */
/* 函数定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */


//static uint32_t Temp_Data_Tx ;

void BY25Q80_Init( void )
{
    /* 1.使能时钟 ---------------------------------------- */
    /* Enable clock */
    LL_APB1_GRP2_EnableClock( LL_APB1_GRP2_PERIPH_SPI1 );

    MCU_GPIO_CLOCK_ENABLE( UC_FLASH_CS_PORT );
    MCU_GPIO_CLOCK_ENABLE( UC_FLASH_SCLK_PORT );
    MCU_GPIO_CLOCK_ENABLE( UC_FLASH_MISO_PORT );
    MCU_GPIO_CLOCK_ENABLE( UC_FLASH_MOSI_PORT );

    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);

    /* 2.初始化GPIO -------------------------------------- */
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    LL_GPIO_StructInit( &GPIO_InitStruct );

    /* CS */
    GPIO_InitStruct.Pin = FLASH_PIN_CS;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL ;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init( FLASH_PORT_CS , &GPIO_InitStruct);
    FLASH_CS_SET();
    
    /* SCLK */
    GPIO_InitStruct.Pin = FLASH_PIN_SCLK;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_10 ;
    LL_GPIO_Init( FLASH_PORT_SCLK , &GPIO_InitStruct);

    /* MISO */
    GPIO_InitStruct.Pin = FLASH_PIN_MISO;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init( FLASH_PORT_MISO , &GPIO_InitStruct);

    /* MOSI */
    GPIO_InitStruct.Pin = FLASH_PIN_MOSI;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init( UC_FLASH_MOSI_PORT , &GPIO_InitStruct);

    // /* 3.初始化硬件 SPI ---------------------------------- */
    LL_SPI_InitTypeDef SPI_InitStruct = {0};

    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    /* BY25Q80 latches SI on SCLK rising edges and changes SO on falling
       edges: SPI mode 0. */
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
    /* 72 MHz PCLK only offers power-of-two divisors. DIV2 gives 36 MHz,
     * the nearest available clock to the requested 32 MHz. */
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2 ;
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    SPI_InitStruct.CRCPoly = 0x0U;
    LL_SPI_Init(FLASH_SPI, &SPI_InitStruct);
    
    /* Enable FLASH_SPI */
    LL_SPI_Enable(FLASH_SPI);

}
/**
 * @brief 读取指定地址的数据
 * 
 * @param Address 地址比
 * @param Bytes 
 * @param Size 
 * 
 */
void BY25Q80_Read( uint32_t Address , uint8_t Bytes[  ] , uint32_t Size )
{
    if( Bytes == NULL || Size == 0 || Address >= BY25Q80_CAPACITY_BYTES ) return;
    if( Size > BY25Q80_CAPACITY_BYTES - Address ) Size = BY25Q80_CAPACITY_BYTES - Address;
    FLASH_CS_CLEAR(  );

    //发送读指令
    Flash_SPI_Swap_Data( 0x03 );

    /* 发送地址 */
    Flash_SPI_Swap_Data( Address >> 16 );
    Flash_SPI_Swap_Data( Address >> 8  );
    Flash_SPI_Swap_Data( Address       );

    /* 读取数据 */
    for( uint32_t i = 0 ; i < Size ; i++ )
    {
        Bytes[ i ] = Flash_SPI_Swap_Data( 0xFF ) ;
    }

    FLASH_CS_SET(  );
}


/**
 * @brief 读取 ID 和 制造厂商 
 * 
 * @param Manufactor 
 * @param DeviceID 
 * 
 */
void BY25Q80_Read_JEDEC_ID( uint8_t * Manufactor , uint16_t * DeviceID )
{
    uint8_t Manu ;
    uint16_t Device ;

    FLASH_CS_CLEAR(  );

    Flash_SPI_Swap_Data( 0x9F );

    
    Manu = Flash_SPI_Swap_Data( 0xff );
    if( Manufactor ) *Manufactor = Manu ;

   
    Device = Flash_SPI_Swap_Data( 0xff );
    Device <<= 8 ;
    Device |= Flash_SPI_Swap_Data( 0xff );
    if( DeviceID )  *DeviceID = Device ;
    

    FLASH_CS_SET(  );
}

bool BY25Q80_Is_Present( void )
{
    uint8_t manufacturer = 0;
    uint16_t device = 0;
    BY25Q80_Read_JEDEC_ID( &manufacturer, &device );
    return manufacturer == BY25Q80_JEDEC_MANUFACTURER && device == BY25Q80_JEDEC_DEVICE;
}

/**
 * @brief 交换数据
 * 
 * @param Data 
 * 
 * @return uint8_t 
 */
uint8_t Flash_SPI_Swap_Data( uint8_t Data )
{
    // 等待TXE标志（发送缓冲区为空）
    while (!LL_SPI_IsActiveFlag_TXE( FLASH_SPI ));

    // 发送数据
    LL_SPI_TransmitData8( FLASH_SPI , Data );    

    // 等待RXNE标志（接收缓冲区非空）
    while (!LL_SPI_IsActiveFlag_RXNE( FLASH_SPI ));   
	while ( LL_SPI_IsActiveFlag_BSY( FLASH_SPI ) );

    // 读取接收到的数据
    return LL_SPI_ReceiveData8( FLASH_SPI );
}



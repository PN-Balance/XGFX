/**
 * @file HKND_St7789W3.c
 * @author Pop_Cat (xuzhicheng@longshuo-tech.com)
 * @brief 
 * @version 0.0.0.24.11.08
 * @date 2025-03-20
 * 
 * 
 */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 */
/* -------------------------------------------------------------------------------------------------------------------------- */
#include "HKND_St7789W3.h"
#include "MCU.h"
#include "Delay.h"
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 私有宏定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */
#define ST7789W3_Delay_mS( X ) Delay_ms( X )

#define ST_SPI SPI2
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 外部变量引用声明 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 文件作用域对象 */
/* -------------------------------------------------------------------------------------------------------------------------- */
static Orth_Dir_t Curren_Direction ;

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 内部函数声明 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 函数定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */
/**
 * @brief 设置背光
 * 
 * @param Brightness 
 * 
 */
void ST7789W3_Set_Brighness( uint8_t Brightness )
{
    if( Brightness )
    {
        ST7789W3_LED_High(  );
    }
    else
    {
        ST7789W3_LED_Low(  );
    }
}

/**
 * @brief 初始化 ST7789W3 芯片
 * 
 * @param Direction_Init 初始化默认的屏幕方向
 */
void ST7789W3_Init( Orth_Dir_t Direction_Init )
{
	/* 1.使能时钟 ---------------------------------------- */   
    /* Enable clock */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2);

    MCU_GPIO_CLOCK_ENABLE( ST7789W3_PORT_SCLK );
    MCU_GPIO_CLOCK_ENABLE( ST7789W3_PORT_MOSI );
    MCU_GPIO_CLOCK_ENABLE( ST7789W3_PORT_RS );
    MCU_GPIO_CLOCK_ENABLE( ST7789W3_PORT_CS );
    MCU_GPIO_CLOCK_ENABLE( ST7789W3_PORT_RESET );
    MCU_GPIO_CLOCK_ENABLE( ST7789W3_PORT_LED );


    LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
	
	
    /* 2.初始化GPIO -------------------------------------- */
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

	// ------------ // 硬件 SPI 端口
    LL_GPIO_StructInit( &GPIO_InitStruct );

    /* SCLK */
    GPIO_InitStruct.Pin = ST7789W3_PIN_SCLK ;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_1 ;
    LL_GPIO_Init( ST7789W3_PORT_SCLK , &GPIO_InitStruct);

    /* MOSI */
    GPIO_InitStruct.Pin = ST7789W3_PIN_MOSI ;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE ;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init( ST7789W3_PORT_MOSI , &GPIO_InitStruct);

	// ------------ // 软件端口
    LL_GPIO_StructInit( &GPIO_InitStruct );

    /* CS */
    GPIO_InitStruct.Pin = ST7789W3_PIN_CS;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL ;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_0;
    LL_GPIO_Init( ST7789W3_PORT_CS , &GPIO_InitStruct);


	/* RS */
	GPIO_InitStruct.Pin = ST7789W3_PIN_RS ;
	LL_GPIO_Init( ST7789W3_PORT_RS , &GPIO_InitStruct);
	ST7789W3_RS_High(  );

	// /* CS */
	// GPIO_InitStruct.Pin = ST7789W3_PIN_CS ;
	// LL_GPIO_Init( ST7789W3_PORT_CS , &GPIO_InitStruct);
	// ST7789W3_CS_High(  );
	
	/* RESET */
	GPIO_InitStruct.Pin = ST7789W3_PIN_RESET ;
	LL_GPIO_Init( ST7789W3_PORT_RESET , &GPIO_InitStruct);
	ST7789W3_RESET_High(  );

    /* LED */
    GPIO_InitStruct.Pin = ST7789W3_PIN_LED ;
    LL_GPIO_Init( ST7789W3_PORT_LED , &GPIO_InitStruct);
    ST7789W3_LED_Low(  );

	/* 3.初始化 SPI ---------------------------------- */
    LL_SPI_InitTypeDef  SPI_InitStruct = {0};
    LL_SPI_StructInit( &SPI_InitStruct );

    /* ST_SPI parameter configuration*/
    SPI_InitStruct.TransferDirection = LL_SPI_HALF_DUPLEX_TX ;
    SPI_InitStruct.Mode = LL_SPI_MODE_MASTER;
    SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
    SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
    SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE;
    SPI_InitStruct.NSS = LL_SPI_NSS_SOFT;
    SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV2 ;
    SPI_InitStruct.BitOrder = LL_SPI_MSB_FIRST;
    SPI_InitStruct.CRCCalculation = LL_SPI_CRCCALCULATION_DISABLE;
    SPI_InitStruct.CRCPoly = 0x0U;
    LL_SPI_Init(ST_SPI, &SPI_InitStruct);
    
    /* 4.开启 SPI */
    /* Enable ST_SPI */
    LL_SPI_Enable(ST_SPI);

    // /* 硬件复位 */
	ST7789W3_RESET_Low();
	ST7789W3_Delay_mS(20);
	ST7789W3_RESET_High();
	

	// /* 软件复位 */
	ST7789W3_Write_Address_Of_Data(0x11); //Sleep out 
	ST7789W3_Delay_mS( 120 );       //Delay 120ms 

    /* */
    /* TX 请求发送数据 */
    LL_SYSCFG_SetDMARemap( DMA1, LL_DMA_CHANNEL_1 , LL_SYSCFG_DMA_MAP_SPI2_TX );

    /* 显示方向寄存器 */
	Curren_Direction = Direction_Init ;
	ST7789W3_Write_Address_Of_Data(0x36);
	switch( Curren_Direction )
	{
		case Dir_Bottom :
			ST7789W3_Write_Data_8bit(0x00);
		break;

		case Dir_Top :
			ST7789W3_Write_Data_8bit(0xC0);
		break;

		case Dir_Left :
			ST7789W3_Write_Data_8bit(0x70);
		break;

		case Dir_Right :
			ST7789W3_Write_Data_8bit(0xA0);
		break;
	}


	ST7789W3_Write_Address_Of_Data(0x3A);
	ST7789W3_Write_Data_8bit(0x05);

	ST7789W3_Write_Address_Of_Data(0xB2);
	ST7789W3_Write_Data_8bit(0x0C);
	ST7789W3_Write_Data_8bit(0x0C);
	ST7789W3_Write_Data_8bit(0x00);
	ST7789W3_Write_Data_8bit(0x33);
	ST7789W3_Write_Data_8bit(0x33); 

	ST7789W3_Write_Address_Of_Data(0xB7); 
	ST7789W3_Write_Data_8bit(0x35);  

	ST7789W3_Write_Address_Of_Data(0xBB);
	ST7789W3_Write_Data_8bit(0x35);

	ST7789W3_Write_Address_Of_Data(0xC0);
	ST7789W3_Write_Data_8bit(0x2C);

	ST7789W3_Write_Address_Of_Data(0xC2);
	ST7789W3_Write_Data_8bit(0x01);

	ST7789W3_Write_Address_Of_Data(0xC3);
	ST7789W3_Write_Data_8bit(0x13);   

	ST7789W3_Write_Address_Of_Data(0xC4);
	ST7789W3_Write_Data_8bit(0x20);  

	ST7789W3_Write_Address_Of_Data(0xC6); 
	// ST7789W3_Write_Data_8bit(0x0F);   //60fps 
	ST7789W3_Write_Data_8bit(0x0A);	  //44fps

	ST7789W3_Write_Address_Of_Data(0xD0); 
	ST7789W3_Write_Data_8bit(0xA4);
	ST7789W3_Write_Data_8bit(0xA1);

	ST7789W3_Write_Address_Of_Data(0xD6); 
	ST7789W3_Write_Data_8bit(0xA1);

	ST7789W3_Write_Address_Of_Data(0xE0);
	ST7789W3_Write_Data_8bit(0xF0);
	ST7789W3_Write_Data_8bit(0x00);
	ST7789W3_Write_Data_8bit(0x04);
	ST7789W3_Write_Data_8bit(0x04);
	ST7789W3_Write_Data_8bit(0x04);
	ST7789W3_Write_Data_8bit(0x05);
	ST7789W3_Write_Data_8bit(0x29);
	ST7789W3_Write_Data_8bit(0x33);
	ST7789W3_Write_Data_8bit(0x3E);
	ST7789W3_Write_Data_8bit(0x38);
	ST7789W3_Write_Data_8bit(0x12);
	ST7789W3_Write_Data_8bit(0x12);
	ST7789W3_Write_Data_8bit(0x28);
	ST7789W3_Write_Data_8bit(0x30);

	ST7789W3_Write_Address_Of_Data(0xE1);
	ST7789W3_Write_Data_8bit(0xF0);
	ST7789W3_Write_Data_8bit(0x07);
	ST7789W3_Write_Data_8bit(0x0A);
	ST7789W3_Write_Data_8bit(0x0D);
	ST7789W3_Write_Data_8bit(0x0B);
	ST7789W3_Write_Data_8bit(0x07);
	ST7789W3_Write_Data_8bit(0x28);
	ST7789W3_Write_Data_8bit(0x33);
	ST7789W3_Write_Data_8bit(0x3E);
	ST7789W3_Write_Data_8bit(0x36);
	ST7789W3_Write_Data_8bit(0x14);
	ST7789W3_Write_Data_8bit(0x14);
	ST7789W3_Write_Data_8bit(0x29);
	ST7789W3_Write_Data_8bit(0x32);

	ST7789W3_Write_Address_Of_Data(0x21); 

	ST7789W3_Write_Address_Of_Data(0x11);
	// ST7789W3_Delay_mS(20);	
	ST7789W3_Write_Address_Of_Data(0x29); 


	if( Curren_Direction == Dir_Bottom || Curren_Direction == Dir_Top )
	{
	    ST7789W3_Fill( 0x0000 , 0 , 0 , 172 , 320 );
	}
	else
	{
	    ST7789W3_Fill( 0x0000 , 0 , 0 , 320 , 172 );
	}
}


/**
 * @brief ST7789W3 总线发送一字节数据
 * 
 * @param Byte 要发送的字节
 */
void ST7789W3_Write_Bus( uint8_t Byte ) 
{	
    // 等待TXE标志（发送缓冲区为空）
    while (!LL_SPI_IsActiveFlag_TXE( ST_SPI ));

    // 发送数据
    LL_SPI_TransmitData8( ST_SPI , Byte );    

    // 等待RXNE标志（接收缓冲区非空）
	while ( LL_SPI_IsActiveFlag_BSY( ST_SPI ) );
    
    
    for( uint8_t i = 0 ; i < 4 ; i++ )
    {
        __nop();
    }
}

/**
 * @brief ST7789W3 发送8bit数据
 * 
 * @param Data_8Bit 要发送的8bit数据
 */
void ST7789W3_Write_Data_8bit( uint8_t Data_8Bit )
{
	ST7789W3_RS_High();
	ST7789W3_CS_Low();
	ST7789W3_Write_Bus( Data_8Bit );
	ST7789W3_CS_High();
}

/**
 * @brief ST7789W3 发送16bit数据
 * 
 * @param Data_16Bit 要发送的16bit数据
 */
void ST7789W3_Write_Data_16bit( uint16_t Data_16Bit )
{
	ST7789W3_RS_High();
	ST7789W3_CS_Low();
	ST7789W3_Write_Bus( Data_16Bit >> 8);
	ST7789W3_Write_Bus( Data_16Bit );
	ST7789W3_CS_High();
}

/**
 * @brief ST7789W3 把想要修改的数据寄存器的地址写入
 * 
 * @param Address  要写入的地址
 */
void ST7789W3_Write_Address_Of_Data( uint8_t Address )
{
	ST7789W3_RS_Low();
	ST7789W3_CS_Low();
	ST7789W3_Write_Bus( Address );
	ST7789W3_CS_High();
}

/**
 * @brief 设置数据即将写入的窗口
 * 
 * @param X_Start 窗口左上角坐标
 * @param Y_Start 窗口左上角坐标
 * @param X_End 窗口右下角坐标
 * @param Y_End 窗口右下角坐标
 */
void ST7789W3_Window_Set( uint16_t X_Start , uint16_t Y_Start , uint16_t X_End , uint16_t Y_End )
{ 
	if( Curren_Direction == Dir_Bottom )
	{
		ST7789W3_Write_Address_Of_Data(0x2a);//列地址设置
		ST7789W3_Write_Data_16bit(X_Start +34 );
		ST7789W3_Write_Data_16bit(X_End + 34);
		
		ST7789W3_Write_Address_Of_Data(0x2b);//行地址设置
		ST7789W3_Write_Data_16bit(Y_Start);
		ST7789W3_Write_Data_16bit(Y_End);
		
		ST7789W3_Write_Address_Of_Data(0x2c);//写命令
	}
	else 
	if( Curren_Direction == Dir_Top )
	{
		ST7789W3_Write_Address_Of_Data(0x2a);//列地址设置
		ST7789W3_Write_Data_16bit(X_Start + 34);
		ST7789W3_Write_Data_16bit(X_End + 34 );
        
		ST7789W3_Write_Address_Of_Data(0x2b);//行地址设置
		ST7789W3_Write_Data_16bit(Y_Start);
		ST7789W3_Write_Data_16bit(Y_End );

		ST7789W3_Write_Address_Of_Data(0x2c);//写命令
	}
    else
	if( Curren_Direction == Dir_Left )
	{
		ST7789W3_Write_Address_Of_Data(0x2a);//列地址设置
		ST7789W3_Write_Data_16bit(X_Start);
		ST7789W3_Write_Data_16bit(X_End);

		ST7789W3_Write_Address_Of_Data(0x2b);//行地址设置
		ST7789W3_Write_Data_16bit(Y_Start + 34);
		ST7789W3_Write_Data_16bit(Y_End + 34);

		ST7789W3_Write_Address_Of_Data(0x2c);//写命令
	}
    else
	if( Curren_Direction == Dir_Right )
	{
		ST7789W3_Write_Address_Of_Data(0x2a);//列地址设置
		ST7789W3_Write_Data_16bit(X_Start );
		ST7789W3_Write_Data_16bit(X_End );

		ST7789W3_Write_Address_Of_Data(0x2b);//行地址设置
		ST7789W3_Write_Data_16bit(Y_Start + 34);
		ST7789W3_Write_Data_16bit(Y_End + 34 );

		ST7789W3_Write_Address_Of_Data(0x2c);//写命令
	}
}

/**
 * @brief 设置屏幕的方向
 * 
 * @param Direction 方向
 */
void ST7789W3_Direction_Set( Orth_Dir_t Direction )
{
	Curren_Direction = Direction ;

	ST7789W3_Write_Address_Of_Data(0x36);
	switch( Curren_Direction )
	{
		case Orth_Dir_0 :
			ST7789W3_Write_Data_8bit(0x00);
		break;

		case Orth_Dir_180 :
			ST7789W3_Write_Data_8bit(0xC0);
		break;

		case Orth_Dir_90 :
			ST7789W3_Write_Data_8bit(0x70);
		break;

		case Orth_Dir_270 :
			ST7789W3_Write_Data_8bit(0xA0);
		break;
	}
}

void ST7789W3_Point( int16_t X , int16_t Y , uint16_t Colour )
{
    ST7789W3_Window_Set( X , Y , 1 , 1 );

    ST7789W3_RS_High(  );
	ST7789W3_CS_Low(  );

    ST7789W3_Write_Data_16bit( Colour );

    ST7789W3_CS_High(  );
}
/**
 * @brief ST7789W3 指定区域的 显示缓冲区 设置为同一颜色
 * 
 * @param X 坐标
 * @param Y 坐标
 * @param Width 宽度
 * @param Height 高度
 * @param Colour 颜色
 */
void ST7789W3_Fill( uint16_t Colour , int16_t X , int16_t Y , uint16_t Width ,  uint16_t Height  )
{
    ST7789W3_Window_Set( X , Y , X + Width - 1 , Y + Height - 1 );//填充范围设置

    /* SPI 16 位发送 */
	LL_SPI_Disable(ST_SPI);
    LL_SPI_SetDataWidth( ST_SPI , LL_SPI_DATAWIDTH_16BIT );
    LL_SPI_Enable(ST_SPI); 

	ST7789W3_RS_High(  );
	ST7789W3_CS_Low(  );

    uint8_t DMA_Divide_Times ;
    uint32_t Total_Size = Width * Height ;
    uint32_t Single_Send_Number ;

    if( !Total_Size ) return ;

    volatile uint16_t local_color = Colour;
 

    /* 几次发送 */
    DMA_Divide_Times = ( Total_Size + UINT16_MAX - 1 ) / UINT16_MAX ;


    /* 等DMA发送完成 */
    for( uint8_t Times = 0 ; Times < DMA_Divide_Times ; Times++ )
    {
        /* 计算当前这一次传输的次数 */
        if( Total_Size > UINT16_MAX )
            Single_Send_Number = UINT16_MAX ;
        else 
            Single_Send_Number = Total_Size ;
        
        /* 计算剩余 */
        Total_Size -= Single_Send_Number ;
        
        /* 开始发送 */
        /* Initialize DMA Channel 1 */
        LL_DMA_InitTypeDef DMA_InitStruct;
        DMA_InitStruct.MemoryOrM2MDstAddress  = (uint32_t)&local_color;
        DMA_InitStruct.MemoryOrM2MDstIncMode  = LL_DMA_MEMORY_NOINCREMENT;
        DMA_InitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
        
        DMA_InitStruct.PeriphOrM2MSrcAddress  = (uint32_t)&ST_SPI->DR;
        DMA_InitStruct.PeriphOrM2MSrcIncMode  = LL_DMA_PERIPH_NOINCREMENT;
        DMA_InitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;

        DMA_InitStruct.Direction              = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
        DMA_InitStruct.Mode                   = LL_DMA_MODE_NORMAL;
        
        DMA_InitStruct.NbData                 = Single_Send_Number ;
        DMA_InitStruct.Priority               = LL_DMA_PRIORITY_VERYHIGH ;
        LL_DMA_Init(DMA1, LL_DMA_CHANNEL_1, &DMA_InitStruct);

        /* 开始DMA传输 */
        LL_SPI_EnableDMAReq_TX(ST_SPI);
        LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
        

        /* 等待传输完成 */
        while( !LL_DMA_IsActiveFlag_TC1( DMA1 ) );
        while ( LL_SPI_IsActiveFlag_BSY( ST_SPI ) );
        
        /* 停止DMA传输 */
        LL_SPI_DisableDMAReq_TX(ST_SPI);
        LL_DMA_DisableChannel( DMA1 , LL_DMA_CHANNEL_1 );

        /* 清除标志 */
        LL_DMA_ClearFlag_TC1( DMA1 );

    }


    ST7789W3_CS_High(  );

    /* SPI 8 位发送 */
    LL_SPI_Disable(ST_SPI);
    LL_SPI_SetDataWidth( ST_SPI , LL_SPI_DATAWIDTH_8BIT );
    LL_SPI_Enable(ST_SPI);

}


/**
 * @brief 刷新区域的内容到屏幕
 * 
 * @param X 坐标
 * @param Y 坐标
 * @param Width 宽
 * @param Height 高
 * @param Buffer 缓冲区
 * 
 */
void ST7789W3_Flush( uint16_t * Buffer , int16_t X , int16_t Y , uint16_t Width ,  uint16_t Height  )
{
    ST7789W3_Window_Set( X , Y , X + Width - 1 , Y + Height - 1 );//填充范围设置

    /* SPI 16 位发送 */
	LL_SPI_Disable(ST_SPI);
    LL_SPI_SetDataWidth( ST_SPI , LL_SPI_DATAWIDTH_16BIT );
    LL_SPI_Enable(ST_SPI); 

	ST7789W3_RS_High(  );
	ST7789W3_CS_Low(  );

    uint8_t DMA_Divide_Times ;
    uint32_t Total_Size = Width * Height ;
    uint32_t Single_Send_Number ;

    if( !Total_Size ) return ;

    /* 几次发送 */
    DMA_Divide_Times = ( Total_Size + UINT16_MAX - 1 ) / UINT16_MAX ;


    /* 等DMA发送完成 */
    for( uint8_t Times = 0 ; Times < DMA_Divide_Times ; Times++ )
    {
        /* 计算当前这一次传输的次数 */
        if( Total_Size > UINT16_MAX )
            Single_Send_Number = UINT16_MAX ;
        else 
            Single_Send_Number = Total_Size ;
        
        /* 计算剩余 */
        Total_Size -= Single_Send_Number ;
        
        /* 开始发送 */
        /* Initialize DMA Channel 1 */
        LL_DMA_InitTypeDef DMA_InitStruct;
        DMA_InitStruct.MemoryOrM2MDstAddress  = (uint32_t)Buffer;
        DMA_InitStruct.MemoryOrM2MDstIncMode  = LL_DMA_MEMORY_INCREMENT;
        DMA_InitStruct.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
        
        DMA_InitStruct.PeriphOrM2MSrcAddress  = (uint32_t)&ST_SPI->DR;
        DMA_InitStruct.PeriphOrM2MSrcIncMode  = LL_DMA_PERIPH_NOINCREMENT;
        DMA_InitStruct.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;

        DMA_InitStruct.Direction              = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
        DMA_InitStruct.Mode                   = LL_DMA_MODE_NORMAL;
        
        DMA_InitStruct.NbData                 = Single_Send_Number ;
        DMA_InitStruct.Priority               = LL_DMA_PRIORITY_VERYHIGH ;
        LL_DMA_Init(DMA1, LL_DMA_CHANNEL_1, &DMA_InitStruct);

        /* 开始DMA传输 */
        LL_SPI_EnableDMAReq_TX(ST_SPI);
        LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
        

        /* 等待传输完成 */
        while( !LL_DMA_IsActiveFlag_TC1( DMA1 ) );
        while ( LL_SPI_IsActiveFlag_BSY( ST_SPI ) );
        
        /* 停止DMA传输 */
        LL_SPI_DisableDMAReq_TX(ST_SPI);
        LL_DMA_DisableChannel( DMA1 , LL_DMA_CHANNEL_1 );

        /* 清除标志 */
        LL_DMA_ClearFlag_TC1( DMA1 );

        Buffer += Single_Send_Number ;
    }


    ST7789W3_CS_High(  );

    /* SPI 8 位发送 */
    LL_SPI_Disable(ST_SPI);
    LL_SPI_SetDataWidth( ST_SPI , LL_SPI_DATAWIDTH_8BIT );
    LL_SPI_Enable(ST_SPI);

}

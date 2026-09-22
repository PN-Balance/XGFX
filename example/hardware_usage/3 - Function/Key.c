/**
 * @file Key.c
 * @author Pop_Cat (xuzhicheng@longshuo-tech.com)
 * @brief 按键功能
 * @version 0.0.0.24.11.08
 * @date 2025-03-19
 * 
 * 
 */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 */
/* -------------------------------------------------------------------------------------------------------------------------- */
#include "Key.h"

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 私有宏定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */
#define KEY_TICK_GET_INTERFACE(  )  Sys_Tick_Get(  )
#define KEY_SCAN_TIME   ( 20 ) /* ms */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 外部变量引用声明 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 文件作用域对象 */
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 设备上的所有按键 */
volatile Key_t Device_Keys[ KEY_TOTAL_NUMB ];

/* 事件存放缓冲区 */
volatile Key_Buffer_t Key_Buffer ;

volatile bool Ket_Scan_State = false ;
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 内部函数声明 */
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 缓冲区队列入队 */
static bool Key_Event_Enqueue( uint8_t ID , Key_Event_Type_t Event , uint16_t Times );

/* 缓冲区队列出队 */
static bool Key_Buffer_Dequeue( Key_Event_Node_t * Node );

/* 缓冲区复位 */
static void Key_Buffer_Reset( void );

/* 缓冲区被按下的按键占用 */
static void Key_Buffer_Pressed( uint8_t Pressed_Key );

/* 缓冲区被释放 */
static void Key_Buffer_Idle( void );

/* 占用缓冲区的按键正在老化 */
static void Key_Buffer_Aging( void );

static void Key_All_Wait_Active( void );
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 函数定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */
/**
 * @brief 把一个事件节点存入缓冲区队列
 * 
 * @param Event 事件 
 * @param Times 次数
 * @return bool 是否满队
 */
static bool Key_Event_Enqueue( uint8_t ID , Key_Event_Type_t Event , uint16_t Times )
{
	/* 满队无法入队 */
	if( Key_Buffer.Quantity > KEY_EVENT_BUFF_SIZE )
		return true ;

	/* 赋值 */
	Key_Buffer.Event_Queue[ Key_Buffer.Head ].Event = Event ;
	Key_Buffer.Event_Queue[ Key_Buffer.Head ].ID = ID ;
	Key_Buffer.Event_Queue[ Key_Buffer.Head ].Times = Times ;

	/* 递增数量和索引 */
	Key_Buffer.Quantity++ ;
	CICLE_ADD( Key_Buffer.Head , KEY_EVENT_BUFF_SIZE );

	/* 可以入队 */
	return false ;
}



/**
 * @brief 从缓冲区队列中出队一个节点
 * 
 * @param Node 事件节点
 * @return bool 是否无法出队
 */
static bool Key_Buffer_Dequeue( Key_Event_Node_t * Node )
{
	/* 队列为空无法出队 */
	if( !Key_Buffer.Quantity )
		return true ;

	/* 赋值 */
	Node->Times = Key_Buffer.Event_Queue[ Key_Buffer.Tail ].Times ;
	Node->ID = Key_Buffer.Event_Queue[ Key_Buffer.Tail ].ID ;
	Node->Event = Key_Buffer.Event_Queue[ Key_Buffer.Tail ].Event ;

	/* 递减数量和索引 */
	Key_Buffer.Quantity-- ;
	CICLE_ADD( Key_Buffer.Tail , KEY_EVENT_BUFF_SIZE );

	return false ;
}

/**
 * @brief 清空按键缓冲区
 * 
 */
static void Key_Buffer_Reset( void )
{
	Key_Buffer.Head = 0 ;
	Key_Buffer.Quantity = 0 ;
	Key_Buffer.Tail = 0 ;
	Key_Buffer.State = Key_Buf_State_Idle ;
}

/**
 * @brief 所有按键恢复空闲状态
 * 
 * 
 */
static void Key_All_Wait_Active( void )
{
    for( uint8_t i = 0 ; i < KEY_TOTAL_NUMB ; i++ )
    {
        Device_Keys[ i ].State = Key_State_Wating_Active ;
    }
}

/* 按键按下占用缓冲区 */
static void Key_Buffer_Pressed( uint8_t Pressed_Key )
{
	Key_Buffer.Pressed_Key = Pressed_Key ;
	Key_Buffer.State = Key_Buf_State_Pressed ;
}

/* 按键事件处理完毕，所有的按键恢复空闲 */
static void Key_Buffer_Idle( void )
{
	Key_Buffer.State = Key_Buf_State_Idle ;

	/* 按键事件处理完毕，所有的按键恢复空闲 */
	for( uint8_t i = 0 ; i < KEY_TOTAL_NUMB ; i++ )
	{
		Device_Keys[ i ].State = Key_State_Wating_Active ;
	}
}

/* 按键缓冲区进入老化状态 ， 允许其他按键按下打断 */
static void Key_Buffer_Aging( void )
{
	Key_Buffer.State = Key_Buf_State_Aging ;

	/* 处理当前的按键的时候如果有其他按键处于抖动的状态则重置其他按键为空闲 */
	for( uint8_t i = 0 ; i < KEY_TOTAL_NUMB ; i++ )
	{
		if( i == Key_Buffer.Pressed_Key )
			continue;

		/* 所有其他按键恢复待激活状态 */
		Device_Keys[ i ].State = Key_State_Wating_Active ;
	}
}


/**
 * @brief 按键功能初始化
 * 
 * @param Data 可能传入的数据
 */
void Key_Init( void )
{
    /* 缓冲区复位 */
	Key_Buffer_Reset(  );
    Key_All_Wait_Active(  );


    /* 未指定的初始化按键 初始化为默认状态 */
    Key_Set_t Key_Setting ;
    Key_Set_Struct_Init( &Key_Setting );
    for( uint8_t i = 0 ; i < KEY_TOTAL_NUMB ; i++ )
    {
        Key_Set( i , &Key_Setting );
    }

	/* 初始化按键端口 和 定时器 */
    Key_Port_Input_Init(  );
    Key_Port_Timer_Init(  );


	/* 显式 停止输入按键 */
	Key_Stop(  );
	Ket_Scan_State = false ;
}

/* 清空缓冲区 开始检测输入的按键 */
void Key_Start( void )
{
    Key_Port_Scan( false );

    Key_All_Wait_Active(  );
	Key_Buffer_Reset(  );

	Key_Port_Scan( true );
	Ket_Scan_State = true ;
}

/* 清空缓冲区 停止检测输入的按键 */
void Key_Stop( void )
{
	Key_Port_Scan( false );
	Key_Buffer_Reset(  );

	Ket_Scan_State = false ;
}

/* 读取某个按键的状态 */
bool Key_Is_Pressed( uint8_t Key_ID )
{
	return Key_Port_Is_Pressed( Key_ID );
}

/**
 * @brief 读取按键缓冲区中的内容
 * 
 * @param Size 缓冲区的大小
 * @param Read_Buffer 缓冲区地址
 * @return uint8_t 读取的个数
 */
uint8_t Key_Buffer_Read( uint8_t  Size , Key_Event_Node_t * Read_Buffer )
{
	uint8_t i ;

	if( !Key_Buffer.Quantity )
		return 0 ;

	/* 进入临界区 */
	Key_Port_Scan( false );

	/* 出队 */
	for( i = 0 ; i < Size ; i++ )
	{
		if( Key_Buffer_Dequeue( Read_Buffer + i ) )
		{
			break;
		}
	}

	/* 退出临界区 */
	Key_Port_Scan( true );

	return i ;
}


/**
 * @brief 复位所有按钮 
 * 
 * @param Key_Map 按键位图 
 */
void Key_Reset_ALL( void )
{
    Key_Start(  );
}

/**
 * @brief 设置按键的属性
 * 
 * @param Key_ID    按键id
 * @param Set       按键属性
 * 
 */
void Key_Set( uint8_t Key_ID , Key_Set_t * Set )
{
    if( Key_ID >= KEY_TOTAL_NUMB )  return ;

    Device_Keys[ Key_ID ].Shake = Set->Shake ;

    Device_Keys[ Key_ID ].Long_Enable = Set->Long_Enable ;
    Device_Keys[ Key_ID ].Long = Set->Long ;

    Device_Keys[ Key_ID ].Continue_Enable = Set->Continue_Enable ;

    /* 最大连按数必须大于 2 */
    if( Set->Continue_Max < 2 )
        Device_Keys[ Key_ID ].Continue_Max = 2 ;
    else
        Device_Keys[ Key_ID ].Continue_Max = Set->Continue_Max ;

    Device_Keys[ Key_ID ].Age = Set->Age ;

    Device_Keys[ Key_ID ].Interval_Enable = Set->Interval_Enable ;
    Device_Keys[ Key_ID ].Interval = Set->Interval ;
}

/**
 * @brief 初始化一个 设置按键结构体
 * 
 * @param Set 设置按键结构体
 */
void Key_Set_Struct_Init( Key_Set_t * Set )
{
	Set->Shake = 30 ;

	Set->Long_Enable = true ;
	Set->Long = 1800 ;
	
	Set->Continue_Enable = true ;
	Set->Continue_Max = 3 ;
	Set->Age = 300 ;

	Set->Interval_Enable = false ;
	Set->Interval = 200 ;
}

/**
 * @brief 判断按键事件是否属于按下事件
 * 
 * @param Event 
 * 
 * @return true 
 * @return false 
 */
bool Key_Event_Is_Pressed( Key_Event_Type_t Event )
{
    return ( Key_Event_Press == Event || Key_Event_Continue_Press == Event );
}

/**
 * @brief 判断按键缓冲区是否非空
 * 
 * 
 * @return true 
 * @return false 
 */
bool Key_Buffer_Not_Empty( void )
{
	return !! Key_Buffer.Quantity ;
}

/**
 * @brief 互斥按键读取
 * 
 */
void Key_Scan_Mux_Enter( void )
{
	Key_Port_Scan( false );
}

/**
 * @brief 互斥按键退出
 * 
 */
void Key_Scan_Mux_Exit( void )
{
	if( Ket_Scan_State )
	{
		Key_Port_Scan( true );
	}
}

/**
 * @brief 按键扫描回调函数
 * 
 */
void Key_Call_Back_Scan( void )
{
	uint8_t Key_Start = 0 ;
	uint8_t Key_End = KEY_TOTAL_NUMB ;
	volatile Key_t * Key ;

	/* 缓冲区有按键按下的时候只会处理缓冲区中的按键 */
	/* 减小处理压力 ， 永远只有一个按键被处理 */
	/* 用户任意的输入只有一个按键会响应 */
	switch( Key_Buffer.State )
	{
		/* 缓冲区有活跃的按键 */
		case Key_Buf_State_Pressed :
		{
			Key_Start = Key_Buffer.Pressed_Key ;
			Key_End = Key_Buffer.Pressed_Key + 1 ;
			Device_Keys[ Key_Buffer.Pressed_Key ].Pressed = Key_Port_Is_Pressed( Key_Buffer.Pressed_Key );
		}
		break;

		/* 缓冲区空闲或者老化中 */
		case Key_Buf_State_Idle :
		case Key_Buf_State_Aging :
		{
			Key_Port_Read_State_All( Device_Keys );
		}	
		break;
	}

	/* 扫描按键 */
	for( uint8_t Key_ID = Key_Start ; Key_ID < Key_End ; Key_ID++ )
	{
		/* 讨论的按键对象 */
		Key = Device_Keys + Key_ID ;


		/* 状态机 */
		switch( Key->State )
		{
			/* 等待激活 状态 */
			case Key_State_Wating_Active :
			{
				if( !Key->Pressed )
					Key->State = Key_State_Release ;
			}
			break;


			/* 松开 状态 */
			case Key_State_Release :
			{
				/* 按键依然松开 */
				if( !Key->Pressed )
					break;

				/* 转为抖动状态 */
				Key->State = Key_State_Shake ;

				/* 清零按键计数 */
				Key->Press_Counter = 0 ;
				Key->Press_Times = 0 ;
			}
			break;
			

			/* 抖动 状态 */
			case Key_State_Shake :
			{
				/* 累计按下的时长 */
				Key->Press_Counter += KEY_SCAN_TIME ;

				/* 按键没有持续按下 */
				if( !Key->Pressed )
				{
					/* 恢复松开状态 */
					Key->State = Key_State_Release ;

					/* 按键松开的情况下的异常抖动 */
					if( !Key->Press_Times )
						break;
					
					/* 当前按键连按结束后老化期间抖动了进入  */
					if( Key->Press_Times > 1 && Key_ID == Key_Buffer.Pressed_Key )
					{
						/* >>> 连续短按结束结束事件 */
						Key_Event_Enqueue( Key_ID , Key_Event_Continue_Press_End , Key->Press_Times );

						/* 所有按键等待激活 */
						Key_Buffer_Idle(  );
						break;
					}
				}


				/* 按键持续按下并且超过抖动时长 */
				if( Key->Press_Counter > Key->Shake )
				{
					/* 转为 按下状态 */
					Key->State = Key_State_Pressed ;

					/* 累计按下的次数 */
					Key->Press_Times++ ;

					/* 按键连续按下 */
					if( Key->Press_Times > 1 )
					{
						/* >>> 连续按下事件 */
						Key_Event_Enqueue( Key_ID , Key_Event_Continue_Press , Key->Press_Times );
					}
					/* 按键从松开状态按下 */
					else
					{
						/* 当前按键打断了其他按键的老化 */
						if( Key_Buf_State_Aging == Key_Buffer.State )
						{
							/* 老化中的其他按键恢复待激活状态 */
							Device_Keys[ Key_Buffer.Pressed_Key ].State = Key_State_Wating_Active ;

							if( Device_Keys[ Key_Buffer.Pressed_Key ].Press_Times > 1 )
								/* >>> 连续短按结束事件 */
								Key_Event_Enqueue( Key_Buffer.Pressed_Key , Key_Event_Continue_Press_End , Device_Keys[ Key_Buffer.Pressed_Key ].Press_Times );
                            else
                            if( 1 == Device_Keys[ Key_Buffer.Pressed_Key ].Press_Times )
                            {
                                /* >>> 短按结束事件 */
                                Key_Event_Enqueue( Key_Buffer.Pressed_Key , Key_Event_Short_Press , 0 );
                            }
						}

						/* >>> 按下事件 */
						Key_Event_Enqueue( Key_ID , Key_Event_Press , 0 );
					}

					/* 禁止其他按键 */
					Key_Buffer_Pressed( Key_ID );
					Key_End = Key_ID ;
					break; 
				}
				

			}
			break;


			/* 按下 状态 */
			case Key_State_Pressed :
			{
				/* 累计按下时间 */
				Key->Press_Counter += KEY_SCAN_TIME ;

				/* 按键松开了 */
				if( !Key->Pressed )
				{
					/* 
					按下时长超过长按阈值 并且 使能了长按 发送长按松开按键 ， 
					没有使能长按短按时长超过了阈值视为无效，
					发现按键错按后长按可取消操作 
					*/
					if( Key->Long_Enable && Key->Press_Counter > Key->Long )
					{
						/* >>> 长按松开事件 */
						Key_Event_Enqueue( Key_ID , Key_Event_Long_Press_Release , 0 );

						/* 所有按键等待激活 */
						Key_Buffer_Idle(  );
						break;
					}

					/* 第一次松开 */
					if( 1 == Key->Press_Times )
					{
                        /* 不支持连击的情况下松开直接短按 支持连击的情况下需要转为老化状态后等待老化 */
						/* >>> 短按事件 */
						if( !Key->Continue_Enable ) Key_Event_Enqueue( Key_ID , Key_Event_Short_Press , 0 );
					}
					/* 连续按下后松开 */
					else
					{
						/* >>> 连续短按事件 */
						Key_Event_Enqueue( Key_ID ,  Key_Event_Continue_Short_Press , Key->Press_Times );

						/* 连按到达最大允许值 */
						if(  Key->Press_Times >= Key->Continue_Max )
						{
							/* >>> 连续短按结束事件 */
							Key_Event_Enqueue( Key_ID , Key_Event_Continue_Press_End , Key->Press_Times );
						}
					}

					/* 支持连按并且连按次数未超上限的按键转为老化状态 老化状态允许其他按键打断 */
					if( Key->Continue_Enable && Key->Press_Times < Key->Continue_Max )
					{
						Key->Aging_Counter = 0 ;
						Key->State = Key_State_Aging ;
						Key_Buffer_Aging(  );
					}
					/* 不支持的按键 或者到达最大连按次数转为松开状态 缓冲区转为空闲状态 */
					else
					{
						Key_Buffer_Idle(  );
					}

					break;
				}
				

				/* 按键按下时长首次超过长按阈值 */
				if( Key->Long_Enable && Key->Long <= Key->Press_Counter && Key->Press_Counter < ( Key->Long + KEY_SCAN_TIME ) )
				{
					/* 连续按下途中长按 */
					if( Key->Press_Times > 1 )
					{
						/* >>> 连续短按结束事件 */
						Key_Event_Enqueue( Key_ID , Key_Event_Continue_Press_End , Key->Press_Times - 1 );
					}
					
					/* >>> 长按事件 */
					Key_Event_Enqueue( Key_ID , Key_Event_Long_Press , 0 );

					/* 间隔计数清零 */
					Key_Buffer.Interval_Counter = 0 ;
				}
				else
				/* 超出长按后的连续间隔发送 - 使能的话 */
				if( Key->Interval_Enable && Key->Press_Counter >= ( Key->Long + KEY_SCAN_TIME ) )
				{
					/* 累计间隔时间 */
					Key_Buffer.Interval_Counter += KEY_SCAN_TIME ;

					/* 间隔时间到 */
					if( Key_Buffer.Interval_Counter > Key->Interval )
					{
						/* 累计次数 ， 清除计时 */
						Key_Buffer.Interval_Counter = 0 ;

						/* >>> 间隔发送事件 */
						Key_Event_Enqueue( Key_ID , Key_Event_Interval ,  0 );
					}
				}

			}
			break;

			/* 老化 状态 */
			case Key_State_Aging :
			{
				/* 累计老化时间 */
				Key->Aging_Counter += KEY_SCAN_TIME ;


				/* 按键按下 或者 异常抖动 */
				if( Key->Pressed )
				{
					/* 清零按下计时 */
					Key->Press_Counter = 0 ;

					/* 禁止其他按键 */
					Key_Buffer_Pressed( Key_ID );
					Key_End = Key_ID ;

					/* 转为抖动状态 */
					Key->State = Key_State_Shake ;
					break;
				}


				/* 老化完成 */
				if( Key->Aging_Counter > Key->Age )
				{
					/* 连续短按结束 */
					if( Key->Press_Times > 1 )
					{
						/* >>> 连续短按结束事件 */
						Key_Event_Enqueue( Key_ID , Key_Event_Continue_Press_End , Key->Press_Times );
					}
                    /* 单次按下老化完成 */
                    else
                    if( 1 == Key->Press_Times )
                    {
                        /* >>> 发送短按事件 */
                        Key_Event_Enqueue( Key_ID , Key_Event_Short_Press , 0 );
                    }

					/* 所有按键等待激活 */
					Key_Buffer_Idle(  );
					break;
				}
			}
			break;

		}
	}
}



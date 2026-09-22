/**
 * @file Key.h
 * @author Pop_Cat (xuzhicheng@longshuo-tech.com)
 * @brief 
 * @version 0.0.0.24.11.08
 * @date 2025-07-26
 * 
 * 
 */
#ifndef _KEY_H_
#define _KEY_H_

#ifdef __cplusplus
extern "C" {
#endif

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 */
/* -------------------------------------------------------------------------------------------------------------------------- */
#include "General_Type.h"
#include "User_Config.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdlib.h"
#include "Tick.h"
#include "Easy_Func.h"
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 宏定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* 配置 ******************** */

/* 按键总数 <= 32  */
#define KEY_TOTAL_NUMB ( 3 )     

/* 默认的连按最大值 */
#define DEF_CONTINUE_MAX ( 0xFF )   

/* ************************* */

/* 按键ID */
#define KEY_ID_0	0
#define KEY_ID_1	1
#define KEY_ID_2	2
#define KEY_ID_3	3
#define KEY_ID_4	4
#define KEY_ID_5	5
#define KEY_ID_6	6
#define KEY_ID_7	7
#define KEY_ID_8	8
#define KEY_ID_9	9
#define KEY_ID_10	10
#define KEY_ID_11	11
#define KEY_ID_12	12
#define KEY_ID_13	13
#define KEY_ID_14	14
#define KEY_ID_15	15
#define KEY_ID_16	16
#define KEY_ID_17	17
#define KEY_ID_18	18
#define KEY_ID_19	19
#define KEY_ID_20	20
#define KEY_ID_21	21
#define KEY_ID_22	22
#define KEY_ID_23	23
#define KEY_ID_24	24
#define KEY_ID_25	25
#define KEY_ID_26	26
#define KEY_ID_27	27
#define KEY_ID_28	28
#define KEY_ID_29	29
#define KEY_ID_30	30
#define KEY_ID_31	31


/* id 转位图 */
#define KEY_BIT( X ) ( 0x00000001 << X )

/* 所有的按键 */
#define KEY_ALL ( 0xFFFFFFFF )

/* 有效的按键掩码 */
#define KEY_ALL_VALID   ( 0xFFFFFFFF >> ( 32 - KEY_TOTAL_NUMB ) )

/* 除了或上的按键 */
#define KEY_ALL_BUT( BUT_KEY ) ( ALL_KEY & ( !BUT_KEY ))

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 类型定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* 按键状态类型 */
typedef enum {
    Key_State_Wating_Active , ///< 等待激活

    Key_State_Release , ///< 松开
    Key_State_Shake , ///< 抖动
    Key_State_Pressed , ///< 按下
    Key_State_Aging ,  ///< 老化
} Key_State_t ;


/* 按键产生的事件 */
typedef enum {
    Key_Event_Press = 0 , /* 从松开到按下的那一刻的按下事件 */
    Key_Event_Short_Press = 1 , /* 短按事件 */
    Key_Event_Long_Press = 2 ,  /* 按键按住的时间超过阈值的情况下发送的事件 */
    Key_Event_Long_Press_Release = 3 ,  /* 长按松开事件 */
    Key_Event_Interval = 4 ,    /* 按键按住的时间超过阈值的情况下持续按住 ，定时发送的事件  */
   
    Key_Event_Continue_Press = 5 ,  /* 连续的按下事件 */
    Key_Event_Continue_Short_Press = 6 ,    /* 连续短按事件 */
    Key_Event_Continue_Press_End = 7 ,  /* 连续短按结束事件 */
}Key_Event_Type_t ;

/* 按键对象 */
typedef struct {
    Key_State_t State ; /* 按键的状态 */
    bool Pressed ;    /* 按键是否按下 */
    Tick_t Press_Counter ;  /* 按下时长计数器 */
    uint8_t Press_Times ;   /* 按下的次数 */
    Tick_t Aging_Counter ;  /* 老化时长计数器 */

    uint16_t Shake ;/* 按键抖动的时间 */

    bool Long_Enable ;   /* 长按使能 */
    uint16_t Long ; /* 长按时间 */

    bool Continue_Enable ;    /* 连按使能 */
    uint16_t Age ;  /* 老化时间 */
    uint8_t Continue_Max ;   /* 最多连按次数 */

    bool Interval_Enable ;    /* 长按后间隔事件 */
    uint16_t Interval ; /* 长按间隔事件的间隔时间 */
}Key_t ;


/* 设置按键的属性 */
typedef struct {
    uint16_t Shake ;/* 按键抖动的时间 */

    bool Long_Enable ;   /* 长按使能 */
    uint16_t Long ; /* 长按时间 */

    bool Continue_Enable ;    /* 连按使能 */
    uint16_t Age ;  /* 老化时间 */
    uint8_t Continue_Max ;   /* 最多连按次数 */

    bool Interval_Enable ;    /* 长按后间隔事件 */
    uint16_t Interval ; /* 长按间隔事件的间隔时间 */
} Key_Set_t ;

/* 单个按键事件记录节点 */
typedef struct {
    uint8_t ID ;
    Key_Event_Type_t Event ;
    uint8_t Times ;    /* 连续按下的次数 / 连续发送的次数 */
} Key_Event_Node_t ;


/* 按键初始化结构体 */
typedef struct {
    bool Scan_Enable ;
} Key_Init_t ;


/* 按键缓冲区状态 */
typedef enum {
    Key_Buf_State_Idle ,   /* 空闲 可以处理按键输入 */
    Key_Buf_State_Aging ,  /* 活跃的按键处于老化中 ， 其他按键输入可以抢占缓冲区 */
    Key_Buf_State_Pressed   /* 当前有活跃的按键而且不可被其他按键抢占 */
} Key_Buf_State_t ;



/* 一个按键有事件的时候直接忽略其他按键 
    这样做的原因
    1. 对于非法的输入只会保证一个输入有效 
    2. 减小事务处理压力 ， 多个按键输入就要开启足够的 buff 并且还有按键处理时间过长导致的很久之前的按键得到的不合时间的响应问题
*/
#define KEY_EVENT_BUFF_SIZE ( 20  )  
typedef struct {
    Key_Buf_State_t State ;  /* 缓冲区的状态 */
    
    uint8_t Pressed_Key ;    /* 当前缓冲区的占有按键 */
    uint8_t Head ;  /* 当前缓冲区的按键队列的头 */
    uint8_t Tail ;  /* 当前缓冲区的按键队列的尾 */
    uint8_t Quantity ;   /* 当前按键缓冲区的事件个数 */
    Key_Event_Node_t Event_Queue[ KEY_EVENT_BUFF_SIZE ];    

    uint16_t Interval_Counter ; /* 当前活跃的按键的连续发送计数器 */
    
} Key_Buffer_t ;



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
/* 初始化 */
void Key_Init( void );

/* 清空缓冲区 ， 所有按键恢复等待激活状态 开始检测输入的按键 */
void Key_Start( void );

/* 清空缓冲区 停止检测输入的按键 */
void Key_Stop( void );

/* 读取某个按键的状态 */
bool Key_Is_Pressed( uint8_t Key_ID );

/* 读取按键缓冲区中的内容 */
uint8_t Key_Buffer_Read( uint8_t Size , Key_Event_Node_t * Read_Buffer );

/* 复位所有按钮 */
void Key_Reset_ALL( void );

/* 设置按键的属性 */
void Key_Set( uint8_t Key_ID , Key_Set_t * Set );

/* 初始化设置按键的属性 */
void Key_Set_Struct_Init( Key_Set_t * Set );

/* 判断一个事件是否属于按下事件 */
bool Key_Event_Is_Pressed( Key_Event_Type_t Event );

/* 判断按键缓冲区非空 */
bool Key_Buffer_Not_Empty( void );

// 互斥访问按键输入资源 函数必须成对使用
void Key_Scan_Mux_Enter( void );
void Key_Scan_Mux_Exit( void );
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 函数声明 - 接口 */
/* -------------------------------------------------------------------------------------------------------------------------- */
void Key_Port_Input_Init( void );
void Key_Port_Timer_Init( void );
void Key_Port_Scan( bool New_State );
void Key_Port_Read_State_All( volatile Key_t * Key );
bool Key_Port_Is_Pressed( uint8_t ID );

/* for debug */
/* for factory */
uint16_t prv_Key_ADC_Get( void );
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 函数声明 - 回调 */
/* -------------------------------------------------------------------------------------------------------------------------- */
void Key_Call_Back_Scan( void );

#ifdef __cplusplus
}
#endif

#endif

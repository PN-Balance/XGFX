/**
 * @file Area.c
 * @author Pop_Cat (xuzhicheng@longshuo-tech.com)
 * @brief 
 * @version 0.0.0.24.11.08
 * @date 2026-06-23
 * 
 * 
 */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 - 必须 模块所必须依赖的头文件 */
/* -------------------------------------------------------------------------------------------------------------------------- */
#include "Area.h"
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 - 配置 通过宏定义之类的参数对模块进行配置的头文件 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 - debug 为调试而存在的头文件 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------------------------------------------------------- */
/* 头文件引用 - save 为快速的跨文件变量访问存在的头文件 */
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
/* 内部函数声明 */
/* -------------------------------------------------------------------------------------------------------------------------- */
static int16_t prv_Get_Anchor_Offset_Y( Anchor_t Anchor, uint16_t Height );
static int16_t prv_Get_Anchor_Offset_X( Anchor_t Anchor, uint16_t Width );
/* -------------------------------------------------------------------------------------------------------------------------- */
/* 函数定义 */
/* -------------------------------------------------------------------------------------------------------------------------- */

/**
 * @brief 计算两个区域的交集
 * 
 * @param A1        第一个区域
 * @param A2        第二个区域
 * @param Com       交集区域
 * 
 * @return true      如果有交集
 * @return false     如果没有交集
 */
bool Area_Com( const Area_t * A1, const Area_t * A2, Area_t * Com )
{
    if( Com == NULL ) return false ;
    Com->H = Com->W = Com->X = Com->Y = 0;
 
    if( !Area_Is_Valid( A1 ) || !Area_Is_Valid( A2 ) )
        return false;
    
    // 计算交集的左边界（取两个区域X的最大值）
    int32_t left = (A1->X > A2->X) ? A1->X : A2->X;
    
    // 计算交集的右边界（取两个区域右边界的最小值）
    int32_t a1_right = (int32_t)A1->X + A1->W ;
    int32_t a2_right = (int32_t)A2->X + A2->W ;
    int32_t right = ( a1_right < a2_right ) ? a1_right : a2_right ;
    
    // 计算交集的上边界（取两个区域Y的最大值）
    int32_t top = (A1->Y > A2->Y) ? A1->Y : A2->Y;
    
    // 计算交集的下边界（取两个区域下边界的最小值）
    int32_t a1_bottom = (int32_t)A1->Y + A1->H ;
    int32_t a2_bottom = (int32_t)A2->Y + A2->H ;
    int32_t bottom = ( a1_bottom < a2_bottom ) ? a1_bottom : a2_bottom ;
    
    // 检查是否有交集
    if (left >= right || top >= bottom) {
        return false;  // 没有交集
    }
    
    // 设置交集区域
    Com->X = (int16_t)left;
    Com->Y = (int16_t)top;
    Com->W = (uint16_t)( right - left );
    Com->H = (uint16_t)( bottom - top );
    
    return true;  // 有交集
}

/**
 * @brief 判断一个区域是否在另一个区域内
 * 
 * @param Active    活动区域
 * @param Target    目标区域
 * 
 * @return true      如果Active在Target内
 * @return false     如果Active不在Target内
 */
bool Area_In( const Area_t * Active, const Area_t * Target )
{

    if( !Area_Is_Valid(Active) || !Area_Is_Valid(Target) ) 
        return false ;

    // 检查Active的四个角是否都在Target内
    bool top_left_in = ( Active->X >= Target->X &&
                         Active->Y >= Target->Y &&
                         (int32_t)Active->X + Active->W <= (int32_t)Target->X + Target->W &&
                         (int32_t)Active->Y + Active->H <= (int32_t)Target->Y + Target->H );
                       
    return top_left_in;
}

/**
 * @brief 判断两个区域是否有重叠
 * 
 * @param Active    活动区域
 * @param Target    目标区域
 *  
 * @return true      如果有重叠
 * @return false     如果没有重叠
 */
bool Area_Over( const Area_t * Active, const Area_t * Target )
{
    if( !Area_Is_Valid( Active ) || !Area_Is_Valid( Target ) ) return false ;
    // 检查是否在水平方向有重叠
    bool horizontal_overlap = !((int32_t)Active->X + Active->W <= (int32_t)Target->X || 
                             (int32_t)Target->X + Target->W <= (int32_t)Active->X);
                             
    // 检查是否在垂直方向有重叠
    bool vertical_overlap = !((int32_t)Active->Y + Active->H <= (int32_t)Target->Y || 
                            (int32_t)Target->Y + Target->H <= (int32_t)Active->Y);
                            
    return horizontal_overlap && vertical_overlap;
}

/**
 * @brief 判断Active是否在Target内
 * 
 * @param Active    活动区域
 * @param Target    目标区域
 * 
 * @return true      如果Active在Target内
 * @return false     如果Active不在Target内
 */
bool Area_Contain( const Area_t * Active, const Area_t * Target )
{
    return Area_In( Active , Target ) ;
}

bool Area_Equal( const Area_t * A1 , const Area_t * A2 )
{
    if( !Area_Is_Valid( A1 ) || !Area_Is_Valid( A2 ) )
        return false ;

    return (A1->X == A2->X) && (A1->Y == A2->Y) && (A1->W == A2->W) && (A1->H == A2->H);
}

bool Area_Is_Valid( const Area_t * A )
{
    return A != NULL && A->W > 0 && A->H > 0 ;
}

/**
 * @brief 获取区域指定锚点的X坐标
 * 
 * @param Area_t *      指向区域结构的指针
 * @param Anchor    锚点类型，定义在Anchor_t枚举中
 * @return int16_t  指定锚点的X坐标
 * 
 * @note 区域始终存储左上角的绝对坐标，此函数计算并返回指定锚点的X坐标
 * @see Anchor_t
 */
int16_t Area_Get_Anchor_X( const Area_t * A, Anchor_t Anchor) 
{
    if( A == NULL ) return 0 ;
    return A->X + prv_Get_Anchor_Offset_X(Anchor, A->W);
}

/**
 * @brief 获取区域指定锚点的Y坐标
 * 
 * @param Area_t *      指向区域结构的指针
 * @param Anchor    锚点类型，定义在Anchor_t枚举中
 * @return int16_t  指定锚点的Y坐标
 * 
 * @note 区域始终存储左上角的绝对坐标，此函数计算并返回指定锚点的Y坐标
 * @see Anchor_t
 */
int16_t Area_Get_Anchor_Y( const Area_t * A, Anchor_t Anchor) 
{
    if( A == NULL ) return 0 ;
    return A->Y + prv_Get_Anchor_Offset_Y(Anchor, A->H);
}

/**
 * @brief 通过指定的锚点重新定位一个区域
 * 
 * @param A         指向要移动的区域结构的指针
 * @param Anchor    指定作为基准的锚点类型
 * @param X         目标锚点的X坐标
 * @param Y         目标锚点的Y坐标
 * 
 * @see Area_Get_Anchor_X
 * @see Area_Get_Anchor_Y
 */
void Area_Move( Area_t * A , Anchor_t Anchor , int16_t X , int16_t Y ) 
{
    if( A == NULL ) return ;
    A->X = X - prv_Get_Anchor_Offset_X( Anchor , A->W );
    A->Y = Y - prv_Get_Anchor_Offset_Y( Anchor , A->H );
}

/**
 * @brief 在当前位置基础上偏移区域
 * 
 * @param A         指向要移动的区域结构的指针
 * @param X_Offset  X方向偏移量
 * @param Y_Offset  Y方向偏移量
 * 
 * @note 此函数简单地在当前坐标基础上加上偏移量，实现区域移动
 */
void Area_Move_Offset( Area_t * A, int16_t X_Offset, int16_t Y_Offset) {
    if( A == NULL ) return ;
    A->X += X_Offset;
    A->Y += Y_Offset;
}


/**
 * @brief 获取锚点相对于左上角的偏移量
 * 
 * @param Anchor     锚点类型
 * @param Width      区域宽度
 * 
 * @return int16_t 
 */
static int16_t prv_Get_Anchor_Offset_X(Anchor_t Anchor, uint16_t Width) 
{
    if( 0 == Width )
        return 0;

    switch (Anchor) {
        case Anchor_LT: case Anchor_LM: case Anchor_LB:
            return 0;                     // 左边界
        case Anchor_CT: case Anchor_CM: case Anchor_CB:
            return Width / 2;       // 水平中心
        case Anchor_RT: case Anchor_RM: case Anchor_RB:
            return Width - 1;             // 右边界
    }
    return 0;
}

/**
 * @brief 获取锚点相对于左上角的偏移量
 * 
 * @param Anchor 
 * @param Height 
 * 
 * @return int16_t 
 */
static int16_t prv_Get_Anchor_Offset_Y(Anchor_t Anchor, uint16_t Height) 
{
    if( 0 == Height )
        return 0;

    switch (Anchor) {
        case Anchor_LT: case Anchor_CT: case Anchor_RT:
            return 0;                     // 上边界
        case Anchor_LM: case Anchor_CM: case Anchor_RM:
            return Height / 2;      // 垂直中心
        case Anchor_LB: case Anchor_CB: case Anchor_RB:
            return Height - 1;            // 下边界
    }
    return 0;
}

/**
 * @brief 
 * 
 * @param A 
 * @param Offset 
 * @param Anchor 
 * 
 */
void Area_Inflate( Area_t * A , int16_t Offset , Anchor_t Anchor )
{
    if( A == NULL ) return ;
    uint16_t Delta = Offset < 0 ? -Offset : Offset;
    Delta *= 2 ;
    Area_t Raw_Area = { .X = A->X , .Y = A->Y , .W = A->W , .H = A->H };
    int16_t X , Y ;
    if( Offset < 0 )
    {
        if( Delta >= A->W )
            A->W = 0;

        if( Delta >= A->H )
            A->H = 0;

        if( 0 == A->W || 0 == A->H )
        {
            A->W = A->H = 0 ;
            return ;
        }

        A->W -= Delta ;
        A->H -= Delta ;
    }
    else
    {
        if ( A->W > UINT16_MAX - Delta )
            A->W = UINT16_MAX;
        else
            A->W += Delta;

        if ( A->H > UINT16_MAX - Delta )
            A->H = UINT16_MAX;
        else
            A->H += Delta;
    }

    X = Area_Get_Anchor_X( &Raw_Area , Anchor );
    Y = Area_Get_Anchor_Y( &Raw_Area , Anchor );
    Area_Move( A , Anchor , X , Y );
}

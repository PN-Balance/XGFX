#include "User.h"
#include "MCU.h"
#include "Delay.h"
#include "Tick.h"
#include "Power.h"
#include "GFX_Hardware_Test.h"

void User_Main( void )
{
    MCU_Init();
    Delay_Init();
    Tick_Init();
    Power_Port_Power_Reset_Periph_Init();
    Power_ON;
    GFX_Hardware_Test_Run();
}

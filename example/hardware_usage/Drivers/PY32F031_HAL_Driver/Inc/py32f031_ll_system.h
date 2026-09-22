/**
  ******************************************************************************
  * @file    py32f031_ll_system.h
  * @author  MCU Application Team
  * @brief   Header file of SYSTEM LL module.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 Puya Semiconductor Co.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by Puya under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef PY32F031_LL_SYSTEM_H
#define PY32F031_LL_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "py32f0xx.h"

/** @addtogroup PY32F031_LL_Driver
  * @{
  */

#if defined (FLASH) || defined (SYSCFG) || defined (DBGMCU)

/** @defgroup SYSTEM_LL SYSTEM
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
/** @defgroup SYSTEM_LL_Private_Constants SYSTEM Private Constants
  * @{
  */

/**
  * @}
  */

/* Private macros ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup SYSTEM_LL_Exported_Constants SYSTEM Exported Constants
  * @{
  */

/** @defgroup SYSTEM_LL_EC_REMAP SYSCFG REMAP
  * @{
  */
#define LL_SYSCFG_REMAP_FLASH               0x00000000U                                           /*!< Main Flash memory mapped at 0x00000000 */
#define LL_SYSCFG_REMAP_SYSTEMFLASH         SYSCFG_CFGR1_MEM_MODE_0                               /*!< System Flash memory mapped at 0x00000000 */
#define LL_SYSCFG_REMAP_SRAM                (SYSCFG_CFGR1_MEM_MODE_1 | SYSCFG_CFGR1_MEM_MODE_0)   /*!< Embedded SRAM mapped at 0x00000000 */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_TIM1_IC1_SRC TIM1 IC1 SOURCE
  * @{
  */
#define LL_SYSCFG_TIM1_IC1_SRC_IO            0x00000000U
#if defined(COMP1)
#define LL_SYSCFG_TIM1_IC1_SRC_COMP1         SYSCFG_CFGR1_TIM1_IC1_SRC_0
#endif
#if defined(COMP2)
#define LL_SYSCFG_TIM1_IC1_SRC_COMP2         SYSCFG_CFGR1_TIM1_IC1_SRC_1
#endif
/**
  * @}
  */


/** @defgroup SYSTEM_LL_EC_TIM2_IC4_SRC TIM2 IC4 SOURCE
  * @{
  */
#define LL_SYSCFG_TIM2_IC4_SRC_IO            0x00000000U
#if defined(COMP1)
#define LL_SYSCFG_TIM2_IC4_SRC_COMP1         SYSCFG_CFGR1_TIM2_IC4_SRC_0
#endif
#if defined(COMP2)
#define LL_SYSCFG_TIM2_IC4_SRC_COMP2         SYSCFG_CFGR1_TIM2_IC4_SRC_1
#endif
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_ETR_SRC_TIM1 TIM1 ETR SOURCE
  * @{
  */
#define LL_SYSCFG_ETR_SRC_TIM1_GPIO          0x00000000U
#if defined(COMP1)
#define LL_SYSCFG_ETR_SRC_TIM1_COMP1         SYSCFG_CFGR1_ETR_SRC_TIM1_0
#endif
#if defined(COMP2)
#define LL_SYSCFG_ETR_SRC_TIM1_COMP2         SYSCFG_CFGR1_ETR_SRC_TIM1_1
#endif
#define LL_SYSCFG_ETR_SRC_TIM1_ADC           (SYSCFG_CFGR1_ETR_SRC_TIM1_1 | SYSCFG_CFGR1_ETR_SRC_TIM1_0)
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_ETR_SRC_TIM2 TIM2 ETR SOURCE
  * @{
  */
#define LL_SYSCFG_ETR_SRC_TIM2_GPIO          0x00000000U
#if defined(COMP1)
#define LL_SYSCFG_ETR_SRC_TIM2_COMP1         SYSCFG_CFGR1_ETR_SRC_TIM2_0
#endif
#if defined(COMP2)
#define LL_SYSCFG_ETR_SRC_TIM2_COMP2         SYSCFG_CFGR1_ETR_SRC_TIM2_1
#endif
#define LL_SYSCFG_ETR_SRC_TIM2_ADC           (SYSCFG_CFGR1_ETR_SRC_TIM2_1 | SYSCFG_CFGR1_ETR_SRC_TIM2_0)
/**
  * @}
  */

/** @defgroup SYSTEM_LL_GPIO_SEL GPIO SEL
  * @{
  */
#define LL_SYSCFG_GPIO_SEL_FASTIO          0x00000000U 
#define LL_SYSCFG_GPIO_SEL_AHB             SYSCFG_CFGR1_GPIO_AHB_SEL
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_TIMBREAK TIMER BREAK INPUT
  * @{
  */
#if defined(SYSCFG_CFGR2_LOCKUP_LOCK)
#define LL_SYSCFG_TIMBREAK_LOCKUP_TO_ALL      SYSCFG_CFGR2_LOCKUP_LOCK
#endif
#if defined(SYSCFG_CFGR2_PVD_LOCK)
#define LL_SYSCFG_TIMBREAK_PVD_TO_ALL         SYSCFG_CFGR2_PVD_LOCK
#endif
#if defined(SYSCFG_CFGR2_COMP1_BRK_TIM1)
#define LL_SYSCFG_TIMBREAK_COMP1_TO_TIM1      SYSCFG_CFGR2_COMP1_BRK_TIM1
#endif
#if defined(SYSCFG_CFGR2_COMP2_BRK_TIM1)
#define LL_SYSCFG_TIMBREAK_COMP2_TO_TIM1      SYSCFG_CFGR2_COMP2_BRK_TIM1
#endif

#if defined(SYSCFG_CFGR2_COMP1_BRK_TIM16)
#define LL_SYSCFG_TIMBREAK_COMP1_TO_TIM16     SYSCFG_CFGR2_COMP1_BRK_TIM16
#endif
#if defined(SYSCFG_CFGR2_COMP2_BRK_TIM16)
#define LL_SYSCFG_TIMBREAK_COMP2_TO_TIM16     SYSCFG_CFGR2_COMP2_BRK_TIM16
#endif

#if defined(SYSCFG_CFGR2_COMP1_BRK_TIM17)
#define LL_SYSCFG_TIMBREAK_COMP1_TO_TIM17     SYSCFG_CFGR2_COMP1_BRK_TIM17
#endif
#if defined(SYSCFG_CFGR2_COMP2_BRK_TIM17)
#define LL_SYSCFG_TIMBREAK_COMP2_TO_TIM17     SYSCFG_CFGR2_COMP2_BRK_TIM17
#endif
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_TIMOCREF_CLR TIMER OCREF INPUT  
  * @{
  */
#if defined(SYSCFG_CFGR2_COMP1_OCREF_CLR_TIM1)
#define LL_SYSCFG_OCREF_CLR_TIM1_COMP1         SYSCFG_CFGR2_COMP1_OCREF_CLR_TIM1
#endif
#if defined(SYSCFG_CFGR2_COMP1_OCREF_CLR_TIM2)
#define LL_SYSCFG_OCREF_CLR_TIM2_COMP1         SYSCFG_CFGR2_COMP1_OCREF_CLR_TIM2
#endif
#if defined(SYSCFG_CFGR2_COMP2_OCREF_CLR_TIM1)
#define LL_SYSCFG_OCREF_CLR_TIM1_COMP2         SYSCFG_CFGR2_COMP2_OCREF_CLR_TIM1
#endif
#if defined(SYSCFG_CFGR2_COMP2_OCREF_CLR_TIM2)
#define LL_SYSCFG_OCREF_CLR_TIM2_COMP2         SYSCFG_CFGR2_COMP2_OCREF_CLR_TIM2
#endif
/**
  * @}
  */


#if (defined(DMA) || defined(DMA1))
/** @defgroup SYSTEM_LL_EC_DMA_MAP DMA MAP 
  * @{
  */
#define LL_SYSCFG_DMA_MAP_ADC            (0x00000000U)
#define LL_SYSCFG_DMA_MAP_SPI1_TX        (                                                                                                                                  SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_SPI1_RX        (                                                                                                        SYSCFG_CFGR3_DMA1_MAP_1                          )
#define LL_SYSCFG_DMA_MAP_SPI2_TX        (                                                                                                        SYSCFG_CFGR3_DMA1_MAP_1 | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_SPI2_RX        (                                                                              SYSCFG_CFGR3_DMA1_MAP_2                                                    )
#define LL_SYSCFG_DMA_MAP_USART1_TX      (                                                                              SYSCFG_CFGR3_DMA1_MAP_2                           | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_USART1_RX      (                                                                              SYSCFG_CFGR3_DMA1_MAP_2 | SYSCFG_CFGR3_DMA1_MAP_1                          )
#define LL_SYSCFG_DMA_MAP_USART2_TX      (                                                                              SYSCFG_CFGR3_DMA1_MAP_2 | SYSCFG_CFGR3_DMA1_MAP_1 | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_USART2_RX      (                                                    SYSCFG_CFGR3_DMA1_MAP_3                                                                              )
#define LL_SYSCFG_DMA_MAP_I2C1_TX        (                                                    SYSCFG_CFGR3_DMA1_MAP_3                                                     | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_I2C1_RX        (                                                    SYSCFG_CFGR3_DMA1_MAP_3                           | SYSCFG_CFGR3_DMA1_MAP_1                          )
#define LL_SYSCFG_DMA_MAP_TIM1_CH1       (                                                    SYSCFG_CFGR3_DMA1_MAP_3                           | SYSCFG_CFGR3_DMA1_MAP_1 | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_TIM1_CH2       (                                                    SYSCFG_CFGR3_DMA1_MAP_3 | SYSCFG_CFGR3_DMA1_MAP_2                                                    )
#define LL_SYSCFG_DMA_MAP_TIM1_CH3       (                                                    SYSCFG_CFGR3_DMA1_MAP_3 | SYSCFG_CFGR3_DMA1_MAP_2                           | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_TIM1_CH4       (                                                    SYSCFG_CFGR3_DMA1_MAP_3 | SYSCFG_CFGR3_DMA1_MAP_2 | SYSCFG_CFGR3_DMA1_MAP_1                          )
#define LL_SYSCFG_DMA_MAP_TIM1_COM       (                                                    SYSCFG_CFGR3_DMA1_MAP_3 | SYSCFG_CFGR3_DMA1_MAP_2 | SYSCFG_CFGR3_DMA1_MAP_1 | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_TIM1_UP        (                          SYSCFG_CFGR3_DMA1_MAP_4                                                                                                        )
#define LL_SYSCFG_DMA_MAP_TIM1_TRG       (                          SYSCFG_CFGR3_DMA1_MAP_4                                                                               | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_TIM2_CH1       (                          SYSCFG_CFGR3_DMA1_MAP_4                                                     | SYSCFG_CFGR3_DMA1_MAP_1                          )
#define LL_SYSCFG_DMA_MAP_TIM2_CH3       (                          SYSCFG_CFGR3_DMA1_MAP_4                                                     | SYSCFG_CFGR3_DMA1_MAP_1 | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_TIM2_CH4       (                          SYSCFG_CFGR3_DMA1_MAP_4                           | SYSCFG_CFGR3_DMA1_MAP_2                                                    )
#define LL_SYSCFG_DMA_MAP_TIM2_TRG       (                          SYSCFG_CFGR3_DMA1_MAP_4                           | SYSCFG_CFGR3_DMA1_MAP_2                           | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_TIM2_UP        (                          SYSCFG_CFGR3_DMA1_MAP_4                           | SYSCFG_CFGR3_DMA1_MAP_2 | SYSCFG_CFGR3_DMA1_MAP_1                          )
#define LL_SYSCFG_DMA_MAP_TIM2_CH2       (                          SYSCFG_CFGR3_DMA1_MAP_4                           | SYSCFG_CFGR3_DMA1_MAP_2 | SYSCFG_CFGR3_DMA1_MAP_1 | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_TIM16_CH1      (                          SYSCFG_CFGR3_DMA1_MAP_4 | SYSCFG_CFGR3_DMA1_MAP_3                                                                              )
#define LL_SYSCFG_DMA_MAP_TIM16_UP       (                          SYSCFG_CFGR3_DMA1_MAP_4 | SYSCFG_CFGR3_DMA1_MAP_3                                                     | SYSCFG_CFGR3_DMA1_MAP_0)
#define LL_SYSCFG_DMA_MAP_TIM17_CH1      (                          SYSCFG_CFGR3_DMA1_MAP_4 | SYSCFG_CFGR3_DMA1_MAP_3                           | SYSCFG_CFGR3_DMA1_MAP_1                          )
#define LL_SYSCFG_DMA_MAP_TIM17_UP       (                          SYSCFG_CFGR3_DMA1_MAP_4 | SYSCFG_CFGR3_DMA1_MAP_3                           | SYSCFG_CFGR3_DMA1_MAP_1 | SYSCFG_CFGR3_DMA1_MAP_0)
#if defined(USART3)
#define LL_SYSCFG_DMA_MAP_USART3_TX      (                          SYSCFG_CFGR3_DMA1_MAP_4 | SYSCFG_CFGR3_DMA1_MAP_3 | SYSCFG_CFGR3_DMA1_MAP_2                                                    )
#define LL_SYSCFG_DMA_MAP_USART3_RX      (                          SYSCFG_CFGR3_DMA1_MAP_4 | SYSCFG_CFGR3_DMA1_MAP_3 | SYSCFG_CFGR3_DMA1_MAP_2                           | SYSCFG_CFGR3_DMA1_MAP_0)
#endif
#if defined(I2C2)
#define LL_SYSCFG_DMA_MAP_I2C2_TX        (                          SYSCFG_CFGR3_DMA1_MAP_4 | SYSCFG_CFGR3_DMA1_MAP_3 | SYSCFG_CFGR3_DMA1_MAP_2 | SYSCFG_CFGR3_DMA1_MAP_1                          )
#define LL_SYSCFG_DMA_MAP_I2C2_RX        (                          SYSCFG_CFGR3_DMA1_MAP_4 | SYSCFG_CFGR3_DMA1_MAP_3 | SYSCFG_CFGR3_DMA1_MAP_2 | SYSCFG_CFGR3_DMA1_MAP_1 | SYSCFG_CFGR3_DMA1_MAP_0)
#endif
#if defined(LCD)
#define LL_SYSCFG_DMA_MAP_LCD            (SYSCFG_CFGR3_DMA1_MAP_5                                                                                                                                  )
#endif
#endif /* DMA Or DMA1 */
/**
  * @}
  */

/** @defgroup SYSCFG_EIIC I2C PIN SIGNAL 
  * @{
  */
#define LL_SYSCFG_EIIC_PA2          SYSCFG_IOCFG_PA_EIIC_0
#define LL_SYSCFG_EIIC_PA3          SYSCFG_IOCFG_PA_EIIC_1
#define LL_SYSCFG_EIIC_PA7          SYSCFG_IOCFG_PA_EIIC_2
#define LL_SYSCFG_EIIC_PA8          SYSCFG_IOCFG_PA_EIIC_3
#define LL_SYSCFG_EIIC_PA9          SYSCFG_IOCFG_PA_EIIC_4
#define LL_SYSCFG_EIIC_PA10         SYSCFG_IOCFG_PA_EIIC_5
#define LL_SYSCFG_EIIC_PA11         SYSCFG_IOCFG_PA_EIIC_6
#define LL_SYSCFG_EIIC_PA12         SYSCFG_IOCFG_PA_EIIC_7
#define LL_SYSCFG_EIIC_PB6          SYSCFG_IOCFG_PB_EIIC_0
#define LL_SYSCFG_EIIC_PB7          SYSCFG_IOCFG_PB_EIIC_1
#define LL_SYSCFG_EIIC_PB8          SYSCFG_IOCFG_PB_EIIC_2
#define LL_SYSCFG_EIIC_PB10         SYSCFG_IOCFG_PB_EIIC_3
#define LL_SYSCFG_EIIC_PB11         SYSCFG_IOCFG_PB_EIIC_4
#define LL_SYSCFG_EIIC_PF0          SYSCFG_IOCFG_PF_EIIC_0
#define LL_SYSCFG_EIIC_PF1          SYSCFG_IOCFG_PF_EIIC_1
#define LL_SYSCFG_EIIC_PF5          SYSCFG_IOCFG_PF_EIIC_2
#define LL_SYSCFG_EIIC_PF6          SYSCFG_IOCFG_PF_EIIC_3
/**
  * @}
  */

/** @defgroup SYSCFG_EHS LED PIN HIGH DRIVER SIGNAL 
  * @{
  */
#define LL_SYSCFG_EHS_PA0           SYSCFG_IOCFG_PA_EHS_0
#define LL_SYSCFG_EHS_PA15          SYSCFG_IOCFG_PA_EHS_1
#define LL_SYSCFG_EHS_PB2           SYSCFG_IOCFG_PB_EHS_0
#define LL_SYSCFG_EHS_PB3           SYSCFG_IOCFG_PB_EHS_1
#define LL_SYSCFG_EHS_PB4           SYSCFG_IOCFG_PB_EHS_2
#define LL_SYSCFG_EHS_PB5           SYSCFG_IOCFG_PB_EHS_3
#define LL_SYSCFG_EHS_PB6           SYSCFG_IOCFG_PB_EHS_4
#define LL_SYSCFG_EHS_PB7           SYSCFG_IOCFG_PB_EHS_5
/**
  * @}
  */

/** @defgroup SYSCFG_PU_IIC I2C PIN PULL-UP 
  * @{
  */
#define LL_SYSCFG_PU_IIC_PF5         SYSCFG_IOCFG_PF_PU_IIC_0
#define LL_SYSCFG_PU_IIC_PF6         SYSCFG_IOCFG_PF_PU_IIC_1
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_GPIO_PORT GPIO PORT
  * @{
  */
#define LL_SYSCFG_GPIO_PORTA              0x00000000U
#define LL_SYSCFG_GPIO_PORTB              0x00000001U
#define LL_SYSCFG_GPIO_PORTF              0x00000002U
/**
  * @}
  */
  
/** @defgroup SYSTEM_LL_EC_GPIO_PIN GPIO PIN
  * @{
  */
#define LL_SYSCFG_GPIO_PIN_0              0x00000001U
#define LL_SYSCFG_GPIO_PIN_1              0x00000002U
#define LL_SYSCFG_GPIO_PIN_2              0x00000004U
#define LL_SYSCFG_GPIO_PIN_3              0x00000008U
#define LL_SYSCFG_GPIO_PIN_4              0x00000010U
#define LL_SYSCFG_GPIO_PIN_5              0x00000020U
#define LL_SYSCFG_GPIO_PIN_6              0x00000040U
#define LL_SYSCFG_GPIO_PIN_7              0x00000080U
#define LL_SYSCFG_GPIO_PIN_8              0x00000100U
#define LL_SYSCFG_GPIO_PIN_9              0x00000200U
#define LL_SYSCFG_GPIO_PIN_10             0x00000400U
#define LL_SYSCFG_GPIO_PIN_11             0x00000800U
#define LL_SYSCFG_GPIO_PIN_12             0x00001000U
#define LL_SYSCFG_GPIO_PIN_13             0x00002000U
#define LL_SYSCFG_GPIO_PIN_14             0x00004000U
#define LL_SYSCFG_GPIO_PIN_15             0x00008000U
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_LATENCY FLASH LATENCY
  * @{
  */
#define LL_FLASH_LATENCY_0                 0x00000000U               /*!< FLASH Zero Latency cycle */
#define LL_FLASH_LATENCY_1                 FLASH_ACR_LATENCY_0       /*!< FLASH One Latency cycle */
#define LL_FLASH_LATENCY_2                 FLASH_ACR_LATENCY_1       /*!< FLASH Two Latency cycle */
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_APB1_GRP1_STOP_IP  DBGMCU APB1 GRP1 STOP IP
  * @{
  */
#if defined(DBGMCU_APB_FZ1_DBG_TIM2_STOP)
#define LL_DBGMCU_APB1_GRP1_TIM2_STOP      DBGMCU_APB_FZ1_DBG_TIM2_STOP        /*!< TIM2 counter stopped when core is halted */
#endif
#if defined(DBGMCU_APB_FZ1_DBG_RTC_STOP)
#define LL_DBGMCU_APB1_GRP1_RTC_STOP       DBGMCU_APB_FZ1_DBG_RTC_STOP         /*!< RTC Calendar frozen when core is halted */
#endif
#if defined(DBGMCU_APB_FZ1_DBG_WWDG_STOP)
#define LL_DBGMCU_APB1_GRP1_WWDG_STOP      DBGMCU_APB_FZ1_DBG_WWDG_STOP        /*!< Debug Window Watchdog stopped when Core is halted */
#endif
#if defined(DBGMCU_APB_FZ1_DBG_IWDG_STOP)
#define LL_DBGMCU_APB1_GRP1_IWDG_STOP      DBGMCU_APB_FZ1_DBG_IWDG_STOP        /*!< Debug Independent Watchdog stopped when Core is halted */
#endif

#if defined(DBGMCU_APB_FZ1_DBG_LPTIM_STOP)
#define LL_DBGMCU_APB1_GRP1_LPTIM1_STOP    DBGMCU_APB_FZ1_DBG_LPTIM_STOP      /*!< LPTIM1 counter stopped when Core is halted */
#endif
/**
  * @}
  */

/** @defgroup SYSTEM_LL_EC_APB2_GRP1_STOP_IP DBGMCU APB2 GRP1 STOP IP
  * @{
  */
#if defined(DBGMCU_APB_FZ2_DBG_TIM1_STOP)
#define LL_DBGMCU_APB1_GRP2_TIM1_STOP      DBGMCU_APB_FZ2_DBG_TIM1_STOP        /*!< TIM1 counter stopped when core is halted */
#endif
#if defined(DBGMCU_APB_FZ2_DBG_TIM14_STOP)
#define LL_DBGMCU_APB1_GRP2_TIM14_STOP     DBGMCU_APB_FZ2_DBG_TIM14_STOP       /*!< TIM14 counter stopped when core is halted */
#endif
#if defined(DBGMCU_APB_FZ2_DBG_TIM16_STOP)
#define LL_DBGMCU_APB1_GRP2_TIM16_STOP     DBGMCU_APB_FZ2_DBG_TIM16_STOP       /*!< TIM16 counter stopped when core is halted */
#endif
#if defined(DBGMCU_APB_FZ2_DBG_TIM17_STOP)
#define LL_DBGMCU_APB1_GRP2_TIM17_STOP     DBGMCU_APB_FZ2_DBG_TIM17_STOP       /*!< TIM17 counter stopped when core is halted */
#endif
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @defgroup SYSTEM_LL_Exported_Functions SYSTEM Exported Functions
  * @{
  */

/** @defgroup SYSTEM_LL_EF_SYSCFG SYSCFG
  * @{
  */
/**
  * @brief  Set FASTIO or AHB to access the GPIO register control
  * @rmtoll SYSCFG_CFGR1 GPIO_AHB_SEL      LL_SYSCFG_SetGPIOAccess
  * @param  GPIOSel This parameter can be one of the following values:
  *         @arg @ref LL_SYSCFG_GPIO_SEL_FASTIO
  *         @arg @ref LL_SYSCFG_GPIO_SEL_AHB
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_SetGPIOAccess(uint32_t GPIOSel)
{
  MODIFY_REG(SYSCFG->CFGR1, SYSCFG_CFGR1_GPIO_AHB_SEL, GPIOSel);
}

/**
  * @brief  Get FASTIO or AHB to access the GPIO register control
  * @rmtoll SYSCFG_CFGR1 GPIO_AHB_SEL      LL_SYSCFG_GetGPIOAccess
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_SYSCFG_GPIO_SEL_FASTIO
  *         @arg @ref LL_SYSCFG_GPIO_SEL_AHB
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetGPIOAccess(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->CFGR1, SYSCFG_CFGR1_GPIO_AHB_SEL));
}  

/**
  * @brief  Set memory mapping at address 0x00000000
  * @rmtoll SYSCFG_CFGR1 MEM_MODE      LL_SYSCFG_SetRemapMemory
  * @param  Memory This parameter can be one of the following values:
  *         @arg @ref LL_SYSCFG_REMAP_FLASH
  *         @arg @ref LL_SYSCFG_REMAP_SYSTEMFLASH
  *         @arg @ref LL_SYSCFG_REMAP_SRAM
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_SetRemapMemory(uint32_t Memory)
{
  MODIFY_REG(SYSCFG->CFGR1, SYSCFG_CFGR1_MEM_MODE, Memory);
}

/**
  * @brief  Get memory mapping at address 0x00000000
  * @rmtoll SYSCFG_CFGR1 MEM_MODE      LL_SYSCFG_GetRemapMemory
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_SYSCFG_REMAP_FLASH
  *         @arg @ref LL_SYSCFG_REMAP_SYSTEMFLASH
  *         @arg @ref LL_SYSCFG_REMAP_SRAM
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetRemapMemory(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->CFGR1, SYSCFG_CFGR1_MEM_MODE));
}

/**
  * @brief  Enables COMPx as TIMx break input
  * @note   Depending on devices and packages, some Peripherals may not be available.
  *         Refer to device datasheet for Peripherals availability.
  * @param  TIMBreakInputs This parameter can be a combination of the following values:
  *         @arg @ref LL_SYSCFG_TIMBREAK_LOCKUP_TO_ALL
  *         @arg @ref LL_SYSCFG_TIMBREAK_PVD_TO_ALL
  *         @arg @ref LL_SYSCFG_TIMBREAK_COMP1_TO_TIM1
  *         @arg @ref LL_SYSCFG_TIMBREAK_COMP2_TO_TIM1
  *         @arg @ref LL_SYSCFG_TIMBREAK_COMP1_TO_TIM16
  *         @arg @ref LL_SYSCFG_TIMBREAK_COMP2_TO_TIM16
  *         @arg @ref LL_SYSCFG_TIMBREAK_COMP1_TO_TIM17
  *         @arg @ref LL_SYSCFG_TIMBREAK_COMP2_TO_TIM17
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_EnableTIMBreakInputs(uint32_t TIMBreakInputs)
{
  SET_BIT(SYSCFG->CFGR2, TIMBreakInputs);
}

/**
  * @brief  Disables COMPx as TIMx break input
  * @note   Depending on devices and packages, some Peripherals may not be available.
  *         Refer to device datasheet for Peripherals availability.
  * @param  TIMBreakInputs This parameter can be a combination of the following values:
  *         @arg @ref LL_SYSCFG_TIMBREAK_LOCKUP_TO_ALL
  *         @arg @ref LL_SYSCFG_TIMBREAK_PVD_TO_ALL
  *         @arg @ref LL_SYSCFG_TIMBREAK_COMP1_TO_TIM1
  *         @arg @ref LL_SYSCFG_TIMBREAK_COMP2_TO_TIM1
  *         @arg @ref LL_SYSCFG_TIMBREAK_COMP1_TO_TIM16
  *         @arg @ref LL_SYSCFG_TIMBREAK_COMP2_TO_TIM16
  *         @arg @ref LL_SYSCFG_TIMBREAK_COMP1_TO_TIM17
  *         @arg @ref LL_SYSCFG_TIMBREAK_COMP2_TO_TIM17
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_DisableTIMBreakInputs(uint32_t TIMBreakInputs)
{
  CLEAR_BIT(SYSCFG->CFGR2, TIMBreakInputs);
}

/**
  * @brief  Indicate if COMPx as TIMx break input
  * @note   Depending on devices and packages, some Peripherals may not be available.
  *         Refer to device datasheet for Peripherals availability.
  * @param  TIMBreakInputs This parameter can be one of the following values:
  *         @arg @ref LL_SYSCFG_TIMBREAK_LOCKUP_TO_ALL
  *         @arg @ref LL_SYSCFG_TIMBREAK_PVD_TO_ALL
  *         @arg @ref LL_SYSCFG_TIMBREAK_COMP1_TO_TIM1
  *         @arg @ref LL_SYSCFG_TIMBREAK_COMP2_TO_TIM1
  *         @arg @ref LL_SYSCFG_TIMBREAK_COMP1_TO_TIM16
  *         @arg @ref LL_SYSCFG_TIMBREAK_COMP2_TO_TIM16
  *         @arg @ref LL_SYSCFG_TIMBREAK_COMP1_TO_TIM17
  *         @arg @ref LL_SYSCFG_TIMBREAK_COMP2_TO_TIM17
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsEnabledTIMBreakInputs(uint32_t TIMBreakInputs)
{
  return ((READ_BIT(SYSCFG->CFGR2, TIMBreakInputs) == (TIMBreakInputs)) ? 1UL : 0UL);
}

/**
  * @brief  Enables COMPx as TIMx Ocref input 
  * @note   Depending on devices and packages, some Peripherals may not be available.
  *         Refer to device datasheet for Peripherals availability.
  * @param  TIMOcrefInputs This parameter can be one of the following values:
  *         @arg @ref LL_SYSCFG_OCREF_CLR_TIM1_COMP1
  *         @arg @ref LL_SYSCFG_OCREF_CLR_TIM2_COMP1
  *         @arg @ref LL_SYSCFG_OCREF_CLR_TIM1_COMP2
  *         @arg @ref LL_SYSCFG_OCREF_CLR_TIM2_COMP2
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_EnableTIMOcrefInputs(uint32_t TIMOcrefInputs)
{
  SET_BIT(SYSCFG->CFGR2, TIMOcrefInputs);
}

/**
  * @brief  Disables COMPx as TIMx Ocref input 
  * @note   Depending on devices and packages, some Peripherals may not be available.
  *         Refer to device datasheet for Peripherals availability.
  * @param  TIMOcrefInputs This parameter can be one of the following values:
  *         @arg @ref LL_SYSCFG_OCREF_CLR_TIM1_COMP1
  *         @arg @ref LL_SYSCFG_OCREF_CLR_TIM2_COMP1
  *         @arg @ref LL_SYSCFG_OCREF_CLR_TIM1_COMP2
  *         @arg @ref LL_SYSCFG_OCREF_CLR_TIM2_COMP2
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_DisableTIMOcrefInputs(uint32_t TIMOcrefInputs)
{
  CLEAR_BIT(SYSCFG->CFGR2, TIMOcrefInputs);
}

/**
  * @brief  Indicate if COMPx as TIMx Ocref input  
  * @note   Depending on devices and packages, some Peripherals may not be available.
  *         Refer to device datasheet for Peripherals availability.
  * @param  TIMOcrefInputs This parameter can be one of the following values:
  *         @arg @ref LL_SYSCFG_OCREF_CLR_TIM1_COMP1
  *         @arg @ref LL_SYSCFG_OCREF_CLR_TIM2_COMP1
  *         @arg @ref LL_SYSCFG_OCREF_CLR_TIM1_COMP2
  *         @arg @ref LL_SYSCFG_OCREF_CLR_TIM2_COMP2
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsEnabledTIMOcrefInputs(uint32_t TIMOcrefInputs)
{
  return ((READ_BIT(SYSCFG->CFGR2, TIMOcrefInputs) == (TIMOcrefInputs)) ? 1UL : 0UL);
}

/**
  * @brief  Set the TIMER1 ETR input source
  * @note   Depending on devices and packages, some Peripherals may not be available.
  *         Refer to device datasheet for Peripherals availability.
  * @param  source This parameter can be one of the following values:
  *         @arg @ref LL_SYSCFG_ETR_SRC_TIM1_GPIO
  *         @arg @ref LL_SYSCFG_ETR_SRC_TIM1_COMP1
  *         @arg @ref LL_SYSCFG_ETR_SRC_TIM1_COMP2
  *         @arg @ref LL_SYSCFG_ETR_SRC_TIM1_ADC
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_SetTIM1ETRSource(uint32_t source)
{
  MODIFY_REG(SYSCFG->CFGR1, SYSCFG_CFGR1_ETR_SRC_TIM1, source);
}

/**
  * @brief  Get the TIMER1 ETR input source
  * @note   Depending on devices and packages, some Peripherals may not be available.
  *         Refer to device datasheet for Peripherals availability.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_SYSCFG_ETR_SRC_TIM1_GPIO
  *         @arg @ref LL_SYSCFG_ETR_SRC_TIM1_COMP1
  *         @arg @ref LL_SYSCFG_ETR_SRC_TIM1_COMP2
  *         @arg @ref LL_SYSCFG_ETR_SRC_TIM1_ADC
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetTIM1ETRSource(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->CFGR1, SYSCFG_CFGR1_ETR_SRC_TIM1));
}

/**
  * @brief  Set the TIMER2 ETR input source
  * @note   Depending on devices and packages, some Peripherals may not be available.
  *         Refer to device datasheet for Peripherals availability.
  * @param  source This parameter can be one of the following values:
  *         @arg @ref LL_SYSCFG_ETR_SRC_TIM2_GPIO
  *         @arg @ref LL_SYSCFG_ETR_SRC_TIM2_COMP1
  *         @arg @ref LL_SYSCFG_ETR_SRC_TIM2_COMP2
  *         @arg @ref LL_SYSCFG_ETR_SRC_TIM2_ADC
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_SetTIM2ETRSource(uint32_t source)
{
  MODIFY_REG(SYSCFG->CFGR1, SYSCFG_CFGR1_ETR_SRC_TIM2, source);
}

/**
  * @brief  Get the TIMER2 ETR input source
  * @note   Depending on devices and packages, some Peripherals may not be available.
  *         Refer to device datasheet for Peripherals availability.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_SYSCFG_ETR_SRC_TIM2_GPIO
  *         @arg @ref LL_SYSCFG_ETR_SRC_TIM2_COMP1
  *         @arg @ref LL_SYSCFG_ETR_SRC_TIM2_COMP2
  *         @arg @ref LL_SYSCFG_ETR_SRC_TIM2_ADC
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetTIM2ETRSource(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->CFGR1, SYSCFG_CFGR1_ETR_SRC_TIM2));
}

/**
  * @brief  Set the TIMER1 IC1 source
  * @note   Depending on devices and packages, some Peripherals may not be available.
  *         Refer to device datasheet for Peripherals availability.
  * @param  source This parameter can be one of the following values:
  *         @arg @ref LL_SYSCFG_TIM1_IC1_SRC_IO
  *         @arg @ref LL_SYSCFG_TIM1_IC1_SRC_COMP1
  *         @arg @ref LL_SYSCFG_TIM1_IC1_SRC_COMP2
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_SetTIM1IC1Source(uint32_t source)
{
  MODIFY_REG(SYSCFG->CFGR1, SYSCFG_CFGR1_TIM1_IC1_SRC, source);
}

/**
  * @brief  Get the TIMER1 IC1 source
  * @note   Depending on devices and packages, some Peripherals may not be available.
  *         Refer to device datasheet for Peripherals availability.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_SYSCFG_TIM1_IC1_SRC_IO
  *         @arg @ref LL_SYSCFG_TIM1_IC1_SRC_COMP1
  *         @arg @ref LL_SYSCFG_TIM1_IC1_SRC_COMP2
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetTIM1IC1Source(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->CFGR1, SYSCFG_CFGR1_TIM1_IC1_SRC));
}

/**
  * @brief  Set the TIMER2 IC4 source
  * @note   Depending on devices and packages, some Peripherals may not be available.
  *         Refer to device datasheet for Peripherals availability.
  * @param  source This parameter can be one of the following values:
  *         @arg @ref LL_SYSCFG_TIM2_IC4_SRC_IO
  *         @arg @ref LL_SYSCFG_TIM2_IC4_SRC_COMP1
  *         @arg @ref LL_SYSCFG_TIM2_IC4_SRC_COMP2
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_SetTIM2IC4Source(uint32_t source)
{
  MODIFY_REG(SYSCFG->CFGR1, SYSCFG_CFGR1_TIM2_IC4_SRC, source);
}

/**
  * @brief  Get the TIMER2 IC4 source
  * @note   Depending on devices and packages, some Peripherals may not be available.
  *         Refer to device datasheet for Peripherals availability.
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_SYSCFG_TIM2_IC4_SRC_IO
  *         @arg @ref LL_SYSCFG_TIM2_IC4_SRC_COMP1
  *         @arg @ref LL_SYSCFG_TIM2_IC4_SRC_COMP2
  * @retval None
  */
__STATIC_INLINE uint32_t LL_SYSCFG_GetTIM2IC4Source(void)
{
  return (uint32_t)(READ_BIT(SYSCFG->CFGR1, SYSCFG_CFGR1_TIM2_IC4_SRC));
}

/**
  * @}
  */

/** @defgroup SYSTEM_LL_EF_GPIO_ENS GPIO Filter Control 
  * @{
  */
/**
  * @brief  Enable GPIO Filter
  * @note   Depending on devices and packages, some IOs may not be available.
  *         Refer to device datasheet for IOs availability.
  * @param  GPIOPort This parameter can be a combination of the following values:
  *         @arg @ref LL_SYSCFG_GPIO_PORTA
  *         @arg @ref LL_SYSCFG_GPIO_PORTB
  *         @arg @ref LL_SYSCFG_GPIO_PORTF
  * @param  GPIOPin This parameter can be a combination of the following values:
  *         @arg @ref LL_SYSCFG_GPIO_PIN_0
  *         @arg @ref LL_SYSCFG_GPIO_PIN_1
  *         @arg @ref LL_SYSCFG_GPIO_PIN_2
  *         @arg @ref LL_SYSCFG_GPIO_PIN_3
  *         @arg @ref LL_SYSCFG_GPIO_PIN_4
  *         @arg @ref LL_SYSCFG_GPIO_PIN_5
  *         @arg @ref LL_SYSCFG_GPIO_PIN_6
  *         @arg @ref LL_SYSCFG_GPIO_PIN_7
  *         @arg @ref LL_SYSCFG_GPIO_PIN_8
  *         @arg @ref LL_SYSCFG_GPIO_PIN_9
  *         @arg @ref LL_SYSCFG_GPIO_PIN_10
  *         @arg @ref LL_SYSCFG_GPIO_PIN_11
  *         @arg @ref LL_SYSCFG_GPIO_PIN_12
  *         @arg @ref LL_SYSCFG_GPIO_PIN_13
  *         @arg @ref LL_SYSCFG_GPIO_PIN_14
  *         @arg @ref LL_SYSCFG_GPIO_PIN_15
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_EnableGPIOFilter(uint32_t GPIOPort, uint32_t GPIOPin)
{
  if(GPIOPort == LL_SYSCFG_GPIO_PORTA)
  {
    SET_BIT(SYSCFG->PA_ENS, GPIOPin);
  }
  else if(GPIOPort == LL_SYSCFG_GPIO_PORTB)
  {
    SET_BIT(SYSCFG->PB_ENS, GPIOPin);
  }
  else if(GPIOPort == LL_SYSCFG_GPIO_PORTF)
  {
    SET_BIT(SYSCFG->PF_ENS, GPIOPin);
  }
  else
  {
    
  }
}

/** 
  * @brief  Disable GPIO Filter
  * @note   Depending on devices and packages, some IOs may not be available.
  *         Refer to device datasheet for IOs availability.
  * @param  GPIOPort This parameter can be a combination of the following values:
  *         @arg @ref LL_SYSCFG_GPIO_PORTA
  *         @arg @ref LL_SYSCFG_GPIO_PORTB
  *         @arg @ref LL_SYSCFG_GPIO_PORTF
  * @param  GPIOPin This parameter can be a combination of the following values:
  *         @arg @ref LL_SYSCFG_GPIO_PIN_0
  *         @arg @ref LL_SYSCFG_GPIO_PIN_1
  *         @arg @ref LL_SYSCFG_GPIO_PIN_2
  *         @arg @ref LL_SYSCFG_GPIO_PIN_3
  *         @arg @ref LL_SYSCFG_GPIO_PIN_4
  *         @arg @ref LL_SYSCFG_GPIO_PIN_5
  *         @arg @ref LL_SYSCFG_GPIO_PIN_6
  *         @arg @ref LL_SYSCFG_GPIO_PIN_7
  *         @arg @ref LL_SYSCFG_GPIO_PIN_8
  *         @arg @ref LL_SYSCFG_GPIO_PIN_9
  *         @arg @ref LL_SYSCFG_GPIO_PIN_10
  *         @arg @ref LL_SYSCFG_GPIO_PIN_11
  *         @arg @ref LL_SYSCFG_GPIO_PIN_12
  *         @arg @ref LL_SYSCFG_GPIO_PIN_13
  *         @arg @ref LL_SYSCFG_GPIO_PIN_14
  *         @arg @ref LL_SYSCFG_GPIO_PIN_15
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_DisableGPIOFilter(uint32_t GPIOPort, uint32_t GPIOPin)
{
  if(GPIOPort == LL_SYSCFG_GPIO_PORTA)
  {
    CLEAR_BIT(SYSCFG->PA_ENS, GPIOPin);
  }
  else if(GPIOPort == LL_SYSCFG_GPIO_PORTB)
  {
    CLEAR_BIT(SYSCFG->PB_ENS, GPIOPin);
  }
  else if(GPIOPort == LL_SYSCFG_GPIO_PORTF)
  {
    CLEAR_BIT(SYSCFG->PF_ENS, GPIOPin);
  }
  else
  {
    
  }
}

/**
  * @brief  Indicate if enable the GPIO State in GPIO Filter
  * @param  GPIOPort This parameter can be a combination of the following values:
  *         @arg @ref LL_SYSCFG_GPIO_PORTA
  *         @arg @ref LL_SYSCFG_GPIO_PORTB
  *         @arg @ref LL_SYSCFG_GPIO_PORTF
  * @param  GPIOPin This parameter can be a combination of the following values:
  *         @arg @ref LL_SYSCFG_GPIO_PIN_0
  *         @arg @ref LL_SYSCFG_GPIO_PIN_1
  *         @arg @ref LL_SYSCFG_GPIO_PIN_2
  *         @arg @ref LL_SYSCFG_GPIO_PIN_3
  *         @arg @ref LL_SYSCFG_GPIO_PIN_4
  *         @arg @ref LL_SYSCFG_GPIO_PIN_5
  *         @arg @ref LL_SYSCFG_GPIO_PIN_6
  *         @arg @ref LL_SYSCFG_GPIO_PIN_7
  *         @arg @ref LL_SYSCFG_GPIO_PIN_8
  *         @arg @ref LL_SYSCFG_GPIO_PIN_9
  *         @arg @ref LL_SYSCFG_GPIO_PIN_10
  *         @arg @ref LL_SYSCFG_GPIO_PIN_11
  *         @arg @ref LL_SYSCFG_GPIO_PIN_12
  *         @arg @ref LL_SYSCFG_GPIO_PIN_13
  *         @arg @ref LL_SYSCFG_GPIO_PIN_14
  *         @arg @ref LL_SYSCFG_GPIO_PIN_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsEnabledGPIOFilter(uint32_t GPIOPort, uint32_t GPIOPin)
{
  if(GPIOPort == LL_SYSCFG_GPIO_PORTA)
  {
    return ((READ_BIT(SYSCFG->PA_ENS, GPIOPin) == (GPIOPin)) ? 1UL : 0UL);
  }
  else if(GPIOPort == LL_SYSCFG_GPIO_PORTB)
  {
    return ((READ_BIT(SYSCFG->PB_ENS, GPIOPin) == (GPIOPin)) ? 1UL : 0UL);
  }
  else
  {
    return ((READ_BIT(SYSCFG->PF_ENS, GPIOPin) == (GPIOPin)) ? 1UL : 0UL);
  }
}

/**
  * @}
  */

/** @defgroup SYSTEM_LL_EF_GPIO_ANA2EN   GPIO Analog2 
  * @{
  */
/**
  * @brief  Enable GPIO Analog2
  * @note   Depending on devices and packages, some IOs may not be available.
  *         Refer to device datasheet for IOs availability.
  * @param  GPIOPort This parameter can be a combination of the following values:
  *         @arg @ref LL_SYSCFG_GPIO_PORTA
  *         @arg @ref LL_SYSCFG_GPIO_PORTB
  *         @arg @ref LL_SYSCFG_GPIO_PORTF
  * @param  GPIOPin This parameter can be a combination of the following values:
  *         @arg @ref LL_SYSCFG_GPIO_PIN_0
  *         @arg @ref LL_SYSCFG_GPIO_PIN_1
  *         @arg @ref LL_SYSCFG_GPIO_PIN_2
  *         @arg @ref LL_SYSCFG_GPIO_PIN_3
  *         @arg @ref LL_SYSCFG_GPIO_PIN_4
  *         @arg @ref LL_SYSCFG_GPIO_PIN_5
  *         @arg @ref LL_SYSCFG_GPIO_PIN_6
  *         @arg @ref LL_SYSCFG_GPIO_PIN_7
  *         @arg @ref LL_SYSCFG_GPIO_PIN_8
  *         @arg @ref LL_SYSCFG_GPIO_PIN_9
  *         @arg @ref LL_SYSCFG_GPIO_PIN_10
  *         @arg @ref LL_SYSCFG_GPIO_PIN_11
  *         @arg @ref LL_SYSCFG_GPIO_PIN_12
  *         @arg @ref LL_SYSCFG_GPIO_PIN_13
  *         @arg @ref LL_SYSCFG_GPIO_PIN_14
  *         @arg @ref LL_SYSCFG_GPIO_PIN_15
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_EnableGPIOAnalog2(uint32_t GPIOPort, uint32_t GPIOPin) /*   */
{
  if(GPIOPort == LL_SYSCFG_GPIO_PORTA)
  {
    SET_BIT(SYSCFG->PA_ANA2EN, GPIOPin);
  }
  else if(GPIOPort == LL_SYSCFG_GPIO_PORTB)
  {
    SET_BIT(SYSCFG->PB_ANA2EN, GPIOPin);
  }
  else if(GPIOPort == LL_SYSCFG_GPIO_PORTF)
  {
    SET_BIT(SYSCFG->PF_ANA2EN, GPIOPin);
  }
  else
  {
    
  }
}

/**
  * @brief  Disable GPIO Filter
  * @note   Depending on devices and packages, some IOs may not be available.
  *         Refer to device datasheet for IOs availability.
  * @param  GPIOPort This parameter can be a combination of the following values:
  *         @arg @ref LL_SYSCFG_GPIO_PORTA
  *         @arg @ref LL_SYSCFG_GPIO_PORTB
  *         @arg @ref LL_SYSCFG_GPIO_PORTF
  * @param  GPIOPin This parameter can be a combination of the following values:
  *         @arg @ref LL_SYSCFG_GPIO_PIN_0
  *         @arg @ref LL_SYSCFG_GPIO_PIN_1
  *         @arg @ref LL_SYSCFG_GPIO_PIN_2
  *         @arg @ref LL_SYSCFG_GPIO_PIN_3
  *         @arg @ref LL_SYSCFG_GPIO_PIN_4
  *         @arg @ref LL_SYSCFG_GPIO_PIN_5
  *         @arg @ref LL_SYSCFG_GPIO_PIN_6
  *         @arg @ref LL_SYSCFG_GPIO_PIN_7
  *         @arg @ref LL_SYSCFG_GPIO_PIN_8
  *         @arg @ref LL_SYSCFG_GPIO_PIN_9
  *         @arg @ref LL_SYSCFG_GPIO_PIN_10
  *         @arg @ref LL_SYSCFG_GPIO_PIN_11
  *         @arg @ref LL_SYSCFG_GPIO_PIN_12
  *         @arg @ref LL_SYSCFG_GPIO_PIN_13
  *         @arg @ref LL_SYSCFG_GPIO_PIN_14
  *         @arg @ref LL_SYSCFG_GPIO_PIN_15
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_DisableGPIOAnalog2(uint32_t GPIOPort, uint32_t GPIOPin) /*   */
{
  if(GPIOPort == LL_SYSCFG_GPIO_PORTA)
  {
    CLEAR_BIT(SYSCFG->PA_ANA2EN, GPIOPin);
  }
  else if(GPIOPort == LL_SYSCFG_GPIO_PORTB)
  {
    CLEAR_BIT(SYSCFG->PB_ANA2EN, GPIOPin);
  }
  else if(GPIOPort == LL_SYSCFG_GPIO_PORTF)
  {
    CLEAR_BIT(SYSCFG->PF_ANA2EN, GPIOPin);
  }
  else
  {
    
  }
}

/**
  * @brief  Indicate if enable the GPIO State in GPIO Analog
  * @param  GPIOPort This parameter can be a combination of the following values:
  *         @arg @ref LL_SYSCFG_GPIO_PORTA
  *         @arg @ref LL_SYSCFG_GPIO_PORTB
  *         @arg @ref LL_SYSCFG_GPIO_PORTF
  * @param  GPIOPin This parameter can be a combination of the following values:
  *         @arg @ref LL_SYSCFG_GPIO_PIN_0
  *         @arg @ref LL_SYSCFG_GPIO_PIN_1
  *         @arg @ref LL_SYSCFG_GPIO_PIN_2
  *         @arg @ref LL_SYSCFG_GPIO_PIN_3
  *         @arg @ref LL_SYSCFG_GPIO_PIN_4
  *         @arg @ref LL_SYSCFG_GPIO_PIN_5
  *         @arg @ref LL_SYSCFG_GPIO_PIN_6
  *         @arg @ref LL_SYSCFG_GPIO_PIN_7
  *         @arg @ref LL_SYSCFG_GPIO_PIN_8
  *         @arg @ref LL_SYSCFG_GPIO_PIN_9
  *         @arg @ref LL_SYSCFG_GPIO_PIN_10
  *         @arg @ref LL_SYSCFG_GPIO_PIN_11
  *         @arg @ref LL_SYSCFG_GPIO_PIN_12
  *         @arg @ref LL_SYSCFG_GPIO_PIN_13
  *         @arg @ref LL_SYSCFG_GPIO_PIN_14
  *         @arg @ref LL_SYSCFG_GPIO_PIN_15
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsEnabledGPIOAnalog2(uint32_t GPIOPort, uint32_t GPIOPin)
{
  if(GPIOPort == LL_SYSCFG_GPIO_PORTA)
  {
    return ((READ_BIT(SYSCFG->PA_ANA2EN, GPIOPin) == (GPIOPin)) ? 1UL : 0UL);
  }
  else if(GPIOPort == LL_SYSCFG_GPIO_PORTB)
  {
    return ((READ_BIT(SYSCFG->PB_ANA2EN, GPIOPin) == (GPIOPin)) ? 1UL : 0UL);
  }
  else
  {
    return ((READ_BIT(SYSCFG->PF_ANA2EN, GPIOPin) == (GPIOPin)) ? 1UL : 0UL);
  }
}

/**
  * @}
  */

/** @defgroup SYSTEM_LL_EF_GPIO_EHS   GPIO EhsSignal 
  * @{
  */

/**
  * @brief  Enable high driving capability of LED pins.
  * @param  PORT_Pin specifies the pin
  *         This parameter can be any combination of the following values:
  *            @arg LL_SYSCFG_EHS_PA0
  *            @arg LL_SYSCFG_EHS_PA15
  *            @arg LL_SYSCFG_EHS_PB2
  *            @arg LL_SYSCFG_EHS_PB3
  *            @arg LL_SYSCFG_EHS_PB4
  *            @arg LL_SYSCFG_EHS_PB5
  *            @arg LL_SYSCFG_EHS_PB6
  *            @arg LL_SYSCFG_EHS_PB7
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_EnableLEDPinHighDrvSignal(uint32_t PORT_Pin)
{
  SET_BIT(SYSCFG->IOCFG, PORT_Pin);
}

/**
  * @brief  Disable high driving capability of LED pins.
  * @param  PORT_Pin specifies the pin
  *         This parameter can be any combination of the following values:
  *            @arg LL_SYSCFG_EHS_PA0
  *            @arg LL_SYSCFG_EHS_PA15
  *            @arg LL_SYSCFG_EHS_PB2
  *            @arg LL_SYSCFG_EHS_PB3
  *            @arg LL_SYSCFG_EHS_PB4
  *            @arg LL_SYSCFG_EHS_PB5
  *            @arg LL_SYSCFG_EHS_PB6
  *            @arg LL_SYSCFG_EHS_PB7
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_DisableLEDPinHighDrvSignal(uint32_t PORT_Pin)
{
  CLEAR_BIT(SYSCFG->IOCFG, PORT_Pin);
}

/**
  * @brief  Indicate if enable the GPIO State
  * @param  PORT_Pin specifies the pin
  *         This parameter can be any combination of the following values:
  *            @arg LL_SYSCFG_EHS_PA0
  *            @arg LL_SYSCFG_EHS_PA15
  *            @arg LL_SYSCFG_EHS_PB2
  *            @arg LL_SYSCFG_EHS_PB3
  *            @arg LL_SYSCFG_EHS_PB4
  *            @arg LL_SYSCFG_EHS_PB5
  *            @arg LL_SYSCFG_EHS_PB6
  *            @arg LL_SYSCFG_EHS_PB7
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsEnabledLEDPinHighDrvSignal(uint32_t PORT_Pin)
{
  return ((READ_BIT(SYSCFG->IOCFG, PORT_Pin) == (PORT_Pin)) ? 1UL : 0UL);
}

/**
  * @}
  */

/** @defgroup SYSTEM_LL_EF_GPIO_PUIIC   GPIO I2cPullUp 
  * @{
  */

/**
  * @brief  Enable I2C pin pull-up.
  * @param  PORT_Pin specifies the pin
  *         This parameter can be any combination of the following values:
  *            @arg LL_SYSCFG_PU_IIC_PF5
  *            @arg LL_SYSCFG_PU_IIC_PF6
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_EnableI2CPinPullUp(uint32_t PORT_Pin)
{
  SET_BIT(SYSCFG->IOCFG, PORT_Pin);
}

/**
  * @brief  Disable I2C pin pull-up.
  * @param  PORT_Pin specifies the pin
  *         This parameter can be any combination of the following values:
  *            @arg LL_SYSCFG_PU_IIC_PF5
  *            @arg LL_SYSCFG_PU_IIC_PF6
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_DisableI2CPinPullUp(uint32_t PORT_Pin)
{
  CLEAR_BIT(SYSCFG->IOCFG, PORT_Pin);
}

/**
  * @brief  Indicate if enable the GPIO State
  * @param  PORT_Pin specifies the pin
  *         This parameter can be any combination of the following values:
  *            @arg LL_SYSCFG_PU_IIC_PF5
  *            @arg LL_SYSCFG_PU_IIC_PF6
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsEnabledI2CPinPullUp(uint32_t PORT_Pin)
{
  return ((READ_BIT(SYSCFG->IOCFG, PORT_Pin) == (PORT_Pin)) ? 1UL : 0UL);
}

/**
  * @}
  */

/** @defgroup SYSTEM_LL_EF_GPIO_EIIC   GPIO EIICSignal 
  * @{
  */

/**
  * @brief  Enable I2C pin Signal.
  * @param  PORT_Pin specifies the pin
  *         This parameter can be any combination of the following values:
  *            @arg LL_SYSCFG_EIIC_PA2
  *            @arg LL_SYSCFG_EIIC_PA3
  *            @arg LL_SYSCFG_EIIC_PA7
  *            @arg LL_SYSCFG_EIIC_PA8
  *            @arg LL_SYSCFG_EIIC_PA9
  *            @arg LL_SYSCFG_EIIC_PA10
  *            @arg LL_SYSCFG_EIIC_PA11
  *            @arg LL_SYSCFG_EIIC_PA12
  *            @arg LL_SYSCFG_EIIC_PB6
  *            @arg LL_SYSCFG_EIIC_PB7
  *            @arg LL_SYSCFG_EIIC_PB8
  *            @arg LL_SYSCFG_EIIC_PB10
  *            @arg LL_SYSCFG_EIIC_PB11
  *            @arg LL_SYSCFG_EIIC_PF0
    *          @arg LL_SYSCFG_EIIC_PF1
  *            @arg LL_SYSCFG_EIIC_PF5
  *            @arg LL_SYSCFG_EIIC_PF6
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_EnableI2CPinSignal(uint32_t PORT_Pin)
{
  SET_BIT(SYSCFG->IOCFG, PORT_Pin);
}

/**
  * @brief  Disable I2C pin Signal.
  * @param  PORT_Pin specifies the pin
  *         This parameter can be any combination of the following values:
  *            @arg LL_SYSCFG_EIIC_PA2
  *            @arg LL_SYSCFG_EIIC_PA3
  *            @arg LL_SYSCFG_EIIC_PA7
  *            @arg LL_SYSCFG_EIIC_PA8
  *            @arg LL_SYSCFG_EIIC_PA9
  *            @arg LL_SYSCFG_EIIC_PA10
  *            @arg LL_SYSCFG_EIIC_PA11
  *            @arg LL_SYSCFG_EIIC_PA12
  *            @arg LL_SYSCFG_EIIC_PB6
  *            @arg LL_SYSCFG_EIIC_PB7
  *            @arg LL_SYSCFG_EIIC_PB8
  *            @arg LL_SYSCFG_EIIC_PB10
  *            @arg LL_SYSCFG_EIIC_PB11
  *            @arg LL_SYSCFG_EIIC_PF0
    *          @arg LL_SYSCFG_EIIC_PF1
  *            @arg LL_SYSCFG_EIIC_PF5
  *            @arg LL_SYSCFG_EIIC_PF6
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_DisableI2CPinSignal(uint32_t PORT_Pin)
{
  CLEAR_BIT(SYSCFG->IOCFG, PORT_Pin);
}

/**
  * @brief  Indicate if enable the GPIO State
  * @param  PORT_Pin specifies the pin
  *         This parameter can be any combination of the following values:
  *            @arg LL_SYSCFG_EIIC_PA2
  *            @arg LL_SYSCFG_EIIC_PA3
  *            @arg LL_SYSCFG_EIIC_PA7
  *            @arg LL_SYSCFG_EIIC_PA8
  *            @arg LL_SYSCFG_EIIC_PA9
  *            @arg LL_SYSCFG_EIIC_PA10
  *            @arg LL_SYSCFG_EIIC_PA11
  *            @arg LL_SYSCFG_EIIC_PA12
  *            @arg LL_SYSCFG_EIIC_PB6
  *            @arg LL_SYSCFG_EIIC_PB7
  *            @arg LL_SYSCFG_EIIC_PB8
  *            @arg LL_SYSCFG_EIIC_PB10
  *            @arg LL_SYSCFG_EIIC_PB11
  *            @arg LL_SYSCFG_EIIC_PF0
    *          @arg LL_SYSCFG_EIIC_PF1
  *            @arg LL_SYSCFG_EIIC_PF5
  *            @arg LL_SYSCFG_EIIC_PF6
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsEnabledI2CPinSignal(uint32_t PORT_Pin)
{
  return ((READ_BIT(SYSCFG->IOCFG, PORT_Pin) == (PORT_Pin)) ? 1UL : 0UL);
}

/**
  * @}
  */

/** @defgroup SYSTEM_LL_EF_FLASH FLASH
  * @{
  */

/**
  * @brief  Set FLASH Latency
  * @rmtoll FLASH_ACR    LATENCY       LL_FLASH_SetLatency
  * @param  Latency This parameter can be one of the following values:
  *         @arg @ref LL_FLASH_LATENCY_0
  *         @arg @ref LL_FLASH_LATENCY_1
  *         @arg @ref LL_FLASH_LATENCY_2
  * @retval None
  */
__STATIC_INLINE void LL_FLASH_SetLatency(uint32_t Latency)
{
  MODIFY_REG(FLASH->ACR, FLASH_ACR_LATENCY, Latency);
}

/**
  * @brief  Get FLASH Latency
  * @rmtoll FLASH_ACR    LATENCY       LL_FLASH_GetLatency
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_FLASH_LATENCY_0
  *         @arg @ref LL_FLASH_LATENCY_1
  *         @arg @ref LL_FLASH_LATENCY_2
  */
__STATIC_INLINE uint32_t LL_FLASH_GetLatency(void)
{
  return (uint32_t)(READ_BIT(FLASH->ACR, FLASH_ACR_LATENCY));
}

/**
  * @}
  */

/** @defgroup SYSTEM_LL_EF_OCREF TIM Ocref Control
  * @{
  */

/**
  * @brief  Enable COMP2 Ocref CLR TIM2
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_EnableCOMP2OcrefCLRTIM2()
{
  SET_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_COMP2_OCREF_CLR_TIM2);
}

/**
  * @brief  Disable COMP2 Ocref CLR TIM2
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_DisableCOMP2OcrefCLRTIM2()
{
  CLEAR_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_COMP2_OCREF_CLR_TIM2);
}

/**
  * @brief  Indicate if enable the COMP2 Ocref CLR TIM2
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsEnabledComp2OcrefTim2()
{
  return ((READ_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_COMP2_OCREF_CLR_TIM2) == (SYSCFG_CFGR2_COMP2_OCREF_CLR_TIM2)) ? 1UL : 0UL);
}

/**
  * @brief  Enable COMP2 Ocref CLR TIM1
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_EnableCOMP2OcrefCLRTIM1()
{
  SET_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_COMP2_OCREF_CLR_TIM1);
}

/**
  * @brief  Disable COMP2 Ocref CLR TIM1
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_DisableCOMP2OcrefCLRTIM1()
{
  CLEAR_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_COMP2_OCREF_CLR_TIM1);
}

/**
  * @brief  Indicate if enable the COMP2 Ocref CLR TIM1
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsEnabledComp2OcrefTim1()
{
  return ((READ_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_COMP2_OCREF_CLR_TIM1) == (SYSCFG_CFGR2_COMP2_OCREF_CLR_TIM1)) ? 1UL : 0UL);
}

/**
  * @brief  Enable COMP1 Ocref CLR TIM2
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_EnableCOMP1OcrefCLRTIM2()
{
  SET_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_COMP1_OCREF_CLR_TIM2);
}

/**
  * @brief  Disable COMP1 Ocref CLR TIM2
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_DisableCOMP1OcrefCLRTIM2()
{
  CLEAR_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_COMP1_OCREF_CLR_TIM2);
}

/**
  * @brief  Indicate if enable the COMP1 Ocref CLR TIM2
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsEnabledComp1OcrefTim2()
{
  return ((READ_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_COMP1_OCREF_CLR_TIM2) == (SYSCFG_CFGR2_COMP1_OCREF_CLR_TIM2)) ? 1UL : 0UL);
}

/**
  * @brief  Enable COMP1 Ocref CLR TIM1
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_EnableCOMP1OcrefCLRTIM1()
{
  SET_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_COMP1_OCREF_CLR_TIM1);
}

/**
  * @brief  Disable COMP1 Ocref CLR TIM1
  * @retval None
  */
__STATIC_INLINE void LL_SYSCFG_DisableCOMP1OcrefCLRTIM1()
{
  CLEAR_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_COMP1_OCREF_CLR_TIM1);
}

/**
  * @brief  Indicate if enable the COMP1 Ocref CLR TIM1
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_SYSCFG_IsEnabledComp1OcrefTim1()
{
  return ((READ_BIT(SYSCFG->CFGR2, SYSCFG_CFGR2_COMP1_OCREF_CLR_TIM1) == (SYSCFG_CFGR2_COMP1_OCREF_CLR_TIM1)) ? 1UL : 0UL);
}

/**
  * @}
  */

/** @defgroup SYSTEM_LL_EF_DBGMCU DBGMCU
  * @{
  */

/**
  * @brief  Return the device identifier
  * @retval Values between Min_Data=0x00 and Max_Data=0xFFFF
  */
__STATIC_INLINE uint32_t LL_DBGMCU_GetDeviceID(void)
{
  return (uint32_t)(READ_BIT(DBGMCU->IDCODE, DBGMCU_IDCODE_DEV_ID));
}

/**
  * @brief  Return the device revision identifier
  * @retval Values between Min_Data=0x00 and Max_Data=0xFFFF
  */
__STATIC_INLINE uint32_t LL_DBGMCU_GetRevisionID(void)
{
  return (uint32_t)(READ_BIT(DBGMCU->IDCODE, DBGMCU_IDCODE_REV_ID) >> DBGMCU_IDCODE_REV_ID_Pos);
}

/**
  * @brief  Enable the Debug Module during STOP mode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_EnableDBGStopMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOP);
}

/**
  * @brief  Disable the Debug Module during STOP mode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_DisableDBGStopMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOP);
}

/**
  * @brief  Indicate if enable the Debug Module during STOP mode
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DBGMCU_IsEnabledDBGStopMode(void)
{
  return ((READ_BIT(DBGMCU->CR, DBGMCU_CR_DBG_STOP) == (DBGMCU_CR_DBG_STOP)) ? 1UL : 0UL);
}

/**
  * @brief  Enable the Debug Module during SLEEP mode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_EnableDBGSleepMode(void)
{
  SET_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEP);
}

/**
  * @brief  Disable the Debug Module during SLEEP mode
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_DisableDBGSleepMode(void)
{
  CLEAR_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEP);
}

/**
  * @brief  Indicate if enable the Debug Module during SLEEP mode
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DBGMCU_IsEnabledDBGSleepMode(void)
{
  return ((READ_BIT(DBGMCU->CR, DBGMCU_CR_DBG_SLEEP) == (DBGMCU_CR_DBG_SLEEP)) ? 1UL : 0UL);
}


/**
  * @brief  Freeze APB1 peripherals (group1 peripherals)
  * @note   Depending on devices and packages, some Peripherals may not be available.
  *         Refer to device datasheet for Peripherals availability.
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM2_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_RTC_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_WWDG_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_IWDG_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_LPTIM1_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB1_GRP1_FreezePeriph(uint32_t Periphs)
{
  SET_BIT(DBGMCU->APBFZ1, Periphs);
}

/**
  * @brief  Unfreeze APB1 peripherals (group1 peripherals)
  * @note   Depending on devices and packages, some Peripherals may not be available.
  *         Refer to device datasheet for Peripherals availability.
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM2_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_RTC_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_WWDG_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_IWDG_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_LPTIM1_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB1_GRP1_UnFreezePeriph(uint32_t Periphs)
{
  CLEAR_BIT(DBGMCU->APBFZ1, Periphs);
}

/**
  * @brief  Indicate if Freeze APB1 peripherals (group1 peripherals)
  * @note   Depending on devices and packages, some Peripherals may not be available.
  *         Refer to device datasheet for Peripherals availability.
  * @param  Periphs This parameter can be one of the following values:
  *         @arg @ref LL_DBGMCU_APB1_GRP1_TIM2_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_RTC_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_WWDG_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_IWDG_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP1_LPTIM1_STOP
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DBGMCU_APB1_GRP1_IsFreezePeriph(uint32_t Periphs)
{
  return ((READ_BIT(DBGMCU->APBFZ1, Periphs) == (Periphs)) ? 1UL : 0UL);
}

/**
  * @brief  Freeze APB1 peripherals(group2 peripherals)
  * @note   Depending on devices and packages, some Peripherals may not be available.
  *         Refer to device datasheet for Peripherals availability.
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB1_GRP2_TIM1_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP2_TIM14_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP2_TIM16_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP2_TIM17_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB1_GRP2_FreezePeriph(uint32_t Periphs)
{
  SET_BIT(DBGMCU->APBFZ2, Periphs);
}

/**
  * @brief  Unfreeze APB1 peripherals(group2 peripherals)
  * @note   Depending on devices and packages, some Peripherals may not be available.
  *         Refer to device datasheet for Peripherals availability.
  * @param  Periphs This parameter can be a combination of the following values:
  *         @arg @ref LL_DBGMCU_APB1_GRP2_TIM1_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP2_TIM14_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP2_TIM16_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP2_TIM17_STOP
  * @retval None
  */
__STATIC_INLINE void LL_DBGMCU_APB1_GRP2_UnFreezePeriph(uint32_t Periphs)
{
  CLEAR_BIT(DBGMCU->APBFZ2, Periphs);
}

/**
  * @brief  Indicate if Freeze APB1 peripherals (group2 peripherals)
  * @note   Depending on devices and packages, some Peripherals may not be available.
  *         Refer to device datasheet for Peripherals availability.
  * @param  Periphs This parameter can be one of the following values:
  *         @arg @ref LL_DBGMCU_APB1_GRP2_TIM1_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP2_TIM14_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP2_TIM16_STOP
  *         @arg @ref LL_DBGMCU_APB1_GRP2_TIM17_STOP
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_DBGMCU_APB1_GRP2_IsFreezePeriph(uint32_t Periphs)
{
  return ((READ_BIT(DBGMCU->APBFZ2, Periphs) == (Periphs)) ? 1UL : 0UL);
}
/**
  * @}
  */
/**
  * @}
  */

/**
  * @}
  */



#endif /* defined (FLASH) || defined (SYSCFG) || defined (DBGMCU) */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* PY32F031_LL_SYSTEM_H */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/

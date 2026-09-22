/**
  ******************************************************************************
  * @file    py32f031_ll_opa.h
  * @author  MCU Application Team
  * @brief   Header file of OPA LL module.
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
#ifndef __PY32F031_LL_OPA_H
#define __PY32F031_LL_OPA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "py32f0xx.h"

/** @addtogroup PY32F031_LL_Driver
  * @{
  */

#if defined (OPA) 

/** @defgroup OPA_LL OPA
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private constants ---------------------------------------------------------*/
/** @defgroup OPA_LL_Private_Constants OPA Private Constants
  * @{
  */


/**
  * @}
  */


/* Private macros ------------------------------------------------------------*/
/** @defgroup OPA_LL_Private_Macros OPA Private Macros
  * @{
  */
  
/**
  * @}
  */


/* Exported types ------------------------------------------------------------*/
#if defined(USE_FULL_LL_DRIVER)


#endif /* USE_FULL_LL_DRIVER */

/* Exported constants --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @defgroup OPA_LL_Exported_Functions OPA Exported Functions
  * @{
  */

/** @defgroup OPA_LL_EF_CONFIGURATION_OPA_INSTANCE Configuration of OPA hierarchical scope: OPA instance
  * @{
  */
#if defined(OPA_CR_OPA1EN)
/**
  * @brief  Enable the OPA1.
  * @rmtoll CR      OPA1EN            LL_OPA1_Enable
  * @param  OPAx OPA instance
  * @retval None
  */
__STATIC_INLINE void LL_OPA1_Enable(OPA_TypeDef *OPAx)
{
  SET_BIT(OPAx->CR, OPA_CR_OPA1EN);
}

/**
  * @brief  Disable the OPA1.
  * @rmtoll CR      OPA1EN            LL_OPA1_Disable
  * @param  OPAx OPA instance
  * @retval None
  */
__STATIC_INLINE void LL_OPA1_Disable(OPA_TypeDef *OPAx)
{
  CLEAR_BIT(OPAx->CR, OPA_CR_OPA1EN);
}

/**
  * @brief  Get the OPA1 Enable state.
  * @rmtoll CR      OPA1EN            LL_OPA1_IsEnabled
  * @param  OPAx OPA instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_OPA1_IsEnabled(OPA_TypeDef *OPAx)
{
  return (READ_BIT(OPAx->CR, OPA_CR_OPA1EN) == OPA_CR_OPA1EN);
}

/**
  * @brief  Enable the output of OPA1.
  * @rmtoll OENR      OPA1OEN          LL_OPA1_EnableOutput
  * @param  OPAx OPA instance
  * @retval None
  */
__STATIC_INLINE void LL_OPA1_EnableOutput(OPA_TypeDef *OPAx)
{
  SET_BIT(OPAx->OENR, OPA_OENR_OPA1OEN);
}

/**
  * @brief  Disable the output of OPA1.
  * @rmtoll OENR      OPA1OEN          LL_OPA1_DisableOutput
  * @param  OPAx OPA instance
  * @retval None
  */
__STATIC_INLINE void LL_OPA1_DisableOutput(OPA_TypeDef *OPAx)
{
  CLEAR_BIT(OPAx->OENR, OPA_OENR_OPA1OEN);
}

/**
  * @brief  Get the OPA1 Output state.
  * @rmtoll OENR      OPA1OEN          LL_OPA1_IsEnabledOutput
  * @param  OPAx OPA instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_OPA1_IsEnabledOutput(OPA_TypeDef *OPAx)
{
  return (READ_BIT(OPAx->OENR, OPA_OENR_OPA1OEN) == OPA_OENR_OPA1OEN);
}
#endif

#if defined(OPA_CR_OPA2EN)
/**
  * @brief  Enable the OPA2.
  * @rmtoll CR      OPA2EN              LL_OPA2_Enable
  * @param  OPAx OPA instance
  * @retval None
  */
__STATIC_INLINE void LL_OPA2_Enable(OPA_TypeDef *OPAx)
{
  SET_BIT(OPAx->CR, OPA_CR_OPA2EN);
}

/**
  * @brief  Disable the OPA2.
  * @rmtoll CR      OPA2EN              LL_OPA2_Disable
  * @param  OPAx OPA instance
  * @retval None
  */
__STATIC_INLINE void LL_OPA2_Disable(OPA_TypeDef *OPAx)
{
  CLEAR_BIT(OPAx->CR, OPA_CR_OPA2EN);
}

/**
  * @brief  Get the OPA2 Enable state.
  * @rmtoll CR      OPA2EN              LL_OPA2_IsEnabled
  * @param  OPAx OPA instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_OPA2_IsEnabled(OPA_TypeDef *OPAx)
{
  return (READ_BIT(OPAx->CR, OPA_CR_OPA2EN) == OPA_CR_OPA2EN);
}

/**
  * @brief  Enable the OPA.
  * @rmtoll OENR      OPA2OEN          LL_OPA2_EnableOutput
  * @param  OPAx OPA instance
  * @retval None
  */
__STATIC_INLINE void LL_OPA2_EnableOutput(OPA_TypeDef *OPAx)
{
  SET_BIT(OPAx->OENR, OPA_OENR_OPA2OEN);
}

/**
  * @brief  Disable the the output of OPA2.
  * @rmtoll OENR      OPA2OEN          LL_OPA2_DisableOutput
  * @param  OPAx OPA instance
  * @retval None
  */
__STATIC_INLINE void LL_OPA2_DisableOutput(OPA_TypeDef *OPAx)
{
  CLEAR_BIT(OPAx->OENR, OPA_OENR_OPA2OEN);
}

/**
  * @brief Get the OPA2 Output state.
  * @rmtoll OENR      OPA2OEN          LL_OPA2_IsEnabledOutput
  * @param  OPAx OPA instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_OPA2_IsEnabledOutput(OPA_TypeDef *OPAx)
{
  return (READ_BIT(OPAx->OENR, OPA_OENR_OPA2OEN) == OPA_OENR_OPA2OEN);
}
#endif

/**
  * @}
  */

#if defined(USE_FULL_LL_DRIVER)
/** @defgroup OPA_LL_EF_Init Initialization and de-initialization functions
  * @{
  */

/**
  * @}
  */
#endif /* USE_FULL_LL_DRIVER */

/**
  * @}
  */

/**
  * @}
  */

#endif /* OPA */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __PY32F031_LL_OPA_H */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/

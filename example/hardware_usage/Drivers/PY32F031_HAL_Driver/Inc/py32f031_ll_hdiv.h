/**
  ******************************************************************************
  * @file    py32f031_ll_hdiv.h
  * @author  MCU Application Team
  * @brief   Header file of HDIV LL module.
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
#ifndef PY32F031_LL_HDIV_H
#define PY32F031_LL_HDIV_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "py32f0xx.h"

/** @addtogroup PY32F031_LL_Driver
  * @{
  */

#if defined (HDIV)

/** @defgroup HDIV_LL HDIV
  * @{
  */

/* Private types -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup HDIV_LL_Exported_Constants HDIV Exported Constants
  * @{
  */

/** @defgroup HDIV_LL_EC_MODE Sign mode Selection
  * @{
  */
#define LL_HDIV_MODE_UNSIGNED             0x00000000U
#define LL_HDIV_MODE_SIGNED               HDIV_SIGN_SIGN
/**
  * @}
  */


/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup HDIV_LL_Exported_Macros HDIV Exported Macros
  * @{
  */
/** @defgroup HDIV_LL_EM_WRITE_READ Common Write and read registers macros
  * @{
  */
/**
  * @brief  Write a value in HDIV register
  * @param  __INSTANCE__ HDIV Instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_HDIV_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/**
  * @brief  Read a value in HDIV register
  * @param  __INSTANCE__ HDIV Instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_HDIV_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup HDIV_LL_Exported_Functions HDIV Exported Functions
  * @{
  */

/**
  * @brief  Set the HDIV sign.
  * @param  HDIVx HDIV Instance
  * @param  Sign This parameter can be one of the following values:
  *         @arg @ref LL_HDIV_MODE_UNSIGNED
  *         @arg @ref LL_HDIV_MODE_SIGNED
  * @retval None
  */
__STATIC_INLINE void LL_HDIV_SetSign(HDIV_TypeDef *HDIVx, uint32_t Sign)
{
  MODIFY_REG(HDIVx->SIGN, HDIV_SIGN_SIGN, Sign);
}

/**
  * @brief  Get the HDIV sign.
  * @param  HDIVx HDIV Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_HDIV_MODE_UNSIGNED
  *         @arg @ref LL_HDIV_MODE_SIGNED
  */
__STATIC_INLINE uint32_t LL_HDIV_GetSign(HDIV_TypeDef *HDIVx)
{
  return (uint32_t)(READ_BIT(HDIVx->SIGN, HDIV_SIGN_SIGN));
}

/**
  * @brief  Set the HDIV Dividend value.
  * @param  HDIVx HDIV Instance
  * @param  Dividend This parameter must be a number between Min_Data = 0x00000000 and Max_Data = 0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE void LL_HDIV_SetDividend(HDIV_TypeDef *HDIVx, int32_t Dividend)
{
  WRITE_REG(HDIVx->DEND, Dividend);
}

/**
  * @brief  Get the HDIV Dividend value.
  * @param  HDIVx HDIV Instance
  * @retval Returned value is a number between Min_Data = 0x00000000 and Max_Data = 0xFFFFFFFF
  */
__STATIC_INLINE int32_t LL_HDIV_GetDividend(HDIV_TypeDef *HDIVx)
{
  return (int32_t)(READ_REG(HDIVx->DEND));
}


/**
  * @brief  Set the HDIV Divisor value.
  * @param  HDIVx HDIV Instance
  * @param  Divisor This parameter must be a number between Min_Data = 0x00000000 and Max_Data = 0xFFFFFFFF
  * @retval None
  */
__STATIC_INLINE void LL_HDIV_SetDivisor(HDIV_TypeDef *HDIVx, int32_t Divisor)
{
  WRITE_REG(HDIVx->SOR, Divisor);
}

/**
  * @brief  Get the HDIV Divisor value.
  * @param  HDIVx HDIV Instance
  * @retval Returned value is a number between Min_Data = 0x00000000 and Max_Data = 0xFFFFFFFF
  */
__STATIC_INLINE int32_t LL_HDIV_GetDivisor(HDIV_TypeDef *HDIVx)
{
  return (int32_t)(READ_REG(HDIVx->SOR));
}

/**
  * @brief  Get the HDIV Quotient value.
  * @param  HDIVx HDIV Instance
  * @retval Returned value is a number between Min_Data = 0x00000000 and Max_Data = 0xFFFFFFFF
  */
__STATIC_INLINE int32_t LL_HDIV_GetQuotient(HDIV_TypeDef *HDIVx)
{
  return (int32_t)(READ_REG(HDIVx->QUOT));
}

/**
  * @brief  Get the HDIV Remainder value.
  * @param  HDIVx HDIV Instance
  * @retval Returned value is a number between Min_Data = 0x00000000 and Max_Data = 0xFFFFFFFF
  */
__STATIC_INLINE int32_t LL_HDIV_GetRemainder(HDIV_TypeDef *HDIVx)
{
  return (int32_t)(READ_REG(HDIVx->REMD));
}

/**
  * @brief  Check if Divide by zero warning flag bit is set.
  * @param  HDIVx HDIV Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_HDIV_IsActiveFlag_HDIV_ZERO(HDIV_TypeDef *HDIVx)
{
  return ((READ_BIT(HDIVx->STAT, HDIV_STAT_ZERO) == (HDIV_STAT_ZERO)) ? 1UL : 0UL);
}

/**
  * @brief  Check if Division operation end flag bit is set.
  * @param  HDIVx HDIV Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_HDIV_IsActiveFlag_HDIV_END(HDIV_TypeDef *HDIVx)
{
  return ((READ_BIT(HDIVx->STAT, HDIV_STAT_END) == (HDIV_STAT_END)) ? 1UL : 0UL);
}


/**
  * @}
  */


/**
  * @}
  */

#endif /* HDIV */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* PY32F031_LL_HDIV_H */

/************************ (C) COPYRIGHT Puya *****END OF FILE****/

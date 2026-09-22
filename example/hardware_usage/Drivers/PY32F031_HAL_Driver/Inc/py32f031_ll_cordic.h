/**
  ******************************************************************************
  * @file    py32f031_ll_cordic.h
  * @author  MCU Application Team
  * @brief   Header file of CORDIC LL module.
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
#ifndef PY32F031_LL_CORDIC_H
#define PY32F031_LL_CORDIC_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "py32f0xx.h"

/** @addtogroup PY32F031_LL_Driver
  * @{
  */

#if defined(CORDIC)

/** @defgroup CORDIC_LL CORDIC
  * @{
  */

/* Private variables ---------------------------------------------------------*/
/* Private constants ---------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup CORDIC_LL_Exported_Constants CORDIC Exported Constants
  * @{
  */

/** @defgroup CORDIC_LL_EC_FUNCTION Function
  * @{
  */
#define LL_CORDIC_FUNCTION_ARCTAN_MOD   (0x00000000U)               /*!< Arctan and Mod operation */
#define LL_CORDIC_FUNCTION_SIN_COS      (CORDIC_CR_CORDIC_MODE)     /*!< Cosine and Sine operation*/
/**
  * @}
  */

/** @defgroup CORDIC_LL_EC_START_MODE Start Mode
  * @{
  */
#define LL_CORDIC_STARTMODE_AUTO      (0x00000000U)             /*!< Start calculation after performing write operations on registers CORDIC_THETA, CORDIC_X,CORDIC_Y */
#define LL_CORDIC_STARTMODE_NOT_AUTO  CORDIC_CR_START_MODE      /*!< Calculate when the position of the START register is 1 */
/**
  * @}
  */

/** @defgroup CORDIC_LL_EC_IN_SIZE In Size
  * @{
  */
#define LL_CORDIC_INSIZE_32BITS       (0x00000000U)             /*!< 32 bits input data size (Q1.31 format) */
#define LL_CORDIC_INSIZE_16BITS       CORDIC_CR_ARGSIZE         /*!< 16 bits input data size (Q1.15 format) */
/**
  * @}
  */

/** @defgroup CORDIC_LL_EC_OUT_SIZE Out Size
  * @{
  */
#define LL_CORDIC_OUTSIZE_32BITS      (0x00000000U)             /*!< 32 bits output data size (Q1.31 format) */
#define LL_CORDIC_OUTSIZE_16BITS      CORDIC_CR_RESSIZE         /*!< 16 bits output data size (Q1.15 format) */
/**
  * @}
  */

/** @defgroup CORDIC_LL_EC_ARCTAN_MOD_IN_OUT_SIZE Arctan Mod In Out Size
  * @{
  */
#define LL_CORDIC_ARCTAN_MOD_IN_OUT_SIZE_NOT_24BITS      (0x00000000U)             /*!< Determined by the RESSIZE and ARGSIZE registers */
#define LL_CORDIC_ARCTAN_MOD_IN_OUT_SIZE_24BITS          CORDIC_CR_VECSIZE         /*!< Arctan/mod input and output data with a bit width of 24 bits */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/
/** @defgroup CORDIC_LL_Exported_Macros CORDIC Exported Macros
  * @{
  */

/** @defgroup CORDIC_LL_EM_WRITE_READ Common Write and read registers Macros
  * @{
  */

/**
  * @brief  Write a value in CORDIC register.
  * @param  __INSTANCE__ CORDIC Instance
  * @param  __REG__ Register to be written
  * @param  __VALUE__ Value to be written in the register
  * @retval None
  */
#define LL_CORDIC_WriteReg(__INSTANCE__, __REG__, __VALUE__) WRITE_REG(__INSTANCE__->__REG__, (__VALUE__))

/**
  * @brief  Read a value in CORDIC register.
  * @param  __INSTANCE__ CORDIC Instance
  * @param  __REG__ Register to be read
  * @retval Register value
  */
#define LL_CORDIC_ReadReg(__INSTANCE__, __REG__) READ_REG(__INSTANCE__->__REG__)
/**
  * @}
  */

/**
  * @}
  */


/* Exported functions --------------------------------------------------------*/

/** @defgroup CORDIC_LL_Exported_Functions CORDIC Exported Functions
  * @{
  */

/** @defgroup CORDIC_LL_EF_Configuration CORDIC Configuration functions
  * @{
  */

/**
  * @brief  Set iterations.
  * @param  CORDICx CORDIC Instance
  * @param  Iterations This parameter must be a number between Min_Data = 1 and Max_Data = 24
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_SetIterations(CORDIC_TypeDef *CORDICx, uint32_t Iterations)
{
  MODIFY_REG(CORDICx->CR, CORDIC_CR_ITERATION, Iterations);
}

/**
  * @brief  Get iterations.
  * @param  CORDICx CORDIC Instance
  * @retval Returned value is a number between Min_Data = 1 and Max_Data = 24
  */
__STATIC_INLINE uint32_t LL_CORDIC_GetIterations(CORDIC_TypeDef *CORDICx)
{
  return (uint32_t)(READ_BIT(CORDICx->CR, CORDIC_CR_ITERATION));
}

/**
  * @brief  Set function.
  * @param  CORDICx CORDIC Instance
  * @param  Function This parameter can be one of the following values:
  *         @arg @ref LL_CORDIC_FUNCTION_ARCTAN_MOD
  *         @arg @ref LL_CORDIC_FUNCTION_SIN_COS
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_SetFunction(CORDIC_TypeDef *CORDICx, uint32_t Function)
{
  MODIFY_REG(CORDICx->CR, CORDIC_CR_CORDIC_MODE, Function);
}

/**
  * @brief  Get function.
  * @param  CORDICx CORDIC Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_CORDIC_FUNCTION_ARCTAN_MOD
  *         @arg @ref LL_CORDIC_FUNCTION_SIN_COS
  */
__STATIC_INLINE uint32_t LL_CORDIC_GetFunction(CORDIC_TypeDef *CORDICx)
{
  return (uint32_t)(READ_BIT(CORDICx->CR, CORDIC_CR_CORDIC_MODE));
}

/**
  * @brief  Set start mode.
  * @param  CORDICx CORDIC Instance
  * @param  StartMode This parameter can be one of the following values:
  *         @arg @ref LL_CORDIC_STARTMODE_AUTO
  *         @arg @ref LL_CORDIC_STARTMODE_NOT_AUTO
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_SetStartMode(CORDIC_TypeDef *CORDICx, uint32_t StartMode)
{
  MODIFY_REG(CORDICx->CR, CORDIC_CR_START_MODE, StartMode);
}

/**
  * @brief  Get start mode.
  * @param  CORDICx CORDIC Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_CORDIC_STARTMODE_AUTO
  *         @arg @ref LL_CORDIC_STARTMODE_NOT_AUTO
  */
__STATIC_INLINE uint32_t LL_CORDIC_GetStartMode(CORDIC_TypeDef *CORDICx)
{
  return (uint32_t)(READ_BIT(CORDICx->CR, CORDIC_CR_START_MODE));
}

/**
  * @brief  Set start.
  * @param  CORDICx CORDIC Instance
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_SetStart(CORDIC_TypeDef *CORDICx)
{
  SET_BIT(CORDICx->CR, CORDIC_CR_START);
}

/**
  * @brief  Set input data width.
  * @param  CORDICx CORDIC Instance
  * @param  InSize This parameter can be one of the following values:
  *         @arg @ref LL_CORDIC_INSIZE_32BITS
  *         @arg @ref LL_CORDIC_INSIZE_16BITS
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_SetInSize(CORDIC_TypeDef *CORDICx, uint32_t InSize)
{
  MODIFY_REG(CORDICx->CR, CORDIC_CR_ARGSIZE, InSize);
}

/**
  * @brief  Get input data width.
  * @param  CORDICx CORDIC Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_CORDIC_INSIZE_32BITS
  *         @arg @ref LL_CORDIC_INSIZE_16BITS
  */
__STATIC_INLINE uint32_t LL_CORDIC_GetInSize(CORDIC_TypeDef *CORDICx)
{
  return (uint32_t)(READ_BIT(CORDICx->CR, CORDIC_CR_ARGSIZE));
}

/**
  * @brief  Set output data width.
  * @param  CORDICx CORDIC Instance
  * @param  OutSize This parameter can be one of the following values:
  *         @arg @ref LL_CORDIC_OUTSIZE_32BITS
  *         @arg @ref LL_CORDIC_OUTSIZE_16BITS
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_SetOutSize(CORDIC_TypeDef *CORDICx, uint32_t OutSize)
{
  MODIFY_REG(CORDICx->CR, CORDIC_CR_RESSIZE, OutSize);
}

/**
  * @brief  Get output data width.
  * @param  CORDICx CORDIC Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_CORDIC_OUTSIZE_32BITS
  *         @arg @ref LL_CORDIC_OUTSIZE_16BITS
  */
__STATIC_INLINE uint32_t LL_CORDIC_GetOutSize(CORDIC_TypeDef *CORDICx)
{
  return (uint32_t)(READ_BIT(CORDICx->CR, CORDIC_CR_RESSIZE));
}

/**
  * @brief  Set Arctan and Mod InOutSize data width.
  * @param  CORDICx CORDIC Instance
  * @param  ArctanModInOutSize This parameter can be one of the following values:
  *         @arg @ref LL_CORDIC_ARCTAN_MOD_IN_OUT_SIZE_NOT_24BITS
  *         @arg @ref LL_CORDIC_ARCTAN_MOD_IN_OUT_SIZE_24BITS
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_SetArctanModInOutSize(CORDIC_TypeDef *CORDICx, uint32_t ArctanModInOutSize)
{
  MODIFY_REG(CORDICx->CR, CORDIC_CR_VECSIZE, ArctanModInOutSize);
}

/**
  * @brief  Get Arctan and Mod InOutSize data width.
  * @param  CORDICx CORDIC Instance
  * @retval Returned value can be one of the following values:
  *         @arg @ref LL_CORDIC_ARCTAN_MOD_IN_OUT_SIZE_NOT_24BITS
  *         @arg @ref LL_CORDIC_ARCTAN_MOD_IN_OUT_SIZE_24BITS
  */
__STATIC_INLINE uint32_t LL_CORDIC_GetArctanModInOutSize(CORDIC_TypeDef *CORDICx)
{
  return (uint32_t)(READ_BIT(CORDICx->CR, CORDIC_CR_VECSIZE));
}

/**
  * @}
  */

/** @defgroup CORDIC_LL_EF_IT_Management IT_Management
  * @{
  */
/**
  * @brief  Enable CORDIC interrupt.
  * @param  CORDICx CORDIC Instance
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_EnableIT_CORDIC(CORDIC_TypeDef *CORDICx)
{
  SET_BIT(CORDICx->CR, CORDIC_CR_CORDIC_INT);
}

/**
  * @brief  Enable SQRT interrupt.
  * @param  CORDICx CORDIC Instance
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_EnableIT_SQRT(CORDIC_TypeDef *CORDICx)
{
  SET_BIT(CORDICx->CR, CORDIC_CR_SQRT_INT);
}

/**
  * @brief  Disable CORDIC interrupt.
  * @param  CORDICx CORDIC Instance
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_DisableIT_CORDIC(CORDIC_TypeDef *CORDICx)
{
  CLEAR_BIT(CORDICx->CR, CORDIC_CR_CORDIC_INT);
}

/**
  * @brief  Disable SQRT interrupt.
  * @param  CORDICx CORDIC Instance
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_DisableIT_SQRT(CORDIC_TypeDef *CORDICx)
{
  CLEAR_BIT(CORDICx->CR, CORDIC_CR_SQRT_INT);
}

/**
  * @brief  Check if CORDIC Interrupt is enabled or disabled.
  * @param  CORDICx CORDIC Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_CORDIC_IsEnabledIT_CORDIC(CORDIC_TypeDef *CORDICx)
{
  return ((READ_BIT(CORDICx->CR, CORDIC_CR_CORDIC_INT) == (CORDIC_CR_CORDIC_INT)) ? 1U : 0U);
}

/**
  * @brief  Check if SQRT Interrupt is enabled or disabled.
  * @param  CORDICx CORDIC Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_CORDIC_IsEnabledIT_SQRT(CORDIC_TypeDef *CORDICx)
{
  return ((READ_BIT(CORDICx->CR, CORDIC_CR_SQRT_INT) == (CORDIC_CR_SQRT_INT)) ? 1U : 0U);
}

/**
  * @brief  Enable CORDIC_INT_MASK.
  * @param  CORDICx CORDIC Instance
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_EnableIntMask_CORDIC(CORDIC_TypeDef *CORDICx)
{
  CLEAR_BIT(CORDICx->CR, CORDIC_CR_CORDIC_INT_MASK);
}

/**
  * @brief  Enable CORDIC_ERROR_INT_MASK.
  * @param  CORDICx CORDIC Instance
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_EnableIntMask_CORDIC_ERROR(CORDIC_TypeDef *CORDICx)
{
  CLEAR_BIT(CORDICx->CR, CORDIC_CR_CORDIC_ERROR_INT_MASK);
}

/**
  * @brief  Enable ARCTAN_MOD_OV_MASK.
  * @param  CORDICx CORDIC Instance
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_EnableIntMask_ARCTAN_MOD_OV(CORDIC_TypeDef *CORDICx)
{
  CLEAR_BIT(CORDICx->CR, CORDIC_CR_ARCTAN_MOD_OV_MASK);
}

/**
  * @brief  Enable SQRT_INT_MASK.
  * @param  CORDICx CORDIC Instance
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_EnableIntMask_SQRT(CORDIC_TypeDef *CORDICx)
{
  CLEAR_BIT(CORDICx->CR, CORDIC_CR_SQRT_INT_MASK);
}

/**
  * @brief  Disable CORDIC_INT_MASK.
  * @param  CORDICx CORDIC Instance
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_DisableIntMask_CORDIC(CORDIC_TypeDef *CORDICx)
{
  SET_BIT(CORDICx->CR, CORDIC_CR_CORDIC_INT_MASK);
}

/**
  * @brief  Disable CORDIC_ERROR_INT_MASK.
  * @param  CORDICx CORDIC Instance
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_DisableIntMask_CORDIC_ERROR(CORDIC_TypeDef *CORDICx)
{
  SET_BIT(CORDICx->CR, CORDIC_CR_CORDIC_ERROR_INT_MASK);
}

/**
  * @brief  Disable ARCTAN_MOD_OV_MASK.
  * @param  CORDICx CORDIC Instance
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_DisableIntMask_ARCTAN_MOD_OV(CORDIC_TypeDef *CORDICx)
{
  SET_BIT(CORDICx->CR, CORDIC_CR_ARCTAN_MOD_OV_MASK);
}

/**
  * @brief  Disable SQRT_INT_MASK.
  * @param  CORDICx CORDIC Instance
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_DisableIntMask_SQRT(CORDIC_TypeDef *CORDICx)
{
  SET_BIT(CORDICx->CR, CORDIC_CR_SQRT_INT_MASK);
}

/**
  * @brief  Check if CORDIC_INT_MASK is enabled or disabled.
  * @param  CORDICx CORDIC Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_CORDIC_IsEnabledIntMask_CORDIC(CORDIC_TypeDef *CORDICx)
{
  return ((READ_BIT(CORDICx->CR, CORDIC_CR_CORDIC_INT_MASK) == 0) ? 1U : 0U);
}

/**
  * @brief  Check if CORDIC_ERROR_INT_MASK is enabled or disabled.
  * @param  CORDICx CORDIC Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_CORDIC_IsEnabledIntMask_CORDIC_ERROR(CORDIC_TypeDef *CORDICx)
{
  return ((READ_BIT(CORDICx->CR, CORDIC_CR_CORDIC_ERROR_INT_MASK) == 0) ? 1U : 0U);
}

/**
  * @brief  Check if ARCTAN_MOD_OV_MASK is enabled or disabled.
  * @param  CORDICx CORDIC Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_CORDIC_IsEnabledIntMask_ARCTAN_MOD_OV(CORDIC_TypeDef *CORDICx)
{
  return ((READ_BIT(CORDICx->CR, CORDIC_CR_ARCTAN_MOD_OV_MASK) == 0) ? 1U : 0U);
}

/**
  * @brief  Check if SQRT_INT_MASK is enabled or disabled.
  * @param  CORDICx CORDIC Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_CORDIC_IsEnabledIntMask_SQRT(CORDIC_TypeDef *CORDICx)
{
  return ((READ_BIT(CORDICx->CR, CORDIC_CR_SQRT_INT_MASK) == 0) ? 1U : 0U);
}

/**
  * @}
  */

/** @defgroup CORDIC_LL_EF_FLAG_Management FLAG_Management
  * @{
  */

/**
  * @brief  Check CCFF flag state.
  * @param  CORDICx CORDIC Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_CORDIC_IsActiveFlag_CCFF(CORDIC_TypeDef *CORDICx)
{
  return ((READ_BIT(CORDICx->SR, CORDIC_SR_CCFF) == (CORDIC_SR_CCFF)) ? 1U : 0U);
}

/**
  * @brief  Check CCEF flag state.
  * @param  CORDICx CORDIC Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_CORDIC_IsActiveFlag_CCEF(CORDIC_TypeDef *CORDICx)
{
  return ((READ_BIT(CORDICx->SR, CORDIC_SR_CCEF) == (CORDIC_SR_CCEF)) ? 1U : 0U);
}

/**
  * @brief  Check ACEF flag state.
  * @param  CORDICx CORDIC Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_CORDIC_IsActiveFlag_ACEF(CORDIC_TypeDef *CORDICx)
{
  return ((READ_BIT(CORDICx->SR, CORDIC_SR_ACEF) == (CORDIC_SR_ACEF)) ? 1U : 0U);
}

/**
  * @brief  Check BSY flag state.
  * @param  CORDICx CORDIC Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_CORDIC_IsActiveFlag_BSY(CORDIC_TypeDef *CORDICx)
{
  return ((READ_BIT(CORDICx->SR, CORDIC_SR_BSY) == (CORDIC_SR_BSY)) ? 1U : 0U);
}

/**
  * @brief  Check SCFF flag state.
  * @param  CORDICx CORDIC Instance
  * @retval State of bit (1 or 0).
  */
__STATIC_INLINE uint32_t LL_CORDIC_IsActiveFlag_SCFF(CORDIC_TypeDef *CORDICx)
{
  return ((READ_BIT(CORDICx->SR, CORDIC_SR_SCFF) == (CORDIC_SR_SCFF)) ? 1U : 0U);
}

/**
  * @brief  Clear CCFF flag
  * @param  CORDICx CORDIC Instance
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_ClearFlag_CCFF(CORDIC_TypeDef *CORDICx)
{
  CLEAR_BIT(CORDICx->SR, CORDIC_SR_CCFF);
}

/**
  * @brief  Clear CCEF flag
  * @param  CORDICx CORDIC Instance
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_ClearFlag_CCEF(CORDIC_TypeDef *CORDICx)
{
  CLEAR_BIT(CORDICx->SR, CORDIC_SR_CCEF);
}

/**
  * @brief  Clear ACEF flag
  * @param  CORDICx CORDIC Instance
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_ClearFlag_ACEF(CORDIC_TypeDef *CORDICx)
{
  CLEAR_BIT(CORDICx->SR, CORDIC_SR_ACEF);
}

/**
  * @brief  Clear SCFF flag
  * @param  CORDICx CORDIC Instance
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_ClearFlag_SCFF(CORDIC_TypeDef *CORDICx)
{
  CLEAR_BIT(CORDICx->SR, CORDIC_SR_SCFF);
}

/**
  * @}
  */

/** @defgroup CORDIC_LL_EF_Data_Management Data_Management
  * @{
  */

/**
  * @brief  Set input THETA.
  * @param  CORDICx CORDIC Instance
  * @param  Theta input THETA
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_SetTheta(CORDIC_TypeDef *CORDICx, int32_t Theta)
{
  WRITE_REG(CORDICx->THETA, Theta);
}

/**
  * @brief  Get input THETA.
  * @param  CORDICx CORDIC Instance
  * @retval Return input THETA
  */
__STATIC_INLINE int32_t LL_CORDIC_GetTheta(CORDIC_TypeDef *CORDICx)
{
  return (int32_t)(READ_REG(CORDICx->THETA));
}

/**
  * @brief  Get SIN calculation result.
  * @param  CORDICx CORDIC Instance
  * @retval Return SIN calculation result
  */
__STATIC_INLINE int32_t LL_CORDIC_GetSin(CORDIC_TypeDef *CORDICx)
{
  return (int32_t)(READ_REG(CORDICx->SIN));
}

/**
  * @brief  Get COS calculation result.
  * @param  CORDICx CORDIC Instance
  * @retval Return COS calculation result
  */
__STATIC_INLINE int32_t LL_CORDIC_GetCos(CORDIC_TypeDef *CORDICx)
{
  return (int32_t)(READ_REG(CORDICx->COS));
}

/**
  * @brief  Set input coordinate X.
  * @param  CORDICx CORDIC Instance
  * @param  CoordinateX Input coordinate X
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_SetCoordinateX(CORDIC_TypeDef *CORDICx, int32_t CoordinateX)
{
  WRITE_REG(CORDICx->X, CoordinateX);
}

/**
  * @brief  Get input coordinate X.
  * @param  CORDICx CORDIC Instance
  * @retval Return input coordinate X
  */
__STATIC_INLINE int32_t LL_CORDIC_GetCoordinateX(CORDIC_TypeDef *CORDICx)
{
  return (int32_t)(READ_REG(CORDICx->X));
}

/**
  * @brief  Set input coordinate Y.
  * @param  CORDICx CORDIC Instance
  * @param  CoordinateY Input coordinate Y
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_SetCoordinateY(CORDIC_TypeDef *CORDICx, int32_t CoordinateY)
{
  WRITE_REG(CORDICx->Y, CoordinateY);
}

/**
  * @brief  Get input coordinate Y.
  * @param  CORDICx CORDIC Instance
  * @retval Return input coordinate Y
  */
__STATIC_INLINE int32_t LL_CORDIC_GetCoordinateY(CORDIC_TypeDef *CORDICx)
{
  return (int32_t)(READ_REG(CORDICx->Y));
}

/**
  * @brief  Get MOD calculation result.
  * @param  CORDICx CORDIC Instance
  * @retval Return MOD calculation result
  */
__STATIC_INLINE int32_t LL_CORDIC_GetMod(CORDIC_TypeDef *CORDICx)
{
  return (int32_t)(READ_REG(CORDICx->MOD));
}

/**
  * @brief  Get ARCTAN calculation result.
  * @param  CORDICx CORDIC Instance
  * @retval Return ARCTAN calculation result
  */
__STATIC_INLINE int32_t LL_CORDIC_GetArctan(CORDIC_TypeDef *CORDICx)
{
  return (int32_t)(READ_REG(CORDICx->ARCTAN));
}

/**
  * @brief  Set input radicand.
  * @param  CORDICx CORDIC Instance
  * @param  Rad Input radicand
  * @retval None
  */
__STATIC_INLINE void LL_CORDIC_SQRT_SetRad(CORDIC_TypeDef *CORDICx, uint32_t Rad)
{
  WRITE_REG(CORDICx->DSP_RAD, Rad);
}

/**
  * @brief  Get input radicand.
  * @param  CORDICx CORDIC Instance
  * @retval Return input radicand
  */
__STATIC_INLINE uint32_t LL_CORDIC_SQRT_GetRad(CORDIC_TypeDef *CORDICx)
{
  return READ_REG(CORDICx->DSP_RAD);
}

/**
  * @brief  Get Square Root Result.
  * @param  CORDICx CORDIC Instance
  * @retval Return Square Root Result
  */
__STATIC_INLINE uint16_t LL_CORDIC_SQRT_GetResult(CORDIC_TypeDef *CORDICx)
{
  return (uint16_t)(READ_REG(CORDICx->DSP_SQRT));
}

/**
  * @}
  */



#if defined(USE_FULL_LL_DRIVER)
/** @defgroup CORDIC_LL_EF_Init Initialization and de-initialization functions
  * @{
  */
ErrorStatus LL_CORDIC_DeInit(CORDIC_TypeDef *CORDICx);

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

#endif /* defined(CORDIC) */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* PY32F031_LL_CORDIC_H */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"	
    
#define T1_PIN GPIO_PIN_8
#define T1_GPIO_PORT GPIOD
#define T1_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define T1_HI T1_GPIO_PORT->BSRR =T1_PIN
#define T1_LO T1_GPIO_PORT->BSRR = ((uint32_t)T1_PIN) << 16U
#define T1_TG T1_GPIO_PORT->ODR ^= T1_PIN

#define T2_PIN GPIO_PIN_9
#define T2_GPIO_PORT GPIOD
#define T2_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define T2_HI T2_GPIO_PORT->BSRR =T2_PIN
#define T2_LO T2_GPIO_PORT->BSRR = ((uint32_t)T2_PIN) << 16U
#define T2_TG T2_GPIO_PORT->ODR ^= T2_PIN

extern __IO uint32_t *CYCCNT; // Cycle Count Register
extern __IO uint32_t *CNTRL;  // Control Register

static __INLINE void init_cyc_tick(void) {
  *CYCCNT = 0;
  *CNTRL = *CNTRL | 1;
}
static __INLINE uint32_t cyc_tick(void) { return DWT->CYCCNT; }


/* Private includes ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* For configMAX_PRIORITIES =7 */   
typedef enum {
	TreadPrioIdle = configMAX_PRIORITIES - 7,         /*< priority: idle (lowest)      */           
	TreadPrioLow = configMAX_PRIORITIES - 6,          /*< priority: low                */   
	TreadPrioBelowNormal = configMAX_PRIORITIES - 5,  /*< priority: below normal       */  
	TreadPrioNormal = configMAX_PRIORITIES - 4,       /*< priority: normal (default)   */
	TreadPrioAboveNormal = configMAX_PRIORITIES - 3,  /*< priority: above normal       */
	TreadPrioHigh = configMAX_PRIORITIES - 2,         /*< priority: high               */
	TreadPrioRealtime = configMAX_PRIORITIES - 1,     /*< priority: realtime (highest) */
} thread_prio_t;  
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
/* Private defines -----------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

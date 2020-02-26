/**
  ******************************************************************************
  * @file    hooks.c 
  * @author  Trembach D.N.
  * @version V1.5.0
  * @date    25-02-2020
  * @brief   Файл вспомогательных функций портирования FreeRTOS.
  *
  *
  ******************************************************************************
  * @attention
  *
  *
  * <h2><center>&copy; COPYRIGHT 2020 OneTiOne </center></h2>
  ******************************************************************************
  */

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h"

static volatile unsigned int stackOverflow;
static volatile unsigned int mallocFailed;

/* Указать размешение heap */
#if ( configAPPLICATION_ALLOCATED_HEAP == 1 )
/* Allocate the memory for the heap. */
uint8_t __attribute__((section(".RTOS_HEAP_CCMRAM"))) ucHeap[configTOTAL_HEAP_SIZE];	
#endif /* configAPPLICATION_ALLOCATED_HEAP */

/**
  * @brief  Функция вызывается при переполнении стека FreeRTOS, 
  * @param  TaskHandle_t xTask - хэндл задачи
  * @param  signed portCHAR *pcTaskName  - имя текущей задачи
  * @retval None
  */
void vApplicationStackOverflowHook(TaskHandle_t xTask,
                                   signed portCHAR *pcTaskName)
{
	stackOverflow++;
	printf("StackOverflowHook: %s\n", pcTaskName);
        while(1);
}

/**
  * @brief  Функция вызывается при отсутствии памяти в куче FreeRTOS, 
  *    
  * @param  None
  * @retval None
  */
void vApplicationMallocFailedHook(void)
{
	mallocFailed++;
	printf("MallocFailedHook: %d\n", xPortGetFreeHeapSize());
        while(1);
}

/**
  * @brief  Функция настройки и запуска таймера, 
  *         который используется для подсчета времени работы задач FreeRTOS 
  *         
  *         Время тика таймера должено быть, по крайней мере в 10 раз меньше
  *         частоты тика FreeRTOS
  *    
  * @param  None
  * @retval None
  */
void vConfigureTimerForRunTimeStats(void)
{
  TIM_HandleTypeDef     Tim_Handle;
  RCC_ClkInitTypeDef    clkconfig;
  uint32_t              uwTimclock, uwAPB1Prescaler = 0U;
  uint32_t              uwPrescalerValue = 0U;
  uint32_t              pFLatency;

  /* Enable TIM5 clock */
  __HAL_RCC_TIM5_CLK_ENABLE();
  
  /* Get clock configuration */
  HAL_RCC_GetClockConfig(&clkconfig, &pFLatency);
  
  /* Get APB1 prescaler */
  uwAPB1Prescaler = clkconfig.APB1CLKDivider;
  
  /* Compute TIM5 clock */
  if (uwAPB1Prescaler == RCC_HCLK_DIV1) 
  {
    uwTimclock = HAL_RCC_GetPCLK1Freq();
  }
  else
  {
    uwTimclock = 2*HAL_RCC_GetPCLK1Freq();
  }
  
  /* Compute the prescaler value to have TIM5 counter clock equal to 0.01 MHz */
  uwPrescalerValue = (uint32_t) ((uwTimclock / 1000000U) - 1U);
  
  /* Initialize TIM5 */
  Tim_Handle.Instance = TIM5;
  
  /* Initialize TIMx peripheral as follow:
  + Period = [(TIM5CLK/1000) - 1]. to have a (1/1000) s time base.
  + Prescaler = (uwTimclock/1000000 - 1) to have a 1MHz counter clock.
  + ClockDivision = 0
  + Counter direction = Up
  */
  Tim_Handle.Init.Period = 0xFFFFFFFF;
  Tim_Handle.Init.Prescaler = uwPrescalerValue;
  Tim_Handle.Init.ClockDivision = 0;
  Tim_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;
  HAL_TIM_Base_Init(&Tim_Handle);
  HAL_TIM_Base_Start(&Tim_Handle);
}

/**
  * @brief  Функция возращает значения счетчика таймера, 
  *         который используется для подсчета времени работы задач FreeRTOS 
  *         
  * @param  None
  * @retval unsigned long значение счетчика таймера
  */
unsigned long ulGetRuntimeCounterValue(void)
{
  return TIM5->CNT;
}

/**************    COPYRIGHT 2020 OneTiOne Tech. Co., Ltd.   *****END OF FILE*******/
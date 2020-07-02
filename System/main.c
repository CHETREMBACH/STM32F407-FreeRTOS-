/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "lwip.h"
#include "printf_dbg.h"
#include "cmd_process.h"
#include "poe_driver.h"

// Формирование кода версии
volatile const char __version__[] = "BOARD STM32F07VE";    
volatile const char __date__[] = __DATE__;
volatile const char __time__[] = __TIME__;

volatile __IO uint32_t *CYCCNT = (__IO uint32_t *)0xE0001004;  // Cycle Count Register
volatile __IO uint32_t *CNTRL = (__IO uint32_t *)0xE0001000;  // Control Register

/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Definitions for defaultTask */
/* Private function prototypes -----------------------------------------------*/

/**
 * @brief  Start Thread 
 * @param  None
 * @retval None
 */
void system_thread(void *arg)
{ 
     GPIO_InitTypeDef GPIO_InitStruct;
     uint32_t time_temp;

	//Подключение интерфейса отладки
	DBG_Hardware_Setup();

	/* init code for LWIP */
	LWIP_Init();	
		
	//Инициализация задачи диагностического терминала 
	xTaskCreate(terminal_task, (const char*)"CmdTrmnl", configMINIMAL_STACK_SIZE * 5, NULL, TreadPrioNormal, NULL);
	
     //Инициализация выводов тест
     GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
     GPIO_InitStruct.Pull = GPIO_PULLUP;
     GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

     T1_GPIO_CLK_ENABLE();
     GPIO_InitStruct.Pin = T1_PIN;
     HAL_GPIO_Init(T1_GPIO_PORT, &GPIO_InitStruct);
     T1_HI;

     T2_GPIO_CLK_ENABLE();
     GPIO_InitStruct.Pin = T2_PIN;
     HAL_GPIO_Init(T2_GPIO_PORT, &GPIO_InitStruct);
     T2_HI;
     //Инициализация циклического счетчика 
     init_cyc_tick();

	/*  Функция настройки интерфейса poe контроль.*/
	Init_POE_Config();
	
	
	// Информационная шапка программы
	printf("______________________________________________\r\n");
	printf("\r\n");
	printf("   %s \r\n", __version__);
	printf("   DATA: %s \r\n", __date__);
	printf("   TIME: %s \r\n", __time__);
	printf("   CPU FREQ = %.9lu Hz \r\n", SystemCoreClock);  
	printf("______________________________________________\r\n"); 
  
	
    for (;;) {
      vTaskDelay(500);
    }
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();
 
	/* Init thread */
	xTaskCreate(system_thread, (const char*)"SysTask", configMINIMAL_STACK_SIZE * 2, NULL, TreadPrioNormal, NULL);
	
	/* Start scheduler */
	vTaskStartScheduler();
  
	/* We should never get here as control is now taken by the scheduler */
	NVIC_SystemReset();
	return 0;
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

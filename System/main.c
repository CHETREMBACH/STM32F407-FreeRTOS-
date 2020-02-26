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
#include "printf_dbg.h"
#include "cmd_process.h"

// Формирование кода версии
volatile const char __version__[] = "BOARD STM32F07VE";    
volatile const char __date__[] = __DATE__;
volatile const char __time__[] = __TIME__;

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
	//Подключение интерфейса отладки
	DBG_Hardware_Setup();

	//Инициализация задачи диагностического терминала 
	xTaskCreate(terminal_task, (const char*)"CmdTrmnl", configMINIMAL_STACK_SIZE * 2, NULL, TreadPrioNormal, NULL);
	
	// Информационная шапка программы
	printf("______________________________________________\r\n");
	printf("\r\n");
	printf("   %s \r\n", __version__);
	printf("   DATA: %s \r\n", __date__);
	printf("   TIME: %s \r\n", __time__);
	//printf("   CPU FREQ = %.9d Hz \r\n", SystemCoreClock);  
	printf("______________________________________________\r\n"); 
  
	for (;;)
	{
		vTaskDelay(500); 
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

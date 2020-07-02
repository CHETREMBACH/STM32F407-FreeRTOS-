/**
  ******************************************************************************
  * @file    poe_driver.c
  * @author  Трембач Д.Н., Дэйта Экспресс
  * @version V2.0.0
  * @date    Создан:  11.06.2020
  *   	    Изменен:  11.06.2020	
  * @brief   Модуль работы интерфейса poe контроль
  *          
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "main.h"
#include "driver_i2c.h"
#include "poe_cntrl.h"

#define NOTIFY_IRQ_POE         (1l<<1) 
#define NOTIFY_SOFT_TIMER      (1l<<2)  

TaskHandle_t            handle_poe_task;        /*  Указатель на собственную задачу                         */ 
TimerHandle_t           xSoftTimer;             /*  Програмный таймер периодического уведомления задачи     */
                                                /*  Содержимое сообщения полученного задачей                 */   
static uint32_t         NotifiedValue;    

uint8_t dst_buf[40];

/* буфер отправляемого пакета */
uint8_t box_buf[11];

/**
  * @brief  Обработчик прерываний линий Wiegand.
  * @param  None
  * @retval None
  */
void EXTI9_5_IRQHandler(void)
{
	BaseType_t xHigherPriorityTaskWoken;  
	/* EXTI line interrupt detected */
	if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_14) != RESET)
	{
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_14);
		if (handle_poe_task  != NULL)
		{    
			xTaskNotifyFromISR( handle_poe_task,
				NOTIFY_IRQ_POE,
				eSetBits,
				&xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);      
		} 		
	}
}

/**
  * @brief  Функция отработки програмного таймера 
  * @param  TimerHandle_t pxTimer - указатель на таймер вызвавщий функцию
  * @retval None
  */
void TimCallback( TimerHandle_t pxTimer )
{
  /* Функция обработки програмного таймера.*/
  /* Усли Указатель на задачу не нулевой */
  if (  handle_poe_task  != NULL  )
  {
    if ( xSoftTimer == pxTimer )
    {/* Устанавливаем событие Uptime */
      xTaskNotify( handle_poe_task,                  /* Указатель на уведомлюемую задачу                         */
                  NOTIFY_SOFT_TIMER,                     /* Значения уведомления                                     */
                  eSetBits );                            /* Текущее уведомление добавляются к уже прописанному       */
    }    
  }  
}

/**
  * @brief  Задача контроля работы POE
  * @param  pvParameters - None
  * @retval None
  */
void Control_POE_Task(void * pvParameters)
{
  /* Запуск таймера */
  if ( xSoftTimer != NULL )
  {
    xTimerStart( xSoftTimer, 0 );
  }
    
  init_poe_cntrl();
  
  /* Цикл периодического запуска задачи                                       */ 
  while (1) 
  {
    /* Обнуляем сообщение */
    NotifiedValue = 0;
    /*================================== Проверка наличия сообщений ====================================*/
    xTaskNotifyWait(0x00000000,       /* Не очищайте биты уведомлений при входе               */
                    0xFFFFFFFF,       /* Сбросить значение уведомления до 0 при выходе        */
                    &(NotifiedValue), /* Значение уведомленное передается в  NotifiedValue    */
                    portMAX_DELAY  ); /* Блокировка задачи до появления уведомления           */
    /* Получено уведомление. Проверяем, какие биты были установлены. */
    /*=========================================================================*/
    /*=========================================================================*/
    if( ( NotifiedValue  & NOTIFY_SOFT_TIMER ) != 0 )
    { /* Периодическое уведомление по таймеру */
      period_poe_cntrl();
    }
    /*=========================================================================*/    
    /*=========================================================================*/      
    if ( ( NotifiedValue  & NOTIFY_IRQ_POE ) != 0 )
    { /* Уведомление - пакет прочитан - отправляем */
      irq_poe_cntrl();
    }  
  }
}

/**
  * @brief  Функция настройки интерфейса poe контроль.
  * @param  None
  * @retval None
  */
void Init_POE_Config(void)
{
	GPIO_InitTypeDef   GPIO_InitStructure;
	/* Enable GPIOA clock */
	__HAL_RCC_GPIOB_CLK_ENABLE();
  
	/* Configure PA0 pin as input floating */
	GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStructure.Pull = GPIO_PULLUP;
	GPIO_InitStructure.Pin = GPIO_PIN_14;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* Enable and set EXTI Line14 Interrupt to the lowest priority */
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 7, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);	
  
  /* Открытие таймера периодического уведомления задачи */
  xSoftTimer = xTimerCreate( "TmUpTask",        /* Текстовое имя, не используется в RTOS kernel.                    */
                            5000,               /* Период таймера в тиках.                                          */
                            pdTRUE,             /* Время будет автоматически перезагружать себя, когда оно истечет. */
                            NULL,               /* В функцию параметры не передаем                                  */
                            TimCallback );      /* Указатель на функцию , которую вызывает таймер.                  */ 
  
  /* Открытие задачи контроля  */
  xTaskCreate( Control_POE_Task, (const char*)"poe_tsk", configMINIMAL_STACK_SIZE, NULL, TreadPrioAboveNormal, &handle_poe_task );
}

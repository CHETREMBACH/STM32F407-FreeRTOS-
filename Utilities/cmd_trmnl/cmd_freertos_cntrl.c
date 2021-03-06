/**
 ******************************************************************************
 * @file    cmd_freertos_cntrl.c
 * @author  Trembach D.N.
 * @version V1.2.0
 * @date    03-12-2019
 * @brief   файл  функций обработчиков команд статистики FreeRtos  
 ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2020 OneTiOne </center></h2>
 ******************************************************************************
 */

 /* Includes ------------------------------------------------------------------*/
#include "cmd_freertos_cntrl.h"
#include "cmd_hlp.h"

/* Описание массива статистики по задачам */
TaskStatus_t TaskStArray[MAX_NUMBER_TASK_STAT];

/**
  * @brief  формирование строки времени работы системы разделенной на дни часы минуты секунды миллесекунды.
  * @param  None
  * @retval None
  */
static void data_time_work( void )
{
  uint32_t temp_day;
  uint32_t temp_hour;
  uint32_t temp_min;
  uint32_t temp_sec;
  uint32_t temp_mlsec;  
  uint32_t temp_time_work;
  
  /* Считываем текущее время */
  temp_time_work = xTaskGetTickCount();
  /* Вычисляем число дней    */
  temp_day = (uint32_t)(temp_time_work/(1000*60*60*24));
  /* Определяем остаток      */ 
  temp_time_work = temp_time_work - temp_day*(1000*60*60*24);
  /* Вычисляем число часов   */
  temp_hour = (uint32_t)(temp_time_work/(1000*60*60));
  /* Определяем остаток      */
  temp_time_work = temp_time_work - temp_hour*(1000*60*60);
  /* Вычисляем число минут   */
  temp_min = (uint32_t)(temp_time_work/(1000*60));
  /* Определяем остаток      */
  temp_time_work = temp_time_work - temp_min*(1000*60);
  /* Вычисляем число секунд  */
  temp_sec = (uint32_t)(temp_time_work/(1000));
  /* Определяем остаток - миллисекунд */
  temp_mlsec = temp_time_work - temp_sec*(1000);
  /* Время работы FreeRTOS */
  printf("\nTime work  %.2lu:%.2lu:%.2lu:%.2lu:%.3lu ms\n",temp_day,temp_hour,temp_min,temp_sec,temp_mlsec);	
} 

/**
  * @brief  формирование строки свободного места системы.
  * @param  None
  * @retval None
  */
static void free_heap( void )
{
  
  printf("\nFreeHeapMemSize     %d byte\n",xPortGetFreeHeapSize());	 
}

/**
  * @brief  формирование строки числа задач системы.
  * @param  None
  * @retval None
  */
static void number_task( void )
{
  printf("\nNumber of tasks    %3lu\n",uxTaskGetNumberOfTasks());	
}

/**
  * @brief  формирование титульной строки задач системы.
  * @param  None
  * @retval None
  */
static void title_task( void )
{
  printf("\nNumber   Name     Pri  FStсk State Time%% Time tick\n");	
} 

/**
  * @brief  формирование строки параметров задачи.
  * @param  None
  * @retval None
  */
static void var_task( void )
{
  /* Переменная для вычисления времени работы задачи в процентах */
  uint32_t PercentageTime;
  
  /* счетчики задач */
  UBaseType_t num_print_task,number_of_tasks;
  
  char Task_Stats;
  uint32_t ulStatsAsPercentage;  
    
  /* Формирование массива статистики по задачам */
  number_of_tasks = uxTaskGetSystemState( TaskStArray, (UBaseType_t)MAX_NUMBER_TASK_STAT, &PercentageTime );
  
  /* Для процентных расчетов. */
  PercentageTime /= 100UL;    
  
  /* Цикл вывода статистики по задачам на экран  */
  for(UBaseType_t cnt_task = 0; cnt_task < number_of_tasks; cnt_task++)
  {
    /* поиск в массиве задачи с данным порядковым номером	*/
    for(num_print_task = 0; num_print_task < number_of_tasks; num_print_task++)
    {
      if (TaskStArray[ num_print_task ].xTaskNumber == (cnt_task+1))
      {
        /* What percentage of the total run time has the task used?
        This will always be rounded down to the nearest integer.
        ulTotalRunTimeDiv100 has already been divided by 100. */
        ulStatsAsPercentage = TaskStArray[ num_print_task ].ulRunTimeCounter / PercentageTime;
        
        /* Create a human readable table from the binary data. */
        
        switch( TaskStArray[ num_print_task ].eCurrentState )
        {
        case eRunning:		Task_Stats = ( 'X' );
        break;          
        
        case eReady:		Task_Stats = ( 'R' );
        break;
        
        case eBlocked:		Task_Stats = ( 'B' );
        break;
        
        case eSuspended:	Task_Stats = ( 'S' );
        break;
        
        case eDeleted:		Task_Stats = ( 'D' );
        break;
        
        default:	/* Should not get here, but it is included
          to prevent static checking errors. */
          Task_Stats = ( 'A' );
          break;
        }

        if( ulStatsAsPercentage > 0UL )
        {
	        printf("%2u %12s   %2u   %5lu   %c    %2u%%     %lu\n",
		        (uint8_t) TaskStArray[num_print_task].xTaskNumber,
                        TaskStArray[ num_print_task ].pcTaskName,
                        ( uint8_t ) TaskStArray[ num_print_task ].uxCurrentPriority,
                        ( uint32_t ) TaskStArray[ num_print_task ].usStackHighWaterMark,
                        Task_Stats,
                        ( uint8_t ) ulStatsAsPercentage,
                        ( uint32_t ) TaskStArray[ num_print_task ].ulRunTimeCounter );
          
        }
        else
        {
	        printf("%2u %12s   %2u   %5lu   %c    <1%%     %lu\n",
		        (uint8_t) TaskStArray[num_print_task].xTaskNumber,
                        TaskStArray[ num_print_task ].pcTaskName, 
                        ( uint8_t ) TaskStArray[ num_print_task ].uxCurrentPriority, 
                        ( uint32_t ) TaskStArray[ num_print_task ].usStackHighWaterMark,
                        Task_Stats,							
                        ( uint32_t ) TaskStArray[ num_print_task ].ulRunTimeCounter );
        }
        
        vTaskDelay(1);
      }	
    }
  }
  printf("\n");
}

/**
  * @brief  обработчик команды просмотра параметров FreeRTOS
  * @param  cmd_parametr_t *parametr - указатель на параметр команды
  * @retval uint16_t - возвращаемое значение
  */
uint16_t freertos_cmd_handler(cmd_parametr_t *parametr)
{
  if (parametr != NULL)
  {
    if (strcmp(parametr->var_ch, "time") == 0)
    {
      data_time_work();
      return 0;	
    }
    
    if (strcmp(parametr->var_ch, "heap") == 0)
    {
      free_heap();
      return 0;	
    }    
       
    if (strcmp(parametr->var_ch, "all") == 0)    
    { 
      data_time_work();      
      free_heap();
      number_task();
      title_task();
      var_task();      
      return 0;	
    }
  }    
  
  printf("Invalid parameter\r\n");    
  return 0;					
}

/*================= команда статуса freertos =============================*/
const cmd_t freertos_cmd = 
{ 
  /* имя ( мнемомика ) команды        */   
  "freertos",
  /* тип параметра команды            */
    VR_STRING, VR_NONE, VR_NONE, VR_NONE, VR_NONE,
  /* краткое описание команды         */
  "FreeRTOS view control command",
  /* расширенное описание команды     */ 
  "the command is designed to view the status of resources FreeRTOS.\r\n  command format: >freertos <STRING> \r\n   time - system uptime. \r\n   heap - size free memory heap. \r\n   all - task list status.",
  /* указатель на обработчик команд   */    		
  freertos_cmd_handler
};

/**
  * @brief  Инициализация указателя на команду просмотра параметров LwIP
  * @param  none
  * @retval none
  */
void freertos_cmd_init()
{
   add_terminal_cmd(&freertos_cmd);   
}

/******************* (C) COPYRIGHT 2020 OneTiOne  *****END OF FILE****/
 /**
  ******************************************************************************
  * @file    cmd_process.c
  * @author  Trembach D.N.
  * @version V1.0.0
  * @date    28-09-2019
  * @brief   файл функций обработки запросов команд терминала  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2020 OneTiOne </center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include "cmd_hdl.h"
#include "cmd_hlp.h"

#if  (LOG_STRIME_ENABLE == 1) 
/* запуск статистики с роутера */
#include "router_log.h"
#endif /*#if  (LOG_STRIME_ENABLE == 1) */ 

/* Массив указателей на описание команд с указателями на обработчик */
cmd_t* array_cmd[MAX_SIZE_NUM_CMD];
/* число команд терминала */
uint8_t max_num_cmd = 0;

/* индекс загрузки команды/параметра команды */
uint8_t index_wr_cmd;
/* буфер данных для команды */
char data_cmd[MAX_SIZE_CMD];
/* буфер данных для параметра команды */
char data_param_cmd[MAX_SIZE_PARAM];
/* автомат состояний для приема данных */
uint8_t fsm_dat_prc = 0;

/**
  * @brief  функция поиска мнемомики в массиве команд
  * @param  chat* cmd_name - указатель мнемокод команды
  * @retval uint8_t - индекс команды, если 255 - нет совпадений по команде 
  */
uint8_t scan_cmd( char* cmd_name )
{
  /* Цикл по всем командам */
  for ( uint8_t i = 0; i < max_num_cmd;i++ )
  {  
    if ( strcmp( cmd_name,array_cmd[i]->cmd_name) == 0)
    {
      /* есть совпадение */
      return i;
    }
  }
  /* нет совпадений по команде */
  return 255;  
}

/**
  * @brief  функция конвертирования строки в параметр заданного типа с проверкой  
  * @param  type_cmd_prmtr_e  cmd_type_parametr - тип параметра команды
  * @param  cmd_parametr_t *parametr - указатель на структуру параметра команды
  * @retval uint8_t статус конвертирования параметра
  *    0 - успешное конвертирование
  *    1 - некорректные данные
  *    2 - переполнение
  *    3 - данные отсутствуют
  */
uint8_t get_parametr_cmd( type_cmd_prmtr_e  cmd_type_parametr, char *ch_param ,cmd_parametr_t *parametr )
{
  uint32_t temp_var_u32;  
  int32_t temp_var_i32;  
  
  /* Проверка наличие данных для конвертирования */
  if ( ch_param[0] == '\0' )
  {
    /* Если задан тип с пустым параметром */
    if ( cmd_type_parametr == VR_NONE )
    {
      /* успешное конвертирование*/
      return  0;      
    }
    /* данные для конвертирования отсутствуют */
    return 3;    
  }
  
  switch(cmd_type_parametr)
  {
  case  VR_UINT8:
  case VR_UINT16:
  case VR_UINT32:

    if ( sscanf( ch_param, "%lu", &temp_var_u32 ) == 1)
    {
      /* Проверка для UINT8 */
      if ( cmd_type_parametr == VR_UINT8 )
      {
        if ( temp_var_u32 <= 0xFF )
        {
          parametr->var_u08 = (uint8_t)temp_var_u32;
          /* успешное конвертирование*/
          return  0;            
        }
        else
        { 
          /* переполнение */
          return  2;          
        }  
      }
      
      /* Проверка для UINT16 */
      if ( cmd_type_parametr == VR_UINT16 )
      {
        if ( temp_var_u32 <= 0xFFFF  )
        {
          parametr->var_u16 = (uint16_t)temp_var_u32;
          /* успешное конвертирование*/
          return  0;            
        }
        else
        { 
          /* переполнение */
          return  2;          
        }  
      }
      
      /* Проверка для UINT32 уже выполнена при конвертировнии */ 
      parametr->var_u32 = temp_var_u32;
      /* успешное конвертирование*/
      return  0;
    }
    /* некорректные данные */
    return 1;
  
    
  case   VR_INT8:       
  case  VR_INT16:       
  case  VR_INT32: 

    if ( sscanf( ch_param, "%li", &temp_var_i32 ) == 1)
    {
      /* Проверка для INT8 */
      if ( cmd_type_parametr == VR_INT8 )
      {
        if ( ( temp_var_i32 <= 127 )&&( temp_var_i32 >=  -128 ) )
        {
          parametr->var_i08 = (int8_t)temp_var_i32;
          /* успешное конвертирование*/
          return  0;            
        }
        else
        { 
          /* переполнение */
          return  2;          
        }
      }
      
      /* Проверка для INT16 */
      if ( cmd_type_parametr == VR_INT16 )
      {
        if ( ( temp_var_i32 <= 32767 )&&( temp_var_i32 >= -32768 ) )
        {
          parametr->var_i16 = (int16_t)temp_var_i32;
          /* успешное конвертирование*/
          return  0;            
        }
        else
        { 
          /* переполнение */
          return  2;          
        }  
      }
      /* Проверка для INT32 уже выполнена при конвертировнии */ 
      parametr->var_i32 =  temp_var_i32;
      /* успешное конвертирование*/
      return  0;
    }
    /* некорректные данные */
    return 1;
    
  case VR_DOUBLE:
    if ( sscanf( ch_param, "%la", &(parametr->var_dbl) ) == 1)
    {
      /* успешное конвертирование*/
      return  0;    
    }
    else
    {
      /* некорректные данные */
      return 1;
    }  
    
  case VR_CHAR: 
    /* Если второй символ - завершение строки */
    if ( ch_param[1] == '\0' )
    { 
      parametr->var_ch[0] = ch_param[0]; 
      /* успешное конвертирование*/
      return  0;    
    }
    else
    { 
      /* переполнение */
      return  2;          
    }  
    
  case VR_STRING:
    /* Копирование данных */
    strncpy( parametr->var_ch, ch_param, (MAX_SIZE_PARAM - 1));
    /* Установка завершающего символа ( на всякий случай ) */
    parametr->var_ch[(MAX_SIZE_PARAM - 1)] = '\0';
    /* успешное конвертирование*/
    return  0;    
    
  case VR_NONE:
  default:
    /* некорректные данные */
    return 1;
  }
}

/**
  * @brief  функция обработки команд  
  * @param  char *cmd - указатель на мнемомику команды
  * @param  char *param - указатель на мнемомику параметра
  * @retval none 
  */
void command_processing( char *cmd, char *param  )
{
  uint8_t index_cmd; 
  cmd_parametr_t cmd_prmtr;
  uint8_t status_cmd_prmtr;
#ifdef  DBG_PRCS_EN    
  if ( param != NULL )  printf("Получена команда: <%s> с параметром: <%s>\r\n",cmd,param );/* Функция с параметром   */ 
                  else  printf("Получена команда: <%s> без параметров\r\n",cmd );          /* Функция без параметров */
#endif  
  /* Получение индекса команды */
  index_cmd = scan_cmd(cmd);
  if ( index_cmd == 255)
  {
    /* нет совпадений по команде */
#ifdef  DBG_PRCS_EN          
    printf("Команда не распознана  \r\n\r\n"); 
#else
  #ifdef  DBG_ERR_PRCS_EN          
      printf("Команда не распознана  \r\n\r\n"); 
  #endif 
#endif  
  }
  else
  {
    /* есть совпадений по команде */
#ifdef  DBG_PRCS_EN       
    printf("Команда распознана  \r\n");
#endif   
    if ( param != NULL )
    {  
      /* Подготовка параметра */
	    status_cmd_prmtr = get_parametr_cmd(array_cmd[index_cmd]->cmd_type_parametr, param, &cmd_prmtr);
      /*  */
	    switch (status_cmd_prmtr)  
      {
      case 0: /*- успешное конвертирование*/
#ifdef  DBG_PRCS_EN   
        printf("Успешное конвертирование параметра.\r\n");
#endif 
        /* Вызов функции обработчика */
	      array_cmd[index_cmd]->cmd_handler_cb(&cmd_prmtr);
        /* Выход */
#ifdef  DBG_PRCS_EN 
        printf("\r\n");       
#endif 
        return;
      case 1: /*- некорректные данные     */
#ifdef  DBG_PRCS_EN          
        printf("Данные параметра некорректные - команда отменена.\r\n\r\n");  
#else
  #ifdef  DBG_ERR_PRCS_EN          
        printf("Данные параметра некорректные - команда отменена.\r\n\r\n");  
  #endif 
#endif         
        return;
      case 2: /*- переполнение            */
#ifdef  DBG_PRCS_EN          
        printf("Переполнение при конвертировании параметра - команда отменена.\r\n\r\n");  
#else
  #ifdef  DBG_ERR_PRCS_EN          
        printf("Переполнение при конвертировании параметра - команда отменена.\r\n\r\n");   
  #endif 
#endif          
        return;
      case 3: /*- данные отсутствуют      */
      default:
#ifdef  DBG_PRCS_EN   
        printf("Данные параметра отсутствуют.\r\n");
#endif 
        /* Вызов функции обработчика без параметров */
        array_cmd[index_cmd]->cmd_handler_cb( NULL );
        /* Выход */
#ifdef  DBG_PRCS_EN  
        printf("\r\n"); 
#endif        
        return;      
      }
    }
    else
    {
#ifdef  DBG_PRCS_EN  
      printf("Данные параметра отсутствуют.\r\n");
#endif          
      /* Вызов функции обработчика без параметров */
      array_cmd[index_cmd]->cmd_handler_cb( NULL );
      /* Выход */
#ifdef  DBG_PRCS_EN  
      printf("\r\n");         
#endif          
    }  
  }  
}

/**
  * @brief  функция конвертирования строки в параметр заданного типа с проверкой  
  * @param  type_cmd_prmtr_e  cmd_type_parametr - тип параметра команды
  * @param  cmd_parametr_t *parametr - указатель на структуру параметра команды
  * @retval uint8_t статус конвертирования параметра
  *    0 - успешное конвертирование
  *    1 - некорректные данные
  *    2 - переполнение
  *    3 - данные отсутствуют
  */

/**
  * @brief  функция обработки терминальных данных 
  * @param  char ch - входящий байт
  * @retval none 
  */
void terminal_data_processing( char ch )
{ 
  /**/
  switch( fsm_dat_prc )
  {
  case 0:
    /* Поиск начала команды */
    if ( ch == '>' )
    {
      /* Найдено начало команды */
      fsm_dat_prc = 1;
      index_wr_cmd = 0;
    }
    break;
    
  case 1:
    /* Загрузка мнемомики команды */
    if ( ch == ' ' ) 
    {
      /* Найдено конец команды */
      /* Установка завершающего символа */
      data_cmd[index_wr_cmd] = 0;
      /* Включение режима загрузки параметра */
      fsm_dat_prc = 2;
    }
    else
    {
      if ( ( ch == 0x0D ) || ( ch == 0x0A ) )
      {
        /* Найдено конец команды */
        /* Установка завершающего символа */
        data_cmd[index_wr_cmd] = 0;
        /* Выполнение команды без параметра */
        /* функция обработки команд   */
        command_processing( data_cmd, NULL );        
        /* Включение режима поиск начала команды */
        fsm_dat_prc = 0;
      }
      else
      {
        if ( ch == '>' ) 
        {
          /* Найдено начало команды */
          /* Установка завершающего символа */
          data_cmd[index_wr_cmd] = 0;
          /* Выполнение команды без параметра */
          /* функция обработки команд   */
          command_processing( data_cmd, NULL );   
          /* Включение режима загрузки команды */
          fsm_dat_prc = 1;
          index_wr_cmd = 0;
        }
        else
        {  
          /* Загрузка команды */
          data_cmd[index_wr_cmd] = ch;
          /* При переполнении отключаем инкремент */
          if ( index_wr_cmd < ( MAX_SIZE_CMD -1 ) )
          {
            index_wr_cmd++;
          }          
        }
      }  
    }  
    break;  
    
  case 2:
    /* Поиск начала параметра команды */
    /* пропуск пробелов */ 
    if ( ch == ' ' ) break;
    
    /* Обнаружение завершающих символов */
    if ( ( ch == 0x0D ) || ( ch == 0x0A ) )
    {
      /* Найдено конец команды */
      /* Установка завершающего символа */
      data_cmd[index_wr_cmd] = 0;
      /* Выполнение команды без параметра */
      /* функция обработки команд   */
      command_processing( data_cmd, NULL );   
      /* Включение режима поиск начала команды */
      fsm_dat_prc = 0;
    }
    else
    {
      if ( ch == '>' ) 
      {
        /* Найдено начало команды */
        /* Установка завершающего символа */
        data_cmd[index_wr_cmd] = 0;
        /* Выполнение команды без параметра */
        /* функция обработки команд   */
        command_processing( data_cmd, NULL );   
        /* Включение режима загрузки команды */
        fsm_dat_prc = 1;
        index_wr_cmd = 0;
      }
      else
      {  
        /* включение начала загрузки параметра команды */
        data_param_cmd[0] = ch;
        index_wr_cmd = 1;
        /* Включение режима загрузки параметра */
        fsm_dat_prc = 3;        
      }
    }   
    break;
  case 3:
    /* Загрузка мнемомики команды */
    if ( ( ch == ' ' ) || ( ch == 0x0D ) || ( ch == 0x0A ) )
    {
      /* Найдено конец параметра команды */
      /* Установка завершающего символа */
      data_param_cmd[index_wr_cmd] = 0;
      /* Выполнение команды с параметром */
      /* функция обработки команд   */
      command_processing( data_cmd, data_param_cmd );   
      /* Включение режима поиск начала команды */
      fsm_dat_prc = 0;
    }
    else
    {
      if ( ch == '>' ) 
      {
        /* Найдено начало команды */
        /* Установка завершающего символа */
        data_param_cmd[index_wr_cmd] = 0;
        /* Выполнение команды с параметром */
        /* функция обработки команд   */
        command_processing( data_cmd, data_param_cmd ); 
        /* Включение режима загрузки команды */
        fsm_dat_prc = 1;
        index_wr_cmd = 0;
      }
      else
      {  
        /* Загрузка команды */
        data_param_cmd[index_wr_cmd] = ch;
        /* При переполнении отключаем инкремент */
        if ( index_wr_cmd < ( MAX_SIZE_PARAM -1 ) )
        {
          index_wr_cmd++;
        }
      }
    }  
    break;   
  default:
    fsm_dat_prc = 0;
    break;  
  }
}

#if  (DBG_UART_ENABLE == 1)
#include "uart_dbg.h"
#endif  /* (DBG_UART_ENABLE == 1) */

/**
  * @brief  функция полинга терминала команд  
  * @param  none
  * @retval none 
  */
void terminal_cntrl( void )
{
#if  (DBG_UART_ENABLE == 1)
  while( index_wr_buf_cmd != index_rd_buf_cmd )
  {
    /* Обработка байта */
    terminal_data_processing( buf_cmd[index_rd_buf_cmd] );
    /* смещение индекса чтения */
    index_rd_buf_cmd++;
    /* Проверка на переполнение  */
    if ( index_rd_buf_cmd >= DBG_UART_MAX_SIZE_CMD_BUFF )
    {
      /* Организация циклического буфера */  
      index_rd_buf_cmd = 0;    
    }
  }
#endif  /* (DBG_UART_ENABLE == 1) */
  
}

/**
  * @brief  terminal task
  * @param  cmd_t* p_cmp - указатель на структуру обработки и описания команды
  * @retval bool false - команда добавлена успешно
  *              true - ошибка добавления команды      
  */
bool add_terminal_cmd(const cmd_t* p_cmp )
{
  if ( max_num_cmd < MAX_SIZE_NUM_CMD)
  {
    if ( p_cmp != NULL )
    {
      /* добавить команду */
      array_cmd[max_num_cmd] = (void*)p_cmp;
      /* нарастить максимум активных команд */
      max_num_cmd++;
      /* команда добавлена успешно */
      return false;
    }
  }
  max_num_cmd = MAX_SIZE_NUM_CMD;
  /* ошибка добавления команды */
  return true;
}

#include "cmd_freertos_cntrl.h"

/**
  * @brief  terminal task
  * @param  pvParameters not used
  * @retval None
  */
void terminal_task(void * pvParameters)
{
  /* Инициализация указателя на команду помощи                               */
  help_cmd_init();                                                           
  /* Инициализация указателя на команду управления сбросом микроконтроллера  */
  reset_cmd_init();                                                          
  /* Инициализация указателя на команду управления светодиодом               */
  led_cmd_init();                                 
  /* Инициализация указателя на команду просмотра параметров Freertos        */
  freertos_cmd_init(); 
  
	for (;;)
	{  
		vTaskDelay(100);
		/* функция полинга терминала команд */
		terminal_cntrl();
	}
}

/******************* (C) COPYRIGHT 2020 OneTiOne  *****END OF FILE****/
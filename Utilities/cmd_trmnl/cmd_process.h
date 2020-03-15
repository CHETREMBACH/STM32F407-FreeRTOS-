 /**
  ******************************************************************************
  * @file    cmd_process.h
  * @author  Trembach D.N.
  * @version V1.2.0
  * @date    07-10-2019
  * @brief   файл функций обработки запросов команд терминала  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2020 OneTiOne </center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CMD_PROCESS
#define __CMD_PROCESS

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "main.h"
  
#define MAX_PARAM        (5)
#define MAX_SIZE_DATA    (30)
/* Максимальное число возможных зарегистрированных команд */
#define MAX_SIZE_NUM_CMD (20)  
  
/* перечисление для автомата состояний */
typedef enum { FIND_CMD = 0, PRC_CMD, FIND_PRM, PRC_PRM } fsm_dat_prc_e;

/* перечисление для статуса команды */
typedef enum {
  STAT_NORM = 0,
  STAT_INCORRECT_PARAM,
  STAT_OVERFLOW,
  STAT_DATA_IS_EMPTY,
  STAT_CMD_NOT_LOADED,
  STAT_INCORRECT_CMD
}stat_cmd_e;

/* перечисление для типа параметра команды */
typedef enum
{ 
  VR_NONE = 0,
  VR_UINT8,
  VR_INT8,
  VR_UINT16,
  VR_INT16,	
  VR_UINT32,
  VR_INT32,
  VR_UINT64,
  VR_INT64,  
  VR_FLOAT,
  VR_DOUBLE,
  VR_CHAR,
  VR_STRING
}type_cmd_prmtr_e;

/* тип переменной/параметра для функции обработчика */
typedef struct
{
  bool flag_def;
  union
  {
    uint8_t   var_u08;
    int8_t    var_i08;
    uint16_t  var_u16;  
    int16_t   var_i16; 
    uint32_t  var_u32;
    int32_t   var_i32;
    double    var_dbl;
    char var_ch[MAX_SIZE_DATA];
  };
}cmd_parametr_t;

/* структура команды */
typedef struct
{
  char const*	    cmd_name;                                            /* имя ( мнемомика ) команды        */
  type_cmd_prmtr_e  cmd_type_parametr[MAX_PARAM];                        /* тип параметра команды */     
  char const*	    cmd_descrip;                                         /* краткое описание команды         */
  char const*	    cmd_inform;                                          /* расширенное описание команды     */	
  uint16_t          (*cmd_handler_cb)( cmd_parametr_t *parametr );       /* указатель на обработчик команд   */     
}cmd_t;

extern cmd_t* array_cmd[MAX_SIZE_NUM_CMD];

/**
  * @brief  функция полинга терминала команд  
  * @param  none
  * @retval none 
  */
void terminal_cntrl( void );

/**
  * @brief  terminal task
  * @param  pvParameters not used
  * @retval None
  */
void terminal_task(void * pvParameters);

/**
  * @brief  terminal task
  * @param  cmd_t* p_cmp - указатель на структуру обработки и описания команды
  * @retval bool false - команда добавлена успешно
  *              true - ошибка добавления команды      
  */
bool add_terminal_cmd( const cmd_t* p_cmp );

#ifdef __cplusplus
}
#endif

#endif /* __CMD_PROCESS */

/******************* (C) COPYRIGHT 2020 OneTiOne  *****END OF FILE****/


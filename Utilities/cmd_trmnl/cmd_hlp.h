 /**
  ******************************************************************************
  * @file    cmd_hlp.h
  * @author  Trembach D.N.
  * @version V1.0.0
  * @date    07-10-2019
  * @brief   файл функций обработки команд помощи терминала  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2020 OneTiOne </center></h2>
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CMD_HLP
#define __CMD_HLP

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "cmd_process.h"

/**
  * @brief  Инициализация указателя на команду помощи
  * @param  none
  * @retval none
  */
void help_cmd_init();

#ifdef __cplusplus
}
#endif

#endif /* __CMD_HLP */

/******************* (C) COPYRIGHT 2020 OneTiOne  *****END OF FILE****/


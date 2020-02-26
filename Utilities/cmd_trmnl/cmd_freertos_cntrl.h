/**
 ******************************************************************************
 * @file    cmd_freertos_cntrl.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CMD_FREERTOS_CNTRL
#define __CMD_FREERTOS_CNTRL

#ifdef __cplusplus
extern "C" {
#endif

/* Максимальное число задач в статистике */
#define MAX_NUMBER_TASK_STAT  (30)
	
/**
  * @brief  Инициализация указателя на команду просмотра параметров Freertos
  * @param  none
  * @retval none
  */
void freertos_cmd_init();

#ifdef __cplusplus
}
#endif

#endif /* __CMD_FREERTOS_CNTRL */

/******************* (C) COPYRIGHT 2020 OneTiOne  *****END OF FILE****/


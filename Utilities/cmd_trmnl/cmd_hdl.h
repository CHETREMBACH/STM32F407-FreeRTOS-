 /**
  ******************************************************************************
  * @file    cmd_hdl.h
  * @author  Trembach D.N.
  * @version V1.2.0
  * @date    07-10-2019
  * @brief   файл описания функций обработчиков команд  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2020 OneTiOne </center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __CMD_HDL
#define __CMD_HDL

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief  Инициализация указателя на команду управления сбросом микроконтроллера
  * @param  none
  * @retval none
  */
void reset_cmd_init();

/**
  * @brief  Инициализация указателя на команду управления светодиодом
  * @param  none
  * @retval none
  */
void led_cmd_init();

#ifdef __cplusplus
}
#endif

#endif /* __CMD_HDL */

/******************* (C) COPYRIGHT 2020 OneTiOne  *****END OF FILE****/


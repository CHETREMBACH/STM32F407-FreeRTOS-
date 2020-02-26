/**
  ******************************************************************************
  * @file    printf_dbg.h
  * @version V1.5.0
  * @date    25-02-2020
  * @brief   Перенаправление библиотечной C-функции printf.
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2020 OneTiOne </center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PRINTF_DBG_H
#define __PRINTF_DBG_H

#ifdef __cplusplus
extern "C" {
#endif

#if  (DBG_UART_ENABLE == 1)
#include "uart_dbg.h"
#endif  /* (DBG_UART_ENABLE == 1) */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdio.h"

/**
  * @brief  Инициализация аппаратной части отладки
  * @param  None
  * @retval None
  */
void DBG_Hardware_Setup(void);

#ifdef __cplusplus
}
#endif
	
#endif /* __PRINTF_DBG_H */

/******************* (C) COPYRIGHT 2020 OneTiOne  *****END OF FILE****/

/**
 ******************************************************************************
 * @file    w25qx_test.h
 * @version V1.0.0
 * @date    08-03-2020
 * @brief   Файл c функциями тестирования драйвера w25qx
 *
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2020 OneTiOne </center></h2>
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __W25QX_TEST_H
#define __W25QX_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "printf_dbg.h"
    
/**
 * @brief  Инициализация указателя на команды тестирования драйвера w25qx
 * @retval none
 */
void w25qx_cmd_init();

#ifdef __cplusplus
}
#endif

#endif /* __W25QX_TEST_H */
/******************* (C) COPYRIGHT 2020 OneTiOne  *****END OF FILE****/
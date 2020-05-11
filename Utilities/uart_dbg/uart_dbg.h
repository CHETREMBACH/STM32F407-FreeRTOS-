/**
  ******************************************************************************
  * @file    uart_dbg.h
  * @version V1.5.0
  * @date    25-02-2020
  * @brief   Инициализация драйвера для запуска UART в режиме отладки
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2020 OneTiOne </center></h2>
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UART_DBG_H
#define __UART_DBG_H

#ifdef __cplusplus
extern "C" {
#endif

#if  (DBG_UART_ENABLE == 1)

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "printf_dbg.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
#include "stdio.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define DBG_UART_PORT                    USART1
#define DBG_UART_CLK_ENABLE()            __HAL_RCC_USART1_CLK_ENABLE();
#define DBG_UART_RX_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define DBG_UART_TX_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE() 

#define DBG_UART_FORCE_RESET()           __HAL_RCC_USART1_FORCE_RESET()
#define DBG_UART_RELEASE_RESET()         __HAL_RCC_USART1_RELEASE_RESET()

/* Definition for DBG_UART Pins */
#define DBG_UART_TX_PIN                 GPIO_PIN_9
#define DBG_UART_TX_GPIO_PORT           GPIOA 
#define DBG_UART_TX_AF                  GPIO_AF7_USART1
#define DBG_UART_RX_PIN                 GPIO_PIN_10
#define DBG_UART_RX_GPIO_PORT           GPIOA 
#define DBG_UART_RX_AF                  GPIO_AF7_USART1

/* Definition for DBG_UART's NVIC */
#define DBG_UART_IRQn                   USART1_IRQn
#define DBG_UART_IRQHandler             USART1_IRQHandler

#define DBG_BaudRate     	            921600 //1843200 //115200 // 10800000 //460800 // 230400 //38400 //


/* Размер буфера диагностических сообщений */
#define DBG_UART_MAX_SIZE_BUFF       (8000)
/* Размер буфера приема команд */
#define DBG_UART_MAX_SIZE_CMD_BUFF   (50)

/* Буфер для диагностического сообщения */
extern volatile uint8_t buf_dbg[DBG_UART_MAX_SIZE_BUFF];
/* Индекс записи сообщения */
extern uint16_t index_wr_buf_mes;
/* Индекс чтения сообщения */
extern volatile uint16_t index_rd_buf_mes;


/* Буфер для приема команды */
extern volatile uint8_t buf_cmd[DBG_UART_MAX_SIZE_CMD_BUFF];
/* Индекс записи команды */
extern volatile uint16_t index_wr_buf_cmd;
/* Индекс чтения команды */
extern uint16_t index_rd_buf_cmd;

/**
  * @brief  Инициализация аппаратной части отладки по uart.
  * @param  None
  * @retval None
  */
void DBG_UART_Setup(void);

#endif /*  (DBG_UART_ENABLE == 1) */

#ifdef __cplusplus
}
#endif

#endif /* __UART_DBG_H */

/******************* (C) COPYRIGHT 2020 OneTiOne  *****END OF FILE****/

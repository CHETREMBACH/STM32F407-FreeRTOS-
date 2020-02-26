/**
  ******************************************************************************
  * @file    printf_dbg.c
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

/* Includes ------------------------------------------------------------------*/
#include "printf_dbg.h"

/**
  * @brief  Инициализация аппаратной части отладки
  * @param  None
  * @retval None
  */
void DBG_Hardware_Setup(void)
{
#if  (DBG_UART_ENABLE == 1)
	/* Инициализация аппаратной части отладки по uart */
	DBG_UART_Setup();
#endif  /* (DBG_UART_ENABLE == 1) */  
}

/* Разрешение прерывания передачика UART */
void uart_irq_txe_en(void);

/**
 * @brief Перенаправление библиотечной C-функции printf на USART.
 */
int _write(int file, char *data, int len)
{
	len = len;	
#if  (DBG_UART_ENABLE == 1)
	if (index_rd_buf_mes == index_wr_buf_mes)
	{
		/* Данных в буфере нет - включение передачи */
    /* Загружаем символ в буфер */    
		buf_dbg[index_wr_buf_mes] = data[0]; 
    
		/* Проверка на переполнение  */
		if (index_wr_buf_mes < (DBG_UART_MAX_SIZE_BUFF - 1))
		{
			/* Увеличение индекса */
			index_wr_buf_mes++;
		} 
		else
		{
			/* Организация циклического буфера */  
			index_wr_buf_mes = 0;    
		}    
    
		/* включаем прерывание по передаче */
		uart_irq_txe_en();
 
	}
	else
	{
		/* Есть данные загружаем данные и инкрементируем индекс */
        /* Загружаем символ в буфер */    
		buf_dbg[index_wr_buf_mes] = data[0];  
		/* Проверка на переполнение  */
		if (index_wr_buf_mes < (DBG_UART_MAX_SIZE_BUFF - 1))
		{
			/* Увеличение индекса */
			index_wr_buf_mes++;
		} 
		else
		{
			/* Организация циклического буфера */  
			index_wr_buf_mes = 0;    
		}        
	}
#endif  /* (DBG_UART_ENABLE == 1) */ 
	return 1;
}
/******************* (C) COPYRIGHT 2020 OneTiOne  *****END OF FILE****/

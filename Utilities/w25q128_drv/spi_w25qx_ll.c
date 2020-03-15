/**
  ******************************************************************************
  * @file    spi_w25qx_ll.c 
  * @author  Trembach D.N.
  * @version V1.0.0
  * @date    14-03-2020
  * @brief   Файл драйвера управления w25qx по SPI hal_ll
  *
  ******************************************************************************
  * @attention
  *
  *
  * <h2><center>&copy; COPYRIGHT 2020 DEX</center></h2>
  ******************************************************************************
 */

#include "conf_w25qx.h"
#include "spi_w25qx_ll.h" 
#include "FreeRTOS.h" 
#include "event_groups.h" 
#include "stm32f4xx_it.h"


/* Declare a variable to hold the created event group. */
EventGroupHandle_t xDMAEvent;
const EventBits_t event_dma_rx_cmlt = 0x00000010;
const EventBits_t event_dma_tx_cmlt = 0x00000020;

#if (W25QX_SPI_DMA_MODE == 1) 
// Переменная dummy для приёма/передачи 
static uint8_t    spi_dummy = 0xFF;
#endif


/**
 * @brief  Уведомление о завершении приема данных по SPI.
 * @param  None
 * @retval None
 */
void w25qx_note_dma_rx_tc(void) {
  BaseType_t xHiPrioTW = pdFALSE;
  /* Was the message posted successfully? */
  if (xEventGroupSetBitsFromISR(xDMAEvent, event_dma_rx_cmlt, &xHiPrioTW) !=
      pdFAIL) {
    portYIELD_FROM_ISR(xHiPrioTW);
  }
}
/**
 * @brief  Уведомление о завершении передачи данных по SPI.
 * @param  None
 * @retval None
 */
void w25qx_note_dma_tx_tc(void) {
  BaseType_t xHiPrioTW = pdFALSE;
  /* Was the message posted successfully? */
  if (xEventGroupSetBitsFromISR(xDMAEvent, event_dma_tx_cmlt, &xHiPrioTW) !=
      pdFAIL) {
    portYIELD_FROM_ISR(xHiPrioTW);
  }
}


/* Инициализация интерфейса w25qx */
void w25qx_interface_init(void)
{
	LL_GPIO_InitTypeDef GPIO_InitSt;
	LL_SPI_InitTypeDef 	SPI_InitSt;
	LL_DMA_InitTypeDef  DMA_InitSt;	
	
	/* GPIO init */ 
	GPIO_InitSt.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitSt.Pull = LL_GPIO_PULL_NO;
	GPIO_InitSt.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitSt.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitSt.Alternate = LL_GPIO_AF_0;

	LL_AHB1_GRP1_EnableClock(W25QX_CS_GPIO_CLK);
	GPIO_InitSt.Pin = W25QX_CS_PIN;
	LL_GPIO_Init(W25QX_CS_GPIO_PORT, &GPIO_InitSt);
	W25QX_CS_HI;
	
#if ( W25QX_RESET_ENABLE != 0 )  	
	LL_AHB1_GRP1_EnableClock(W25QX_RESET_GPIO_CLK);
	GPIO_InitSt.Pin = W25QX_RESET_PIN;
	LL_GPIO_Init(W25QX_RESET_GPIO_PORT, &GPIO_InitSt);
	W25QX_RESET_HI;		
#endif		
	
#if ( W25QX_WP_ENABLE != 0 )  	
	LL_AHB1_GRP1_EnableClock(W25QX_WP_GPIO_CLK);
	GPIO_InitSt.Pin = W25QX_WP_PIN;
	LL_GPIO_Init(W25QX_WP_GPIO_PORT, &GPIO_InitSt);
	W25QX_WP_HI;		
#endif	
  
    /* Configure SPI GPIO */
    /* SPI GPIO pin configuration  */
	GPIO_InitSt.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitSt.Pull = LL_GPIO_PULL_NO;
	GPIO_InitSt.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitSt.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitSt.Alternate = W25QX_SPI_GPIO_AF;	

	LL_AHB1_GRP1_EnableClock(W25QX_SCK_GPIO_CLK);
	GPIO_InitSt.Pin = W25QX_SCK_PIN;
	LL_GPIO_Init(W25QX_SCK_GPIO_PORT, &GPIO_InitSt);

	LL_AHB1_GRP1_EnableClock(W25QX_MOSI_GPIO_CLK);
	GPIO_InitSt.Pin = W25QX_MOSI_PIN;
	LL_GPIO_Init(W25QX_MOSI_GPIO_PORT, &GPIO_InitSt);	
	
	LL_AHB1_GRP1_EnableClock(W25QX_MISO_GPIO_CLK);
	GPIO_InitSt.Pin = W25QX_MISO_PIN;
	LL_GPIO_Init(W25QX_MISO_GPIO_PORT, &GPIO_InitSt);	
    
	/* Enable SPI clock */
	W25QX_SPI_CLK_EnableClock();
	
	LL_SPI_DeInit(W25QX_SPI_INSTANCE);
	LL_SPI_StructInit(&SPI_InitSt);
	
	SPI_InitSt.TransferDirection = LL_SPI_FULL_DUPLEX;
	SPI_InitSt.Mode              = LL_SPI_MODE_MASTER;
	SPI_InitSt.DataWidth         = LL_SPI_DATAWIDTH_8BIT;
	SPI_InitSt.ClockPolarity     = LL_SPI_POLARITY_LOW;
	SPI_InitSt.ClockPhase        = LL_SPI_PHASE_1EDGE;
	SPI_InitSt.NSS               = LL_SPI_NSS_SOFT;
	SPI_InitSt.BaudRate          = LL_SPI_BAUDRATEPRESCALER_DIV2;
	SPI_InitSt.BitOrder          = LL_SPI_MSB_FIRST;
	SPI_InitSt.CRCCalculation    = LL_SPI_CRCCALCULATION_DISABLE;
	SPI_InitSt.CRCPoly           = 7U;	
		
	LL_SPI_Init( W25QX_SPI_INSTANCE, &SPI_InitSt);	

#if (W25QX_SPI_DMA_MODE == 1)  

	/* создать обработчик события завершения передачи DMA */
	xDMAEvent = xEventGroupCreate(); 

	/* Enable DMA clock */
	W25QX_DMA_EnableClock();
	
	LL_DMA_DeInit(W25QX_DMA_INSTANCE, W25QX_DMA_TX_STREAM);
	
	LL_DMA_StructInit(&DMA_InitSt);	

	/* Set DMA_InitStruct fields  */
	DMA_InitSt.Channel                = W25QX_DMA_TX_CHANNEL;	
	DMA_InitSt.PeriphOrM2MSrcAddress  = (uint32_t)&(W25QX_SPI_INSTANCE->DR);	
	DMA_InitSt.Direction              = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
	DMA_InitSt.Mode                   = LL_DMA_MODE_NORMAL;

	DMA_InitSt.PeriphOrM2MSrcIncMode  = LL_DMA_PERIPH_NOINCREMENT;
	DMA_InitSt.MemoryOrM2MDstIncMode  = LL_DMA_MEMORY_INCREMENT;
	DMA_InitSt.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
	DMA_InitSt.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
	DMA_InitSt.Priority               = LL_DMA_PRIORITY_LOW;
	DMA_InitSt.FIFOMode               = LL_DMA_FIFOMODE_DISABLE;
	DMA_InitSt.FIFOThreshold          = LL_DMA_FIFOTHRESHOLD_1_4;
	DMA_InitSt.MemBurst               = LL_DMA_MBURST_SINGLE;
	DMA_InitSt.PeriphBurst            = LL_DMA_PBURST_SINGLE;	

	LL_DMA_Init( W25QX_DMA_INSTANCE, W25QX_DMA_TX_STREAM, &DMA_InitSt );	
		
	W25QX_DMA_TX_ClearFlag_HT(); 
	W25QX_DMA_TX_ClearFlag_TC(); 
	W25QX_DMA_TX_ClearFlag_TE(); 
	W25QX_DMA_TX_ClearFlag_DME();
	W25QX_DMA_TX_ClearFlag_FE(); 			
	
	LL_SPI_EnableDMAReq_TX(W25QX_SPI_INSTANCE);	

	LL_DMA_DeInit(W25QX_DMA_INSTANCE, W25QX_DMA_RX_STREAM);
	
	LL_DMA_StructInit(&DMA_InitSt);	

	/* Set DMA_InitStruct fields  */
	DMA_InitSt.Channel                = W25QX_DMA_RX_CHANNEL;	
	DMA_InitSt.PeriphOrM2MSrcAddress  = (uint32_t)&(W25QX_SPI_INSTANCE->DR); 	
	DMA_InitSt.Direction              = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
	DMA_InitSt.Mode                   = LL_DMA_MODE_NORMAL;
	DMA_InitSt.PeriphOrM2MSrcIncMode  = LL_DMA_PERIPH_NOINCREMENT;
	DMA_InitSt.MemoryOrM2MDstIncMode  = LL_DMA_MEMORY_INCREMENT;
	DMA_InitSt.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_BYTE;
	DMA_InitSt.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
	DMA_InitSt.Priority               = LL_DMA_PRIORITY_LOW;
	DMA_InitSt.FIFOMode               = LL_DMA_FIFOMODE_DISABLE;
	DMA_InitSt.FIFOThreshold          = LL_DMA_FIFOTHRESHOLD_1_4;
	DMA_InitSt.MemBurst               = LL_DMA_MBURST_SINGLE;
	DMA_InitSt.PeriphBurst            = LL_DMA_PBURST_SINGLE;	

	LL_DMA_Init(W25QX_DMA_INSTANCE, W25QX_DMA_RX_STREAM, &DMA_InitSt);
	
	W25QX_DMA_RX_ClearFlag_HT(); 
	W25QX_DMA_RX_ClearFlag_TC(); 
	W25QX_DMA_RX_ClearFlag_TE(); 
	W25QX_DMA_RX_ClearFlag_DME();
	W25QX_DMA_RX_ClearFlag_FE(); 	
	
	LL_SPI_EnableDMAReq_RX(W25QX_SPI_INSTANCE);	
	
#endif
	LL_SPI_Enable( W25QX_SPI_INSTANCE );
 }  

#if ( W25QX_SPI_DMA_MODE == 1)  
// *****************************************************************************
/* Чтение данных из W25QX используя DMA */
 void W25QX_SPI_SPI_ReadMulti_DMA(uint8_t* dataIn, uint16_t count) {
	 
	 W25QX_DMA_RX_ClearFlag_HT(); 
	 W25QX_DMA_RX_ClearFlag_TC(); 
	 W25QX_DMA_RX_ClearFlag_TE(); 
	 W25QX_DMA_RX_ClearFlag_DME();
	 W25QX_DMA_RX_ClearFlag_FE(); 
	 
	 W25QX_DMA_TX_ClearFlag_HT(); 
	 W25QX_DMA_TX_ClearFlag_TC(); 
	 W25QX_DMA_TX_ClearFlag_TE(); 
	 W25QX_DMA_TX_ClearFlag_DME();
	 W25QX_DMA_TX_ClearFlag_FE(); 
	 	 
	 /* Set Number of data to transfer.*/
	 LL_DMA_SetDataLength(W25QX_DMA_INSTANCE, W25QX_DMA_RX_STREAM, (uint32_t)count);	 
	 /* Set the Memory address.*/
	 LL_DMA_SetMemoryAddress(W25QX_DMA_INSTANCE, W25QX_DMA_RX_STREAM, (uint32_t)dataIn);	
	 /* Set Memory increment mode. */	 
	 LL_DMA_SetMemoryIncMode(W25QX_DMA_INSTANCE, W25QX_DMA_RX_STREAM, LL_DMA_MEMORY_INCREMENT);		 
	 
	 /* Set Number of data to transfer.*/
	 LL_DMA_SetDataLength(W25QX_DMA_INSTANCE, W25QX_DMA_TX_STREAM, (uint32_t)count);	 
	 /* Set the Memory address.*/
	 LL_DMA_SetMemoryAddress(W25QX_DMA_INSTANCE, W25QX_DMA_TX_STREAM, (uint32_t)&spi_dummy);	  
	 /* Set Memory increment mode. */
	 LL_DMA_SetMemoryIncMode(W25QX_DMA_INSTANCE, W25QX_DMA_TX_STREAM, LL_DMA_MEMORY_NOINCREMENT);	  
 
	 LL_DMA_EnableStream(W25QX_DMA_INSTANCE, W25QX_DMA_RX_STREAM);	 	 
	 LL_DMA_EnableStream(W25QX_DMA_INSTANCE, W25QX_DMA_TX_STREAM);	  	 
	 
	 
	 while (LL_DMA_IsActiveFlag_TC3(DMA1) == 0)
	 {
		 __asm volatile("nop");
		 __asm volatile("nop");
		 __asm volatile("nop"); 
	 }; 
	 		
	 LL_DMA_DisableStream(W25QX_DMA_INSTANCE, W25QX_DMA_RX_STREAM);
	 LL_DMA_DisableStream(W25QX_DMA_INSTANCE, W25QX_DMA_TX_STREAM);	
}
                                                                        
/* Записать данные в W25QX используя DMA */                
void W25QX_SPI_WriteMulti_DMA(uint8_t* dataOut, uint16_t count) {
	 
	W25QX_DMA_RX_ClearFlag_HT(); 
	W25QX_DMA_RX_ClearFlag_TC(); 
	W25QX_DMA_RX_ClearFlag_TE(); 
	W25QX_DMA_RX_ClearFlag_DME();
	W25QX_DMA_RX_ClearFlag_FE(); 
	 
	W25QX_DMA_TX_ClearFlag_HT(); 
	W25QX_DMA_TX_ClearFlag_TC(); 
	W25QX_DMA_TX_ClearFlag_TE(); 
	W25QX_DMA_TX_ClearFlag_DME();
	W25QX_DMA_TX_ClearFlag_FE(); 
	 	 
	/* Set Number of data to transfer.*/
	LL_DMA_SetDataLength(W25QX_DMA_INSTANCE, W25QX_DMA_RX_STREAM, (uint32_t)count);	 
	/* Set the Memory address.*/
	LL_DMA_SetMemoryAddress(W25QX_DMA_INSTANCE, W25QX_DMA_RX_STREAM, (uint32_t)&spi_dummy);	
	/* Set Memory increment mode. */	 
	LL_DMA_SetMemoryIncMode(W25QX_DMA_INSTANCE, W25QX_DMA_RX_STREAM, LL_DMA_MEMORY_NOINCREMENT);		 
	 
	/* Set Number of data to transfer.*/
	LL_DMA_SetDataLength(W25QX_DMA_INSTANCE, W25QX_DMA_TX_STREAM, (uint32_t)count);	 
	/* Set the Memory address.*/
	LL_DMA_SetMemoryAddress(W25QX_DMA_INSTANCE, W25QX_DMA_TX_STREAM, (uint32_t)dataOut);	  
	/* Set Memory increment mode. */
	LL_DMA_SetMemoryIncMode(W25QX_DMA_INSTANCE, W25QX_DMA_TX_STREAM, LL_DMA_MEMORY_INCREMENT);	  
 
	LL_DMA_EnableStream(W25QX_DMA_INSTANCE, W25QX_DMA_RX_STREAM);	 	 
	LL_DMA_EnableStream(W25QX_DMA_INSTANCE, W25QX_DMA_TX_STREAM);	  	 
	 
	 
	while (LL_DMA_IsActiveFlag_TC4(DMA1) == 0)
	{
		__asm volatile("nop");
		__asm volatile("nop");
		__asm volatile("nop"); 
	}
	; 
	 		
	LL_DMA_DisableStream(W25QX_DMA_INSTANCE, W25QX_DMA_RX_STREAM);
	LL_DMA_DisableStream(W25QX_DMA_INSTANCE, W25QX_DMA_TX_STREAM);	
}
#endif

void W25QX_SPI_WriteMulti( uint8_t* dataOut, uint32_t count) {
#if (W25QX_SPI_DMA_MODE == 1)
  W25QX_SPI_WriteMulti_DMA(dataOut, count);
#else
  for (uint32_t i = 0; i < count; i++) {
    W25QX_SPI_SendReceive_Data(dataOut[i]);
  }
#endif
}

void W25QX_SPI_ReadMulti( uint8_t* dataIn, uint8_t dummy, uint32_t count) {
#if (W25QX_SPI_DMA_MODE == 1)
  W25QX_SPI_SPI_ReadMulti_DMA(dataIn, count);
#else
  for (uint32_t i = 0; i < count; i++) {
    dataIn[i] = W25QX_SPI_SendReceive_Data(dummy);
  }
#endif
}
/**************    COPYRIGHT 2010 DEX Tech. Co., Ltd.   *****END OF FILE*******/
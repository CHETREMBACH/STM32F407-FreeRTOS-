/**
 * \file
 *
 * \brief W25Q128 configuration file.
 *   
 *        
 *      65536 pages of 256-bytes ( 65536*256 = 16777216B = 16384kB = 16MB )
 *            erase groups 16    (    16*256 =     4096B =     4kB        )  
 *            erase groups 128   (   128*256 =    32768B =    32kB        )   
 *            erase groups 256   (   256*256 =    65536B =    64kB        )   
 *            erase chip         ( 65536*256 = 16777216B = 16384kB = 16MB )  
 *
 * This file contains the possible external configuration of the W25Q128.
 *
 *
 */
#ifndef _CONF_W25QX_H_
#define _CONF_W25QX_H_

#include "stm32f407xx.h"
#include "stm32f4xx_ll_gpio.h"
#include "stm32f4xx_ll_spi.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_rcc.h"
#include "stm32f4xx_ll_dma.h"

#include <stdbool.h>
#include <stdio.h>

#define W25QX_SPI_DMA_MODE  (1)

 //! Номер SPI канала куда подключён W25QX
#define W25QX_SPI_INSTANCE            SPI2
#define W25QX_SPI_GPIO_AF             LL_GPIO_AF_5
#define W25QX_SPI_CLK_EnableClock()   LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI2)

#define W25QX_MOSI_PIN                LL_GPIO_PIN_15
#define W25QX_MOSI_GPIO_PORT          GPIOB                
#define W25QX_MOSI_GPIO_CLK           LL_AHB1_GRP1_PERIPH_GPIOC

#define W25QX_MISO_PIN                LL_GPIO_PIN_14
#define W25QX_MISO_GPIO_PORT          GPIOB                
#define W25QX_MISO_GPIO_CLK           LL_AHB1_GRP1_PERIPH_GPIOB

#define W25QX_SCK_PIN                 LL_GPIO_PIN_10
#define W25QX_SCK_GPIO_PORT           GPIOB
#define W25QX_SCK_GPIO_CLK            LL_AHB1_GRP1_PERIPH_GPIOC

#define W25QX_CS_PIN                  LL_GPIO_PIN_10
#define W25QX_CS_GPIO_PORT            GPIOD                
#define W25QX_CS_GPIO_CLK             LL_AHB1_GRP1_PERIPH_GPIOD
#define W25QX_CS_HI                   WRITE_REG(W25QX_CS_GPIO_PORT->BSRR, W25QX_CS_PIN)
#define W25QX_CS_LO                   WRITE_REG(W25QX_CS_GPIO_PORT->BSRR, (W25QX_CS_PIN << 16))

#define W25QX_WP_ENABLE     (1)       
#if ( W25QX_WP_ENABLE != 0 )          
  #define W25QX_WP_PIN                LL_GPIO_PIN_13          
  #define W25QX_WP_GPIO_PORT          GPIOD                
  #define W25QX_WP_GPIO_CLK           LL_AHB1_GRP1_PERIPH_GPIOD
  #define W25QX_WP_HI                 WRITE_REG(W25QX_WP_GPIO_PORT->BSRR, W25QX_WP_PIN)
  #define W25QX_WP_LO                 WRITE_REG(W25QX_WP_GPIO_PORT->BSRR, (W25QX_WP_PIN << 16))
#endif                                
                                      
#define W25QX_RESET_ENABLE     (1)    
#if ( W25QX_RESET_ENABLE != 0 )       
 #define W25QX_RESET_PIN               LL_GPIO_PIN_14           
 #define W25QX_RESET_GPIO_PORT         GPIOD                
 #define W25QX_RESET_GPIO_CLK          LL_AHB1_GRP1_PERIPH_GPIOD
 #define W25QX_RESET_HI                WRITE_REG(W25QX_RESET_GPIO_PORT->BSRR, W25QX_RESET_PIN)
 #define W25QX_RESET_LO                WRITE_REG(W25QX_RESET_GPIO_PORT->BSRR, (W25QX_RESET_PIN << 16))
#endif   


#if (W25QX_SPI_DMA_MODE == 1)
//------------------------------------------------------------------------------
//                       Константы для DMA
//------------------------------------------------------------------------------

// Адрес порта регистра данных SPI    
#define W25QX_DR_ADDRESS          LL_SPI_DMA_GetRegAddr(W25QX_SPI_INSTANCE)
                                      
// * Определение DMA для SPI RX/TX *  
#define W25QX_DMA_INSTANCE        DMA1
#define W25QX_DMA_EnableClock()   LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1)
// * Канал DMA для SPI_TX *           
#define W25QX_DMA_TX_CHANNEL      LL_DMA_CHANNEL_0
#define W25QX_DMA_TX_STREAM       LL_DMA_STREAM_4
#define W25QX_DMA_TX_IRQn         DMA1_Stream4_IRQn
//#define W25QX_SPI_DMA_TX_IRQHandler   DMA1_Stream4_IRQHandler   
#define W25QX_DMA_TX_ClearFlag_HT()     LL_DMA_ClearFlag_HT4(W25QX_DMA_INSTANCE)
#define W25QX_DMA_TX_ClearFlag_TC()     LL_DMA_ClearFlag_TC4(W25QX_DMA_INSTANCE)
#define W25QX_DMA_TX_ClearFlag_TE()     LL_DMA_ClearFlag_TE4(W25QX_DMA_INSTANCE)
#define W25QX_DMA_TX_ClearFlag_DME()    LL_DMA_ClearFlag_DME4(W25QX_DMA_INSTANCE)
#define W25QX_DMA_TX_ClearFlag_FE()     LL_DMA_ClearFlag_FE4(W25QX_DMA_INSTANCE)                              

// * Канал DMA для SPI_RX *           
#define W25QX_DMA_RX_CHANNEL      LL_DMA_CHANNEL_0
#define W25QX_DMA_RX_STREAM       LL_DMA_STREAM_3   
#define W25QX_DMA_RX_IRQn         DMA1_Stream3_IRQn
//#define W25QX_SPI_DMA_RX_IRQHandler   DMA1_Stream3_IRQHandler   
#define W25QX_DMA_RX_ClearFlag_HT()     LL_DMA_ClearFlag_HT3(W25QX_DMA_INSTANCE)
#define W25QX_DMA_RX_ClearFlag_TC()     LL_DMA_ClearFlag_TC3(W25QX_DMA_INSTANCE)
#define W25QX_DMA_RX_ClearFlag_TE()     LL_DMA_ClearFlag_TE3(W25QX_DMA_INSTANCE)
#define W25QX_DMA_RX_ClearFlag_DME()    LL_DMA_ClearFlag_DME3(W25QX_DMA_INSTANCE)
#define W25QX_DMA_RX_ClearFlag_FE()     LL_DMA_ClearFlag_FE3(W25QX_DMA_INSTANCE)   

#endif 

#endif  // _CONF_W25QX_H_

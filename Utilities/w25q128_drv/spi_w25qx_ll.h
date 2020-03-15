/**
  ******************************************************************************
  * @file    spi_w25qx_ll.h
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

#ifndef _SPI_W25QX_LL_H_
#define _SPI_W25QX_LL_H_
   
#include "conf_w25qx.h"

#define W25QXX_DUMMY_BYTE         0xFF  

/**
 * @brief  Send and Receive 8-Bits in the data
 * @param uint8_t data - send data
 * @retval uint8_t - receive data
 */
__STATIC_INLINE uint8_t W25QX_SPI_SendReceive_Data(uint8_t data) {
	/* Check TXE flag */
	while (LL_SPI_IsActiveFlag_TXE(W25QX_SPI_INSTANCE) == 0) ;
	/* Send data */
	LL_SPI_TransmitData8(W25QX_SPI_INSTANCE, data);
	/* Check RXNE flag */
	while (LL_SPI_IsActiveFlag_RXNE(W25QX_SPI_INSTANCE) == 0) ;
	/* Receive data */
	return LL_SPI_ReceiveData8(W25QX_SPI_INSTANCE);
}

/* Инициализация интерфейса w25qx */
void w25qx_interface_init(void);

/**
 * \brief Send a sequence of bytes to a DataFlash from
 *
 *
 * \param dataOut  data buffer to write
 * \param count    Length of data
 */

void W25QX_SPI_WriteMulti(uint8_t* dataOut, uint32_t count);

/**
 * \brief Receive a sequence of bytes from a DataFlash
 *
 *
 * \param dataIn   data buffer to read
 * \param dummy    data code dummy write
 * \param count    Length of data
 */
void W25QX_SPI_ReadMulti(uint8_t* dataIn, uint8_t dummy, uint32_t count);

/*! \brief Initialize SPI external resource for W25QX DataFlash driver.
 *
 */
__STATIC_INLINE void w25qx_spi_init(void) {
	w25qx_interface_init();
}

/*! \brief Select one external DataFlash component
 *
 */
__STATIC_INLINE void w25qx_spi_select_device(void) {
	W25QX_CS_LO;
}

/*! \brief Unselect one external DataFlash component
 *
 */
__STATIC_INLINE void w25qx_spi_deselect_device(void) {
	W25QX_CS_HI;
}

/*! \brief Send one byte to the DataFlash.
 *
 * \param data The data byte to send.
 * \pre The DataFlash should be selected first using w25qx_spi_select_device
 */
__STATIC_INLINE uint8_t w25qx_spi_write_byte(uint8_t data) {
	return W25QX_SPI_SendReceive_Data(data);
}

/*! \brief Get one byte (read) from the DataFlash.
 *
 * \return The received byte.
 * \pre The DataFlash should be selected first using w25qx_spi_select_device
 */
__STATIC_INLINE uint8_t w25qx_spi_read_byte(void) {
	return W25QX_SPI_SendReceive_Data(W25QXX_DUMMY_BYTE);
}

/*! \brief Send and get one byte from the DataFlash.
 *
 * \return The to send and received byte.
 * \return The received byte.
 * \pre The DataFlash should be selected first using w25qx_spi_select_device
0 */
__STATIC_INLINE uint8_t w25qx_spi_write_read_byte(uint8_t data) {
	return W25QX_SPI_SendReceive_Data(data);
}

/**
 * \brief Receive a sequence of bytes from a DataFlash
 *
 *
 * \param data   data buffer to read
 * \param len    Length of data
 * \pre The DataFlash should be selected first using w25qx_spi_select_device
 */
__STATIC_INLINE void w25qx_spi_read_packet(void const *data, size_t len) {
	W25QX_SPI_ReadMulti((uint8_t *)data, W25QXX_DUMMY_BYTE, (uint32_t)len);
}

/**
 * \brief Send a sequence of bytes to a DataFlash from
 *
 *
 * \param data   data buffer to write
 * \param len    Length of data
 * \pre The DataFlash should be selected first using w25qx_spi_select_device
 *
 */
__STATIC_INLINE void w25qx_spi_write_packet(void const *data, size_t len) {
	W25QX_SPI_WriteMulti((uint8_t *)data, (uint32_t)len);
}

#endif  /* _SPI_W25QX_LL_H_ */
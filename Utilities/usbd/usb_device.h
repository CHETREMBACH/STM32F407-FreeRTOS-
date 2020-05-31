/**
  ******************************************************************************
  * @file           : usb_device.h
  * @version        : v1.0_Cube
  * @brief          : Header for usb_device.c file.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DEVICE__H__
#define __USB_DEVICE__H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "usbd_def.h"

 /* The number of midi messages in the queue */
#define MAX_NUM_MIDI_BOX (10)
 /* Maximum midi message size */
#define MAX_SIZE_DATA_MIDI_BOX (10)

 /* Queue midi message structure */
 typedef struct{
   uint8_t length;
   uint8_t data[MAX_SIZE_DATA_MIDI_BOX];
 } midi_msg;

/**
  * @brief  usb midi thread
  * @param  None
  * @retval None
  */
 void usb_midi_thread(void *arg);

/**
  * @brief  fill tx queue
  * @param  uint8_t *msg  - point message
  * @param  uint16_t length  - size message
  * @retval None
  */
 uint16_t MIDI_DataTx(uint8_t *msg, uint16_t length);

 /**
  * @brief  rx message
  * @param  uint8_t *msg  - point message
  * @param  uint16_t length  - size message
  * @retval None
  */
 uint16_t MIDI_DataRx(uint8_t *msg, uint16_t length);

#ifdef __cplusplus
}
#endif

#endif /* __USB_DEVICE__H__ */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

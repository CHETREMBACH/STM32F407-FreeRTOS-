/**
  ******************************************************************************
  * @file           : usb_device.c
  * @version        : v1.0_Cube
  * @brief          : This file implements the USB Device
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc_midi.h"
#include "usbd_midi.h"
#include "usbd_midi_if.h"

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUsbDeviceFS;

/* Message Queue for Streaming to USB */
QueueHandle_t queue_tx_midi;

/* basic midi rx/tx functions */
uint16_t MIDI_DataRx(uint8_t *msg, uint16_t length);
uint16_t MIDI_DataTx(uint8_t *msg, uint16_t length);
USBD_MIDI_ItfTypeDef USBD_Interface_fops_FS = {MIDI_DataRx, MIDI_DataTx};

/* External variables --------------------------------------------------------*/

/**
  * Init USB device Library, add supported class and start the library
  * 
  */
void USB_device_midi_init(void) 
{
  if (USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK) {
    Error_Handler();
  }
  if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_MIDI) != USBD_OK) {
    Error_Handler();
  }
  if (USBD_MIDI_RegisterInterface(&hUsbDeviceFS, &USBD_Interface_fops_FS) !=
      USBD_OK) {
    Error_Handler();
  }
  if (USBD_Start(&hUsbDeviceFS) != USBD_OK) {
    Error_Handler();
  }
}

/**
 * @brief  fill tx queue
 * @param  uint8_t *msg  - point message
 * @param  uint16_t length  - size message
 * @retval None
 */
uint16_t MIDI_DataTx(uint8_t *msg, uint16_t length) {
  /* Temporary buffer for midi message */
  midi_msg msg_midi;

  /* queue test */
  if (queue_tx_midi == NULL) return USBD_FAIL;
  
  /* length test */
  if (length > MAX_SIZE_DATA_MIDI_BOX) {
    msg_midi.length = MAX_SIZE_DATA_MIDI_BOX;
  } else {
    msg_midi.length = length;
  }

  /* copy message */
  for (uint8_t cntik = 0; cntik < msg_midi.length; cntik++ ) 
  {
    msg_midi.data[cntik] = msg[cntik];
  }

  /* message transfer to queue */
  xQueueSend(queue_tx_midi, (void *)&msg_midi, (TickType_t)0);

  return USBD_OK;
}

/**
 * @brief  rx message
 * @param  uint8_t *msg  - point message
 * @param  uint16_t length  - size message
 * @retval None
 */
uint16_t MIDI_DataRx(uint8_t *msg, uint16_t length) { return USBD_OK; }

/**
 * @brief  usb midi thread
 * @param  None
 * @retval None
 */
void usb_midi_thread(void *arg) {
  /* Temporary buffer for streaming to USB */
  midi_msg msg_midi;

  /*  */
  queue_tx_midi = xQueueCreate(MAX_NUM_MIDI_BOX, sizeof(midi_msg));
  
  if (queue_tx_midi == NULL)
    Error_Handler();

  /* Init Device Library, add supported class and start the library. */
  USB_device_midi_init();

  for (;;) {

     /* Receive a message */ 
     xQueueReceive(queue_tx_midi, &(msg_midi), portMAX_DELAY);
     /* Transmit message */
     USBD_MIDI_Transmit(msg_midi.data, msg_midi.length);
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

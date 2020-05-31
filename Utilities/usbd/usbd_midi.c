/**
  ******************************************************************************
  * @file    usbd_midi.c
  ******************************************************************************

    (CC at)2016 by D.F.Mac. @TripArts Music

*/ 

/* Includes ------------------------------------------------------------------*/
#include "usbd_midi.h"
#include "usbd_desc_midi.h"
#include "stm32f4xx_hal_conf.h"
#include "usbd_ctlreq.h"
#include "stm32f4xx_hal.h"

static uint8_t  USBD_MIDI_Init (USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t  USBD_MIDI_DeInit (USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t  USBD_MIDI_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t  USBD_MIDI_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum);

static uint8_t  *USBD_MIDI_GetCfgDesc (uint16_t *length);

USBD_HandleTypeDef *pInstance = NULL; 

__ALIGN_BEGIN uint8_t USB_Rx_Buffer[MIDI_DATA_OUT_PACKET_SIZE] __ALIGN_END ;

/* USB MIDI interface class callbacks structure */
USBD_ClassTypeDef  USBD_MIDI = 
{
  USBD_MIDI_Init,
  USBD_MIDI_DeInit,
  NULL,
  NULL,
  NULL,
  USBD_MIDI_DataIn,
  USBD_MIDI_DataOut,
  NULL,
  NULL,
  NULL,
  NULL,// HS
  USBD_MIDI_GetCfgDesc,// FS
  NULL,// OTHER SPEED
  NULL,// DEVICE_QUALIFIER
};

/* USB MIDI device Configuration Descriptor */
__ALIGN_BEGIN uint8_t USBD_MIDI_CfgDesc[USB_MIDI_CONFIG_DESC_SIZ] __ALIGN_END =
{
  // configuration descriptor for single midi cable
  0x09, 0x02, LOBYTE(USB_MIDI_CONFIG_DESC_SIZ), HIBYTE(USB_MIDI_CONFIG_DESC_SIZ), 0x02, 0x01, 0x00, 0x80, 0x31,

  // The Audio Interface Collection
  0x09, 0x04, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, // Standard AC Interface Descriptor
  0x09, 0x24, 0x01, 0x00, 0x01, 0x09, 0x00, 0x01, 0x01, // Class-specific AC Interface Descriptor
  0x09, 0x04, 0x01, 0x00, 0x02, 0x01, 0x03, 0x00, 0x00, // MIDIStreaming Interface Descriptors
  0x07, 0x24, 0x01, 0x00, 0x01, 0x41, 0x00,             // Class-Specific MS Interface Header Descriptor

  // MIDI IN JACKS
  0x06, 0x24, 0x02, 0x01, 0x01, 0x00,
  0x06, 0x24, 0x02, 0x02, 0x02, 0x00,

  // MIDI OUT JACKS
  0x09, 0x24, 0x03, 0x01, 0x03, 0x01, 0x02, 0x01, 0x00,
  0x09, 0x24, 0x03, 0x02, 0x06, 0x01, 0x01, 0x01, 0x00,

  // OUT endpoint descriptor
  0x09, 0x05, MIDI_OUT_EP, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00,
  0x05, 0x25, 0x01, 0x01, 0x01,

  // IN endpoint descriptor
  0x09, 0x05, MIDI_IN_EP, 0x02, 0x40, 0x00, 0x00, 0x00, 0x00,
  0x05, 0x25, 0x01, 0x01, 0x03,
};

static uint8_t USBD_MIDI_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx){
  pInstance = pdev;
  USBD_LL_OpenEP(pdev,MIDI_IN_EP,USBD_EP_TYPE_BULK,MIDI_DATA_IN_PACKET_SIZE);
  USBD_LL_OpenEP(pdev,MIDI_OUT_EP,USBD_EP_TYPE_BULK,MIDI_DATA_OUT_PACKET_SIZE);
  USBD_LL_PrepareReceive(pdev,MIDI_OUT_EP,(uint8_t*)(USB_Rx_Buffer),MIDI_DATA_OUT_PACKET_SIZE);
  return 0;
}

static uint8_t USBD_MIDI_DeInit (USBD_HandleTypeDef *pdev, uint8_t cfgidx){
  pInstance = NULL;
  USBD_LL_CloseEP(pdev,MIDI_IN_EP);
  USBD_LL_CloseEP(pdev,MIDI_OUT_EP);
  return 0;
}

static uint8_t USBD_MIDI_DataIn (USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  return USBD_OK;
}

static uint8_t USBD_MIDI_DataOut (USBD_HandleTypeDef *pdev, uint8_t epnum)
{      
  uint16_t length = ((PCD_HandleTypeDef *)pdev->pData)->OUT_ep[epnum].xfer_count;

  USBD_MIDI_ItfTypeDef *pmidi = (USBD_MIDI_ItfTypeDef *)(pdev->pUserData);    
  pmidi->pIf_MidiRx((uint8_t *)&USB_Rx_Buffer, length);
  USBD_LL_PrepareReceive(pdev,MIDI_OUT_EP,(uint8_t*)(USB_Rx_Buffer),MIDI_DATA_OUT_PACKET_SIZE);
  return USBD_OK;
}

static uint8_t *USBD_MIDI_GetCfgDesc (uint16_t *length){
  *length = sizeof (USBD_MIDI_CfgDesc);
  return USBD_MIDI_CfgDesc;
}

uint8_t USBD_MIDI_RegisterInterface(USBD_HandleTypeDef *pdev, USBD_MIDI_ItfTypeDef *fops)
{
  uint8_t ret = USBD_FAIL;
  
  if(fops != NULL){
    pdev->pUserData= fops;
    ret = USBD_OK;    
  }
  return ret;
}

uint8_t USBD_MIDI_Transmit(uint8_t *msg, uint16_t length)
{
  if (pInstance != NULL) USBD_LL_Transmit(pInstance, MIDI_IN_EP, msg, length);
}


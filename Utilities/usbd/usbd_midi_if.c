/**
  ******************************************************************************
  * @file           : usbd_midi_if.c
  * @brief          :
  ******************************************************************************

    (CC at)2016 by D.F.Mac. @TripArts Music

*/

/* Includes ------------------------------------------------------------------*/
#include "usbd_midi_if.h"
#include "usb_device.h"
#include "stm32f4xx_hal.h"
#include "printf_dbg.h"

#define NEXTBYTE(idx, mask) (mask & (idx + 1))

tUsbMidiCable usbmidicable1;
tUsbMidiCable usbmidicable2;

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;

//static uint8_t buf[4];

void USBD_AddNoteOn(uint8_t cable, uint8_t ch, uint8_t note, uint8_t vel)
{
  //uint8_t cable = 0;
  uint8_t txbuf[4];
  cable <<= 4;
  txbuf[0] = cable + 0x9;
  txbuf[1] = 0x90 | ch;
  txbuf[2] = 0x7F & note;
  txbuf[3] = 0x7F & vel;
  MIDI_DataTx(txbuf, 4);
}

void USBD_AddNoteOff(uint8_t cable, uint8_t ch, uint8_t note)
{
  //uint8_t cable = 0;
  uint8_t txbuf[4];

  cable <<= 4;
  txbuf[0] = cable + 0x8;
  txbuf[1] = 0x80 | ch;
  txbuf[2] = 0x7F & note;
  txbuf[3] = 0;
  MIDI_DataTx(txbuf, 4);
}


void USBD_AddSysExMessage(uint8_t cable, uint8_t *msg, uint8_t length)
{
  //uint8_t cable = 0;
  uint8_t txbuf[4];
  int8_t bytes_remain = length;
  uint8_t i = 0;
  
  cable <<= 4;
  
  while ( bytes_remain > 0 )
  {
    if (bytes_remain > 3)
    {
      txbuf[0] = cable + 0x4; //SysEx start or continue with 3 bytes
      txbuf[1] = msg[i++];
      txbuf[2] = msg[i++];
      txbuf[3] = msg[i++];   
      bytes_remain = length - i;
    }
    else
    {
      switch ( bytes_remain ) {
      case 3:
        txbuf[0] = cable + 0x7; //SysEx end or continue with 3 bytes
        txbuf[1] = msg[i++];
        txbuf[2] = msg[i++];
        txbuf[3] = msg[i];
        break;
      case 2:
        txbuf[0] = cable + 0x6; //SysEx end or continue with 2 bytes
        txbuf[1] = msg[i++];
        txbuf[2] = msg[i];
        txbuf[3] = 0;
        break;
      case 1:
        txbuf[0] = cable + 0x5; //SysEx end or continue with 1 byte
        txbuf[1] = msg[i];
        txbuf[2] = 0;
        txbuf[3] = 0;
        break;
      }
      bytes_remain = 0;
    }
    
    MIDI_DataTx(txbuf, 4);
  }
}

//uint16_t MIDI_DataRx(uint8_t* msg, uint16_t length)
//{
//  uint16_t cnt;
//  //uint16_t chk = length % 4; //if (chk != 0) return;
//  //uint16_t *pid;
//  //uint8_t *pbuf;
//  tUsbMidiCable* pcable;
//  
//  for (cnt = 0; cnt < length; cnt += 4)
//  {
//    switch ( msg[cnt] >> 4 ) {
//    case 0:
//      pcable = &usbmidicable1;
//      break;
//    case 1:
//      pcable = &usbmidicable2;
//      break;
//    default:
//      continue;
//    };
//    
//    switch ( msg[cnt] & 0x0F ) {
//    case 0x0:
//    case 0x1:
//      continue;
//    case 0x5:
//    case 0xF:  
//      pcable->buf[ pcable->curidx ] = msg[ cnt+1 ];
//      pcable->curidx = NEXTBYTE(pcable->curidx, USBMIDIMASK);
//      break;
//    case 0x2:
//    case 0x6:
//    case 0xC:
//    case 0xD:
//      pcable->buf[ pcable->curidx ] = msg[ cnt+1 ];
//      pcable->curidx = NEXTBYTE(pcable->curidx, USBMIDIMASK);
//      pcable->buf[ pcable->curidx ] = msg[ cnt+2 ];
//      pcable->curidx = NEXTBYTE(pcable->curidx, USBMIDIMASK);
//      break;
//    default:
//      pcable->buf[ pcable->curidx ] = msg[ cnt+1 ];
//      pcable->curidx = NEXTBYTE(pcable->curidx, USBMIDIMASK);
//      pcable->buf[ pcable->curidx ] = msg[ cnt+2 ];
//      pcable->curidx = NEXTBYTE(pcable->curidx, USBMIDIMASK);
//      pcable->buf[ pcable->curidx ] = msg[ cnt+3 ];
//      pcable->curidx = NEXTBYTE(pcable->curidx, USBMIDIMASK);
//      break;
//    };     
//  };
//
//  return 0;
//}


#ifndef __DRIVER_I2C_H
#define __DRIVER_I2C_H

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"

//
////======================================================================================================
//#define ISSUE_I2C_STOP                 0
//#define NO_I2C_STOP_FOR_RESTART        1
//
//#define I2C_ACTION_ONGOING             0
//#define I2C_ACTION_COMPLETE            1
//
//#define I2C_RX_INT                     1
//#define I2C_TX_INT                     2
//
//#define I2C_COMMAND_STARTED            0
//#define I2C_FAIL_IN_USE                1
//#define I2C_FAIL_NACK                  2
//       
//typedef enum {
//    I2C_100_KHZ        = 0,
//    I2C_400_KHZ        = 1
//} i2cClkRate_t;

/*************************************************************************************************************/
/*                              PROTOTYPES                                                                   */
/*************************************************************************************************************/

/**************************************************************************************************************************************************
*               Prototypes                                                                                                                        *
**************************************************************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __CPLUSPLUS
extern "C" {
#endif

/**************************************************************************************************************************************************
*               Defines                                                                                                                           *
**************************************************************************************************************************************************/
 
#define I2C_1_BYTE     1
#define I2C_2_BYTES    2
#define I2C_3_BYTES    3
#define I2C_4_BYTES    4
#define I2C_5_BYTES    5
#define I2C_6_BYTES    6
#define I2C_7_BYTES    7
#define I2C_8_BYTES    8
#define I2C_9_BYTES    9
#define I2C_10_BYTES  10
#define I2C_11_BYTES  11
#define I2C_12_BYTES  12
#define I2C_13_BYTES  13
#define I2C_14_BYTES  14
#define I2C_15_BYTES  15 
#define I2C_16_BYTES  16

#define I2C_SUCCESSFUL 0

/********************************************************************************************************************
*  I2C Device Parameters Structure Definition                                                                       *
********************************************************************************************************************/

typedef struct 
{
    unsigned char Address_Mode;            ///< 7 bit or 10 bit address mode 
    unsigned char Clock_Rate;              ///< 100 KHz, 400 KHz 
    unsigned char Initialized;
}I2C_Device_Parameter_type;

/**************************************************************************************************/
extern unsigned char I2C_RX_Data;  
  
#define I2C_OPERATION_SUCCESSFUL  (1)
#define __delay_cycles             vTaskDelay
  
void init_I2C_Functions (void);
unsigned char tps_WriteI2CReg (unsigned int i2cAddress, unsigned char registerAddress, unsigned char writeValue);
unsigned char tps_WriteI2CMultiple (unsigned int i2cAddress, unsigned char registerAddress, unsigned char *writeValues, unsigned char numWriteBytes);
unsigned char tps_ReadI2CReg (unsigned int i2cAddress, unsigned char registerAddress, unsigned char *readValue);
unsigned char tps_ReadI2CMultiple (unsigned int i2cAddress, unsigned char registerAddress, unsigned char *readValue, unsigned char numReadBytes);
  
#ifdef __CPLUSPLUS
}
#endif

#endif // #ifndef __DRIVER_I2C_H


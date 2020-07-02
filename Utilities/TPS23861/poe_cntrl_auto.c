﻿/**
******************************************************************************
* @file    poe_cntrl_auto.c
* @author  Трембач Д.Н., Дэйта Экспресс
* @version V2.0.0
* @date    Создан:  11.06.2020
*   	    Изменен:  11.06.2020	
* @brief   Модуль работы интерфейса poe контроль
*          
******************************************************************************
* @attention
*
******************************************************************************
*/

#include "stm32f4xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "main.h"
#include "driver_i2c.h"
#include "TPS23861.h"
#include "power_manage.h"
#include "stdbool.h"


#if MODE_CNTL == 2

TPS238x_On_Off_t autoMode[NUM_OF_TPS23861] = {TPS_OFF};
uint8_t PM_setPriority[NUM_OF_TPS23861 * PM_NUM_OF_PORT] = {High, High, Normal, Normal};


TPS238X_Interrupt_Register_t intStatus;
TPS238X_Interrupt_Mask_Register_t intMask;
uint8_t intDelayTime = 0;
uint8_t sysPortNum, sysPortNum1, sysPortNum2, sysPortNum3, sysPortNum4,sysPortNum5, sysPortNum6, sysPortNum7, sysPortNum8;
TPS238x_Ports_t powerEnablePortEvents, powerGoodPortEvents, detectionPortEvents, classificationPortEvents, icutPortEvents, disconnectPortEvents, inrushPortEvents, ilimPortEvents;
TPS238X_Supply_Event_Register_t supplyEvents;
volatile uint8_t rtn;
TPS238x_Detection_Status_t detectStatus;
TPS238x_Classification_Status_t classStatus;
uint8_t powerEnable;
TPS238x_Ports_t  inactivePorts[NUM_OF_TPS23861];
uint8_t  devNum;

#if (PRINT_STATUS == 1)
uint16_t current, voltage;
uint8_t  temperature;
unsigned long outNum;
#endif

#if (DETAILED_STATUS == 1)
TPS238x_Ports_t  powerEnablePorts;
TPS238x_Ports_t  powerGoodPorts;
#endif

  uint32_t sysPower;



/*************************************************************************************************************************************************
*  TPS23861_I2C_Address
**************************************************************************************************************************************************/
/*!
* @brief Program I2C address of every TPS23861
*
* This function will program TPS23861's I2C address one by one
*
* @param[in]   deviceNumber  number of TPS23861 in the system
* @param[in]   i2cAddList[]  addresses for each TPS23861
* @param[in]   autoMode[]    AUTO bi status for each TPS23861
*
* @return  none
*
**************************************************************************************************************************************************/
void TPS23861_I2C_Address(uint8_t deviceNumber,uint8_t i2cAddList[],TPS238x_On_Off_t autoMode[])
{
  uint8_t i,oldAutoBitSetting,addressChangeNeeded,current_address,rtn;
  
  for (i=0; i < NUM_OF_TPS23861; i++)
  {
    rtn = tps_ReadI2CReg (i2cAddList[i], TPS238X_I2C_SLAVE_ADDRESS_COMMAND, &current_address);
    
    oldAutoBitSetting = (current_address & AUTO_BIT);  //save the old AUTO bit setting
    
    printf("\r\n current_address: %d\n",(unsigned long)(current_address));
    current_address &= 0x7F;
    
    if (autoMode[i] == TPS_ON)
    {
      current_address |= AUTO_BIT;
    }
    
    printf(" - new_address: %d\n",(unsigned long)(current_address));
    printf(" - i2cAddList[i]: %d\n",(unsigned long)(i2cAddList[i]));
    printf(" - rtn: %d\n",(unsigned long)(rtn));
    printf("\r\n");
    
    if ((current_address != (oldAutoBitSetting | i2cAddList[i])) || (rtn != I2C_SUCCESSFUL))
    {
      addressChangeNeeded = true;
    }
  }
  
  // If the address does not match standard, OR a NACK is received,
  //  reprogram the address(es) of the TPS23861's
  if(addressChangeNeeded == true)
  {
    tps_SetI2CAddresses (0x14, NUM_OF_TPS23861, i2cAddList, autoMode);
  }
}  
  
  
void init_poe_cntrl(void)
{
  init_I2C_Functions();
  
  vTaskDelay(180);;   //Wait TPS23861 I2C to be operational
  
  TPS23861_I2C_Address(NUM_OF_TPS23861,i2cAddList,autoMode);   //program TPS23861 I2C address
  
  //Mapping system port for each port
  sysPortNum1 = tps_RegisterPort (i2cAddList[0], TPS238X_PORT_1);
  sysPortNum2 = tps_RegisterPort (i2cAddList[0], TPS238X_PORT_2);
  sysPortNum3 = tps_RegisterPort (i2cAddList[0], TPS238X_PORT_3);
  sysPortNum4 = tps_RegisterPort (i2cAddList[0], TPS238X_PORT_4);
  
//  sysPortNum5 = tps_RegisterPort (i2cAddList[1], TPS238X_PORT_1);
//  sysPortNum6 = tps_RegisterPort (i2cAddList[1], TPS238X_PORT_2);
//  sysPortNum7 = tps_RegisterPort (i2cAddList[1], TPS238X_PORT_3);
//  sysPortNum8 = tps_RegisterPort (i2cAddList[1], TPS238X_PORT_4);
  
  // Get the interrupt status
  rtn =  tps_GetDeviceInterruptStatus (i2cAddList[0], &intStatus);
//  rtn =  tps_GetDeviceInterruptStatus (i2cAddList[1], &intStatus);
  
  // Read and Clear all Events
  rtn =  tps_GetDeviceAllInterruptEvents (i2cAddList[0], TPS_ON, &powerEnablePortEvents, &powerGoodPortEvents, &detectionPortEvents,
                                          &classificationPortEvents, &icutPortEvents, &disconnectPortEvents,
                                          &inrushPortEvents, &ilimPortEvents, &supplyEvents);
  
//  rtn =  tps_GetDeviceAllInterruptEvents (i2cAddList[1], TPS_ON, &powerEnablePortEvents, &powerGoodPortEvents, &detectionPortEvents,
//                                          &classificationPortEvents, &icutPortEvents, &disconnectPortEvents,
//                                          &inrushPortEvents, &ilimPortEvents, &supplyEvents);
  
  intMask.CLMSK_Classificiation_Cycle_Unmask = 1;
  intMask.DEMSK_Detection_Cycle_Unmask = 0;
  intMask.DIMSK_Disconnect_Unmask = 1;
  intMask.PGMSK_Power_Good_Unmask = 1;
  intMask.PEMSK_Power_Enable_Unmask = 1;
  intMask.IFMSK_IFAULT_Unmask = 1;
  intMask.INMSK_Inrush_Fault_Unmask = 1;
  intMask.SUMSK_Supply_Event_Fault_Unmask = 1;
  
  tps_SetDeviceInterruptMask (i2cAddList[0], intMask, intDelayTime);
//  tps_SetDeviceInterruptMask (i2cAddList[1], intMask, intDelayTime);
  
  tps_SetDeviceOpMode (i2cAddList[0], OPERATING_MODE_SEMI_AUTO, OPERATING_MODE_SEMI_AUTO, OPERATING_MODE_SEMI_AUTO, OPERATING_MODE_SEMI_AUTO);
//  tps_SetDeviceOpMode (i2cAddList[1], OPERATING_MODE_SEMI_AUTO, OPERATING_MODE_SEMI_AUTO, OPERATING_MODE_SEMI_AUTO, OPERATING_MODE_SEMI_AUTO);
  
  //Set two event classification if a class 4 classification occurs
  tps_SetDeviceTwoEventEnable(i2cAddList[0],TWO_EVENT_AFTER_CLASS_4,TWO_EVENT_AFTER_CLASS_4,TWO_EVENT_AFTER_CLASS_4,TWO_EVENT_AFTER_CLASS_4);
//  tps_SetDeviceTwoEventEnable(i2cAddList[1],TWO_EVENT_AFTER_CLASS_4,TWO_EVENT_AFTER_CLASS_4,TWO_EVENT_AFTER_CLASS_4,TWO_EVENT_AFTER_CLASS_4);
  
  inactivePorts[0] = TPS238X_ALL_PORTS;
  inactivePorts[1] = TPS238X_ALL_PORTS;
  
  // Power off all ports in case we are re-running this application without physically shutting down ports from previous run
  tps_SetDevicePowerOff (i2cAddList[0], inactivePorts[0]);
//  tps_SetDevicePowerOff (i2cAddList[1], inactivePorts[1]);
  
  // Set up all ports for Disconnect Enable at 7.5 mA
  tps_SetDeviceDisconnectEnable (i2cAddList[0], inactivePorts[0], DCTH_7_5_MILLIAMP, DCTH_7_5_MILLIAMP, DCTH_7_5_MILLIAMP, DCTH_7_5_MILLIAMP);
//  tps_SetDeviceDisconnectEnable (i2cAddList[1], inactivePorts[1], DCTH_7_5_MILLIAMP, DCTH_7_5_MILLIAMP, DCTH_7_5_MILLIAMP, DCTH_7_5_MILLIAMP);
  
  
  
  
  // Wait for input from user to start printing to the UART Terminal
  printf("\r\n");
  //while ((UCA0RXBUF != 'S') && (UCA0RXBUF != 's'))
  {
    printf("\r POE 23861 - Hit 'S' to start");
    vTaskDelay (60);
  }
  
  printf("\r                                           \r");
  printf("Welcome to the POE 23861 - Semi-Auto Mode for Power Management Apllication\r\n\n\n");
  
  rtn = tps_SetDeviceDetectClassEnable (i2cAddList[0], inactivePorts[0], inactivePorts[0]);   // Start Semi-Auto Detection and Classification for all ports
//  rtn = tps_SetDeviceDetectClassEnable (i2cAddList[1], inactivePorts[1], inactivePorts[1]);   // Start Semi-Auto Detection and Classification for all ports
}

void irq_poe_cntrl(void)
{  
  for (devNum = 0; devNum < NUM_OF_TPS23861; devNum++)
  {
    // Get the interrupt and event statuses. Clear the event statuses while reading
    rtn =  tps_GetDeviceInterruptStatus (i2cAddList[devNum], &intStatus);
    
    rtn =  tps_GetDeviceAllInterruptEvents (i2cAddList[devNum], TPS_ON, &powerEnablePortEvents, &powerGoodPortEvents, &detectionPortEvents,
                                            &classificationPortEvents, &icutPortEvents, &disconnectPortEvents,
                                            &inrushPortEvents, &ilimPortEvents, &supplyEvents);
    
    
    // Did we have a new Detection Event?
    if (intStatus.DETC_Detection_Cycle)
    {
      uint8_t target = detectionPortEvents;
      uint8_t i;
      
      for (i = TPS238X_PORT_1; i <= TPS238X_PORT_4; i++)
      {
        if (target & 0x1)
        {
          sysPortNum = tps_GetSystemPortNumber (i2cAddList[devNum], (TPS238x_PortNum_t)i);
          
          rtn = tps_GetPortDetectClassStatus (sysPortNum, &detectStatus, &classStatus);
          if (detectStatus == DETECT_RESIST_VALID)          // Valid
          {
            // add any message to inform the host of the detection results
          }
        }
        
        target >>= 1;
      }
      
    }
    
    // Did we have a new Classification Event?
    if (intStatus.CLASC_Classification_Cycle)
    {
      uint8_t target = classificationPortEvents;
      uint8_t i;
      
      for (i = TPS238X_PORT_1; i <= TPS238X_PORT_4; i++)
      {
        if (target & 0x1)
        {
          sysPortNum = tps_GetSystemPortNumber (i2cAddList[devNum], (TPS238x_PortNum_t)i);
          
          if((tps_GetPortPowerEnableStatus(sysPortNum) == TPS_OFF))        // Bypass second class interrupt to turn on the same port twice
          {
            rtn = tps_GetPortDetectClassStatus (sysPortNum, &detectStatus, &classStatus);
            
            printf("\r\n\n");
            printf("\nClassification Event Port ");
            printf("%d",(unsigned long)(i + (4 * devNum) -1));
            printf("\r\n\n");
            printf(" - Classification Status: ");
            printf("%d",(unsigned long)(classStatus));
            printf("\r\n");
            
            if ((classStatus != CLASS_OVERCURRENT) &&
                (classStatus != CLASS_UNKNOWN) &&
                  (classStatus != CLASS_MISMATCH))
            {
              if(PM_EN)//If power management is enabled, host will make the decision based on power and priority
              {
                PM_powerManagement(sysPortNum);
              }
              else//If If power management is disabled, turn on the port directly with correct settings of current protection
              {
                if(classStatus == CLASS_4)
                {
                  tps_SetPortPoEP(sysPortNum, _2X_ILIM_FOLDBACK_CURVE, ICUT_686_MILLIAMP);
                }
                else
                {
                  tps_SetPortPoEP(sysPortNum, _1X_ILIM_FOLDBACK_CURVE, ICUT_374_MILLIAMP);
                }

#if (NORMAL_OPERATING == 1)
                tps_SetPortPower(sysPortNum,TPS_ON);
#endif
                
#if (VMARK_WORKAROUND == 1)
                tps_SetPortOpMode(sysPortNum,OPERATING_MODE_AUTO);
#endif
              }
            }
            
          }
        }
        target >>= 1;
      }
    }
    
    // Did we have a new Disconnection Event?
    if (intStatus.DISF_Disconnect_Event)
    {
      uint8_t target = disconnectPortEvents;
      
      // Set all of the disconnected ports as inactive
      *(uint8_t *)&inactivePorts[devNum] |= target;
      
    }
    
    // Did we have a new Power Enable Event?
    if (intStatus.PEC_Power_Enable_Change)
    {
      uint8_t target = powerEnablePortEvents;
      uint8_t i;
      
      for (i = TPS238X_PORT_1; i <= TPS238X_PORT_4; i++)
      {
        if (target & 0x1)
        {
          sysPortNum = tps_GetSystemPortNumber (i2cAddList[devNum], (TPS238x_PortNum_t)i);
          
          powerEnable = tps_GetPortPowerEnableStatus (sysPortNum);
          
          if (powerEnable == TPS_ON)          // Valid
          {
#if (VMARK_WORKAROUND == 1)
            tps_SetPortOpMode(sysPortNum,OPERATING_MODE_SEMI_AUTO);
#endif
          }
          
        }
        
        target >>= 1;
      }
      
    }
    
    // Did we have a new Power Good Event?
    if (intStatus.PGC_Power_Good_Change)
    {
      uint8_t target = powerGoodPortEvents;
      uint8_t i;
      
      for (i = TPS238X_PORT_1; i <= TPS238X_PORT_4; i++)
      {
        if (target & 0x1)
        {
          sysPortNum = tps_GetSystemPortNumber (i2cAddList[devNum], (TPS238x_PortNum_t)i);
          
        }
        
        target >>= 1;
      }
      
    }
    
    
    // Fault condition
    if(intStatus.SUPF_Supply_Event_Fault)
    {
      uint8_t *temp = (uint8_t *)&supplyEvents;
      uint8_t target = *temp;
      
      if(target & 0x10)
      {
        printf("VPWR undervlotage occurred");
        printf("\r\n");
      }
      else if(target & 0x20)
      {
        printf("VDD undervlotage occurred");
        printf("\r\n");
      }
      
      else if(target & 0x80)
      {
        printf("Thermal shutdown occurred");
        printf("\r\n");
      }
      
    }
    
    if(intStatus.INRF_Inrush_Fault)
    {
      uint8_t target = inrushPortEvents;
      uint8_t i;
      
      *(uint8_t *)&inactivePorts[devNum] |= target;
      
      for (i = TPS238X_PORT_1; i <= TPS238X_PORT_4; i++)
      {
        if (target & 0x1)
        {
          printf("Start Fault Port");
          printf("%d",(unsigned long)(i + (4 * devNum) - 1));
          printf("\r\n");
        }
        target >>= 1;
      }
    }
    
    if(intStatus.IFAULT_ICUT_ILIM_Fault)
    {
      uint8_t targetIcut = icutPortEvents;
      uint8_t targetIlim = ilimPortEvents;
      uint8_t i;
      
      *(uint8_t *)&inactivePorts[devNum] |= targetIcut;
      *(uint8_t *)&inactivePorts[devNum] |= targetIlim;
      
      for (i = TPS238X_PORT_1; i <= TPS238X_PORT_4; i++)
      {
        if (targetIcut & 0x01)
        {
          printf("ICUT Fault Port");
          printf("%d",(unsigned long)(i + (4 * devNum) - 1));
          printf("\r\n");
        }
        
        if(targetIlim & 0x01)
        {
          printf("ILIM Fault Port");
          printf("%d",(unsigned long)(i + (4 * devNum) - 1));
          printf("\r\n");
        }
        
        targetIcut >>= 1;
        targetIlim >>= 1;
        
      }
    }
  }   
}  

void period_poe_cntrl(void)
{
  sysPower = PM_calSysPower();
  printf("\r\n\n");
  printf("\n System Power ");
  printf("%d",(unsigned long)(sysPower));
  printf("mW \r\n");
  
  tps_GetDeviceInputVoltage (i2cAddList[0], &voltage);
  printf("Input Voltage: ");
  outNum = ((unsigned long)voltage * 3662) / 1000;
  printf("%d",outNum);
  printf("mV \r\n");
  
  tps_GetDeviceTemperature (i2cAddList[0], &temperature);
  printf("Device Temperature: ");
  outNum = CONVERT_TEMP((unsigned long)temperature);
  printf("%d",outNum);
  printf(" degrees C\r\n\n");
  
  if (!(inactivePorts[0] & PORT_1_VALUE))
  {
    tps_GetPortMeasurements (sysPortNum1, &voltage, &current);
    printf("Port 1 Voltage: ");
    outNum = ((unsigned long)voltage * 3662) / 1000;
    printf("%d",outNum);
    printf("mV      Current: ");
    outNum = ((unsigned long)current * 61039) / 1000000;
    printf("%d",outNum);
    printf("mA \r\n");
  }
  
  if (!(inactivePorts[0] & PORT_2_VALUE))
  {
    tps_GetPortMeasurements (sysPortNum2, &voltage, &current);
    printf("Port 2 Voltage: ");
    outNum = ((unsigned long)voltage * 3662) / 1000;
    printf("%d",outNum);
    printf("mV      Current: ");
    outNum = ((unsigned long)current * 61039) / 1000000;
    printf("%d",outNum);
    printf("mA \r\n");
  }
  
  if (!(inactivePorts[0] & PORT_3_VALUE))
  {
    tps_GetPortMeasurements (sysPortNum3, &voltage, &current);
    printf("Port 3 Voltage: ");
    outNum = ((unsigned long)voltage * 3662) / 1000;
    printf("%d",outNum);
    printf("mV      Current: ");
    outNum = ((unsigned long)current * 61039) / 1000000;
    printf("%d",outNum);
    printf("mA \r\n");
  }
  
  if (!(inactivePorts[0] & PORT_4_VALUE))
  {
    tps_GetPortMeasurements (sysPortNum4, &voltage, &current);
    printf("Port 4 Voltage: ");
    outNum = ((unsigned long)voltage * 3662) / 1000;
    printf("%d",outNum);
    printf("mV      Current: ");
    outNum = ((unsigned long)current * 61039) / 1000000;
    printf("%d",outNum);
    printf("mA \r\n\n");
  }
  
  if (!(inactivePorts[1] & PORT_1_VALUE))
  {
    tps_GetPortMeasurements (sysPortNum5, &voltage, &current);
    printf("Port 5 Voltage: ");
    outNum = ((unsigned long)voltage * 3662) / 1000;
    printf("%d",outNum);
    printf("mV      Current: ");
    outNum = ((unsigned long)current * 61039) / 1000000;
    printf("%d",outNum);
    printf("mA \r\n");
  }
  
  if (!(inactivePorts[1] & PORT_2_VALUE))
  {
    tps_GetPortMeasurements (sysPortNum6, &voltage, &current);
    printf("Port 6 Voltage: ");
    outNum = ((unsigned long)voltage * 3662) / 1000;
    printf("%d",outNum);
    printf("mV      Current: ");
    outNum = ((unsigned long)current * 61039) / 1000000;
    printf("%d",outNum);
    printf("mA \r\n");
  }
  
  if (!(inactivePorts[1] & PORT_3_VALUE))
  {
    tps_GetPortMeasurements (sysPortNum7, &voltage, &current);
    printf("Port 7 Voltage: ");
    outNum = ((unsigned long)voltage * 3662) / 1000;
    printf("%d",outNum);
    printf("mV      Current: ");
    outNum = ((unsigned long)current * 61039) / 1000000;
    printf("%d",outNum);
    printf("mA \r\n");
  }
  
  if (!(inactivePorts[1] & PORT_4_VALUE))
  {
    tps_GetPortMeasurements (sysPortNum8, &voltage, &current);
    printf("Port 8 Voltage: ");
    outNum = ((unsigned long)voltage * 3662) / 1000;
    printf("%d",outNum);
    printf("mV      Current: ");
    outNum = ((unsigned long)current * 61039) / 1000000;
    printf("%d",outNum);
    printf("mA \r\n\n");
  }
  
  
#if (DETAILED_STATUS == 1)
  for (devNum = 0; devNum < NUM_OF_TPS23861; devNum++)
  {
    // read current value of all event registers (Do not clear)
    rtn =  tps_GetDeviceAllInterruptEvents (i2cAddList[devNum], TPS_OFF, &powerEnablePortEvents, &powerGoodPortEvents, &detectionPortEvents,
                                            &classificationPortEvents, &icutPortEvents, &disconnectPortEvents,
                                            &inrushPortEvents, &ilimPortEvents, &supplyEvents);
    
    printf("\n---- Event Registers -----Dev : ");
    printf("%X",devNum);
    printf("----- \r\n0x");
    printf("%X",(powerGoodPortEvents<<4) | powerEnablePortEvents);
    printf("   0x");
    printf("%X",(classificationPortEvents << 4) | detectionPortEvents);
    printf("   0x");
    printf("%X",(disconnectPortEvents << 4) | icutPortEvents);
    printf("   0x");
    printf("%X",(ilimPortEvents << 4) | inrushPortEvents);
    printf("   0x");
    printf("%X",(*(unsigned char*)&supplyEvents << 4));
    printf("\r\n\n");
    
    printf("---- Port Status -----\r\n0x");
    sysPortNum = tps_GetSystemPortNumber (i2cAddList[devNum], TPS238X_PORT_1);
    rtn = tps_GetPortDetectClassStatus (sysPortNum, &detectStatus, &classStatus);
    printf("%X",(classStatus<<4) | detectStatus);
    printf("   0x");
    sysPortNum = tps_GetSystemPortNumber (i2cAddList[devNum], TPS238X_PORT_2);
    rtn = tps_GetPortDetectClassStatus (sysPortNum, &detectStatus, &classStatus);
    printf("%X",(classStatus<<4) | detectStatus);
    printf("   0x");
    sysPortNum = tps_GetSystemPortNumber (i2cAddList[devNum], TPS238X_PORT_3);
    rtn = tps_GetPortDetectClassStatus (sysPortNum, &detectStatus, &classStatus);
    printf("%X",(classStatus<<4) | detectStatus);
    printf("   0x");
    sysPortNum = tps_GetSystemPortNumber (i2cAddList[devNum], TPS238X_PORT_4);
    rtn = tps_GetPortDetectClassStatus (sysPortNum, &detectStatus, &classStatus);
    printf("%X",(classStatus<<4) | detectStatus);
    printf("\r\n\n");
    printf("---- Power Status -----\r\n0x");
    rtn = tps_GetDevicePowerStatus (i2cAddList[devNum], &powerEnablePorts, &powerGoodPorts);
    printf("%X",(powerGoodPorts<<4) | powerEnablePorts);
    printf("\r\n\n");
    
    
    printf("------------------------------------------\r\n");
  }
#endif  
}

#endif /* MODE_CNTRL */ 
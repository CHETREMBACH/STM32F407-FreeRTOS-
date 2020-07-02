/*************************************************************************************************************************************************/
/*!     \file TPS23861.h   
*
*       \brief Register File containing hardware definitions and other configurations by the TPS23861 software API functions
*                                                                                                                                                 
*       \date January 2013                                                                                                          
*
*       This file contains the software structures, defines, and prototypes for the TPS23861.c API functions
*/                                                                                                                                                 
/**************************************************************************************************************************************************
*       Copyright © 2013-2016 Texas Instruments Incorporated - http://www.ti.com/                                                                      *
***************************************************************************************************************************************************
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*                                                                                                                                                 *
**************************************************************************************************************************************************/
#ifndef __TPS23861_H_
#define __TPS23861_H_

#include "main.h"
#include "printf_dbg.h"

#define TPS_SUCCESSFUL                         0x00
#define TPS_ERR_PORT_IN_USE                    0x80
#define TPS_ERR_PORT_NOT_IN_USE                0x81
#define TPS_ERR_NO_PORT_AVAILABLE              0x82
#define TPS_ERR_I2C_ERROR                      0x83
#define TPS_ERR_I2C_ADDRESS_MISMATCH           0x40
#define TPS_ERR_I2C_AUTOBIT_MISMATCH           0x41


#define TPS_GIVE_ME_NEXT_PORT_AVAILABLE        0xff
#define TPS_PORT_NOT_REGISTERED_VALUE          0xff

#define SWIZZLE_BYTES(x)                       {unsigned short y; y = x; x = (((y & 0xff) << 8) | (y >> 8)); }     ///< Used to switch MSB and LSB bytes in an unsigned short variable

#define TPS_MAX_SYSTEM_PORTS                   32

#define TPS_VOLTAGE_SCALE_FACTOR               3662
#define TPS_CURRENT_SCALE_FACTOR               61039

typedef enum {
  TPS_OFF                                    = 0x0,   
  TPS_ON                                     = 0x1
} TPS238x_On_Off_t;

///Used with functions that indicate a single port. 
/// @note that the hardware registers use a bit value with a separate bit for each port number. Conversion can be done with the CONVERT_PORT_NUM () macro
typedef enum {
  TPS238X_PORT_1                             = 1,       
  TPS238X_PORT_2                             = 2,
  TPS238X_PORT_3                             = 3,
  TPS238X_PORT_4                             = 4
} TPS238x_PortNum_t;

/// This structure is used with many functions where multiple ports can be indicated at the same time
#define PORT_1_VALUE                           0x1            ///< Bit value of the TPS238x_Ports_t Port 1
#define PORT_2_VALUE                           0x2            ///< Bit value of the TPS238x_Ports_t Port 2
#define PORT_3_VALUE                           0x4            ///< Bit value of the TPS238x_Ports_t Port 3
#define PORT_4_VALUE                           0x8            ///< Bit value of the TPS238x_Ports_t Port 4

typedef unsigned char TPS238x_Ports_t;

#define TPS238X_ALL_PORTS                    ((TPS238x_Ports_t)0xf)

typedef struct {
    unsigned int      i2cAddress;
    TPS238x_PortNum_t devicePortNum;
} TPS238x_System_Port_Map_t;


/// Interrupt Register - used in tps_GetInterruptStatus() to return events <br>
///   Hardware Command 00h; 1 data byte; read only                         <br><br>
/// Provides the faults and events that are currently active. 
/// @note The interrupt status flag may be active for events or faults that are masked. No interrupt will be generated, but the status
///        may be present along side of another event which is allowed to generate an interrupt.  
typedef struct {
	unsigned char PEC_Power_Enable_Change                    : 1;   ///< Indicates a power enable status change occurred on at least one port
	unsigned char PGC_Power_Good_Change                      : 1;   ///< Indicates a power good change occurred on at least one port
	unsigned char DISF_Disconnect_Event                      : 1;   ///< Indicates a disconnect event occurred on at least one port
	unsigned char DETC_Detection_Cycle                       : 1;   ///< Indicates at least one detection cycle occurred on at least one port
	unsigned char CLASC_Classification_Cycle                 : 1;   ///< Indicates at least one classification cycle occurred on at least one port
	unsigned char IFAULT_ICUT_ILIM_Fault                     : 1;   ///< Indicates that an ICUT or ILIM fault occurred on at least one port
	unsigned char INRF_Inrush_Fault                          : 1;   ///< Indicates that an Inrush fault occurred on at least one port
	unsigned char SUPF_Supply_Event_Fault                    : 1;   ///< Indicates that a supply event fault occurred
}TPS238X_Interrupt_Register_t;

#define PEC                                    0x01   ///< Indicates a power enable status change occurred on at least one port
#define PGC                                    0x02   ///< Indicates a power good change occurred on at least one port
#define DISF                                   0x04   ///< Indicates a disconnect event occurred on at least one port
#define DETC                                   0x08   ///< Indicates at least one detection cycle occurred on at least one port
#define CLASC                                  0x10   ///< Indicates at least one classification cycle occurred on at least one port
#define IFAULT                                 0x20   ///< Indicates that an ICUT or ILIM fault occurred on at least one port
#define INRF                                   0x40   ///< Indicates that an Inrush fault occurred on at least one port
#define SUPF                                   0x80   ///< Indicates that a supply event fault occurred

/// Interrupt Mask Register - Used in tps_GetInterruptMask() and tps_SetInterruptMask() <br>
///                Hardware - Command 01h; 1 data byte; read/write                      <br><br>
/// Enables the various events and faults to generate interrupts
/// @note that writing a 0 masks the events. Writing a 1 to the bit unmasks the interrupt
typedef struct {
	unsigned char PEMSK_Power_Enable_Unmask                  : 1;   ///< Enable power enable interrupts
	unsigned char PGMSK_Power_Good_Unmask                    : 1;   ///< Enable power good interrupts
	unsigned char DIMSK_Disconnect_Unmask                    : 1;   ///< Enable disconnect event interrupts
	unsigned char DEMSK_Detection_Cycle_Unmask               : 1;   ///< Enable detection cycle event interrupts
	unsigned char CLMSK_Classificiation_Cycle_Unmask         : 1;   ///< Enable classification cycle event interrupts
	unsigned char IFMSK_IFAULT_Unmask                        : 1;   ///< Enable ICUT or OLIM fault interrupts
	unsigned char INMSK_Inrush_Fault_Unmask                  : 1;   ///< Enable Inrush fault interrupts
	unsigned char SUMSK_Supply_Event_Fault_Unmask            : 1;   ///< Enable supply event fault interrupts
}TPS238X_Interrupt_Mask_Register_t;

#define PEMSK                                  0x01   ///< Enable power enable interrupts
#define PGMSK                                  0x02   ///< Enable power good interrupts
#define DIMSK                                  0x04   ///< Enable disconnect event interrupts
#define DEMSK                                  0x08   ///< Enable detection cycle event interrupts
#define CLMSK                                  0x10   ///< Enable classification cycle event interrupts
#define IFMSK                                  0x20   ///< Enable ICUT or OLIM fault interrupts
#define INMSK                                  0x40   ///< Enable Inrush fault interrupts
#define SUMSK                                  0x80   ///< Enable supply event fault interrupts

/// Power Event Register - Software API uses TPS238x_Ports_t; Software retrieves these events as part of tps_GetAllInterruptEvents() <br>    
///             Hardware - Command 02h; 1 data byte; read only     <br>
///                      - Command 03h; 1 data byte; Clear on read <br><br>
/// Changes to the power enable or power good status occurred for at least one port
typedef struct {
	unsigned char PEC1_Power_Enable_Port_1_Event             : 1;   ///< Change to power enable status for port 1
	unsigned char PEC2_Power_Enable_Port_2_Event             : 1;   ///< Change to power enable status for port 2
	unsigned char PEC3_Power_Enable_Port_3_Event             : 1;   ///< Change to power enable status for port 3
	unsigned char PEC4_Power_Enable_Port_4_Event             : 1;   ///< Change to power enable status for port 4
	unsigned char PGC1_Power_Good_Port_1_Event               : 1;   ///< Change to power good status for port 1
	unsigned char PGC2_Power_Good_Port_2_Event               : 1;   ///< Change to power good status for port 2
	unsigned char PGC3_Power_Good_Port_3_Event               : 1;   ///< Change to power good status for port 3
	unsigned char PGC4_Power_Good_Port_4_Event               : 1;   ///< Change to power good status for port 4
}TPS238X_Power_Enable_Register_t;

#define PEC1                                   0x01   ///< Change to power enable status for port 1
#define PEC2                                   0x02   ///< Change to power enable status for port 2
#define PEC3                                   0x04   ///< Change to power enable status for port 3
#define PEC4                                   0x08   ///< Change to power enable status for port 4
#define PGC1                                   0x10   ///< Change to power good status for port 1
#define PGC2                                   0x20   ///< Change to power good status for port 2
#define PGC3                                   0x40   ///< Change to power good status for port 3
#define PGC4                                   0x80   ///< Change to power good status for port 4
                                 
#define POWER_GOOD_EVENT_SHIFT                 0
#define POWER_ENABLE_EVENT_SHIFT               4

/// Detection Event Register - Software API uses TPS238x_Ports_t; Software retrieves these events as part of tps_GetAllInterruptEvents() <br>   
///                 Hardware - Command 04h; 1 data byte; read only      <br>
///                          - Command 05h; 1 data byte; Clear on read  <br><br>
/// Detection and/or classification cycles occurred on at least one port
typedef struct {
	unsigned char DETC1_Detection_Cycle_Port_1_Event         : 1;   ///< Detection cycle occurred on port 1
	unsigned char DETC2_Detection_Cycle_Port_2_Event         : 1;   ///< Detection cycle occurred on port 2
	unsigned char DETC3_Detection_Cycle_Port_3_Event         : 1;   ///< Detection cycle occurred on port 3
	unsigned char DETC4_Detection_Cycle_Port_4_Event         : 1;   ///< Detection cycle occurred on port 4
	unsigned char CLSC1_Classification_Cycle_Port_1_Event    : 1;   ///< Classification cycle occurred on port 1
	unsigned char CLSC2_Classification_Cycle_Port_2_Event    : 1;   ///< Classification cycle occurred on port 2
	unsigned char CLSC3_Classification_Cycle_Port_3_Event    : 1;   ///< Classification cycle occurred on port 3
	unsigned char CLSC4_Classification_Cycle_Port_4_Event    : 1;   ///< Classification cycle occurred on port 4
}TPS238X_Detection_Event_Register_t;

#define DETC1                                  0x01   ///< Detection cycle occurred on port 1
#define DETC2                                  0x02   ///< Detection cycle occurred on port 2
#define DETC3                                  0x04   ///< Detection cycle occurred on port 3
#define DETC4                                  0x08   ///< Detection cycle occurred on port 4
#define CLSC1                                  0x10   ///< Classification cycle occurred on port 1
#define CLSC2                                  0x20   ///< Classification cycle occurred on port 2
#define CLSC3                                  0x40   ///< Classification cycle occurred on port 3
#define CLSC4                                  0x80   ///< Classification cycle occurred on port 4
              
#define DETECTION_EVENT_SHIFT                  0
#define CLASSIFICATION_EVENT_SHIFT             4

/// Fault Event Register - Software API uses TPS238x_Ports_t; Software retrieves these events as part of tps_GetAllInterruptEvents() <br>  
///             Hardware - Command 06h; 1 data byte; read only        <br>
///                      - Command 07h; 1 data byte; Clear on read    <br><br>
/// ICUT fault and disconnect events occurred for at least one port
typedef struct {
	unsigned char ICUT1_ICUT_Fault_Port_1_Event              : 1;    ///< ICUT fault occurred at port 1
	unsigned char ICUT2_ICUT_Fault_Port_2_Event              : 1;    ///< ICUT fault occurred at port 2
	unsigned char ICUT3_ICUT_Fault_Port_3_Event              : 1;    ///< ICUT fault occurred at port 3
	unsigned char ICUT4_ICUT_Fault_Port_4_Event              : 1;    ///< ICUT fault occurred at port 4
	unsigned char DISF1_Disconnect_Port_1_Event              : 1;    ///< Disconnect event occurred at port 1
	unsigned char DISF2_Disconnect_Port_2_Event              : 1;    ///< Disconnect event occurred at port 2
	unsigned char DISF3_Disconnect_Port_3_Event              : 1;    ///< Disconnect event occurred at port 3
	unsigned char DISF4_Disconnect_Port_4_Event              : 1;    ///< Disconnect event occurred at port 4
}TPS238X_Fault_Event_Register_t;

#define ICUT1                                  0x01    ///< ICUT fault occurred at port 1
#define ICUT2                                  0x02    ///< ICUT fault occurred at port 2
#define ICUT3                                  0x04    ///< ICUT fault occurred at port 3
#define ICUT4                                  0x08    ///< ICUT fault occurred at port 4
#define DISF1                                  0x10    ///< Disconnect event occurred at port 1
#define DISF2                                  0x20    ///< Disconnect event occurred at port 2
#define DISF3                                  0x40    ///< Disconnect event occurred at port 3
#define DISF4                                  0x80    ///< Disconnect event occurred at port 4

#define ICUT_EVENT_SHIFT                       0
#define DISCONNECT_EVENT_SHIFT                 4

/// Inrush/ILIM Event Register - Software API uses TPS238x_Ports_t; Software retrieves these events as part of tps_GetAllInterruptEvents() <br> 
///                   Hardware - Command 08h; 1 data byte; read only      <br>
///                            - Command 09h; 1 data byte; Clear on read  <br><br>
/// Inrush and ILIM fault events have occurred for at least one port
typedef struct {
	unsigned char INR1_Inrush_Fault_Port_1_Event             : 1;    ///< Inrush fault occurred at port 1
	unsigned char INR2_Inrush_Fault_Port_2_Event             : 1;    ///< Inrush fault occurred at port 2
	unsigned char INR3_Inrush_Fault_Port_3_Event             : 1;    ///< Inrush fault occurred at port 3
	unsigned char INR4_Inrush_Fault_Port_4_Event             : 1;    ///< Inrush fault occurred at port 4
	unsigned char ILIM1_Limit_Output_Current_Port_1_Event    : 1;    ///< ILIM fault occurred at port 1
	unsigned char ILIM2_Limit_Output_Current_Port_2_Event    : 1;    ///< ILIM fault occurred at port 2
	unsigned char ILIM3_Limit_Output_Current_Port_3_Event    : 1;    ///< ILIM fault occurred at port 3
	unsigned char ILIM4_Limit_Output_Current_Port_4_Event    : 1;    ///< ILIM fault occurred at port 4
}TPS238X_Inrush_ILIM_Event_Register_t;

#define INR1                                   0x01    ///< Inrush fault occurred at port 1
#define INR2                                   0x02    ///< Inrush fault occurred at port 2
#define INR3                                   0x04    ///< Inrush fault occurred at port 3
#define INR4                                   0x08    ///< Inrush fault occurred at port 4
#define ILIM1                                  0x10    ///< ILIM fault occurred at port 1
#define ILIM2                                  0x20    ///< ILIM fault occurred at port 2
#define ILIM3                                  0x40    ///< ILIM fault occurred at port 3
#define ILIM4                                  0x80    ///< ILIM fault occurred at port 4

#define INRUSH_EVENT_SHIFT                     0
#define ILIM_EVENT_SHIFT                       4


/// Supply Event Register - Software retrieves these events as part of tps_GetAllInterruptEvents() <br>
///              Hardware - Command 0Ah; 1 data byte; read only      <br>
///                       - Command 0Bh; 1 data byte; Clear on read  <br><br>
/// Identifies events with the system power supply
typedef struct {
	unsigned char Reserved_1                                 : 4;
	unsigned char VPUV_VPower_Undervoltage_Event             : 1;   ///< VPWR Undervoltage
	unsigned char VDUV_Vdd_UVLO_Event                        : 1;   ///< VDD UVLO Occurred. Power on reset happened
	unsigned char Reserved_2                                 : 1;
	unsigned char TSD_Thermal_Shutdown_Event                 : 1;   ///< Thermal shutdown occurred
}TPS238X_Supply_Event_Register_t;

#define VPUV                                   0x10
#define VDUV                                   0x20
#define TSD                                    0x80

/// Port Status Register - Results returned in tps_GetPortDetectClassStatus(), tps_GetPortDetectionStatus(), and tps_GetPortClassificationtionStatus() <br> 
///             Hardware - Command 0Ch; Port 1; 1 data byte; read only  <br>
///                      - Command 0Dh; Port 2; 1 data byte; read only  <br>
///                      - Command 0Eh; Port 3; 1 data byte; read only  <br>
///                      - Command 0Fh; Port 4; 1 data byte; read only  <br><br>
/// Uses TPS238x_Classification_Status_t and TPS238x_Detection_Status_t to provide the most recent classification and detection results for the port
typedef struct {
    unsigned char Detect                                     : 4;
    unsigned char Class                                      : 4;
}TPS238X_Port_Status_Register_t;

/// The classification status
typedef enum {
  CLASS_UNKNOWN                              = 0x0,   ///< Unknown - invalid
  CLASS_1                                    = 0x1,   ///< Class 1
  CLASS_2                                    = 0x2,   ///< Class 2
  CLASS_3                                    = 0x3,   ///< Class 3
  CLASS_4                                    = 0x4,   ///< Class 4
  CLASS_0                                    = 0x6,   ///< Class 0
  CLASS_OVERCURRENT                          = 0x7,   ///< Overcurrent - invalid
  CLASS_MISMATCH                             = 0x8,   ///< Class mismatch - invalid
  CLASS_5                                    = 0x9    ///< Class 5
} TPS238x_Classification_Status_t;

/// Detection status
typedef enum {
  DETECT_UNKNOWN                             = 0x0,   ///< Unknown - invalid
  DETECT_SHORT_CIRCUIT                       = 0x1,   ///< Short circuit (<1.8 kOhm) - invalid
  DETECT_RESIST_LOW                          = 0x3,   ///< Resistance too low - invalid
  DETECT_RESIST_VALID                        = 0x4,   ///< Resistance valid
  DETECT_RESIST_HIGH                         = 0x5,   ///< Resistance too hig - invalid
  DETECT_OPEN_CIRCUIT                        = 0x6,   ///< Open circuit - invalid
  DETECT_MOSFET_FAULT                        = 0x8,   ///< MOSFET Fault - invalid
  DETECT_LEGACY                              = 0x9,   ///< Legacy detect - valid legacy detection
  DETECT_CAP_INVALID_CLAMP_VOLTAGE           = 0xA,   ///< Detect measurement beyond clamp voltage - invalid legacy detection
  DETECT_CAP_INVALID_DELTA_V                 = 0xB,   ///< Insufficient Delta V - invalid legacy detection
  DETECT_CAP_INVALID_LEGACY_RANGE            = 0xC    ///< Capacitance measurement outside range of legacy device - invalid legacy detection
} TPS238x_Detection_Status_t;


#define DETECT                                 0x0F
#define CLASS                                  0xF0
#define CLASS_SHIFT                            4
#define DETECT_SHIFT                           0
#define GET_DETECT(x)                          (x & DETECT)
#define GET_CLASS(x)                           (x >> CLASS_SHIFT)


/// Power Status Register - Software API uses TPS238x_Ports_t; Status returned in the 
///                            tps_GetPowerStatus(), tps_GetPowerEnableStatus(), and tps_GetPowerGoodStatus () <br>   
///              Hardware - Command 10h; 1 data byte; read only   <br><br>
/// Provides status for each port about the power enable and power good settings
typedef struct {
	unsigned char PE1_Power_Enable_Port_1_Status             : 1;   ///< Port 1 has power enabled
	unsigned char PE2_Power_Enable_Port_2_Status             : 1;   ///< Port 2 has power enabled
	unsigned char PE3_Power_Enable_Port_3_Status             : 1;   ///< Port 3 has power enabled
	unsigned char PE4_Power_Enable_Port_4_Status             : 1;   ///< Port 4 has power enabled
	unsigned char PG1_Power_Good_Port_1_Status               : 1;   ///< Port 1 is powered on with good voltage levels
	unsigned char PG2_Power_Good_Port_2_Status               : 1;   ///< Port 2 is powered on with good voltage levels
	unsigned char PG3_Power_Good_Port_3_Status               : 1;   ///< Port 3 is powered on with good voltage levels
	unsigned char PG4_Power_Good_Port_4_Status               : 1;   ///< Port 4 is powered on with good voltage levels
}TPS238x_Power_Stauts_Register_t;

#define PE1_STATUS                             0x01   ///< Port 1 has power enabled
#define PE2_STATUS                             0x02   ///< Port 2 has power enabled
#define PE3_STATUS                             0x04   ///< Port 3 has power enabled
#define PE4_STATUS                             0x08   ///< Port 4 has power enabled
#define PG1_STATUS                             0x10   ///< Port 1 is powered on with good voltage levels
#define PG2_STATUS                             0x20   ///< Port 2 is powered on with good voltage levels
#define PG3_STATUS                             0x40   ///< Port 3 is powered on with good voltage levels
#define PG4_STATUS                             0x80   ///< Port 4 is powered on with good voltage levels
#define POWER_ENABLE_STATUS                   (PE1_STATUS + PE2_STATUS + PE3_STATUS + PE4_STATUS)
#define POWER_GOOD_STATUS                     (PG1_STATUS + PG2_STATUS + PG3_STATUS + PG4_STATUS)
#define POWER_GOOD_SHIFT                       4
#define GET_POWER_ENABLE_STATUS(x)            (x & POWER_ENABLE_STATUS)
#define GET_POWER_GOOD_STATUS(x)              (x >> POWER_GOOD_SHIFT)

/// I2C Slave Address Register - Software configurable using tpc_SetI2CAddresses() <br>
///                   Hardware - Command 11h; 1 data byte; read only               <br>
///                            - writable during I2C slave address programming protocol <br><br>
/// A chain of TPS23861 devices can be configured by the processor to provide each device with a unique I2C address
typedef struct {
	unsigned char I2C_slave_address                          : 7;    ///< 7 bit I2C address
	unsigned char Auto                                       : 1;    ///< The part will default into auto mode after power on if this bit is set
}TPS238x_I2C_Slave_Address_Register_t;

#define AUTO_BIT                               0x80    ///< The part will default into auto mode after power on if this bit is set
#define I2C_ADDRESS_MASK                       0x7F    ///< 7 bit I2C address

#define TPS238X_ALERT_RESPONSE_ADDRESS         0x0C
#define TPS238X_BROADCAST_ADDRESS              0x30
#define TPS238X_UNLOCK_CODE                    0xAA      ///< Unlock code for the I2C address registers to prevent disastrous overwrites

/// Operating Mode Register - Uses TPS238x_Operating_Modes_t; Configured in the tps_ConfigPort() function  <br>
///                Hardware - Command 12h; 1 data byte; read/write                                         <br><br>
/// Configures the operating mode for each port (off, manual, semi-auto, or auto)
typedef struct {
	unsigned char Port_1_Operating_Mode                      : 2;    ///< Operating mode for port 1
	unsigned char Port_2_Operating_Mode                      : 2;    ///< Operating mode for port 2
	unsigned char Port_3_Operating_Mode                      : 2;    ///< Operating mode for port 3
	unsigned char Port_4_Operating_Mode                      : 2;    ///< Operating mode for port 4
}TPS238x_Operating_Mode_Register_t;

/// Operating mode for each of the ports (off, manual, semi-auto, auto)
typedef enum {
  OPERATING_MODE_OFF                         = 0x0,   ///< Off; no detection or classifications
  OPERATING_MODE_MANUAL                      = 0x1,   ///< Manual, detection and classification upon command
  OPERATING_MODE_SEMI_AUTO                   = 0x2,   ///< Semi-auto, automatic detection and classification (if enabled), but no automatic power on
  OPERATING_MODE_AUTO                        = 0x3    ///< Auto, automatic detection,. classification, anb power on
} TPS238x_Operating_Modes_t;

#define OPERATING_MODE_MASK                    0x3

#define OPERATING_PORT_1_MODE                  0x03
#define OPERATING_PORT_2_MODE                  0x0C
#define OPERATING_PORT_3_MODE                  0x30
#define OPERATING_PORT_4_MODE                  0xC0

/// Disconnect Enable Register - Software API uses TPS238x_Ports_t. Configured in tps_SetDisconnectEnable() and tps_SetPortDisconnectEnable()   <br>
///                   Hardware - Command 13h; 1 data byte; read/write <br><br>
/// Enable the disconnect detection mechanism for the indicated ports. 
typedef struct {
	unsigned char DCDE1_Disconnect_Enable_Port_1             : 1;   ///< DC disconnect enable for 2 pair operation in port 1
	unsigned char DCDE2_Disconnect_Enable_Port_2             : 1;   ///< DC disconnect enable for 2 pair operation in port 2
	unsigned char DCDE3_Disconnect_Enable_Port_3             : 1;   ///< DC disconnect enable for 2 pair operation in port 3
	unsigned char DCDE4_Disconnect_Enable_Port_4             : 1;   ///< DC disconnect enable for 2 pair operation in port 4
	unsigned char Reserved_3                                 : 4;
} TPS238x_Disconnect_Enable_Register_t;

#define DCDE1                                  0x01     ///< DC disconnect enable for 2 pair operation in port 1
#define DCDE2                                  0x02     ///< DC disconnect enable for 2 pair operation in port 2
#define DCDE3                                  0x04     ///< DC disconnect enable for 2 pair operation in port 3
#define DCDE4                                  0x08     ///< DC disconnect enable for 2 pair operation in port 4

/// Detect/Class Enable Register - Software API uses TPS238x_Ports_t;  Configured in tps_SetDetectClassEnable() and tps_SetPortDetectClassEnable()   <br>
///                     Hardware - Command 14h; 1 data byte; read/write  <br><br>
///  Enables detection and classification for each port. Behave differently in Manual, Semi-auto and auto modes. <br>
///  In manual mode, setting a bit means one cycle of detection or classification is performed for the indicated port. The bit
///   is cleared automatically when the cycle is complete. This is similar to the restart commands<br>
/// In semi-auto mode, detection and classifications are performed continuously as long as the port is not powered up. Classifications will
///  follow valid detections. <br>
/// In auto mode, classifications will follow valid detections, and power up will follow valid classifications. <br>
/// @note During cool down time following a fault, execution of this command will be delayed till the end of the cool down period.
typedef struct {
	unsigned char DETE1_Detection_Enable_Port_1              : 1;     ///< Enable detections for port 1
	unsigned char DETE2_Detection_Enable_Port_2              : 1;     ///< Enable detections for port 2
	unsigned char DETE3_Detection_Enable_Port_3              : 1;     ///< Enable detections for port 3
	unsigned char DETE4_Detection_Enable_Port_4              : 1;     ///< Enable detections for port 4
	unsigned char CLE1_Classification_Enable_Port_1          : 1;     ///< Enable classifications for port 1
	unsigned char CLE2_Classification_Enable_Port_2          : 1;     ///< Enable classifications for port 2
	unsigned char CLE3_Classification_Enable_Port_3          : 1;     ///< Enable classifications for port 3
	unsigned char CLE4_Classification_Enable_Port_4          : 1;     ///< Enable classifications for port 4
} TPS238x_Detect_Classification_Enable_Register_t;

#define DETE1                                  0x01     ///< Enable detections for port 1
#define DETE2                                  0x02     ///< Enable detections for port 2
#define DETE3                                  0x04     ///< Enable detections for port 3
#define DETE4                                  0x08     ///< Enable detections for port 4
#define CLE1                                   0x10     ///< Enable classifications for port 1
#define CLE2                                   0x20     ///< Enable classifications for port 2
#define CLE3                                   0x40     ///< Enable classifications for port 3
#define CLE4                                   0x80     ///< Enable classifications for port 4

/// Port Power Priority Register - Software API uses TPS238x_Ports_t; Configured in tps_FastShutdownEnable() and tps_FastPortShutdownEnable   <br>
///                     Hardware - Command 15h; 1 data byte; read/write  <br><br>
/// Port power priority bits used for fast shutdown
typedef struct {
    unsigned char Reserved_4                                 : 4;
	unsigned char FSE1_Fast_Shutdown_Enable_Port_1           : 1;   ///< Enable fast shutdown port 1
	unsigned char FSE2_Fast_Shutdown_Enable_Port_2           : 1;   ///< Enable fast shutdown port 2
	unsigned char FSE3_Fast_Shutdown_Enable_Port_3           : 1;   ///< Enable fast shutdown port 3
	unsigned char FSE4_Fast_Shutdown_Enable_Port_4           : 1;   ///< Enable fast shutdown port 4
} TPS238x_Fast_Shutdown_t;

#define FSE1                                   0x10
#define FSE2                                   0x20
#define FSE3                                   0x40
#define FSE4                                   0x80

#define FSE_SHIFT                              4

/// Timing Configuration Register - Software configured in tps_SetTiming() <br>
///                      Hardware - Command 16h; 1 data byte; read/write   <br><br>
/// Set the timing configurations used by all four ports
typedef struct {
	unsigned char TDIS_Time_Disconnect_Delay                 : 2;    ///< Disconnect delay, which is the time to turn off a port once there is a disconnect condition
    unsigned char TICUT_ICUT_Fault_Timing                    : 2;    ///< ICUT Fault Timing period, which is the overcurrent time duration before port turn off 
	unsigned char TSTART_Start_Time                          : 2;    ///< TSTART period, which is the maximum allowed overcurrent time during inrush
	unsigned char TLIM_ILIM_Fault_Timing                     : 2;    ///< ILIM fault timing, which is the foldback current time limit duration before port turn off
} TPS238x_Timing_Configuration_Register_t;

/// Set the ILIM fault timing, which is the foldback current time limit duration before port turn off. Used in tps_SetTiming()
typedef enum {
  TLIM_60_MS                                 = 0x0,      ///< 60 ms
  TLIM_30_MS                                 = 0x1,      ///< 30 ms
  TLIM_15_MS                                 = 0x2,      ///< 15 ms
  TLIM_10_MS                                 = 0x3       ///< 10 ms
} TPS238x_ILIM_Timing_t;

/// Set the length of the TSTART period, which is the maximum allowed overcurrent time during inrush. Used in tps_SetTiming()
typedef enum {
  TSTART_60_MS                               = 0x0,      ///< 60 ms
  TSTART_30_MS                               = 0x1,      ///< 30 ms
  TSTART_120_MS                              = 0x2       ///< 120 ms
} TPS238x_TStart_Timing_t;

/// Set the length of the ICUT Fault Timing period, which is the overcurrent time duration before port turn off. Used in tps_SetTiming()
typedef enum {
  TICUT_60_MS                                = 0x0,      ///< 60 ms
  TICUT_30_MS                                = 0x1,      ///< 30 ms
  TICUT_120_MS                               = 0x2,      ///< 120 ms
  TICUT_240_MS                               = 0x3       ///< 240 ms
} TPS238x_TICUT_Timing_t;

/// Set the length of the Disconnect delay, which is the time to turn off a port once there is a disconnect condition. Used in tps_SetTiming()
typedef enum {
  TDIS_360_MS                                = 0x0,      ///< 360 ms
  TDIS_90_MS                                 = 0x1,      ///< 90 ms
  TDIS_180_MS                                = 0x2,      ///< 180 ms
  TDIS_720_MS                                = 0x3       ///< 720 ms
} TPS238x_TDIS_Timing_t;


#define TDIS_MASK                              0x3
#define TDIS_SHIFT                             0x0

#define TICUT_MASK                             0xC
#define TICUT_SHIFT                            0x2

#define TSTART_MASK                            0x30
#define TSTART_SHIFT                           0x4

#define TLIM_MASK                              0xC0
#define TLIM_SHIFT                             0x6

/// General Mask 1 Register                                         <br>
///                Hardware - Command 17h; 1 data byte; read/write  <br><br>
///
typedef struct {
	unsigned char _250M_Current_Sense_250_mOhm                 : 1;     ///< 0 = 255 mOhm current sense resistor; 1 = 250 mOhm
	unsigned char Reserved_5                                   : 3;
	unsigned char MAINS_Detection_Voltage_Measurement_Duration : 1;     ///< 0 = Convert port voltage 800 A/D per second; 1 = 960
	unsigned char Reserved_6                                   : 2;
	unsigned char INTEN_INT_Pin_Mask                           : 1;     ///< 1 = Interrupts will generate \\INT pin output 
} TPS238x_General_Mask_1_Register_t;

#define _960_A_D_CONV_PER_SECOND               0x10
#define _800_A_D_CONV_PER_SECOND               0x00


#define _250M                                  0x01      ///< 250 mOhm resistor used as current sensor
#define MAINS                                  0x10      ///< Convert port voltage rate of 960 or 800 A/D conversions per sec
#define INTEN                                  0x80      ///< External INT/ enabled by any unmasked bit of interrupt register

/// Detect/Class Restart Register - Software API uses TPS238x_Ports_t; Configured in tps_RestartDetection(), tps_RestartPortDetection(),
///                                                 tps_RestartClassification(), tps_RestartPortClassification(), and tps_RestartDetectClass()   
///                      Hardware - Command 18h; 1 data byte; write only
/// In manual mode, the indicated ports will restart detection/classification. In auto mode, action is dependant on the ports operating mode
/// @note During a cool down period following a fault condition, this command will be accepted, but deferrred until the cool down timer expires.

typedef struct {
	unsigned char RDET1_Restart_Detection_Port_1             : 1;    ///< Restart Detection Port 1
	unsigned char RDET2_Restart_Detection_Port_2             : 1;    ///< Restart Detection Port 2
	unsigned char RDET3_Restart_Detection_Port_3             : 1;    ///< Restart Detection Port 3
	unsigned char RDET4_Restart_Detection_Port_4             : 1;    ///< Restart Detection Port 4
	unsigned char RCL1_Restart_Classification_Port_1         : 1;    ///< Restart Classification Port 1
	unsigned char RCL2_Restart_Classification_Port_2         : 1;    ///< Restart Classification Port 2
	unsigned char RCL3_Restart_Classification_Port_3         : 1;    ///< Restart Classification Port 3
	unsigned char RCL4_Restart_Classification_Port_4         : 1;    ///< Restart Classification Port 4
} TPS238x_Detect_Class_Restart_Register_t;

#define RDET1                                  0x01    ///< Restart Detection Port 1
#define RDET2                                  0x02    ///< Restart Detection Port 2
#define RDET3                                  0x04    ///< Restart Detection Port 3
#define RDET4                                  0x08    ///< Restart Detection Port 4
#define RCL1                                   0x10    ///< Restart Classification Port 1
#define RCL2                                   0x20    ///< Restart Classification Port 2
#define RCL3                                   0x40    ///< Restart Classification Port 3
#define RCL4                                   0x80    ///< Restart Classification Port 4

#define RESTART_DETECTION_SHIFT                0
#define RESTART_DETECTION_MASK                 0x0F

#define RESTART_CLASSIFCATION_SHIFT            4
#define RESTART_CLASSIFCATION_MASK             0xF0

/// Power Enable Register - Software API uses TPS238x_Ports_t; Configured in tps_SetPortPower, tps_SetPowerOn(), and tps_SetPowerOff() <br>
///              Hardware - Command 19h; 1 data byte; write only    <br><br>
/// Directs the TPS23861 to power a port on or off.
/// @note Setting PWON and PWOFF for the same register turns the port OFF

typedef struct {
	unsigned char PWON1_Power_On_Port_1                      : 1;     ///< Power on Port 1
	unsigned char PWON2_Power_On_Port_2                      : 1;     ///< Power on Port 2
	unsigned char PWON3_Power_On_Port_3                      : 1;     ///< Power on Port 3
	unsigned char PWON4_Power_On_Port_4                      : 1;     ///< Power on Port 4
	unsigned char PWOFF1_Power_On_Port_1                     : 1;     ///< Power off Port 1
	unsigned char PWOFF2_Power_On_Port_2                     : 1;     ///< Power off Port 2
	unsigned char PWOFF3_Power_On_Port_3                     : 1;     ///< Power off Port 3
	unsigned char PWOFF4_Power_On_Port_4                     : 1;     ///< Power off Port 4
} TPS238x_Power_Enable_Register_t;

#define PWON1                                  0x01     ///< Power on Port 1
#define PWON2                                  0x02     ///< Power on Port 2
#define PWON3                                  0x04     ///< Power on Port 3
#define PWON4                                  0x08     ///< Power on Port 4
#define PWOFF1                                 0x10     ///< Power off Port 1 
#define PWOFF2                                 0x20     ///< Power off Port 2
#define PWOFF3                                 0x40     ///< Power off Port 3
#define PWOFF4                                 0x80     ///< Power off Port 4

#define POWER_OFF_SHIFT                        4
#define POWER_OFF_MASK                         0xF0

#define POWER_ON_SHIFT                         0
#define POWER_ON_MASK                          0x0F

/// Reset Register - Software API uses TPS238x_Ports_t in tps_ResetPorts() and tps_ResetPort()<br>   
///       Hardware - Command 1Ah; 1 data byte; write only <br><br>
/// Forces reset events to occur.
typedef struct {
	unsigned char RESP1_Reset_Port_1                         : 1;    ///< Reset port 1
	unsigned char RESP1_Reset_Port_2                         : 1;    ///< Reset port 2
	unsigned char RESP1_Reset_Port_3                         : 1;    ///< Reset port 3
	unsigned char RESP1_Reset_Port_4                         : 1;    ///< Reset port 4
	unsigned char RESAL_Reset_Registers                      : 1;    ///< Reset register bits; Equivalent to power up reset
	unsigned char Reserved_7                                 : 1;
	unsigned char CLINP_Clear_Interrupt_Pin                  : 1;     ///< Note this does not affect any of the interrupt status bits
	unsigned char CLRAIN_Clear_Interrupt_Bits                : 1;     ///< Note this also releases the Interrupt Pin
} TPS238x_Reset_Register_t;

#define RESP1                                  0x01
#define RESP2                                  0x02
#define RESP3                                  0x04
#define RESP4                                  0x08
#define RESAL                                  0x10
#define CLINP                                  0x40
#define CLRAIN                                 0x80

/// ID Register                                          <br>
///    Hardware - Command 1Bh; 1 data byte; read/write   <br>
///
typedef struct {
	unsigned char ICV_IC_Version_Number                      : 3;     ///< IC Version number
	unsigned char MFR_ID_Manufacture_ID_Number               : 5;     ///< Manufacture Identification Number
} TPS238x_ID_Register_t;

#define ICV                                    0x07
#define MFR_ID                                 0xF8
#define MFR_ID_SHIFT                           3
#define MFR_ID_DEFAULT                         0x0A  // This is the the results in the MFR_ID section of the register
#define ICV_DEFAULT                            0x00
#define ID_REGISTER_DEFAULT                    ((MFR_ID_DEFAULT<<MFR_ID_SHIFT) | ICV_DEFAULT)

/// Legacy Detect Mode Register - Software API uses TPS238x_Ports_t in tps_ConfigPort()  <br>
///                    Hardware - Command 20h; 1 data byte; read/write                   <br><br>
/// The TPS23861 can perform a legacy-detect operation only or a standard detection followed by a legacy-detect operation.
typedef struct {
	unsigned char LEGMOD1_Legacy_Detect_Port_1               : 2;   ///< Port 1 setting: Use TPS238x_Legacy_Detect_t values
	unsigned char LEGMOD2_Legacy_Detect_Port_2               : 2;   ///< Port 2 setting: Use TPS238x_Legacy_Detect_t values
	unsigned char LEGMOD3_Legacy_Detect_Port_3               : 2;   ///< Port 3 setting: Use TPS238x_Legacy_Detect_t values
	unsigned char LEGMOD4_Legacy_Detect_Port_4               : 2;   ///< Port 4 setting: Use TPS238x_Legacy_Detect_t values
} TPS238x_Legacy_Detect_Register_t;

/// Define the legacy-detect mode for each port . Used in tps_ConfigPort() 
typedef enum {
  LEGACY_DETECT_DISABLED                     = 0x0,          ///< Perform standard detection operation only
  LEGACY_DETECT_ONLY                         = 0x1,          ///< Perform legacy detection operation only
  LEGACY_DETECT_STANDARD_THEN_LEGACY         = 0x2           ///< Perform a standard detection operation and follow with a legacy detection operation
} TPS238x_Legacy_Detect_t;                   


#define LEGMOD1                                0x03
#define LEGMOD2                                0x0C
#define LEGMOD3                                0x30
#define LEGMOD4                                0xC0

#define LEGACY_MODE_MASK                       3

#define LEGMOD1_SHIFT                          0
#define LEGMOD2_SHIFT                          2
#define LEGMOD3_SHIFT                          4
#define LEGMOD4_SHIFT                          6

/// Two Event Classification Register - Software configured in tps_ConfigPort() <br>
///                          Hardware - Command 21h; 1 data byte; read/write.   <br> <br>
/// Allows the user to estables two-event physical-layer classification when a class 4 or 5 PD is classified.
typedef struct {
	unsigned char TECLEN1_Two_Event_Classification_Port_1    : 2;   ///< Port 1 setting: Use TPS238x_Two_Event_t values 
	unsigned char TECLEN2_Two_Event_Classification_Port_2    : 2;   ///< Port 2 setting: Use TPS238x_Two_Event_t values
	unsigned char TECLEN3_Two_Event_Classification_Port_3    : 2;   ///< Port 3 setting: Use TPS238x_Two_Event_t values
	unsigned char TECLEN4_Two_Event_Classification_Port_4    : 2;   ///< Port 4 setting: Use TPS238x_Two_Event_t values
} TPS238x_Two_Event_Classification_Register_t;

/// Set the conditions for PSE-initiated two-event physical classifications. Used in tps_ConfigPort()
typedef enum {
  TWO_EVENT_DISABLE                          = 0x0,          ///< Two-event classification is disabled
  TWO_EVENT_AFTER_CLASS_4                    = 0x1,          ///< Start second classification event if class 4 event occurs
  TWO_EVENT_AFTER_CLASS_5                    = 0x2,          ///< Start second classification event if class 5 event occurs
  TWO_EVENT_AFTER_CLASS_4_OR_5               = 0x3           ///< Start second classification event if class 4 or 5 event occurs
} TPS238x_Two_Event_t;

#define TECLEN1                                0x03
#define TECLEN2                                0x0C
#define TECLEN3                                0x30
#define TECLEN4                                0xC0

#define TWO_EVENT_MASK                         3

#define TECLEN1_SHIFT                          0
#define TECLEN2_SHIFT                          2
#define TECLEN3_SHIFT                          4
#define TECLEN4_SHIFT                          6

/// Four Pair Mode Register - Software configures using tps_Config4Pair() <br>
///                Hardware - Command 22h; 1 data byte; read/write <br><br>
/// Establishes four pair operation and defines disconnect modes for 4 four pair architectures
typedef struct {
	unsigned char _4P12DIS_Disconnect_Mode_Four_Port_1_2     : 3;   ///< Disconnect mode combinied ports 1&2; Use TPS238x_Four_Pair_t values 
	unsigned char _4P12EN_Enable_Four_Port_Mode_1_2          : 1;   ///< Enable 4 pair mode for ports 3&4
	unsigned char _4P34DIS_Disconnect_Mode_Four_Port_3_4     : 3;   ///< Disconnect mode combinied ports 1&2; Use TPS238x_Four_Pair_t values 
	unsigned char _4P34EN_Enable_Four_Port_Mode_3_4          : 1;   ///< Enable 4 pair mode for ports 3&4
} TPS238x_Four_Port_Mode_Register_t;

/// Define Four Pair Disconnect modes in tps_Config4Pair()
typedef enum {
  FOUR_PAIR_DISCONNECT_DISABLED              = 0x0,
  FOUR_PAIR_DISCONNECT_BASED_ON_LOWER_PORT   = 0x1,       ///< eg. Disconnect 1 and 2 if port 1 disconnect conditions met
  FOUR_PAIR_DISCONNECT_BASED_ON_HIGHER_PORT  = 0x2,       ///< eg. Disconnect 1 and 2 if port 2 disconnect conditions met
  FOUR_PAIR_DISCONNECT_BASED_ON_EITHER_PORT  = 0x3,       ///< eg. Disconnect 1 and 2 if port 1 or 2 disconnect conditions met
  FOUR_PAIR_DISCONNECT_BASED_ON_BOTH_PORTS   = 0x4        ///< eg. Disconnect 1 and 2 if port 1 and 2 disconnect conditions met
} TPS238x_Four_Pair_t;

#define _4P12DIS                               0x07
#define _4P12EN                                0x08
#define _4P34DIS                               0x70
#define _4P34EN                                0x80

#define FOUR_PAIR_DISCONNECT_MASK              7

#define _4P12DIS_SHIFT                         0
#define _4P12EN_SHIFT                          3

#define _4P34DIS_SHIFT                         4
#define _4P34EN_SHIFT                          7

/// Class 5 Enable - Timer Register - Software configured in tps_ConfigPort(), tps_SetInterruptMask, and tps_GetInterruptMask()  <br>
///                        Hardware - Command 27h; 1 data byte; read/write <br><br>
/// Allows for each port to enable class 5 classifications AND adds the ability to defer non-critical interrupts
typedef struct {
	unsigned char TMR_Timer_Period_10_ms                     : 4;   ///< Timer used to gather non-critical interrupts; 10ms LSB
	unsigned char CL5EN1_Class_5_Enable_Port_1               : 1;   ///< Enable class 5 classifications for port 1
	unsigned char CL5EN2_Class_5_Enable_Port_2               : 1;   ///< Enable class 5 classifications for port 2
	unsigned char CL5EN3_Class_5_Enable_Port_3               : 1;   ///< Enable class 5 classifications for port 3
	unsigned char CL5EN4_Class_5_Enable_Port_4               : 1;   ///< Enable class 5 classifications for port 4
} TPS238x_Class_5_Enable_Timer_Register_t;

#define TMR_MASK                               0x0F   ///< Location of interrupt delay time bits. Value in 10ms increments
#define CL5EN1                                 0x10   ///< Enable class 5 classifications for port 1
#define CL5EN2                                 0x20   ///< Enable class 5 classifications for port 2
#define CL5EN3                                 0x40   ///< Enable class 5 classifications for port 3
#define CL5EN4                                 0x80   ///< Enable class 5 classifications for port 4

#define CLASS_5_ENABLE_SHIFT                   4

/// Disconnect Threshold Register - Software configured in tps_ConfigPort()   <br>
///                      Hardware - Command 29h; 1 data byte; read/write      <br><br>
/// Allows the user to set the current threshold for disconnection for each port
typedef struct {
	unsigned char DCTH1_Disconnect_Current_Threshold_Port_1  : 2;     ///< Port 1: Use TPS238x_Disconnect_Threshold_t values
	unsigned char DCTH2_Disconnect_Current_Threshold_Port_2  : 2;     ///< Port 2: Use TPS238x_Disconnect_Threshold_t values
	unsigned char DCTH3_Disconnect_Current_Threshold_Port_3  : 2;     ///< Port 3: Use TPS238x_Disconnect_Threshold_t values
	unsigned char DCTH4_Disconnect_Current_Threshold_Port_4  : 2;     ///< Port 4: Use TPS238x_Disconnect_Threshold_t values
} TPS238x_Disconnect_Threshold_Register_t;

/// Current levels for disconnect threshold. Used in tps_ConfigPort()
typedef enum {
  DCTH_7_5_MILLIAMP                          = 0x0,      ///< 7.5 mA
  DCTH_15_MILLIAMP                           = 0x1,      ///< 15 mA
  DCTH_30_MILLIAMP                           = 0x2,      ///< 30 mA
  DCTH_50_MILLIAMP                           = 0x3       ///< 50 mA
} TPS238x_Disconnect_Threshold_t;

#define DCTH1                                  0x03
#define DCTH2                                  0x0C
#define DCTH3                                  0x30
#define DCTH4                                  0xC0

#define DISCONNECT_THRESHOLD_MASK              3

#define DCTH1_SHIFT                            0
#define DCTH2_SHIFT                            2
#define DCTH3_SHIFT                            4
#define DCTH4_SHIFT                            6

/// ICUT21 Config Register - Software configured in tps_ConfigPort()     <br>
///               Hardware - Command 2Ah; 1 data byte; read/write        <br><br>
/// Allow the user to define the ICUT Threshold for ports 1 and 2. If the ICUT current is exceeded, the TICUT
///  timer will begin to count. If timer reaches 0, an ICUT fault will be declared and the port shut down.
typedef struct {
	unsigned char ICUT_Current_Threshold_Port_1              : 3;    ///< Use TPS238x_ICUT_Config_t values
	unsigned char Reserved_8                                 : 1;
	unsigned char ICUT_Current_Threshold_Port_2              : 3;    ///< Use TPS238x_ICUT_Config_t values
	unsigned char Reserved_9                                 : 1;
} TPS238x_ICUT21_Config_Regsiter_t;

/// ICUT Current thresholds. Used in tps_ConfigPort()
typedef enum {
  ICUT_374_MILLIAMP                          = 0x0,       ///< 374 mA
  ICUT_110_MILLIAMP                          = 0x1,       ///< 110 mA
  ICUT_204_MILLIAMP                          = 0x2,       ///< 204 mA

  ICUT_754_MILLIAMP                          = 0x4,       ///< 754 mA
  ICUT_592_MILLIAMP                          = 0x5,       ///< 592 mA
  ICUT_686_MILLIAMP                          = 0x6,       ///< 686 mA
  ICUT_920_MILLIAMP                          = 0x7        ///< 920 mA
} TPS238x_ICUT_Config_t;

#define ICUT_THRESHOLD_MASK                    7

#define ICUT_PORT_1                            0x07
#define ICUT_PORT_2                            0x70

#define ICUT_PORT_1_SHIFT                      0
#define ICUT_PORT_2_SHIFT                      4

/// ICUT43 Config Register  - Software configured in tps_ConfigPort()    <br>
///               Hardware - Command 2Bh; 1 data byte; read/write        <br><br>
/// Allow the user to define the ICUT Threshold for ports 1 and 2. If the ICUT current is exceeded, the TICUT
///  timer will begin to count. If timer reaches 0, an ICUT fault will be declared and the port shut down.

typedef struct {
	unsigned char ICUT_Current_Threshold_Port_3              : 3;     ///< Use TPS238x_ICUT_Config_t values 
	unsigned char Reserved_10                                : 1;
	unsigned char ICUT_Current_Threshold_Port_4              : 3;     ///< Use TPS238x_ICUT_Config_t values
	unsigned char Reserved_11                                : 1;
} TPS238x_ICUT43_Config_Regsiter_t;

#define ICUT_PORT_3                            0x07
#define ICUT_PORT_4                            0x70

#define ICUT_PORT_3_SHIFT                      0
#define ICUT_PORT_4_SHIFT                      4

/// Temperature Register - Software gets this value using tps_GetTemperature() <br>
///             Hardware - Command 2Ch; 1 data byte; read only                 <br><br>
/// Die temperature
typedef struct {
	unsigned char Temp_Value;
} TPS238x_Temperature_Register_t;

#define CONVERT_TEMP(x)                        (((x*652)-20000)/1000)       ///< Macro to convert result from tps_GetTemperature() into degrees C (float)

/// Input Voltage Register - Software gets this value using tps_GetInputVoltage()     <br>
///               Hardware - Command 2Eh; 2 data byte (LSB followed by MSB); read only   <br><br>
/// The system will measure the input voltage around 1/sec. The returned value has an LSB of 3.662mV

typedef union {
	struct Input_Voltage_Short_t {
		unsigned short Input_Voltage                         : 14;       ///< Voltage with 3.662 mV lsb
		unsigned short Reserved_12                           :  2;
	} Input_Voltage_Short;

	struct Input_Voltage_Char_t {
		unsigned char  Input_Voltage_LSB                     : 8;
		unsigned char  Input_Voltage_MSB                     : 6;
		unsigned char  Reserved_13                           : 2;
	} Input_Voltage_Char;
} TPS238x_Input_Voltage_Register_u;

typedef unsigned short TPS238x_Input_Voltage_t;

#define TPS2368X_INPUT_VOLTAGE_MASK_SHORT                    0x3FFF

/// Port 1 Current Register - Software gets this value using tps_GetPortMeasurements()  <br>
///                Hardware - Command 30h; 2 data byte (LSB followed by MSB); read only <br>
/// Port 2 Current Register                                                             <br>
///                Hardware - Command 34h; 2 data byte (LSB followed by MSB); read only <br>
/// Port 3 Current Register                                                             <br>
///                Hardware - Command 38h; 2 data byte (LSB followed by MSB); read only <br>
/// Port 4 Current Register                                                             <br>
///                Hardware - Command 3Ch; 2 data byte (LSB followed by MSB); read only <br><br>
/// 14 bit data conversion result of the current for the port.  The LSB is 63.360uA

typedef union {
	struct Port_Current_Short_t {
		unsigned short Port_Current                          : 14;    ///< Port current with lsb of 63.360 micro-Amps
		unsigned short Reserved_14                           :  2;
	} Port_Current_Short;

	struct Port_Current_Char_t {
		unsigned char  Port_Current_LSB                      : 8;
		unsigned char  Port_Current_MSB                      : 6;
		unsigned char  Reserved_15                           : 2;
	} Port_Current_Char;
} TPS238x_Port_Current_Register_u;

typedef unsigned short TPS238x_Port_Current_t;

#define TPS2368X_PORT_CURRENT_MASK_SHORT                     0x3FFF

/// Port 1 Voltage Register - Software gets this value using tps_GetPortMeasurements()  <br> 
///                Hardware - Command 32h; 2 data byte (LSB followed by MSB); read only <br>
/// Port 2 Voltage Register                                                             <br>
///                Hardware - Command 36h; 2 data byte (LSB followed by MSB); read only <br>
/// Port 3 Voltage Register                                                             <br>
///                Hardware - Command 3Ah; 2 data byte (LSB followed by MSB); read only <br>
/// Port 4 Voltage Register                                                             <br>
///                Hardware - Command 3Eh; 2 data byte (LSB followed by MSB); read only <br><br>
///     14 bit data conversion result of the voltage for the port. The LSB is 3.662mV
typedef union {
	struct Port_Voltage_Short_t {
		unsigned short Port_Voltage                          : 14;       ///< Voltage of port with lsb of 3.662 milli-Volts
		unsigned short Reserved_16                           :  2;
	} Port_Voltage_Short;

	struct Port_Voltage_Char_t {
		unsigned char  Port_Voltage_LSB                      : 8;
		unsigned char  Port_Voltage_MSB                      : 6;
		unsigned char  Reserved_17                           : 2;
	} Port_Voltage_Char;
} TPS238x_Port_Voltage_Register_u;

typedef unsigned short TPS238x_Port_Voltage_t;

#define TPS2368X_PORT_VOLTAGE_MASK_SHORT                     0x3FFF

/// PoE Plus Register - Software configures this in tps_ConfigPort() <br> 
///          Hardware - Command 40h; 1 data byte; read/write <br><br>
///

typedef struct {
	unsigned char Reserved_18                                : 4;
	unsigned char POEP1_Foldback_Curve_Port_1                : 1;
	unsigned char POEP1_Foldback_Curve_Port_2                : 1;
	unsigned char POEP1_Foldback_Curve_Port_3                : 1;
	unsigned char POEP1_Foldback_Curve_Port_4                : 1;
} TPS238x_PoE_Plus_Register_t;

/// Foldback curve applied to a port when powered on. Used in tps_ConfigPort()
typedef enum {
  _1X_ILIM_FOLDBACK_CURVE                    = 0x0,       ///< 1 x Ilim foldback curve applied when port is powered on
  _2X_ILIM_FOLDBACK_CURVE                    = 0x1        ///< 2 x Ilim foldback curve applied when port is powered on
} TPS238x_POE_Plus_Foldback_t;

#define POEP1                                  0x10
#define POEP2                                  0x20
#define POEP3                                  0x40
#define POEP4                                  0x80

#define POE_PLUS_SHIFT                         4

/// I2C Watchdog Register                                          <br>
///              Hardware - Command 42h; 1 data byte; read/write   <br><br>
/// Monitors the I2C clock line in order to detect hung I2C communcations
typedef struct {
	unsigned char WDS_Watchdog_Status                        : 1;     ///< Watchdog timer has expired
	unsigned char IWD_I2C_Watchdog_Disable                   : 4;     ///< Set to IWD_MASK_VALUE to disable I2C watchdog (default)
	unsigned char Reserved_19                                : 3;
} TPS238x_I2C_Watchdog_Register_t;

#define WDS                                    0x01
#define IWD                                    0x1E

#define IWD_SHIFT                              1
#define IWD_MASK_VALUE                         0xB            ///< Value to place in the IWD bits to disable the I2C watchdog (default setting)

/// Cool Down - Gate Drive Register - Software configures this in tps_SetTiming() <br>
///                        Hardware - Command 45h; 1 data byte; read/write        <br><br>
/// Fault cool down timer and Gate Pullup Current
typedef struct {
	unsigned char Reserved_20                                : 5;
	unsigned char IGATE_Gate_Pullup_Current                  : 1;     ///< 0=50uA; 1=25uA
	unsigned char CLDN_Fault_Cool_Down_Timer                 : 2;     ///< Use TPS238x_Cool_Down_Timing_t values
} TPS238x_Cool_Down_Gate_Drive_Register_t;

/// Used in tps_SetTiming() to set the time between a port shut down due to a fault and a subsequent power up
typedef enum {
  COOL_DOWN_1_SEC                            = 0x0,     ///< Cool down time of 1 sec after port shut down due to fault before port power up
  COOL_DOWN_2_SEC                            = 0x2,     ///< Cool down time of 2 sec after port shut down due to fault before port power up
  COOL_DOWN_4_SEC                            = 0x3      ///< Cool down time of 4 sec after port shut down due to fault before port power up
} TPS238x_Cool_Down_Timing_t;

#define IGATE                                  0x20
#define CLDN_MASK                              0xC0

#define CLDN_SHIFT                             6

#define GATE_PULLUP_CURRENT_50_MICROAMP        0
#define GATE_PULLUP_CURRENT_25_MICROAMP        1

/// Port 1 Detect Resistance Register - Software gets these values using tps_GetDetectResistance() <br>
///                          Hardware - Command 60h; 2 data byte (LSB followed by MSB); read only  <br>
/// Port 2 Detect Resistance Register                                                              <br>
///                          Hardware - Command 62h; 2 data byte (LSB followed by MSB); read only  <br>
/// Port 3 Detect Resistance Register                                                              <br>
///                          Hardware - Command 64h; 2 data byte (LSB followed by MSB); read only  <br>
/// Port 4 Detect Resistance Register                                                              <br>
///                          Hardware - Command 66h; 2 data byte (LSB followed by MSB); read only  <br><br>
/// The most recent 2 point Detection Resistance measurement result. The value is only good when the status does
///  not indicate a fault AND at least one detection operation has taken place on the port. The LSB is 11.0966 ohms.

typedef union {
	struct Port_Detect_Resistance_Short_t {
		unsigned short Port_Detect_Resistance                : 14;   ///< LSB of 11.0966 ohms
		unsigned short Detect_Status                         :  2;   ///< Use TPS238x_Detect_Resistance_Status_t values
	} Port_Detect_Resistance_Short;

	struct Detect_Resistance_Char_t {
		unsigned char  Port_Detect_Resistance_LSB            : 8;
		unsigned char  Port_Detect_Resistance_MSB            : 6;
		unsigned char  Detect_Status                         : 2;
	} Port_Detect_Resistance_Char;
} TPS238x_Port_Detect_Resistance_Register_u;

typedef unsigned short TPS238x_Port_Detect_Resistance_t;

// Determines the most recent detection result status. Used in tps_GetDetectResistance()
typedef enum {
  RS_STATUS_GOOD                             = 0x0,      ///< If at least one detection operation and this status, the detection resistance measuremnt is good
  RS_STATUS_SHORT_CIRCUIT                    = 0x1,      ///< Short circuit < 2 kOhms)
  RS_STATUS_OPEN_CIRCUIT                     = 0x2,      ///< Open circuit
  RS_STATUS_MOSFET_SHORT_FAULT               = 0x3       ///< MOSFET short fault
} TPS238x_Detect_Resistance_Status_t;

#define PORT_RESISTANCE_MASK_SHORT             0x3FFF
#define RS_MASK_SHORT                          0xC000
#define RS_SHIFT_SHORT                         14

/// Port 1 Detect Voltage Difference Register - Software reads this value using tps_GetDetectVoltageDifference() <br> 
///                                  Hardware - Command 68h; 2 data byte (LSB followed by MSB); read only  <br>
/// Port 2 Detect Voltage Difference Register                                                              <br>
///                                  Hardware - Command 6Ah; 2 data byte (LSB followed by MSB); read only  <br>
/// Port 3 Detect Voltage Difference Register                                                              <br>
///                                  Hardware - Command 6Ch; 2 data byte (LSB followed by MSB); read only  <br>
/// Port 4 Detect Voltage Difference Register                                                              <br>
///                                  Hardware - Command 6Eh; 2 data byte (LSB followed by MSB); read only  <br><br>
/// This register is used to determine the presence of a legacy PD by measuring the PD input capacitance on the PI. 
/// A charge is injected into the PI and the resulting voltage differnce is reported. The resulting 12-bit data
/// conversion has an LSB of 4.884 mV, and is only valid if the status on the port is VDS_STATUS_VALID_MEASUREMENT.
typedef union {
	struct Port_Voltage_Difference_Short_t {
		unsigned short Port_Voltage_Difference               : 12;    ///< LSB of 4.884 mV
		unsigned short Voltage_Difference_Status             :  4;    ///< Use TPS238x_Detect_Voltage_Difference_Status_t values
	} Port_Voltage_Difference_Short;

	struct Port_Voltage_Difference_Char_t {
		unsigned char  Port_Voltage_Difference_LSB           : 8;
		unsigned char  Port_Voltage_Difference_MSB           : 6;
		unsigned char  Voltage_Difference_Status             : 2;
	} Port_Voltage_Difference_Char;
} TPS238x_Port_Voltage_Difference_Register_u;

typedef unsigned short TPS238x_Port_Voltage_Difference_t;

/// Indicates the status of the most recent voltage difference result on the port. The measurement is only valid
/// when the status is VDS_STATUS_VALID_MEASUREMENT. Used in tps_GetDetectVoltageDifference()
typedef enum {
  VDS_STATUS_POWER_ON_RESET                  = 0x0,          ///< Power-on reset
  VDS_STATUS_VALID_MEASUREMENT               = 0x1,          ///< Valid measurement
  VDS_STATUS_TIMEOUT                         = 0x2,          ///< Unable to achieve 2.2 V to take first measurement before timeout
  VDS_STATUS_FIRST_MEASUREMENT_EXCESS        = 0x3,          ///< First measurement exceeds V det-clamp (min)
  VDS_STATUS_SECOND_MEASUREMENT_EXCESS       = 0x4,          ///< Second measurement exceeds V det-clamp (min)
  VDS_STATUS_INSUFFICIENT_SIGNAL             = 0x5           ///< Delta V < 0.9 V (insufficient signal)
} TPS238x_Detect_Voltage_Difference_Status_t;


#define PORT_VOLTAGE_DIFFERENCE_MASK_SHORT     0x0FFF
#define VDS_MASK_SHORT                         0xF000
#define VDS_SHIFT_SHORT                        12

/// A macro that converts a port number into a port value
#define CONVERT_PORT_NUM(x)                    (1 << ((uint8_t)x-1))  


/**********************************************************************
                 I2C Commands for the TPS23861
***********************************************************************/

// I2C Commands Values
#define TPS238X_INTERRUPT_COMMAND                            0x00
#define TPS238X_INTERRUPT_MASK_COMMAND                       0x01
#define TPS238X_POWER_EVENT_COMMAND                          0x02
#define TPS238X_POWER_EVENT_CLEAR_COMMAND                    0x03
#define TPS238X_DETECTION_EVENT_COMMAND                      0x04
#define TPS238X_DETECTION_EVENT_CLEAR_COMMAND                0x05
#define TPS238X_FAULT_EVENT_COMMAND                          0x06
#define TPS238X_FAULT_EVENT_CLEAR_COMMAND                    0x07
#define TPS238X_INRUSH_LIMIT_EVENT_COMMAND                   0x08
#define TPS238X_INRUSH_LIMIT_EVENT_CLEAR_COMMAND             0x09
#define TPS238X_SUPPLY_EVENT_COMMAND                         0x0A
#define TPS238X_SUPPLY_EVENT_CLEAR_COMMAND                   0x0B
#define TPS238X_PORT_1_STATUS_COMMAND                        0x0C
#define TPS238X_PORT_2_STATUS_COMMAND                        0x0D
#define TPS238X_PORT_3_STATUS_COMMAND                        0x0E
#define TPS238X_PORT_4_STATUS_COMMAND                        0x0F
#define TPS238X_POWER_STATUS_COMMAND                         0x10
#define TPS238X_I2C_SLAVE_ADDRESS_COMMAND                    0x11
#define TPS238X_OPERATING_MODE_COMMAND                       0x12
#define TPS238X_DISCONNECT_ENABLE_COMMAND                    0x13
#define TPS238X_DETECT_CLASS_ENABLE_COMMAND                  0x14
#define TPS238X_PORT_POWER_PRIORITY_COMMAND                  0x15
#define TPS238X_TIMING_CONFIGURATION_COMMAND                 0x16
#define TPS238X_GENERAL_MASK_1_COMMAND                       0x17
#define TPS238X_DETECT_CLASS_RESTART_COMMAND                 0x18
#define TPS238X_POWER_ENABLE_COMMAND                         0x19
#define TPS238X_RESET_COMMAND                                0x1A
#define TPS238X_ID_COMMAND                                   0x1B

#define TPS238X_TEST_ENABLE_COMMAND                          0x1D

#define TPS238X_LEGACY_DETECT_MODE_COMMAND                   0x20
#define TPS238X_TWO_EVENT_CLASSIFICATION_COMMAND             0x21
#define TPS238X_FOUR_PAIR_MODE_COMMAND                       0x22

#define TPS238X_CLASS_FIVE_TIMER_ENABLE_COMMAND              0x27

#define TPS238X_DISCONNECT_THRESHOLD_COMMAND                 0x29
#define TPS238X_ICUT21_CONFIGURATION_COMMAND                 0x2A
#define TPS238X_ICUT43_CONFIGURATION_COMMAND                 0x2B
#define TPS238X_TEMPERATURE_COMMAND                          0x2C

#define TPS238X_INPUT_VOLTAGE_COMMAND                        0x2E
#define TPS238X_INPUT_VOLTAGE_LSB_COMMAND                    0x2E
#define TPS238X_INPUT_VOLTAGE_MSB_COMMAND                    0x2F

#define TPS238X_PORT_1_CURRENT_COMMAND                       0x30
#define TPS238X_PORT_1_CURRENT_LSB_COMMAND                   0x30
#define TPS238X_PORT_1_CURRENT_MSB_COMMAND                   0x31

#define TPS238X_PORT_1_VOLTAGE_COMMAND                       0x32
#define TPS238X_PORT_1_VOLTAGE_LSB_COMMAND                   0x32
#define TPS238X_PORT_1_VOLTAGE_MSB_COMMAND                   0x33

#define TPS238X_PORT_2_CURRENT_COMMAND                       0x34
#define TPS238X_PORT_2_CURRENT_LSB_COMMAND                   0x34
#define TPS238X_PORT_2_CURRENT_MSB_COMMAND                   0x35

#define TPS238X_PORT_2_VOLTAGE_COMMAND                       0x36
#define TPS238X_PORT_2_VOLTAGE_LSB_COMMAND                   0x36
#define TPS238X_PORT_2_VOLTAGE_MSB_COMMAND                   0x37

#define TPS238X_PORT_3_CURRENT_COMMAND                       0x38
#define TPS238X_PORT_3_CURRENT_LSB_COMMAND                   0x38
#define TPS238X_PORT_3_CURRENT_MSB_COMMAND                   0x39

#define TPS238X_PORT_3_VOLTAGE_COMMAND                       0x3A
#define TPS238X_PORT_3_VOLTAGE_LSB_COMMAND                   0x3A
#define TPS238X_PORT_3_VOLTAGE_MSB_COMMAND                   0x3B

#define TPS238X_PORT_4_CURRENT_COMMAND                       0x3C
#define TPS238X_PORT_4_CURRENT_LSB_COMMAND                   0x3C
#define TPS238X_PORT_4_CURRENT_MSB_COMMAND                   0x3D

#define TPS238X_PORT_4_VOLTAGE_COMMAND                       0x3E
#define TPS238X_PORT_4_VOLTAGE_LSB_COMMAND                   0x3E
#define TPS238X_PORT_4_VOLTAGE_MSB_COMMAND                   0x3F

#define TPS238X_POE_PLUS_COMMAND                             0x40
#define TPS238X_FIRMWARE_REVISION_COMMAND                    0x41
#define TPS238X_I2C_WATCHDOG_COMMAND                         0x42
#define TPS238X_DEVICE_ID_COMMAND                            0x43

#define TPS238X_COOL_DOWN_GATE_DRIVE_COMMAND                 0x45

#define TPS238X_PORT_1_DETECT_RESISTANCE_COMMAND             0x60
#define TPS238X_PORT_1_DETECT_RESISTANCE_LSB_COMMAND         0x60
#define TPS238X_PORT_1_DETECT_RESISTANCE_MSB_COMMAND         0x61

#define TPS238X_PORT_2_DETECT_RESISTANCE_COMMAND             0x62
#define TPS238X_PORT_2_DETECT_RESISTANCE_LSB_COMMAND         0x62
#define TPS238X_PORT_2_DETECT_RESISTANCE_MSB_COMMAND         0x63

#define TPS238X_PORT_3_DETECT_RESISTANCE_COMMAND             0x64
#define TPS238X_PORT_3_DETECT_RESISTANCE_LSB_COMMAND         0x64
#define TPS238X_PORT_3_DETECT_RESISTANCE_MSB_COMMAND         0x65

#define TPS238X_PORT_4_DETECT_RESISTANCE_COMMAND             0x66
#define TPS238X_PORT_4_DETECT_RESISTANCE_LSB_COMMAND         0x66
#define TPS238X_PORT_4_DETECT_RESISTANCE_MSB_COMMAND         0x67

#define TPS238X_PORT_1_DETECT_VOLTAGE_DIFF_COMMAND           0x68
#define TPS238X_PORT_1_DETECT_VOLTAGE_DIFF_LSB_COMMAND       0x68
#define TPS238X_PORT_1_DETECT_VOLTAGE_DIFF_MSB_COMMAND       0x69

#define TPS238X_PORT_2_DETECT_VOLTAGE_DIFF_COMMAND           0x6A
#define TPS238X_PORT_2_DETECT_VOLTAGE_DIFF_LSB_COMMAND       0x6A
#define TPS238X_PORT_2_DETECT_VOLTAGE_DIFF_MSB_COMMAND       0x6B

#define TPS238X_PORT_3_DETECT_VOLTAGE_DIFF_COMMAND           0x6C
#define TPS238X_PORT_3_DETECT_VOLTAGE_DIFF_LSB_COMMAND       0x6C
#define TPS238X_PORT_3_DETECT_VOLTAGE_DIFF_MSB_COMMAND       0x6D

#define TPS238X_PORT_4_DETECT_VOLTAGE_DIFF_COMMAND           0x6E
#define TPS238X_PORT_4_DETECT_VOLTAGE_DIFF_LSB_COMMAND       0x6E
#define TPS238X_PORT_4_DETECT_VOLTAGE_DIFF_MSB_COMMAND       0x6F


/*************************************************************************************************************/
/*                              PROTOTYPES                                                                   */
/*************************************************************************************************************/

#ifdef __CPLUSPLUS
extern "C" {
#endif

uint8_t tps_RegisterPort (uint16_t device_i2c_address, TPS238x_PortNum_t devicePortNum);
uint16_t tps_GetDeviceI2CAddress (uint8_t systemPortNum);
TPS238x_PortNum_t tps_GetDevicePortNum (uint8_t systemPortNum);
uint8_t tps_GetSystemPortNumber (uint16_t deviceI2CAddress, TPS238x_PortNum_t devicePortNum);
uint8_t tps_SetI2CAddresses (uint8_t temp_i2cAddress, uint8_t numDevices, uint8_t *list_ofAddresses, TPS238x_On_Off_t *list_ofAutoMode);
uint8_t tps_SetDeviceInterruptMask (uint8_t device_i2c_address, TPS238X_Interrupt_Mask_Register_t intMask, uint8_t intDelayTime);
uint8_t tps_GetDeviceInterruptMask (uint8_t device_i2c_address, TPS238X_Interrupt_Mask_Register_t *intMask, uint8_t *intDelayTime);
uint8_t tps_GetDeviceInterruptStatus (uint8_t device_i2c_address, TPS238X_Interrupt_Register_t *status);
uint8_t tps_GetDeviceAllInterruptEvents (uint8_t device_i2c_address,
                                         TPS238x_On_Off_t clearEvent,
                                         TPS238x_Ports_t *powerEnablePortEvents,
                                         TPS238x_Ports_t *powerGoodPortEvents,
                                         TPS238x_Ports_t *detectionPortEvents,
                                         TPS238x_Ports_t *classificationPortEvents,
                                         TPS238x_Ports_t *icutPortEvents,
                                         TPS238x_Ports_t *disconnectPortEvents,
                                         TPS238x_Ports_t *inrushPortEvents,
                                         TPS238x_Ports_t *ilimPortEvents,
                                         TPS238X_Supply_Event_Register_t *supplyEvents);
uint8_t tps_GetPortDetectClassStatus (uint8_t systemPortNum, TPS238x_Detection_Status_t *detectionStatus,
                                      TPS238x_Classification_Status_t *classificationStatus);
uint8_t tps_GetPortDetectionStatus (uint8_t systemPortNum, TPS238x_Detection_Status_t *detectionStatus);
uint8_t tps_GetPortClassificationStatus (uint8_t systemPortNum, TPS238x_Classification_Status_t *classificationStatus);
uint8_t tps_GetDevicePowerStatus (uint8_t device_i2c_address, TPS238x_Ports_t *powerEnablePorts, TPS238x_Ports_t *powerGoodPorts);
uint8_t tps_GetDevicePowerEnableStatus (uint8_t device_i2c_address, TPS238x_Ports_t *powerEnablePorts);
uint8_t tps_GetPortPowerEnableStatus (uint8_t systemPortNum);
uint8_t tps_GetDevicePowerGoodStatus (uint8_t device_i2c_address, TPS238x_Ports_t *powerGoodPorts);
uint8_t tps_GetPortPowerGoodStatus (uint8_t systemPortNum);
uint8_t tps_SetDevicePowerOn (uint8_t device_i2c_address, TPS238x_Ports_t portsPoweredOn);
uint8_t tps_SetDevicePowerOff (uint8_t device_i2c_address, TPS238x_Ports_t portsPoweredOff);
uint8_t tps_SetPortPower (uint8_t systemPortNum, TPS238x_On_Off_t on_off);
uint8_t tps_GetDeviceDetectionEnable (uint8_t device_i2c_address, TPS238x_Ports_t *detectPorts);
uint8_t tps_GetPortDetectionEnable (uint8_t systemPortNum);
uint8_t tps_GetDeviceClassificationEnable (uint8_t device_i2c_address, TPS238x_Ports_t *classPorts);
uint8_t tps_GetPortClassificationEnable (uint8_t systemPortNum);
uint8_t tps_GetDeviceDetectClassEnable (uint8_t device_i2c_address, TPS238x_Ports_t *detectPorts, TPS238x_Ports_t *classPorts);
uint8_t tps_SetDeviceDetectClassEnable (uint8_t device_i2c_address, TPS238x_Ports_t detectPorts, TPS238x_Ports_t classPorts);
uint8_t tps_SetPortDetectClassEnable (uint8_t systemPortNum, TPS238x_On_Off_t on_off_detect, TPS238x_On_Off_t on_off_class);
uint8_t tps_GetDeviceDisconnectEnable (uint8_t device_i2c_address, TPS238x_Ports_t *disconnectPorts);
uint8_t tps_SetDeviceDisconnectEnable (uint8_t device_i2c_address, TPS238x_Ports_t disconnectPorts, TPS238x_Disconnect_Threshold_t disconnectThreshold1,
                                       TPS238x_Disconnect_Threshold_t disconnectThreshold2, TPS238x_Disconnect_Threshold_t disconnectThreshold3,
                                       TPS238x_Disconnect_Threshold_t disconnectThreshold4);
uint8_t tps_SetPortDisconnectEnable (uint8_t systemPortNum, TPS238x_On_Off_t on_off, TPS238x_Disconnect_Threshold_t disconnectThreshold);
uint8_t tps_SetDeviceTiming (uint8_t device_i2c_address, TPS238x_ILIM_Timing_t ilimTiming, TPS238x_TStart_Timing_t startTiming,
                             TPS238x_TICUT_Timing_t icutTiming, TPS238x_TDIS_Timing_t disconnectTiming,
                             TPS238x_Cool_Down_Timing_t coolDownFaultTiming);
uint8_t tps_FastShutdownDeviceEnable (uint8_t device_i2c_address, TPS238x_Ports_t ports);
uint8_t tps_FastShutdownPortEnable (uint8_t systemPortNum, TPS238x_On_Off_t on_off);
uint8_t tps_ConfigPort (uint8_t systemPortNum, TPS238x_Operating_Modes_t operatingMode, TPS238x_Legacy_Detect_t legacyDetect,
                        TPS238x_Two_Event_t twoEvent, TPS238x_On_Off_t class5Enable,
                        TPS238x_Disconnect_Threshold_t disconnectThreshold, TPS238x_ICUT_Config_t icutCurrentThreshold,
                        TPS238x_POE_Plus_Foldback_t poepFoldbackCurve);
uint8_t tps_ConfigDevice4Pair (uint8_t device_i2c_address, TPS238x_On_Off_t on_off_Port12, TPS238x_Four_Pair_t disconnectModePort12,
                               TPS238x_On_Off_t on_off_Port34, TPS238x_Four_Pair_t disconnectModePort34);
uint8_t tps_SetPortOpMode (uint8_t systemPortNum, TPS238x_Operating_Modes_t operatingMode);
uint8_t tps_SetDeviceOpMode (uint8_t device_i2c_address, TPS238x_Operating_Modes_t operatingMode1, TPS238x_Operating_Modes_t operatingMode2,
                             TPS238x_Operating_Modes_t operatingMode3, TPS238x_Operating_Modes_t operatingMode4);
uint8_t tps_RestartDeviceDetection (uint8_t device_i2c_address, TPS238x_Ports_t detectPorts);
uint8_t tps_RestartPortDetection (uint8_t systemPortNum);
uint8_t tps_RestartDeviceClassification (uint8_t device_i2c_address, TPS238x_Ports_t classPorts);
uint8_t tps_RestartPortClassification (uint8_t systemPortNum);
uint8_t tps_RestartDeviceDetectClass (uint8_t device_i2c_address, TPS238x_Ports_t detectPorts, TPS238x_Ports_t classPorts);
uint8_t tps_ResetDevicePort (uint8_t systemPortNum);
uint8_t tps_ResetPort (uint8_t systemPortNum);
uint8_t tps_GetPortMeasurements (uint8_t systemPortNum, uint16_t *voltage, uint16_t *current);
uint8_t tps_GetDeviceInputVoltage (uint8_t device_i2c_address, uint16_t *voltage);
uint8_t tps_GetDeviceTemperature (uint8_t device_i2c_address, uint8_t *temperature);
uint8_t tps_GetPortDetectResistance (uint8_t systemPortNum, uint16_t *detectResistance,
                                     TPS238x_Detect_Resistance_Status_t *detectResistanceStatus);
uint8_t tps_GetPortDetectVoltageDifference (uint8_t systemPortNum, uint16_t *detectVoltageDiff,
                                            TPS238x_Detect_Voltage_Difference_Status_t *detectVoltageDiffStatus);
uint8_t tps_ReleasePort (uint8_t systemPortNum);
uint8_t tps_ResetInterruptPin (uint8_t device_i2c_address);

uint8_t tps_SetPortICUT(uint8_t systemPortNum,TPS238x_ICUT_Config_t icutCurrentThreshold);
uint8_t tps_GetPortICUT(uint8_t systemPortNum);
uint8_t tps_SetPortILIM(uint8_t systemPortNum, TPS238x_POE_Plus_Foldback_t poepFoldbackCurve);
uint8_t tps_GetPortILIM(uint8_t systemPortNum);
uint8_t tps_SetDeviceTwoEventEnable(uint8_t device_i2c_address,TPS238x_Two_Event_t twoEvent1,TPS238x_Two_Event_t twoEvent2,TPS238x_Two_Event_t twoEvent3,TPS238x_Two_Event_t twoEvent4);
uint8_t tps_SetPortPoEP(uint8_t systemPortNum,TPS238x_POE_Plus_Foldback_t poepFoldbackCurve,TPS238x_ICUT_Config_t icutCurrentThreshold);
uint32_t tps_GetPortPower (uint8_t systemPortNum);


#ifdef __CPLUSPLUS
}
#endif

//User configurable variables
extern uint8_t i2cAddList[NUM_OF_TPS23861];

#endif /* __TPS238X_H_ */

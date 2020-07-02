/*
 * power_manage.h
 *
 *  Created on: Aug 24, 2016
 *
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
 *
**************************************************************************************************************************************************/







#ifndef POWER_MANAGE_H_
#define POWER_MANAGE_H_

#include "TPS23861.h"


#define PM_EN                   1
#define PM_POWER_BUDGET	        42000       //system power budget unit:mW
#define PM_CLASS0_POWER   		15400	 	//unit: mW;
#define PM_CLASS1_POWER   		4000 		//unit: mW;
#define PM_CLASS2_POWER   		7000 		//unit: mW;
#define PM_CLASS3_POWER   		15400 		//unit: mW;
#define PM_CLASS4_POWER   		30000 		//unit: mW;


typedef enum {
  Low			= 0x0,
  Normal        = 0x1,
  High			= 0x2
} PM_Priority_t;

typedef enum {
  PM_CHECK	    = 0x0,
  PM_OVERLIMIT	= 0x1,
  PM_POWERUP	= 0x2,
  PM_POWERDOWN	= 0x3
} PM_STATE_t;


extern uint8_t PM_powerMonitor;
extern uint8_t PM_setPriority[NUM_OF_TPS23861 * PM_NUM_OF_PORT];

uint32_t PM_calSysPower(void);
void PM_powerManagement(uint8_t PM_sysPortNumber);
void PM_monitorSysPower(void);
void PM_restartOffPortDetectionClassification(void);

#endif /* POWER_MANAGE_H_ */

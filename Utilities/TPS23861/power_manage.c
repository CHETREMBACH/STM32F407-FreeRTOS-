/*
 * power_manage.c
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



#include "power_manage.h"
#include "poe_cntrl.h"

/*************************************************************************************************************************************************
* PM_calSysPower
**************************************************************************************************************************************************/
/*!
* @brief Calculate current total consumed power of the system
*
* This function will calculate total consumed power of the system
*
* @param[in]   none

*
* @return  uint32_t system power
*
**************************************************************************************************************************************************/

uint32_t PM_calSysPower(void)
{
	uint8_t devNum, portNum,sysPortNum,portPG;
	uint32_t sysPower = 0;


	for(devNum = 0; devNum < NUM_OF_TPS23861; devNum++)
	{
		for(portNum = 0; portNum < PM_NUM_OF_PORT; portNum++)
		{
			sysPortNum = tps_GetSystemPortNumber((uint16_t)i2cAddList[devNum], (TPS238x_PortNum_t)(portNum + 1));

			portPG = tps_GetPortPowerGoodStatus(sysPortNum);

			if(portPG == TPS_ON)
			{
				sysPower = sysPower + tps_GetPortPower(sysPortNum);
			}

		}
	}

	return(sysPower);
}


uint8_t PM_highestPriorityPort(uint8_t *pPortPriority)
{
	uint8_t highestPriorityPort_S = 0;
	uint8_t devNum, portNum,sysPortNum;
	for(devNum = 0; devNum < NUM_OF_TPS23861; devNum++)
	{
		for(portNum = 0; portNum < PM_NUM_OF_PORT; portNum++)
		{
			sysPortNum = tps_GetSystemPortNumber(i2cAddList[devNum], (TPS238x_PortNum_t)(portNum + 1));

			if(pPortPriority[highestPriorityPort_S]< pPortPriority[sysPortNum])
			{
				highestPriorityPort_S = sysPortNum;
			}
		}
	}

	return (highestPriorityPort_S);
}



/*************************************************************************************************************************************************
* PM_getActLowestPrioPort
**************************************************************************************************************************************************/
/*!
* @brief Find the ON port which has the lowest priority among all ON ports
*
* This function will find the ON port which has the lowest priority among all ON ports
*
* @param[in]   none

*
* @return  uint8_t system port number
*
**************************************************************************************************************************************************/


uint8_t PM_getActLowestPrioPort(void)
{
	uint8_t devNum, portNum,sysPortNum,portPG;
	uint8_t lowestPrioPort = PM_highestPriorityPort(&PM_setPriority[0]);

	for(devNum = 0; devNum < NUM_OF_TPS23861; devNum++)
	{
		for(portNum = 0; portNum < PM_NUM_OF_PORT; portNum++)
		{
			sysPortNum = tps_GetSystemPortNumber(i2cAddList[devNum], (TPS238x_PortNum_t)(portNum + 1));

			portPG = tps_GetPortPowerGoodStatus(sysPortNum);

			if(portPG == TPS_ON)
			{
				if(PM_setPriority[sysPortNum] < PM_setPriority[lowestPrioPort])
				{
					lowestPrioPort = sysPortNum;
				}
			}
		}
	}

	return(lowestPrioPort);
}


/*************************************************************************************************************************************************
* PM_getClassHighestPrioPort
**************************************************************************************************************************************************/
/*!
* @brief Find the port which has the highest priority among all ports that finish classification and wait for power on
*
* This function will find the port which has the highest priority among all ports that finish classification and wait for power on
*
* @param[in]   system port number of the port which finishes classification and waits for PM's decision

*
* @return  uint8_t system port number of the highest priority port
*
**************************************************************************************************************************************************/

uint8_t PM_getClassHighestPrioPort(uint8_t PM_sysPortNumber)
{
	uint8_t devNum, portNum,sysPortNum,portPG;
	TPS238x_Classification_Status_t portClassStatus;
	uint8_t highestPrioPort = PM_sysPortNumber;
	for(devNum = 0; devNum < NUM_OF_TPS23861; devNum++)
	{
		for(portNum = 0; portNum < PM_NUM_OF_PORT; portNum++)
		{
			sysPortNum = tps_GetSystemPortNumber(i2cAddList[devNum], (TPS238x_PortNum_t)(portNum + 1));
			tps_GetPortClassificationStatus(sysPortNum,&portClassStatus);
			portPG = tps_GetPortPowerGoodStatus(sysPortNum);

			if((portPG != TPS_ON)&&
			   (portClassStatus != CLASS_OVERCURRENT) &&
			   (portClassStatus != CLASS_UNKNOWN) &&
			   (portClassStatus != CLASS_MISMATCH))
			{
				if(PM_setPriority[sysPortNum] > PM_setPriority[PM_sysPortNumber])
				{
					highestPrioPort = sysPortNum;
				}

			}

		}
	}

	return(highestPrioPort);
}


/*************************************************************************************************************************************************
* PM_getRequestPower
**************************************************************************************************************************************************/
/*!
* @brief Find the port which has the highest priority among all ports that finish classification and wait for power on
*
* This function will find the port which has the highest priority among all ports that finish classification and wait for power on
*
* @param[in]   system port number of the port which finishes classification and waits for PM's decision

*
* @return  uint8_t system port number of the highest priority port
*
**************************************************************************************************************************************************/

uint32_t PM_getRequestPower(uint8_t PM_sysPortNumber)
{
	TPS238x_Classification_Status_t classStatus;
	uint32_t portPowerEstimate;
	tps_GetPortClassificationStatus(PM_sysPortNumber, &classStatus);

	switch(classStatus)
	{

	    case CLASS_0: portPowerEstimate = PM_CLASS0_POWER; break;
	    case CLASS_1: portPowerEstimate = PM_CLASS1_POWER; break;
	    case CLASS_2: portPowerEstimate = PM_CLASS2_POWER; break;
	    case CLASS_3: portPowerEstimate = PM_CLASS3_POWER; break;
	    case CLASS_4: portPowerEstimate = PM_CLASS4_POWER; break;
	    default:      portPowerEstimate = PM_POWER_BUDGET; break;
	}
	return(portPowerEstimate);

}





/*************************************************************************************************************************************************
* PM_restartOffPortDetectionClassification
**************************************************************************************************************************************************/
/*!
* @brief Restart detection/classification of all OFF ports
*
* This function will Restart detection/classification of all OFF ports
*
* @param[in]   none

*
* @return  none
*
**************************************************************************************************************************************************/

void PM_restartOffPortDetectionClassification(void)

{
	uint8_t devNum;
	TPS238x_Ports_t PGood;
	TPS238x_Ports_t enablePort;
	for(devNum = 0;devNum < NUM_OF_TPS23861; devNum++)
	{
		tps_GetDevicePowerGoodStatus(i2cAddList[devNum],&PGood);

		enablePort =(~PGood);

		tps_RestartDeviceDetectClass(i2cAddList[devNum],enablePort,enablePort);
	}

}





/*************************************************************************************************************************************************
*  PM_getPowerofPortsHigherPriority
**************************************************************************************************************************************************/
/*!
* @brief Returns the total actual power of all ports that have higher priority than the current port
*
* This function will return the total actual power of all ports that have higher priority than the current port.
*
* @param[in]   PM_sysPortNumber        system port number of current port

*
* @return  uint32_t    total actual power of all ports that have higher priority than the current port

**************************************************************************************************************************************************/

uint32_t PM_getPowerofPortsHigherPriority(uint8_t PM_sysPortNumber)
{
	uint8_t devNum, portNum,sysPortNum,portPG;
	uint32_t powerofHigherPriority = 0;

	for(devNum = 0; devNum < NUM_OF_TPS23861; devNum++)
	{
		for(portNum = 0; portNum < PM_NUM_OF_PORT; portNum++)
		{
			sysPortNum = tps_GetSystemPortNumber(i2cAddList[devNum], (TPS238x_PortNum_t)(portNum + 1));

			portPG = tps_GetPortPowerGoodStatus(sysPortNum);

			if(portPG == TPS_ON)
			{
				if(PM_setPriority[sysPortNum] >= PM_setPriority[PM_sysPortNumber])
				{
					powerofHigherPriority = powerofHigherPriority + tps_GetPortPower(sysPortNum);
				}
			}

		}
	}

	return(powerofHigherPriority);
}




/*************************************************************************************************************************************************
* PM_monitorSysPower
**************************************************************************************************************************************************/
/*!
* @brief Real time check the total used system power. If it exceeds power budget, turn off the ports based on priority
*
* This function will real time check the total used system power. If it exceeds power budget, turn off the ports based on priority
*
* @param[in]   none

*
* @return  none
*
**************************************************************************************************************************************************/

void PM_monitorSysPower(void)
{
	uint32_t sysPower;
	uint8_t lowestPrioPort;
	uint8_t powerOffPort;

	sysPower = PM_calSysPower();

	lowestPrioPort = PM_getActLowestPrioPort();

	powerOffPort = lowestPrioPort;

	if(sysPower > PM_POWER_BUDGET)
	{
		tps_SetPortPower(powerOffPort, TPS_OFF);

		while((tps_GetPortPowerGoodStatus(powerOffPort)) == TPS_ON)
		{

		}

		printf("\n Power Off port From Minitor: ");
		printf("%d\n",(unsigned long)(powerOffPort));
		printf("\n System Power ");
		printf("%d\n",(unsigned long)(sysPower));

	}

}





/*************************************************************************************************************************************************
*  PM_getPowerofPortsHigherPriority
**************************************************************************************************************************************************/
/*!
* @brief Manage each port "turn on" decisions based on power and priority
*
* This function will manage each port "turn on" decisions based on power and priority
*
* @param[in]   PM_sysPortNumber        system port number of current port

*
* @return  none

**************************************************************************************************************************************************/

void PM_powerManagement(uint8_t PM_sysPortNumber)
{
	PM_STATE_t PM_MODE = PM_CHECK;
	uint32_t sysPower;
	uint32_t portPowerEstimate;
	uint8_t lowestPrioPort;
	uint8_t powerOffPort = 0xFF;
	uint8_t powerOnPort = 0xFF;
	TPS238x_Classification_Status_t classStatus;



	if(PM_MODE == PM_CHECK)
	{
		sysPower = PM_calSysPower();
		portPowerEstimate = PM_getRequestPower(PM_sysPortNumber);

		if((sysPower + portPowerEstimate) > PM_POWER_BUDGET)
		{
			PM_MODE = PM_OVERLIMIT;
		}

		else
		{
				PM_MODE = PM_POWERUP;
				powerOnPort = PM_sysPortNumber;
		}
	}

	if(PM_MODE == PM_OVERLIMIT)
	{
		lowestPrioPort = PM_getActLowestPrioPort();   //Find the turn on port with lowest priority

		//Check the total power of ports that have higer priority than current port.
		//If turning off ports that have lower priority can allocate enough power to current port, turn off ports.
		//Otherwise, don't turn off ports
		if((PM_getPowerofPortsHigherPriority(PM_sysPortNumber) + portPowerEstimate) <= PM_POWER_BUDGET)
		{
			PM_MODE = PM_POWERDOWN;

			powerOffPort = lowestPrioPort; //when the port needs to be powered on has the same priority with other ports, it depends on how the host loops the ports

			printf("\r\n");
			printf("\n Power Off port: ");
			printf("%d\n",(unsigned long)(powerOffPort));
			printf("\n System Power ");
			printf("%d\n",(unsigned long)(sysPower));
			printf("\r\n");


		}
		else
		{
			PM_MODE = PM_CHECK;
		}
	}

	if(PM_MODE == PM_POWERDOWN)
	{
		tps_SetPortPower(powerOffPort, TPS_OFF);

		while((tps_GetPortPowerGoodStatus(powerOffPort)) == TPS_ON)
		{

		}

		PM_MODE = PM_CHECK;
	}

	if(PM_MODE == PM_POWERUP)
	{
		tps_GetPortClassificationStatus(powerOnPort, &classStatus);

		if(classStatus == CLASS_4)
		{
			tps_SetPortPoEP(powerOnPort, _2X_ILIM_FOLDBACK_CURVE, ICUT_686_MILLIAMP);
		}
		else
		{
			tps_SetPortPoEP(powerOnPort, _1X_ILIM_FOLDBACK_CURVE, ICUT_374_MILLIAMP);
		}

#if (NORMAL_OPERATING == 1)
		tps_SetPortPower(powerOnPort,TPS_ON);
#endif

#if (VMARK_WORKAROUND == 1)
		tps_SetPortOpMode(powerOnPort,OPERATING_MODE_AUTO);

		while((tps_GetPortPowerEnableStatus(powerOnPort)) == TPS_OFF)
		{

		}


		tps_SetPortOpMode(powerOnPort,OPERATING_MODE_SEMI_AUTO);

#endif


		printf("\r\n");
		printf("\n Power On port: ");
		printf("%d\n",(unsigned long)(powerOnPort));
		printf("\r\n");
		printf("\n System Power ");
		printf("%d\n",(unsigned long)(sysPower));
		printf("\r\n");

		PM_MODE = PM_CHECK;
	}



}

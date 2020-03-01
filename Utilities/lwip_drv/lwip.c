/**
 ******************************************************************************
  * File Name          : LWIP.c
  * Description        : This file provides initialization code for LWIP
  *                      middleWare.
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
#include "lwip.h"
#include "lwip/init.h"
#include "lwip/netif.h"

/* Private function prototypes -----------------------------------------------*/
/* ETH Variables initialization ----------------------------------------------*/
void Error_Handler(void);

/* Variables Initialization */
Eth_CntrlTypeDef cntrl_netif;

/**
  * @brief  Check the ETH link state and update netif accordingly.
  * @param  argument: netif
  * @retval None
  */
void ethernet_link_thread(void * pvParameters)
{
	struct netif *netif = (struct netif *) pvParameters;
	
	uint32_t regvalue = 0;
	
	cntrl_netif.CntNoLink = 0; 
	cntrl_netif.TimeLink = 0;
	
	if ((regvalue & PHY_LINKED_STATUS) != PHY_LINKED_STATUS)
	{
		netif_set_down(netif);
		netif_set_link_down(netif);
		cntrl_netif.linkUp = false;	
		cntrl_netif.TimeLink = 0;
	}
	else 
	{
		netif_set_up(netif);
		netif_set_link_up(netif);
		cntrl_netif.linkUp = true;				
	}	

	
	for (;;)
	{
		HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &regvalue);
		
		if ((regvalue & PHY_LINKED_STATUS) != PHY_LINKED_STATUS)
		{
			if (cntrl_netif.linkUp != false)
			{
				netif_set_down(netif);
				netif_set_link_down(netif);
				cntrl_netif.linkUp = false;	
				cntrl_netif.CntNoLink++; 
				cntrl_netif.TimeLink = 0;
			}
		}
		else 
		{
			if (cntrl_netif.linkUp != true)
			{
				netif_set_up(netif);
				netif_set_link_up(netif);
				cntrl_netif.linkUp = true;				
			}
			else
			{
				cntrl_netif.TimeLink += 100; 
			}
		}
		vTaskDelay(100);
	}
}

/**
  * LwIP initialization function
  */
void LWIP_Init(void)
{
	cntrl_netif.AutoNegotiation = ETH_AUTONEGOTIATION_ENABLE;
	cntrl_netif.Speed = ETH_SPEED_100M;
	cntrl_netif.DuplexMode = ETH_MODE_FULLDUPLEX;	
	
	cntrl_netif.MACAddr[0] = 0x00;
	cntrl_netif.MACAddr[1] = 0x80;	
	cntrl_netif.MACAddr[2] = 0xE1;	
	cntrl_netif.MACAddr[3] = 0x00;
	cntrl_netif.MACAddr[4] = 0x00;	
	cntrl_netif.MACAddr[5] = 0x01;		
		
	/* Initilialize the LwIP stack with RTOS */
	tcpip_init(NULL, NULL);
    		
	/* IP addresses initialization without DHCP (IPv4) */
	IP4_ADDR(&(cntrl_netif.ipaddr), 192, 168, 1, 231);
	IP4_ADDR(&(cntrl_netif.netmask), 255, 255, 255, 255);
	IP4_ADDR(&(cntrl_netif.gw), 192, 168, 1, 1);

	/* add the network interface (IPv4/IPv6) with RTOS */
	netif_add(&(cntrl_netif.gnetif), &(cntrl_netif.ipaddr), &(cntrl_netif.netmask), &(cntrl_netif.gw), NULL, &ethernetif_init, &tcpip_input);

	/* Registers the default network interface */
	netif_set_default(&(cntrl_netif.gnetif));
	
	//ethernetif_update_config(&(cntrl_netif.gnetif));	
	
	/* Set the link callback function, this function is called on change of link status */
	netif_set_link_callback(&(cntrl_netif.gnetif), ethernetif_update_config);	

	if (netif_is_link_up(&(cntrl_netif.gnetif)))
	{
		/* When the netif is fully configured this function must be called */
		netif_set_up(&(cntrl_netif.gnetif));
	}
	else
	{
		/* When the netif link is down this function must be called */
		netif_set_down(&(cntrl_netif.gnetif));
	}
	
	/* create the task that link control */
	xTaskCreate(ethernet_link_thread, (const char*)"Link_if", configMINIMAL_STACK_SIZE * 1.5, &(cntrl_netif.gnetif), LwIP_THREAD_PRIO, NULL);		
}


#if LWIP_NETIF_LINK_CALLBACK
/**
  * @brief  Link callback function, this function is called on change of link status
  *         to update low level driver configuration.
* @param  netif: The network interface
  * @retval None
  */
void ethernetif_update_config(struct netif *netif)
{
	__IO uint32_t tickstart = 0;
	uint32_t regvalue = 0;
  
	if (netif_is_link_up(netif))
	{ 
		/* Restart the auto-negotiation */
		if (heth.Init.AutoNegotiation != ETH_AUTONEGOTIATION_DISABLE)
		{
			/* Enable Auto-Negotiation */
			HAL_ETH_WritePHYRegister(&heth, PHY_BCR, PHY_AUTONEGOTIATION);
      
			/* Get tick */
			tickstart = HAL_GetTick();
      
			/* Wait until the auto-negotiation will be completed */
			do
			{
				HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &regvalue);
        
				/* Check for the Timeout ( 1s ) */
				if ((HAL_GetTick() - tickstart) > 1000)
				{     
					/* In case of timeout */ 
					goto error;
				}   
			} while (((regvalue & PHY_AUTONEGO_COMPLETE) != PHY_AUTONEGO_COMPLETE));
      
			/* Read the result of the auto-negotiation */
			HAL_ETH_ReadPHYRegister(&heth, PHY_SR, &regvalue);
      
			/* Configure the MAC with the Duplex Mode fixed by the auto-negotiation process */
			if ((regvalue & PHY_DUPLEX_STATUS) != (uint32_t)RESET)
			{
				/* Set Ethernet duplex mode to Full-duplex following the auto-negotiation */
				heth.Init.DuplexMode = ETH_MODE_FULLDUPLEX;  
			}
			else
			{
				/* Set Ethernet duplex mode to Half-duplex following the auto-negotiation */
				heth.Init.DuplexMode = ETH_MODE_HALFDUPLEX;           
			}
			/* Configure the MAC with the speed fixed by the auto-negotiation process */
			if (regvalue & PHY_SPEED_STATUS)
			{  
				/* Set Ethernet speed to 10M following the auto-negotiation */
				heth.Init.Speed = ETH_SPEED_10M; 
			}
			else
			{   
				/* Set Ethernet speed to 100M following the auto-negotiation */ 
				heth.Init.Speed = ETH_SPEED_100M;
			}
		}
		else /* AutoNegotiation Disable */
		{
error :
			/* Check parameters */
			assert_param(IS_ETH_SPEED(heth.Init.Speed));
			assert_param(IS_ETH_DUPLEX_MODE(heth.Init.DuplexMode));
      
			/* Set MAC Speed and Duplex Mode to PHY */
			HAL_ETH_WritePHYRegister(&heth,
				PHY_BCR,
				((uint16_t)(heth.Init.DuplexMode >> 3) |
			                                               (uint16_t)(heth.Init.Speed >> 1))); 
		}

		/* ETHERNET MAC Re-Configuration */
		HAL_ETH_ConfigMAC(&heth, (ETH_MACInitTypeDef *) NULL);

		/* Restart MAC interface */
		HAL_ETH_Start(&heth);   
	}
	else
	{
		/* Stop MAC interface */
		HAL_ETH_Stop(&heth);
	}

	ethernetif_notify_conn_changed(netif);
}

/**
  * @brief  This function notify user about link status changement.
  * @param  netif: the network interface
  * @retval None
  */
__weak void ethernetif_notify_conn_changed(struct netif *netif)
{
	/* NOTE : This is function could be implemented in user file 
	          when the callback is needed,
	        */

}
#endif /* LWIP_NETIF_LINK_CALLBACK */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

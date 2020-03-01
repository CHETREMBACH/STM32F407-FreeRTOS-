/**
  ******************************************************************************
  * File Name          : LWIP.h
  * Description        : This file provides code for the configuration
  *                      of the LWIP.
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
  *************************************************************************  

  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __mx_lwip_H
#define __mx_lwip_H
#ifdef __cplusplus
extern "C" {
#endif

	/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "ethernetif.h"
	 
	/* Includes for RTOS ---------------------------------------------------------*/
#if WITH_RTOS
#include "lwip/tcpip.h"
#endif /* WITH_RTOS */

	/* Global Variables ----------------------------------------------------------*/
	extern ETH_HandleTypeDef heth;

	/* LWIP init function */	
	void LWIP_Init(void);

	/** 
	  * @brief  ETH Control structure 
	  */ 
	typedef struct 
	{
		ip4_addr_t         ipaddr; /* ipaddr the new IP address    */
		ip4_addr_t         netmask; /* netmask the new netmask      */
		ip4_addr_t         gw; /* gw the new default gateway   */
		uint8_t            MACAddr[6]; /* MAC Address                  */
	                                                                      
		uint32_t           AutoNegotiation; /* autoNegotiation mode         */
		uint32_t           Speed; /* ethernet speed: 10/100 Mbps  */
		uint32_t           DuplexMode; /* MAC duplex mode              */	                                      
                                                                                
		struct netif       gnetif;           /* network interface structure  */ 
		ETH_HandleTypeDef  EthHandle; /* Global Ethernet handle       */
                                                                                
		bool               Flag_HTML_Init; /* status init HTML             */ 
		bool               linkUp; /* status link                  */
		uint8_t            CntNoLink; /* counter re link              */
		uint32_t           TimeLink; /* time work link               */ 
	}Eth_CntrlTypeDef;

	/* Variables Initialization */
	extern Eth_CntrlTypeDef cntrl_netif;	 
	 
#ifdef __cplusplus
}
#endif
#endif /*__ mx_lwip_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

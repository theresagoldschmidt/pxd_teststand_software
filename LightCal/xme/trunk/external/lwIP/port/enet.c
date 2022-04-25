/*
 * Copyright (c) 2011-2012, fortiss GmbH.
 * Licensed under the Apache License, Version 2.0.
 *
 * Use, modification and distribution are subject to the terms specified
 * in the accompanying license file LICENSE.txt located at the root directory
 * of this software distribution. A copy is available at
 * http://chromosome.fortiss.org/.
 *
 * This file is part of CHROMOSOME.
 *
 * $Id$
 */

/**
 * \file
 *         High-level ethernet functinos
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 */

/* Retained copyright */
//*****************************************************************************
//
// enet.c - lwIP Intialization and Application run-time code for static IP.
//
// Copyright (c) 2006-2011 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 6852 of the EK-LM3S8962 Firmware Package.
//
//*****************************************************************************

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdint.h>
#include "lwiplib.h"

#include "enet.h"
#include "bsp/enet_arch.h"
#include "network_settings.h"


/******************************************************************************/
/***   Global variables                                                     ***/
/******************************************************************************/
// Flag to indicate if Ethernet controller has been initialized.
#if NO_SYS
static volatile uint8_t g_bEnetInit = 0;
#endif

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
#if MAC_ADDR_FIRMWARE
void Get_MAC_Address (eth_mac_addr_t macAddress) {
	// In this implementation, the MAC address is compiled into the firmware.
	macAddress[0] = MAC_ADDR0;
	macAddress[1] = MAC_ADDR1;
	macAddress[2] = MAC_ADDR2;
	macAddress[3] = MAC_ADDR3;
	macAddress[4] = MAC_ADDR4;
	macAddress[5] = MAC_ADDR5;
}
#endif // #if MAC_ADDR_FIRMWARE

/// Returns the current IP address.
unsigned long
EnetGetIPAddr(void)
{
	// Return the current IP address.
	return(lwIPLocalIPAddrGet());
}

#if !GET_MAC_ADDR_FROM_BOOTLOADER
void EnetInit(void)
#else
void EnetInit(eth_mac_addr_t macAddress)
#endif // #if !GET_MAC_ADDR_FROM_BOOTLOADER
{
#if !GET_MAC_ADDR_FROM_BOOTLOADER
	eth_mac_addr_t macAddress;
#endif // #if !GET_MAC_ADDR_FROM_BOOTLOADER

	// Initialize Ethernet peripheral
	ETH_driver_init();

#if !GET_MAC_ADDR_FROM_BOOTLOADER
	// Retrieve MAC address (e.g., from EEPROM, hard coded, etc.)
	Get_MAC_Address(macAddress);
#endif // #if !GET_MAC_ADDR_FROM_BOOTLOADER

    // Program the MAC address into the Ethernet controller.
	Set_MAC_Address(macAddress);

	// Initialize LWIP stack
	lwIPInit(macAddress,
			(IP_ADDR0 << 24) + (IP_ADDR1 << 16) + (IP_ADDR2 << 8) + (IP_ADDR3 << 0),
			(NETMASK_ADDR0 << 24) + (NETMASK_ADDR1 << 16) + (NETMASK_ADDR2 << 8) + (NETMASK_ADDR3 << 0),
			(GW_ADDR0 << 24) + (GW_ADDR1 << 16) + (GW_ADDR2 << 8) + (GW_ADDR3 << 0),
			IPMODE);

	//
	// Initialization has been completed.
	//
#if NO_SYS
	g_bEnetInit = 1;
#endif
}
/// Application runtime code that should be called periodically to run the
/// varios TCP/IP stack timers.
#if NO_SYS
void
EnetTick(unsigned long ulTickMS)
{
	// Check to see if we have been initialized.
	if(!g_bEnetInit)
	{
		return;
	}

	// Call the lwip Library tick handler.
	lwIPTimer(ulTickMS);
}
#endif

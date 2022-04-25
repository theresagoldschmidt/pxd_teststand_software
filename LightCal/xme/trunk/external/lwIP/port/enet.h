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
 *         Cortex-M3 high-level ethernet functinos
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 */
 
 /* Retained copyright */
//*****************************************************************************
//
// enet.h - Ethernet Definitions/Prototypes.
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

#ifndef LWIP_PORT_COMMON_ENET_H
#define LWIP_PORT_COMMON_ENET_H
/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdint.h>
#include "network_settings.h"

/******************************************************************************/
/***   Typedefs                                                             ***/
/******************************************************************************/
typedef enum {
	IPADDR_STATIC = 0,
	IPADDR_DHCP = 1,
	IPADDR_AUTOIP = 2,
} ip_addr_config_mode_t;

typedef uint8_t eth_mac_addr_t[6];

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
#if !GET_MAC_ADDR_FROM_BOOTLOADER
void EnetInit(void);
#else
void EnetInit(eth_mac_addr_t macAddress);
#endif // #if !GET_MAC_ADDR_FROM_BOOTLOADER

void EnetTick(unsigned long ulTickMS);
unsigned long EnetGetIPAddr(void);

#endif // LWIP_PORT_COMMON_ENET_H

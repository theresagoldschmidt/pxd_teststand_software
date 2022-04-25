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
 *        LM3s8962 specific ethernet functions.
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

#ifndef LWIP_PORT_LM3S8962_ENET_H
#define LWIP_PORT_LM3S8962_ENET_H
/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "enet.h"

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
void ETH_driver_init(void);

void Get_MAC_address(eth_mac_addr_t macAddress);
void Set_MAC_address(eth_mac_addr_t macAddress);

#endif // LWIP_PORT_LM3S8962_ENET_H

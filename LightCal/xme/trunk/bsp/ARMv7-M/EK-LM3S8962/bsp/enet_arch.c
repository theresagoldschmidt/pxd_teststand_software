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
 *         LM3s8962 specific ethernet functions.
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

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/ethernet.h"
#include "driverlib/flash.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "utils/locator.h"
#include "utils/lwiplib.h"
#include "utils/ustdlib.h"
#include "enet_arch.h"

void ETH_driver_init() {
	//
	// Enable and Reset the Ethernet Controller.
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ETH);
	SysCtlPeripheralReset(SYSCTL_PERIPH_ETH);

	//
	// Enable Port F for Ethernet LEDs.
	//  LED0        Bit 3   Output
	//  LED1        Bit 2   Output
	//
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_2 | GPIO_PIN_3);
}

void Get_MAC_Address (eth_mac_addr_t macAddress) {
	unsigned long ulUser0, ulUser1;

	//
	// Configure the hardware MAC address for Ethernet Controller
	// filtering of incoming packets.
	//
	// For the LM3S6965 Evaluation Kit, the MAC address will be stored in the
	// non-volatile USER0 and USER1 registers.  These registers can be read
	// using the FlashUserGet function, as illustrated below.
	//
    FlashUserGet(&ulUser0, &ulUser1);

    //
    // Convert the 24/24 split MAC address from NV ram into a 32/16 split
    // MAC address needed to program the hardware registers, then program
    // the MAC address into the Ethernet Controller registers.
    //
    macAddress[0] = ((ulUser0 >>  0) & 0xff);
    macAddress[1] = ((ulUser0 >>  8) & 0xff);
    macAddress[2] = ((ulUser0 >> 16) & 0xff);
    macAddress[3] = ((ulUser1 >>  0) & 0xff);
    macAddress[4] = ((ulUser1 >>  8) & 0xff);
    macAddress[5] = ((ulUser1 >> 16) & 0xff);
}

void Set_MAC_Address (eth_mac_addr_t macAddress) {

	// Program the MAC address into the Ethernet controller.
	EthernetMACAddrSet(ETH_BASE, macAddress);
}

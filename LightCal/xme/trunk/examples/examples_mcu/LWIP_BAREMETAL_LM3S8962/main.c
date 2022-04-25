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
 */

/**
 * \file
 *         Example for barematel usage of lwIP library with luminary boards.
 *
 * \author
 *		   Hauke Stähle <staehle@fortiss.org>
 */

#include "bsp/pins_leds.h"
#include "xme/hal/dio_arch.h"

#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "enet.h"

#include "utils/lwiplib.h"
#include "xme/hal/console.h"

// System clock speed
unsigned long g_ulSystemClock;

// Please specify destination IP address here
#define DST_IP_1 192
#define DST_IP_2 168
#define DST_IP_3 17
#define DST_IP_4 2

// Please specify destination port here
#define DST_PORT 3333

// Rate for SysTick interrupt
#define CLOCK_RATE 300

int
main(void)
{
	uint8_t ip1, ip2, ip3, ip4;

    // Set the clocking to run at 50MHz from the PLL.
    SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_8MHZ);
    SysCtlPWMClockSet(SYSCTL_PWMDIV_8);

    // Get the system clock speed.
    g_ulSystemClock = SysCtlClockGet();

	// Initialize the LwIP stack.
	EnetInit();

    // Configure SysTick to periodically interrupt.
    SysTickPeriodSet(g_ulSystemClock / CLOCK_RATE);
    SysTickIntEnable();
    SysTickEnable();

    xme_hal_console_init();

    {
    	// Buffer for Data to send
    	struct pbuf *p;
    	// Pointer to access buffer data
    	unsigned char *pucData;
    	// Storage for destination IP address
    	struct ip_addr addr;
    	// Destination port
    	u16_t port;
    	// PCB for udp packet
    	struct udp_pcb *pcb;

    	// Generate new PCB for udp packet
    	pcb = udp_new();

    	// Allocate memory to store data to send
    	p = pbuf_alloc(PBUF_RAW, sizeof(char) * 5, PBUF_RAM);

    	// Write "HELLO" into buffer
    	pucData = p->payload;
    	pucData[0] = 'H'; pucData[1] = 'E';	pucData[2] = 'L'; pucData[3] = 'L';	pucData[4] = 'O';

    	// Set the destination IP address
    	ip1 = DST_IP_1; ip2 = DST_IP_2; ip3 = DST_IP_3; ip4 = DST_IP_4;
    	IP4_ADDR( &addr, ip1, ip2, ip3, ip4 );

    	// Set the destination port
    	port = DST_PORT;

    	// Write some status information
    	xme_hal_console_string("System initialized.\nSending UDP packets.\n\n");
    	xme_hal_console_string("Destination port:\n");
    	xme_hal_console_uint16_d( port );
    	xme_hal_console_string("\nDestination IP:\n");
    	xme_hal_console_uint8_d( ip1 );
    	xme_hal_console_string(".");
    	xme_hal_console_uint8_d( ip2 );
    	xme_hal_console_string(".");
    	xme_hal_console_uint8_d( ip3 );
    	xme_hal_console_string(".");
    	xme_hal_console_uint8_d( ip4 );
    	xme_hal_console_string("\n");

    	//
		// Loop forever.
		//
		while(1)
		{
			int x = 0;

			// Send the packet via the lwIP stack
			udp_sendto(pcb, p, &addr, port);

			// Wait a little bit before sending the next packet
			for ( x = 0; x<200000; x++);
		}
    }
}

//
// Handler for system tick
//
void
SysTickIntHandler(void)
{
    //
    // Run the Ethernet handler.
    //
    EnetTick(10);
}

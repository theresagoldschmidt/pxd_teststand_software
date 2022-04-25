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
 *         Example for FreeRTOS usage of lwIP library
 *         with luminary boards.
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

#include "xme/hal/net.h"

#include "xme/core/componentList.h"
#include "xme/core/core.h"
#include "xme/core/defines.h"
#include "xme/core/nodeManager.h"
#include "xme/core/interfaceManager.h"
#include "xme/core/broker.h"

// Please specify destination port here
// The program will send a broadcast packet to that port periodically.
// A broadcast packet is an UDP packet with a destination address of
// 255.255.255.255.
#define DST_PORT 3333

// Prototype for function sending the broadcast packets.
void udpsender(void *);

// A small web server will start with the program as well. The response
// of the web server is stored in the buffer indexdata.
const static char indexdata[] =
"<html> \
		<head><title>A test page</title></head> \
		<body> \
		This is a small test page. \
		</body> \
		</html>";

// The buffer for the initial request for the webpage. A browser will
// send a specific message over a TCP connection which includes the file
// the browser wants to load. That request is stored in the buffer buf.
#define BUF_SIZE 4096
char buf[BUF_SIZE];

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define XME_CORE_NODEMANAGER_DEVICE_TYPE ((xme_core_device_type_t)0x0000000000111112)
#define XME_CORE_NODEMANAGER_DEVICE_GUID ((xme_core_device_guid_t)0x0000000000211114)

/******************************************************************************/
/***   Component configurations                                             ***/
/******************************************************************************/
XME_COMPONENT_CONFIG_INSTANCE(xme_core_nodeManager) =
{
	// public
	XME_CORE_NODEMANAGER_DEVICE_TYPE, // deviceType
	XME_CORE_NODEMANAGER_DEVICE_GUID // deviceGuid
};

/******************************************************************************/
/***   Component descriptor                                                 ***/
/******************************************************************************/
XME_COMPONENT_LIST_BEGIN
	XME_COMPONENT_LIST_ITEM(xme_core_nodeManager, 0)
	XME_COMPONENT_LIST_EMPTY
	XME_COMPONENT_LIST_EMPTY
	XME_COMPONENT_LIST_EMPTY
	XME_COMPONENT_LIST_EMPTY
	XME_COMPONENT_LIST_EMPTY
	XME_COMPONENT_LIST_EMPTY
	XME_COMPONENT_LIST_EMPTY
	XME_COMPONENT_LIST_EMPTY
	XME_COMPONENT_LIST_EMPTY
XME_COMPONENT_LIST_END;

// This function processes an incomming connection for the webserver.
// This includes reading from the TCP socket the browser conencted
// to, checking if it a request for a webpage and finally sending
// the small example webpage stored in indexdata.
static void
process_connection(xme_hal_net_socketHandle_t conn)
{

	uint16_t bRead;

	/* Read data from the connection */
	bRead = xme_hal_net_readSocket(conn, buf, BUF_SIZE);

	/* Check if the request was an HTTP "GET /\r\n". */
	if(buf[0] == 'G' && buf[1] == 'E' &&
			buf[2] == 'T' && buf[3] == ' ' &&
			buf[4] == '/' ) {
		/* Send the actual web page. */
		xme_hal_net_writeSocket(conn, indexdata, sizeof(indexdata));
		/* Close the connection. */
		xme_hal_net_destroySocket(conn);
	}
}

// The webserver.
// In this function, a socket is created and bind to port number 80,
// which is used for the transfer of webpages. After that, this function
// waits for a connection on that port and processes it.
void
webserver(void *param)
{
	xme_hal_net_socketHandle_t conn, newconn;

	/* Create a new socket */
	conn = xme_hal_net_createSocket(
				NULL,
				XME_HAL_NET_SOCKET_TCP,
				NULL,
				80);

	if ( conn == XME_HAL_NET_INVALID_SOCKET_HANDLE )
	{
		xme_hal_console_string("Create socket error (2)\n");
		while (1);
	}

	// Open socket
	if ( xme_hal_net_openSocket( conn ) != XME_CORE_STATUS_SUCCESS )
	{
		xme_hal_console_string("Open socket error (2)\n");
		while (1);
	}

	/* Loop forever. */
	while(1) {

		/* Accept a new connection. */
		newconn = xme_hal_net_waitForConnection(conn);

		if ( newconn == XME_HAL_NET_INVALID_SOCKET_HANDLE )
		{
			xme_hal_console_string("Accept socket error (2)\n");
		}

		// Print a dot everytime the webpage is accessed
		xme_hal_console_string(".");

		/* Process the incomming connection. */
		process_connection(newconn);

		/* Deallocate connection handle. */
		xme_hal_net_destroySocket(newconn);
	}
}

int
main(void)
{
	uint16_t p;

	xme_core_init();

	// Set the clocking to run at 50MHz from the PLL.
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
			SYSCTL_XTAL_8MHZ);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_8);

	// Initialize the LwIP.
	// TODO: Should be part of the interface abstraction. See ticket #816
	EnetInit(0);

	// Initialize console
	xme_hal_console_init();

	// Destination port
	p = DST_PORT;

	// Write some status information
	xme_hal_console_string("System initialized.\nSending UDP packets.\n\n");
	xme_hal_console_string("Destination port:\n");
	xme_hal_console_uint16_d( p );
	xme_hal_console_string("\nDestination IP:\n");
	xme_hal_console_string("255.255.255.255\n" );

	// Create one task for the web server and one for sending periodic udp packets
	xTaskCreate( webserver, "Web", 256, NULL, 0, NULL );
	xTaskCreate( udpsender, "UDP", 256, NULL, 0, NULL );

	// Start FreeRTOS scheduler
	// TODO: Should be a XME call. See ticket #817
	vTaskStartScheduler();
}

// This is a simple example function how to send broadcast packets. First,
// a socket with broadcast ability is created. In an infinite loop, a broadcast
// packet is then send to the specified port.
void
udpsender(void *a)
{
	(void)a;
	xme_hal_net_socketHandle_t sock;

	// Create socket
	sock = xme_hal_net_createSocket(
			NULL,
			XME_HAL_NET_SOCKET_UDP | XME_HAL_NET_SOCKET_BROADCAST,
			NULL,
			DST_PORT);

	if ( sock == XME_HAL_NET_INVALID_SOCKET_HANDLE )
	{
		xme_hal_console_string("Create socket error\n");
		while (1);
	}

	// Open socket
	if ( xme_hal_net_openSocket( sock ) != XME_CORE_STATUS_SUCCESS )
	{
		xme_hal_console_string("Open socket error\n");
		while (1);
	}

	while (1)
	{
		// Send an UDP broadcast packet
		if (
				xme_hal_net_writeSocket(
				sock,
				"Hallo",
				6) == 0 )
		{
			xme_hal_console_string("Write socket error\n");
		}

		vTaskDelay(1000);
	}
}







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
 *         Network communication testsuite slave.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *
 * \note
 *         This file implements a process that is lauched as a subprocess of
 *         the main network communication testsuite. This process serves as
 *         a communication partner for the various testcases.
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "tests/tests.h"
#include "tests/core/test_core.h"

#include "xme/core/componentList.h"

#include "xme/hal/net.h"
#include "xme/hal/sleep.h"

#include <stdlib.h>
#include <string.h>

// TODO: See ticket #536
xme_hal_net_interfaceDescr_t intf =
{
	0
};

// TODO: See ticket #536
xme_core_interface_address_t intfAddress =
{
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

/******************************************************************************/
/***   Component descriptor                                                 ***/
/******************************************************************************/
XME_COMPONENT_LIST_BEGIN
	XME_COMPONENT_LIST_EMPTY
XME_COMPONENT_LIST_END;

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
int
xme_tests_hal_net_slave(int argc, char* argv[])
{
	// Arguments: <port> [ <read-timeout> <expected-request> [ <response-delay> <response-to-send> ] ]
	//
	// This testsuite listens for an incoming connection on the given port.
	// If the <expected-request> argument is specified it expects the
	// given request to be received once connected (maximum length: 255 bytes).
	// If the request is received successfully and the <response-to-send>
	// argument is specified, that response is sent to the peer.
	// After that, the function returns.

#define RECEIVE_BUFFER_SIZE 256

	xme_hal_net_socketHandle_t s;
	char receiveBuffer[RECEIVE_BUFFER_SIZE];
	uint16_t receiveBufferPos;
	uint16_t requestLength;
	const char* protocol;
	uint16_t socketType;
	uint16_t port;
	int requestTimeout;
	const char* request;
	int responseDelay;
	const char* response;
	xme_hal_net_socketHandle_t clientConnection;

	XME_TESTSUITE_BEGIN();

	XME_TEST_MSG_REC
	(
		argc >= 2 && argc <= 6 && argc != 3 && argc != 5,
		XME_TESTSUITE_STOP(),
		"Fatal: Inappropriate number of command line arguments!"
	);

	// Read input parameters
	protocol = argv[0];
	XME_TEST_MSG_REC
	(
		!(strcmp(protocol, "udp") && strcmp(protocol, "tcp")),
		XME_TESTSUITE_STOP(),
		"Fatal: First parameter must be either \"udp\" or \"tcp\"!"
	);
	socketType = (strcmp(protocol, "udp") ? XME_HAL_NET_SOCKET_TCP : XME_HAL_NET_SOCKET_UDP);
	port = atoi(argv[1]);
	requestTimeout = (argc > 2) ? atoi(argv[2]) : 0;
	request = (argc > 2) ? argv[3] : 0;
	responseDelay = (argc > 4) ? atoi(argv[4]) : 0;
	response = (argc > 4) ? argv[5] : 0;

	printf("[server] Configured to listen on %s port %u\n", (XME_HAL_NET_SOCKET_TCP & socketType) ? "TCP" : "UDP", port);

	XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_initInterface(&intfAddress));

	// Create a socket bound to the given port
	printf("[server] Creating a local socket bound to port %u...\n", port);
	XME_TEST(XME_HAL_NET_INVALID_SOCKET_HANDLE != (s = xme_hal_net_createSocket(&intf, socketType | XME_HAL_NET_SOCKET_NONBLOCKING, 0, port)));
	XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_openSocket(s));

	// Check whether we should receive something
	requestLength = strlen(request);
	if (requestLength > 0)
	{
		uint16_t time;

		if (XME_HAL_NET_SOCKET_TCP == socketType)
		{
			clientConnection = xme_hal_net_waitForConnection(s);
		}
		else
		{
			clientConnection = s;
		}
		
		// Keep waiting for packets until the timeout elapses or until
		// we have received at least the expected number of bytes
		memset(receiveBuffer, 0, sizeof(receiveBuffer));
		receiveBufferPos = 0;
		for (time = 0; (time <= requestTimeout && receiveBufferPos < requestLength); time++)
		{
			int numBytes;

			printf("\r[server] Waiting for incoming data... [%3d second(s), %5u byte(s)]", requestTimeout-time, receiveBufferPos);

			if (numBytes = xme_hal_net_readSocket(clientConnection, &receiveBuffer[receiveBufferPos], RECEIVE_BUFFER_SIZE-receiveBufferPos))
			{
				receiveBufferPos += numBytes;
			}
			else
			{
				// Sleep for 1 second and try again later
				xme_hal_sleep_sleep(1000);
			}
		}

		printf("\r[server] Waiting for incoming data... [%5u byte(s)]               \n", receiveBufferPos);

		XME_TEST(time <= requestTimeout);
		XME_TEST(time <= (requestTimeout*1000));

		printf("[server] Received request '%s' (%d byte(s), expected '%s')\n", receiveBuffer, receiveBufferPos, request);

		XME_TEST(0 == strcmp(receiveBuffer, request));
	}

	// Wait for response delay to elapse
	if (responseDelay > 0)
	{
		printf("[server] Waiting %d second(s)...\n", responseDelay);
		xme_hal_sleep_sleep(1000 * responseDelay);
	}

	// Check whether we should send a response
	if (response)
	{
		// Of course we only can send a response if we received a request
		if (receiveBufferPos > 0)
		{
			int responseLength = strlen(response);

			// Send the response to the peer
			printf("[server] Sending response '%s'...\n", response);

			XME_TEST(xme_hal_net_writeSocket(clientConnection, response, responseLength) == responseLength);
		}
		else
		{
			printf("[server] Skipping sending of response '%s', because no request was received!\n", response);
		}
	}

	XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_closeSocket(clientConnection));
	XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_destroySocket(clientConnection));
	if (s != clientConnection)
	{
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_closeSocket(s));
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_destroySocket(s));
	}

	xme_hal_net_finiInterface(&intf);

	XME_TESTSUITE_END();
}

int
main (int argc, char* argv[])
{
	XME_TESTSUITE_BEGIN();
	XME_TEST_MSG_REC(argc >= 2, return 255, "Fatal: Inappropriate number of command line arguments!");
	XME_TESTSUITE(xme_tests_core_init(), "xme_core_init testsuite failed!");
	XME_TESTSUITE(xme_tests_hal_net_slave(argc-1, &argv[1]), "xme_tests_hal_net_slave testsuite failed!");
	XME_TESTSUITE(xme_tests_core_fini(), "xme_core_fini testsuite failed!");
	XME_TESTSUITE_END();
}

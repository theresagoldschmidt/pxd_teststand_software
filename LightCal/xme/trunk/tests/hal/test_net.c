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
 *         Network communication testsuite.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "tests/hal/test_net.h"

#include "tests/tests.h"
#include "xme/hal/net.h"

#ifdef WIN32

#include "windows.h"

#else // #ifdef WIN32

#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <errno.h>

#endif // #ifdef WIN32
/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define RECEIVE_BUFFER_SIZE 256

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
// TODO: See ticket #536
xme_hal_net_interfaceDescr_t intf;

// TODO: See ticket #536
xme_core_interface_address_t intfAddress =
{
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
#ifdef WIN32
HANDLE
launchServerProcess(const char* protocol, uint16_t port, const char* expectedRequest, const char* responseToSend)
{
	const char* serverProcessName = "tests_slave_net.exe";
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	char buffer[MAX_PATH+64];

	memset(&si, 0, sizeof(si));
	si.cb = sizeof(si);
	si.wShowWindow = SW_SHOW;
	si.dwFlags = STARTF_USESHOWWINDOW;

	memset(&pi, 0, sizeof(pi));

#ifdef WIN32
	_snprintf_s
	(
		(char*)&buffer, sizeof(buffer), _TRUNCATE,
#else // #ifdef WIN32
	snprintf
	(
		&buffer, sizeof(buffer),
#endif // #ifdef WIN32
		"\"%s\" %s %u 5 \"%s\" 1 \"%s\"",
		serverProcessName,
		protocol,
		port,
		expectedRequest,
		responseToSend
	);

	if (0 == CreateProcess(0, buffer, 0, 0, true, 0, 0, 0, &si, &pi))
	{
		return 0;
	}
	else
	{
		// Make sure the server is initialized
		WaitForInputIdle(pi.hProcess, 2000);
		Sleep(500);

		return pi.hProcess;
	}
}
#else // #ifdef WIN32
pid_t
launchServerProcess(const char* protocol, uint16_t port, const char* expectedRequest, const char* responseToSend)
{
	char buffer[PATH_MAX+64];	//excessive memory assignment
	//char buffer[100];
	pid_t processId;
	int i=0;
	int status =0;
	char *args[]={"tests_slave_net",protocol,"","5",expectedRequest,"1",responseToSend, NULL};
	
	memset(&buffer, 0, sizeof(buffer));
	
	snprintf
	(
		(char*)&buffer, sizeof(buffer),"%u",port
	);
	args[2]=buffer;
	
	//printf("%s %s %s %s %s %s %s\n",args[0],args[1],args[2],args[3],args[4],args[5],args[6]);	
	processId=fork();	
	
	if((pid_t)-1 == processId)
	{
		printf("Fork error. Exiting.\n");  /* something went wrong */
		exit(1);
		return 0;        
	}

	else if( processId==0 ) // Child			
	{
		printf("I am the child!\n"); /* A zero indicates the child process */
		execv ("tests_slave_net", args);
		sleep(5);

	}	
	else	// Parent	
	{
		printf("I am the parent!\n"); /* A non-zero indicates the parent process */
		//wait(&processId);
		printf("PROCESS_ID: %d\n",processId);
		return processId;
		
	}
}
#endif // #ifdef WIN32

int
xme_tests_hal_net_init()
{
	XME_TESTSUITE_BEGIN();

	XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_initInterface(&intfAddress));

	XME_TESTSUITE_END();
}

int
xme_tests_hal_net_generic()
{
	// Performs generic tests against the xme_hal_net_*() API.

	// TODO: Test select() API! See ticket #726

	xme_hal_net_socketHandle_t s1, s2, s3;

	XME_TESTSUITE_BEGIN();

	XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_hal_net_destroySocket(XME_HAL_NET_INVALID_SOCKET_HANDLE));
	XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_hal_net_destroySocket(1234));

	XME_TEST(XME_HAL_NET_INVALID_SOCKET_HANDLE != (s1 = xme_hal_net_createSocket(&intf, XME_HAL_NET_SOCKET_TCP, "localhost", 1234)));
	XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_destroySocket(s1));
	XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_hal_net_destroySocket(s1));

	XME_TEST(XME_HAL_NET_INVALID_SOCKET_HANDLE != (s2 = xme_hal_net_createSocket(&intf, XME_HAL_NET_SOCKET_TCP, "localhost", 1234)));
	XME_TEST(XME_HAL_NET_INVALID_SOCKET_HANDLE != (s3 = xme_hal_net_createSocket(&intf, XME_HAL_NET_SOCKET_UDP, "localhost", 2345)));
	XME_TEST(s1 == s2); // Handles should be reused
	XME_TEST(s2 != s3); // Handles must not collide
	
	printf("Testing TCP timeout condition. This might take a while...\n");
	XME_TEST(XME_CORE_STATUS_CONNECTION_REFUSED == xme_hal_net_openSocket(s2));
	XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_closeSocket(s2));
	XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_destroySocket(s2));

	printf("Testing UDP timeout condition. This might take a while...\n");
	XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_openSocket(s3)); // UDP connections should connect immediatly
	XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_closeSocket(s3));
	XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_destroySocket(s3));

	XME_TESTSUITE_END();
}

int
xme_tests_hal_net_local(uint16_t socketType)
{
	// Sends data over a non-blocking local connection.
	// This testcase creates two non-blocking local sockets.
	// A request is sent through the socket, followed by a response.

	char receiveBuffer[RECEIVE_BUFFER_SIZE];
	const char* sendBuffer = "Hello world!";
	const char* expectedResponse = "It works!";
	xme_hal_net_socketHandle_t s, c;

	XME_TESTSUITE_BEGIN();

	// Create a non-blocking local socket that will act as a server
	printf("Creating local server socket...\n");
	XME_TEST(s = xme_hal_net_createSocket(&intf, socketType | XME_HAL_NET_SOCKET_NONBLOCKING, 0, 1230));
	XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_openSocket(s));

	// Create a non-blocking local socket that will act as a client
	printf("Creating local client socket...\n");
	XME_TEST(c = xme_hal_net_createSocket(&intf, socketType | XME_HAL_NET_SOCKET_NONBLOCKING, "localhost", 1230));
	XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_openSocket(c));

	// Client sends some data
	printf("Sending client request...\n");
	XME_TEST(strlen(sendBuffer) == xme_hal_net_writeSocket(c, sendBuffer, strlen(sendBuffer)));

	printf("Waiting...\n");
#ifdef WIN32
	Sleep(500);
#else //#ifdef WIN32
	sleep(1);
#endif //#ifdef WIN32
	// Server reads the data
	printf("Reading client request...\n");
	memset(receiveBuffer, 0, sizeof(receiveBuffer));
	XME_TEST(strlen(sendBuffer) == xme_hal_net_readSocket(s, receiveBuffer, RECEIVE_BUFFER_SIZE));
	XME_TEST(0 == strcmp(receiveBuffer, sendBuffer));

	if (strlen(receiveBuffer) > 0)
	{
		// Send a reply to the client
		printf("Sending server response...\n");
		XME_TEST(strlen(expectedResponse) == xme_hal_net_writeSocket(s, expectedResponse, strlen(expectedResponse)));

		// Close the server socket
		printf("Closing server socket...\n");
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_closeSocket(s));
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_destroySocket(s));

		printf("Waiting...\n");
#ifdef WIN32
		Sleep(500);
#else //#ifdef WIN32
		sleep(1);
#endif //#ifdef WIN32

		// Read the response
		printf("Receiving server response...\n");
		memset(receiveBuffer, 0, sizeof(receiveBuffer));
		XME_TEST(xme_hal_net_readSocket(c, receiveBuffer, sizeof(receiveBuffer)) == strlen(expectedResponse));
		XME_TEST(0 == strcmp(receiveBuffer, expectedResponse));
	}
	else
	{
		printf("Did not receive client request! Skipping sending of reply.\n");

		// Close the server socket
		printf("Closing server socket...\n");
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_closeSocket(s));
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_destroySocket(s));
	}

	// Close the client socket
	printf("Closing client socket...\n");
	XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_closeSocket(c));
	XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_destroySocket(c));

	XME_TESTSUITE_END();
}

int
xme_tests_hal_net_localhost(uint16_t socketType)
{
	// Send data between two processes.
	// This testcase creates a server processes that responds to requests
	// and responds with a response once a request has been received.

	char receiveBuffer[RECEIVE_BUFFER_SIZE];
	uint16_t port = 1234;
	const char* sendBuffer = "Hello server!";
	const char* expectedResponse = "Server response!";
	uint16_t responseTimeout = 5;
	xme_hal_net_socketHandle_t s;
	xme_core_status_t ready;

#ifdef WIN32
	HANDLE process;
	DWORD slaveExitCode;
#else // #ifdef WIN32
	pid_t process;
	int slaveExitCode;
#endif //#ifdef WIN32

	XME_TESTSUITE_BEGIN();

	printf("[server] Configured to send request on %s port %u\n", (XME_HAL_NET_SOCKET_TCP & socketType) ? "TCP" : "UDP", port);

	printf("[client] Launching server process...\n");

	XME_TEST(0 != (process = launchServerProcess((XME_HAL_NET_SOCKET_TCP & socketType) ? "tcp" : "udp", port, sendBuffer, expectedResponse)));

	if (0 != process)
	{
		printf("[client] Sending request '%s' (%d byte(s))...\n", sendBuffer, (int)strlen(sendBuffer));

		// Open a socket and send some data
		XME_TEST(s = xme_hal_net_createSocket(&intf, socketType | XME_HAL_NET_SOCKET_NONBLOCKING, "localhost", port));
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_openSocket(s));
		XME_TEST(strlen(sendBuffer) == xme_hal_net_writeSocket(s, sendBuffer, strlen(sendBuffer)));

		// Await response
		ready = xme_hal_net_selectSocket(s, true, false, 3000);
		XME_TEST(XME_CORE_STATUS_SUCCESS == ready);

		if (XME_CORE_STATUS_SUCCESS == ready)
		{
			uint16_t time;
			uint16_t receiveBufferPos;
			uint16_t expectedResponseLength = strlen(expectedResponse);

			// Keep waiting for packets until the timeout elapses or until
			// we have received at least the expected number of bytes
			memset(receiveBuffer, 0, sizeof(receiveBuffer));
			receiveBufferPos = 0;
			for (time = 0; time <= responseTimeout && receiveBufferPos < expectedResponseLength; time++)
			{
				int numBytes;

				printf("\r[client] Waiting for incoming data... [%3d second(s), %5u byte(s)]", responseTimeout-time, receiveBufferPos);

				if (numBytes = xme_hal_net_readSocket(s, &receiveBuffer[receiveBufferPos], RECEIVE_BUFFER_SIZE-receiveBufferPos))
				{
					receiveBufferPos += numBytes;
				}
				else
				{
					// Sleep for 1 second and try again later
#ifdef WIN32
					Sleep(1000);
#else //#ifdef WIN32
					sleep(2);
#endif //#ifdef WIN32

				}
			}

			printf("\r[client] Waiting for incoming data... [%5u byte(s)]               \n", receiveBufferPos);

			XME_TEST(time <= responseTimeout);

			printf("[client] Received response '%s' (%d byte(s), expected '%s')\n", receiveBuffer, receiveBufferPos, expectedResponse);

			XME_TEST(0 == strcmp(receiveBuffer, expectedResponse));
		}

		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_closeSocket(s));
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_destroySocket(s));
#ifdef WIN32
		// Wait for the slave to terminate
		do
		{
			if (0 == GetExitCodeProcess(process, &slaveExitCode))
			{
				break;
			}
		}
		while (STILL_ACTIVE == slaveExitCode);
#else // #ifdef WIN32
		waitpid(0,&slaveExitCode,WNOHANG); //One can wait for children in ones own process group
/*
		do
		{
			if ( kill ( process, 0 ) == -1 && errno == ESRCH )
			{
				break;
			}
		}
		while (1);
*/
#endif //#ifdef WIN32

		XME_TEST(0 == slaveExitCode);
	}

	XME_TESTSUITE_END();
}

int
xme_tests_hal_net_udp_broadcast()
{
	// Broadcast data over a local UDP connection
	// This testcase creates a server processes that responds to UDP
	// broadcast requests and responds with a response once a request
	// has been received.

	uint16_t socketType = XME_HAL_NET_SOCKET_UDP; // Broadcast of course only works with UDP
	char receiveBuffer[RECEIVE_BUFFER_SIZE];
	uint16_t port = 1235;
	const char* sendBuffer = "Hello world!";
	const char* expectedResponse = "Server broadcast response!";
	uint16_t responseTimeout = 5;
	xme_hal_net_socketHandle_t s;
	xme_core_status_t ready;

#ifdef WIN32
	HANDLE process;
	DWORD slaveExitCode;
#else // #ifdef WIN32
	pid_t process;
	int slaveExitCode;
#endif //#ifdef WIN32

	XME_TESTSUITE_BEGIN();

	printf("[server] Configured to send broadcast request on %s port %u\n", (XME_HAL_NET_SOCKET_TCP & socketType) ? "TCP" : "UDP", port);

	printf("[client] Launching server process...\n");

	XME_TEST(0 != (process = launchServerProcess((XME_HAL_NET_SOCKET_TCP & socketType) ? "tcp" : "udp", port, sendBuffer, expectedResponse)));

	if (0 != process)
	{
		printf("[client] Sending request '%s' (%d byte(s))...\n", sendBuffer, (int) strlen(sendBuffer));

		// Open a socket and broadcast some data
		XME_TEST(s = xme_hal_net_createSocket(&intf, socketType | XME_HAL_NET_SOCKET_NONBLOCKING | XME_HAL_NET_SOCKET_BROADCAST, 0, port));
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_openSocket(s));
		XME_TEST(strlen(sendBuffer) == xme_hal_net_writeSocket(s, sendBuffer, strlen(sendBuffer)));

		// Await response
		ready = xme_hal_net_selectSocket(s, true, false, 3000);
		XME_TEST(XME_CORE_STATUS_SUCCESS == ready);

		if (XME_CORE_STATUS_SUCCESS == ready)
		{
			uint16_t time;
			uint16_t receiveBufferPos;
			uint16_t expectedResponseLength = strlen(expectedResponse);

			// Keep waiting for packets until the timeout elapses or until
			// we have received at least the expected number of bytes
			memset(receiveBuffer, 0, sizeof(receiveBuffer));
			receiveBufferPos = 0;
			for (time = 0; time <= responseTimeout && receiveBufferPos < expectedResponseLength; time++)
			{
				int numBytes;

				printf("\r[client] Waiting for incoming data... [%3d second(s), %5u byte(s)]", responseTimeout-time, receiveBufferPos);

				if (numBytes = xme_hal_net_readSocket(s, &receiveBuffer[receiveBufferPos], RECEIVE_BUFFER_SIZE-receiveBufferPos))
				{
					receiveBufferPos += numBytes;
				}
				else
				{
					// Sleep for 1 second and try again later
#ifdef WIN32
					Sleep(1000);
#else //#ifdef WIN32
					sleep(2);
#endif //#ifdef WIN32

				}
			}

			printf("\r[client] Waiting for incoming data... [%5u byte(s)]               \n", receiveBufferPos);

			XME_TEST(time <= responseTimeout);

			printf("[client] Received response '%s' (%d byte(s), expected '%s')\n", receiveBuffer, receiveBufferPos, expectedResponse);

			XME_TEST(0 == strcmp(receiveBuffer, expectedResponse));
		}

		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_closeSocket(s));
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_hal_net_destroySocket(s));

		// Wait for the slave to terminate
#ifdef WIN32
		do
		{
			if (0 == GetExitCodeProcess(process, &slaveExitCode))
			{
				break;
			}
		}
		while (STILL_ACTIVE == slaveExitCode);
#else // #ifdef WIN32
		waitpid(0,&slaveExitCode,WNOHANG); // One can wait for children in ones own process group: 
/*
		do
		{
			if ( kill ( process, 0 ) == -1 && errno == ESRCH )
			{
				break;
			}
		}
		while (1);
*/
#endif //#ifdef WIN32

		XME_TEST(0 == slaveExitCode);
	}

	XME_TESTSUITE_END();
}

int
xme_tests_hal_net_fini()
{
	XME_TESTSUITE_BEGIN();

	xme_hal_net_finiInterface(&intf);

	XME_TESTSUITE_END();
}

int
xme_tests_hal_net()
{
	XME_TESTSUITE_BEGIN();

	XME_TESTSUITE(xme_tests_hal_net_init(), "xme_tests_hal_net_init testsuite failed!");
	XME_TESTSUITE(xme_tests_hal_net_generic(), "xme_tests_hal_net_generic testsuite failed!");
	XME_TESTSUITE(xme_tests_hal_net_local(XME_HAL_NET_SOCKET_UDP), "xme_tests_hal_net_local(UDP) testsuite failed!");
	//XME_TESTSUITE(xme_tests_hal_net_local(XME_HAL_NET_SOCKET_TCP), "xme_tests_hal_net_local(TCP) testsuite failed!"); // Disabled for the moment since it does not work as expected (TCP needs blocking accept())
	XME_TESTSUITE(xme_tests_hal_net_localhost(XME_HAL_NET_SOCKET_UDP), "xme_tests_hal_net_localhost(UDP) testsuite failed!");
	XME_TESTSUITE(xme_tests_hal_net_localhost(XME_HAL_NET_SOCKET_TCP), "xme_tests_hal_net_localhost(TCP) testsuite failed!");
	XME_TESTSUITE(xme_tests_hal_net_udp_broadcast(), "xme_tests_hal_net_udp_broadcast testsuite failed!");
	XME_TESTSUITE(xme_tests_hal_net_fini(), "xme_tests_hal_net_fini testsuite failed!");

	XME_TESTSUITE_END();
}

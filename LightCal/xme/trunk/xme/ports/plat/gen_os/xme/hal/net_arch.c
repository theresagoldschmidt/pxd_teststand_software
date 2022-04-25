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
 *         Network communication abstraction (architecture specific part:
 *         generic OS based implementation).
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Hauke Staehle <staehle@fortiss.org>
 *         Deniz Kabakci <deniz.kabakci@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/net.h"

#include "xme/core/defines.h"
#include "xme/core/log.h"

#include "xme/hal/math.h"
#include "xme/hal/sync.h"
#include "xme/hal/table.h"

#ifdef WIN32
#	include <WinSock2.h>
#	include <WS2tcpip.h>
#	include <MSWSock.h>
#else // #ifdef WIN32
#	include <errno.h>
#	include <netdb.h>
#	include <arpa/inet.h>
#	include <netinet/in.h>
#	include <sys/socket.h>
#	include <sys/types.h>
#	include <fcntl.h>
#	include <sys/ioctl.h>
#endif // #ifdef WIN32

// Define socket as integer
#ifndef WIN32
#	ifndef SOCKET
#		define SOCKET int
#	endif
#	ifndef INVALID_SOCKET
#		define INVALID_SOCKET -1
#	endif
#	ifndef closesocket
#		define closesocket(s) close(s)
#	endif
#endif

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
typedef struct
{
	xme_hal_net_interfaceDescr_t* intf;
	uint16_t flags;
	const char* hostname;
	uint16_t port;
	SOCKET socket;
	struct sockaddr peerAddr;
	bool connected;
}
xme_hal_net_socketItem;

typedef struct
{
	//private
	XME_HAL_TABLE(xme_hal_net_socketItem, sockets, XME_HAL_DEFINES_MAX_SOCKETS); // socketHandle is an index into this table
	xme_hal_sync_criticalSectionHandle_t criticalSectionHandle; ///< Critical section handle for protecting critical regions.
}
xme_hal_net_configStruct_t;

#define GENERIC_ADDRESS_TO_IP4(generic_address) (*((uint32_t *)generic_address))
#define GENERIC_ADDRESS_TO_PORT(generic_address) (*(((uint16_t *)generic_address)+2))

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
static xme_hal_net_configStruct_t xme_hal_net_config;

uint16_t
xme_hal_net_writeDatagram_ipaddress
(
	xme_hal_net_socketItem* socketItem,
	uint32_t remoteAddress,
	uint16_t remotePort,
	const void* buffer,
	uint16_t count
);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_hal_net_init()
{
	XME_HAL_TABLE_INIT(xme_hal_net_config.sockets);

	XME_CHECK
	(
		XME_HAL_SYNC_INVALID_CRITICAL_SECTION_HANDLE != (xme_hal_net_config.criticalSectionHandle = xme_hal_sync_createCriticalSection()),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

#ifdef WIN32
	{
		WSADATA wsaData;

		int status = WSAStartup(MAKEWORD(2,2), &wsaData);

		XME_CHECK_MSG
		(
			NO_ERROR == status,
			XME_CORE_STATUS_UNSUPPORTED,
			XME_LOG_ERROR, "WSAStartup() failed with code %d!", status
		);

		XME_CHECK_MSG_REC
		(
			0x02 == HIBYTE(wsaData.wVersion),
			XME_CORE_STATUS_UNSUPPORTED,
			{
				WSACleanup();
			},
			XME_LOG_ERROR, "WSA version is not 2.2, but %u.%u!", HIBYTE(wsaData.wVersion), LOBYTE(wsaData.wVersion)
		);

		XME_LOG_IF
		(
			0x02 != LOBYTE(wsaData.wVersion),
			XME_LOG_WARNING, "Requested WSA version 2.2, but only %u.%u is supported!", HIBYTE(wsaData.wVersion), LOBYTE(wsaData.wVersion)
		);
	}
#endif // #ifdef WIN32

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_hal_net_fini()
{
#ifdef WIN32
	WSACleanup();
#endif // #ifdef WIN32

	xme_hal_sync_destroyCriticalSection(xme_hal_net_config.criticalSectionHandle);
	xme_hal_net_config.criticalSectionHandle = XME_HAL_SYNC_INVALID_CRITICAL_SECTION_HANDLE;

	XME_HAL_TABLE_FINI(xme_hal_net_config.sockets);
}

xme_core_status_t
xme_hal_net_initInterface
(
	xme_core_interface_address_t* localAddress
)
{
	// Returns one of the following status codes:
    //  - XME_CORE_STATUS_SUCCESS if the network communication interface
    //    has been properly initialized.
    //  - XME_HAL_STATUS_INVALID_INTERFACE if the given interface was
    //    invalid.
    //  - XME_CORE_STATUS_OUT_OF_RESOURCES if not enough resources are
    //    available to initialize the interface.

	// TODO: See ticket #808

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_hal_net_finiInterface
(
	xme_hal_net_interfaceDescr_t* intf
)
{
	// TODO: See ticket #809
}

xme_hal_net_socketHandle_t
xme_hal_net_createSocket
(
	xme_hal_net_interfaceDescr_t* intf,
	uint16_t flags,
	const char* hostname,
	uint16_t port
)
{
	xme_hal_net_socketHandle_t newSocketHandle;
	xme_hal_net_socketItem* socketItem;

	// Either XME_HAL_NET_SOCKET_TCP or XME_HAL_NET_SOCKET_UDP must be set
	if (!((flags & XME_HAL_NET_SOCKET_TCP) ^ (flags & XME_HAL_NET_SOCKET_UDP)))
	{
		return XME_HAL_NET_INVALID_SOCKET_HANDLE;
	}

	xme_hal_sync_enterCriticalSection(xme_hal_net_config.criticalSectionHandle);
	{
		// Allocate a unique socket handle
		newSocketHandle = (xme_hal_net_socketHandle_t)XME_HAL_TABLE_ADD_ITEM(xme_hal_net_config.sockets);
	}
	xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);

	// Retrieve the socket descriptor
	socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, newSocketHandle);

	XME_CHECK
	(
		NULL != socketItem,
		XME_HAL_NET_INVALID_SOCKET_HANDLE
	);

	// Initialize the socket descriptor
	socketItem->intf = intf;
	socketItem->flags = flags;
	socketItem->hostname = hostname;
	socketItem->port = port;
#ifdef WIN32
	socketItem->socket = INVALID_SOCKET;
#else // #ifdef WIN32
	socketItem->socket = -1;
#endif // #ifdef WIN32
	memset(&socketItem->peerAddr, 0, sizeof(socketItem->peerAddr));
	socketItem->connected = false;

	// Return the unique socket handle
	return newSocketHandle;
}

xme_core_status_t
xme_hal_net_destroySocket(xme_hal_net_socketHandle_t socketHandle)
{
	// Returns one of the following status codes:
	//  - XME_CORE_STATUS_SUCCESS if the socket has been successfully
	//    closed and destroyed.
	//  - XME_CORE_STATUS_INVALID_HANDLE if the given socket handle was
	//    invalid.
	xme_core_status_t rval;

	// Try to close the socket to shut down gracefully.
	// This will succeed in any case if the socket handle is valid.
	XME_CHECK
	(
		XME_CORE_STATUS_SUCCESS == (rval = xme_hal_net_closeSocket(socketHandle)),
		rval
	);

	xme_hal_sync_enterCriticalSection(xme_hal_net_config.criticalSectionHandle);
	{
		// Invalidate the socket handle and free associated memory
		XME_HAL_TABLE_REMOVE_ITEM(xme_hal_net_config.sockets, (xme_hal_table_rowHandle_t)socketHandle);
	}
	xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_hal_net_openSocket
(
	xme_hal_net_socketHandle_t socketHandle
)
{
	// Returns one of the following status codes:
	//  - XME_CORE_STATUS_SUCCESS if the socket has been successfully
	//    opened or was already opened before.
	//  - XME_CORE_STATUS_OPERATION_FAILED if the socket could not be
	//    opened.
	//  - XME_CORE_STATUS_INVALID_HANDLE if the given socket handle was
	//    invalid.

	xme_hal_net_socketItem* socketItem;
	struct addrinfo ai;
	struct addrinfo* aiResult = 0;
	struct addrinfo* aiPtr = 0;
	const struct sockaddr *foundAddr = 0;
	size_t foundLen = 0;
	char portName[6];
	int status;

	xme_hal_sync_enterCriticalSection(xme_hal_net_config.criticalSectionHandle);
	{
		XME_CHECK_REC
		(
			NULL != (socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, socketHandle)),
			XME_CORE_STATUS_INVALID_HANDLE,
			{
				xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);
			}
		);
	}
	xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);

	memset(&ai, 0, sizeof(ai));
	ai.ai_flags = (socketItem->flags & XME_HAL_NET_SOCKET_NUMERIC_ADDRESS) ? AI_NUMERICHOST : AF_UNSPEC;
	ai.ai_family = (socketItem->flags & XME_HAL_NET_SOCKET_IPV6) ? AF_INET6 : AF_INET;
	ai.ai_socktype = (socketItem->flags & XME_HAL_NET_SOCKET_TCP) ? SOCK_STREAM : SOCK_DGRAM;
	ai.ai_protocol = (socketItem->flags & XME_HAL_NET_SOCKET_TCP) ? IPPROTO_TCP : IPPROTO_UDP;

	if (0 == socketItem->hostname && !(socketItem->flags & XME_HAL_NET_SOCKET_BROADCAST))
	{
		// If null is specified as hostname and the socket is not configured
		// for broadcast, the socket will be a local socket that can be used
		// to receive data. Opening the socket will bind it to the given
		// local port and set the blocking behavior of the socket.
		struct sockaddr_in recvAddr; // TODO: IPv6 is not really supported, because it uses a different input address structure! See ticket #812

		XME_CHECK
		(
			(socketItem->socket = socket(ai.ai_family, ai.ai_socktype, ai.ai_protocol)) != INVALID_SOCKET,
			XME_CORE_STATUS_INVALID_CONFIGURATION
		);

		// Create the address we are receiving on
		memset((char*)&recvAddr, 0, sizeof(recvAddr));
		recvAddr.sin_family = ai.ai_family;
		recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		recvAddr.sin_port = htons(socketItem->port);

		XME_CHECK_REC
		(
			bind(socketItem->socket, (struct sockaddr*)&recvAddr, sizeof(recvAddr)) == 0,
			XME_CORE_STATUS_INVALID_CONFIGURATION,
			{
				closesocket(socketItem->socket);
				socketItem->socket = INVALID_SOCKET;
			}
		);

		if (XME_HAL_NET_SOCKET_TCP & socketItem->flags)
		{
#if WIN32
			XME_CHECK_REC
			(
				listen(socketItem->socket, SOMAXCONN) != SOCKET_ERROR,
				(WSAEOPNOTSUPP == WSAGetLastError()) ? XME_CORE_STATUS_INVALID_CONFIGURATION : XME_CORE_STATUS_OUT_OF_RESOURCES,
			{
					closesocket(socketItem->socket);
					socketItem->socket = INVALID_SOCKET;
				}
			);
#else // #if WIN32
			XME_CHECK_REC
			(
				listen(socketItem->socket, SOMAXCONN) == 0,
				(ENOTSUP == errno) ? XME_CORE_STATUS_INVALID_CONFIGURATION : XME_CORE_STATUS_OUT_OF_RESOURCES,
			{
					closesocket(socketItem->socket);
					socketItem->socket = INVALID_SOCKET;
				}
			);
#endif // #if WIN32
		}

		// The socket is not really connected!
		socketItem->connected = false;
	}
	else
	{
		if (socketItem->flags & XME_HAL_NET_SOCKET_BROADCAST)
		{
			// Broadcast

			struct sockaddr_in recvAddr;
			uint32_t yes = true;
	
			// Create the address we are receiving on
			memset((char*)&recvAddr, 0, sizeof(recvAddr));
			recvAddr.sin_family = ai.ai_family;
			recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
			recvAddr.sin_port = htons((0 != socketItem->port) ? socketItem->port : XME_HAL_NET_IP_PORT_NUMBER_DATA_CENTRIC_COMMUNICATION);

			socketItem->socket = socket(ai.ai_family, ai.ai_socktype, ai.ai_protocol);

			XME_CHECK_REC
			(
				setsockopt(socketItem->socket, SOL_SOCKET, SO_BROADCAST, (const char*)&yes, sizeof(yes)) == 0, 
				XME_CORE_STATUS_INVALID_CONFIGURATION,
				{
					closesocket(socketItem->socket);
					socketItem->socket = INVALID_SOCKET;
					freeaddrinfo(aiResult);
				}
			);

			XME_CHECK_REC
			(
				setsockopt(socketItem->socket, SOL_SOCKET, SO_REUSEADDR, (const char*)&yes, sizeof(yes)) == 0, 
				XME_CORE_STATUS_INVALID_CONFIGURATION,
				{
					closesocket(socketItem->socket);
					socketItem->socket = INVALID_SOCKET;
					freeaddrinfo(aiResult);
				}
			);

			XME_CHECK_REC
			(
				setsockopt(socketItem->socket, IPPROTO_IP, IP_PKTINFO, (const char*)&yes, sizeof(yes)) == 0, 
				XME_CORE_STATUS_INVALID_CONFIGURATION,
				{
					closesocket(socketItem->socket);
					socketItem->socket = INVALID_SOCKET;
					freeaddrinfo(aiResult);
				}
			);

			// Bind is neccessary to join multicast groups.
			status = bind(socketItem->socket, (struct sockaddr*)&recvAddr, sizeof(recvAddr));
			if ( status )
			{
#if WIN32
				printf("Error in bind %d\n", WSAGetLastError());
#else // #if WIN32
				printf("Error in bind %d\n", errno);
#endif // #if WIN32
			}

			// Broadcast sockets remain unconnected
			socketItem->connected = false;
		}
		else
		{
			// If a hostname to connect to is specified, opening the socket will
			// try to resolve the host name and connect to the that peer.

#ifdef WIN32
			_snprintf_s(portName, sizeof(portName), _TRUNCATE, "%u", socketItem->port);
#else // #ifdef WIN32
			snprintf(portName, sizeof(portName), "%u", socketItem->port);
#endif // #ifdef WIN32

			status = getaddrinfo(socketItem->hostname, portName, &ai, &aiResult);

			XME_CHECK_REC
			(
//#ifdef WIN32
//				WSATRY_AGAIN != status,
//#else // #ifdef WIN32
				EAI_AGAIN != status,
//#endif // #ifdef WIN32
				XME_CORE_STATUS_TEMPORARY_FAILURE,
				{
					freeaddrinfo(aiResult);
				}
			);

			XME_CHECK_REC
			(
//#ifdef WIN32
//				WSAHOST_NOT_FOUND != status,
//#else // #ifdef WIN32
				EAI_NONAME != status,
//#endif // #ifdef WIN32
				XME_CORE_STATUS_NOT_FOUND,
				{
					freeaddrinfo(aiResult);
				}
			);

			XME_CHECK_REC
			(
				0 == status,
				XME_CORE_STATUS_INVALID_CONFIGURATION,
				{
					freeaddrinfo(aiResult);
				}
			);

			XME_ASSERT(0 != aiResult);

			// Iterate over the result list and obtain at least one useable address
			status = 0;
			for (aiPtr = aiResult; aiPtr != 0; aiPtr = aiPtr->ai_next)
			{
				// The following assertions are on the style of (a => b), i.e., (!a || b)
				XME_ASSERT(!(socketItem->flags & XME_HAL_NET_SOCKET_IPV6) || AF_INET6 == aiPtr->ai_family);
				XME_ASSERT(  socketItem->flags & XME_HAL_NET_SOCKET_IPV6  || AF_INET == aiPtr->ai_family);
				XME_ASSERT(!(socketItem->flags & XME_HAL_NET_SOCKET_TCP)  || SOCK_STREAM == aiPtr->ai_socktype);
				XME_ASSERT(!(socketItem->flags & XME_HAL_NET_SOCKET_UDP)  || SOCK_DGRAM == aiPtr->ai_socktype);
				XME_ASSERT(!(socketItem->flags & XME_HAL_NET_SOCKET_TCP)  || IPPROTO_TCP == aiPtr->ai_protocol);
				XME_ASSERT(!(socketItem->flags & XME_HAL_NET_SOCKET_UDP)  || IPPROTO_UDP == aiPtr->ai_protocol);

				// We just consider the first working item returned.
				// In case multiple items are returned, issue a warning here.
				if (socketItem->connected)
				{
					XME_LOG
					(
						XME_LOG_WARNING, "Host name lookup for '%s' returned multiple results, connected to first address!",
						socketItem->hostname
					);
					break;
				}

				socketItem->socket = socket(ai.ai_family, ai.ai_socktype, ai.ai_protocol);
				if (INVALID_SOCKET == socketItem->socket)
				{
					continue;
				}

				status = connect(socketItem->socket, aiResult->ai_addr, aiResult->ai_addrlen);
				if (0 != status)
				{
					// Remember actual error code in status before closing
					// the socket, otherwise it will be reset to zero
#if WIN32
					status = WSAGetLastError();
#else // #if WIN32
					status = errno;
#endif // #if WIN32

					XME_ASSERT(0 != status);

					closesocket(socketItem->socket);
					socketItem->socket = INVALID_SOCKET;
				}
				else
				{
					socketItem->connected = true;
				}
			}

			freeaddrinfo(aiResult);

#ifdef WIN32
			if (0 != status)
			{
				XME_CHECK
				(
					WSAETIMEDOUT != status,
					XME_CORE_STATUS_TIMEOUT
				);
				XME_CHECK
				(
					WSAECONNREFUSED != status,
					XME_CORE_STATUS_CONNECTION_REFUSED
				);
				XME_CHECK
				(
					WSAEHOSTUNREACH != status,
					XME_CORE_STATUS_NOT_FOUND
				);

				return XME_CORE_STATUS_INVALID_CONFIGURATION;
			}
#else // #ifdef WIN32
			if (0 != status)
			{
				XME_CHECK
				(
					ETIMEDOUT != status,
					XME_CORE_STATUS_TIMEOUT
				);
				XME_CHECK
				(
					ECONNREFUSED != status,
					XME_CORE_STATUS_CONNECTION_REFUSED
				);
				XME_CHECK
				(
					EHOSTUNREACH != status,
					XME_CORE_STATUS_NOT_FOUND
				);

				return XME_CORE_STATUS_INVALID_CONFIGURATION;
			}
#endif // #ifdef WIN32
		}

#ifdef WIN32
		XME_CHECK
		(
			INVALID_SOCKET != socketItem->socket,
			XME_CORE_STATUS_INVALID_CONFIGURATION
		);
#else // #ifdef WIN32
		XME_CHECK
		(
			-1 != socketItem->socket,
			XME_CORE_STATUS_INVALID_CONFIGURATION
		);
#endif // #ifdef WIN32

		XME_CHECK
		(
			socketItem->connected || (socketItem->flags & XME_HAL_NET_SOCKET_BROADCAST),
			XME_CORE_STATUS_NOT_FOUND
		);
	}

	// Set socket blocking behavior
	XME_CHECK
	(
		XME_CORE_STATUS_SUCCESS == xme_hal_net_setBlockingBehavior(socketHandle, !(socketItem->flags & XME_HAL_NET_SOCKET_NONBLOCKING)),
		XME_CORE_STATUS_INVALID_CONFIGURATION
	);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_hal_net_closeSocket
(
	xme_hal_net_socketHandle_t socketHandle
)
{
	xme_hal_net_socketItem* socketItem;

	xme_hal_sync_enterCriticalSection(xme_hal_net_config.criticalSectionHandle);
	{
		// Verify socket handle
		XME_CHECK_REC
		(
			NULL != (socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, socketHandle)),
			XME_CORE_STATUS_INVALID_HANDLE,
			{
				xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);
			}
		);
	}
	xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);

	if (socketItem->connected)
	{
#ifdef WIN32
		// Wait a maximum of 100ms for data from the remote host
		DWORD timeoutMs = 100;

		// <http://msdn.microsoft.com/en-us/library/ms740481%28v=VS.85%29.aspx>
		// says we should do the following for a graceful shutdown:

		// 1. Call shutdown with how=SD_SEND.
		shutdown(socketItem->socket, SD_SEND);

		// Make sure the following recv() call does not block too long
		setsockopt(socketItem->socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeoutMs, sizeof(timeoutMs));

		// 2. Call recv or WSARecv until the function completes with success and
		//    indicates zero bytes were received. If SOCKET_ERROR is returned,
		//    then the graceful disconnect is not possible.
		while (1)
		{
			char buffer[64];
			int numBytes = recv(socketItem->socket, (char*)&buffer, 64, 0);
			if (0 == numBytes || SOCKET_ERROR == numBytes)
			{
				break;
			}
		}

		// 3. Call closesocket.
		closesocket(socketItem->socket);

		socketItem->socket = INVALID_SOCKET;

#else // #ifdef WIN32

		// Calling shutdown here won't hurt...
		shutdown(socketItem->socket, SHUT_RDWR);

		close(socketItem->socket);

		socketItem->socket = -1;
#endif // #ifdef WIN32

		socketItem->connected = false;
	}

	return XME_CORE_STATUS_SUCCESS;
}

bool
xme_hal_net_isSocketConnected
(
	xme_hal_net_socketHandle_t socketHandle
)
{
	xme_hal_net_socketItem* socketItem;

	xme_hal_sync_enterCriticalSection(xme_hal_net_config.criticalSectionHandle);
	{
		// Verify socket handle
		XME_CHECK_REC
		(
			NULL != (socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, socketHandle)),
			false,
			{
				xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);
			}
		);
	}
	xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);

	return socketItem->connected;
}

xme_core_status_t
xme_hal_net_setBlockingBehavior
(
	xme_hal_net_socketHandle_t socketHandle,
	bool blocking
)
{
	xme_hal_net_socketItem* socketItem;

	xme_hal_sync_enterCriticalSection(xme_hal_net_config.criticalSectionHandle);
	{
		// Verify socket handle
		XME_CHECK_REC
		(
			NULL != (socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, socketHandle)),
			XME_CORE_STATUS_INVALID_HANDLE,
			{
				xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);
			}
		);
	}
	xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);

	// Change blocking behavior
#ifdef WIN32
	{
		u_long nonBlocking = blocking ? 0 : 1;

		XME_CHECK
		(
			ioctlsocket(socketItem->socket, FIONBIO, &nonBlocking) == 0,
			XME_CORE_STATUS_INVALID_CONFIGURATION
		);
	}
#else // #ifdef WIN32
	XME_CHECK
	(
		fcntl(socketItem->socket, F_SETFL, blocking ? 0 : (O_NONBLOCK | FASYNC)) != -1,
		XME_CORE_STATUS_INVALID_CONFIGURATION
	);
#endif // #ifdef WIN32

	// Remember new blocking behavior
	if (blocking)
	{
		socketItem->flags &= ~XME_HAL_NET_SOCKET_NONBLOCKING;
	}
	else
	{
		socketItem->flags |= XME_HAL_NET_SOCKET_NONBLOCKING;
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_hal_net_selectSocket
(
	xme_hal_net_socketHandle_t socketHandle,
	bool read,
	bool write,
	uint16_t timeoutMs
)
{
	xme_hal_net_socketItem* socketItem;
	float tmp = (float)timeoutMs / 1000;
#if WIN32
	struct fd_set readfds;
	struct fd_set writefds;
#else // #if WIN32
	fd_set readfds;
	fd_set writefds;
#endif // #if WIN32
	
	struct timeval tv;

#ifdef WIN32
	int numDescriptors;
#else // WIN32
	ssize_t numDescriptors;
#endif // WIN32

	xme_hal_sync_enterCriticalSection(xme_hal_net_config.criticalSectionHandle);
	{
		// Verify socket handle
		XME_CHECK_REC
		(
			NULL != (socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, socketHandle)),
			XME_CORE_STATUS_INVALID_HANDLE,
			{
				xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);
			}
		);
	}
	xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);

	tv.tv_sec = (long int)tmp;
	tmp -= tv.tv_sec;
	tv.tv_usec = (long int)(tmp * 1000000.0f);
	
	FD_ZERO(&readfds);
	FD_SET(socketItem->socket, &readfds);

	FD_ZERO(&writefds);
	FD_SET(socketItem->socket, &writefds);

	numDescriptors = select(socketItem->socket+1, read ? &readfds : NULL, write ? &writefds : NULL, NULL, (0 == timeoutMs) ? 0 : &tv);

	XME_CHECK
	(
		0 != numDescriptors,
		XME_CORE_STATUS_TIMEOUT
	);

#ifdef WIN32
	XME_CHECK
	(
		SOCKET_ERROR != numDescriptors,
		XME_CORE_STATUS_INTERNAL_ERROR
	);
#else // #ifdef WIN32
	XME_CHECK
	(
		-1 != numDescriptors,
		XME_CORE_STATUS_INTERNAL_ERROR
	);
#endif // #ifdef WIN32

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_hal_net_selectMultipleSockets(uint16_t max_socket,
								  xme_hal_net_socketHandle_t* socketSetRead,
								  xme_hal_net_socketHandle_t* socketSetWrite,
								  xme_hal_net_socketHandle_t* socketSetError,
								  uint16_t timeoutMs)
{
	xme_hal_net_socketItem* socketItem;
	xme_hal_net_socketHandle_t socketHandle;
	float tmp = (float)timeoutMs / 1000;
#if WIN32
	struct fd_set readfds;
	struct fd_set writefds;
	struct fd_set exceptfds;
#else // #if WIN32
	fd_set readfds;
	fd_set writefds;
	fd_set exceptfds;
#endif // #if WIN32
	struct timeval tv;
	uint16_t i;
	uint16_t maxInternalSocketHandle;

	
#ifdef WIN32
	int numDescriptors;
#else // WIN32
	ssize_t numDescriptors;
#endif // WIN32
	tv.tv_sec = timeoutMs / 1000;
	tv.tv_usec = ( timeoutMs % 1000 );
	maxInternalSocketHandle = 0;

	FD_ZERO(&readfds);
	if ( socketSetRead != NULL )
	{
		for ( i = 0; i < max_socket; i++ )
		{
			socketHandle = socketSetRead[i];
			if ( socketHandle == XME_HAL_NET_INVALID_SOCKET_HANDLE ) continue;

			xme_hal_sync_enterCriticalSection(xme_hal_net_config.criticalSectionHandle);
			{
				// Verify socket handle
				XME_CHECK_REC
				(
					NULL != (socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, socketHandle)),
					XME_CORE_STATUS_INVALID_HANDLE,
					{
						xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);
					}
				);
			}
			xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);

			FD_SET(socketItem->socket, &readfds);
			if ( socketItem->socket > maxInternalSocketHandle ) maxInternalSocketHandle = socketItem->socket;
		}
	}

	FD_ZERO(&writefds);
	if ( socketSetWrite != NULL )
	{
		for ( i = 0; i < max_socket; i++ )
		{
			socketHandle = socketSetWrite[i];
			if ( socketHandle == XME_HAL_NET_INVALID_SOCKET_HANDLE ) continue;

			xme_hal_sync_enterCriticalSection(xme_hal_net_config.criticalSectionHandle);
			{
				// Verify socket handle
				XME_CHECK_REC
				(
					NULL != (socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, socketHandle)),
					XME_CORE_STATUS_INVALID_HANDLE,
					{
						xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);
					}
				);
			}
			xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);

			FD_SET(socketItem->socket, &writefds);
			if ( socketItem->socket > maxInternalSocketHandle ) maxInternalSocketHandle = socketItem->socket;
		}
	}

	FD_ZERO(&exceptfds);
	if ( socketSetError != NULL )
	{
		for ( i = 0; i < max_socket; i++ )
		{
			socketHandle = socketSetError[i];
			if ( socketHandle == XME_HAL_NET_INVALID_SOCKET_HANDLE ) continue;

			xme_hal_sync_enterCriticalSection(xme_hal_net_config.criticalSectionHandle);
			{
				// Verify socket handle
				XME_CHECK_REC
				(
					NULL != (socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, socketHandle)),
					XME_CORE_STATUS_INVALID_HANDLE,
					{
						xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);
					}
				);
			}
			xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);

			FD_SET(socketItem->socket, &exceptfds);
			if ( socketItem->socket > maxInternalSocketHandle ) maxInternalSocketHandle = socketItem->socket;
		}
	}

	numDescriptors = select(
		max_socket,
		(socketSetRead != NULL )? &readfds : NULL,
		(socketSetWrite != NULL ) ? &writefds : NULL,
		(socketSetError != NULL ) ? &exceptfds : NULL,
		(0 == timeoutMs) ? 0 : &tv );

	XME_CHECK
	(
		0 != numDescriptors,
		XME_CORE_STATUS_TIMEOUT
	);

#ifdef WIN32
	XME_CHECK
	(
		SOCKET_ERROR != numDescriptors,
		XME_CORE_STATUS_INTERNAL_ERROR
	);
#else // #ifdef WIN32
	XME_CHECK
	(
		-1 != numDescriptors,
		XME_CORE_STATUS_INTERNAL_ERROR
	);
#endif // #ifdef WIN32

	if ( socketSetRead != NULL )
	{
		for ( i = 0; i < max_socket; i ++ )
		{
			socketHandle = socketSetRead[i];
			if ( socketHandle == XME_HAL_NET_INVALID_SOCKET_HANDLE ) continue;
			socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, socketHandle);
			if ( FD_ISSET(socketItem->socket, &readfds) ) continue;
			socketSetRead[i] = XME_HAL_NET_INVALID_SOCKET_HANDLE;
		}
	}

	if ( socketSetWrite != NULL )
	{
		for ( i = 0; i < max_socket; i ++ )
		{
			socketHandle = socketSetWrite[i];
			if ( socketHandle == XME_HAL_NET_INVALID_SOCKET_HANDLE ) continue;
			socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, socketHandle);
			if ( FD_ISSET(socketItem->socket, &writefds) ) continue;
			socketSetWrite[i] = XME_HAL_NET_INVALID_SOCKET_HANDLE;
		}
	}
	
	if ( socketSetError != NULL )
	{
		for ( i = 0; i < max_socket; i ++ )
		{
			socketHandle = socketSetError[i];
			if ( socketHandle == XME_HAL_NET_INVALID_SOCKET_HANDLE ) continue;
			socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, socketHandle);
			if ( FD_ISSET(socketItem->socket, &exceptfds) ) continue;
			socketSetError[i] = XME_HAL_NET_INVALID_SOCKET_HANDLE;
		}
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_hal_net_socketHandle_t
xme_hal_net_waitForConnection
(
	xme_hal_net_socketHandle_t socketHandle
)
{
	xme_hal_net_socketItem* socketItem;
	bool wasNonBlocking;
	struct sockaddr peerAddr;
	int addrLength;
	SOCKET connectedSocket;
	xme_hal_net_socketHandle_t connectedSocketHandle;
	xme_hal_net_socketItem* connectedSocketItem;

	xme_hal_sync_enterCriticalSection(xme_hal_net_config.criticalSectionHandle);
	{
		// Verify socket handle
		XME_CHECK_REC
		(
			NULL != (socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, socketHandle)),
			XME_HAL_NET_INVALID_SOCKET_HANDLE,
			{
				xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);
			}
		);
	}
	xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);

	// This only works on the server side
	XME_CHECK
	(
		0 == socketItem->hostname,
		XME_HAL_NET_INVALID_SOCKET_HANDLE
	);

	// This only works for connection-oriented sockets
	XME_CHECK
	(
		XME_HAL_NET_SOCKET_TCP & socketItem->flags,
		XME_HAL_NET_INVALID_SOCKET_HANDLE
	);

	// Set the socket to blocking regardless of the XME_HAL_NET_SOCKET_NONBLOCKING flag
	wasNonBlocking = (socketItem->flags & XME_HAL_NET_SOCKET_NONBLOCKING);
	XME_CHECK
	(
		XME_CORE_STATUS_SUCCESS == xme_hal_net_setBlockingBehavior(socketHandle, true),
		XME_HAL_NET_INVALID_SOCKET_HANDLE
	);

	memset(&peerAddr, 0, sizeof(peerAddr));

	addrLength = sizeof(peerAddr);
	connectedSocket = accept(socketItem->socket, &peerAddr, &addrLength);

	// Restore intended blocking behavior
	XME_CHECK
	(
		XME_CORE_STATUS_SUCCESS == xme_hal_net_setBlockingBehavior(socketHandle, !wasNonBlocking),
		XME_HAL_NET_INVALID_SOCKET_HANDLE
	);

	// Check whether a client has connected
	XME_CHECK
	(
		INVALID_SOCKET != connectedSocket,
		XME_HAL_NET_INVALID_SOCKET_HANDLE
	);

	xme_hal_sync_enterCriticalSection(xme_hal_net_config.criticalSectionHandle);
	{
		// Allocate a unique socket handle for the client connection
		connectedSocketHandle = (xme_hal_net_socketHandle_t)XME_HAL_TABLE_ADD_ITEM(xme_hal_net_config.sockets);
	}
	xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);

	// Initialize the socket descriptor
	connectedSocketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, connectedSocketHandle);
	connectedSocketItem->intf = socketItem->intf;
	connectedSocketItem->flags = socketItem->flags;
	connectedSocketItem->hostname = socketItem->hostname;
	connectedSocketItem->port = socketItem->port;
	connectedSocketItem->socket = connectedSocket;
	memset(&connectedSocketItem->peerAddr, 0, sizeof(connectedSocketItem->peerAddr));
	memcpy(&connectedSocketItem->peerAddr, &peerAddr, addrLength);
	connectedSocketItem->connected = true;

	// Also apply the intended blocking behavior to the client socket
	XME_CHECK_REC
	(
		XME_CORE_STATUS_SUCCESS == xme_hal_net_setBlockingBehavior(connectedSocketHandle, !wasNonBlocking),
		XME_HAL_NET_INVALID_SOCKET_HANDLE,
		{
			closesocket(socketItem->socket);
			XME_HAL_TABLE_REMOVE_ITEM(xme_hal_net_config.sockets, (xme_hal_table_rowHandle_t)connectedSocketHandle);
		}
	);

	// Return the connected socket handle
	return connectedSocketHandle;
}

uint16_t
xme_hal_net_readSocket
(
	xme_hal_net_socketHandle_t socketHandle, 
	void* buffer, 
	uint16_t count
)
{
	xme_hal_net_socketItem* socketItem;
	int addrLength;

#ifdef WIN32
	int numBytes;
#else // #ifdef WIN32
	ssize_t numBytes;
	// the control data is dumped here
	char controlBuffer[sizeof(struct in_pktinfo) + sizeof(struct cmsghdr)];
	// the remote/source sockaddr is put here - not necessary in this case
	//struct sockaddr_in peeraddr;
	struct iovec  iov[1];
#if 0	//initialization moved below
	struct msghdr mh = {
	    //.msg_name = &peeraddr,
	    .msg_name = 0,
	    //.msg_namelen = sizeof(peeraddr),
	    .msg_namelen = 0,
	    .msg_control = controlBuffer,
	    .msg_controllen = sizeof(controlBuffer),
	    .msg_iov= iov,
    	    .msg_iovlen = 1
	};
#endif
	struct msghdr mh; 
	struct cmsghdr *cmsg;
	struct in_pktinfo *pi;


#endif // #ifdef WIN32

	xme_hal_sync_enterCriticalSection(xme_hal_net_config.criticalSectionHandle);
	{
		// Verify socket handle
		XME_CHECK_REC
		(
			NULL != (socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, socketHandle)),
			0,
			{
				xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);
			}
		);
	}
	xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);

	memset(buffer, 0, count);

	addrLength = sizeof(struct sockaddr);

#ifdef WIN32
{
	GUID WSARecvMsg_GUID = WSAID_WSARECVMSG;
	LPFN_WSARECVMSG WSARecvMsg;
	DWORD NumberOfBytes;
	WSAMSG WSAMsg;
	unsigned long bNr;
	WSABUF WSABuf;
	WSACMSGHDR *pCMsgHdr;

	DWORD flags;

	// Memory for packet headers. The only activated header should be the
	// PKT_INFO header. This means, the neccessary size is exactly the size
	// of the PKT_INFO header plus the generic header.
	char controlBuffer[sizeof(struct in_pktinfo) + sizeof(WSACMSGHDR)];

	// Get function pointer for WSARecvMsg call.
	// Only with this function, the destination address
	// can be retrieved.
	if ( WSAIoctl(socketItem->socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&WSARecvMsg_GUID, sizeof WSARecvMsg_GUID,
		&WSARecvMsg, sizeof WSARecvMsg,
		&NumberOfBytes, NULL, NULL) == SOCKET_ERROR )
	{
		return 0;
	}

	// Initialize buffer for incoming data
	WSABuf.buf = (char *)buffer;
	WSABuf.len = count;

	// Initialize Parameters for call
	memset( &WSAMsg, 0, sizeof(WSAMsg) );
	WSAMsg.name = 0; // Not interested in remote name
	WSAMsg.namelen = 0; // Not interested in remote name
	WSAMsg.lpBuffers = &WSABuf; // Buffer array for incoming data
	WSAMsg.dwBufferCount = 1; // Size of buffer array for incoming data
	WSAMsg.Control.buf = controlBuffer; // Buffer for packet header data
	WSAMsg.Control.len = sizeof(controlBuffer); // Available space for packet header data

	// TODO: Same function should be used here. But WSARecvMsg gives error for TCP sockets. See ticket #811
	flags = 0;
	if ( socketItem->flags & XME_HAL_NET_SOCKET_UDP )
	{
		if ( WSARecvMsg(socketItem->socket, &WSAMsg, &bNr, NULL, NULL ) == SOCKET_ERROR )
		{
			return 0;
		}
	} else {
		if ( WSARecv(socketItem->socket, &WSABuf, 1, &bNr, &flags, NULL, NULL ) == SOCKET_ERROR )
		{
			return 0;
		}
	}
	numBytes = bNr;

	// Iterate through the packet headers and
	// extract destination IP address
	pCMsgHdr = WSA_CMSG_FIRSTHDR(&WSAMsg);
	while ( pCMsgHdr )
	{
		if ( pCMsgHdr->cmsg_type == IP_PKTINFO )
		{
			struct in_pktinfo *info;
			info = (struct in_pktinfo *)WSA_CMSG_DATA(pCMsgHdr);
			((struct sockaddr_in *)&(socketItem->peerAddr))->sin_addr.S_un.S_addr = info->ipi_addr.S_un.S_addr;
			break;
		}
		pCMsgHdr = WSA_CMSG_NXTHDR(&WSAMsg, pCMsgHdr);
	}

}	
#else // #ifdef WIN32

	/* Initialize the message header for receiving */
    	memset(&mh, 0, sizeof (mh));
	// the remote/source sockaddr is put here - not necessary in this case
	//mh.msg_name = &peeraddr;
	//mh.msg_namelen = sizeof(peeraddr);
	mh.msg_control = controlBuffer;
	mh.msg_controllen = sizeof(struct in_pktinfo) + sizeof(struct cmsghdr);
	iov->iov_base=buffer;
	iov->iov_len=count;
	mh.msg_iov = iov;
    	mh.msg_iovlen = 1;
	
	numBytes = recvmsg(socketItem->socket, &mh, 0);
	
	for ( // iterate all the control headers
	    cmsg = CMSG_FIRSTHDR(&mh);
	    cmsg != NULL;
	    cmsg = CMSG_NXTHDR(&mh, cmsg))
	{
	    // ignore the control headers that don't match what we want
	    if (cmsg->cmsg_level != IPPROTO_IP || cmsg->cmsg_type != IP_PKTINFO)
	    {
	        continue;
	    }
	    pi = CMSG_DATA(cmsg);
	    //((struct sockaddr_in *)&(socketItem->peerAddr))->sin_addr.S_addr = pi->ipi_addr.S_addr;
	    ((struct sockaddr_in *)&(socketItem->peerAddr))->sin_addr = pi->ipi_addr;
	
	    // at this point, peeraddr is the source sockaddr
	    // pi->ipi_spec_dst is the destination in_addr
	    // pi->ipi_addr is the receiving interface in_addr
	}
#endif // #ifdef WIN32

	//printf("\nReceived from peer: %d.%d.%d.%d\n", (uint8_t)socketItem->peerAddr.sa_data[2], (uint8_t) socketItem->peerAddr.sa_data[3], (uint8_t) socketItem->peerAddr.sa_data[4], (uint8_t) socketItem->peerAddr.sa_data[5]);

#ifdef WIN32
	XME_CHECK
	(
		SOCKET_ERROR != numBytes,
		0
	);
#else // #ifdef WIN32
	XME_CHECK
	(
		-1 != numBytes,
		0
	);
#endif // #ifdef WIN32

	return numBytes;
}

uint16_t
xme_hal_net_writeSocket
(
	xme_hal_net_socketHandle_t socketHandle,
	const void* buffer,
	uint16_t count
)
{
	xme_hal_net_socketItem* socketItem;

#ifdef WIN32
	int numBytes = 0;
#else // #ifdef WIN32
	ssize_t numBytes = 0;
#endif // #ifdef WIN32

	xme_hal_sync_enterCriticalSection(xme_hal_net_config.criticalSectionHandle);
	{
		// Verify socket handle
		XME_CHECK_REC
		(
			NULL != (socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, socketHandle)),
			0,
			{
				xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);
			}
		);
	}
	xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);

	if (socketItem->connected)
	{
		// Dedicated connection

#ifdef WIN32
		numBytes = send(socketItem->socket, (char*)buffer, count, 0);
#else // #ifdef WIN32
		numBytes = send(socketItem->socket, buffer, count, 0);
#endif // #ifdef WIN32

		//printf("\nGeneric send\n");
	}
	else if (socketItem->flags & XME_HAL_NET_SOCKET_BROADCAST)
	{
		// Broadcast
		numBytes = xme_hal_net_writeDatagram_ipaddress(
			socketItem,
			INADDR_BROADCAST,
			socketItem->port,
			buffer,
			count);
	}
	else
	{
		// Reply

#ifdef WIN32
		numBytes = sendto(socketItem->socket, (char*)buffer, count, 0, (struct sockaddr*)&socketItem->peerAddr, sizeof(struct sockaddr_in));
#else // #ifdef WIN32
		numBytes = sendto(socketItem->socket, buffer, count, 0, (struct sockaddr*)&socketItem->peerAddr, sizeof(struct sockaddr_in));
#endif // #ifdef WIN32

		//printf("\nSent to peer: %d.%d.%d.%d\n", socketItem->peerAddr.sa_data[2], socketItem->peerAddr.sa_data[3], socketItem->peerAddr.sa_data[4], socketItem->peerAddr.sa_data[5]);
	}

#ifdef WIN32
	XME_CHECK
	(
		SOCKET_ERROR != numBytes,
		0
	);
#else // #ifdef WIN32
	XME_CHECK
	(
		-1 != numBytes,
		0
	);
#endif // #ifdef WIN32

	return numBytes;
}


uint16_t
xme_hal_net_writeDatagram
(
	xme_hal_net_socketHandle_t socketHandle,
	xme_core_interface_address_t* remoteAddress,
	const void* buffer,
	uint16_t count
)
{
	xme_hal_net_socketItem* socketItem;

	xme_hal_sync_enterCriticalSection(xme_hal_net_config.criticalSectionHandle);
	{
		// Verify socket handle
		XME_CHECK_REC
		(
			NULL != (socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, socketHandle)),
			0,
			{
				xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);
			}
		);
	}
	xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);

	return xme_hal_net_writeDatagram_ipaddress(
		socketItem,
		GENERIC_ADDRESS_TO_IP4(remoteAddress),
		GENERIC_ADDRESS_TO_PORT(remoteAddress),
		buffer,
		count);
}

uint16_t
xme_hal_net_writeDatagram_ipaddress
(
	xme_hal_net_socketItem* socketItem,
	uint32_t remoteAddress,
	uint16_t remotePort,
	const void* buffer,
	uint16_t count
)
{
	struct sockaddr_in broadcastAddr;
	
#ifdef WIN32
	int numBytes;
#else // #ifdef WIN32
	ssize_t numBytes;
#endif // #ifdef WIN32

	if ( socketItem -> connected )
	{
		return 0;
	}

	// Broadcast
	memset(&broadcastAddr, 0, sizeof(struct sockaddr_in));
	broadcastAddr.sin_family = (socketItem->flags & XME_HAL_NET_SOCKET_IPV6) ? AF_INET6 : AF_INET;  // TODO: IPv6 is not really supported, because it uses a different input address structure! See ticket #812
	broadcastAddr.sin_port = htons( remotePort );
	broadcastAddr.sin_addr.s_addr = htonl( remoteAddress );

#ifdef WIN32
	numBytes = sendto(socketItem->socket, (char*)buffer, count, 0, (struct sockaddr*)&broadcastAddr, sizeof(struct sockaddr_in));
#else // #ifdef WIN32
	numBytes = sendto(socketItem->socket, (char*)buffer, count, 0, (const struct sockaddr*)&broadcastAddr, sizeof(struct sockaddr_in));
	//printf("\nSent to peer: %d.%d.%d.%d\n", (uint8_t) socketItem->peerAddr.sa_data[2], (uint8_t) socketItem->peerAddr.sa_data[3], (uint8_t) socketItem->peerAddr.sa_data[4], (uint8_t) socketItem->peerAddr.sa_data[5]);
#endif // #ifdef WIN32

#ifdef WIN32
	XME_CHECK
	(
		SOCKET_ERROR != numBytes,
		0
	);
#else // #ifdef WIN32
	XME_CHECK
	(
		-1 != numBytes,
		0
	);
#endif // #ifdef WIN32

	return numBytes;
}

xme_core_status_t
xme_hal_net_joinMulticastGroup
(
	xme_hal_net_socketHandle_t socketHandle,
	xme_core_interface_address_t* remoteAddress
)
{
	xme_hal_net_socketItem* socketItem;
	struct ip_mreq imr; 
	int ret;

	xme_hal_sync_enterCriticalSection(xme_hal_net_config.criticalSectionHandle);
	{
		// Verify socket handle
		XME_CHECK_REC
		(
			NULL != (socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, socketHandle)),
			XME_CORE_STATUS_INVALID_HANDLE,
			{
				xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);
			}
		);
	}
	xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);

	imr.imr_multiaddr.s_addr  = htonl( GENERIC_ADDRESS_TO_IP4(remoteAddress) );
	imr.imr_interface.s_addr  = INADDR_ANY;

	ret = setsockopt(socketItem->socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &imr, sizeof(imr));  

	if ( ret == 0 ) return XME_CORE_STATUS_SUCCESS;

	return XME_CORE_STATUS_INTERNAL_ERROR;
}

xme_core_status_t
xme_hal_net_leaveMulticastGroup(xme_hal_net_socketHandle_t socketHandle, xme_core_interface_address_t* remoteAddress)
{
	xme_hal_net_socketItem* socketItem;
	struct ip_mreq imr;
	int ret;

	xme_hal_sync_enterCriticalSection(xme_hal_net_config.criticalSectionHandle);
	{
		// Verify socket handle
		XME_CHECK_REC
		(
			NULL != (socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, socketHandle)),
			XME_CORE_STATUS_INVALID_HANDLE,
			{
				xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);
			}
		);
	}
	xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);

	imr.imr_multiaddr.s_addr = htonl( GENERIC_ADDRESS_TO_IP4(remoteAddress) );
	imr.imr_interface.s_addr = INADDR_ANY;

	ret = setsockopt(socketItem->socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *) &imr, sizeof(imr));

	if ( ret == 0 ) return XME_CORE_STATUS_SUCCESS;

	return XME_CORE_STATUS_INTERNAL_ERROR;
}

void
xme_hal_net_getAddressOfLastReception
(
	xme_hal_net_socketHandle_t socketHandle, xme_core_interface_address_t* remoteAddress
)
{
	xme_hal_net_socketItem* socketItem;
	struct sockaddr_in *sAdd;

	xme_hal_sync_enterCriticalSection(xme_hal_net_config.criticalSectionHandle);
	{
		// Verify socket handle
		XME_CHECK_REC
		(
			NULL != (socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, socketHandle)),
			,
			{
				xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);
			}
		);
	}
	xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);
	
	sAdd = (struct sockaddr_in *)(&(socketItem->peerAddr));
	
#if WIN32
	GENERIC_ADDRESS_TO_IP4(remoteAddress) = ntohl( sAdd->sin_addr.S_un.S_addr );
#else // #if WIN32
	GENERIC_ADDRESS_TO_IP4(remoteAddress) = ntohl( sAdd->sin_addr.s_addr );
#endif // #if WIN32

	GENERIC_ADDRESS_TO_PORT(remoteAddress) = ntohs( sAdd->sin_port );
}

uint16_t
xme_hal_net_get_available_data_size
(
	xme_hal_net_socketHandle_t socketHandle
)
{
	xme_hal_net_socketItem* socketItem;
	unsigned long dataCount = 0;

	xme_hal_sync_enterCriticalSection(xme_hal_net_config.criticalSectionHandle);
	{
		// Verify socket handle
		XME_CHECK_REC
		(
			NULL != (socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, socketHandle)),
			0,
			{
				xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);
			}
		);
	}
	xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);

	// Determine number of bytes ready to be read
#if WIN32
	{
		DWORD ret;
		XME_CHECK
		(
			0 == WSAIoctl(socketItem->socket, FIONREAD, NULL, 0, &dataCount, sizeof(dataCount), &ret, NULL, NULL ),
			0
		);
	}
#else // #if WIN32
	XME_CHECK
	(
		0 == ioctl(socketItem->socket, FIONREAD, &dataCount),
		0
	);
#endif // #if WIN32

	return (uint16_t)XME_HAL_MATH_MIN(dataCount, 0xFFFF);
}

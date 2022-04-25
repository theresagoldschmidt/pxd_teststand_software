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
 *         FreeRTOS/lwIP implementation).
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Hauke Staehle <staehle@fortiss.org>
 */

/**
 * TODO: (tickets #818 and #819)
 * 	- Parameter "hostname" is not interpreted, so no initial connection to
 * 	  another host is possible. Anyway, accepting connections from foreign
 * 	  hosts works. See ticket #819
 * 	- Multicast not tested. See ticket #818
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/net.h"

#include "xme/core/defines.h"
#include "xme/core/log.h"

#include "xme/hal/math.h"
#include "xme/hal/sched.h"
#include "xme/hal/sync.h"
#include "xme/hal/table.h"

#include "lwip/sockets.h"
#include "lwip/netdb.h"

#include <stdint.h>

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
typedef struct
{
	xme_hal_net_interfaceDescr_t* intf;
	uint16_t flags;
	const char* hostname;
	uint16_t port;
	int socket;
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

static xme_hal_net_callback_t xme_hal_net_callback = NULL;

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

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_hal_net_fini()
{
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
	xme_hal_sync_enterCriticalSection(xme_hal_net_config.criticalSectionHandle);
	{
		socketItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_net_config.sockets, newSocketHandle);
	}
	xme_hal_sync_leaveCriticalSection(xme_hal_net_config.criticalSectionHandle);

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
	socketItem->socket = -1;
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

	// Parameters for socket creation
	int type;

	size_t foundLen = 0;
	char portName[6];
	int status;

	struct addrinfo  ai;
	struct addrinfo* aiResult = 0;
	struct addrinfo* aiPtr = 0;

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

	//assert( socketItem->flags & XME_HAL_NET_SOCKET_NUMERIC_ADDRESS ); // Only numeric addresses supported
	//assert( !(socketItem->flags & XME_HAL_NET_SOCKET_IPV6) ); // Only IPV4 supported
	type = (socketItem->flags & XME_HAL_NET_SOCKET_TCP) ? SOCK_STREAM : SOCK_DGRAM;

	if (0 == socketItem->hostname && !(socketItem->flags & XME_HAL_NET_SOCKET_BROADCAST))
	{
		// If null is specified as hostname and the socket is not configured
		// for broadcast, the socket will be a local socket that can be used
		// to receive data. Opening the socket will bind it to the given
		// local port and set the blocking behavior of the socket.
		struct sockaddr_in recvAddr;

		XME_CHECK
		(
			(socketItem->socket = lwip_socket(0 /* not used */, type, 0 /*not used*/)) != -1,
			XME_CORE_STATUS_INVALID_CONFIGURATION
		);

		// Create the address we are receiving on
		memset((char*)&recvAddr, 0, sizeof(recvAddr));
		recvAddr.sin_len = sizeof(recvAddr);
		recvAddr.sin_family = AF_INET;
		recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		recvAddr.sin_port = htons(socketItem->port);

		XME_CHECK_REC
		(
			lwip_bind(socketItem->socket, (struct sockaddr*)&recvAddr, sizeof(recvAddr)) == 0,
			XME_CORE_STATUS_INVALID_CONFIGURATION,
			{
				closesocket(socketItem->socket);
				socketItem->socket = -1;
			}
		);

		if (XME_HAL_NET_SOCKET_TCP & socketItem->flags)
		{
			// Listen for incoming connection requests on the created socket
			XME_CHECK_REC
			(
				lwip_listen(socketItem->socket, 0xFF) != -1,
				XME_CORE_STATUS_INVALID_CONFIGURATION,
				{
					closesocket(socketItem->socket);
					socketItem->socket = -1;
				}
			);
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
			bool yes = true;

			// Create the address we are receiving on
			memset((char*)&recvAddr, 0, sizeof(recvAddr));
			recvAddr.sin_len = sizeof(recvAddr);
			recvAddr.sin_family = AF_INET;
			recvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            recvAddr.sin_port = htons((0 != socketItem->port) ? socketItem->port : XME_HAL_NET_IP_PORT_NUMBER_DATA_CENTRIC_COMMUNICATION);

			socketItem->socket = lwip_socket(0 /* not used */, type, 0 /*not used*/);

			// Bind is neccessary to join multicast groups.
			status = lwip_bind(socketItem->socket, (struct sockaddr*)&recvAddr, sizeof(recvAddr));

			// Broadcast sockets remain unconnected
			socketItem->connected = false;
		}
		else
		{

			// If a hostname to connect to is specified, opening the socket will
			// try to resolve the host name and connect to the that peer.

			// TODO: See ticket #819
			//snprintf(portName, sizeof(portName), "%u", socketItem->port);

			status = lwip_getaddrinfo(socketItem->hostname, NULL, &ai, &aiResult);

			XME_CHECK_REC
			(
				EAI_MEMORY != status,
				XME_CORE_STATUS_OUT_OF_RESOURCES,
				{
						lwip_freeaddrinfo(aiResult);
				}
			);

			XME_CHECK_REC
			(
				EAI_FAIL != status,
				XME_CORE_STATUS_NOT_FOUND,
				{
						lwip_freeaddrinfo(aiResult);
				}
			);

			XME_CHECK_REC
			(
				0 == status,
				XME_CORE_STATUS_INVALID_CONFIGURATION,
				{
						lwip_freeaddrinfo(aiResult);
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

				socketItem->socket = lwip_socket(0 /* not used */, type, 0 /*not used*/);;
				if (-1 == socketItem->socket)
				{
					continue;
				}

				// TODO: Port should already be set at call to lwip_getaddrinfo(). See ticket #819
				((struct sockaddr_in *)aiResult->ai_addr)->sin_port = htons(socketItem->port);

				status = lwip_connect(socketItem->socket, aiResult->ai_addr, aiResult->ai_addrlen);
				if (0 != status)
				{
					closesocket(socketItem->socket);
					socketItem->socket = -1;
				}
				else
				{
					socketItem->connected = true;
				}
			}

			lwip_freeaddrinfo(aiResult);

			XME_CHECK
			(
				0 == status,
				XME_CORE_STATUS_INVALID_CONFIGURATION
			);
		}

		XME_CHECK
		(
			-1 != socketItem->socket,
			XME_CORE_STATUS_INVALID_CONFIGURATION
		);

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
		// Calling shutdown here won't hurt...
		lwip_shutdown(socketItem->socket, 0 /* Not used */);

		lwip_close(socketItem->socket);

		socketItem->socket = -1;

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
	{
		uint32_t nonBlocking = blocking ? 0 : 1;

		XME_CHECK
		(
			lwip_ioctl(socketItem->socket, FIONBIO, &nonBlocking) == 0,
			XME_CORE_STATUS_INVALID_CONFIGURATION
		);
	}

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
	struct fd_set readfds;
	struct fd_set writefds;
	struct timeval tv;

	int numDescriptors;

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

	numDescriptors = lwip_select(socketItem->socket+1, read ? &readfds : NULL, write ? &writefds : NULL, NULL, (0 == timeoutMs) ? 0 : &tv);

	XME_CHECK
	(
		0 != numDescriptors,
		XME_CORE_STATUS_TIMEOUT
	);

	XME_CHECK
	(
		-1 != numDescriptors,
		XME_CORE_STATUS_INTERNAL_ERROR
	);

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
	struct fd_set readfds;
	struct fd_set writefds;
	struct fd_set exceptfds;
	struct timeval tv;
	uint16_t i;
	uint16_t maxInternalSocketHandle;

	int numDescriptors;

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

	numDescriptors = lwip_select(
		maxInternalSocketHandle+1,
		(socketSetRead != NULL )? &readfds : NULL,
		(socketSetWrite != NULL ) ? &writefds : NULL,
		(socketSetError != NULL ) ? &exceptfds : NULL,
		(0 == timeoutMs) ? 0 : &tv );

	XME_CHECK
	(
		0 != numDescriptors,
		XME_CORE_STATUS_TIMEOUT
	);

	XME_CHECK
	(
		-1 != numDescriptors,
		XME_CORE_STATUS_INTERNAL_ERROR
	);

	if ( socketSetRead != NULL )
	{
		for ( i = 0; i < max_socket; i ++ )
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
	socklen_t addrLength;
	int connectedSocket;
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
	connectedSocket = lwip_accept(socketItem->socket, &peerAddr, &addrLength);

	// Restore intended blocking behavior
	XME_CHECK
	(
		XME_CORE_STATUS_SUCCESS == xme_hal_net_setBlockingBehavior(socketHandle, !wasNonBlocking),
		XME_HAL_NET_INVALID_SOCKET_HANDLE
	);

	// Check whether a client has connected
	XME_CHECK
	(
		-1 != connectedSocket,
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
			closesocket(connectedSocket);
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
	socklen_t addrLength;

	int numBytes;

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

	numBytes = lwip_recvfrom(socketItem->socket, buffer, count, 0, (struct sockaddr*)&socketItem->peerAddr, &addrLength);

	XME_CHECK
	(
		-1 != numBytes,
		0
	);

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
	int numBytes = 0;


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
		numBytes = lwip_send(socketItem->socket, (char*)buffer, count, 0);
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

		numBytes = lwip_sendto(socketItem->socket, (char*)buffer, count, 0, (struct sockaddr*)&socketItem->peerAddr, sizeof(struct sockaddr_in));

		//printf("\nSent to peer: %d.%d.%d.%d\n", socketItem->peerAddr.sa_data[2], socketItem->peerAddr.sa_data[3], socketItem->peerAddr.sa_data[4], socketItem->peerAddr.sa_data[5]);
	}

	XME_CHECK
	(
		-1 != numBytes,
		0
	);

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

	int numBytes;

	if ( socketItem -> connected )
	{
		return 0;
	}

	// Broadcast
	memset(&broadcastAddr, 0, sizeof(struct sockaddr_in));
	broadcastAddr.sin_len = sizeof(broadcastAddr);
	broadcastAddr.sin_family = AF_INET;
	broadcastAddr.sin_port = htons( remotePort );
	broadcastAddr.sin_addr.s_addr = htonl( remoteAddress );

	numBytes = lwip_sendto(socketItem->socket, (char*)buffer, count, 0, (struct sockaddr*)&broadcastAddr, sizeof(struct sockaddr_in));

	XME_CHECK
	(
		-1 != numBytes,
		0
	);

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

	ret = lwip_setsockopt(socketItem->socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &imr, sizeof(imr));

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

	ret = lwip_setsockopt(socketItem->socket, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *) &imr, sizeof(imr));

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
	
	GENERIC_ADDRESS_TO_IP4(remoteAddress) = ntohl( *(uint32_t *)&(sAdd->sin_addr) );
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
	XME_CHECK
	(
		-1 != ioctlsocket(socketItem->socket, FIONREAD, &dataCount),
		0
	);

	return (uint16_t)XME_HAL_MATH_MIN(dataCount, 0xFFFF);
}

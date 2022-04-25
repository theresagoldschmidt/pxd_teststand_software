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
 *         Generic header for network communication abstraction.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_HAL_NET_H
#define XME_HAL_NET_H

/**
 * \defgroup hal_net Network communication
 *
 * \brief  Network communication abstraction.
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/core.h"
#include "xme/core/interface.h"

#include <stdint.h>

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define XME_HAL_NET_IP_PORT_NUMBER_DATA_CENTRIC_COMMUNICATION 0x804F ///< IP port number used for data centric communication. This port number is part of a port number range that is unassigned by IANA and is hence unlikely to be assigned in the future.
#define XME_HAL_NET_IP_PORT_NUMBER_LOGIN                      0x804E ///< IP port number used for login. This port number is part of a port number range that is unassigned by IANA and is hence unlikely to be assigned in the future.

#define XME_HAL_NET_SOCKET_UDP 0x0001 ///< Socket will be datagram-based (UDP).
#define XME_HAL_NET_SOCKET_TCP 0x0002 ///< Socket will be connection-oriented (TCP).
#define XME_HAL_NET_SOCKET_BROADCAST 0x0004 ///< Socket will allow sending broadcasts. Will only work in combination with XME_HAL_NET_SOCKET_UDP.
#define XME_HAL_NET_SOCKET_NONBLOCKING 0x0008 ///< Socket operations will be non-blocking.
#define XME_HAL_NET_SOCKET_NUMERIC_ADDRESS 0x0040 ///< Address specified is numeric (optional optimization).
#define XME_HAL_NET_SOCKET_IPV6 0x0080 ///< Address specified is an IPv6 address.

typedef void (*xme_hal_net_callback_t)
(
	xme_core_interface_state_t status
);

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \ingroup hal_net
 *
 * \typedef xme_hal_net_socketHandle_t
 *
 * \brief Socket handle.
 */
typedef enum
{
	XME_HAL_NET_INVALID_SOCKET_HANDLE = 0, ///< Invalid socket handle.
	XME_HAL_NET_MAX_SOCKET_HANDLE = XME_HAL_TABLE_MAX_ROW_HANDLE ///< Largest possible socket handle.
}
xme_hal_net_socketHandle_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \ingroup hal_net
 *
 * \brief  Initializes the network abstraction.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the network abstraction has been
 *            properly initialized.
 *          - XME_CORE_STATUS_OUT_OF_RESOURCES if network abstraction
 *            initialization failed.
 */
xme_core_status_t
xme_hal_net_init();

/**
 * \ingroup hal_net
 *
 * \brief  Frees resources occupied by the network abstraction.
 */
void
xme_hal_net_fini();

/**
 * \ingroup hal_net
 *
 * \brief  Initializes the given network communication interface.
 *
 * \param  address The local address of the interface.
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the network communication interface
 *            has been properly initialized.
 *          - XME_HAL_STATUS_INVALID_INTERFACE if the given interface was
 *            invalid.
 *          - XME_CORE_STATUS_OUT_OF_RESOURCES if not enough resources are
 *            available to initialize the interface.
 */
xme_core_status_t
xme_hal_net_initInterface(xme_core_interface_address_t* localAddress);

/**
 * \ingroup hal_net
 *
 * \brief  Deinitializes the given network communication interface and frees
 *         all resources associated with it.
 *
 * \param  intf Network communication interface to deinitialize.
 */
void
xme_hal_net_finiInterface(xme_hal_net_interfaceDescr_t* intf);

/**
 * \ingroup hal_net
 *
 * \brief  Creates a new communication socket for the given network
 *         communication interface.
 *
 * \param  intf Network communication interface for which to create the
 *         communication socket.
 * \param  flags A combination of the following flags:
 *          - XME_HAL_NET_SOCKET_UDP: Socket will be datagram-based (UDP).
 *            Not compatible with XME_HAL_NET_SOCKET_TCP.
 *          - XME_HAL_NET_SOCKET_TCP: Socket will be connection-oriented
 *            (TCP). Not compatible with XME_HAL_NET_SOCKET_UDP.
 *          - XME_HAL_NET_SOCKET_NUMERIC_ADDRESS: Address specified is numeric
 *            (optional optimization).
 *          - XME_HAL_NET_SOCKET_IPV6: Address specified is an IPv6 address
 *            (must be specified in case of IPv6 addressing).
 * \param  hostname Destination host name for the socket.
 * \param  port Port number under which to bind to the destination address.
 * \return Returns a non-zero socket handle on success and
 *         XME_HAL_NET_INVALID_SOCKET on error.
 */
xme_hal_net_socketHandle_t
xme_hal_net_createSocket(xme_hal_net_interfaceDescr_t* intf, uint16_t flags, const char* hostname, uint16_t port);

/**
 * \ingroup hal_net
 *
 * \brief  Closes and destroys the given socket.
 *
 * \param  socketHandle Handle of the socket to close and destroy.
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the socket has been successfully
 *            closed and destroyed.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the given socket handle was
 *            invalid.
 */
xme_core_status_t
xme_hal_net_destroySocket(xme_hal_net_socketHandle_t socketHandle);

/**
 * \ingroup hal_net
 *
 * \brief  Tries to open the communication channel.
 *
 *         If the given socket is to be bound to a remote peer (hostname
 *         parameter was set to non-zero in xme_hal_net_createSocket()), the
 *         function will try to resolve the address of the host and, upon
 *         success, open the communication channel under the specified port.
 *         Depending on the type of transport protocol used, handshaking may
 *         occur.
 *
 *         If the given socket is to be used for sending (hostname parameter
 *         was set to zero in xme_hal_net_createSocket()), the function will
 *         bind the socket to the local host under the specified port.
 *
 *         If the given socket has already been opened, the function silently
 *         reports success.
 *
 * \param  socketHandle Handle of the socket to open.
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the socket has been successfully
 *            opened or was already opened before.
 *          - XME_CORE_STATUS_OPERATION_FAILED if the socket could not be
 *            opened.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the given socket handle was
 *            invalid.
 */
xme_core_status_t
xme_hal_net_openSocket(xme_hal_net_socketHandle_t socketHandle);

/**
 * \ingroup hal_net
 *
 * \brief  Closes the communication channel.
 *         In case the socket is already closed, the function silently reports
 *         success.
 *
 * \param  socketHandle Handle of the socket to close.
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the socket has been successfully
 *            closed or was already closed before.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the given socket handle was
 *            invalid.
 */
xme_core_status_t
xme_hal_net_closeSocket(xme_hal_net_socketHandle_t socketHandle);

/**
 * \ingroup hal_net
 *
 * \brief  Returns whether the communication channel is currently open.
 *
 * \param  socketHandle Handle of the socket to open.
 * \return Returns whether the communication channel is currently open.
 */
bool
xme_hal_net_isSocketConnected(xme_hal_net_socketHandle_t socketHandle);

// TODO: See ticket #722
xme_core_status_t
xme_hal_net_setBlockingBehavior(xme_hal_net_socketHandle_t socketHandle, bool blocking);

/**
 * \ingroup hal_net
 *
 * \brief  Waits until read or write operations are available, or until
 *         the given timeout interval expires.
 *
 * \note   Some functionality may not be supported on all platforms!
 *
 * \param  socketHandle Handle of the socket to open.
 * \param  read If true, the function returns as soon as at least one byte is
 *         available to read from the socket.
 * \param  write If true, the function returns as soon as at least one byte
 *         may be written to the socket.
 * \param  timeoutMs Timeout interval in milliseconds after which the function
 *         should return even if no read or write operations are available.
 *         Specifying zero for this parameter will cause the function to never
 *         return before the requested operations are available.
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if read or write operations are
 *            available.
 *          - XME_CORE_STATUS_TIMEOUT if the given timeout interval elapsed
 *            before read or write operations were available.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the given socket handle was
 *            invalid.
 *          - XME_CORE_STATUS_INTERNAL_ERROR if the operation failed
 *            internally.
 */
xme_core_status_t
xme_hal_net_selectSocket(xme_hal_net_socketHandle_t socketHandle, bool read, bool write, uint16_t timeoutMs);


// TODO: See ticket #722
/**
 * \ingroup hal_net
 *
 */
xme_core_status_t
xme_hal_net_selectMultipleSockets(uint16_t max_socket,
								  xme_hal_net_socketHandle_t* socketSetRead,
								  xme_hal_net_socketHandle_t* socketSetWrite,
								  xme_hal_net_socketHandle_t* socketSetError,
								  uint16_t timeoutMs);

// TODO: See ticket #722
/**
 * \ingroup hal_net
 *
 */
xme_hal_net_socketHandle_t
xme_hal_net_waitForConnection(xme_hal_net_socketHandle_t socketHandle);

/**
 * \ingroup hal_net
 *
 * \brief  Receives up to count bytes from the given socket, places them in
 *         into the given buffer and returns the number of bytes actually read.
 *
 * \note   If the XME_HAL_NET_SOCKET_NONBLOCKING flag is set on the socket,
 *         the function will return immediatly with a return value of 0 if
 *         there is no data available. Otherwise, the function will block
 *         until data is available.
 *
 * \param  buffer Buffer to place received data in. Must be at least of the
 *         size specified in count.
 * \param  count Size of the given buffer or number of bytes to read at
 *         maximum, whichever is smaller.
 * \return Number of bytes actually read.
 */
uint16_t
xme_hal_net_readSocket(xme_hal_net_socketHandle_t socketHandle, void* buffer, uint16_t count);

/**
 * \ingroup hal_net
 *
 * \brief  Sends up to count bytes from the given buffer over the given socket
 *         and returns the number of bytes actually sent.
 *
 * \note   If the XME_HAL_NET_SOCKET_NONBLOCKING flag is set on the socket,
 *         the function will return immediatly with a return value of 0 if
 *         no data can be sent. Otherwise, the function will block until
 *         data can be sent.
 *
 * \param  buffer Buffer with data to send. Must be at least of size count.
 * \param  count Number of bytes to send.
 * \return Number of bytes actually sent.
 */
uint16_t
xme_hal_net_writeSocket(xme_hal_net_socketHandle_t socketHandle, const void* buffer, uint16_t count);

/**
 * \ingroup hal_net
 *
 * \brief  Uses given socket to send a datagram.
 *         For IP-Networks, this function can be used for broadcast as 
 *         well as multicast.
 *
 * \param socketHandle Socket to use for sending.
 * \param remoteAddress Address of destination.
 * \param buffer Buffer with data to send. Must be at least of size count.
 * \param count Number of bytes to send.
 * \return Number of bytes actually sent.
 */
uint16_t
xme_hal_net_writeDatagram(xme_hal_net_socketHandle_t socketHandle, xme_core_interface_address_t* remoteAddress, const void* buffer, uint16_t count);


/**
 * \ingroup hal_net
 *
 * \brief Joins a multicast group.
 *
 * \param socketHandle Socket to use for joining.
 * \param remoteAddress Address of multicast group.
 * \return Returns one of the following values:
 *          - XME_CORE_STATUS_INTERNAL_ERROR On error.
 *			- XME_CORE_STATUS_SUCCESS On success.
 */
xme_core_status_t
xme_hal_net_joinMulticastGroup(xme_hal_net_socketHandle_t socketHandle, xme_core_interface_address_t* remoteAddress);

/**
 * \ingroup hal_net
 *
 * \brief Leaves a multicast group.
 *
 * \param socketHandle Socket to use for sending.
 * \param remoteAddress Address of destination.
 * \param buffer Buffer with data to send. Must be at least of size count.
 * \param count Number of bytes to send.
 * \return Number of bytes actually sent.
 */
xme_core_status_t
xme_hal_net_leaveMulticastGroup(xme_hal_net_socketHandle_t socketHandle, xme_core_interface_address_t* remoteAddress);

// TODO: This is probably not thread-safe! See ticket #810
/**
 * \ingroup hal_net
 *
 * \brief Get the address of the peer host.
 *
 * \param socketHandle Socket that received something before.
 * \param remoteAddress Pointer to memory to store address.
 */
void
xme_hal_net_getAddressOfLastReception(xme_hal_net_socketHandle_t socketHandle, xme_core_interface_address_t* remoteAddress);

/**
 * \ingroup hal_net
 *
 * \brief Gets the available data to read of a specific socket.
 *
 * \param socketHandle Socket to be checked.
 * \return Number of bytes available. Zero on error or if zero bytes are available.
 */
uint16_t xme_hal_net_get_available_data_size (xme_hal_net_socketHandle_t socketHandle);

/**
 * \ingroup hal_net
 *
 * \brief Callback for interface change.
 *
 * \param cb Callback function.
 */
void
xme_hal_net_registerCallback(xme_hal_net_callback_t cb);

/**
 * \ingroup hal_net
 *
 * \brief Clear callback function.
 */
void
xme_hal_net_clearCallback();

/******************************************************************************/
/***   Platform-specific includes                                           ***/
/******************************************************************************/
#include "xme/hal/net_arch.h"

#endif // #ifndef XME_HAL_NET_H

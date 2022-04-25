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
 *         Plugin for the interface manager to work with IP/socket based networks.
 *
 * \author
 *         Hauke Staehle <staehle@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/net_plugin.h"
#include "xme/hal/net.h"
#include "xme/hal/sched.h"
#include <stdio.h>

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

/**
 * \brief Thread to process incoming packets.
 *        The thread must not started twice. The thread will
 *        block on socket events and the callback is executed
 *        once a packet arrived. If no callback is specified,
 *        the packets will remain in the interface buffer.
 *
 * \param socket Socket to process incoming packets. 
 *
 * \return Returns one of the following values:
 *         - XME_CORE_STATUS_SUCCESS If thread was successfully created.
 *         - XME_CORE_STATUS_INTERNAL_ERROR On error.
 */
xme_core_status_t 
xme_core_net_plugin_startWorkingThread(xme_hal_net_socketHandle_t socket);

/**
 * \brief Stops processing of incoming packets.
 *        This function may block the system up to two seconds.
 *
 * \return Returns one of the following values:
 *         - XME_CORE_STATUS_SUCCESS If thread was successfully stopped.
 *         - XME_CORE_STATUS_INTERNAL_ERROR On error.
 */
xme_core_status_t
xme_core_net_plugin_stopWorkingThread(void);

/**
 * These are the plugin interface prototypes
 */
xme_core_status_t xme_core_net_plugin_init (xme_core_interface_address_t *localAddress);
xme_core_status_t xme_core_net_plugin_fini (void);

uint16_t xme_core_net_plugin_read_non_blocking (void *buffer, uint16_t count, xme_core_dataChannel_t *channel);
uint16_t xme_core_net_plugin_read_blocking (void *buffer, uint16_t count, xme_core_dataChannel_t *channel);

uint16_t xme_core_net_plugin_write_non_blocking (const void *buffer, uint16_t count, xme_core_dataChannel_t channel);
uint16_t xme_core_net_plugin_write_blocking (const void *buffer, uint16_t count, xme_core_dataChannel_t channel);

xme_core_status_t xme_core_net_plugin_join_channel (xme_core_dataChannel_t channelNumber);
xme_core_status_t xme_core_net_plugin_leave_channel (xme_core_dataChannel_t channelNumber);

uint16_t xme_core_net_plugin_get_available_data_size (void);

xme_core_interface_state_t xme_core_net_plugin_wait_for_state_change (void);

void xme_core_net_plugin_register_callback(xme_core_interface_callback_t);
void xme_core_net_plugin_clear_callback(void);

xme_core_status_t xme_core_net_plugin_provide_channel(xme_core_dataChannel_t channel);
xme_core_status_t xme_core_net_plugin_unprovide_channel(xme_core_dataChannel_t channel);

xme_core_status_t xme_core_net_plugin_add_channel_to_phy_address_mapping (xme_core_dataChannel_t channel, xme_core_interface_address_t *address);
xme_core_status_t xme_core_net_plugin_remove_channel_to_phy_address_mapping (xme_core_dataChannel_t channel, xme_core_interface_address_t *address);

void xme_core_net_interfaceCallback(xme_core_interface_state_t status);

/**
 * Handle for receive task
 */
xme_hal_sched_taskHandle_t receiveDataTaskHandle;

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
static xme_core_interface_callback_t xme_core_net_plugin_callback;
static xme_hal_net_socketHandle_t xme_core_net_plugin_socket;

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/

/*
 * 239.192.x.x is chosen as the address range for XME multicast communication.
 *
 * While the first two bytes of the IP address are fixed, the third and fourth byte
 * represent the channel number. The port is fixed to the value
 * XME_HAL_NET_IP_PORT_NUMBER_DATA_CENTRIC_COMMUNICATION.
 *
 * RFC:
 * "239.192.0.0/14 is defined to be the IPv4 Organization Local Scope,
 * and is the space from which an organization should allocate sub-
 * ranges when defining scopes for private use."
 *
 * http://tools.ietf.org/html/rfc2365
 *
 */
#define CHANNEL_TO_MULTICAST_ADDRESS(channel, address) \
	do { \
		*((uint32_t *)&address.data[0]) = (	\
				((uint32_t)239 << 24 ) | \
				((uint32_t)192 << 16 ) | \
				(((uint32_t)channel / 256) << 8 ) |	\
				((uint32_t)channel % 256) ); \
		*((uint16_t *)&address.data[4]) = XME_HAL_NET_IP_PORT_NUMBER_DATA_CENTRIC_COMMUNICATION; \
	} while (0)


/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_core_net_plugin_init (xme_core_interface_address_t *localAddress)
{
	// TODO: Error handling! See ticket #721

	xme_hal_net_initInterface(localAddress);

	// Create one socket
	xme_core_net_plugin_socket =
		xme_hal_net_createSocket(
			NULL,
			XME_HAL_NET_SOCKET_UDP | XME_HAL_NET_SOCKET_BROADCAST,
			NULL,
			0
		);

	// Open and bind it
	xme_hal_net_openSocket( xme_core_net_plugin_socket );

	receiveDataTaskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;
	if ( xme_core_net_plugin_startWorkingThread( xme_core_net_plugin_socket ) != XME_CORE_STATUS_SUCCESS )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t xme_core_net_plugin_fini (void)
{
	xme_core_net_plugin_stopWorkingThread();
	xme_hal_net_closeSocket( xme_core_net_plugin_socket );
	xme_hal_net_destroySocket( xme_core_net_plugin_socket );

	return XME_CORE_STATUS_SUCCESS;
}

uint16_t xme_core_net_plugin_read_non_blocking (void *buffer, uint16_t count, xme_core_dataChannel_t *channel)
{
	uint16_t rcount;
	xme_core_interface_address_t add;

	xme_hal_net_setBlockingBehavior( xme_core_net_plugin_socket, false );

	rcount = xme_hal_net_readSocket( xme_core_net_plugin_socket, buffer, count );
	if ( rcount == 0 ) return 0;

	xme_hal_net_getAddressOfLastReception( xme_core_net_plugin_socket, &add );
	*channel = (xme_core_dataChannel_t)(add.data[1] * 256 + add.data[0]);

	return rcount;
}

uint16_t xme_core_net_plugin_read_blocking (void *buffer, uint16_t count, xme_core_dataChannel_t *channel)
{
	uint16_t rcount;
	xme_core_interface_address_t add;

	xme_hal_net_setBlockingBehavior( xme_core_net_plugin_socket, true );

	rcount = xme_hal_net_readSocket( xme_core_net_plugin_socket, buffer, count );
	if ( rcount == 0 ) return 0;

	xme_hal_net_getAddressOfLastReception( xme_core_net_plugin_socket, &add );
	*channel = (xme_core_dataChannel_t)(add.data[1] * 256 + add.data[0]);

	return rcount;
}

uint16_t xme_core_net_plugin_write_non_blocking (const void *buffer, uint16_t count, xme_core_dataChannel_t channel)
{
	xme_core_interface_address_t add;
	
	CHANNEL_TO_MULTICAST_ADDRESS(channel, add);

	xme_hal_net_setBlockingBehavior( xme_core_net_plugin_socket, false );

	return xme_hal_net_writeDatagram(xme_core_net_plugin_socket, &add, buffer, count );
}

uint16_t xme_core_net_plugin_write_blocking (const void *buffer, uint16_t count, xme_core_dataChannel_t channel)
{
	xme_core_interface_address_t add;
	
	CHANNEL_TO_MULTICAST_ADDRESS(channel, add);
	
	xme_hal_net_setBlockingBehavior( xme_core_net_plugin_socket, true );

	return xme_hal_net_writeDatagram(xme_core_net_plugin_socket, &add, buffer, count );
}

xme_core_status_t xme_core_net_plugin_join_channel (xme_core_dataChannel_t channelNumber)
{
	xme_core_interface_address_t add;

	CHANNEL_TO_MULTICAST_ADDRESS(channelNumber, add);

	return xme_hal_net_joinMulticastGroup( xme_core_net_plugin_socket, & add );
}

xme_core_status_t xme_core_net_plugin_leave_channel (xme_core_dataChannel_t channelNumber)
{
	xme_core_interface_address_t add;

	CHANNEL_TO_MULTICAST_ADDRESS(channelNumber, add);

	return xme_hal_net_leaveMulticastGroup( xme_core_net_plugin_socket, &add );
}

xme_hal_net_interfaceDescr_t* xme_core_net_plugin_get_interface(void)
{
	static xme_hal_net_interfaceDescr_t intf =
	{
		(xme_core_interface_interfaceId_t)0,
		xme_core_net_plugin_init,
		xme_core_net_plugin_fini,
		xme_core_net_plugin_read_non_blocking,
		xme_core_net_plugin_read_blocking,
		xme_core_net_plugin_write_non_blocking,
		xme_core_net_plugin_write_blocking,
		xme_core_net_plugin_join_channel,
		xme_core_net_plugin_leave_channel,
		xme_core_net_plugin_get_available_data_size,
		xme_core_net_plugin_wait_for_state_change,
		xme_core_net_plugin_register_callback,
		xme_core_net_plugin_clear_callback,
		xme_core_net_plugin_provide_channel,
		xme_core_net_plugin_unprovide_channel,
		xme_core_net_plugin_add_channel_to_phy_address_mapping,
		xme_core_net_plugin_remove_channel_to_phy_address_mapping
	};
	return &intf;
}

uint16_t xme_core_net_plugin_get_available_data_size (void)
{
	return xme_hal_net_get_available_data_size( xme_core_net_plugin_socket );
}

void xme_core_net_plugin_register_callback(xme_core_interface_callback_t cb)
{
	xme_core_net_plugin_callback = cb;
}

void xme_core_net_plugin_clear_callback(void)
{
	xme_core_net_plugin_callback = NULL;
}

void xme_core_net_plugin_interfaceCallback(xme_core_interface_state_t status)
{
	xme_core_interface_interfaceId_t id = (xme_core_net_plugin_get_interface())->interfaceID;
	if ( xme_core_net_plugin_callback )
	{
		xme_core_net_plugin_callback(status, id);
	}
}

xme_core_interface_state_t
xme_core_net_plugin_wait_for_state_change (void)
{
	xme_core_status_t status;
	status = XME_CORE_STATUS_TIMEOUT;
	
	while ( status != XME_CORE_STATUS_SUCCESS )
	{
		status = xme_hal_net_selectSocket( xme_core_net_plugin_socket, true, false, 60 );
		if ( status != XME_CORE_STATUS_SUCCESS && status != XME_CORE_STATUS_TIMEOUT )
		{
			return XME_CORE_INTERFACE_STATE_ERROR;
		}
	}

	return XME_CORE_INTERFACE_STATE_DATA_AVAILABLE;
}

void
xme_core_net_plugin_taskReceiveData(void* userData)
{
	xme_hal_net_socketHandle_t socketHandle;
	xme_core_status_t result;

	socketHandle = (xme_hal_net_socketHandle_t)userData;

	result = xme_hal_net_selectSocket(socketHandle, true, false, 1000);

	// Return if there are no data available
	XME_CHECK
	(
		XME_CORE_STATUS_TIMEOUT != result,
	);

	XME_LOG_IF
	(
		XME_CORE_STATUS_SUCCESS != result,
		XME_LOG_ERROR,
		"Unable to determine whether data have arrived!\n"
	);

	// New data is available, inform everybody
	if (xme_core_net_plugin_callback && xme_core_net_plugin_get_available_data_size())
	{
		xme_core_net_plugin_interfaceCallback(XME_CORE_INTERFACE_STATE_DATA_AVAILABLE);
	}
}

xme_core_status_t
xme_core_net_plugin_startWorkingThread
(
	xme_hal_net_socketHandle_t socketHandle
)
{
	XME_CHECK
	(
		XME_HAL_SCHED_INVALID_TASK_HANDLE == receiveDataTaskHandle,
		XME_CORE_STATUS_INTERNAL_ERROR
	);

	XME_CHECK
	(
		XME_HAL_SCHED_INVALID_TASK_HANDLE != (receiveDataTaskHandle =
			xme_hal_sched_addTask(0, 1, XME_HAL_SCHED_PRIORITY_NORMAL, xme_core_net_plugin_taskReceiveData, (void*)socketHandle)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_net_plugin_stopWorkingThread()
{
	XME_CHECK
	(
		XME_HAL_SCHED_INVALID_TASK_HANDLE != receiveDataTaskHandle,
		XME_CORE_STATUS_INTERNAL_ERROR
	);

	XME_CHECK
	(
		XME_CORE_STATUS_SUCCESS == xme_hal_sched_removeTask(receiveDataTaskHandle),
		XME_CORE_STATUS_INTERNAL_ERROR
	);

	receiveDataTaskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t xme_core_net_plugin_provide_channel(xme_core_dataChannel_t channel)
{
	// Nothing to do
	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t xme_core_net_plugin_unprovide_channel(xme_core_dataChannel_t channel)
{
	// Nothing to do
	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t xme_core_net_plugin_add_channel_to_phy_address_mapping (xme_core_dataChannel_t channel, xme_core_interface_address_t *address)
{
	// Nothing to do
	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t xme_core_net_plugin_remove_channel_to_phy_address_mapping (xme_core_dataChannel_t channel, xme_core_interface_address_t *address)
{
	// Nothing to do
	return XME_CORE_STATUS_SUCCESS;
}

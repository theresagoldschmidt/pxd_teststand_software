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
 *         Plugin for the interface manager to work with TCP/socket based networks.
 *
 * \author
 *         Hauke Staehle <staehle@fortiss.org>
 */

// TODO: (tickets #722, #726, #753, #754)
// - Check for race conditions. See ticket #753
// - Documentation. See ticket #722
// - Test development. See ticket #726
// - Integrate defines into cmake environment. See ticket #754

/*
 * Example usage:

 * In order to open a listening socket for publishing a certain topic, the
 * following code can be used. A subscriber then should connect to that socket
 * in order to receive the data.
 *
			xme_hal_table_rowHandle_t handle;
			xme_core_net_plugin_tcp_listening_t* item;
			xme_core_net_plugin_tcp_reader_t* readerItem;

			handle = XME_HAL_TABLE_ADD_ITEM ( xme_core_net_plugin_tcp_listeningTable );
			item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_net_plugin_tcp_listeningTable, handle);

			item->channel = 111;
			item->localHost = "localhost";
			item->localPort = 222;
			item->socketHandle = XME_HAL_NET_INVALID_SOCKET_HANDLE;

			xme_core_net_plugin_tcp_provide_channel( 111 );

 * In order to connect to a remote socket, which equals subscribing to a certain topic,
 * use the following piece of code:

			handle = XME_HAL_TABLE_ADD_ITEM ( xme_core_net_plugin_tcp_readerTable );
			readerItem = XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_core_net_plugin_tcp_readerTable, handle);

			readerItem->channel = 1000;
			readerItem->remoteHost = "192.168.17.2";
			readerItem->remotePort = 222;
			readerItem->socketHandle = XME_HAL_NET_INVALID_SOCKET_HANDLE;

			xme_core_net_plugin_tcp_join_channel( 1000 );

 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/net_plugin_tcp.h"
#include "xme/core/dataChannel.h"
#include "xme/hal/net.h"
#include "xme/hal/sched.h"

#include <stdint.h>

#include "xme/hal/console.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/

#define TCP_PLUGIN_READER_TABLE_SIZE 10
#define TCP_PLUGIN_LISTENING_TABLE_SIZE 10
#define TCP_PLUGIN_INBOUND_WRITER_TABLE_SIZE 10
#define TCP_PLUGIN_MAX_SELECT_SOCKETS 16
#define TCP_PLUGIN_CH_TP_PHY_MAPPING_TABLE_SIZE 10

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

typedef struct
{
	xme_core_dataChannel_t channel;
	char* hostName;
	uint16_t hostPort;
}
xme_core_net_plugin_tcp_ch_to_phy_mapping_t;

typedef struct
{
	xme_core_dataChannel_t channel;
	char* remoteHost;
	uint16_t remotePort;
	bool shouldConnect;
	xme_hal_net_socketHandle_t socketHandle;
}
xme_core_net_plugin_tcp_reader_t;

typedef struct
{
	xme_core_dataChannel_t channel;
	char* localHost;
	uint16_t localPort;
	xme_hal_net_socketHandle_t socketHandle;
}
xme_core_net_plugin_tcp_listening_t;

typedef struct
{
	xme_hal_net_socketHandle_t socketHandle;
	xme_core_dataChannel_t channel;
}
xme_core_net_plugin_tcp_writer_t;

XME_HAL_TABLE
(
	xme_core_net_plugin_tcp_reader_t,
	xme_core_net_plugin_tcp_readerTable,
	TCP_PLUGIN_READER_TABLE_SIZE
);

XME_HAL_TABLE
(
	xme_core_net_plugin_tcp_listening_t,
	xme_core_net_plugin_tcp_listeningTable,
	TCP_PLUGIN_LISTENING_TABLE_SIZE
);

XME_HAL_TABLE
(
	xme_core_net_plugin_tcp_writer_t,
	xme_core_net_plugin_tcp_writerTable,
	TCP_PLUGIN_INBOUND_WRITER_TABLE_SIZE
);

XME_HAL_TABLE
(
	xme_core_net_plugin_tcp_ch_to_phy_mapping_t,
	xme_core_net_plugin_tcp_ch_to_phy_mapping_table,
	TCP_PLUGIN_CH_TP_PHY_MAPPING_TABLE_SIZE
);

/**
 *  TODO: See ticket #722
 */
xme_core_status_t 
xme_core_net_plugin_tcp_startWorkingThread(void);

/**
 *  TODO: See ticket #722
 */
xme_core_status_t
xme_core_net_plugin_tcp_stopWorkingThread(void);

/**
 * These are the plugin interface prototypes
 */
xme_core_status_t xme_core_net_plugin_tcp_init (xme_core_interface_address_t *localAddress);
xme_core_status_t xme_core_net_plugin_tcp_fini (void);

uint16_t xme_core_net_plugin_tcp_read_non_blocking (void *buffer, uint16_t count, xme_core_dataChannel_t *channel);
uint16_t xme_core_net_plugin_tcp_read_blocking (void *buffer, uint16_t count, xme_core_dataChannel_t *channel);

uint16_t xme_core_net_plugin_tcp_write_non_blocking (const void *buffer, uint16_t count, xme_core_dataChannel_t channel);
uint16_t xme_core_net_plugin_tcp_write_blocking (const void *buffer, uint16_t count, xme_core_dataChannel_t channel);

xme_core_status_t xme_core_net_plugin_tcp_join_channel (xme_core_dataChannel_t channelNumber);
xme_core_status_t xme_core_net_plugin_tcp_leave_channel (xme_core_dataChannel_t channelNumber);

uint16_t xme_core_net_plugin_tcp_get_available_data_size (void);

xme_core_interface_state_t xme_core_net_plugin_tcp_wait_for_state_change (void);

void xme_core_net_plugin_tcp_register_callback(xme_core_interface_callback_t);
void xme_core_net_plugin_tcp_clear_callback(void);

xme_core_status_t xme_core_net_plugin_tcp_provide_channel(xme_core_dataChannel_t channel);
xme_core_status_t xme_core_net_plugin_tcp_unprovide_channel(xme_core_dataChannel_t channel);

xme_core_status_t xme_core_net_plugin_tcp_add_channel_to_phy_address_mapping (xme_core_dataChannel_t channel, xme_core_interface_address_t *address);
xme_core_status_t xme_core_net_plugin_tcp_remove_channel_to_phy_address_mapping (xme_core_dataChannel_t channel, xme_core_interface_address_t *address);

void xme_core_net_interfaceCallback(xme_core_interface_state_t status);

xme_core_status_t xme_core_net_plugin_tcp_hostname_and_port_for_channel(xme_core_dataChannel_t channel, char **hostname, uint16_t *port);

// Internal connection function
xme_hal_net_socketHandle_t connect_reader(xme_core_net_plugin_tcp_reader_t *readerItem);

/**
 * Handle for receive task
 */
xme_hal_sched_taskHandle_t workingTaskHandle;
xme_hal_sched_taskHandle_t connectingTaskHandle;

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
static xme_core_interface_callback_t xme_core_net_plugin_tcp_callback;

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_core_net_plugin_tcp_init (xme_core_interface_address_t *localAddress)
{
	// TODO: Error handling! See ticket #721
	
	xme_hal_net_initInterface(localAddress);

	XME_HAL_TABLE_INIT(xme_core_net_plugin_tcp_readerTable);
	XME_HAL_TABLE_INIT(xme_core_net_plugin_tcp_listeningTable);
	XME_HAL_TABLE_INIT(xme_core_net_plugin_tcp_writerTable);
	XME_HAL_TABLE_INIT(xme_core_net_plugin_tcp_ch_to_phy_mapping_table);

	workingTaskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;
	if ( xme_core_net_plugin_tcp_startWorkingThread() != XME_CORE_STATUS_SUCCESS )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

/*
	{
		xme_hal_table_rowHandle_t handle;
		xme_core_net_plugin_tcp_listening_t* item;
		xme_core_net_plugin_tcp_reader_t* readerItem;

		handle = XME_HAL_TABLE_ADD_ITEM ( xme_core_net_plugin_tcp_listeningTable );
		item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_net_plugin_tcp_listeningTable, handle);

		item->channel = 3333;
		item->localHost = "localhost";
		item->localPort = 3333;
		item->socketHandle = XME_HAL_NET_INVALID_SOCKET_HANDLE;

		xme_core_net_plugin_tcp_provide_channel( 3333, NULL );

		handle = XME_HAL_TABLE_ADD_ITEM ( xme_core_net_plugin_tcp_readerTable );
		readerItem = XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_core_net_plugin_tcp_readerTable, handle);

		readerItem->channel = 3333;
		readerItem->remoteHost = "192.168.17.190";
		readerItem->remotePort = 3333;
		readerItem->socketHandle = XME_HAL_NET_INVALID_SOCKET_HANDLE;

		///xme_core_net_plugin_tcp_join_channel( 1000 );
	}
*/

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t xme_core_net_plugin_tcp_fini (void)
{
	// TODO: Close all open connections. See ticket #755
	xme_hal_net_finiInterface(xme_core_net_plugin_tcp_get_interface());

	XME_HAL_TABLE_FINI(xme_core_net_plugin_tcp_readerTable);
	XME_HAL_TABLE_FINI(xme_core_net_plugin_tcp_listeningTable);
	XME_HAL_TABLE_FINI(xme_core_net_plugin_tcp_writerTable);
	XME_HAL_TABLE_INIT(xme_core_net_plugin_tcp_ch_to_phy_mapping_table);

	return XME_CORE_STATUS_SUCCESS;
}

uint16_t xme_core_net_plugin_tcp_read_non_blocking (void *buffer, uint16_t count, xme_core_dataChannel_t *channel)
{
	xme_core_status_t status;
	uint16_t i, maxNumber;
	xme_hal_net_socketHandle_t socketSetRead[16];
	uint16_t numBytes;

	status = XME_CORE_STATUS_TIMEOUT;

	i = 0;

	// Add all "connected" sockets	
	XME_HAL_TABLE_ITERATE
	(
		xme_core_net_plugin_tcp_writerTable,
		xme_hal_table_rowHandle_t, tableHandle,
		xme_core_net_plugin_tcp_writer_t, writerItem,
		{
			if ( writerItem->socketHandle != XME_HAL_NET_INVALID_SOCKET_HANDLE )
			{
				socketSetRead[i++] = writerItem->socketHandle;
			}				
		}
	);

	maxNumber = i;

	status = xme_hal_net_selectMultipleSockets( 
		maxNumber,
		socketSetRead,
		NULL,
		NULL,
		1
	);

	// Do not do anything on timeout
	if ( status != XME_CORE_STATUS_TIMEOUT )
	{
		return 0;
	}

	// Do not do anything on error
	if ( status != XME_CORE_STATUS_SUCCESS )
	{
		return 0;
	}

	// Check connected sockets
	for ( i = 0; i < maxNumber; i++ )
	{
		if ( socketSetRead[i] == XME_HAL_NET_INVALID_SOCKET_HANDLE ) continue;

		numBytes = xme_hal_net_readSocket( socketSetRead[i], buffer, count );

		if ( numBytes != 0 )
		{
			// TODO: Error checking. See ticket #721
			XME_HAL_TABLE_ITERATE
			(
				xme_core_net_plugin_tcp_writerTable,
				xme_hal_table_rowHandle_t, tableHandle,
				xme_core_net_plugin_tcp_writer_t, writerItem,
				{
					if ( writerItem->socketHandle == socketSetRead[i] )
					{
						*channel = writerItem->channel;
						return numBytes;
					}				
				}
			);

			// Lost data?
		}
	}

	return 0;
}

uint16_t xme_core_net_plugin_tcp_read_blocking (void *buffer, uint16_t count, xme_core_dataChannel_t *channel)
{
	xme_core_status_t status;
	uint16_t i, maxNumber;
	xme_hal_net_socketHandle_t socketSetRead[16];
	uint16_t numBytes;

	status = XME_CORE_STATUS_TIMEOUT;

	while ( status == XME_CORE_STATUS_TIMEOUT )
	{
		i = 0;

		// Add all "connected" sockets	
		XME_HAL_TABLE_ITERATE
		(
			xme_core_net_plugin_tcp_readerTable,
			xme_hal_table_rowHandle_t, tableHandle,
			xme_core_net_plugin_tcp_reader_t, readerItem,
			{
				if ( readerItem->socketHandle != XME_HAL_NET_INVALID_SOCKET_HANDLE )
				{
					socketSetRead[i++] = readerItem->socketHandle;
				}				
			}
		);

		maxNumber = i;

		status = xme_hal_net_selectMultipleSockets( 
			maxNumber,
			socketSetRead,
			NULL,
			NULL,
			1000
		);

		if ( status != XME_CORE_STATUS_TIMEOUT ) break;
	}

	
	// Do not do anything on error
	if ( status != XME_CORE_STATUS_SUCCESS )
	{
		return 0;
	}

	// Check connected sockets
	for ( i = 0; i < maxNumber; i++ )
	{
		if ( socketSetRead[i] == XME_HAL_NET_INVALID_SOCKET_HANDLE ) continue;

		numBytes = xme_hal_net_readSocket( socketSetRead[i], buffer, count );
		
		if ( numBytes != 0 )
		{
			// TODO: Error checking. See ticket #721
			XME_HAL_TABLE_ITERATE
			(
				xme_core_net_plugin_tcp_readerTable,
				xme_hal_table_rowHandle_t, tableHandle,
				xme_core_net_plugin_tcp_reader_t, readerItem,
				{
					if ( readerItem->socketHandle == socketSetRead[i] )
					{
						*channel = readerItem->channel;
						return numBytes;
					}				
				}
			);

			// Lost data?
		}
	}

	return 0;
}

uint16_t xme_core_net_plugin_tcp_write_non_blocking (const void *buffer, uint16_t count, xme_core_dataChannel_t channel)
{
	// Check all "connected" sockets and write data when channel matches
	XME_HAL_TABLE_ITERATE
	(
		xme_core_net_plugin_tcp_writerTable,
		xme_hal_table_rowHandle_t, tableHandle,
		xme_core_net_plugin_tcp_writer_t, writerItem,
		{			
			if ( writerItem->channel == channel )
			{
				if ( writerItem->socketHandle != XME_HAL_NET_INVALID_SOCKET_HANDLE )
				{
					//TODO: Error processing. See ticket #721
					xme_hal_net_setBlockingBehavior( writerItem->socketHandle, false );
					xme_hal_net_writeSocket( writerItem->socketHandle, buffer, count );
					xme_hal_net_setBlockingBehavior( writerItem->socketHandle, true );
				}
			}
			
		}
	);

	// TODO: what if some sockets succeeded and some failed? See ticket #748
	// What to do here?
	return count;
}

uint16_t xme_core_net_plugin_tcp_write_blocking (const void *buffer, uint16_t count, xme_core_dataChannel_t channel)
{
	// Check all "connected" sockets and write data when channel matches
	XME_HAL_TABLE_ITERATE
	(
		xme_core_net_plugin_tcp_writerTable,
		xme_hal_table_rowHandle_t, tableHandle,
		xme_core_net_plugin_tcp_writer_t, writerItem,
		{			
			if ( writerItem->channel == channel )
			{
				if ( writerItem->socketHandle != XME_HAL_NET_INVALID_SOCKET_HANDLE )
				{
					//TODO: Error processing. See ticket #721
					xme_hal_net_writeSocket( writerItem->socketHandle, buffer, count );
				}
			}		
		}
	);

	// TODO: what if some sockets succeeded and some failed? See ticket #748
	// What to do here?
	return count;
}

xme_core_status_t xme_core_net_plugin_tcp_join_channel (xme_core_dataChannel_t channelNumber)
{
	bool connectionOk = true;
	xme_hal_net_socketHandle_t socket;
	xme_hal_table_rowHandle_t handle;
	xme_core_net_plugin_tcp_reader_t* readerItem;

	handle = XME_HAL_TABLE_ADD_ITEM ( xme_core_net_plugin_tcp_readerTable );
	readerItem = XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_core_net_plugin_tcp_readerTable, handle);
	
	readerItem->socketHandle = XME_HAL_NET_INVALID_SOCKET_HANDLE;
	readerItem->shouldConnect = false;
	readerItem->channel = channelNumber;

	if ( xme_core_net_plugin_tcp_hostname_and_port_for_channel( channelNumber, &readerItem->remoteHost, &readerItem->remotePort ) != XME_CORE_STATUS_SUCCESS )
	{
		return XME_CORE_STATUS_NO_SUCH_VALUE;
	}

	socket = connect_reader( readerItem );
	if ( socket == XME_HAL_NET_INVALID_SOCKET_HANDLE )
	{
		readerItem->shouldConnect = true;
		connectionOk = false;
	} else {
		readerItem->socketHandle = socket;
	}

	return connectionOk ? XME_CORE_STATUS_SUCCESS : XME_CORE_STATUS_CONNECTION_REFUSED;
}

xme_core_status_t xme_core_net_plugin_tcp_leave_channel (xme_core_dataChannel_t channelNumber)
{
	// Check all oubtound sockets and destroy when channel and address matches
	char *iHost;
	uint16_t iPort;

	if ( xme_core_net_plugin_tcp_hostname_and_port_for_channel(channelNumber, &iHost, &iPort ) != XME_CORE_STATUS_SUCCESS )
	{
		return XME_CORE_STATUS_NO_SUCH_VALUE;
	}

	XME_HAL_TABLE_ITERATE
	(
		xme_core_net_plugin_tcp_readerTable,
		xme_hal_table_rowHandle_t, tableHandle,
		xme_core_net_plugin_tcp_reader_t, outboundTableItem,
		{			
			if ( outboundTableItem->channel == channelNumber )
			{
				if( outboundTableItem->remotePort==iPort && (strcmp(iHost,outboundTableItem->remoteHost) == 0) )
				{
					if ( outboundTableItem->socketHandle != XME_HAL_NET_INVALID_SOCKET_HANDLE )
					{
						//TODO: Error processing. See ticket #721
						xme_hal_net_closeSocket( outboundTableItem->socketHandle );
						xme_hal_net_destroySocket( outboundTableItem->socketHandle );
						outboundTableItem->socketHandle = XME_HAL_NET_INVALID_SOCKET_HANDLE;
					}
					xme_hal_mem_free( outboundTableItem->remoteHost );
					XME_HAL_TABLE_REMOVE_ITEM( xme_core_net_plugin_tcp_readerTable, tableHandle );
				}
			}			
		}
	);
	
	return XME_CORE_STATUS_SUCCESS;
}

xme_hal_net_interfaceDescr_t* xme_core_net_plugin_tcp_get_interface(void)
{
	static xme_hal_net_interfaceDescr_t intf =
	{
		(xme_core_interface_interfaceId_t)0,
		xme_core_net_plugin_tcp_init,
		xme_core_net_plugin_tcp_fini,
		xme_core_net_plugin_tcp_read_non_blocking,
		xme_core_net_plugin_tcp_read_blocking,
		xme_core_net_plugin_tcp_write_non_blocking,
		xme_core_net_plugin_tcp_write_blocking,
		xme_core_net_plugin_tcp_join_channel,
		xme_core_net_plugin_tcp_leave_channel,
		xme_core_net_plugin_tcp_get_available_data_size,
		xme_core_net_plugin_tcp_wait_for_state_change,
		xme_core_net_plugin_tcp_register_callback,
		xme_core_net_plugin_tcp_clear_callback,
		xme_core_net_plugin_tcp_provide_channel,
		xme_core_net_plugin_tcp_unprovide_channel,
		xme_core_net_plugin_tcp_add_channel_to_phy_address_mapping,
		xme_core_net_plugin_tcp_remove_channel_to_phy_address_mapping
	};
	return &intf;
}

uint16_t xme_core_net_plugin_tcp_get_available_data_size (void)
{
	xme_core_status_t status;
	uint16_t i, maxNumber;
	xme_hal_net_socketHandle_t socketSetRead[TCP_PLUGIN_MAX_SELECT_SOCKETS];

	status = XME_CORE_STATUS_TIMEOUT;

	i = 0;

	// Add all "connected" sockets	
	XME_HAL_TABLE_ITERATE
	(
		xme_core_net_plugin_tcp_readerTable,
		xme_hal_table_rowHandle_t, tableHandle,
		xme_core_net_plugin_tcp_reader_t, readerItem,
		{
			if ( readerItem->socketHandle != XME_HAL_NET_INVALID_SOCKET_HANDLE )
			{
				socketSetRead[i++] = readerItem->socketHandle;
			}				
		}
	);

	maxNumber = i;

	status = xme_hal_net_selectMultipleSockets( 
		maxNumber,
		socketSetRead,
		NULL,
		NULL,
		1
	);

	// Do not do anything on timeout
	if ( status == XME_CORE_STATUS_TIMEOUT )
	{
		return 0;
	}

	// Do not do anything on error
	if ( status != XME_CORE_STATUS_SUCCESS )
	{
		return 0;
	}

	// Check connected sockets
	for ( i = 0; i < maxNumber; i++ )
	{
		if ( socketSetRead[i] == XME_HAL_NET_INVALID_SOCKET_HANDLE ) continue;

		// TODO: Error checking. See ticket #721
		XME_HAL_TABLE_ITERATE
		(
			xme_core_net_plugin_tcp_readerTable,
			xme_hal_table_rowHandle_t, tableHandle,
			xme_core_net_plugin_tcp_reader_t, readerItem,
			{
				if ( readerItem->socketHandle == socketSetRead[i] )
				{
					return xme_hal_net_get_available_data_size( readerItem->socketHandle );
				}				
			}
		);

		// Lost data?
	}

	return 0;
}

void xme_core_net_plugin_tcp_register_callback(xme_core_interface_callback_t cb)
{
	xme_core_net_plugin_tcp_callback = cb;
}

void xme_core_net_plugin_tcp_clear_callback(void)
{
	xme_core_net_plugin_tcp_callback = NULL;
}

void xme_core_net_plugin_tcp_interfaceCallback(xme_core_interface_state_t status)
{
	xme_core_interface_interfaceId_t id = (xme_core_net_plugin_tcp_get_interface())->interfaceID;
	if ( xme_core_net_plugin_tcp_callback )
	{
		xme_core_net_plugin_tcp_callback(status, id);
	}
}

xme_core_interface_state_t
xme_core_net_plugin_tcp_wait_for_state_change (void)
{
	xme_core_status_t status;
	uint16_t i;
	xme_hal_net_socketHandle_t socketSetRead[16];

	status = XME_CORE_STATUS_TIMEOUT;

	while ( status != XME_CORE_STATUS_SUCCESS )
	{
		i = 0;
		
		// Check all "connected" sockets
		XME_HAL_TABLE_ITERATE
		(
			xme_core_net_plugin_tcp_readerTable,
			xme_hal_table_rowHandle_t, tableHandle,
			xme_core_net_plugin_tcp_reader_t, readerItem,
			{
				if ( readerItem->socketHandle != XME_HAL_NET_INVALID_SOCKET_HANDLE )
				{
					socketSetRead[i++] = readerItem->socketHandle;
				}				
			}
		);
		
		status = xme_hal_net_selectMultipleSockets( 
			i,
			socketSetRead,
			NULL,
			NULL,
			1000
		);

		if ( status != XME_CORE_STATUS_SUCCESS && status != XME_CORE_STATUS_TIMEOUT )
		{
			return XME_CORE_INTERFACE_STATE_ERROR;
		}
	}

	return XME_CORE_INTERFACE_STATE_DATA_AVAILABLE;
}

xme_hal_net_socketHandle_t connect_reader(xme_core_net_plugin_tcp_reader_t *readerItem)
{
	xme_hal_net_socketHandle_t socket;
	xme_core_status_t stat;

	socket = xme_hal_net_createSocket(
				xme_core_net_plugin_tcp_get_interface(),
				XME_HAL_NET_SOCKET_TCP,
				readerItem->remoteHost,
				readerItem->remotePort );
	stat = xme_hal_net_openSocket( socket );

	if ( stat != XME_CORE_STATUS_SUCCESS )
	{
		xme_hal_net_closeSocket( socket );
		xme_hal_net_destroySocket( socket );
		socket = XME_HAL_NET_INVALID_SOCKET_HANDLE;
	}

	return socket;
}

void
xme_core_net_plugin_tcp_connectingTask(void *userData)
{
	xme_hal_net_socketHandle_t socket;

	// Handle all "should" connect sockets
	XME_HAL_TABLE_ITERATE
	(
			xme_core_net_plugin_tcp_readerTable,
			xme_hal_table_rowHandle_t, tableHandle,
			xme_core_net_plugin_tcp_reader_t, readerItem,
		{
			if ( readerItem->shouldConnect == true )
			{
				socket = connect_reader(readerItem);
				if ( socket == XME_HAL_NET_INVALID_SOCKET_HANDLE ) {
					continue;
				} else {
					readerItem->shouldConnect = false;
					readerItem->socketHandle = socket;
				}
			}				
		}
	);
}

void
xme_core_net_plugin_tcp_taskWorking(void* userData)
{
	xme_core_status_t status;
	uint16_t i, j, listeningNumber, readerNumber, maxNumber, readerNumberError;
	xme_hal_net_socketHandle_t socketSetRead[TCP_PLUGIN_MAX_SELECT_SOCKETS];
	xme_hal_net_socketHandle_t socketSetError[TCP_PLUGIN_MAX_SELECT_SOCKETS];
									
	status = XME_CORE_STATUS_TIMEOUT;

	i = 0; j = 0;

	// First add all "listening" sockets
	XME_HAL_TABLE_ITERATE
	(
		xme_core_net_plugin_tcp_listeningTable,
		xme_hal_table_rowHandle_t, tableHandle,
		xme_core_net_plugin_tcp_listening_t, listeningItem,
		{
			if ( listeningItem->socketHandle != XME_HAL_NET_INVALID_SOCKET_HANDLE )
			{
				socketSetRead[i++] = listeningItem->socketHandle;
			}				
		}
	);

	// All entries in socketSetRead below this number are listening socketss
	listeningNumber = i;

	// Then add all "connected" sockets	
	XME_HAL_TABLE_ITERATE
	(
			xme_core_net_plugin_tcp_readerTable,
			xme_hal_table_rowHandle_t, tableHandle,
			xme_core_net_plugin_tcp_reader_t, readerItem,
		{
			if ( readerItem->socketHandle != XME_HAL_NET_INVALID_SOCKET_HANDLE )
			{
				socketSetError[j++] = readerItem->socketHandle;
				socketSetRead[i++] = readerItem->socketHandle;				
			}				
		}
	);

	// All entries in socketSetRead below this number and above listeningNumber are reading sockets
	readerNumber = i;
	readerNumberError = j;

	// We also add the write-sockets here in order to check for unallowed data delivery and to check
	// for broken connections
	XME_HAL_TABLE_ITERATE
	(
			xme_core_net_plugin_tcp_writerTable,
			xme_hal_table_rowHandle_t, tableHandle,
			xme_core_net_plugin_tcp_writer_t, writerItem,
		{
			if ( writerItem->socketHandle != XME_HAL_NET_INVALID_SOCKET_HANDLE )
			{
				socketSetRead[i++] = writerItem->socketHandle;				
			}				
		}
	);

	maxNumber = i;

	for ( ; j<maxNumber; j++ )
	{
		socketSetError[j] = XME_HAL_NET_INVALID_SOCKET_HANDLE;
	}

	status = xme_hal_net_selectMultipleSockets( 
		maxNumber,
		socketSetRead,
		NULL,
		socketSetError,
		500
	);

	// Do not do anything if timeout happened
	if ( status == XME_CORE_STATUS_TIMEOUT )
	{
		return;
	}

	// Do not do anything on error
	if ( status != XME_CORE_STATUS_SUCCESS )
	{
		return;
	}

	// Check error states for readers
	for ( i = 0; i < readerNumberError; i++ )
	{
		if ( socketSetError[i] == XME_HAL_NET_INVALID_SOCKET_HANDLE ) continue;
		
		XME_HAL_TABLE_ITERATE
		(
		xme_core_net_plugin_tcp_readerTable,
		xme_hal_table_rowHandle_t, tableHandle,
		xme_core_net_plugin_tcp_reader_t, readerItem,
			{
				if ( readerItem->socketHandle == socketSetError[i] )
				{
					xme_hal_net_closeSocket( socketSetError[i] );						
					xme_hal_net_destroySocket( socketSetError[i] );
					socketSetError[i] = XME_HAL_NET_INVALID_SOCKET_HANDLE;
					readerItem->socketHandle = XME_HAL_NET_INVALID_SOCKET_HANDLE;
					readerItem->shouldConnect = true;
				}				
			}
		);
	}

	// Check error states for writers
	for ( ; i < maxNumber; i++ )
	{
		if ( socketSetError[i] == XME_HAL_NET_INVALID_SOCKET_HANDLE ) continue;
		
		XME_HAL_TABLE_ITERATE
		(
		xme_core_net_plugin_tcp_writerTable,
		xme_hal_table_rowHandle_t, tableHandle,
		xme_core_net_plugin_tcp_writer_t, writerItem,
			{
				if ( writerItem->socketHandle == socketSetRead[i] )
				{
					xme_hal_net_closeSocket( socketSetError[i] );
					xme_hal_net_destroySocket( socketSetError[i] );
					socketSetError[i] = XME_HAL_NET_INVALID_SOCKET_HANDLE;
					writerItem->socketHandle = XME_HAL_NET_INVALID_SOCKET_HANDLE;
				}				
			}
		);
	}

	// Check listening sockets.
	for ( i = 0; i < listeningNumber; i++ )
	{
		if ( socketSetRead[i] == XME_HAL_NET_INVALID_SOCKET_HANDLE ) continue;

		// Accept the new connection
		{
			xme_hal_table_rowHandle_t handle;
			xme_core_net_plugin_tcp_writer_t* itemNew;

			// TODO: Error checking. See ticket #721
			handle = XME_HAL_TABLE_ADD_ITEM (xme_core_net_plugin_tcp_writerTable);
			itemNew = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_net_plugin_tcp_writerTable, handle);
			
			itemNew->channel = -1;

			// TODO: Error checking. See ticket #721
			XME_HAL_TABLE_ITERATE
			(
				xme_core_net_plugin_tcp_listeningTable,
				xme_hal_table_rowHandle_t, tableHandle,
				xme_core_net_plugin_tcp_listening_t, inboundItem,
				{
					if ( inboundItem->socketHandle == socketSetRead[i] )
					{
						itemNew->channel = inboundItem->channel;
						break;
					}				
				}
			);

			itemNew->socketHandle = xme_hal_net_waitForConnection( socketSetRead[i] );
		}
	}

	// Check connected sockets
	for ( ; i < maxNumber; i++ )
	{
		if ( socketSetRead[i] == XME_HAL_NET_INVALID_SOCKET_HANDLE ) continue;

		// Will call the callback for every available message.
		// TODO: Is that a good idea? (Fragmentation of packets) See ticket #756

		// If no data is available but an event was triggererd, this very likely means
		// that the connection is broken. So it is closed here.
		if ( !xme_hal_net_get_available_data_size( socketSetRead[i] ) )
		{
			XME_HAL_TABLE_ITERATE
			(
			xme_core_net_plugin_tcp_readerTable,
			xme_hal_table_rowHandle_t, tableHandle,
			xme_core_net_plugin_tcp_reader_t, readerItem,
				{
					if ( readerItem->socketHandle == socketSetRead[i] )
					{
						xme_hal_net_closeSocket( socketSetRead[i] );						
						xme_hal_net_destroySocket( socketSetRead[i] );
						socketSetRead[i] = XME_HAL_NET_INVALID_SOCKET_HANDLE;
						readerItem->socketHandle = XME_HAL_NET_INVALID_SOCKET_HANDLE;
						readerItem->shouldConnect = true;
					}				
				}
			);

			XME_HAL_TABLE_ITERATE
			(
			xme_core_net_plugin_tcp_writerTable,
			xme_hal_table_rowHandle_t, tableHandle,
			xme_core_net_plugin_tcp_writer_t, writerItem,
				{
					if ( writerItem->socketHandle == socketSetRead[i] )
					{
						xme_hal_net_closeSocket( socketSetRead[i] );
						xme_hal_net_destroySocket( socketSetRead[i] );
						socketSetRead[i] = XME_HAL_NET_INVALID_SOCKET_HANDLE;
						writerItem->socketHandle = XME_HAL_NET_INVALID_SOCKET_HANDLE;
					}				
				}
			);
		} else {

			if ( i < readerNumber )
			{
				xme_core_net_plugin_tcp_interfaceCallback(XME_CORE_INTERFACE_STATE_DATA_AVAILABLE);
			} else {
				uint16_t size;
				xme_hal_sharedPtr_t shmHandle;
				
				// A socket reserved for writing got data. The data is discarded.
				// TODO: Check for errors! See ticket #721
				size = xme_hal_net_get_available_data_size( socketSetRead[i] );
				shmHandle = xme_hal_sharedPtr_create(size);

				if (shmHandle == XME_HAL_SHAREDPTR_INVALID_POINTER)
				{
					continue;
				}

				xme_hal_net_readSocket( socketSetRead[i], xme_hal_sharedPtr_getPointer(shmHandle), size );
				xme_hal_sharedPtr_destroy( shmHandle );
			}
		}
	}	
}

xme_core_status_t
xme_core_net_plugin_tcp_startWorkingThread()
{
	XME_CHECK
	(
		XME_HAL_SCHED_INVALID_TASK_HANDLE == workingTaskHandle,
		XME_CORE_STATUS_INTERNAL_ERROR
	);

	XME_CHECK
	(
		XME_HAL_SCHED_INVALID_TASK_HANDLE == connectingTaskHandle,
		XME_CORE_STATUS_INTERNAL_ERROR
	);

	XME_CHECK
	(
		XME_HAL_SCHED_INVALID_TASK_HANDLE != (workingTaskHandle =
			xme_hal_sched_addTask(0, 1, XME_HAL_SCHED_PRIORITY_NORMAL, xme_core_net_plugin_tcp_taskWorking, NULL)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	XME_CHECK
	(
		XME_HAL_SCHED_INVALID_TASK_HANDLE != (connectingTaskHandle =
			xme_hal_sched_addTask(0, 1000, XME_HAL_SCHED_PRIORITY_NORMAL, xme_core_net_plugin_tcp_connectingTask, NULL)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_net_plugin_tcp_stopWorkingThread()
{
	XME_CHECK
	(
		XME_HAL_SCHED_INVALID_TASK_HANDLE != workingTaskHandle,
		XME_CORE_STATUS_INTERNAL_ERROR
	);

	XME_CHECK
	(
		XME_HAL_SCHED_INVALID_TASK_HANDLE != connectingTaskHandle,
		XME_CORE_STATUS_INTERNAL_ERROR
	);

	XME_CHECK
	(
		XME_CORE_STATUS_SUCCESS == xme_hal_sched_removeTask(connectingTaskHandle),
		XME_CORE_STATUS_INTERNAL_ERROR
	);

	XME_CHECK
	(
		XME_CORE_STATUS_SUCCESS == xme_hal_sched_removeTask(workingTaskHandle),
		XME_CORE_STATUS_INTERNAL_ERROR
	);

	workingTaskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t xme_core_net_plugin_tcp_provide_channel(xme_core_dataChannel_t channel)
{
	bool connectionOk = true;
	xme_hal_table_rowHandle_t handle;
	xme_core_net_plugin_tcp_listening_t* listeningItem;

	handle = XME_HAL_TABLE_ADD_ITEM ( xme_core_net_plugin_tcp_listeningTable );
	listeningItem = XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_core_net_plugin_tcp_listeningTable, handle);

	listeningItem->socketHandle = XME_HAL_NET_INVALID_SOCKET_HANDLE;
	listeningItem->channel = channel;
	if ( xme_core_net_plugin_tcp_hostname_and_port_for_channel( channel, &listeningItem->localHost, &listeningItem->localPort ) 
		!= XME_CORE_STATUS_SUCCESS )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	listeningItem->socketHandle = xme_hal_net_createSocket(
							xme_core_net_plugin_tcp_get_interface(),
							XME_HAL_NET_SOCKET_TCP,
							NULL,
							listeningItem->localPort );

	if ( listeningItem->socketHandle == XME_HAL_NET_INVALID_SOCKET_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}
	// TODO: Error processing. See ticket #721
	// Host has to be "NULL" in order to create a listening socket
	if ( xme_hal_net_openSocket( listeningItem->socketHandle ) != XME_CORE_STATUS_SUCCESS )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t xme_core_net_plugin_tcp_unprovide_channel(xme_core_dataChannel_t channel)
{
	// Check all "connected" sockets and destroy when channel matches
	XME_HAL_TABLE_ITERATE
	(
		xme_core_net_plugin_tcp_writerTable,
		xme_hal_table_rowHandle_t, tableHandle,
		xme_core_net_plugin_tcp_writer_t, writerItem,
		{			
			if ( writerItem->channel == channel )
			{
				if ( writerItem->socketHandle != XME_HAL_NET_INVALID_SOCKET_HANDLE )
				{
					//TODO: Error processing. See ticket #721
					xme_hal_net_closeSocket( writerItem->socketHandle );
					xme_hal_net_destroySocket( writerItem->socketHandle );
					writerItem->socketHandle = XME_HAL_NET_INVALID_SOCKET_HANDLE;
					XME_HAL_TABLE_REMOVE_ITEM( xme_core_net_plugin_tcp_writerTable, tableHandle );
				}
			}			
		}
	);

	// Check all "listening" sockets and destroy when channel matches
	XME_HAL_TABLE_ITERATE
	(
		xme_core_net_plugin_tcp_listeningTable,
		xme_hal_table_rowHandle_t, tableHandle,
		xme_core_net_plugin_tcp_listening_t, listenItem,
		{
			if ( listenItem->channel == channel )
			{
				if ( listenItem->socketHandle != XME_HAL_NET_INVALID_SOCKET_HANDLE )
				{
					// TODO: Error processing. See ticket #721
					xme_hal_net_closeSocket( listenItem->socketHandle );
					xme_hal_net_destroySocket( listenItem->socketHandle );
					listenItem->socketHandle = XME_HAL_NET_INVALID_SOCKET_HANDLE;
				}
				xme_hal_mem_free( listenItem->localHost );
				XME_HAL_TABLE_REMOVE_ITEM( xme_core_net_plugin_tcp_listeningTable, tableHandle );
			}
		}
	);
	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_net_plugin_tcp_add_channel_to_phy_address_mapping (xme_core_dataChannel_t channel,	xme_core_interface_address_t *address)
{
	xme_hal_table_rowHandle_t handle;
	xme_core_net_plugin_tcp_ch_to_phy_mapping_t* item;

	handle = XME_HAL_TABLE_ADD_ITEM ( xme_core_net_plugin_tcp_ch_to_phy_mapping_table );
	if ( handle == XME_HAL_TABLE_INVALID_ROW_HANDLE ) return XME_CORE_STATUS_OUT_OF_RESOURCES;

	item = XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_core_net_plugin_tcp_ch_to_phy_mapping_table, handle);

	item->channel = channel;
	GENERIC_ADDRESS_TO_HOST_TCP_MALLOC_TCP( *address, item->hostName );
	GENERIC_ADDRESS_TO_PORT_TCP( *address, item->hostPort );

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t xme_core_net_plugin_tcp_remove_channel_to_phy_address_mapping (xme_core_dataChannel_t channel, xme_core_interface_address_t *address)
{
	bool found = false;

	XME_HAL_TABLE_ITERATE
	(
		xme_core_net_plugin_tcp_ch_to_phy_mapping_table,
		xme_hal_table_rowHandle_t, tableHandle,
		xme_core_net_plugin_tcp_ch_to_phy_mapping_t, item,
		{
			if ( item->channel == channel )
			{
				uint16_t iPort;
				char* iHostName;
				GENERIC_ADDRESS_TO_PORT_TCP(*address, iPort );
				iHostName = GENERIC_ADDRESS_TO_HOST_TCP(*address);

				if( iPort == item->hostPort && (strcmp(item->hostName,iHostName)) == 0 )
				{
					XME_HAL_TABLE_REMOVE_ITEM( xme_core_net_plugin_tcp_ch_to_phy_mapping_table, tableHandle );
					found = true;
				}
			}
		}
	);

	return found ? XME_CORE_STATUS_SUCCESS : XME_CORE_STATUS_NO_SUCH_VALUE;
}

xme_core_status_t xme_core_net_plugin_tcp_hostname_and_port_for_channel(xme_core_dataChannel_t channel, char **hostname, uint16_t *port)
{
	XME_HAL_TABLE_ITERATE
	(
		xme_core_net_plugin_tcp_ch_to_phy_mapping_table,
		xme_hal_table_rowHandle_t, tableHandle,
		xme_core_net_plugin_tcp_ch_to_phy_mapping_t, item,
		{
			if ( item->channel == channel )
			{
				*hostname = item->hostName;
				*port = item->hostPort;
				return XME_CORE_STATUS_SUCCESS;
			}
		}
	);

	return XME_CORE_STATUS_NO_SUCH_VALUE;
}

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
 *         Interface manager.
 *
 * \author
 *         Hauke Stähle <staehle@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/interfaceManager.h"
#include "xme/core/defines.h"
#include "xme/core/net_plugin.h"
#include "xme/core/net_plugin_tcp.h"
#include "xme/core/packet.h"
#include "xme/core/nodeManager.h"
#include "xme/hal/sync.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/

// TODO: Integrate defines into cmake environment. See ticket #754
#define XME_CORE_INTERFACEMANAGER_MAXIMUM_JOINABLE_CHANNELS 16

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
void 
xme_core_interfaceManager_deviceCallback
(
	xme_core_interface_state_t status,
	xme_core_interface_interfaceId_t interfaceID
);

typedef struct
{
	xme_core_interface_interfaceId_t interfaceID;
	xme_core_dataChannel_t channelID;
	uint16_t channelJoinedTimes;
}
xme_core_interfaceManager_channelStatus_t;

xme_hal_sync_criticalSectionHandle_t criticalSectionHandle;

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
static xme_core_interfaceManager_callback_t xme_core_interfaceManager_callback = NULL;

XME_HAL_TABLE
(
	xme_hal_net_interfaceDescr_t*,
	xme_core_interfaceManager_interfaceTable,
	XME_CORE_INTERFACEMANAGER_INTERFACES_MAX
);

XME_HAL_TABLE
(
	xme_core_interfaceManager_channelStatus_t,
	xme_core_interfaceManager_channelStatusTable,
	XME_CORE_INTERFACEMANAGER_MAXIMUM_JOINABLE_CHANNELS
);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_core_interfaceManager_init()
{
	xme_hal_net_interfaceDescr_t *intf;
	
	XME_HAL_TABLE_INIT( xme_core_interfaceManager_interfaceTable );
	XME_HAL_TABLE_INIT( xme_core_interfaceManager_channelStatusTable );

	XME_CHECK
	(
		XME_HAL_SYNC_INVALID_CRITICAL_SECTION_HANDLE != (criticalSectionHandle = xme_hal_sync_createCriticalSection()),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	intf = xme_core_net_plugin_get_interface();
	XME_CHECK
	(
		intf->init(NULL) == XME_CORE_STATUS_SUCCESS,
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// Who should actually get the handle here?
	xme_core_interfaceManager_addInterface( intf );

	intf = xme_core_net_plugin_tcp_get_interface();
	XME_CHECK
	(
		intf->init(NULL) == XME_CORE_STATUS_SUCCESS,
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// Who should actually get the handle here?
	xme_core_interfaceManager_addInterface( intf );

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_core_interfaceManager_fini()
{

	XME_HAL_TABLE_ITERATE
	(
		xme_core_interfaceManager_interfaceTable,
		xme_hal_table_rowHandle_t, intfHandle,
		xme_hal_net_interfaceDescr_t *, intfItem,
		{
			if ( intfHandle != XME_CORE_INTERFACEMANAGER_INVALID_INTERFACE_ID )
			{
				(*intfItem)->fini();
			}
		}
	);

	xme_hal_sync_destroyCriticalSection(criticalSectionHandle);
	criticalSectionHandle = XME_HAL_SYNC_INVALID_CRITICAL_SECTION_HANDLE;

	XME_HAL_TABLE_FINI( xme_core_interfaceManager_channelStatusTable );
	XME_HAL_TABLE_FINI( xme_core_interfaceManager_interfaceTable );	
}

xme_hal_sharedPtr_t
xme_core_interfaceManager_read_non_blocking
(
	xme_core_interface_interfaceId_t interfaceRef,
	xme_core_dataChannel_t *channel
)
{
	uint16_t sampleSize;
	uint16_t payloadSize;

	xme_hal_sharedPtr_t shmHandle;

	xme_core_packet_sample_t sample;
	xme_core_packet_sample_header_t* sampleHeader;
	void* samplePayload;

	xme_hal_net_interfaceDescr_t* intf =  *XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_interfaceManager_interfaceTable, interfaceRef);
	if ( intf == NULL )
	{
		return XME_HAL_SHAREDPTR_INVALID_POINTER;
	}

	sampleSize = xme_core_interfaceManager_get_available_data_size(interfaceRef);
	sample = xme_hal_sharedPtr_create(sampleSize);
	if (sample == XME_HAL_SHAREDPTR_INVALID_POINTER)
	{
		return XME_HAL_SHAREDPTR_INVALID_POINTER;
	}

	if ( !sampleSize )
	{
		return sample;
	}
	sampleSize = intf->read_non_blocking(xme_hal_sharedPtr_getPointer(sample),sampleSize,channel);

	sampleHeader = xme_core_packet_sample_header(sample);
	samplePayload = xme_core_packet_sample_payload(sample);

	if (!XME_CORE_PACKET_VALID(*sampleHeader))
	{
		XME_LOG
		(
			XME_LOG_DEBUG, "Discarding packet with invalid header (magic=0x%02X, ver=0x%02X, type=0x%02X)\n",
			sampleHeader->packetHeader.magic, sampleHeader->packetHeader.version, sampleHeader->packetHeader.packetType
		);

		xme_hal_sharedPtr_destroy(sample);
		return XME_HAL_SHAREDPTR_INVALID_POINTER;
	}
	else if ( sampleHeader->senderGuid == xme_core_nodeManager_getDeviceGuid())
	{
		XME_LOG
		(
			XME_LOG_DEBUG, "Discarding own packet (magic=0x%02X, ver=0x%02X, type=0x%02X)\n",
			sampleHeader->packetHeader.magic, sampleHeader->packetHeader.version, sampleHeader->packetHeader.packetType
		);

		xme_hal_sharedPtr_destroy(sample);
		return XME_HAL_SHAREDPTR_INVALID_POINTER;
	}

	payloadSize = sampleSize - sizeof(xme_core_packet_sample_header_t);
	shmHandle = xme_hal_sharedPtr_create( payloadSize );
	if (shmHandle == XME_HAL_SHAREDPTR_INVALID_POINTER)
	{
		xme_core_packet_sample_destroy( sample );
		return XME_HAL_SHAREDPTR_INVALID_POINTER;
	}

	memcpy(
		xme_hal_sharedPtr_getPointer(shmHandle),
		samplePayload,
		payloadSize
		);

	xme_core_packet_sample_destroy( sample );

	return shmHandle;
}


xme_hal_sharedPtr_t
xme_core_interfaceManager_read_blocking
(
	xme_core_interface_interfaceId_t interfaceRef,
	xme_core_dataChannel_t *channel
)
{
	uint16_t sampleSize;
	uint16_t payloadSize;

	xme_hal_sharedPtr_t shmHandle;

	xme_core_interface_state_t intStatus;

	xme_core_packet_sample_t sample;
	xme_core_packet_sample_header_t* sampleHeader;
	void* samplePayload;

	xme_hal_net_interfaceDescr_t* intf =  *XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_interfaceManager_interfaceTable, interfaceRef);
	if ( intf == NULL )
	{
		return XME_HAL_SHAREDPTR_INVALID_POINTER;
	}

	while ( 1 )
	{
		sampleSize = 0;
		while ( !sampleSize )
		{
			intStatus = intf->wait_for_state_change();
			if ( intStatus != XME_CORE_INTERFACE_STATE_DATA_AVAILABLE )
			{
				return XME_HAL_SHAREDPTR_INVALID_POINTER;
			}
			sampleSize = xme_core_interfaceManager_get_available_data_size(interfaceRef);
		}

		sample = xme_core_packet_sample_create_totalSize(sampleSize);
		if (sample == XME_HAL_SHAREDPTR_INVALID_POINTER)
		{
			return XME_HAL_SHAREDPTR_INVALID_POINTER;
		}

		sampleSize = intf->read_blocking(xme_hal_sharedPtr_getPointer(sample),sampleSize,channel);

		sampleHeader = xme_core_packet_sample_header(sample);
		samplePayload = xme_core_packet_sample_payload(sample);

		if (!XME_CORE_PACKET_VALID(*sampleHeader))
		{
			XME_LOG
			(
				XME_LOG_DEBUG, "Discarding packet with invalid header (magic=0x%02X, ver=0x%02X, type=0x%02X)\n",
				sampleHeader->packetHeader.magic, sampleHeader->packetHeader.version, sampleHeader->packetHeader.packetType
			);
		}
		else if ( sampleHeader->senderGuid == xme_core_nodeManager_getDeviceGuid())
		{
			XME_LOG
			(
				XME_LOG_DEBUG, "Discarding own packet (magic=0x%02X, ver=0x%02X, type=0x%02X)\n",
				sampleHeader->packetHeader.magic, sampleHeader->packetHeader.version, sampleHeader->packetHeader.packetType
			);
		}
		else
		{
			// Valid packet not issued by this node
			break;
		}

		xme_core_packet_sample_destroy(sample);
	}


	payloadSize = sampleSize - sizeof(xme_core_packet_sample_header_t);
	shmHandle = xme_hal_sharedPtr_create( payloadSize );
	if (shmHandle == XME_HAL_SHAREDPTR_INVALID_POINTER)
	{
		xme_core_packet_sample_destroy( sample );
		return XME_HAL_SHAREDPTR_INVALID_POINTER;
	}

	memcpy(
		xme_hal_sharedPtr_getPointer(shmHandle),
		samplePayload,
		payloadSize
		);

	xme_core_packet_sample_destroy( sample );

	return shmHandle;
}

uint16_t
xme_core_interfaceManager_write_non_blocking
(
	xme_core_interface_interfaceId_t interfaceRef,
	xme_core_dataChannel_t channel,
	xme_hal_sharedPtr_t shmHandle
)
{
	xme_core_packet_sample_t sample;
	xme_core_packet_sample_header_t* sampleHeader;
	void* samplePayload;
	uint16_t sentSize;

	xme_hal_net_interfaceDescr_t* intf =  *XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_interfaceManager_interfaceTable, interfaceRef);
	if ( intf == NULL )
	{
		return 0;
	}

	sample = xme_core_packet_sample_create_payloadSize( xme_hal_sharedPtr_getSize(shmHandle) );
	if (sample == XME_HAL_SHAREDPTR_INVALID_POINTER)
	{
		return XME_CORE_STATUS_OUT_OF_RESOURCES;
	}

	sampleHeader = xme_core_packet_sample_header( sample );
	samplePayload = xme_core_packet_sample_payload( sample );

	sampleHeader->senderGuid = xme_core_nodeManager_getDeviceGuid();

	memcpy(
		samplePayload,
		xme_hal_sharedPtr_getPointer(shmHandle),
		xme_hal_sharedPtr_getSize(shmHandle)
	);

	sentSize = intf->write_non_blocking(
		xme_hal_sharedPtr_getPointer(sample),
		xme_hal_sharedPtr_getSize(sample),
		channel);

	xme_core_packet_sample_destroy( sample );

	return sentSize;
}

uint16_t
xme_core_interfaceManager_write_blocking
(
	xme_core_interface_interfaceId_t interfaceRef,
	xme_core_dataChannel_t channel,
	xme_hal_sharedPtr_t shmHandle
)
{
	uint16_t payloadSize = xme_hal_sharedPtr_getSize(shmHandle);
	xme_hal_net_interfaceDescr_t* intf =  *XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_core_interfaceManager_interfaceTable, interfaceRef );
	uint16_t alreadySent = 0;
	uint16_t justSent = 0;
	uint16_t sampleSize = 0;

	xme_core_packet_sample_t sample;
	xme_core_packet_sample_header_t* sampleHeader;
	void* samplePayload;

	if ( intf == NULL ) return 0;

	sample = xme_core_packet_sample_create_payloadSize( payloadSize );
	if (sample == XME_HAL_SHAREDPTR_INVALID_POINTER)
	{
		return XME_CORE_STATUS_OUT_OF_RESOURCES;
	}

	sampleHeader = xme_core_packet_sample_header( sample );
	samplePayload = xme_core_packet_sample_payload( sample );
	sampleSize = xme_hal_sharedPtr_getSize( sample );

	sampleHeader->senderGuid = xme_core_nodeManager_getDeviceGuid();

	memcpy(
		samplePayload,
		xme_hal_sharedPtr_getPointer(shmHandle),
		xme_hal_sharedPtr_getSize(shmHandle)
	);

	while ( alreadySent != sampleSize )
	{
		justSent = intf->write_blocking(
			xme_hal_sharedPtr_getPointer(sample),
			xme_hal_sharedPtr_getSize(sample),
			channel);

		// If nothing is sent in blocking mode, something went wrong.
		if ( justSent == 0 ) 
		{
			break;
		}

		alreadySent += justSent;
	}

	xme_core_packet_sample_destroy( sample );
	
	return alreadySent;
}

xme_core_status_t
xme_core_interfaceManager_join_channel
(
	xme_core_interface_interfaceId_t interfaceRef,
	xme_core_dataChannel_t channelNumber
)
{
	xme_core_status_t status;
	xme_hal_table_rowHandle_t row;
	xme_core_interfaceManager_channelStatus_t* channelRow;

	xme_hal_net_interfaceDescr_t* intf = *XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_core_interfaceManager_interfaceTable, interfaceRef );
	if ( intf == NULL ) return XME_CORE_STATUS_NO_SUCH_VALUE;

	xme_hal_sync_enterCriticalSection(criticalSectionHandle);
	{
		/* Check if already joined channel */
		XME_HAL_TABLE_ITERATE
		(
			xme_core_interfaceManager_channelStatusTable,
			xme_hal_table_rowHandle_t, channelStatusHandel,
			xme_core_interfaceManager_channelStatus_t, channelStatusItem,
			{
				if ( channelStatusItem->interfaceID == interfaceRef &&
					channelStatusItem->channelID == channelNumber )
				{
					// Unfortunately it is not enough to handle duplicate channel joins at this level.
					// This is why a similar mechanism has been implemented in the routing table in r683
					// (compare also Ticket #899). The assertion below ensures that the routing table
					// code works correctly, because we should never see duplicate channel joins at this
					// level.
					XME_ASSERT(false);

					channelStatusItem->channelJoinedTimes++;
					xme_hal_sync_leaveCriticalSection(criticalSectionHandle);
					return XME_CORE_STATUS_SUCCESS;
				}
			}
		);
	}
	xme_hal_sync_leaveCriticalSection(criticalSectionHandle);

	status = intf->join_channel(channelNumber);
	if ( status != XME_CORE_STATUS_SUCCESS )
	{
		return status;
	}

	xme_hal_sync_enterCriticalSection(criticalSectionHandle);
	{
		row = XME_HAL_TABLE_ADD_ITEM( xme_core_interfaceManager_channelStatusTable );
		channelRow = XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_core_interfaceManager_channelStatusTable, row );
	}
	xme_hal_sync_leaveCriticalSection(criticalSectionHandle);

	if ( row != XME_HAL_TABLE_INVALID_ROW_HANDLE ) {
		channelRow->channelID = channelNumber;
		channelRow->channelJoinedTimes = 1;
		channelRow->interfaceID = interfaceRef;
	} else {
		(void)(intf->leave_channel(channelNumber));
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_interfaceManager_leave_channel
(
	xme_core_interface_interfaceId_t interfaceRef,
	xme_core_dataChannel_t channelNumber
)
{
	xme_core_interfaceManager_channelStatus_t* _channelStatusItem;
	xme_hal_table_rowHandle_t _channelStatusHandle;

	xme_hal_net_interfaceDescr_t* intf =  *XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_core_interfaceManager_interfaceTable, interfaceRef );
	if ( intf == NULL ) return XME_CORE_STATUS_NO_SUCH_VALUE;

	/* Search for channel */
	_channelStatusHandle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
	xme_hal_sync_enterCriticalSection(criticalSectionHandle);
	{
		XME_HAL_TABLE_ITERATE
		(
			xme_core_interfaceManager_channelStatusTable,
			xme_hal_table_rowHandle_t, channelStatusHandle,
			xme_core_interfaceManager_channelStatus_t, channelStatusItem,
			{
				if ( channelStatusItem->interfaceID == interfaceRef &&
					channelStatusItem->channelID == channelNumber )
				{
					_channelStatusItem = channelStatusItem;
					_channelStatusHandle = channelStatusHandle;
				}
			}
		);
	}
	xme_hal_sync_leaveCriticalSection(criticalSectionHandle);

	if ( _channelStatusHandle == XME_HAL_TABLE_INVALID_ROW_HANDLE )
	{
		return XME_CORE_STATUS_NO_SUCH_VALUE;
	}

	// Unfortunately it is not enough to handle duplicate channel joins at this level.
	// This is why a similar mechanism has been implemented in the routing table in r683
	// (compare also Ticket #899). The assertion below ensures that the routing table
	// code works correctly, because we should never see duplicate channel joins at this
	// level.
	XME_ASSERT(1 == _channelStatusItem->channelJoinedTimes);

	_channelStatusItem->channelJoinedTimes--;
	if ( _channelStatusItem->channelJoinedTimes )
	{
		return XME_CORE_STATUS_SUCCESS;
	}
	
	xme_hal_sync_enterCriticalSection(criticalSectionHandle);
	{
		XME_HAL_TABLE_REMOVE_ITEM( xme_core_interfaceManager_channelStatusTable, (xme_hal_table_rowHandle_t)_channelStatusHandle );
	}
	xme_hal_sync_leaveCriticalSection(criticalSectionHandle);

	return intf->leave_channel(channelNumber);
}

xme_core_interface_interfaceId_t
xme_core_interfaceManager_addInterface
(
	xme_hal_net_interfaceDescr_t* interfaceDesc
)
{
	xme_hal_table_rowHandle_t row;
	xme_hal_net_interfaceDescr_t** intRow;

	row = XME_HAL_TABLE_ADD_ITEM( xme_core_interfaceManager_interfaceTable );
	intRow = XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_core_interfaceManager_interfaceTable, row );

	if ( row != XME_CORE_INTERFACEMANAGER_INVALID_INTERFACE_ID ) {
		*intRow = interfaceDesc;
		interfaceDesc->interfaceID = (xme_core_interface_interfaceId_t)row;
		interfaceDesc->register_callback(xme_core_interfaceManager_deviceCallback);
		return (xme_core_interface_interfaceId_t)row;
	}

	return XME_CORE_INTERFACEMANAGER_INVALID_INTERFACE_ID;
}

void
xme_core_interfaceManager_removeInterface
(
	xme_core_interface_interfaceId_t interfaceRef
)
{
	xme_hal_net_interfaceDescr_t* intf = *XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_core_interfaceManager_interfaceTable, interfaceRef );
	if ( intf == NULL ) return;

	intf->clear_callback();
	XME_HAL_TABLE_REMOVE_ITEM( xme_core_interfaceManager_interfaceTable, (xme_hal_table_rowHandle_t)interfaceRef );
}

uint16_t
xme_core_interfaceManager_get_available_data_size
(
	xme_core_interface_interfaceId_t interfaceHandle
)
{
	xme_hal_net_interfaceDescr_t* intf = *XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_core_interfaceManager_interfaceTable, interfaceHandle );

	if ( intf == NULL ) return 0;
	return intf->get_available_data_size();
}

void
xme_core_interfaceManager_registerCallback
(
	xme_core_interfaceManager_callback_t cb
)
{
	xme_core_interfaceManager_callback = cb;
}

void
xme_core_interfaceManager_clearCallback()
{
	xme_core_interfaceManager_callback = NULL;
}

void
xme_core_interfaceManager_deviceCallback
(
	xme_core_interface_state_t status,
	xme_core_interface_interfaceId_t interfaceID
)
{
	if ( xme_core_interfaceManager_callback )
	{
		xme_core_interfaceManager_callback(status, interfaceID);
	}
}

xme_core_status_t
xme_core_interfaceManager_provide_channel
(
	xme_core_interface_interfaceId_t interfaceRef,
	xme_core_dataChannel_t channelNumber
)
{
	xme_hal_net_interfaceDescr_t* intf = *XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_core_interfaceManager_interfaceTable, interfaceRef );

	if ( intf == NULL ) return 0;
	return intf->provide_channel(channelNumber);
}

xme_core_status_t
xme_core_interfaceManager_unprovide_channel
(
	xme_core_interface_interfaceId_t interfaceRef,
	xme_core_dataChannel_t channelNumber
)
{
	xme_hal_net_interfaceDescr_t* intf = *XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_core_interfaceManager_interfaceTable, interfaceRef );

	if ( intf == NULL ) return 0;
	return intf->unprovide_channel(channelNumber);
}

// TODO: See ticket #729
xme_core_status_t
xme_core_interfaceManager_add_channel_to_phy_address_mapping (
	xme_core_interface_interfaceId_t interfaceRef,
	xme_core_dataChannel_t channel,
	xme_core_interface_address_t *address
)
{
	xme_hal_net_interfaceDescr_t* intf = *XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_core_interfaceManager_interfaceTable, interfaceRef );

	if ( intf == NULL ) return 0;
	return intf->add_channel_to_phy_address_mapping(channel,address);
}

// TODO: See ticket #729
xme_core_status_t
xme_core_interfaceManager_remove_channel_to_phy_address_mapping (
	xme_core_interface_interfaceId_t interfaceRef,
	xme_core_dataChannel_t channel,
	xme_core_interface_address_t *address
)
{
	xme_hal_net_interfaceDescr_t* intf = *XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_core_interfaceManager_interfaceTable, interfaceRef );

	if ( intf == NULL ) return 0;
	return intf->remove_channel_to_phy_address_mapping(channel,address);
}

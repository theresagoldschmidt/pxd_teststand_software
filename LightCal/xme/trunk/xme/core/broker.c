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
 *         Broker.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Stephan Sommer <sommer@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/broker.h"
#include "xme/core/defines.h"
#include "xme/core/interfaceManager.h"
#include "xme/core/interface.h"
#include "xme/hal/sharedPtr.h"

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_core_broker_init()
{
	xme_core_interfaceManager_registerCallback(xme_core_broker_interfaceManager_callback);

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_core_broker_fini()
{
	xme_core_interfaceManager_clearCallback();
}

xme_core_status_t
xme_core_broker_deliverData
(
	xme_core_dataChannel_t dataChannel,
	xme_hal_sharedPtr_t dataHandle,
	xme_core_component_t sendingComponent
)
{
	XME_ASSERT(XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != dataChannel);
	XME_ASSERT(XME_HAL_SHAREDPTR_INVALID_POINTER != dataHandle);
	XME_ASSERT(XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT != sendingComponent);

	xme_core_broker_deliverDataToComponent(dataChannel, dataHandle, sendingComponent);
	xme_core_broker_deliverDataToNetwork(dataChannel, dataHandle);
	xme_core_broker_deleiverDataToTransformation(dataChannel, dataHandle, sendingComponent);
	
	return XME_CORE_STATUS_SUCCESS;
}


// delivers data to transformation
xme_core_status_t
xme_core_broker_deleiverDataToTransformation
(
	xme_core_dataChannel_t dataChannel,
	xme_hal_sharedPtr_t dataHandle,
	xme_core_component_t sendingComponent
)
{
	XME_HAL_TABLE_ITERATE
	(
		xme_core_routingTable_config.translationRoutes,
		xme_hal_table_rowHandle_t, handle,
		xme_core_routingTable_translationRoute_t, item,
		{
			if (item->sourceDataChannel == dataChannel)
			{
				// TODO: do we want to allow recusion? See ticket #742
				xme_core_broker_deliverData(item->destinationDataChannel, dataHandle, sendingComponent);
			}
		}
	);
	
	return XME_CORE_STATUS_SUCCESS;
}

// delivers data to network
xme_core_status_t
xme_core_broker_deliverDataToNetwork
(
	xme_core_dataChannel_t dataChannel,
	xme_hal_sharedPtr_t dataHandle
)
{
	// network delivery; iterate through table and notify all "listeners"
	XME_HAL_TABLE_ITERATE
	(
		xme_core_routingTable_config.outboundRoutes,
		xme_hal_table_rowHandle_t, handle,
		xme_core_routingTable_outboundRoute_t, item,
		{ 
			// send to interface manager
			if (item->dataChannel == dataChannel)
			{
				xme_core_interfaceManager_write_blocking( item->viaInterface, item->dataChannel, dataHandle );
			}
		}
	);

	return XME_CORE_STATUS_SUCCESS;
}


// delivers data to component
xme_core_status_t
xme_core_broker_deliverDataToComponent
(
	xme_core_dataChannel_t sourceDataChannel,
	xme_hal_sharedPtr_t dataHandle,
	xme_core_component_t sendingComponent
)
{
	// local delivery; iterate through table and notify all "listeners"
	XME_HAL_TABLE_ITERATE
	(
		xme_core_routingTable_config.localDestinationRoutes,
		xme_hal_table_rowHandle_t, handle,
		xme_core_routingTable_localDestinationRoute_t, item,
		{
			if (item->dataChannel == sourceDataChannel)
			{
				// Skip looping messages
				//
				// TODO (See ticket #744): This looks like a hack. The use case of subscribing to
				//       something that is published by the same component is rather rare and shall
				//       be handled in the component. This requires the component to be
				//       able to determine whether a received message was actually sent
				//       by the same component or not (this is not possible without
				//       further information than just the message content).
				//       When this is realized, we should also remove the "sendingComponent"
				//       parameter from xme_core_broker_deliverData() and related functions.
/*
				// This is to prevent the delivery of a message to a component that itself
				// sent that message.
				if ( (sendingComponent != XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT) &&
					(sendingComponent == item->toComponent) )
				{
					continue;
				}
*/

				// Tell resource manager to notify component
				xme_core_resourceManager_notifyComponent(sourceDataChannel, item->toComponent, item->toPort, dataHandle);
			}
		}
	);

	return XME_CORE_STATUS_SUCCESS;
}

// data ready at network interface
static void
xme_core_broker_interfaceManager_callback
(
	xme_core_interface_state_t status,
	xme_core_interface_interfaceId_t interfaceHandle
)
{
	xme_core_dataChannel_t channel;
	xme_hal_sharedPtr_t dataHandle;

	// Check if status change was because new data is available
	if ( status != XME_CORE_INTERFACE_STATE_DATA_AVAILABLE )
		return;

	dataHandle = xme_core_interfaceManager_read_blocking(interfaceHandle,  &channel);
	xme_core_broker_deliverDataToComponent(channel, dataHandle, XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT);
	xme_core_broker_deleiverDataToTransformation(channel, dataHandle, XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT);

	// xme_core_interfaceManager_read_blocking() returns a memory handle that needs to be freed here.
	xme_hal_sharedPtr_destroy(dataHandle);
}

xme_core_status_t
xme_core_broker_dccSendTopicData
(
	xme_core_component_t sendingComponent,
	xme_core_component_port_t fromPublicationPort,
	xme_hal_sharedPtr_t dataHandle
)
{
	xme_core_dataChannel_t dataChannel;
	bool requestForwarded = false;

	XME_HAL_TABLE_ITERATE
	(
		xme_core_routingTable_config.localSourceRoutes,
		xme_hal_table_rowHandle_t, handle,
		xme_core_routingTable_localSourceRoute_t, item,
		{
			if ((item->fromComponent == sendingComponent) && (item->fromPort == fromPublicationPort))
			{
				dataChannel = item->dataChannel;
				xme_core_broker_deliverData(dataChannel, dataHandle, sendingComponent);

				requestForwarded = true;

				// TODO: transformation route???!!! See ticket #742

				// The routing table might contain multiple items for the current component and port.
				// Hence, we must not stop after the first match.
			}
		}
	);

	return requestForwarded ? XME_CORE_STATUS_SUCCESS : XME_CORE_STATUS_NOT_FOUND;
}

xme_core_status_t
xme_core_broker_rrSendRequest
(
	xme_core_component_t sendingComponent,
	xme_core_component_port_t fromRequestPort,
	xme_hal_sharedPtr_t dataHandle
)
{
	xme_core_dataChannel_t dataChannel;
	bool requestForwarded = false;

	XME_HAL_TABLE_ITERATE
	(
		xme_core_routingTable_config.localSourceRoutes,
		xme_hal_table_rowHandle_t, handle,
		xme_core_routingTable_localSourceRoute_t, item,
		{
			if ((item->fromComponent == sendingComponent) && (item->fromPort == fromRequestPort))
			{
				dataChannel = item->dataChannel;
				xme_core_broker_deliverData(dataChannel, dataHandle, sendingComponent);

				requestForwarded = true;

				// TODO: transformation route???!!! See ticket #742

				// The routing table might contain multiple items for the current component and port.
				// Hence, we must not stop after the first match.
			}
		}
	);

	return requestForwarded ? XME_CORE_STATUS_SUCCESS : XME_CORE_STATUS_NOT_FOUND;
}

xme_core_status_t
xme_core_broker_rrSendResponse
(
	xme_core_dataChannel_t responseDataChannel,
	xme_core_component_t sendingComponent,
	xme_core_component_port_t fromRequestHandlerPort,
	xme_hal_sharedPtr_t dataHandle
)
{
	xme_core_status_t status;

	#ifndef NDEBUG
		uint16_t numMatches = 0;

		XME_HAL_TABLE_ITERATE
		(
			xme_core_routingTable_config.localSourceRoutes,
			xme_hal_table_rowHandle_t, handle,
			xme_core_routingTable_localSourceRoute_t, item,
			{
				if ((item->fromComponent == sendingComponent) && (item->fromPort == fromRequestHandlerPort) && (item->dataChannel == responseDataChannel))
				{
					numMatches++;
				}
			}
		);

		XME_ASSERT(1 == numMatches);
	#endif // #ifndef NDEBUG

	status = xme_core_broker_deliverData(responseDataChannel, dataHandle, sendingComponent);

	return status;
}

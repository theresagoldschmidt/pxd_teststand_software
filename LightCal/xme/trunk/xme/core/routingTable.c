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
 *         Routing table.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <string.h>
#include "xme/core/routingTable.h"

#include "xme/core/interfaceManager.h"
#include "xme/core/component.h"
#include "xme/core/defines.h"
#include "xme/core/resourceManager.h"

#include "xme/hal/table.h"


/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
// TODO: Make static. See ticket #791
xme_core_routingTable_configStruct_t xme_core_routingTable_config;


void*
xme_core_resourceManager_getInterfaceCallback
(
	xme_core_interface_interfaceId_t toInterface
)
{
	// TODO: See ticket #772

	return NULL;
}



xme_core_status_t
xme_core_routingTable_init()
{
	XME_HAL_TABLE_INIT(xme_core_routingTable_config.localSourceRoutes);
	XME_HAL_TABLE_INIT(xme_core_routingTable_config.localDestinationRoutes);
	XME_HAL_TABLE_INIT(xme_core_routingTable_config.outboundRoutes);
	XME_HAL_TABLE_INIT(xme_core_routingTable_config.translationRoutes);
	XME_HAL_TABLE_INIT(xme_core_routingTable_config.inboundRoutes);

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_core_routingTable_fini()
{
	XME_HAL_TABLE_FINI(xme_core_routingTable_config.translationRoutes);
	XME_HAL_TABLE_FINI(xme_core_routingTable_config.outboundRoutes);
	XME_HAL_TABLE_FINI(xme_core_routingTable_config.localDestinationRoutes);
	XME_HAL_TABLE_FINI(xme_core_routingTable_config.localSourceRoutes);
	XME_HAL_TABLE_FINI(xme_core_routingTable_config.inboundRoutes);
}

xme_core_status_t
xme_core_routingTable_addLocalSourceRoute
(
	xme_core_dataChannel_t dataChannel,
	xme_core_component_t fromComponent,
	xme_core_component_port_t fromPort
)
{
	xme_core_component_portType_t portType;
	xme_hal_table_rowHandle_t handle;
	xme_core_routingTable_localSourceRoute_t* item;

	XME_CHECK
	(
		XME_CORE_COMPONENT_PORTTYPE_INVALID !=
		(
			portType =
				xme_core_resourceManager_getPortType
				(
					fromComponent,
					fromPort,
					true
				)
		),
		XME_CORE_STATUS_INVALID_PARAMETER
	);

	// TODO: Adapt port types. See ticket #792
	XME_ASSERT
	(
		(XME_CORE_COMPONENT_PORTTYPE_DCC_PUBLICATION == portType) ||
		(XME_CORE_COMPONENT_PORTTYPE_RR_REQUEST_SENDER == portType) ||
		(XME_CORE_COMPONENT_PORTTYPE_RR_RESPONSE_SENDER == portType)
	);

	// Check whether this route is already present in the table.
	// TODO: See ticket #889.
	handle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
	XME_HAL_TABLE_GET_NEXT
	(
		xme_core_routingTable_config.localSourceRoutes,
		xme_hal_table_rowHandle_t, handle,
		xme_core_routingTable_localSourceRoute_t, item,
		item->dataChannel == dataChannel && /* TODO: dataChannelProperties. See ticket #899 */ item->fromComponent == fromComponent && item->fromPort == fromPort
	);

	if (NULL != item)
	{
		// This route is already present
		XME_ASSERT(item->lockCount < (uint16_t)-1);
		item->lockCount++;
	}
	else
	{
		XME_CHECK
		(
			XME_HAL_TABLE_INVALID_ROW_HANDLE !=
			(
				handle = XME_HAL_TABLE_ADD_ITEM
				(
					xme_core_routingTable_config.localSourceRoutes
				)
			),
			XME_CORE_STATUS_OUT_OF_RESOURCES
		);

		item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_routingTable_config.localSourceRoutes, handle);
		XME_ASSERT(NULL != item);

		item->dataChannel = dataChannel;
		//item->dataChannelProperties = ...; TODO: See ticket #771
		item->fromComponent = fromComponent;
		item->fromPort = fromPort;
		item->lockCount = 0;
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_routingTable_addLocalDestinationRoute
(
	xme_core_dataChannel_t dataChannel,
	xme_core_component_t toComponent,
	xme_core_component_port_t toPort
)
{
	xme_core_component_portType_t portType;
	xme_hal_table_rowHandle_t handle;
	xme_core_routingTable_localDestinationRoute_t* item;

	XME_CHECK
	(
		XME_CORE_COMPONENT_PORTTYPE_INVALID !=
		(
			portType =
				xme_core_resourceManager_getPortType
				(
					toComponent,
					toPort,
					false
				)
		),
		XME_CORE_STATUS_INVALID_PARAMETER
	);

	XME_ASSERT
	(
		(XME_CORE_COMPONENT_PORTTYPE_DCC_SUBSCRIPTION == portType) ||
		(XME_CORE_COMPONENT_PORTTYPE_RR_REQUEST_HANDLER == portType) ||
		(XME_CORE_COMPONENT_PORTTYPE_RR_RESPONSE_HANDLER == portType)
	);

	// Check whether this route is already present in the table.
	// TODO: See ticket #889.
	handle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
	XME_HAL_TABLE_GET_NEXT
	(
		xme_core_routingTable_config.localDestinationRoutes,
		xme_hal_table_rowHandle_t, handle,
		xme_core_routingTable_localSourceRoute_t, item,
		item->dataChannel == dataChannel && item->toComponent == toComponent && item->toPort == toPort
	);

	if (NULL != item)
	{
		// This route is already present
		XME_ASSERT(item->lockCount < (uint16_t)-1);
		item->lockCount++;
	}
	else
	{
		XME_CHECK
		(
			XME_HAL_TABLE_INVALID_ROW_HANDLE !=
			(
				handle = XME_HAL_TABLE_ADD_ITEM
				(
					xme_core_routingTable_config.localDestinationRoutes
				)
			),
			XME_CORE_STATUS_OUT_OF_RESOURCES
		);

		item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_routingTable_config.localDestinationRoutes, handle);
		XME_ASSERT(NULL != item);

		item->dataChannel = dataChannel;
		item->toComponent = toComponent;
		item->toPort = toPort;
		item->lockCount = 0;
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_routingTable_addOutboundRoute
(
	xme_core_dataChannel_t dataChannel,
	xme_core_interface_interfaceId_t toInterface
)
{
	xme_hal_table_rowHandle_t handle;
	xme_core_routingTable_outboundRoute_t* item;

	// Check whether this route is already present in the table.
	// TODO: See ticket #889.
	handle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
	XME_HAL_TABLE_GET_NEXT
	(
		xme_core_routingTable_config.outboundRoutes,
		xme_hal_table_rowHandle_t, handle,
		xme_core_routingTable_outboundRoute_t, item,
		item->dataChannel == dataChannel && item->viaInterface == toInterface
	);

	if (NULL != item)
	{
		// This route is already present
		XME_ASSERT(item->lockCount < (uint16_t)-1);
		item->lockCount++;
	}
	else
	{
		XME_CHECK
		(
			XME_HAL_TABLE_INVALID_ROW_HANDLE !=
			(
				handle = XME_HAL_TABLE_ADD_ITEM
				(
					xme_core_routingTable_config.outboundRoutes
				)
			),
			XME_CORE_STATUS_OUT_OF_RESOURCES
		);

		item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_routingTable_config.outboundRoutes, handle);
		XME_ASSERT(NULL != item);

		item->dataChannel = dataChannel;
		item->viaInterface = toInterface;
		item->lockCount = 0;

		XME_CHECK
		(
			xme_core_interfaceManager_provide_channel(toInterface, dataChannel) == XME_CORE_STATUS_SUCCESS,
			XME_CORE_STATUS_OUT_OF_RESOURCES
		);
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_routingTable_addInboundRoute
(
	xme_core_dataChannel_t dataChannel,
	xme_core_interface_interfaceId_t inInterface
)
{
	xme_hal_table_rowHandle_t handle;
	xme_core_routingTable_inboundRoute_t* item;

	// Check whether this route is already present in the table.
	// TODO: See ticket #889.
	handle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
	XME_HAL_TABLE_GET_NEXT
	(
		xme_core_routingTable_config.inboundRoutes,
		xme_hal_table_rowHandle_t, handle,
		xme_core_routingTable_inboundRoute_t, item,
		item->dataChannel == dataChannel && item->inInterface == inInterface
	);

	if (NULL != item)
	{
		// This route is already present
		XME_ASSERT(item->lockCount < (uint16_t)-1);
		item->lockCount++;
	}
	else
	{
		XME_CHECK
		(
			XME_HAL_TABLE_INVALID_ROW_HANDLE !=
			(
				handle = XME_HAL_TABLE_ADD_ITEM
				(
					xme_core_routingTable_config.inboundRoutes
				)
			),
			XME_CORE_STATUS_OUT_OF_RESOURCES
		);

		item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_routingTable_config.inboundRoutes, handle);
		XME_ASSERT(NULL != item);

		item->dataChannel = dataChannel;
		item->inInterface = inInterface;
		item->lockCount = 0;

		XME_CHECK
		(
			xme_core_interfaceManager_join_channel(inInterface, dataChannel) == XME_CORE_STATUS_SUCCESS,
			XME_CORE_STATUS_OUT_OF_RESOURCES
		);
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_routingTable_addTranslationRoute
(
	xme_core_dataChannel_t sourceDataChannel,
	xme_core_dataChannel_t destinationDataChannel
)
{
	xme_hal_table_rowHandle_t handle;
	xme_core_routingTable_translationRoute_t* item;

	// Check whether this route is already present in the table.
	// TODO: See ticket #889.
	handle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
	XME_HAL_TABLE_GET_NEXT
	(
		xme_core_routingTable_config.translationRoutes,
		xme_hal_table_rowHandle_t, handle,
		xme_core_routingTable_inboundRoute_t, item,
		item->sourceDataChannel == sourceDataChannel && item->destinationDataChannel == destinationDataChannel
	);

	if (NULL != item)
	{
		// This route is already present
		XME_ASSERT(item->lockCount < (uint16_t)-1);
		item->lockCount++;
	}
	else
	{
		XME_CHECK
		(
			XME_HAL_TABLE_INVALID_ROW_HANDLE !=
			(
				handle = XME_HAL_TABLE_ADD_ITEM
				(
					xme_core_routingTable_config.translationRoutes
				)
			),
			XME_CORE_STATUS_OUT_OF_RESOURCES
		);

		item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_routingTable_config.translationRoutes, handle);
		XME_ASSERT(NULL != item);

		item->sourceDataChannel = sourceDataChannel;
		item->destinationDataChannel = destinationDataChannel;
		item->lockCount = 1;
	}

	return XME_CORE_STATUS_SUCCESS;
}

// TODO (See ticket #793): If the expressiveness of the following functions is enough, they
//                         should be combined into a single removal function that gets called
//                         from specialized inlined versions corresponding to the functions
//                         below.

xme_core_status_t
xme_core_routingTable_removeLocalSourceRoute
(
	xme_core_dataChannel_t dataChannel
)
{
	// TODO (See ticket #794): This assumes that a local source route only originates from a single
	//                         local component port. If this does not hold, more parameters have to
	//                         be added to this function and the search procedure adapted.
	XME_HAL_TABLE_ITERATE_BEGIN
	(
		xme_core_routingTable_config.localSourceRoutes,
		xme_hal_table_rowHandle_t, handle,
		xme_core_routingTable_localSourceRoute_t, item
	);
	{
		if (item->dataChannel == dataChannel)
		{
			// Check whether this route has been added multiple times.
			// TODO: See ticket #889.
			if (item->lockCount > 0)
			{
				item->lockCount--;
			}
			else
			{
				XME_HAL_TABLE_REMOVE_ITEM(xme_core_routingTable_config.localSourceRoutes, handle);
			}
			return XME_CORE_STATUS_SUCCESS;
		}
	}
	XME_HAL_TABLE_ITERATE_END();

	return XME_CORE_STATUS_NO_SUCH_VALUE;
}

xme_core_status_t
xme_core_routingTable_removeLocalDestinationRoute
(
	xme_core_dataChannel_t dataChannel,
	xme_core_component_t toComponent,
	xme_core_component_port_t toSubscription
)
{
	XME_HAL_TABLE_ITERATE_BEGIN
	(
		xme_core_routingTable_config.localDestinationRoutes,
		xme_hal_table_rowHandle_t, handle,
		xme_core_routingTable_localDestinationRoute_t, item
	);
	{
		if ((item->dataChannel == dataChannel) && (item->toComponent == toComponent) && (item->toPort == toSubscription))
		{
			// Check whether this route has been added multiple times.
			// TODO: See ticket #889.
			if (item->lockCount > 0)
			{
				item->lockCount--;
			}
			else
			{
				XME_HAL_TABLE_REMOVE_ITEM(xme_core_routingTable_config.localDestinationRoutes, handle);
			}
			return XME_CORE_STATUS_SUCCESS;
		}
	}
	XME_HAL_TABLE_ITERATE_END();

	return XME_CORE_STATUS_NO_SUCH_VALUE;
}

xme_core_status_t
xme_core_routingTable_removeOutboundRoute
(
	xme_core_dataChannel_t dataChannel,
	xme_core_interface_interfaceId_t toInterface
)
{
	XME_HAL_TABLE_ITERATE
	(
		xme_core_routingTable_config.outboundRoutes,
		xme_hal_table_rowHandle_t, handle,
		xme_core_routingTable_outboundRoute_t, item,
		{
			if ((item->dataChannel == dataChannel) && (item->viaInterface == toInterface))
			{
				// Check whether this route has been added multiple times.
				// TODO: See ticket #889.
				if (item->lockCount > 0)
				{
					item->lockCount--;
				}
				else
				{
					XME_CHECK
					(
						xme_core_interfaceManager_unprovide_channel(toInterface, dataChannel) == XME_CORE_STATUS_SUCCESS,
						XME_CORE_STATUS_INTERNAL_ERROR
					);
					XME_HAL_TABLE_REMOVE_ITEM(xme_core_routingTable_config.outboundRoutes, handle);					
				}
				return XME_CORE_STATUS_SUCCESS;
			}
		}
	);

	return XME_CORE_STATUS_NO_SUCH_VALUE;
}

xme_core_status_t
xme_core_routingTable_removeInboundRoute
(
	xme_core_dataChannel_t dataChannel,
	xme_core_interface_interfaceId_t inInterface
)
{
	XME_HAL_TABLE_ITERATE
	(
		xme_core_routingTable_config.inboundRoutes,
		xme_hal_table_rowHandle_t, handle,
		xme_core_routingTable_inboundRoute_t, item,
		{
			if ((item->dataChannel == dataChannel) && (item->inInterface == inInterface))
			{
				// Check whether this route has been added multiple times.
				// TODO: See ticket #889.
				if (item->lockCount > 0)
				{
					item->lockCount--;
				}
				else
				{
					XME_CHECK
					(
						xme_core_interfaceManager_leave_channel(inInterface, dataChannel) == XME_CORE_STATUS_SUCCESS,
						XME_CORE_STATUS_INTERNAL_ERROR
					);
					XME_HAL_TABLE_REMOVE_ITEM(xme_core_routingTable_config.inboundRoutes, handle);
				}
				return XME_CORE_STATUS_SUCCESS;
			}
		}
	);

	return XME_CORE_STATUS_NO_SUCH_VALUE;
}

xme_core_status_t
xme_core_routingTable_removeTranslationRoute
(
	xme_core_dataChannel_t sourceDataChannel,
	xme_core_dataChannel_t destinationDataChannel
)
{
	XME_HAL_TABLE_ITERATE
	(
		xme_core_routingTable_config.translationRoutes,
		xme_hal_table_rowHandle_t, handle,
		xme_core_routingTable_translationRoute_t, item,
		{
			if (item->sourceDataChannel == sourceDataChannel &&
			    item->destinationDataChannel == destinationDataChannel)
			{
				// Check whether this route has been added multiple times.
				// TODO: See ticket #889.
				if (item->lockCount > 0)
				{
					item->lockCount--;
				}
				else
				{
					XME_HAL_TABLE_REMOVE_ITEM(xme_core_routingTable_config.translationRoutes, handle);
				}
				return XME_CORE_STATUS_SUCCESS;
			}
		}
	);

	return XME_CORE_STATUS_NO_SUCH_VALUE;
}

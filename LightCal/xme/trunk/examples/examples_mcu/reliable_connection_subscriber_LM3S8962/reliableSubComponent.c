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
 *         This project shows an example on how to connect
 *         to a publisher in a reliable way.
 *
 *         This means that a connection is setup via
 *         TCP to the publsiher. The project uses
 *         freeRTOS and lwIP.
 *
 * \author
 *         Hauke Stähle <staehle@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/component.h"
#include "xme/core/defines.h"
#include "xme/core/nodeManager.h"
#include "xme/core/interfaceManager.h"
#include "xme/core/broker.h"
#include "xme/core/topic.h"
#include "reliableSubComponent.h"

#include "xme/hal/console.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/

/******************************************************************************/
/***   Component configurations                                             ***/
/******************************************************************************/

/*
 *
 * Please change according to your needs!
 * Please change according to your needs!
 * Please change according to your needs!
 *
 */
#define PUBLISHER_NAME "192.168.17.2"
#define PUBLISHER_PORT 33211

#define TOPIC_ID (xme_core_topic_t)233
#define CHANNEL_ID 1444

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_dcc_subscriptionHandle_t subHandle;

void
dummy_subscription
(
		xme_hal_shm_bufferHandle_t dataHandle,
		void* userData
)
{
	char *txt = (char *)xme_hal_shm_handleToPointer( dataHandle );
	static uint16_t count = 0;
	uint16_t size = xme_hal_shm_buffer_getHandleSize( dataHandle );

	count++;
	xme_hal_console_uint16_d( count );
	xme_hal_console_string(">");
	xme_hal_console_stringn( (char *)xme_hal_shm_handleToPointer( dataHandle ), size );
	xme_hal_console_string("\n");
}

xme_core_status_t
reliableSubComponent_create(xme_core_nodeManager_configStruct_t* config)
{
	// Create subscription to chat topic
	subHandle = xme_core_dcc_subscribeTopic( 
		TOPIC_ID,
		XME_CORE_MD_EMPTY_META_DATA, 
		dummy_subscription, 
		NULL 
	);

	if ( subHandle == XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Local channel to component
	if ( xme_core_routingTable_addLocalDestinationRoute(
		CHANNEL_ID,
		xme_core_dcc_getComponentFromSubscriptionHandle(subHandle),
		xme_core_dcc_getPortFromSubscriptionHandle(subHandle)
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Add channel to address mapping for tcp interface
	{
		xme_core_interface_address_t publisher;
		PORT_TO_GENERIC_ADDRESS_TCP( publisher, PUBLISHER_PORT );
		HOST_TO_GENERIC_ADDRESS_TCP( publisher, PUBLISHER_NAME );
		if ( xme_core_interfaceManager_add_channel_to_phy_address_mapping(
			(xme_core_interface_interfaceId_t) 2,
			CHANNEL_ID,
			&publisher ) != XME_CORE_STATUS_SUCCESS )
		{
			return XME_CORE_STATUS_INTERNAL_ERROR;
		}
	}

	// Remote channel to chat component
	{
		if ( xme_core_routingTable_addInboundRoute(
			CHANNEL_ID,
			(xme_core_interface_interfaceId_t) 2
		) != XME_CORE_STATUS_SUCCESS )
		{
			/*
			 * If the publisher is not available, this command
			 * will fail.
			 *
			 * If it does fail, the system automatically tries
			 * to connect to the publisher until the connection
			 * is established.
			 *
			 * If the connection breaks, the system automatically
			 * tries to reconnect.
			 */
			xme_hal_console_string("Connection to publisher failed.\n");
			xme_hal_console_string("Retry in progress...\n");
		};
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
reliableSubComponent_activate(xme_core_nodeManager_configStruct_t* config)
{
	// Nothing to do here
	return XME_CORE_STATUS_SUCCESS;
}

void
reliableSubComponent_deactivate(xme_core_nodeManager_configStruct_t* config)
{
	// Nothing to do here
}

void
reliableSubComponent_destroy(xme_core_nodeManager_configStruct_t* config)
{
	if ( xme_core_routingTable_removeLocalDestinationRoute(
		CHANNEL_ID,
		xme_core_dcc_getComponentFromSubscriptionHandle(subHandle),
		xme_core_dcc_getPortFromSubscriptionHandle(subHandle) ) != XME_CORE_STATUS_SUCCESS )
	{
		xme_hal_console_string("Failure xme_core_broker_removeLocalDestinationRoute");
	}

	{
		if ( xme_core_routingTable_removeInboundRoute(
			CHANNEL_ID,
			(xme_core_interface_interfaceId_t) 2
		) != XME_CORE_STATUS_SUCCESS )
		{
			xme_hal_console_string("Failure xme_core_broker_removeInboundRoute");
		};
	}

	if ( xme_core_dcc_unsubscribeTopic(subHandle) != XME_CORE_STATUS_SUCCESS )
	{
		xme_hal_console_string("Failure xme_core_dcc_unsubscribeTopic");
	}
}

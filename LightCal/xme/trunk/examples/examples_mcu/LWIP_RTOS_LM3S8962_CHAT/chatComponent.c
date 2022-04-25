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
 *         Port of the chat example project to run on a luminary microcontroller
 *         with the FreeRTOS and lwIP libraries.
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
#include "chatComponent.h"

#include "xme/hal/console.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/

/******************************************************************************/
/***   Component configurations                                             ***/
/******************************************************************************/

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_dcc_subscriptionHandle_t subHandle;
xme_core_dcc_publicationHandle_t pubHandle;

#define CHAT_TOPIC_ID (xme_core_topic_t)300
#define CHAT_CHANNEL 123

void
dummy_subscription
(
		xme_hal_sharedPtr_t dataHandle,
		void* userData
)
{
	static uint16_t count = 0;
	count++;

	xme_hal_console_uint16_d( count );
	xme_hal_console_string(">");
	xme_hal_console_string( (char *)xme_hal_sharedPtr_getPointer( dataHandle ) );
	xme_hal_console_string("\n");
}

xme_core_status_t
chatComponent_create(xme_core_nodeManager_configStruct_t* config)
{
	// Create subscription to chat topic
	subHandle = xme_core_dcc_subscribeTopic( 
		CHAT_TOPIC_ID, 
		XME_CORE_MD_EMPTY_META_DATA,
		false,
		dummy_subscription, 
		NULL 
	);

	if ( subHandle == XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Create publisher for chat topic
	pubHandle = xme_core_dcc_publishTopic( 
		CHAT_TOPIC_ID,
		XME_CORE_MD_EMPTY_META_DATA,
		false,
		NULL
	);

	if ( pubHandle == XME_CORE_DCC_INVALID_PUBLICATION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Local channel from chat component
	if ( xme_core_routingTable_addLocalSourceRoute( 
		CHAT_CHANNEL, 
		xme_core_dcc_getComponentFromPublicationHandle(pubHandle),
		xme_core_dcc_getPortFromPublicationHandle(pubHandle) 
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Local channel to chat component
	if ( xme_core_routingTable_addLocalDestinationRoute(
		CHAT_CHANNEL,
		xme_core_dcc_getComponentFromSubscriptionHandle(subHandle),
		xme_core_dcc_getPortFromSubscriptionHandle(subHandle)
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Remote channel from chat component
	if ( xme_core_routingTable_addOutboundRoute(
		CHAT_CHANNEL,
		(xme_core_interface_interfaceId_t) 1
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Remote channel to chat component
	if ( xme_core_routingTable_addInboundRoute(
		CHAT_CHANNEL,
		(xme_core_interface_interfaceId_t) 1
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	return XME_CORE_STATUS_SUCCESS;
}

void
chatComponent_callback (void* userData)
{
	char txt[] = "Auto chat message.";

	// As the microcontroller board has a lack of a keyboard input device,
	// an automatic text message is sent every some seconds.
	while( 1 )
	{
		xme_core_dcc_sendTopicData(pubHandle, (void *)txt, sizeof(txt) );
		xme_hal_console_string("Sent: ");
		xme_hal_console_string( txt );
		xme_hal_console_string("\n");
		vTaskDelay(2000);
	}

}

xme_core_status_t
chatComponent_activate(xme_core_nodeManager_configStruct_t* config)
{
	xme_hal_sched_addTask(0, 0, XME_HAL_SCHED_PRIORITY_NORMAL, chatComponent_callback, NULL);
	return XME_CORE_STATUS_SUCCESS;
}

void
chatComponent_deactivate(xme_core_nodeManager_configStruct_t* config)
{
	// Nothing to do here
}

void
chatComponent_destroy(xme_core_nodeManager_configStruct_t* config)
{
	if ( xme_core_routingTable_removeLocalSourceRoute(CHAT_CHANNEL) != XME_CORE_STATUS_SUCCESS )
	{
		xme_hal_console_string("Failure xme_core_broker_removeLocalSourceRoute");
	}

	if ( xme_core_routingTable_removeLocalDestinationRoute(
		CHAT_CHANNEL,
		xme_core_dcc_getComponentFromPublicationHandle(pubHandle), 
		xme_core_dcc_getPortFromPublicationHandle(pubHandle) ) != XME_CORE_STATUS_SUCCESS )
	{
		xme_hal_console_string("Failure xme_core_broker_removeLocalDestinationRoute");
	}

	if ( 
		xme_core_routingTable_removeOutboundRoute(
		CHAT_CHANNEL, 
		(xme_core_interface_interfaceId_t) 1
		) != XME_CORE_STATUS_SUCCESS )
	{
		xme_hal_console_string("Failure xme_core_broker_removeOutboundRoute");
	}

	// Remote channel to chat component
	if ( xme_core_routingTable_removeInboundRoute(
		CHAT_CHANNEL,
		(xme_core_interface_interfaceId_t) 1
	) != XME_CORE_STATUS_SUCCESS )
	{
		xme_hal_console_string("Failure xme_core_broker_removeInboundRoute");
	}

	if ( xme_core_dcc_unsubscribeTopic(subHandle) != XME_CORE_STATUS_SUCCESS )
	{
		xme_hal_console_string("Failure xme_core_dcc_unsubscribeTopic");
	}

	if ( xme_core_dcc_unpublishTopic(pubHandle) != XME_CORE_STATUS_SUCCESS )
	{
		xme_hal_console_string("Failure xme_core_dcc_unpublishTopic");
	}
}

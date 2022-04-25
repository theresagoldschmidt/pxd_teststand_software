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
 *         Reliable publisher component.
 *
 * \author
 *         Hauke Stähle <staehle@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/component.h"
#include "xme/core/core.h"
#include "xme/core/defines.h"
#include "xme/core/nodeManager.h"
#include "xme/core/interfaceManager.h"
#include "xme/core/broker.h"
#include "xme/core/topic.h"
#include "reliablePubComponent.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/

/******************************************************************************/
/***   Component configurations                                             ***/
/******************************************************************************/

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_dcc_publicationHandle_t pubHandle;

/*
 *
 * Please change according to your needs!
 * Please change according to your needs!
 * Please change according to your needs!
 *
 */
#define LOCAL_PORT 33211

#define TOPIC_ID (xme_core_topic_t)233
#define CHANNEL_ID 1444

xme_core_status_t
reliablePubComponent_create(xme_core_nodeManager_configStruct_t* config)
{
	// Create publisher for topic
	pubHandle = xme_core_dcc_publishTopic( 
		TOPIC_ID, 
		XME_CORE_MD_EMPTY_META_DATA, 
		NULL
	);

	if ( pubHandle == XME_CORE_DCC_INVALID_PUBLICATION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Local channel from component
	if ( xme_core_routingTable_addLocalSourceRoute( 
		CHANNEL_ID, 
		xme_core_dcc_getComponentFromPublicationHandle(pubHandle),
		xme_core_dcc_getPortFromPublicationHandle(pubHandle) 
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}
	
	// Add channel to address mapping for tcp interface
	{
		xme_core_interface_address_t address;
		PORT_TO_GENERIC_ADDRESS_TCP( address, LOCAL_PORT );
		HOST_TO_GENERIC_ADDRESS_TCP( address, NULL );
		if ( xme_core_interfaceManager_add_channel_to_phy_address_mapping(
			(xme_core_interface_interfaceId_t) 2,
			CHANNEL_ID,
			&address ) != XME_CORE_STATUS_SUCCESS )
		{
			return XME_CORE_STATUS_INTERNAL_ERROR;
		}
	}

	// Remote channel from component
	if ( xme_core_routingTable_addOutboundRoute(
		CHANNEL_ID,
		(xme_core_interface_interfaceId_t) 2
	) != XME_CORE_STATUS_SUCCESS )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	return XME_CORE_STATUS_SUCCESS;
}

void
reliablePubComponent_callback (void* userData)
{
	char temp[1024];

	printf("Please enter a message. It will be sent to all connected nodes via TCP.\n");

	while( fgets(temp,1024,stdin) != NULL )
	{
		if ( strlen(temp) )
		{
			if ( temp[strlen(temp)-1] == '\n' ) temp[strlen(temp)-1] = '\0';
			xme_core_dcc_sendTopicData(pubHandle, temp, strlen(temp)+1);
		}
	}
}

xme_core_status_t
reliablePubComponent_activate(xme_core_nodeManager_configStruct_t* config)
{
	xme_hal_sched_addTask(0, 0, 0, reliablePubComponent_callback, NULL);
	return XME_CORE_STATUS_SUCCESS;
}

void
reliablePubComponent_deactivate(xme_core_nodeManager_configStruct_t* config)
{
	// Nothing to do here
}

void
reliablePubComponent_destroy(xme_core_nodeManager_configStruct_t* config)
{
	if ( xme_core_routingTable_removeLocalSourceRoute(CHANNEL_ID) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeLocalSourceRoute");
	}

	if ( xme_core_routingTable_removeLocalDestinationRoute(
		CHANNEL_ID,
		xme_core_dcc_getComponentFromPublicationHandle(pubHandle), 
		xme_core_dcc_getPortFromPublicationHandle(pubHandle) ) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeLocalDestinationRoute");
	}

	if ( 
		xme_core_routingTable_removeOutboundRoute(
		CHANNEL_ID, 
		(xme_core_interface_interfaceId_t) 2 ) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeOutboundRoute");
	}

	if ( xme_core_dcc_unpublishTopic(pubHandle) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_dcc_unpublishTopic");
	}
}
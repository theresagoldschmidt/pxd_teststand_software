/*
 * Copyright (c) 2011, fortiss GmbH.
 * All rights reserved.
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
 *         Chat example project.
 *
 * \author
 *         Hauke Stähle <staehle@fortiss.org>
 *         Nadine Keddis <keddis@fortiss.org>
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
#include "chatComponent.h"

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
xme_core_dcc_subscriptionHandle_t subHandleError;
xme_core_dcc_publicationHandle_t pubHandleStart;
xme_core_dcc_publicationHandle_t pubHandleStop;
xme_core_dcc_publicationHandle_t pubHandleCalibrate;
xme_core_dcc_publicationHandle_t pubHandleBootload;

#define START_TOPIC_ID (xme_core_topic_t)300
#define START_CHANNEL (xme_core_dataChannel_t)128

#define STOP_TOPIC_ID (xme_core_topic_t) 301
#define STOP_CHANNEL (xme_core_dataChannel_t)124

#define CALIBRATE_TOPIC_ID (xme_core_topic_t)302
#define CALIBRATE_CHANNEL (xme_core_dataChannel_t)125

#define BOOTLOAD_TOPIC_ID (xme_core_topic_t)303
#define BOOTLOAD_CHANNEL (xme_core_dataChannel_t)126

#define LOGDATA_TOPIC_ID (xme_core_topic_t)304
#define LOGDATA_CHANNEL (xme_core_dataChannel_t)127

#define ERROR_MESSAGE_TOPIC_ID (xme_core_topic_t)778
#define ERROR_MESSAGE_CHANNEL (xme_core_dataChannel_t)743

void
logdata_subscription
(
		xme_hal_shm_bufferHandle_t dataHandle,
		void* userData
)
{
	printf("Received on topic logdata: %s\n", xme_hal_shm_handleToPointer( dataHandle ) );
}

void
errorMessage_subscription
(
	xme_hal_shm_bufferHandle_t dataHandle,
	void* userData
)
{
	printf("Received on topic error message: %s\n", xme_hal_shm_handleToPointer( dataHandle ) );
}

xme_core_status_t
chatComponent_create(xme_core_nodeManager_configStruct_t* config)
{
	// Create subscription to logdata topic
	subHandle = xme_core_dcc_subscribeTopic( 
		LOGDATA_TOPIC_ID, 
		XME_CORE_MD_EMPTY_META_DATA, 
		logdata_subscription, 
		NULL 
	);

	if ( subHandle == XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Create subscription to error message topic
	subHandleError = xme_core_dcc_subscribeTopic( 
		ERROR_MESSAGE_TOPIC_ID, 
		XME_CORE_MD_EMPTY_META_DATA, 
		errorMessage_subscription, 
		NULL 
	);

	if ( subHandleError == XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Create publisher for start topic
	pubHandleStart = xme_core_dcc_publishTopic( 
		START_TOPIC_ID, 
		XME_CORE_MD_EMPTY_META_DATA, 
		NULL
	);

	if ( pubHandleStart == XME_CORE_DCC_INVALID_PUBLICATION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Create publisher for stop topic
	pubHandleStop = xme_core_dcc_publishTopic( 
		STOP_TOPIC_ID, 
		XME_CORE_MD_EMPTY_META_DATA, 
		NULL
	);

	if ( pubHandleStop == XME_CORE_DCC_INVALID_PUBLICATION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Create publisher for calibrate topic
	pubHandleCalibrate = xme_core_dcc_publishTopic( 
		CALIBRATE_TOPIC_ID, 
		XME_CORE_MD_EMPTY_META_DATA, 
		NULL
	);

	if ( pubHandleCalibrate == XME_CORE_DCC_INVALID_PUBLICATION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Create publisher for bootload topic
	pubHandleBootload = xme_core_dcc_publishTopic( 
		BOOTLOAD_TOPIC_ID, 
		XME_CORE_MD_EMPTY_META_DATA, 
		NULL
	);

	if ( pubHandleBootload == XME_CORE_DCC_INVALID_PUBLICATION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Local channel from chat component for topic start
	if ( xme_core_routingTable_addLocalSourceRoute( 
		START_CHANNEL, 
		xme_core_dcc_getComponentFromPublicationHandle(pubHandleStart),
		xme_core_dcc_getPortFromPublicationHandle(pubHandleStart) 
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Local channel from gui component for topic stop
	if ( xme_core_routingTable_addLocalSourceRoute( 
		STOP_CHANNEL, 
		xme_core_dcc_getComponentFromPublicationHandle(pubHandleStop),
		xme_core_dcc_getPortFromPublicationHandle(pubHandleStop) 
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Local channel from gui component for topic calibrate
	if ( xme_core_routingTable_addLocalSourceRoute( 
		CALIBRATE_CHANNEL, 
		xme_core_dcc_getComponentFromPublicationHandle(pubHandleCalibrate),
		xme_core_dcc_getPortFromPublicationHandle(pubHandleCalibrate) 
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Local channel from gui component for topic bootload
	if ( xme_core_routingTable_addLocalSourceRoute( 
		BOOTLOAD_CHANNEL, 
		xme_core_dcc_getComponentFromPublicationHandle(pubHandleBootload),
		xme_core_dcc_getPortFromPublicationHandle(pubHandleBootload) 
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Local channel to chat component for topic logdata
	if ( xme_core_routingTable_addLocalDestinationRoute(
		LOGDATA_CHANNEL,
		xme_core_dcc_getComponentFromSubscriptionHandle(subHandle),
		xme_core_dcc_getPortFromSubscriptionHandle(subHandle)
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Local channel to chat component for topic error message
	if ( xme_core_routingTable_addLocalDestinationRoute(
		ERROR_MESSAGE_CHANNEL,
		xme_core_dcc_getComponentFromSubscriptionHandle(subHandleError),
		xme_core_dcc_getPortFromSubscriptionHandle(subHandleError)
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Remote channel from chat component for topic start
	if ( xme_core_routingTable_addOutboundRoute(
		START_CHANNEL,
		(xme_core_interface_interfaceId_t) 1,
		NULL
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Remote channel from gui component for topic stop
	if ( xme_core_routingTable_addOutboundRoute(
		STOP_CHANNEL,
		(xme_core_interface_interfaceId_t) 1,
		NULL
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Remote channel from gui component for topic calibrate
	if ( xme_core_routingTable_addOutboundRoute(
		CALIBRATE_CHANNEL,
		(xme_core_interface_interfaceId_t) 1,
		NULL
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Remote channel from gui component for topic bootload
	if ( xme_core_routingTable_addOutboundRoute(
		BOOTLOAD_CHANNEL,
		(xme_core_interface_interfaceId_t) 1,
		NULL
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Enable reception of packets for logdata channel
	if ( xme_core_interfaceManager_join_channel(
		(xme_core_interface_interfaceId_t) 1,
		LOGDATA_CHANNEL
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Enable reception of packets for error message channel
	if ( xme_core_interfaceManager_join_channel(
		(xme_core_interface_interfaceId_t) 1,
		ERROR_MESSAGE_CHANNEL
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	return XME_CORE_STATUS_SUCCESS;
}

void
chatComponent_callback (void* userData)
{
	char temp[1024];
	int count = 0;

	printf("Chat client ready. Please enter some text. It will we displayed on other running clients. Multiple instances of this program may run at once, without need for reconfiguration.\n");

	printf("Messages will be printed twice, as they are locally as well as remotely routed.\n");

	while( fgets(temp,1024,stdin) != NULL )
	{
		/*if ( strlen(temp) )
		{
			if ( temp[strlen(temp)-1] == '\n' ) temp[strlen(temp)-1] = '\0';
			xme_core_dcc_sendTopicData(pubHandleStart, temp, strlen(temp)+1);
		}*/
		if ( strlen(temp) && count == 0)
		{
			if ( temp[strlen(temp)-1] == '\n' ) temp[strlen(temp)-1] = '\0';
			xme_core_dcc_sendTopicData(pubHandleStart, temp, strlen(temp)+1);
			count++;
		}
		else if ( strlen(temp) && count == 1)
		{
			if ( temp[strlen(temp)-1] == '\n' ) temp[strlen(temp)-1] = '\0';
			xme_core_dcc_sendTopicData(pubHandleStop, temp, strlen(temp)+1);
			count++;
		}
		else if ( strlen(temp) && count == 2)
		{
			if ( temp[strlen(temp)-1] == '\n' ) temp[strlen(temp)-1] = '\0';
			xme_core_dcc_sendTopicData(pubHandleCalibrate, temp, strlen(temp)+1);
			count++;
		}
		else if ( strlen(temp) && count == 3)
		{
			if ( temp[strlen(temp)-1] == '\n' ) temp[strlen(temp)-1] = '\0';
			xme_core_dcc_sendTopicData(pubHandleBootload, temp, strlen(temp)+1);
			count = 0;
		}
	}
}

xme_core_status_t
chatComponent_activate(xme_core_nodeManager_configStruct_t* config)
{
	xme_hal_sched_addTask(0, 0, 0, chatComponent_callback, NULL);
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
	if ( xme_core_routingTable_removeLocalSourceRoute(START_CHANNEL) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeLocalSourceRoute");
	}
	if ( xme_core_routingTable_removeLocalSourceRoute(STOP_CHANNEL) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeLocalSourceRoute");
	}
	if ( xme_core_routingTable_removeLocalSourceRoute(CALIBRATE_CHANNEL) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeLocalSourceRoute");
	}
	if ( xme_core_routingTable_removeLocalSourceRoute(BOOTLOAD_CHANNEL) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeLocalSourceRoute");
	}

	if ( xme_core_routingTable_removeLocalDestinationRoute(
		LOGDATA_CHANNEL,
		xme_core_dcc_getComponentFromSubscriptionHandle(subHandle), 
		xme_core_dcc_getPortFromSubscriptionHandle(subHandle) ) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeLocalDestinationRoute");
	}

	if ( 
		xme_core_routingTable_removeOutboundRoute(
		START_CHANNEL, 
		(xme_core_interface_interfaceId_t) 1,
		NULL ) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeOutboundRoute");
	}
	if ( 
		xme_core_routingTable_removeOutboundRoute(
		STOP_CHANNEL, 
		(xme_core_interface_interfaceId_t) 1,
		NULL ) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeOutboundRoute");
	}
	if ( 
		xme_core_routingTable_removeOutboundRoute(
		CALIBRATE_CHANNEL, 
		(xme_core_interface_interfaceId_t) 1,
		NULL ) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeOutboundRoute");
	}
	if ( 
		xme_core_routingTable_removeOutboundRoute(
		BOOTLOAD_CHANNEL, 
		(xme_core_interface_interfaceId_t) 1,
		NULL ) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeOutboundRoute");
	}

	if ( xme_core_interfaceManager_leave_channel((xme_core_interface_interfaceId_t) 1, LOGDATA_CHANNEL	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		printf("Failure xme_core_interfaceManager_leave_channel");
	}

	if ( xme_core_dcc_unsubscribeTopic(subHandle) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_dcc_unsubscribeTopic");
	}

	if ( xme_core_dcc_unpublishTopic(pubHandleStart) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_dcc_unpublishTopic");
	}
	if ( xme_core_dcc_unpublishTopic(pubHandleStop) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_dcc_unpublishTopic");
	}
	if ( xme_core_dcc_unpublishTopic(pubHandleCalibrate) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_dcc_unpublishTopic");
	}
	if ( xme_core_dcc_unpublishTopic(pubHandleBootload) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_dcc_unpublishTopic");
	}

}
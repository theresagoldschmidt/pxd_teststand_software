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
 *         Heartbeat test component.
 *
 * \author
 *         Dominik Sojer <sojer@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/

#include "testHeartbeat.h"
#include "xme/core/interfaceManager.h"

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
XME_HAL_TABLE
(
	xme_adv_healthmonitor_heartbeat_t,
	xme_adv_testHeartbeat_heartbeatTable,
	XME_ADV_TESTHEARTBEAT_HEARTBEATS_MAX
);

static int debugErrorCounter; // debug

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/

xme_hal_time_interval_t maxTimestampAge; // max age of timestamp
bool catchAllHeartbeats;

xme_core_status_t xme_adv_testHeartbeat_create(xme_adv_testHeartbeat_configStruct_t* config)
{
	XME_HAL_TABLE_INIT( xme_adv_testHeartbeat_heartbeatTable );
	
	catchAllHeartbeats = config->catchAllHeartBeats;
	if (!config->catchAllHeartBeats) {
		initHeartbeatTable();
	}
	
	debugErrorCounter = 0;
	maxTimestampAge = config->maxTimestampAge;

	// Create subscription to heartbeat topic (for testing of a single node)
	config->subHandle = xme_core_dcc_subscribeTopic( 
		HEARTBEAT_TOPIC_ID, 
		XME_CORE_MD_EMPTY_META_DATA, 
		false,
		receive_heartbeatMultiNode, 
		NULL 
	);

	if ( config->subHandle == XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Create publisher for error message topic
	config->pubHandle = xme_core_dcc_publishTopic( 
		ERROR_MESSAGE_TOPIC_ID, 
		XME_CORE_MD_EMPTY_META_DATA, 
		false,
		NULL
	);

	if ( config->pubHandle == XME_CORE_DCC_INVALID_PUBLICATION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	} 

	// Local channel to testHeartbeat component (heartbeats)
	if ( xme_core_routingTable_addLocalDestinationRoute(
		HEARTBEAT_CHANNEL,
		xme_core_dcc_getComponentFromSubscriptionHandle(config->subHandle),
		xme_core_dcc_getPortFromSubscriptionHandle(config->subHandle)
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Remote channel to testHeartbeat component for topic heartbeats
	// Data is sent over UDP (not reliable)
	if( xme_core_routingTable_addInboundRoute(
		HEARTBEAT_CHANNEL,
		(xme_core_interface_interfaceId_t) 1
	) != XME_CORE_STATUS_SUCCESS )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};


	// Local channel from heartbeat component
	if ( xme_core_routingTable_addLocalSourceRoute( 
		ERROR_MESSAGE_CHANNEL, 
		xme_core_dcc_getComponentFromPublicationHandle(config->pubHandle),
		xme_core_dcc_getPortFromPublicationHandle(config->pubHandle) 
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Remote channel from heartbeat component
	if ( xme_core_routingTable_addOutboundRoute(
		ERROR_MESSAGE_CHANNEL,
		(xme_core_interface_interfaceId_t) 1
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};
	
	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t xme_adv_testHeartbeat_activate(xme_adv_testHeartbeat_configStruct_t* config)
{
	 xme_adv_healthmonitor_errorMessage_t error;

	xme_hal_sched_addTask(maxTimestampAge, maxTimestampAge, 0, testHeartbeat_callback, NULL);

	// notification that test is running
	error.componentId = xme_core_resourceManager_getCurrentComponentId();
	error.nodeId = xme_core_nodeManager_getNodeId();
	error.status = XME_ADV_HEALTHMONITOR_TEST_OK;
	error.identifier = XME_ADV_TEST_HEARTBEAT;
	xme_core_dcc_sendTopicData(config->pubHandle, &error, sizeof(error));

	return XME_CORE_STATUS_SUCCESS;
}

void xme_adv_testHeartbeat_deactivate(xme_adv_testHeartbeat_configStruct_t* config)
{
}

void xme_adv_testHeartbeat_destroy(xme_adv_testHeartbeat_configStruct_t* config) {
	XME_HAL_TABLE_FINI(xme_adv_testHeartbeat_heartbeatTable);
	if ( xme_core_dcc_unsubscribeTopic(config->subHandle) != XME_CORE_STATUS_SUCCESS )
	{
		XME_LOG(XME_LOG_WARNING, "Failure xme_core_dcc_unsubscribeTopic\n");
	}

	if ( xme_core_dcc_unpublishTopic(config->pubHandle) != XME_CORE_STATUS_SUCCESS )
	{
		XME_LOG(XME_LOG_WARNING, "Failure xme_core_dcc_unsubscribeTopic\n");
	}
}

void receive_heartbeatMultiNode
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
)
{
	xme_adv_healthmonitor_heartbeat_t* hb;
	bool foundNode = false;
	hb = (xme_adv_healthmonitor_heartbeat_t*) xme_hal_sharedPtr_getPointer( dataHandle );

	XME_LOG(XME_LOG_DEBUG, "heartbeat received (id %d)\n", hb->nodeId);

	XME_HAL_TABLE_ITERATE
	(
		xme_adv_testHeartbeat_heartbeatTable,
		xme_hal_table_rowHandle_t, heartbeatHandle,
		xme_adv_healthmonitor_heartbeat_t, heartbeatItem,
		{
			if (hb->nodeId == heartbeatItem->nodeId)
			{
				heartbeatItem->maxTimestampAge = xme_hal_time_getCurrentTime();
				foundNode = true;
			}
		}
	);

	if (!foundNode && catchAllHeartbeats) {
		xme_hal_table_rowHandle_t handle;
		xme_adv_healthmonitor_heartbeat_t* inRow;

		handle = XME_HAL_TABLE_ADD_ITEM(xme_adv_testHeartbeat_heartbeatTable);
		inRow = XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_adv_testHeartbeat_heartbeatTable, handle );
		inRow->maxTimestampAge = xme_hal_time_getCurrentTime();
		inRow->nodeId = hb->nodeId;
	}
}

void testHeartbeat_callback (void* userData)
{
	xme_adv_healthmonitor_errorMessage_t error;
	xme_hal_time_interval_t timeStampAge;
	xme_adv_testHeartbeat_configStruct_t* config = (xme_adv_testHeartbeat_configStruct_t*)userData;
	
	XME_HAL_TABLE_ITERATE
	(
		xme_adv_testHeartbeat_heartbeatTable,
		xme_hal_table_rowHandle_t, heartbeatHandle,
		xme_adv_healthmonitor_heartbeat_t, heartbeatItem,
		{
			if(-1 != heartbeatItem->maxTimestampAge) {
				// check age of timestamp
				timeStampAge = xme_hal_time_getInterval(&heartbeatItem->maxTimestampAge, false);

				if (timeStampAge >= maxTimestampAge) {
					error.componentId = xme_core_resourceManager_getCurrentComponentId();
					error.nodeId = heartbeatItem->nodeId;
					error.status = XME_ADV_HEALTHMONITOR_TEST_FAILED;
					error.identifier = XME_ADV_TEST_HEARTBEAT;
					xme_core_dcc_sendTopicData(config->pubHandle, &error, sizeof(error)); // send error message
					debugErrorCounter++;
					XME_LOG(XME_LOG_ERROR, "XME_ADV_HEALTHMONITOR_TEST_FAILED for node %d -> XME_ADV_TEST_HEARTBEAT\n", heartbeatItem->nodeId);

					// instant error reaction
					if (config->callback != NULL) {
						config->callback();
					} else {
						printf("error function not defined\n");
					}
				}
			}
		}
	);
}

void initHeartbeatTable ()
{
	xme_hal_table_rowHandle_t handle;
	xme_adv_healthmonitor_heartbeat_t* inRow;
	int i;
		
	for(i=0; i < 49; i++) {
		handle = XME_HAL_TABLE_ADD_ITEM(xme_adv_testHeartbeat_heartbeatTable);
		inRow = XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_adv_testHeartbeat_heartbeatTable, handle );
		inRow->maxTimestampAge = -1;
		inRow->nodeId = (xme_core_device_guid_t)i;
	}
}

void debugHeartbeatTable ()
{
	XME_LOG(XME_LOG_DEBUG, "Heartbeat Table \n");
	XME_LOG(XME_LOG_DEBUG, "--------------- \n");
	XME_HAL_TABLE_ITERATE
	(
		xme_adv_testHeartbeat_heartbeatTable,
		xme_hal_table_rowHandle_t, heartbeatHandle,
		xme_adv_healthmonitor_heartbeat_t, heartbeatItem,
		{
			XME_LOG(XME_LOG_DEBUG, "node %d, timestamp %d \n", heartbeatItem->nodeId, heartbeatItem->maxTimestampAge);
		}
	);
}
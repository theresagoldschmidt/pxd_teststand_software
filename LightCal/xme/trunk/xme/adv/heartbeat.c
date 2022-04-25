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
 *         Heartbeat sender component.
 *
 * \author
 *         Dominik Sojer <sojer@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/core.h"
#include "xme/core/broker.h"
#include "xme/core/defines.h"
#include "xme/core/nodeManager.h"
#include "xme/core/interfaceManager.h"
#include "heartbeat.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/

/******************************************************************************/
/***   Component configurations                                             ***/
/******************************************************************************/

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t xme_adv_heartbeat_create(xme_adv_heartbeat_configStruct_t* config)
{
	config->taskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;

	// Create publisher for heartbeat topic
	config->pubHandle = xme_core_dcc_publishTopic(
		HEARTBEAT_TOPIC_ID,
		XME_CORE_MD_EMPTY_META_DATA,
		false,
		NULL
	);

	if ( config->pubHandle == XME_CORE_DCC_INVALID_PUBLICATION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Local channel from heartbeat component
	if ( xme_core_routingTable_addLocalSourceRoute( 
		HEARTBEAT_CHANNEL, 
		xme_core_dcc_getComponentFromPublicationHandle(config->pubHandle),
		xme_core_dcc_getPortFromPublicationHandle(config->pubHandle) 
	) != XME_CORE_STATUS_SUCCESS )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Remote channel from heartbeat component
	if ( xme_core_routingTable_addOutboundRoute(
		HEARTBEAT_CHANNEL,
		(xme_core_interface_interfaceId_t) 1
	) != XME_CORE_STATUS_SUCCESS )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t xme_adv_heartbeat_activate(xme_adv_heartbeat_configStruct_t* config)
{
	config->taskHandle = xme_hal_sched_addTask(config->interval, config->interval, 0, heartbeatComponent_callback, config);
	return XME_CORE_STATUS_SUCCESS;
}


void xme_adv_heartbeat_deactivate(xme_adv_heartbeat_configStruct_t* config)
{
	xme_hal_sched_removeTask(config->taskHandle);
	config->taskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;
}

void xme_adv_heartbeat_destroy(xme_adv_heartbeat_configStruct_t* config)
{
	if ( xme_core_dcc_unpublishTopic(config->pubHandle) != XME_CORE_STATUS_SUCCESS )
	{
		XME_LOG(XME_LOG_WARNING, "Failure xme_core_dcc_unsubscribeTopic\n");
	}
	config->pubHandle = XME_CORE_DCC_INVALID_PUBLICATION_HANDLE;

}

void heartbeatComponent_callback (void* userData)
{
	xme_adv_heartbeat_configStruct_t* config = (xme_adv_heartbeat_configStruct_t*)userData;
	xme_adv_healthmonitor_heartbeat_t hb;

	hb.nodeId = xme_core_nodeManager_getDeviceGuid();
	XME_LOG(XME_LOG_DEBUG, "sending heartbeat (id %d)\n", hb.nodeId);
	printf("test");
	xme_core_dcc_sendTopicData(config->pubHandle, &hb, sizeof(hb));
}
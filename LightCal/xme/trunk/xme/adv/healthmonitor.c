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
 *         Health monitor component.
 *
 * \author
 *         Dominik Sojer <sojer@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/

#include "xme/core/nodeManager.h"
#include "healthmonitor.h"
#include "heartbeat.h"
#include "xme/hal/time.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
XME_HAL_TABLE
(
	xme_adv_healthmonitor_errorMessage_t,
	xme_adv_healthmonitor_errorLog,
	XME_ADV_HEALTHMONITOR_ERRORS_MAX
);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/

static int monitorComponent; // monitor component instead of node?

xme_core_status_t xme_adv_healthmonitor_create(xme_adv_healthmonitor_configStruct_t* config)
{
	// if componentId == -1 then HM is monitoring a complete node
	if (config->componentId == -1) {
		monitorComponent = 0;
	} else {
		monitorComponent = 1;
	}

	XME_HAL_TABLE_INIT( xme_adv_healthmonitor_errorLog );

	// create error message publication, subscription and routes
	if (XME_CORE_STATUS_SUCCESS != createHealthmonitorErrorMessages(config)) 
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}
	
	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t xme_adv_healthmonitor_activate(xme_adv_healthmonitor_configStruct_t* config)
{
	return XME_CORE_STATUS_SUCCESS;
}


void xme_adv_healthmonitor_deactivate(xme_adv_healthmonitor_configStruct_t* config)
{
}

void xme_adv_healthmonitor_destroy(xme_adv_healthmonitor_configStruct_t* config) {
	if ( xme_core_dcc_unsubscribeTopic(config->subHandleErrors) != XME_CORE_STATUS_SUCCESS )
	{
		XME_LOG(XME_LOG_WARNING, "Failure xme_core_dcc_unsubscribeTopic\n");
	}
	if ( xme_core_dcc_unpublishTopic(config->pubHandleErrors) != XME_CORE_STATUS_SUCCESS )
	{
		XME_LOG(XME_LOG_WARNING, "Failure xme_core_dcc_unsubscribeTopic\n");
	}

	XME_HAL_TABLE_FINI(xme_adv_healthmonitor_errorLog);
}

void receive_errorMessage
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
)
{
	xme_adv_healthmonitor_errorMessage_t* error;
	xme_adv_healthmonitor_configStruct_t* config;
	xme_hal_table_rowHandle_t handle;
	xme_adv_healthmonitor_errorMessage_t* inRow;
	xme_adv_healthmonitor_errorMessage_t escalateError;

	error = (xme_adv_healthmonitor_errorMessage_t*) xme_hal_sharedPtr_getPointer( dataHandle );
	config = (xme_adv_healthmonitor_configStruct_t*)userData;
	
	// handle error message from monitored node
	if (error->nodeId == config->nodeId) 
	{
		XME_LOG(XME_LOG_DEBUG, "Health Monitor: Received error message (nodeId %d) '%s'", error->nodeId, printHealthmonitorErrNo(error->status));

		// insert error message in error log
		if (XME_ADV_HEALTHMONITOR_TEST_OK != error->status &&
			((0 == monitorComponent) || (1 == monitorComponent && error->componentId == config->componentId)))
		{
			handle = XME_HAL_TABLE_ADD_ITEM(xme_adv_healthmonitor_errorLog);
			inRow = XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_adv_healthmonitor_errorLog, handle );
			inRow = error;
			XME_LOG(XME_LOG_DEBUG, " and stored it in error log. \n");
		} else {
			// check for possible test reset
			XME_HAL_TABLE_ITERATE
			(
				xme_adv_healthmonitor_errorLog,
				xme_hal_table_rowHandle_t, errorMsgHandle,
				xme_adv_healthmonitor_errorMessage_t, tableItem,
				{
					if (error->nodeId == tableItem->nodeId &&
						error->identifier == tableItem->identifier &&
						error->status == XME_ADV_HEALTHMONITOR_TEST_OK &&
						((0 == monitorComponent) || (1 == monitorComponent && error->componentId == tableItem->componentId)))
					{
						// if the same test reported an error earlier but is now ok, then remove it
						XME_HAL_TABLE_REMOVE_ITEM(xme_adv_healthmonitor_errorLog, errorMsgHandle);
						XME_LOG(XME_LOG_DEBUG, " and reset it in error log.");
					}
				}
			);
			XME_LOG(XME_LOG_DEBUG, "\n");
		}

		// check health
		if (XME_CORE_STATUS_SUCCESS != checkHealth())
		{
			// init error reaction
			escalateError.nodeId = error->nodeId;

			// distinguish between node and component monitoring
			if (0 == monitorComponent) 
			{
				escalateError.status = XME_ADV_HEALTHMONITOR_NODE_EXCEPTION;
			} else {
				escalateError.status = XME_ADV_HEALTHMONITOR_COMPONENT_EXCEPTION;
				escalateError.componentId = error->componentId;
			}
			escalateError.identifier = XME_ADV_TEST_NOTEST;
			xme_core_dcc_sendTopicData(config->pubHandleErrors, &escalateError, sizeof(escalateError));
			XME_LOG(XME_LOG_DEBUG, "Health Monitor: Sent error message (nodeId %d): %s \n", escalateError.nodeId, printHealthmonitorErrNo(escalateError.status));

		}
	}
}

char* printHealthmonitorErrNo(xme_adv_healthmonitor_status_t error)
{
	switch(error)
	{
	case 0:
		return "XME_ADV_HEALTHMONITOR_COMPONENT_OK";
	case 1:
		return "XME_ADV_HEALTHMONITOR_COMPONENT_EXCEPTION";
	case 2:
		return "XME_ADV_HEALTHMONITOR_COMPONENT_UNKNOWN";
	case 3:
		return "XME_ADV_HEALTHMONITOR_TEST_OK";
	case 4:
		return "XME_ADV_HEALTHMONITOR_TEST_UNKNOWN";
	case 5:
		return "XME_ADV_HEALTHMONITOR_TEST_FAILED";
	case 6:
		return 	"XME_ADV_HEALTHMONITOR_NODE_OK";
	case 7:
		return "XME_ADV_HEALTHMONITOR_NODE_UNKNOWN";
	case 8:
		return "XME_ADV_HEALTHMONITOR_NODE_EXCEPTION";
	}

	return "XME_ADV_HEALTHMONITOR_STATUS_UNKNOWN";
}

xme_core_status_t createHealthmonitorErrorMessages(xme_adv_healthmonitor_configStruct_t* config)
{
	// Create publisher for error messages
	config->pubHandleErrors = xme_core_dcc_publishTopic( 
		ERROR_MESSAGE_TOPIC_ID, 
		XME_CORE_MD_EMPTY_META_DATA,
		false,
		NULL
	);

	if ( config->pubHandleErrors == XME_CORE_DCC_INVALID_PUBLICATION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	} 

	// Local channel from health monitor component (error messages)
	if ( xme_core_routingTable_addLocalSourceRoute( 
		ERROR_MESSAGE_CHANNEL, 
		xme_core_dcc_getComponentFromPublicationHandle(config->pubHandleErrors),
		xme_core_dcc_getPortFromPublicationHandle(config->pubHandleErrors) 
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Create subscription to error messages
	config->subHandleErrors = xme_core_dcc_subscribeTopic( 
		ERROR_MESSAGE_TOPIC_ID, 
		XME_CORE_MD_EMPTY_META_DATA, 
		false,
		receive_errorMessage, 
		NULL 
	);

	if ( config->subHandleErrors == XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Local channel to health monitor component (error messages)
	if ( xme_core_routingTable_addLocalDestinationRoute(
		ERROR_MESSAGE_CHANNEL,
		xme_core_dcc_getComponentFromSubscriptionHandle(config->subHandleErrors),
		xme_core_dcc_getPortFromSubscriptionHandle(config->subHandleErrors)
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};
	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
checkHealth()
{
	/* TODO: implement rule check. See ticket #1387 */
	return XME_CORE_STATUS_SUCCESS;
}
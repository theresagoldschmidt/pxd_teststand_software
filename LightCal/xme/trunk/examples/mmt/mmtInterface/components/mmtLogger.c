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
 *         Multifunk Modeling Tool (MMT) logger component.
 *         Listens for log messages from XME components and passes them to the 
 *         MMT.
 *
 * \author
 *         Benjamin Wiesmueller <wiesmueller@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdbool.h>

#include "xme/core/core.h"
#include "xme/core/defines.h"

#include "xme/hal/sharedPtr.h"

#include "mmtLogger.h"

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Callback function for listening for log messages.
 *
 * \see    xme_core_dcc_receiveTopicCallback_t
 */
void
xme_adv_mmtLogger_receiveLogMsgTopicCallback
(
	xme_hal_sharedPtr_t dataHandle, 
	void* userData
);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t 
xme_adv_mmtLogger_create(xme_adv_mmtLogger_configStruct_t* config)
{
	// Initialize the private part of the configuration structure
	config->logMsgSubscriptionHandlerHandle = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;

	// Subscribe to log messages
	XME_CHECK
	(
		XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE !=
		(
			config->logMsgSubscriptionHandlerHandle = xme_core_dcc_subscribeTopic
			(
				XME_CORE_TOPIC_LOG_MESSAGE, // TODO: Adjust topic and callback when ticket #665 is resolved.
				XME_CORE_MD_EMPTY_META_DATA,
				false,
				&xme_adv_mmtLogger_receiveLogMsgTopicCallback,
				config
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t 
xme_adv_mmtLogger_activate(xme_adv_mmtLogger_configStruct_t* config)
{
	return XME_CORE_STATUS_SUCCESS;
}

void 
xme_adv_mmtLogger_deactivate(xme_adv_mmtLogger_configStruct_t* config)
{
}

void 
xme_adv_mmtLogger_destroy(xme_adv_mmtLogger_configStruct_t* config)
{
	xme_core_dcc_unsubscribeTopic(config->logMsgSubscriptionHandlerHandle);
	config->logMsgSubscriptionHandlerHandle = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;
}

void
xme_adv_mmtLogger_receiveLogMsgTopicCallback(xme_hal_sharedPtr_t dataHandle, void* userData) 
{
	xme_adv_mmtLogger_configStruct_t* config;
	xme_core_topic_logMsgData_t* logMsg;
	void* data;
	uint16_t dataSize;
	char debugMsg[512];
	mmt_log_t mmtLog; // Log message as passed to the modeling tool
	
	config = (xme_adv_mmtLogger_configStruct_t*)userData;
	XME_ASSERT_NORVAL(NULL != config);

	// Reconstruct topic data
	data = xme_hal_sharedPtr_getPointer(dataHandle);
	logMsg = (xme_core_topic_logMsgData_t*)data;
	dataSize = xme_hal_sharedPtr_getSize(dataHandle);
	XME_CHECK(dataSize >= sizeof(xme_core_topic_logMsgData_t), );
	XME_CHECK(dataSize == sizeof(xme_core_topic_logMsgData_t) + logMsg->msgSize + logMsg->compNameSize, );

	mmtLog.compName = (char*)data + sizeof(xme_core_topic_logMsgData_t) + logMsg->msgSize;
	mmtLog.msg = (char*)data + sizeof(xme_core_topic_logMsgData_t); 
	mmtLog.componentId = logMsg->componentId;
	mmtLog.severity = logMsg->severity;
	mmtLog.deviceGuid = logMsg->deviceGuid;

#ifdef WIN32
	sprintf_s(
#else // #ifdef WIN32
	snprintf(
#endif // #ifdef WIN32
	   debugMsg,
	   512,
	   "mmtLogger: received message: %llu, %d, %d, %s, %s\n", mmtLog.deviceGuid, mmtLog.severity, mmtLog.componentId, mmtLog.compName, mmtLog.msg
	);
	config->mmt_println(debugMsg);

	config->mmt_logCallback(mmtLog);
}

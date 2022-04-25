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
 *         Console logger.
 *
 *         Receives XME_LOG messages and prints them to the console.
 *
 * \author
 *         Benjamin Wiesmueller <wiesmueller@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/prim/consoleLogger.h"

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
void
xme_prim_consoleLogger_receiveDataCallback
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_prim_consoleLogger_create
(
	xme_prim_consoleLogger_configStruct_t* config
)
{
	// Intiailize private part of configuration structure
	config->subscriptionHandle = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;

	// Subscribe to topic
	XME_CHECK
	(
		XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE !=
		(
			config->subscriptionHandle = xme_core_dcc_subscribeTopic( 
				XME_CORE_TOPIC_LOG_MESSAGE,
				XME_CORE_MD_EMPTY_META_DATA,
				config->localOnly,
				&xme_prim_consoleLogger_receiveDataCallback,
				config
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_prim_consoleLogger_activate
(
	xme_prim_consoleLogger_configStruct_t* config
)
{
	// Nothing to do

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_prim_consoleLogger_deactivate
(
	xme_prim_consoleLogger_configStruct_t* config
)
{
	// Nothing to do
}

void
xme_prim_consoleLogger_destroy
(
	xme_prim_consoleLogger_configStruct_t* config
)
{
	// Unsubscribe from topic
	xme_core_dcc_unsubscribeTopic(config->subscriptionHandle);
	config->subscriptionHandle = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;
}

void
xme_prim_consoleLogger_receiveDataCallback
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
)
{
	xme_prim_consoleLogger_configStruct_t* config;
	xme_core_topic_logMsgData_t* logMsg;
	void* data;
	const char* message;
	const char* compName;
	uint16_t dataSize;
	static const char* format= (sizeof(long long)==8) ? ("   (%s, %d, 0x%llX, 0x%llX)\n") : ("   (%s, %d, 0x%lX, 0x%lX)\n");
	
	config = (xme_prim_consoleLogger_configStruct_t*)userData;
	XME_ASSERT_NORVAL(NULL != config);

	// Reconstruct topic data
	data = xme_hal_sharedPtr_getPointer(dataHandle);
	logMsg = (xme_core_topic_logMsgData_t*)data;
	dataSize = xme_hal_sharedPtr_getSize(dataHandle);
	XME_CHECK(dataSize >= sizeof(xme_core_topic_logMsgData_t), );
	XME_CHECK(dataSize == sizeof(xme_core_topic_logMsgData_t) + logMsg->msgSize + logMsg->compNameSize, );

	// Check sverity of message vs. minSeverity of logger
	if (logMsg->severity < config->minSeverity) { return; }

	// Reconstruct log message (appended after topic struct)
	message = (char*)data + sizeof(xme_core_topic_logMsgData_t); 
	
	printf 
	(
		"%s%s",
		(XME_LOG_WARNING == logMsg->severity) ? "Warning: " : (
		(XME_LOG_ERROR == logMsg->severity) ? "Error: " : (
		(XME_LOG_FATAL == logMsg->severity) ? "Fatal: " : (
		(XME_LOG_VERBOSE == logMsg->severity) ? "Verbose: " : (
		(XME_LOG_DEBUG == logMsg->severity) ? "Debug: " : "")))),
		message
	);

	if (config->verbose)
	{
		if (logMsg->compNameSize > 0)
		{
			// Reconstruct component name (appended after log message)
			compName = (char*)data + sizeof(xme_core_topic_logMsgData_t) + logMsg->msgSize;
		}

		printf
		(
			format,
			(logMsg->compNameSize > 0) ? compName : "n/a",
			logMsg->componentId,
			logMsg->deviceType,
			logMsg->deviceGuid
		);
	}
}

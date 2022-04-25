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
 *         Logger component.
 *
 *         Publishes and sends log messages.
 *
 * \author
 *         Benjamin Wiesmueller <wiesmueller@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/component.h"
#include "xme/core/logger.h"
#include "xme/core/nodeManager.h"
#include "xme/core/resourceManager.h"

#include "xme/hal/mem.h"

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
/**
 * Static configuration structure of logger component.
 * Configurations passed in the create/(de-)activate/destroy functions will be
 * ignored.
 * NULL value means that the logger is not initialized.
 */
static xme_core_logger_configStruct_t* xme_core_logger_staticConfig = NULL;

/******************************************************************************/
/***   Global variables                                                     ***/
/******************************************************************************/
extern xme_core_log_logCallback_t xme_core_log_logCallback;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Log a single message.
 * 
 * \param  severity Severity of log message.
 * \param  message Log message.
 */
void
xme_core_logger_log
(
	xme_log_severity_t severity, 
	const char* message
);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_core_logger_create(xme_core_logger_configStruct_t* config)
{
	static xme_core_logger_configStruct_t xme_core_logger_config;
	
	// All initialization is done here (config parameter ignored)
	xme_core_logger_staticConfig = &xme_core_logger_config;
	xme_core_log_logCallback = &xme_core_logger_log;

	// Initialize configuration
	xme_core_logger_staticConfig->publicationHandle = XME_CORE_DCC_INVALID_PUBLICATION_HANDLE;
	xme_core_logger_staticConfig->logNestingLvlTlsHandle = XME_HAL_TLS_INVALID_TLS_HANDLE;

	// Publish log message topic
	XME_CHECK
	(
		XME_CORE_DCC_INVALID_PUBLICATION_HANDLE !=
		(
			xme_core_logger_staticConfig->publicationHandle = xme_core_dcc_publishTopic
			( 
				XME_CORE_TOPIC_LOG_MESSAGE,
				XME_CORE_MD_EMPTY_META_DATA,
				false,
				NULL
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// Allocate thread local storage memory for logNestingLvl
	XME_CHECK_REC
	(
		XME_HAL_TLS_INVALID_TLS_HANDLE !=
		(
			xme_core_logger_staticConfig->logNestingLvlTlsHandle = xme_hal_tls_alloc
			(
				sizeof(uint8_t)
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES,
		// Recovery
		xme_core_dcc_unpublishTopic(xme_core_logger_staticConfig->publicationHandle);
		xme_core_logger_staticConfig->publicationHandle = XME_CORE_DCC_INVALID_PUBLICATION_HANDLE;
	);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_logger_activate(xme_core_logger_configStruct_t* config)
{
	// Nothing to do
	return XME_CORE_STATUS_SUCCESS;
}

void
xme_core_logger_deactivate(xme_core_logger_configStruct_t* config)
{
	// Nothing to do
}

void
xme_core_logger_destroy(xme_core_logger_configStruct_t* config)
{
	xme_core_logger_staticConfig = NULL;
	// Unsubscribe log message topic
	xme_core_dcc_unpublishTopic(xme_core_logger_staticConfig->publicationHandle);
	xme_core_logger_staticConfig->publicationHandle = XME_CORE_DCC_INVALID_PUBLICATION_HANDLE;
	// Free thread local storage for logNestingLvl
	xme_hal_tls_free(xme_core_logger_staticConfig->logNestingLvlTlsHandle);
	xme_core_logger_staticConfig->logNestingLvlTlsHandle = XME_HAL_TLS_INVALID_TLS_HANDLE;
}

void
xme_core_logger_log
(
	xme_log_severity_t severity, 
	const char* message
)
{
	xme_core_topic_logMsgData_t* logMsg;
	size_t dataSize;
	uint8_t* nestingLvl;
	xme_core_component_t componentId;
	const char* compName;
	uint16_t compNameSize;
	uint16_t msgSize;

	// Do nothing if the logger is not initialized
	if (NULL == xme_core_logger_staticConfig) { return; }

	// Get callCount of this thread (nesting level of this function for recursive calls)
	nestingLvl = (uint8_t*)xme_hal_tls_get
	(
		xme_core_logger_staticConfig->logNestingLvlTlsHandle
	);
	// Return when nestingLvl could not be determined
	XME_CHECK(NULL != nestingLvl,);
	
	// Return when nesting level is too high
	// This prevents endless recursion when XME_LOG is used during xme_core_dcc_sendTopicData()
	if (*nestingLvl >= 2) { return; }

	componentId = xme_core_resourceManager_getCurrentComponentId();
	compName = xme_core_resourceManager_getComponentName(componentId);
	msgSize = (uint16_t)strlen(message) + 1;
	compNameSize = (NULL != compName) ? (uint16_t)strlen(compName) + 1 : 0;

	// Compute total size of data that will be sent
	dataSize = sizeof(xme_core_topic_logMsgData_t) + msgSize + compNameSize;

	// Allocate memory for topic data struct and appended data
	logMsg = (xme_core_topic_logMsgData_t*)xme_hal_mem_alloc(dataSize);

	// Set topic data
	logMsg->deviceType = xme_core_nodeManager_getDeviceType();
	logMsg->deviceGuid = xme_core_nodeManager_getDeviceGuid();
	logMsg->componentId = componentId;
	logMsg->severity = severity;
	logMsg->msgSize = msgSize;
	logMsg->compNameSize = compNameSize;

	// Append log message and component name
	xme_hal_mem_copy
	(
		(char*)logMsg + sizeof(xme_core_topic_logMsgData_t),
		message, 
		(size_t)logMsg->msgSize
	);
	if (NULL != compName)
	{
		xme_hal_mem_copy
		(
			(char*)logMsg + sizeof(xme_core_topic_logMsgData_t) + logMsg->msgSize,
			compName, 
			(size_t)logMsg->compNameSize
		);
	}

	(*nestingLvl)++;

	xme_core_dcc_sendTopicData(xme_core_logger_staticConfig->publicationHandle, (void*)logMsg, dataSize);

	(*nestingLvl)--;

	xme_hal_mem_free((void*)logMsg);
}
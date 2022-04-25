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
 *         Logger registry.
 *
 * \author
 *         Benjamin Wiesmueller <wiesmueller@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/component.h"
#include "xme/core/logReg.h"
#include "xme/core/resourceManager.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_core_logReg_loggerDescriptor_t
 *
 * \brief Entry for one registered logger.
 */
typedef struct
{
	xme_core_component_t componentId; ///< Component id of logger.
	xme_core_logReg_loggerHandle_t loggerHandle; ///< Logger handle.
	xme_core_logReg_loggerCallback_t callback; ///< Callback funcion.
	xme_log_severity_t minSeverity; ///< Minimum severity.
	void* userData; ///< Pointer to additional data for the logger.
}
xme_core_logReg_loggerDescriptor_t;

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
static XME_HAL_TABLE
(
	xme_core_logReg_loggerDescriptor_t,
	xme_core_logReg_logRegister,
	XME_HAL_TABLE_MAX_ROW_HANDLE
);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_core_logReg_init()
{
	XME_HAL_TABLE_INIT(xme_core_logReg_logRegister);

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_core_logReg_fini()
{
	XME_HAL_TABLE_FINI(xme_core_logReg_logRegister);
}

xme_core_logReg_loggerHandle_t
xme_core_logReg_registerLogger
(
	xme_log_severity_t minSeverity,
	xme_core_logReg_loggerCallback_t callback,
	void* userData
)
{
	xme_hal_table_rowHandle_t handle;
	xme_core_logReg_loggerDescriptor_t* loggerDescriptor;
	xme_core_component_t loggerCompId;

	XME_CHECK(XME_HAL_TABLE_ITEM_COUNT(xme_core_logReg_logRegister) <= XME_HAL_TABLE_MAX_ROW_HANDLE, XME_CORE_LOGREG_INVALID_LOGGER_HANDLE);

	loggerCompId = xme_core_resourceManager_getCurrentComponentId();
	
	XME_CHECK(loggerCompId != XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT, XME_CORE_LOGREG_INVALID_LOGGER_HANDLE);

	XME_CHECK
	(
		XME_HAL_TABLE_INVALID_ROW_HANDLE !=
		(
			handle = XME_HAL_TABLE_ADD_ITEM(xme_core_logReg_logRegister)
		),
		XME_CORE_LOGREG_INVALID_LOGGER_HANDLE
	);
	
	loggerDescriptor = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_logReg_logRegister, handle);
	loggerDescriptor->componentId = xme_core_resourceManager_getCurrentComponentId();
	loggerDescriptor->loggerHandle = (xme_core_logReg_loggerHandle_t)handle;
	loggerDescriptor->callback = callback;
	loggerDescriptor->minSeverity = minSeverity;
	loggerDescriptor->userData = userData;

	return loggerDescriptor->loggerHandle;
}

xme_core_status_t
xme_core_logReg_deregisterLogger
(
	xme_core_logReg_loggerHandle_t loggerHandle
)
{
	XME_CHECK
	(
		XME_CORE_STATUS_INVALID_HANDLE !=
		(
			XME_HAL_TABLE_REMOVE_ITEM(xme_core_logReg_logRegister, (xme_hal_table_rowHandle_t)loggerHandle)
		),
		XME_CORE_STATUS_INVALID_HANDLE
	);

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_core_logReg_log
(
	xme_log_severity_t severity, 
	const char* message
)
{
	xme_core_component_t componentId; ///< Id of component sending the log message

	componentId = xme_core_resourceManager_getCurrentComponentId();
	
	XME_CHECK(XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT != componentId, );

	// Check if the component sending the log message is a registered logger
	// If yes do nothing and return to prevent endless recursion of log messages
	// (e.g. via broker XME_LOGs when sending data via DCC)
	// TODO (ticket #641): Is there a better way to prevent this, that allows loggers to send
	//                     log messages (and that is thread-safe)?
	XME_HAL_TABLE_ITERATE
	(
		xme_core_logReg_logRegister,
		xme_hal_table_rowHandle_t, handle,
		xme_core_logReg_loggerDescriptor_t, loggerDescriptor,
		{
			if (loggerDescriptor->componentId == componentId)
			{
				return;
			}
		}
	);

	// Iterate over all registered loggers and call their callback function if their minSeverity >= severity
	XME_HAL_TABLE_ITERATE
	(
		xme_core_logReg_logRegister,
		xme_hal_table_rowHandle_t, handle,
		xme_core_logReg_loggerDescriptor_t, loggerDescriptor,
		{
			if (severity >= loggerDescriptor->minSeverity)
			{
				XME_COMPONENT_CONTEXT
				(
					loggerDescriptor->componentId,
					loggerDescriptor->callback(componentId, severity, message, loggerDescriptor->userData);
				);		
			}
		}
	);
}

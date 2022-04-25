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
 *         Log message generator.
 *
 *         Regularly generates log messages with increasing severity level. 
 *         Interval can be specified in configuration. 
 *         Can be used to test and demonstrate logging. *
 * \author
 *         Benjamin Wiesmueller <wiesmueller@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/prim/logMsgGenerator.h"

#include "xme/core/component.h"
#include "xme/core/dcc.h"
#include "xme/core/defines.h"
#include "xme/core/resourceManager.h"
#include "xme/core/log.h"
#include "xme/core/logReg.h"

/******************************************************************************/
/***   Forward declarations                                                 ***/
/******************************************************************************/
void
xme_prim_logMsgGenerator_taskCallback(void* userData);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_prim_logMsgGenerator_create(xme_prim_logMsgGenerator_configStruct_t* config)
{
	// Initialize configuration structure
	config->taskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;
	config->i = 0;
	config->componentId = xme_core_resourceManager_getCurrentComponentId();

	XME_CHECK
	(
		XME_HAL_SCHED_INVALID_TASK_HANDLE !=
		(
			config->taskHandle = xme_core_resourceManager_scheduleTask
			(
				XME_HAL_SCHED_TASK_INITIALLY_SUSPENDED,
				config->intervalMs,
				XME_HAL_SCHED_PRIORITY_NORMAL,
				&xme_prim_logMsgGenerator_taskCallback,
				config
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_prim_logMsgGenerator_activate(xme_prim_logMsgGenerator_configStruct_t* config)
{
	XME_CHECK
	(
		XME_CORE_STATUS_SUCCESS == xme_hal_sched_setTaskExecutionState(config->taskHandle, true),
		XME_CORE_STATUS_INTERNAL_ERROR
	);

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_prim_logMsgGenerator_deactivate(xme_prim_logMsgGenerator_configStruct_t* config)
{
	XME_ASSERT_NORVAL(XME_CORE_STATUS_SUCCESS == xme_hal_sched_setTaskExecutionState(config->taskHandle, false));
}

void
xme_prim_logMsgGenerator_destroy(xme_prim_logMsgGenerator_configStruct_t* config)
{
	config->componentId = XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT;

	XME_ASSERT_NORVAL(XME_CORE_STATUS_SUCCESS == xme_hal_sched_removeTask(config->taskHandle));
	config->taskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;
}

void
xme_prim_logMsgGenerator_taskCallback(void* userData)
{
	xme_prim_logMsgGenerator_configStruct_t* config = (xme_prim_logMsgGenerator_configStruct_t*)userData;
	static xme_log_severity_t logSeverities[] = {XME_LOG_DEBUG, XME_LOG_VERBOSE, XME_LOG_NOTE, XME_LOG_WARNING, XME_LOG_ERROR, XME_LOG_FATAL};

	XME_LOG
	(
		logSeverities[config->i],
		"Log message from logMsgGenerator\n"
	);

	config->i++;
	if (config->i > 5) 
	{
		config->i = 0;
	} 
}

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
 *         Dummy implementation of mmtUpdater.
 *
 * \author
 *         Benjamin Wiesmueller <wiesmueller@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdbool.h>

#include "xme/core/core.h"
#include "xme/core/broker.h"
#include "xme/core/defines.h"
#include "xme/core/nodeManager.h"

#include "mmtUpdaterDummy.h"

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

/**
 * \brief  Send some fake new nodes to the Multifunk tool.
 *
 * \see    xme_hal_sched_taskCallback_t
 */
void
xme_adv_mmtUpdaterDummy_taskCallback(void* userData);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/

xme_core_status_t 
xme_adv_mmtUpdaterDummy_create(xme_adv_mmtUpdaterDummy_configStruct_t* config)
{
	// Initialize the private part of the configuration structure
	config->taskCallbackTaskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;

	// Create a task that will send (fake) new nodes to the Multifunk Tool for testing purposes
	XME_CHECK
	(
		XME_HAL_SCHED_INVALID_TASK_HANDLE !=
		(
			config->taskCallbackTaskHandle =
				xme_hal_sched_addTask
				(
					XME_HAL_SCHED_TASK_INITIALLY_SUSPENDED,
					3000,
					XME_HAL_SCHED_PRIORITY_NORMAL,
					&xme_adv_mmtUpdaterDummy_taskCallback,
					config
				)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t 
xme_adv_mmtUpdaterDummy_activate(xme_adv_mmtUpdaterDummy_configStruct_t* config)
{
	config->mmt_println("#####################################");
	config->mmt_println("XME: xme_adv_mmtUpdaterDummy_activate");

	XME_CHECK
	(
		XME_CORE_STATUS_SUCCESS ==
			xme_hal_sched_setTaskExecutionState(config->taskCallbackTaskHandle, true),
		XME_CORE_STATUS_INTERNAL_ERROR
	);
	
	return XME_CORE_STATUS_SUCCESS;
}

void 
xme_adv_mmtUpdaterDummy_deactivate(xme_adv_mmtUpdaterDummy_configStruct_t* config)
{
	xme_hal_sched_setTaskExecutionState(config->taskCallbackTaskHandle, false);
}

void 
xme_adv_mmtUpdaterDummy_destroy(xme_adv_mmtUpdaterDummy_configStruct_t* config)
{
	xme_hal_sched_removeTask(config->taskCallbackTaskHandle);
	config->taskCallbackTaskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;
}

void 
xme_adv_mmtUpdaterDummy_taskCallback(void* userData)
{
	xme_adv_mmtUpdaterDummy_configStruct_t* config; // Configuration struct of mmtUpdater, passed in userData
	mmt_upd_t update; // Update message
	xme_core_topic_loginRequestData_t requestData; // requestData will be passed as data for a NEW_NODE update message
	static int i = 0; // For testing

	// Initialize variables
	config = (xme_adv_mmtUpdaterDummy_configStruct_t*)userData;

	config->mmt_println("XME: Sending new update");

	// Create update message
	requestData.deviceGuid = i;
	requestData.deviceType = (i<2) ? 0 : 1;

	update.type = NEW_NODE;
	update.len = sizeof(xme_core_topic_loginRequestData_t);
	update.data = (char*)&requestData;

	i++;

	config->mmt_updCallback(update);
}

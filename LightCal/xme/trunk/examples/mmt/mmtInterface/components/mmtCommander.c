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
 *         Multifunk Modeling Tool (MMT) commander component.
 *         Receives commands from the MMT.
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

#include "mmtCommander.h"

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

/**
 * \brief  Callback function for listening for commands from the Multifunk Tool.
 *
 * \see    xme_hal_sched_taskCallback_t
 */
void
xme_adv_mmtCommander_taskCallback(void* userData);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/

xme_core_status_t 
xme_adv_mmtCommander_create(xme_adv_mmtCommander_configStruct_t* config)
{
	// Initialize the private part of the configuration structure
	config->taskCallbackTaskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;

	// Create a task that will listen for commands from the Multifunk Tool
	XME_CHECK
	(
		XME_HAL_SCHED_INVALID_TASK_HANDLE !=
		(
			config->taskCallbackTaskHandle =
				xme_hal_sched_addTask
				(
					XME_HAL_SCHED_TASK_INITIALLY_SUSPENDED,
					200,
					XME_HAL_SCHED_PRIORITY_NORMAL,
					&xme_adv_mmtCommander_taskCallback,
					config
				)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t 
xme_adv_mmtCommander_activate(xme_adv_mmtCommander_configStruct_t* config)
{
	XME_CHECK
	(
		XME_CORE_STATUS_SUCCESS ==
			xme_hal_sched_setTaskExecutionState(config->taskCallbackTaskHandle, true),
		XME_CORE_STATUS_INTERNAL_ERROR
	);
	
	return XME_CORE_STATUS_SUCCESS;
}

void 
xme_adv_mmtCommander_deactivate(xme_adv_mmtCommander_configStruct_t* config)
{
	xme_hal_sched_setTaskExecutionState(config->taskCallbackTaskHandle, false);
}

void 
xme_adv_mmtCommander_destroy(xme_adv_mmtCommander_configStruct_t* config)
{
	xme_hal_sched_removeTask(config->taskCallbackTaskHandle);
	config->taskCallbackTaskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;
}

void
xme_adv_mmtCommander_taskCallback(void* userData)
{
	xme_adv_mmtCommander_configStruct_t* config; // Configuration struct of mmtCommander, passed in userData
	mmt_cmd_t* cmd; // Pointer to command
	
	// Initialize variables
	config = (xme_adv_mmtCommander_configStruct_t*)userData;

	config->mmt_println("XME: Checking for new command");

	cmd = config->mmt_cmdCallback();
	
	switch (cmd->type)
	{
		case NO_CMD :
			config->mmt_println("XME: Received NO_CMD");
			return;
		break;
		case SHUTDOWN_MODELING_INTERFACE :
			config->mmt_println("XME: Received SHUTDOWN_MODELING_INTERFACE");
			xme_core_shutdown();
			return;
		break;
		case UPDATE_ALL :
			config->mmt_println("XME: Received UPDATE_ALL");
			return;
		break;
		default :
			config->mmt_println("XME: Received unknown command");
			return;
	}
}


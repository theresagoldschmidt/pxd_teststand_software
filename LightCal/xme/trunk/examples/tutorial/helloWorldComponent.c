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
 *         Hello World component.
 *
 * \author
 *         Hauke Stähle <staehle@fortiss.org>
 *         Dominik Sojer <sojer@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "helloWorldComponent.h"
#include "xme/core/core.h"
#include "xme/core/dcc.h"
#include "xme/hal/sharedPtr.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define HELLO_WORLD_TOPIC ((xme_core_topic_t)(XME_CORE_TOPIC_USER+50)) ///< Topic identifier for "Hellow World!" messages.

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
/**
 * \brief  Periodic task function for "Hello World!" message sending.
 *
 * \param  userData User-defined data specified when registering the
 *         task function.
 */
static
void
helloWorldComponent_taskCallback(void* userData)
{
	static const char* message = "Hello World!\n";

	// Retrieve the configuration instance of this component, which is passed in userData
	helloWorldComponent_configStruct_t* config = (helloWorldComponent_configStruct_t*)userData;

	// Send "Hello World!" in a data-centric way using HELLO_WORLD_TOPIC
	xme_core_dcc_sendTopicData(config->publicationHandle, (void*)message, strlen(message)+1);
}

/**
 * \brief  Callback function for "Hello World!" message reception.
 *
 * \param  dataHandle Reference (shared pointer) to the incoming data.
 * \param  userData User-defined data specified when registering the
 *         task function.
 */
static
void
helloWorldComponent_receiveDataCallback(xme_hal_sharedPtr_t dataHandle, void* userData)
{
	// Retrieve and print the message
	const char* message = (const char*)xme_hal_sharedPtr_getPointer(dataHandle);
	printf(message);
}

// Documented in helloWorldComponent.h
xme_core_status_t
helloWorldComponent_create(helloWorldComponent_configStruct_t* config)
{
	config->publicationHandle = XME_CORE_DCC_INVALID_PUBLICATION_HANDLE;
	config->taskHandle = XME_CORE_RESOURCEMANAGER_TASK_INFEASIBLE;

	// Create publication for chat topic
	config->publicationHandle =
		xme_core_dcc_publishTopic
		(
			HELLO_WORLD_TOPIC,
			XME_CORE_MD_EMPTY_META_DATA,
			false,
			NULL
		);

	// Check for errors
	if (XME_CORE_DCC_INVALID_PUBLICATION_HANDLE == config->publicationHandle)
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Create subscription for chat topic and register
	// helloWorldComponent_receiveDataCallback as callback function.
	// Pass the configuration instance in userData.
	config->subscriptionHandle =
		xme_core_dcc_subscribeTopic
		(
			HELLO_WORLD_TOPIC,
			XME_CORE_MD_EMPTY_META_DATA,
			false,
			helloWorldComponent_receiveDataCallback,
			config
		);

	// Check for errors
	if (XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE == config->subscriptionHandle)
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	return XME_CORE_STATUS_SUCCESS;
}

// Documented in helloWorldComponent.h
xme_core_status_t
helloWorldComponent_activate(helloWorldComponent_configStruct_t* config)
{
	// Start a task and use helloWorldComponent_taskCallback as callback
	// function. The task will periodically send "Hello World!".
	config->taskHandle = xme_core_resourceManager_scheduleTask(1000, 2000, 0, helloWorldComponent_taskCallback, config);

	// Check for errors
	if (XME_CORE_RESOURCEMANAGER_TASK_INFEASIBLE == config->taskHandle)
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	return XME_CORE_STATUS_SUCCESS;
}

// Documented in helloWorldComponent.h
void
helloWorldComponent_deactivate(helloWorldComponent_configStruct_t* config)
{
	// Cancel the task that sends "Hello World!"
	xme_core_resourceManager_killTask(config->taskHandle);
	config->taskHandle = XME_CORE_RESOURCEMANAGER_TASK_INFEASIBLE;
}

// Documented in helloWorldComponent.h
void
helloWorldComponent_destroy(helloWorldComponent_configStruct_t* config)
{
	xme_core_dcc_unpublishTopic(config->publicationHandle);
	config->publicationHandle = XME_CORE_DCC_INVALID_PUBLICATION_HANDLE;
}

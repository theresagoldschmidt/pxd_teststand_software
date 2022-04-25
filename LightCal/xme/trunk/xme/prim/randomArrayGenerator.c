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
 *         Random array generator.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Marcel Flesch <fleschmarcel@mytum.de>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/prim/randomArrayGenerator.h"

#include "xme/core/dcc.h"
#include "xme/core/defines.h"

#include "xme/hal/random.h"

/******************************************************************************/
/***   Forward declarations                                                 ***/
/******************************************************************************/
void
xme_prim_randomArrayGenerator_taskCallback(void* userData);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_prim_randomArrayGenerator_create(xme_prim_randomArrayGenerator_configStruct_t* config)
{
	// Initialize configuration structure
	config->publicationHandle = XME_CORE_DCC_INVALID_PUBLICATION_HANDLE;
	config->taskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;

	XME_CHECK
	(
		XME_CORE_DCC_INVALID_PUBLICATION_HANDLE !=
		(
			config->publicationHandle = xme_core_dcc_publishTopic
			(
				XME_CORE_TOPIC_ARRAY_OF_DATA,
				XME_CORE_MD_EMPTY_META_DATA,
				NULL
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	XME_CHECK
	(
	 	XME_HAL_SCHED_INVALID_TASK_HANDLE !=
		(
			config->taskHandle = xme_core_resourceManager_scheduleTask
			(
				XME_HAL_SCHED_TASK_INITIALLY_SUSPENDED,
				config->intervalMs,
				XME_HAL_SCHED_PRIORITY_NORMAL,
				&xme_prim_randomArrayGenerator_taskCallback,
				config
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_prim_randomArrayGenerator_activate(xme_prim_randomArrayGenerator_configStruct_t* config)
{
	XME_CHECK
	(
		XME_CORE_STATUS_SUCCESS == xme_hal_sched_setTaskExecutionState(config->taskHandle, true),
		XME_CORE_STATUS_INTERNAL_ERROR
	);

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_prim_randomArrayGenerator_deactivate(xme_prim_randomArrayGenerator_configStruct_t* config)
{
	xme_core_status_t result = xme_hal_sched_setTaskExecutionState(config->taskHandle, false);
	XME_ASSERT_NORVAL(XME_CORE_STATUS_SUCCESS == result);
}

void
xme_prim_randomArrayGenerator_destroy(xme_prim_randomArrayGenerator_configStruct_t* config)
{
	xme_core_status_t result = xme_hal_sched_removeTask(config->taskHandle);
	XME_ASSERT_NORVAL(XME_CORE_STATUS_SUCCESS == result);
	config->taskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;

	result = xme_core_dcc_unpublishTopic(config->publicationHandle);
	XME_ASSERT_NORVAL(XME_CORE_STATUS_SUCCESS == result);
	config->publicationHandle = XME_CORE_DCC_INVALID_PUBLICATION_HANDLE;
}

void
xme_prim_randomArrayGenerator_taskCallback(void* userData)
{
#if(0)
	xme_prim_randomArrayGenerator_configStruct_t* config = (xme_prim_randomArrayGenerator_configStruct_t*)userData;
	xme_core_topic_arrayData_t data;
	int i;

	// generate random number
	data.number = xme_hal_random_randRange(config->minValue, config->maxValue);

	// generate random string
	for (i=0; i < 9; i++)
	{
		data.str[i] = (char)xme_hal_random_randRange(65, 126);
	}
	data.str[9] = '\0';

	// send Topic
	if ( 0 == (xme_core_dcc_sendTopicData(config->publicationHandle, &data, strlen(data.str)+1 + sizeof(data.number))) )
	{
		printf("\nTOPIC_ARRAY_OF_DATA: %s\t%u\n\n", data.str, data.number);
	}
	else
	{
		printf("An error occured while sending TOPIC_ARRAY_OF_DATA\n");
	}
#endif
}

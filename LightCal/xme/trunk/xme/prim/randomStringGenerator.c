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
 *         Random string generator.
 *
 * \author
 *         Marcel Flesch <fleschmarcel@hotmail.com>
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/prim/randomStringGenerator.h"

#include "xme/core/dcc.h"
#include "xme/core/defines.h"

#include "xme/hal/random.h"

/******************************************************************************/
/***   Forward declarations                                                 ***/
/******************************************************************************/
void
xme_prim_randomStringGenerator_taskCallback(void* userData);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_prim_randomStringGenerator_create(xme_prim_randomStringGenerator_configStruct_t* config)
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
				XME_CORE_TOPIC_STRING,
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
				&xme_prim_randomStringGenerator_taskCallback,
				config
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_prim_randomStringGenerator_activate(xme_prim_randomStringGenerator_configStruct_t* config)
{
	XME_CHECK
	(
		XME_CORE_STATUS_SUCCESS == xme_hal_sched_setTaskExecutionState(config->taskHandle, true),
		XME_CORE_STATUS_INTERNAL_ERROR
	);

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_prim_randomStringGenerator_deactivate(xme_prim_randomStringGenerator_configStruct_t* config)
{
	XME_ASSERT_NORVAL(XME_CORE_STATUS_SUCCESS == xme_hal_sched_setTaskExecutionState(config->taskHandle, false));
}

void
xme_prim_randomStringGenerator_destroy(xme_prim_randomStringGenerator_configStruct_t* config)
{
	XME_ASSERT_NORVAL(XME_CORE_STATUS_SUCCESS == xme_hal_sched_removeTask(config->taskHandle));
	config->taskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;

	XME_ASSERT_NORVAL(XME_CORE_STATUS_SUCCESS == xme_core_dcc_unpublishTopic(config->publicationHandle));
	config->publicationHandle = XME_CORE_DCC_INVALID_PUBLICATION_HANDLE;
}

void
xme_prim_randomStringGenerator_taskCallback(void* userData)
{
	xme_prim_randomStringGenerator_configStruct_t* config = (xme_prim_randomStringGenerator_configStruct_t*)userData;
	xme_core_topic_stringData_t data;
	int i;
	int lenght;

	lenght = xme_hal_random_randRange(1, 10);
	for (i=0; i<(lenght-1); i++)
	{
		data.str[i] = (char)xme_hal_random_randRange(65, 126);
	}
	data.str[lenght-1] = '\0';
#if(0)
	printf("\nSTRING_Topic: %s\n\n", data.str);
	xme_core_dcc_sendTopicData(config->publicationHandle, &data.str, strlen(data.str)+1 );
#endif
}

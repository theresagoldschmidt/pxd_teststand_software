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
 *         Component for automatic LED flashing.
 *
 * \author
 *         Hauke Staehle <staehle@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "comp_led_switcher.h"
#include "xme/hal/dio.h"
#include "xme/core/resourceManager.h"
#include "xme/core/topic.h"

/******************************************************************************/
/***   Forward declarations                                                 ***/
/******************************************************************************/
void
xme_comp_led_switcher_task(void* data);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
static char state = 0;
static xme_core_resourceManager_taskHandle_t task;
xme_core_dcc_publicationHandle_t pubHandle;

xme_core_status_t
xme_comp_led_switcher_create(xme_comp_led_switcher_configStruct_t* config)
{
	// Create publisher for led topic
	pubHandle = xme_core_dcc_publishTopic(
		LED_TOPIC_ID,
		XME_CORE_MD_EMPTY_META_DATA,
		NULL
	);

	if ( pubHandle == XME_CORE_DCC_INVALID_PUBLICATION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Local channel from led_switcher component
	if ( xme_core_routingTable_addLocalSourceRoute(
		LED_CHANNEL,
		xme_core_dcc_getComponentFromPublicationHandle(pubHandle),
		xme_core_dcc_getPortFromPublicationHandle(pubHandle)
	) != XME_CORE_STATUS_SUCCESS )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_comp_led_switcher_activate(xme_comp_led_switcher_configStruct_t* config)
{

	task = xme_core_resourceManager_scheduleTask
	(
		config->startMs,
		config->periodMs,
		0,
		xme_comp_led_switcher_task,
		NULL
	);

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_comp_led_switcher_deactivate(xme_comp_led_switcher_configStruct_t* config)
{
	xme_core_resourceManager_killTask( task );
}

void
xme_comp_led_switcher_destroy(xme_comp_led_switcher_configStruct_t* config)
{
	// TODO
}

void
xme_comp_led_switcher_task(void* data)
{
	xme_hal_dio_led_t newLEDState;

	// Determine what LED state to send
	if ( state )
	{
		newLEDState = XME_HAL_DIO_LED_ON;
	}
	else
	{
		newLEDState = XME_HAL_DIO_LED_OFF;
	}

	// Send
	xme_core_dcc_sendTopicData(pubHandle, (void *)&newLEDState, sizeof(xme_hal_dio_led_t) );

	state = !state;
}



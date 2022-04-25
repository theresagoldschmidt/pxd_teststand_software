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
 *         Component for LED flashing.
 *
 * \author
 *         Hauke Staehle <staehle@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "comp_led.h"
#include "xme/hal/dio.h"
#include "xme/core/resourceManager.h"

/******************************************************************************/
/***   Forward declarations                                                 ***/
/******************************************************************************/
void
xme_comp_led_task(void* data);


/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
static char state = 0;
static xme_core_resourceManager_taskHandle_t task;

xme_core_status_t
xme_comp_led_create(xme_comp_led_configStruct_t* config)
{
	xme_hal_dio_led_init( 
			config->clock,
			config->port,
			config->pin
			);

	xme_hal_dio_led_set(
			config->port,
			config->pin,
			XME_HAL_DIO_LED_ON
			);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_comp_led_activate(xme_comp_led_configStruct_t* config)
{

	task = xme_core_resourceManager_scheduleTask
	(
		100,
		100,
		0,
		xme_comp_led_task,
		(void *)config
	);


	return XME_CORE_STATUS_SUCCESS;
}

void
xme_comp_led_deactivate(xme_comp_led_configStruct_t* config)
{
	xme_core_resourceManager_killTask( task );
}

void
xme_comp_led_destroy(xme_comp_led_configStruct_t* config)
{
}

void
xme_comp_led_task(void* data)
{
	xme_comp_led_configStruct_t *config = (xme_comp_led_configStruct_t *)data;
	if ( state )
	{
		xme_hal_dio_led_set(
				config->port,
				config->pin,
				XME_HAL_DIO_LED_ON
				);
	}
	else
	{
		xme_hal_dio_led_set(
				config->port,
				config->pin,
				XME_HAL_DIO_LED_OFF
				);
	}
	state = !state;
}



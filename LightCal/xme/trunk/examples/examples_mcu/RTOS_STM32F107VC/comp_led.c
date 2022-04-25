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
#include "bsp/pins_custom.h"
#include "comp_led.h"
#include "xme/hal/dio.h"
#include "xme/core/resourceManager.h"

#include "lwip/udp.h"
#include "lwip/netif.h"
#include "lwip/debug.h"
#include "lwip_test.h"
#include "lwip/sockets.h"

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
	//************STM32F107VC******
	xme_hal_dio_led_init(XME_BSP_LED_CUSTOM_1_CLOCK, XME_BSP_LED_CUSTOM_1_PORT, XME_BSP_LED_CUSTOM_1_PIN);
	xme_hal_dio_led_set(XME_BSP_LED_CUSTOM_1_PORT, XME_BSP_LED_CUSTOM_1_PIN, XME_HAL_DIO_LED_ON);

	//************LM3S8962*********
	/*xme_hal_dio_led_init(
			config->clock,
			config->port,
			config->pin
			);

	xme_hal_dio_led_set(
			config->port,
			config->pin,
			XME_HAL_DIO_LED_ON
			);*/
	//*****************************

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_comp_led_activate(xme_comp_led_configStruct_t* config)
{

	task = xme_core_resourceManager_scheduleTask
	(
		100,
		100,
		1,
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

	//**************UDP RAW API****************
	//Raw Udp Initialization
	struct udp_pcb *ptel_pcb = raw_init_udp();

	while (1)
	{
		//Receive packets (PC-->MCU)
		raw_receive_udp(ptel_pcb);

		//If something was received then don't send packets
		if (received != 1)
		{
		 //Send packets (MCU-->PC)
		 raw_send_udp(ptel_pcb, "testing");
		}

		received = 0;
		vTaskDelay(100);
	}
	//*****************************************

	//Led switching example
	/*xme_comp_led_configStruct_t *config = (xme_comp_led_configStruct_t *)data;
	if ( state )
	{
		//************STM32F107VC******
		xme_hal_dio_led_toggle(XME_BSP_LED_6_PORT, XME_BSP_LED_6_PIN);

		//************LM3S8962*********
		//xme_hal_dio_led_set(
		//		config->port,
		//		config->pin,
		//		XME_HAL_DIO_LED_ON
		//		);
	}
	else
	{
		//************STM32F107VC******
		GPIO_Write(XME_BSP_LED_1_8_PORT, XME_HAL_DIO_LED_OFF);

		//************LM3S8962*********
		//xme_hal_dio_led_set(
		//		config->port,
		//		config->pin,
		//		XME_HAL_DIO_LED_OFF
		//		);
	}
	state = !state;*/
}



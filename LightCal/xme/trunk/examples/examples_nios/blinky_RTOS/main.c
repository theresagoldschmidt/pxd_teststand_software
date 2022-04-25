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
 *         Nios II FreeRTOS example project.
 *
 * \author
 *		   Hauke Stähle <staehle@fortiss.org>
 *         Kai Huang <khuang@fortiss.org>
 */

/*
 * main.xme.c
 *
 *  Created on: 2012-1-27
 *      Author: khuang
 */
 
#include <string.h>
//#include "ethernetif.h"
//#include "stm32_eth.h"
//#include "eth_drv.h"
//#include "net_conf.h"

#include "xme/core/component.h"
#include "xme/core/core.h"
#include "xme/core/defines.h"
#include "xme/core/nodeManager.h"
#include "xme/core/interfaceManager.h"
#include "xme/core/broker.h"

#include "bsp/pins_leds.h"
#include "xme/hal/dio_arch.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sched.h"

#include "comp_led.h"
#include "comp_led_switcher.h"


/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define XME_CORE_NODEMANAGER_DEVICE_TYPE ((xme_core_device_type_t)0x0000000000111112)
#define XME_CORE_NODEMANAGER_DEVICE_GUID ((xme_core_device_guid_t)0x0000000000211114)

/******************************************************************************/
/***   Component configurations                                             ***/
/******************************************************************************/
XME_COMPONENT_CONFIG_INSTANCE(xme_core_nodeManager) =
{
	// public
	XME_CORE_NODEMANAGER_DEVICE_TYPE, // deviceType
	XME_CORE_NODEMANAGER_DEVICE_GUID // deviceGuid
};

XME_COMPONENT_CONFIG_INSTANCE(xme_comp_led) =
{
	// public
	0, // clock
	XME_BSP_LED_1_PORT, // port
	XME_BSP_LED_4_PIN // pin
};

/*
XME_COMPONENT_CONFIG_INSTANCE(xme_comp_led_switcher) =
{
	// public
	100, // startMs
	100  // periodMs
};
*/
/******************************************************************************/
/***   Component descriptor                                                 ***/
/******************************************************************************/
XME_COMPONENT_LIST_BEGIN
	XME_COMPONENT_LIST_ITEM(xme_core_nodeManager, 0)
	XME_COMPONENT_LIST_ITEM(xme_comp_led, 0)
//	XME_COMPONENT_LIST_ITEM(xme_comp_led_switcher, 0)
XME_COMPONENT_LIST_END;


int
main(void)
{
	// Initialize ethernet peripheral
	//ETH_driver_init();

	// Initialize the LwIP stack
	//LwIP_Init(1);

    xme_core_init();
    //xme_core_run();

    // Should be a XME-call
    vTaskStartScheduler();

    // Should never reach this point
    for (;;);
}

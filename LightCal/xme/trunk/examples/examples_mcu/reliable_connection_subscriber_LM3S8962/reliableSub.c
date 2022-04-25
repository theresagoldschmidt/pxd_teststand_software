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
 *         This project shows an example on how to connect
 *         to a publisher in a reliable way.
 *
 *         This means that a connection is setup via
 *         TCP to the publsiher. The project uses
 *         freeRTOS and lwIP.
 *
 * \author
 *		   Hauke Stähle <staehle@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/componentList.h"
#include "xme/core/core.h"
#include "xme/core/defines.h"
#include "xme/core/nodeManager.h"
#include "xme/core/interfaceManager.h"
#include "xme/core/broker.h"
#include "xme/core/topic.h"
#include "reliableSubComponent.h"

#include "bsp/pins_leds.h"
#include "xme/hal/dio_arch.h"

#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "enet.h"

#include "utils/lwiplib.h"
#include "xme/hal/console.h"

#include "xme/hal/net.h"

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

/******************************************************************************/
/***   Component descriptor                                                 ***/
/******************************************************************************/
XME_COMPONENT_LIST_BEGIN
	XME_COMPONENT_LIST_ITEM(xme_core_nodeManager, 0)
	XME_COMPONENT_LIST_ITEM_NO_CONFIG(reliableSubComponent)
XME_COMPONENT_LIST_END;

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/

static xTaskHandle handleINITTask;

// This function initializes the XME core system.
//
// As the core system already uses functionality of the
// operating system, it is necessary to first start the
// operating system and to do the initialization of
// the XME core system in an own task.
//
// The program will not work if xme_core_init() is called
// before the operating system is started.
void initXME(void *w)
{
	// Initialize the LwIP.
	// TODO: Should be part of the interface abstraction. See ticket #816
	EnetInit(0);

	xme_core_init();
	xme_hal_console_string("XME initialized.\n");

	while (1)
	{
		vTaskDelete(handleINITTask);
		vTaskDelay(1000);
	}
}

int
main (int argc, char* argv[])
{
	// Set the clocking to run at 50MHz from the PLL.
	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
			SYSCTL_XTAL_8MHZ);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_8);

	// Initialize console
	xme_hal_console_init();

	// The initialization of the XME core is put into an own task.
	xTaskCreate( initXME, "INIT", 256, NULL, XME_HAL_SCHED_PRIORITY_NORMAL+2, &handleINITTask );

	// Start FreeRTOS scheduler
	// TODO: Should be a XME call. See ticket #817
	vTaskStartScheduler();

	return 0;
}

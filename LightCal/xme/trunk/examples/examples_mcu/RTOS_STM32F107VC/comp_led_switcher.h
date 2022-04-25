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

#ifndef XME_COMP_LED_SWITCHER_H
#define XME_COMP_LED_SWITCHER_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/mdl.h"
#include "xme/core/resourceManager.h"
#include "xme/hal/io.h"
#include "xme/core/component.h"
#include "common_ids.h"

#include "bsp/pins_leds.h"
#include "xme/hal/dio_arch.h"

/******************************************************************************/
/***   Model constraints                                                    ***/
/******************************************************************************/
// XME_MDL_RESOURCE_DEPENDENCY()

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
XME_COMPONENT_CONFIG_STRUCT
(
	xme_comp_led_switcher,
	xme_hal_time_interval_t startMs;
	xme_hal_time_interval_t periodMs;
);

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
xme_core_status_t
xme_comp_led_switcher_create(xme_comp_led_switcher_configStruct_t* config);

xme_core_status_t
xme_comp_led_switcher_activate(xme_comp_led_switcher_configStruct_t* config);

void
xme_comp_led_switcher_deactivate(xme_comp_led_switcher_configStruct_t* config);

void
xme_comp_led_switcher_destroy(xme_comp_led_switcher_configStruct_t* config);

#endif // #ifndef XME_COMP_LED_SWITCHER_H

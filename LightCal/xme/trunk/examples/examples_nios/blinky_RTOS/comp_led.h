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

#ifndef XME_COMP_LED_H
#define XME_COMP_LED_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/mdl.h"
#include "xme/hal/io.h"
#include "xme/core/component.h"

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
	xme_comp_led,
	xme_hal_dio_clock_t clock;
	xme_hal_dio_port_t port;
	xme_hal_dio_pin_t pin;
);

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
xme_core_status_t
xme_comp_led_create(xme_comp_led_configStruct_t* config);

xme_core_status_t
xme_comp_led_activate(xme_comp_led_configStruct_t* config);

void
xme_comp_led_deactivate(xme_comp_led_configStruct_t* config);

void
xme_comp_led_destroy(xme_comp_led_configStruct_t* config);

#endif // #ifndef XME_COMP_LED_H

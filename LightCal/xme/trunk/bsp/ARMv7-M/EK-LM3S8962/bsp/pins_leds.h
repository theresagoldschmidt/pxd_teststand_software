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
 *         Board support package: Generic LED pin definitions
 *         (architecture specific part: LM3, Board: EK-LM3S8962).
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 *         Hauke Staehle <staehle@fortiss.org>
 */
 
#ifndef BSP_PINS_LEDS_H
#define BSP_PINS_LEDS_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "hw_types.h"
#include "hw_memmap.h"
#include "driverlib/gpio.h"

/******************************************************************************/
/***   Definitions of LEDs provided by board                                ***/
/******************************************************************************/
/// LED 1: I/O clock
#define XME_BSP_LED_1_CLOCK	0

/// LED 1: I/O port
#define XME_BSP_LED_1_PORT	GPIO_PORTF_BASE

/// LED 1: I/O pin
#define XME_BSP_LED_1_PIN	GPIO_PIN_0

#endif // #ifndef BSP_PINS_LEDS_H

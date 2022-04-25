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
 *         Board support package: Pin definitions reserved for user applications.
 *         (architecture specific part: ARMv7-M, Board: KEIL MCBSTM32C).
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 */
 
#ifndef BSP_PINS_CUSTOM_H
#define BSP_PINS_CUSTOM_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "bsp/pins_buttons.h"
#include "bsp/pins_leds.h"

/******************************************************************************/
/***   Buttons for user applications                                        ***/
/******************************************************************************/
/// Custom button 1: I/O clock
#define XME_BSP_BUTTON_CUSTOM_1_CLOCK	XME_BSP_BUTTON_WAKEUP_CLOCK

/// Custom button 1: I/O port
#define XME_BSP_BUTTON_CUSTOM_1_PORT	XME_BSP_BUTTON_WAKEUP_PORT

/// Custom button 1: I/O pin
#define XME_BSP_BUTTON_CUSTOM_1_PIN		XME_BSP_BUTTON_WAKEUP_PIN


/******************************************************************************/
/***   LEDs for user applications                                           ***/
/******************************************************************************/
/// Custom LED 1: I/O clock
#define XME_BSP_LED_CUSTOM_1_CLOCK		XME_BSP_LED_3_CLOCK

/// Custom LED 1: I/O clock
#define XME_BSP_LED_CUSTOM_1_PORT		XME_BSP_LED_3_PORT

/// Custom LED 1: I/O clock
#define XME_BSP_LED_CUSTOM_1_PIN		XME_BSP_LED_3_PIN


/// Custom LED 2: I/O clock
#define XME_BSP_LED_CUSTOM_2_CLOCK		XME_BSP_LED_4_CLOCK

/// Custom LED 2: I/O clock
#define XME_BSP_LED_CUSTOM_2_PORT		XME_BSP_LED_4_PORT

/// Custom LED 2: I/O clock
#define XME_BSP_LED_CUSTOM_2_PIN		XME_BSP_LED_4_PIN


/// Custom LED 3: I/O clock
#define XME_BSP_LED_CUSTOM_3_CLOCK		XME_BSP_LED_5_CLOCK

/// Custom LED 3: I/O clock
#define XME_BSP_LED_CUSTOM_3_PORT		XME_BSP_LED_5_PORT

/// Custom LED 3: I/O clock
#define XME_BSP_LED_CUSTOM_3_PIN		XME_BSP_LED_5_PIN


/// Custom LED 4: I/O clock
#define XME_BSP_LED_CUSTOM_4_CLOCK		XME_BSP_LED_6_CLOCK

/// Custom LED 4: I/O clock
#define XME_BSP_LED_CUSTOM_4_PORT		XME_BSP_LED_6_PORT

/// Custom LED 4: I/O clock
#define XME_BSP_LED_CUSTOM_4_PIN		XME_BSP_LED_6_PIN

// Define more as needed
#endif // #ifndef BSP_PINS_CUSTOM_H

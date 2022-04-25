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
 *         (architecture specific part: ARMv7-M, Board: Olimex STM32-P107).
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
#warning "Definition of custom button conflicts with bootloader mode button."

/// Custom button 1: I/O clock
#define XME_BSP_BUTTON_CUSTOM_1_CLOCK	XME_BSP_BUTTON_WAKEUP_CLOCK

/// Custom button 1: I/O port
#define XME_BSP_BUTTON_CUSTOM_1_PORT	XME_BSP_BUTTON_WAKEUP_PORT

/// Custom button 1: I/O pin
#define XME_BSP_BUTTON_CUSTOM_1_PIN		XME_BSP_BUTTON_WAKEUP_PIN

/******************************************************************************/
/***   LEDs for user applications                                           ***/
/******************************************************************************/
#warning "Definitions of custom LEDs conflict with bootloader mode and network configuration LEDs."

/// Custom LED 1: I/O clock
#define XME_BSP_LED_CUSTOM_1_CLOCK		XME_BSP_LED_1_CLOCK

/// Custom LED 1: I/O clock
#define XME_BSP_LED_CUSTOM_1_PORT		XME_BSP_LED_1_PORT

/// Custom LED 1: I/O clock
#define XME_BSP_LED_CUSTOM_1_PIN		XME_BSP_LED_1_PIN


/// Custom LED 2: I/O clock
#define XME_BSP_LED_CUSTOM_2_CLOCK		XME_BSP_LED_2_CLOCK

/// Custom LED 2: I/O clock
#define XME_BSP_LED_CUSTOM_2_PORT		XME_BSP_LED_2_PORT

/// Custom LED 2: I/O clock
#define XME_BSP_LED_CUSTOM_2_PIN		XME_BSP_LED_2_PIN

// Define more as needed
#endif // #ifndef BSP_PINS_CUSTOM_H

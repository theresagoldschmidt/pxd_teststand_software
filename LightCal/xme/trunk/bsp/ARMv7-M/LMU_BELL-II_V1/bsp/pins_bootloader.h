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
 *         Board support package: Bootloader specific pin definitions
 *         (architecture specific part: ARMv7-M, Board: KEIL MCBSTM32C).
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 */
 
#ifndef BSP_PINS_BOOTLOADER_H
#define BSP_PINS_BOOTLOADER_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "bsp/pins_buttons.h"
#include "bsp/pins_leds.h"

/******************************************************************************/
/***   Bootloader specific buttons                                          ***/
/******************************************************************************/
/// Button to select bootloader mode: I/O clock
#define XME_BSP_BUTTON_BOOTLOADER_CLOCK	XME_BSP_BUTTON_USER_CLOCK

/// Button to select bootloader mode: I/O port
#define XME_BSP_BUTTON_BOOTLOADER_PORT	XME_BSP_BUTTON_USER_PORT

/// Button to select bootloader mode: I/O pin
#define XME_BSP_BUTTON_BOOTLOADER_PIN	XME_BSP_BUTTON_USER_PIN


/******************************************************************************/
/***   Bootloader specific LEDs                                             ***/
/******************************************************************************/
/// LED to signal bootloader mode: I/O clock
#define XME_BSP_LED_BOOTLOADER_CLOCK		XME_BSP_LED_1_CLOCK

/// LED to signal bootloader mode I/O port
#define XME_BSP_LED_BOOTLOADER_PORT			XME_BSP_LED_1_PORT

/// LED to signal bootloader mode I/O pin
#define XME_BSP_LED_BOOTLOADER_PIN			XME_BSP_LED_1_PIN


#endif // #ifndef BSP_PINS_BOOTLOADER_H

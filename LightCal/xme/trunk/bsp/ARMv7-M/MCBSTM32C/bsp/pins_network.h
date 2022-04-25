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
 *         Board support package: Network specific pin definitions
 *         (architecture specific part: ARMv7-M, Board: KEIL MCBSTM32C).
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 */
 
#ifndef BSP_PINS_NETWORK_H
#define BSP_PINS_NETWORK_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "bsp/pins_buttons.h"
#include "bsp/pins_leds.h"

/******************************************************************************/
/***   Network specific buttons                                             ***/
/******************************************************************************/

/// Button to select static IP address (default: DHCP): I/O clock
#define XME_BSP_BUTTON_STATIC_IP_CLOCK	XME_BSP_BUTTON_TAMPER_CLOCK

/// Button to select static IP address (default: DHCP): I/O port
#define XME_BSP_BUTTON_STATIC_IP_PORT	XME_BSP_BUTTON_TAMPER_PORT

/// Button to select static IP address (default: DHCP): I/O pin
#define XME_BSP_BUTTON_STATIC_IP_PIN	XME_BSP_BUTTON_TAMPER_PIN

/******************************************************************************/
/***   Network specific LEDs                                                ***/
/******************************************************************************/
/// LED to signal IP configuration: I/O clock
#define XME_BSP_LED_IP_CONFIG_CLOCK			XME_BSP_LED_2_CLOCK

/// LED to signal static IP configuration: I/O port
#define XME_BSP_LED_IP_CONFIG_PORT			XME_BSP_LED_2_PORT

/// LED to signal static IP configuration: I/O pin
#define XME_BSP_LED_IP_CONFIG_PIN			XME_BSP_LED_2_PIN

// TODO Define ethernet specific I/O pins here! See ticket #846

#endif // #ifndef BSP_PINS_NETWORK_H

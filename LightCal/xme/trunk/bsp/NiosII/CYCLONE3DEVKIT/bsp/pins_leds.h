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
 *         (architecture specific part: Cyclone III Development Kit: Nios II).
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 */
 
#ifndef BSP_PINS_LEDS_H
#define BSP_PINS_LEDS_H

/******************************************************************************/
/***   Includes                                                           ***/
/****************************************************************************/

#include "system.h"
#include "altera_avalon_pio_regs.h"

/******************************************************************************/
/***   Definitions of LEDs provided by board                                ***/
/******************************************************************************/
/// LED 1: I/O clock
#define XME_BSP_LED_1_CLOCK	0//RCC_APB2Periph_GPIOC

/// LED 1: I/O port
#define XME_BSP_LED_1_PORT	LED_PIO_BASE //GPIOC


/// LED 1: I/O pin
#define XME_BSP_LED_1_PIN	0 
#define XME_BSP_LED_2_PIN	1 
#define XME_BSP_LED_3_PIN	2 
#define XME_BSP_LED_4_PIN	3 
#define XME_BSP_LED_5_PIN	4 
#define XME_BSP_LED_6_PIN	5 
#define XME_BSP_LED_7_PIN	6 
#define XME_BSP_LED_8_PIN	7 

/// LED 2: I/O clock
#define XME_BSP_LED_2_CLOCK	0 //RCC_APB2Periph_GPIOC

/// LED 2: I/O port
#define XME_BSP_LED_2_PORT	0 //GPIOC

/// LED 2: I/O pin
//#define XME_BSP_LED_2_PIN	0 //GPIO_Pin_7

#endif // #ifndef BSP_PINS_LEDS_H

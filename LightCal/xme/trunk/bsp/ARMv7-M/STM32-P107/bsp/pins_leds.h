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
 *         (architecture specific part: ARMv7-M, Board: Olimex STM32-P107).
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 */
 
#ifndef BSP_PINS_LEDS_H
#define BSP_PINS_LEDS_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

/******************************************************************************/
/***   Definitions of LEDs provided by board                                ***/
/******************************************************************************/
/// LED 1: I/O clock
#define XME_BSP_LED_1_CLOCK	RCC_APB2Periph_GPIOC

/// LED 1: I/O port
#define XME_BSP_LED_1_PORT	GPIOC

/// LED 1: I/O pin
#define XME_BSP_LED_1_PIN	GPIO_Pin_6


/// LED 2: I/O clock
#define XME_BSP_LED_2_CLOCK	RCC_APB2Periph_GPIOC

/// LED 2: I/O port
#define XME_BSP_LED_2_PORT	GPIOC

/// LED 2: I/O pin
#define XME_BSP_LED_2_PIN	GPIO_Pin_7

#endif // #ifndef BSP_PINS_LEDS_H

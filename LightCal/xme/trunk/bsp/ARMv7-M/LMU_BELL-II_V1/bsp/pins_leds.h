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
 *         (architecture specific part: ARMv7-M, Board: KEIL MCBSTM32C).
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
#define XME_BSP_LED_1_CLOCK	RCC_APB2Periph_GPIOE

/// LED 1: I/O port
#define XME_BSP_LED_1_PORT	GPIOE

/// LED 1: I/O pin
#define XME_BSP_LED_1_PIN	GPIO_Pin_8


/// LED 2: I/O clock
#define XME_BSP_LED_2_CLOCK	RCC_APB2Periph_GPIOE

/// LED 2: I/O port
#define XME_BSP_LED_2_PORT	GPIOE

/// LED 2: I/O pin
#define XME_BSP_LED_2_PIN	GPIO_Pin_9


/// LED 3: I/O clock
#define XME_BSP_LED_3_CLOCK	RCC_APB2Periph_GPIOE

/// LED 3: I/O port
#define XME_BSP_LED_3_PORT	GPIOE

/// LED 3: I/O pin
#define XME_BSP_LED_3_PIN	GPIO_Pin_10


/// LED 4: I/O clock
#define XME_BSP_LED_4_CLOCK	RCC_APB2Periph_GPIOE

/// LED 4: I/O port
#define XME_BSP_LED_4_PORT	GPIOE

/// LED 4: I/O pin
#define XME_BSP_LED_4_PIN	GPIO_Pin_11


/// LED 5: I/O clock
#define XME_BSP_LED_5_CLOCK	RCC_APB2Periph_GPIOE

/// LED 5: I/O port
#define XME_BSP_LED_5_PORT	GPIOE

/// LED 5: I/O pin
#define XME_BSP_LED_5_PIN	GPIO_Pin_12


/// LED 6: I/O clock
#define XME_BSP_LED_6_CLOCK	RCC_APB2Periph_GPIOE

/// LED 6: I/O port
#define XME_BSP_LED_6_PORT	GPIOE

/// LED 6: I/O pin
#define XME_BSP_LED_6_PIN	GPIO_Pin_13


/// LED 7: I/O clock
#define XME_BSP_LED_7_CLOCK	RCC_APB2Periph_GPIOE

/// LED 7: I/O port
#define XME_BSP_LED_7_PORT	GPIOE

/// LED 7: I/O pin
#define XME_BSP_LED_7_PIN	GPIO_Pin_14


/// LED 8: I/O clock
#define XME_BSP_LED_8_CLOCK	RCC_APB2Periph_GPIOE

/// LED 8: I/O port
#define XME_BSP_LED_8_PORT	GPIOE

/// LED 8: I/O pin
#define XME_BSP_LED_8_PIN	GPIO_Pin_15


/// LED 1-8: I/O clock
#define XME_BSP_LED_1_8_CLOCK	RCC_APB2Periph_GPIOE

/// LED 1-8: I/O port
#define XME_BSP_LED_1_8_PORT	GPIOE

/// LED 1-8: I/O pin
#define XME_BSP_LED_1_8_PIN		GPIO_Pin_All

#endif // #ifndef BSP_PINS_LEDS_H

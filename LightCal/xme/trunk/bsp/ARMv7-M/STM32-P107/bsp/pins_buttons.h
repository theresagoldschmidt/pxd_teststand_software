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
 *         Board support package: Generic button pin definitions
 *         (architecture specific part: ARMv7-M, Board: Olimex STM32-P107).
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 */
 
#ifndef BSP_PINS_BUTTONS_H
#define BSP_PINS_BUTTONS_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"

/******************************************************************************/
/***   Definitions of buttons provided by board                             ***/
/******************************************************************************/
// Tamper button
///Tamper button: I/O clock
#define XME_BSP_BUTTON_TAMPER_CLOCK	RCC_APB2Periph_GPIOC

///Tamper button: I/O port
#define XME_BSP_BUTTON_TAMPER_PORT	GPIOC

///Tamper button: I/O pin
#define XME_BSP_BUTTON_TAMPER_PIN	GPIO_Pin_13


// Wakeup botton
///Wake button: I/O clock
#define XME_BSP_BUTTON_WAKEUP_CLOCK	RCC_APB2Periph_GPIOA

///Wake button: I/O port
#define XME_BSP_BUTTON_WAKEUP_PORT	GPIOA

///Wake button: I/O pin
#define XME_BSP_BUTTON_WAKEUP_PIN	GPIO_Pin_0

#endif // #ifndef BSP_PINS_BUTTONS_H

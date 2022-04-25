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
 *         Board support package (architecture specific part: ARMv7-M).
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/dio_arch.h"
#include "stm32f10x_rcc.h"

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/

void
xme_hal_dio_button_init(xme_hal_dio_clock_t clock, xme_hal_dio_port_t port, xme_hal_dio_pin_t pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(clock | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(port, &GPIO_InitStructure);
}

xme_hal_dio_button_t
xme_hal_dio_read_button(xme_hal_dio_port_t port, xme_hal_dio_pin_t pin)
{
	return GPIO_ReadInputDataBit(port, pin);
}

void
xme_hal_dio_led_init(xme_hal_dio_clock_t clock, xme_hal_dio_port_t port, xme_hal_dio_pin_t pin)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(clock | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(port, &GPIO_InitStructure);
}

void
xme_hal_dio_led_set(xme_hal_dio_port_t port, xme_hal_dio_pin_t pin, xme_hal_dio_led_t led_state)
{
	GPIO_WriteBit(port, pin, led_state);
}

void
xme_hal_dio_led_toggle(xme_hal_dio_port_t port, xme_hal_dio_pin_t pin)
{
	GPIO_WriteBit(port, pin, !GPIO_ReadInputDataBit(port, pin));
}

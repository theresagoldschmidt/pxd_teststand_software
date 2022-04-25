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
 */

/**
 * \file
 *         Blinky example project for STM32 board.
 *
 * \author
 *		   Simon Barner <barner@fortiss.org>


/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/dio.h"
#include "bsp/pins_custom.h"


/******************************************************************************/
/***   Implementation                                                      ***/
/******************************************************************************/
void SysTickStart(uint32_t Tick)
{
	RCC_ClocksTypeDef Clocks;
	volatile uint32_t dummy;

	RCC_GetClocksFreq(&Clocks);

	dummy = SysTick->CTRL;
	SysTick->LOAD = (Clocks.HCLK_Frequency/8)/Tick;

	SysTick->CTRL = 1;
}

void SysTickStop(void)
{
	SysTick->CTRL = 0;
}

int main (void) {
	xme_hal_dio_led_init(XME_BSP_LED_CUSTOM_1_CLOCK, XME_BSP_LED_CUSTOM_1_PORT, XME_BSP_LED_CUSTOM_1_PIN);
	xme_hal_dio_led_init(XME_BSP_LED_CUSTOM_2_CLOCK, XME_BSP_LED_CUSTOM_2_PORT, XME_BSP_LED_CUSTOM_2_PIN);

	xme_hal_dio_led_set(XME_BSP_LED_CUSTOM_1_PORT, XME_BSP_LED_CUSTOM_1_PIN, XME_HAL_DIO_LED_ON);
	xme_hal_dio_led_set(XME_BSP_LED_CUSTOM_2_PORT, XME_BSP_LED_CUSTOM_2_PIN, XME_HAL_DIO_LED_OFF);

	SysTickStart(1);

	do
	{
		if( SysTick->CTRL & (1<<16))
		{
			xme_hal_dio_led_toggle(XME_BSP_LED_CUSTOM_1_PORT, XME_BSP_LED_CUSTOM_1_PIN);
			xme_hal_dio_led_toggle(XME_BSP_LED_CUSTOM_2_PORT, XME_BSP_LED_CUSTOM_2_PIN);
		}

	}
	while(1);

	return 0;
}

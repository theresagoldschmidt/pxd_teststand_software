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
 *         Blinky example project for LM3S8962 board.
 *
 * \author
 *		   Hauke Stähle <staehle@fortiss.org>
 */

#include "bsp/pins_leds.h"
#include "xme/hal/dio_arch.h"

// NOTE: Required to fix build
void lwIPEthernetIntHandler (void)
{
}

void SysTickIntHandler (void)
{
}

int
main(void)
{
    volatile unsigned long ulLoop;

    // Enable the GPIO pin for the LED (PF0).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    xme_hal_dio_led_init( 0, XME_BSP_LED_1_PORT, XME_BSP_LED_1_PIN );

    //
    // Loop forever.
    //
    while(1)
    {
        //
        // Turn on the LED.
        //
    	xme_hal_dio_led_set(XME_BSP_LED_1_PORT, XME_BSP_LED_1_PIN, XME_HAL_DIO_LED_ON);

        //
        // Delay for a bit.
        //
        for(ulLoop = 0; ulLoop < 2000000; ulLoop++)
        {
        }

        //
        // Turn off the LED.
        //
        xme_hal_dio_led_set(XME_BSP_LED_1_PORT, XME_BSP_LED_1_PIN, XME_HAL_DIO_LED_OFF);

        //
        // Delay for a bit.
        //
        for(ulLoop = 0; ulLoop < 2000000; ulLoop++)
        {
        }
    }
}

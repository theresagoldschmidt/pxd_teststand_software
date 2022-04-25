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

/*
 * TODO: (ticket #851 and #852)
 * - Align this with the generic definitions in xme/hal/dio.h. See ticket #851
 * - Add plain GPIO access functions in addition to buttons and LEDs. See ticket #852
 */

#ifndef XME_HAL_DIO_ARCH_H
#define XME_HAL_DIO_ARCH_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/

/**
 * \typedef xme_hal_dio_led_t
 *
 * \brief LED states.
 */
typedef enum
{
	XME_HAL_DIO_LED_OFF = 0, ///< LED is off
	XME_HAL_DIO_LED_ON = 1,  ///< LED is on

}
xme_hal_dio_led_t;

/**
 * \typedef xme_hal_dio_button_t
 *
 * \brief Button states.
 */
typedef enum
{
	XME_HAL_DIO_BUTTON_PRESSED = 0,  ///< Button is pressed
	XME_HAL_DIO_BUTTON_RELEASED = 1, ///< Button is released

}
xme_hal_dio_button_t;

/**
 * \typedef xme_hal_dio_clock_t
 *
 * \brief I/O clock.
 */
typedef int xme_hal_dio_clock_t;

/**
 * \typedef xme_hal_dio_port_t
 *
 * \brief I/O port.
 */
typedef unsigned long xme_hal_dio_port_t;

/**
 * \typedef xme_hal_dio_pin_t
 *
 * \brief I/O pin.
 */
typedef unsigned long xme_hal_dio_pin_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
void
xme_hal_dio_button_init(xme_hal_dio_clock_t clock, xme_hal_dio_port_t port, xme_hal_dio_pin_t pin);

// TODO: Later. See ticket #853
//void
//xme_hal_dio_button_fini(xme_hal_dio_clock_t clock, xme_hal_dio_port_t port, xme_hal_dio_pin_t pin);

xme_hal_dio_button_t
xme_hal_dio_read_button(xme_hal_dio_port_t port, xme_hal_dio_pin_t pin);

void
xme_hal_dio_led_init(xme_hal_dio_clock_t clock, xme_hal_dio_port_t port, xme_hal_dio_pin_t pin);

/// TODO: Later. See ticket #853
//void
//xme_hal_dio_led_fini(xme_hal_dio_clock_t clock, xme_hal_dio_port_t port, xme_hal_dio_pin_t pin);

void
xme_hal_dio_led_set(xme_hal_dio_port_t port, xme_hal_dio_pin_t pin, xme_hal_dio_led_t led_state);

void
xme_hal_dio_led_toggle(xme_hal_dio_port_t port, xme_hal_dio_pin_t pin);

#endif // #ifndef XME_HAL_DIO_ARCH_H


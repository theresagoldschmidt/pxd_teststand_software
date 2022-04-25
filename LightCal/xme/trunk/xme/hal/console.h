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
 *         Console abstraction.
 *
 * \author
 *         Hauke Staehle <staehle@fortiss.org>
 */

#ifndef XME_HAL_CONSOLE_H
#define XME_HAL_CONSOLE_H

/**
 * \defgroup hal_console Console
 *
 * \brief  Universal console abstraction.
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/core.h"
#include <stdint.h>

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/

/**
 * \ingroup hal_console
 *
 * \typedef xme_hal_console_base_t
 *
 * \brief Display type for number.
 *
 * \see   xme_hal_console_number()
 */
typedef enum
{
	XME_HAL_CONSOLE_BASE_INVALID = 0,
	XME_HAL_CONSOLE_BASE2 = 2, // Display as binary
	XME_HAL_CONSOLE_BASE10 = 10, // Display as decimal
	XME_HAL_CONSOLE_BASE16 = 16 // Display as hexadecimal
}
xme_hal_console_base_t;

/**
 * \ingroup hal_console
 *
 * \typedef xme_hal_console_num_t
 *
 * \brief Type of number to show on the console.
 *
 * \see   xme_hal_console_number()
 */
typedef enum
{
	XME_HAL_CONSOLE_NUM_INVALID = 0,
	XME_HAL_CONSOLE_NUM_UINT8 = 1,
	XME_HAL_CONSOLE_NUM_INT8 = 2,
	XME_HAL_CONSOLE_NUM_UINT16 = 3,
	XME_HAL_CONSOLE_NUM_INT16 = 4,
	XME_HAL_CONSOLE_NUM_UINT32 = 5,
	XME_HAL_CONSOLE_NUM_INT32 = 6
}
xme_hal_console_num_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

/**
 * \ingroup hal_console
 *
 * \brief Initializes the console.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the console has been properly
 *            initialized.
 *          - XME_CORE_STATUS_OUT_OF_RESOURCES if console initialization
 *            failed.
 */
xme_core_status_t
xme_hal_console_init(void);

/**
 * \ingroup hal_console
 *
 * \brief  Frees resources occupied by the console abstraction.
 */
void
xme_hal_console_fini(void);

/**
 * \ingroup hal_console
 *
 * \brief Reset console (clear screen).
 */
void
xme_hal_console_reset(void);

/**
 * \ingroup hal_console
 *
 * \brief Writes a string to console.
 *
 * \param s String to write. Must be null-terminated.
 */
void
xme_hal_console_string(char *s);

/**
 * \ingroup hal_console
 *
 * \brief Writes a string to console with maximum size.
 *
 * \param s String to write.
 * \param size Maximum size of string.
 */
void
xme_hal_console_stringn(char *s, uint16_t size);

/**
 * \ingroup hal_console
 *
 * \brief Writes a single character to console.
 *
 * \param c Character to write.
 */
void
xme_hal_console_char(char c);

/**
 * \ingroup hal_console
 *
 * \brief Writes a number to the console.
 *
 * \param nr Pointer to number.
 * \param nt The type of the number in memory.
 * \param bt Display type: binary, decimal or hexadecimal.
 */
void
xme_hal_console_number(
		void *nr,
		xme_hal_console_num_t nt,
		xme_hal_console_base_t bt
		);

/**
 * \ingroup hal_console
 *
 * \brief Set cursor position.
 *
 * \param x X position.
 * \param y Y position.
 */
xme_hal_console_setPos(
		uint8_t x,
		uint8_t y
);

#define xme_hal_console_uint8_b( number ) \
	xme_hal_console_number( (void *)&number, XME_HAL_CONSOLE_NUM_UINT8, XME_HAL_CONSOLE_BASE2 )

#define xme_hal_console_uint8_d( number ) \
	xme_hal_console_number( (void *)&number, XME_HAL_CONSOLE_NUM_UINT8, XME_HAL_CONSOLE_BASE10 )

#define xme_hal_console_uint8_x( number ) \
	xme_hal_console_number( (void *)&number, XME_HAL_CONSOLE_NUM_UINT8, XME_HAL_CONSOLE_BASE16 )

#define xme_hal_console_int8_b( number ) \
	xme_hal_console_number( (void *)&number, XME_HAL_CONSOLE_NUM_INT8, XME_HAL_CONSOLE_BASE2 )

#define xme_hal_console_int8_d( number ) \
	xme_hal_console_number( (void *)&number, XME_HAL_CONSOLE_NUM_INT8, XME_HAL_CONSOLE_BASE10 )

#define xme_hal_console_int8_x( number ) \
	xme_hal_console_number( (void *)&number, XME_HAL_CONSOLE_NUM_INT8, XME_HAL_CONSOLE_BASE16 )


#define xme_hal_console_uint16_b( number ) \
	xme_hal_console_number( (void *)&number, XME_HAL_CONSOLE_NUM_UINT16, XME_HAL_CONSOLE_BASE2 )

#define xme_hal_console_uint16_d( number ) \
	xme_hal_console_number( (void *)&number, XME_HAL_CONSOLE_NUM_UINT16, XME_HAL_CONSOLE_BASE10 )

#define xme_hal_console_uint16_x( number ) \
	xme_hal_console_number( (void *)&number, XME_HAL_CONSOLE_NUM_UINT16, XME_HAL_CONSOLE_BASE16 )

#define xme_hal_console_int16_b( number ) \
	xme_hal_console_number( (void *)&number, XME_HAL_CONSOLE_NUM_INT16, XME_HAL_CONSOLE_BASE2 )

#define xme_hal_console_int16_d( number ) \
	xme_hal_console_number( (void *)&number, XME_HAL_CONSOLE_NUM_INT16, XME_HAL_CONSOLE_BASE10 )

#define xme_hal_console_int16_x( number ) \
	xme_hal_console_number( (void *)&number, XME_HAL_CONSOLE_NUM_INT16, XME_HAL_CONSOLE_BASE16 )


#define xme_hal_console_uint32_b( number ) \
	xme_hal_console_number( (void *)&number, XME_HAL_CONSOLE_NUM_UINT32, XME_HAL_CONSOLE_BASE2 )

#define xme_hal_console_uint32_d( number ) \
	xme_hal_console_number( (void *)&number, XME_HAL_CONSOLE_NUM_UINT32, XME_HAL_CONSOLE_BASE10 )

#define xme_hal_console_uint32_x( number ) \
	xme_hal_console_number( (void *)&number, XME_HAL_CONSOLE_NUM_UINT32, XME_HAL_CONSOLE_BASE16 )

#define xme_hal_console_int32_b( number ) \
	xme_hal_console_number( (void *)&number, XME_HAL_CONSOLE_NUM_INT32, XME_HAL_CONSOLE_BASE2 )

#define xme_hal_console_int32_d( number ) \
	xme_hal_console_number( (void *)&number, XME_HAL_CONSOLE_NUM_INT32, XME_HAL_CONSOLE_BASE10 )

#define xme_hal_console_int32_x( number ) \
	xme_hal_console_number( (void *)&number, XME_HAL_CONSOLE_NUM_INT32, XME_HAL_CONSOLE_BASE16 )

#endif /* #ifndef XME_HAL_CONSOLE_H */

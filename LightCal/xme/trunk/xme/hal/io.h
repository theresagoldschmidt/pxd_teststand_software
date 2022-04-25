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
 *         I/O abstraction.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_HAL_IO_H
#define XME_HAL_IO_H

/**
 * \defgroup hal_io General I/O
 *
 * \brief General I/O abstraction.
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
/**
 * \def _BV
 *
 * \brief  Converts a bit number into a byte value.
 */
#ifndef _BV
#	define _BV(bit) (1 << (bit))
#endif // #ifndef _BV

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_hal_io_port_t
 *
 * \brief  Port (pin group) identifier.
 */
typedef uint8_t xme_hal_io_port_t;

/**
 * \typedef xme_hal_io_pin_t
 *
 * \brief  Pin identifier.
 */
typedef uint8_t xme_hal_io_pin_t;

/**
 * \typedef xme_hal_io_pin_t
 *
 * \brief  Pin descriptor.
 */
typedef struct
{
	xme_hal_io_port_t port; ///< Port (pin group) identifier.
	xme_hal_io_pin_t pin; ///< Pin identifier.
}
xme_hal_io_portPin_t;

#endif // #ifndef XME_HAL_IO_H

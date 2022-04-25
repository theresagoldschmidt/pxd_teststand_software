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
 *         Digital I/O abstraction (platform specific part: Posix).
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_HAL_DIO_ARCH_H
#define XME_HAL_DIO_ARCH_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/dio.h"

#include "xme/core/mdl.h"

#include <stdint.h>

/******************************************************************************/
/***   Model constraints                                                    ***/
/******************************************************************************/
XME_MDL_RESOURCE_PROVIDE("dio", "port", "A", "Virtual Port A (8 bit)")
XME_MDL_RESOURCE_PROVIDE("dio", "portpin", "A0", "Virtual Pin A.0")
XME_MDL_RESOURCE_PROVIDE("dio", "portpin", "A1", "Virtual Pin A.1")
XME_MDL_RESOURCE_PROVIDE("dio", "portpin", "A2", "Virtual Pin A.2")
XME_MDL_RESOURCE_PROVIDE("dio", "portpin", "A3", "Virtual Pin A.3")
XME_MDL_RESOURCE_PROVIDE("dio", "portpin", "A4", "Virtual Pin A.4")
XME_MDL_RESOURCE_PROVIDE("dio", "portpin", "A5", "Virtual Pin A.5")
XME_MDL_RESOURCE_PROVIDE("dio", "portpin", "A6", "Virtual Pin A.6")
XME_MDL_RESOURCE_PROVIDE("dio", "portpin", "A7", "Virtual Pin A.7")

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define XME_HAL_DIO_NUM_PORTS 1
#define XME_HAL_DIO_PORT_A 0
#define XME_HAL_DIO_PIN_A0 0
#define XME_HAL_DIO_PIN_A1 1
#define XME_HAL_DIO_PIN_A2 2
#define XME_HAL_DIO_PIN_A3 3
#define XME_HAL_DIO_PIN_A4 4
#define XME_HAL_DIO_PIN_A5 5
#define XME_HAL_DIO_PIN_A6 6
#define XME_HAL_DIO_PIN_A7 7

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_hal_dio_portConfiguration_t
 *
 * \brief  Digital I/O port configuration.
 *         Every bit represents a pin of the port. A zero bit indicates that
 *         the respective pin is configured for input. A one bit indicates
 *         that the respective pin is configured for output.
 */
//typedef uint8_t xme_hal_dio_portConfiguration_t;			//why 2 times? dio.h has the same lines. gives redefinition error

/**
 * \typedef xme_hal_dio_portState_t
 *
 * \brief  Digital I/O port state.
 *         Every bit represents a pin of the port. A zero bit indicates that
 *         the respective pin is currently low. A one bit indicates that the
 *         respective pin is high.
 */
//typedef uint8_t xme_hal_dio_portState_t;

#endif // #ifndef XME_HAL_DIO_ARCH_H

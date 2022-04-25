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
 *         Energy management abstraction.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_HAL_ENERGY_H
#define XME_HAL_ENERGY_H

/**
 * \defgroup hal_energy Energy management
 *
 * \brief Energy management abstraction.
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Sends the device to sleep mode. Sleep mode can only be left via
 *         external or internal events (e.g., interrupts).
 */
void
xme_hal_energy_sleep();

#endif // #ifndef XME_HAL_ENERGY_H

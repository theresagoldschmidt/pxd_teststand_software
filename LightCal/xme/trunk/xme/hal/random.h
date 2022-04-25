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
 *         Random number generator abstraction.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_HAL_RANDOM_H
#define XME_HAL_RANDOM_H

/**
 * \defgroup hal_random Random number generator
 *
 * \brief Random number generator abstraction.
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdint.h>

#include "xme/core/core.h"

#include "xme/hal/random_arch.h"

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \ingroup hal_random
 *
 * \brief  Initializes the random number generator.
 */
xme_core_status_t
xme_hal_random_init();

/**
 * \ingroup hal_random
 *
 * \brief  Frees resources occupied by the random number generator.
 */
void
xme_hal_random_fini();

/**
 * \ingroup hal_random
 *
 * \brief  Registers the calling thread for use of the random number generator
 *         abstraction.
 *
 *         Every thread that will use the random number generator
 *         has to be registered with a call to this function.
 *         This is necessary, because on some platforms (e.g., Windows),
 *         the random number seed is thread-local.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the calling thread has been
 *            successfully registered.
 *          - XME_CORE_STATUS_OUT_OF_RESOURCES if thread registration has
 *            failed.
 */
xme_core_status_t
xme_hal_random_registerThread();

/**
 * \ingroup hal_random
 *
 * \brief  Deregisters the calling thread from use of the random number
 *         generator abstraction.
 */
void
xme_hal_random_deregisterThread();

/**
 * \ingroup hal_random
 *
 * \brief  Generates a (pseudo-)random number in range 0 to
 *         XME_HAL_RANDOM_RAND_MAX.
 *
 * \return (Pseudo-)random number.
 */
uint16_t
xme_hal_random_rand();

/**
 * \ingroup hal_random
 *
 * \brief  Generates a (pseudo-)random number in the given range
 *         (including min and max).
 *
 *         For max < min, the return value is undefined.
 *
 * \param  min Minimum value.
 * \param  max Maximum value.
 * \return (Pseudo-)random number in the given range.
 */
uint16_t
xme_hal_random_randRange(uint16_t min, uint16_t max);

#endif // #ifndef XME_HAL_RANDOM_H

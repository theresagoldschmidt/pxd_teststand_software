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
 *         Random number generator abstraction (architecture specific part:
 *         generic OS based implementation).
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/random.h"

#include <time.h>

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_hal_random_init()
{
	// Initialize random seed
	srand((unsigned int)time(NULL));

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_hal_random_fini()
{
	// Nothing to do
}

xme_core_status_t
xme_hal_random_registerThread()
{
	// Initialize random seed
	srand((unsigned int)time(NULL));

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_hal_random_deregisterThread()
{
	// Nothing to do
}

uint16_t
xme_hal_random_rand()
{
	return rand();
}

uint16_t
xme_hal_random_randRange(uint16_t min, uint16_t max)
{
	uint16_t r;

	XME_ASSERT_RVAL(max >= min, max);
	XME_CHECK(max != min, max);

#if XME_HAL_RANDOM_RAND_MAX < 0xFFFF
	// According to documentation, "RAND_MAX is granted to be at least 32767".
	// But max-min could be larger than XME_HAL_RANDOM_RAND_MAX, causing problems.
	// For example, if max is 65535, min is 0 and XME_HAL_RANDOM_RAND_MAX is 0x7FFF,
	// then this function would only return even numbers, which is obviously bad.
	// Hence the following "workaround" is used in case the most significant bit is
	// not affected by rand().
	r = rand() | (rand() << 15);
#else
	r = rand();
#endif

	return min + (uint16_t)( (max-min) * (((double)r/0xFFFF)) + 0.5);
}

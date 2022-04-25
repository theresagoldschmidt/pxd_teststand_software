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
 *         generic "embedded" implementation).
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
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
	// TODO: See ticket #828
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
	// TODO: See ticket #829
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
	// TODO: See ticket #830
	return 0;
}

uint16_t
xme_hal_random_randRange(uint16_t min, uint16_t max)
{
	XME_ASSERT_RVAL(max >= min, max);

	// TODO: See ticket #831
	return 0;
}

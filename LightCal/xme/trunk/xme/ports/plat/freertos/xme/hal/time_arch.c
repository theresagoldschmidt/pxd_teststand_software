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
 *         Time abstraction (architecture specific part: FreeRTOS
 *         implementation).
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/time.h"

#include "xme/core/defines.h"

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_hal_time_handle_t
xme_hal_time_getCurrentTime(void)
{
	// TODO: See ticket #824
	return (xme_hal_time_handle_t)0;
}

xme_hal_time_interval_t
xme_hal_time_getInterval
(
	xme_hal_time_handle_t* startTime,
	bool reset
)
{
	xme_hal_time_interval_t interval;

	// TODO: See ticket #825

	return 0;
}

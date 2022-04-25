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
 *         Time abstraction (architecture specific part: Windows
 *         implementation).
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/time.h"

#include "xme/core/defines.h"

#include <Windows.h>

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_hal_time_handle_t
xme_hal_time_getCurrentTime(void)
{
	return (xme_hal_time_handle_t)GetTickCount();
}

xme_hal_time_interval_t
xme_hal_time_getInterval
(
	xme_hal_time_handle_t* startTime,
	bool reset
)
{
	xme_hal_time_interval_t interval;
	xme_hal_time_handle_t now;

	XME_ASSERT(NULL != startTime);

	now = xme_hal_time_getCurrentTime();
	interval = xme_hal_time_getIntervalBetween(*startTime, now);

	if (reset)
	{
		*startTime = now;
	}

	return interval;
}

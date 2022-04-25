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
 *         Time abstraction.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_HAL_TIME_H
#define XME_HAL_TIME_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdbool.h>
#include <stdint.h>

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_hal_time_time_t
 *
 * \brief  Handle for a point in time.
 */
typedef uint32_t xme_hal_time_handle_t;

/**
 * \typedef xme_hal_time_interval_t
 *
 * \brief  A time interval in milliseconds.
 */
typedef uint32_t xme_hal_time_interval_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Retrieves a handle for the current point in time.
 *
 * \note   The returned time handle must be destroyed in a call to
 *         xme_hal_time_destroyHandle() when it is not used any more.
 *
 * \return Returns a handle for the current point in time.
 */
xme_hal_time_handle_t
xme_hal_time_getCurrentTime();

/**
 * \brief  Compares two points in time.
 *
 * \param  startTime Start time.
 * \param  stopTime Stop time.
 *
 * \return Returns a value smaller than zero if time1 corresponds to an
 *         earlier point in time than time2. Returns a value larger than
 *         zero if time1 corresponds to a later point in time than time2.
 *         Returns zero if time1 and time2 correspond to the same point
 *         in time (with respect to the granularity of the time system).
 */
char
xme_hal_time_compareTime
(
	xme_hal_time_handle_t time1,
	xme_hal_time_handle_t time2
);

/**
 * \brief  Returns the time interval in milliseconds that has passed between
 *         the given start and stop time (represented by time handles).
 *
 *         In case start time corresponds to a later point in time than
 *         stopTime, the result may be undefined. If you are not sure about
 *         the order, invoke xme_hal_time_compareTime() first.
 *
 * \param  startTime Start time.
 * \param  stopTime Stop time.
 * \param
 */
xme_hal_time_interval_t
xme_hal_time_getIntervalBetween
(
	xme_hal_time_handle_t startTime,
	xme_hal_time_handle_t stopTime
);

/**
 * \brief  Returns the time interval in milliseconds that has passed between
 *         the given start time (represented by a time handle) and the
 *         current point in time.
 *
 * \param  startTime Address of a time handle that corresponds to the point
 *         in time that marks the start of the time interval to measure.
 * \param  reset If set to true, the function will replace the value in
 *         startTime with the current time such that subsequent calls to
 *         xme_hal_time_getInterval() will yield the time interval between
 *         the most recent execution of xme_hal_time_getInterval() and the
 *         current time. If set to true, startTime will not be modified.
 *
 * \return Returns the number of milliseconds that have passed between the
 *         point in time specified by startTime and now.
 */
xme_hal_time_interval_t
xme_hal_time_getInterval
(
	xme_hal_time_handle_t* startTime,
	bool reset
);

/**
 * \brief  Frees all resources associated with the given time handle.
 *
 * \param  timeHandle Time handle to destroy.
 */
void
xme_hal_time_destroyHandle
(
	xme_hal_time_handle_t timeHandle
);

/******************************************************************************/
/***   Platform-specific includes                                           ***/
/******************************************************************************/
#include "xme/hal/time_arch.h"

#endif // #ifndef XME_HAL_TIME_H

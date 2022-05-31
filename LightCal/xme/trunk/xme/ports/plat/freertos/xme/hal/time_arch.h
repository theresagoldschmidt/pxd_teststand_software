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

#ifndef XME_HAL_TIME_ARCH_H
#define XME_HAL_TIME_ARCH_H

#ifndef XME_HAL_TIME_H
	#error This architecture-specific header file should not be included directly. Include the generic header file (usually without "_arch" suffix) instead.
#endif // #ifndef XME_HAL_TIME_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define xme_hal_time_compareTime(time1, time2) ((time1) < (time2)) ? (char)-1 : (((time1) > (time2)) ? (char)1 : (char)0)
#define xme_hal_time_getIntervalBetween(startTime, stopTime) (xme_hal_time_interval_t)((stopTime)-(startTime))
#define xme_hal_time_destroyHandle(timeHandle) /* nop */

#endif // #ifndef XME_HAL_TIME_ARCH_H
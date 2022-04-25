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
 *         Public sleep API.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_CORE_SLEEP_H
#define XME_CORE_SLEEP_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/time.h"

#include <stdint.h>

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Halts execution of the current task for the specified amount of
 *         time.
 *
 * \param  sleepMs Time in milliseconds to defer execution of the current
 *         task.
 */
void
xme_hal_sleep_sleep(xme_hal_time_interval_t sleepMs);

#endif // #ifndef XME_CORE_SLEEP_H

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
 *         Sleep abstraction (platform specific part: Posix).
 *
 * \author
 *	       Deniz Kabakci <deniz.kabakci@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/sleep.h"

#include <unistd.h>

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
void
xme_hal_sleep_sleep(xme_hal_time_interval_t sleepMs)
{	
	usleep(1000*sleepMs);
}

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
 *         Energy management abstraction (platform specific part: Windows).
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/energy.h"

#include <windows.h>

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
void
xme_hal_energy_sleep()
{
	// TODO (See ticket #805): If only SwitchToThread() is used here, this will result in the
	//                         system spinning idle, since the resource manager has not yet been
	//                         implemented, which causes a high CPU usage. This is why we let
	//                         the main thread sleep here for the time being.
	Sleep(1000);

	SwitchToThread();
}

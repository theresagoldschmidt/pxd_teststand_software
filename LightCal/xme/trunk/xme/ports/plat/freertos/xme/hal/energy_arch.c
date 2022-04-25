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
 *         Energy management abstraction (architecture specific part: FreeRTOS).
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/energy.h"

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
void
xme_hal_energy_sleep()
{
	// TODO: See ticket #805
	// SwitchToThread();
}

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
 *         Random number generator.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_PRIM_RANDOMNUMBERGENERATOR_H
#define XME_PRIM_RANDOMNUMBERGENERATOR_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/dcc.h"
#include "xme/core/resourceManager.h"

#include "xme/hal/time.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_prim_randomNumberGenerator_configStruct_t
 *
 * \brief  Random number generator configuration structure.
 */
typedef struct
{
	// public
	int minValue;
	int maxValue;
	xme_hal_time_interval_t intervalMs;
	// private
	xme_core_dcc_publicationHandle_t publicationHandle;
	xme_core_resourceManager_taskHandle_t taskHandle;
}
xme_prim_randomNumberGenerator_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates a random number generator component.
 */
xme_core_status_t
xme_prim_randomNumberGenerator_create(xme_prim_randomNumberGenerator_configStruct_t* config);

/**
 * \brief  Activates a random number generator component.
 */
xme_core_status_t
xme_prim_randomNumberGenerator_activate(xme_prim_randomNumberGenerator_configStruct_t* config);

/**
 * \brief  Deactivates a random number generator component.
 */
void
xme_prim_randomNumberGenerator_deactivate(xme_prim_randomNumberGenerator_configStruct_t* config);

/**
 * \brief  Destroys a random number generator component.
 */
void
xme_prim_randomNumberGenerator_destroy(xme_prim_randomNumberGenerator_configStruct_t* config);

#endif // #ifndef XME_PRIM_RANDOMNUMBERGENERATOR_H

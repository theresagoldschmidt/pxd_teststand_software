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
 *         Random string generator.
 *
 * \author
 *		   Marcel Flesch		<fleschmarcel@hotmail.com>       
 *		   Michael Geisinger	<geisinger@fortiss.org>
 */

#ifndef XME_PRIM_randomStringGenerator_H
#define XME_PRIM_RANDOMSTRINGGENERATOR_H

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
 * \typedef xme_prim_randomStringGenerator_configStruct_t
 *
 * \brief  Random string generator configuration structure.
 */
typedef struct
{
	// public
	xme_hal_time_interval_t intervalMs;
	// private
	xme_core_dcc_publicationHandle_t publicationHandle;
	xme_core_resourceManager_taskHandle_t taskHandle;
}
xme_prim_randomStringGenerator_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates a random string generator component.
 */
xme_core_status_t
xme_prim_randomStringGenerator_create(xme_prim_randomStringGenerator_configStruct_t* config);

/**
 * \brief  Activates a random string generator component.
 */
xme_core_status_t
xme_prim_randomStringGenerator_activate(xme_prim_randomStringGenerator_configStruct_t* config);

/**
 * \brief  Deactivates a random string generator component.
 */
void
xme_prim_randomStringGenerator_deactivate(xme_prim_randomStringGenerator_configStruct_t* config);

/**
 * \brief  Destroys a random string generator component.
 */
void
xme_prim_randomStringGenerator_destroy(xme_prim_randomStringGenerator_configStruct_t* config);

#endif // #ifndef XME_PRIM_randomStringGenerator_H

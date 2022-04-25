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
 *         Log message generator.
 *
 *         Regularly generates log messages with increasing severity level. 
 *         Interval can be specified in configuration. 
 *         Can be used to test and demonstrate logging. *
 * \author
 *         Benjamin Wiesmueller <wiesmueller@fortiss.org>
 */

#ifndef XME_PRIM_LOGMSGGENERATOR_H
#define XME_PRIM_LOGMSGGENERATOR_H

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
 * \typedef xme_prim_logMsgGenerator_configStruct_t
 *
 * \brief  Random number generator configuration structure.
 */
typedef struct
{
	// public
	xme_hal_time_interval_t intervalMs;
	// private
	uint8_t i;
	xme_core_component_t componentId;
	xme_core_resourceManager_taskHandle_t taskHandle;
}
xme_prim_logMsgGenerator_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates a test log message generator component.
 */
xme_core_status_t
xme_prim_logMsgGenerator_create(xme_prim_logMsgGenerator_configStruct_t* config);

/**
 * \brief  Activates a test log message generator component.
 */
xme_core_status_t
xme_prim_logMsgGenerator_activate(xme_prim_logMsgGenerator_configStruct_t* config);

/**
 * \brief  Deactivates a test log message generator component.
 */
void
xme_prim_logMsgGenerator_deactivate(xme_prim_logMsgGenerator_configStruct_t* config);

/**
 * \brief  Destroys a test log message generator component.
 */
void
xme_prim_logMsgGenerator_destroy(xme_prim_logMsgGenerator_configStruct_t* config);

#endif // #ifndef XME_PRIM_LOGMSGGENERATOR_H

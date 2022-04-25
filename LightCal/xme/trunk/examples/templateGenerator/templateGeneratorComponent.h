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
 *         Template generator component.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef TEMPLATEGENERATORCOMPONENT_H
#define TEMPLATEGENERATORCOMPONENT_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/component.h"
#include "xme/core/dcc.h"
#include "xme/core/resourceManager.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
typedef struct
{
	xme_core_resourceManager_taskHandle_t taskHandle; ///< Task handle for user input task.
	xme_hal_sharedPtr_t componentName;
	xme_hal_sharedPtr_t componentClass;
	xme_hal_sharedPtr_t authorName;
	xme_hal_sharedPtr_t authorMail;
	xme_hal_sharedPtr_t componentNamePretty;
	xme_hal_sharedPtr_t componentNamePreprocessor;
	xme_hal_sharedPtr_t componentNameIdentifier;
	xme_hal_sharedPtr_t componentClassUpper;
}
templateGeneratorComponent_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates a template generator component.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            initialized.
 *          - XME_CORE_STATUS_INTERNAL_ERROR on error.
 */
xme_core_status_t
templateGeneratorComponent_create(templateGeneratorComponent_configStruct_t* config);

/**
 * \brief  Activates a template generator component.
 */
xme_core_status_t
templateGeneratorComponent_activate(templateGeneratorComponent_configStruct_t* config);

/**
 * \brief  Deactivates a template generator component.
 */
void
templateGeneratorComponent_deactivate(templateGeneratorComponent_configStruct_t* config);

/**
 * \brief  Destroys a template generator component.
 */
void
templateGeneratorComponent_destroy(templateGeneratorComponent_configStruct_t* config);

#endif // #ifndef TEMPLATEGENERATORCOMPONENT_H

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
 *         Hello World component.
 *
 * \author
 *         Hauke Stähle <staehle@fortiss.org>
 *         Dominik Sojer <sojer@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef HELLOWORLDCOMPONENT_H
#define HELLOWORLDCOMPONENT_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/dcc.h"
#include "xme/core/resourceManager.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
typedef struct
{
	xme_core_dcc_publicationHandle_t publicationHandle;
	xme_core_dcc_subscriptionHandle_t subscriptionHandle;
	xme_core_resourceManager_taskHandle_t taskHandle;
}
helloWorldComponent_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates a Hello World component.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            initialized.
 *          - XME_CORE_STATUS_INTERNAL_ERROR on error.
 */
xme_core_status_t
helloWorldComponent_create(helloWorldComponent_configStruct_t* config);

/**
 * \brief  Activates a Hello World component.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            activated.
 *          - XME_CORE_STATUS_INTERNAL_ERROR on error.
 */
xme_core_status_t
helloWorldComponent_activate(helloWorldComponent_configStruct_t* config);

/**
 * \brief  Deactivates a Hello World component.
 */
void
helloWorldComponent_deactivate(helloWorldComponent_configStruct_t* config);

/**
 * \brief  Destroys a Hello World component.
 */
void
helloWorldComponent_destroy(helloWorldComponent_configStruct_t* config);

#endif // #ifndef HELLOWORLDCOMPONENT_H

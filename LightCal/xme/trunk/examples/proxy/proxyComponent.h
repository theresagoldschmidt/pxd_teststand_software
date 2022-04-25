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
 *         Proxy component.
 *
 * \author
 *         Christian Fuchs <fuchsch@in.tum.de>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/component.h"
#include "xme/core/core.h"
#include "xme/core/defines.h"
#include "xme/core/nodeManager.h"
#include "xme/core/interfaceManager.h"
#include "xme/core/broker.h"


/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates a proxy component.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            initialized.
 *          - XME_CORE_STATUS_INTERNAL_ERROR on error.
 */
xme_core_status_t
proxyComponent_create(xme_core_nodeManager_configStruct_t* config);

/**
 * \brief  Activates a proxy component.
 */
xme_core_status_t
proxyComponent_activate(xme_core_nodeManager_configStruct_t* config);

/**
 * \brief  Deactivates a proxy component.
 */
void
proxyComponent_deactivate(xme_core_nodeManager_configStruct_t* config);

/**
 * \brief  Destroys a proxy component.
 */
void
proxyComponent_destroy(xme_core_nodeManager_configStruct_t* config);

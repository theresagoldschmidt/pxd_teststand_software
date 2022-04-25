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
 *         Reliable publisher component.
 *
 * \author
 *         Hauke Stähle <staehle@fortiss.org>
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
 * \brief  Creates component.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            initialized.
 *          - XME_CORE_STATUS_INTERNAL_ERROR on error.
 */
xme_core_status_t
reliablePubComponent_create(xme_core_nodeManager_configStruct_t* config);

/**
 * \brief  Activates component.
 */
xme_core_status_t
reliablePubComponent_activate(xme_core_nodeManager_configStruct_t* config);

/**
 * \brief  Deactivates component.
 */
void
reliablePubComponent_deactivate(xme_core_nodeManager_configStruct_t* config);

/**
 * \brief  Destroys component.
 */
void
reliablePubComponent_destroy(xme_core_nodeManager_configStruct_t* config);

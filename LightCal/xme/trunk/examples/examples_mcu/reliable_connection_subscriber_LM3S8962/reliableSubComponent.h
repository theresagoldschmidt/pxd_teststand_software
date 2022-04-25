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
 *         This project shows an example on how to connect
 *         to a publisher in a reliable way.
 *
 *         This means that a connection is setup via
 *         TCP to the publsiher. The project uses
 *         freeRTOS and lwIP.
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
reliableSubComponent_create(xme_core_nodeManager_configStruct_t* config);

/**
 * \brief  Activates component.
 */
xme_core_status_t
reliableSubComponent_activate(xme_core_nodeManager_configStruct_t* config);

/**
 * \brief  Deactivates component.
 */
void
reliableSubComponent_deactivate(xme_core_nodeManager_configStruct_t* config);

/**
 * \brief  Destroys component.
 */
void
reliableSubComponent_destroy(xme_core_nodeManager_configStruct_t* config);

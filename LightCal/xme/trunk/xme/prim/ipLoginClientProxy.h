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
 *         IP login client proxy.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_PRIM_IPLOGINCLIENTPROXY_H
#define XME_PRIM_IPLOGINCLIENTPROXY_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/dcc.h"
#include "xme/core/rr.h"

#include "xme/hal/net.h"
#include "xme/hal/sched.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_prim_ipLoginClientProxy_configStruct_t
 *
 * \brief  IP login proxy configuration structure.
 */
typedef struct
{
	// public
	xme_core_interface_interfaceId_t interfaceId;
	// private
	xme_core_rr_requestHandle_t loginRequestHandle;
	xme_core_dcc_publicationHandle_t managementChannelsToNewNode; ///< Publication handle to trigger construction of management channel to new node.
	xme_hal_sched_taskHandle_t loginTaskHandle;
	xme_hal_net_socketHandle_t loginServerSocket;
}
xme_prim_ipLoginClientProxy_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates an IP login client proxy component.
 */
xme_core_status_t
xme_prim_ipLoginClientProxy_create(xme_prim_ipLoginClientProxy_configStruct_t* config);

/**
 * \brief  Activates an IP login client proxy component.
 */
xme_core_status_t
xme_prim_ipLoginClientProxy_activate(xme_prim_ipLoginClientProxy_configStruct_t* config);

/**
 * \brief  Deactivates an IP login client proxy component.
 */
void
xme_prim_ipLoginClientProxy_deactivate(xme_prim_ipLoginClientProxy_configStruct_t* config);

/**
 * \brief  Destroys an IP login client proxy component.
 */
void
xme_prim_ipLoginClientProxy_destroy(xme_prim_ipLoginClientProxy_configStruct_t* config);

#endif // #ifndef XME_PRIM_IPLOGINCLIENTPROXY_H

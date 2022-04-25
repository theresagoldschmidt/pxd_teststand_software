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
 *         IP login server proxy.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_PRIM_IPLOGINSERVERPROXY_H
#define XME_PRIM_IPLOGINSERVERPROXY_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/core.h"
#include "xme/core/rr.h"

#include "xme/core/device.h"

#include "xme/hal/net.h"
#include "xme/hal/sched.h"
#include "xme/hal/time.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
typedef struct
{
	xme_core_rr_responseInstanceHandle_t responseInstanceHandle;
	xme_core_device_type_t deviceType;
	xme_core_device_guid_t deviceGuid;
	xme_hal_time_interval_t responseTimeoutMs;
	xme_hal_time_handle_t lastUpdate;
}
xme_prim_ipLoginServerProxy_pendingResponseItem_t;

/**
 * \typedef xme_prim_ipLoginServerProxy_configStruct_t
 *
 * \brief  IP login server proxy configuration structure.
 */
typedef struct
{
	// public
	// private
	xme_core_rr_requestHandlerHandle_t loginRequestHandlerHandle; ///< Login request handler handle.
	xme_hal_net_socketHandle_t loginClientSocket; ///< Socket over which login requests are sent.
	xme_hal_sched_taskHandle_t loginResponseTaskHandle; ///< Login response task.
	XME_HAL_TABLE(xme_prim_ipLoginServerProxy_pendingResponseItem_t, pendingResponses, XME_HAL_DEFINES_MAX_IPLOGINSERVER_PENDINGRESPONSE_ITEMS); ///< Pending login responses.
}
xme_prim_ipLoginServerProxy_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates an IP login server proxy component.
 */
xme_core_status_t
xme_prim_ipLoginServerProxy_create(xme_prim_ipLoginServerProxy_configStruct_t* config);

/**
 * \brief  Activates an IP login server proxy component.
 */
xme_core_status_t
xme_prim_ipLoginServerProxy_activate(xme_prim_ipLoginServerProxy_configStruct_t* config);

/**
 * \brief  Deactivates an IP login server proxy component.
 */
void
xme_prim_ipLoginServerProxy_deactivate(xme_prim_ipLoginServerProxy_configStruct_t* config);

/**
 * \brief  Destroys an IP login server proxy component.
 */
void
xme_prim_ipLoginServerProxy_destroy(xme_prim_ipLoginServerProxy_configStruct_t* config);

#endif // #ifndef XME_PRIM_IPLOGINSERVERPROXY_H

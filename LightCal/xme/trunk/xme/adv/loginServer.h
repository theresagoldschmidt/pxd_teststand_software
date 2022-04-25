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
 *         Login server.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_ADV_LOGINSERVER_H
#define XME_ADV_LOGINSERVER_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/rr.h"
#include "xme/core/device.h"
#include "xme/core/nodeManager.h"
#include "xme/core/rr.h"

#include "xme/hal/table.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
typedef struct
{
	xme_core_device_type_t deviceType;
	xme_core_device_guid_t deviceGuid;
	xme_core_node_nodeId_t nodeId;
	xme_core_node_nodeId_t edgeNodeId; ///< Unique node identifier of the node that first forwarded the login request.
	xme_core_interface_interfaceId_t edgeNodeInterfaceId; ///< Node unique identifier of the interface on the edge node that first received the login request.
	xme_core_dataChannel_t remoteAnnouncementDataChannel;
	xme_core_dataChannel_t remoteModifyRoutingTableDataChannel;
	xme_core_rr_responseInstanceHandle_t responseInstanceHandle;
}
xme_adv_loginServer_nodeIdAssignment_t;

/**
 * \typedef xme_adv_loginServer_configStruct_t
 *
 * \brief  Login server configuration structure.
 */
typedef struct
{
	// public
	xme_core_node_nodeId_t nextNodeId;
	// private
	XME_HAL_TABLE(xme_adv_loginServer_nodeIdAssignment_t, nodeIdAssignments, XME_HAL_DEFINES_MAX_NODE_ID_ASSIGNMENT_ITEMS);
	xme_core_rr_requestHandlerHandle_t loginRequestHandlerHandle;
	xme_core_rr_requestHandle_t newNodeRequestHandle;
}
xme_adv_loginServer_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates a login server component.
 */
xme_core_status_t
xme_adv_loginServer_create(xme_adv_loginServer_configStruct_t* config);

/**
 * \brief  Activates a login server component.
 */
xme_core_status_t
xme_adv_loginServer_activate(xme_adv_loginServer_configStruct_t* config);

/**
 * \brief  Deactivates a login server component.
 */
void
xme_adv_loginServer_deactivate(xme_adv_loginServer_configStruct_t* config);

/**
 * \brief  Destroys a login server component.
 */
void
xme_adv_loginServer_destroy(xme_adv_loginServer_configStruct_t* config);

#endif // #ifndef XME_ADV_LOGINSERVER_H

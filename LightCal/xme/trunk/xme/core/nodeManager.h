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
 *         Node manager.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_CORE_NODEMANAGER_H
#define XME_CORE_NODEMANAGER_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/core.h"
#include "xme/core/device.h"
#include "xme/core/resourceManager.h"
#include "xme/core/node.h"
#include "xme/core/rr.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_core_nodeManager_configStruct_t
 *
 * \brief  Node manager configuration structure.
 */
typedef struct
{
	// public
	xme_core_device_type_t deviceType; ///< Device type
	xme_core_device_guid_t deviceGuid; ///< Globally unique device identifier (e.g., serial number, MAC address)
	// private
	xme_core_node_nodeId_t nodeId; ///< Unique node identifier.
	xme_core_rr_requestHandle_t loginRequestHandle; ///< Login request handle.
	xme_core_dcc_publicationHandle_t managementChannelsToEdgeNode; ///< Publication handle to trigger construction of management channel to edge node.
	xme_core_resourceManager_taskHandle_t loginTaskHandle; ///< Login task handle.
}
xme_core_nodeManager_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates a node manager component.
 *         Exactly one component of this type must be present on every node.
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            initialized.
 *          - XME_CORE_STATUS_INVALID_CONFIGURATION if a component of this
 *            type has already been initialized. Exactly one component of this
 *            type must be present on every node.
 */
xme_core_status_t
xme_core_nodeManager_create(xme_core_nodeManager_configStruct_t* config);

/**
 * \brief  Activates a node manager component.
 */
xme_core_status_t
xme_core_nodeManager_activate(xme_core_nodeManager_configStruct_t* config);

/**
 * \brief  Deactivates a node manager component.
 */
void
xme_core_nodeManager_deactivate(xme_core_nodeManager_configStruct_t* config);

/**
 * \brief  Destroys a node manager component.
 *         Exactly one component of this type must be present on every node.
 */
void
xme_core_nodeManager_destroy(xme_core_nodeManager_configStruct_t* config);

/**
 * \brief  Returns the globally unique device identifier of this node.
 *
 * \return Globally unique device identifier of this node.
 */
xme_core_device_guid_t
xme_core_nodeManager_getDeviceGuid();

/**
 * \brief  Returns the device type of this node.
 *
 * \return Device type of this node.
 */
xme_core_device_type_t
xme_core_nodeManager_getDeviceType();

/**
 * \brief  Returns the logical node ID currently assigned to the node this
 *         node manager belongs to.
 *
 * \return Currently assigned logical node ID.
 */
xme_core_node_nodeId_t
xme_core_nodeManager_getNodeId();

#endif // #ifndef XME_CORE_NODEMANAGER_H

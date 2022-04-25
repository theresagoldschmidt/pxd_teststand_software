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
 *         coordinator example project.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Dominik Sojer <sojer@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/prim/ipLoginClientProxy.h"

#include "xme/core/componentList.h"
#include "xme/core/core.h"
#include "xme/core/defines.h"
#include "xme/core/nodeManager.h"

#include "xme/adv/loginServer.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define XME_CORE_NODEMANAGER_DEVICE_TYPE ((xme_core_device_type_t)0x00000C004D14A104)
#define XME_CORE_NODEMANAGER_DEVICE_GUID XME_CORE_DEVICE_GUID_RANDOM

/******************************************************************************/
/***   Component configurations                                             ***/
/******************************************************************************/
XME_COMPONENT_CONFIG_INSTANCE(xme_core_nodeManager) =
{
	// public
	XME_CORE_NODEMANAGER_DEVICE_TYPE, // deviceType
	XME_CORE_NODEMANAGER_DEVICE_GUID // deviceGuid
};

XME_COMPONENT_CONFIG_INSTANCE(xme_adv_loginServer) =
{
	// public
	(xme_core_node_nodeId_t)(XME_CORE_NODE_LOCAL_NODE_ID+1) // nextNodeId
};

xme_prim_ipLoginClientProxy_configStruct_t xme_prim_ipLoginClientProxy_config[1];

/******************************************************************************/
/***   Component descriptor                                                 ***/
/******************************************************************************/
XME_COMPONENT_LIST_BEGIN
	XME_COMPONENT_LIST_ITEM(xme_core_nodeManager, 0)
	XME_COMPONENT_LIST_ITEM(xme_prim_ipLoginClientProxy, 0)
	XME_COMPONENT_LIST_ITEM(xme_adv_loginServer, 0)
XME_COMPONENT_LIST_END;

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
int
main (int argc, char* argv[])
{
	XME_CHECK_MSG
	(
		XME_CORE_STATUS_SUCCESS == xme_core_init(),
		1,
		XME_LOG_FATAL,
		"Fatal error during XME initialization!\n"
	);

	xme_core_run();

	xme_core_fini();

	return 0;
}

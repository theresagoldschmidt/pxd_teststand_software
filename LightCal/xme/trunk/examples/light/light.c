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
 *         Light example project.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/adv/loginServer.h"

#include "xme/prim/consoleLogger.h"
#include "xme/prim/ipLoginClientProxy.h"
#include "xme/prim/randomNumberGenerator.h"

#include "xme/core/componentList.h"
#include "xme/core/core.h"
#include "xme/core/defines.h"
#include "xme/core/resourceManager.h"
#include "xme/core/directory.h"

#include "xme/hal/sched.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define XME_CORE_NODEMANAGER_DEVICE_TYPE ((xme_core_device_type_t)0x0000000000011841)
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

XME_COMPONENT_CONFIG_INSTANCE(xme_prim_ipLoginClientProxy) =
{
	// public
	(xme_core_interface_interfaceId_t)0 // interfaceId
};

XME_COMPONENT_CONFIG_INSTANCE(xme_prim_consoleLogger) =
{
	XME_CORE_TOPIC_UNSIGNED_INTEGER // subscribedTopic
};

XME_COMPONENT_CONFIG_INSTANCE(xme_prim_randomNumberGenerator) =
{
	100, // minValue
	200, // maxValue
	5000 // intervalMs
};

/******************************************************************************/
/***   Component descriptor                                                 ***/
/******************************************************************************/
XME_COMPONENT_LIST_BEGIN
	XME_COMPONENT_LIST_ITEM(xme_core_nodeManager, 0)
	XME_COMPONENT_LIST_ITEM(xme_adv_loginServer, 0)
	XME_COMPONENT_LIST_ITEM(xme_prim_ipLoginClientProxy, 0)
	XME_COMPONENT_LIST_ITEM(xme_prim_consoleLogger, 0)
	XME_COMPONENT_LIST_ITEM(xme_prim_randomNumberGenerator, 0)
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

	return 0;
}

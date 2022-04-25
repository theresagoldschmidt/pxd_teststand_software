/*
 * Copyright (c) 2011-2012, fortiss GmbH.
 * Licensed under the Apache License, Version 2.0.
 *
 * Use, modification and distribution are subject to the terms specified
 * in the accompanying license file LICENSE.txt located at the root directory
 * of node software distribution. A copy is available at
 * http://chromosome.fortiss.org/.
 *
 * This file is part of CHROMOSOME.
 *
 * $Id$
 */

/**
 * \file 
 *          Logger node.
 *
 *			This node contains the following components: <name> (<type>)
 *          - ipLoginClientProxy (ipLoginClientProxy)
 *          - logMsgGenerator1 (logMsgGenerator)
 *          - logMsgGenerator2 (logMsgGenerator)
 *          - loginServer (loginServer)
 *
 * \author
 *          Benjamin Wiesmueller <wiesmueller@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/adv/loginServer.h"

#include "xme/prim/ipLoginClientProxy.h"
#include "xme/prim/logMsgGenerator.h"
#include "xme/prim/consoleLogger.h"

#include "xme/core/componentList.h"
#include "xme/core/core.h"
#include "xme/core/defines.h"
#include "xme/core/log.h"
#include "xme/core/logger.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define XME_CORE_NODEMANAGER_DEVICE_TYPE ((xme_core_device_type_t)347561857)
#define XME_CORE_NODEMANAGER_DEVICE_GUID XME_CORE_DEVICE_GUID_RANDOM

/******************************************************************************/
/***   Component configurations                                             ***/
/******************************************************************************/
XME_COMPONENT_CONFIG_INSTANCE(xme_core_nodeManager) =
{
	// public
	XME_CORE_NODEMANAGER_DEVICE_TYPE, // deviceType
	XME_CORE_NODEMANAGER_DEVICE_GUID  // deviceGuid
};

XME_COMPONENT_CONFIG_INSTANCE(xme_prim_ipLoginClientProxy) =
{
	// Component ipLoginClientProxy
	(xme_core_interface_interfaceId_t)(0) // interfaceId
};

XME_COMPONENT_CONFIG_INSTANCE(xme_prim_logMsgGenerator) =
{
	// Component logMsgGenerator1
	{
		4000 // intervalMs
	},
	// Component logMsgGenerator2
	{
		7000 // intervalMs
	}
};

XME_COMPONENT_CONFIG_INSTANCE(xme_prim_consoleLogger) =
{
	// Component consoleLogger
	false,        // localOnly
	XME_LOG_NOTE, // minSeverity
	true          // verbose
};

XME_COMPONENT_CONFIG_INSTANCE(xme_adv_loginServer) =
{
	// Component loginServer
	(xme_core_node_nodeId_t)(XME_CORE_NODE_LOCAL_NODE_ID+1) // nextNodeId
};

/******************************************************************************/
/***   Component descriptor                                                 ***/
/******************************************************************************/
XME_COMPONENT_LIST_BEGIN
	XME_COMPONENT_LIST_ITEM(xme_core_nodeManager, 0)
	XME_COMPONENT_LIST_ITEM_NO_CONFIG(xme_core_logger)
	XME_COMPONENT_LIST_ITEM(xme_prim_ipLoginClientProxy, 0)
	XME_COMPONENT_LIST_ITEM(xme_adv_loginServer, 0)
	XME_COMPONENT_LIST_ITEM(xme_prim_consoleLogger, 0)
	XME_COMPONENT_LIST_ITEM(xme_prim_logMsgGenerator, 0)
	XME_COMPONENT_LIST_ITEM(xme_prim_logMsgGenerator, 1)
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

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
 *         Database example project.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *		   Marcel Flesch <fleschmarcel@mytum.de>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/adv/databaseLogger.h"
#include "xme/adv/loginServer.h"
#include "xme/adv/mstLogger.h"

#include "xme/prim/ipLoginClientProxy.h"
#include "xme/prim/modbusMaster.h"
#include "xme/prim/randomArrayGenerator.h"

#include "xme/core/componentList.h"
#include "xme/core/core.h"
#include "xme/core/defines.h"

#include "xme/hal/sched.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define XME_CORE_NODEMANAGER_DEVICE_TYPE ((xme_core_device_type_t)0x000000000415D045)
#define XME_CORE_NODEMANAGER_DEVICE_GUID ((xme_core_device_guid_t)0x0000000000011841)

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
	XME_CORE_NODE_LOCAL_NODE_ID+1 // nextNodeId
};

XME_COMPONENT_CONFIG_INSTANCE(xme_prim_ipLoginClientProxy) =
{
	// public
	(xme_core_interface_interfaceId_t)0 // interfaceId
};


XME_COMPONENT_CONFIG_INSTANCE(xme_prim_randomArrayGenerator) =
{
	// public
	1, // minValue
	1000, // maxValue
	5000 // intervalMs
};


XME_COMPONENT_CONFIG_INSTANCE(xme_adv_databaseLogger) =
{
	// public
	XME_CORE_TOPIC_ARRAY_OF_DATA,		 // listenTopic
	"192.168.17.187", // dbAddress
	"random", // dbDatabase
	"random", // dbUserName
	"random", // dbPassword
	"table", // tableName
	"timestamp\0string\0number\0",			// columnNames
	"timestamp\0varchar(20)\0int(11)\0"		// columnTypes
};

XME_COMPONENT_CONFIG_INSTANCE(xme_adv_mstLogger) =
{
	// public
	5000 // intervalMs
};

XME_COMPONENT_CONFIG_INSTANCE(xme_prim_modbusMaster) =
{
	// public
	0 // dummy
};

/******************************************************************************/
/***   Component descriptor                                                 ***/
/******************************************************************************/
XME_COMPONENT_LIST_BEGIN
	XME_COMPONENT_LIST_ITEM(xme_core_nodeManager, 0)
	XME_COMPONENT_LIST_ITEM(xme_adv_loginServer, 0)
	XME_COMPONENT_LIST_ITEM(xme_prim_ipLoginClientProxy, 0)
	XME_COMPONENT_LIST_ITEM(xme_adv_databaseLogger, 0)
	XME_COMPONENT_LIST_ITEM(xme_prim_randomArrayGenerator, 0)
	XME_COMPONENT_LIST_ITEM(xme_prim_modbusMaster, 0)
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

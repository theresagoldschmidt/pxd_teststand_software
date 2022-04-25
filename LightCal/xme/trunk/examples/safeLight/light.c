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

#include "xme/prim/ipLoginClientProxy.h"
#include "xme/prim/randomNumberGenerator.h"

#include "xme/core/componentList.h"
#include "xme/core/core.h"
#include "xme/core/defines.h"

#include "xme/hal/sched.h"

#include "xme/adv/testHeartbeat.h"
#include "xme/adv/heartbeat.h"
#include "xme/adv/testConsistency.h"
#include "xme/adv/voter.h"

#include "light.h"

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
	(xme_core_node_nodeId_t)(XME_CORE_NODE_LOCAL_NODE_ID+1) // nextNodeId
};

XME_COMPONENT_CONFIG_INSTANCE(xme_prim_ipLoginClientProxy) =
{
	// public
	(xme_core_interface_interfaceId_t)0 // interfaceId
};

XME_COMPONENT_CONFIG_INSTANCE(xme_prim_randomNumberGenerator) =
{
	// public
	0, // min
	65535, // max
	5000 // intervalMs
};

XME_COMPONENT_CONFIG_INSTANCE(xme_adv_testHeartbeat) =
{
	(xme_hal_time_handle_t)3000,
	true,
	testFunction
};

XME_COMPONENT_CONFIG_INSTANCE(xme_adv_heartbeat) =
{
	(xme_hal_time_handle_t)5000
};

XME_COMPONENT_CONFIG_INSTANCE(xme_adv_testConsistency) =
{
{
	(xme_hal_time_handle_t)10000, // startMs
	(xme_hal_time_handle_t)10000 // periodMs
},
{
	(xme_hal_time_handle_t)3000, // startMs
	(xme_hal_time_handle_t)0 // periodMs
}
};


XME_COMPONENT_CONFIG_INSTANCE(xme_adv_healthmonitor) =
{
	(xme_core_node_nodeId_t)1, // node id
	(xme_core_component_t)-1 // component id
};

XME_COMPONENT_CONFIG_INSTANCE(xme_adv_voter) =
{
	(xme_core_topic_t)101, // inputTopic
	(xme_core_topic_t)202, // outputTopic
	XME_ADV_VOTER_ALGORITHM_MEAN, // algorithm
	(xme_hal_time_handle_t)10000, // startMs
	(xme_hal_time_handle_t)10000 // periodMs
};


/******************************************************************************/
/***   Component descriptor                                                 ***/
/******************************************************************************/
XME_COMPONENT_LIST_BEGIN
	XME_COMPONENT_LIST_ITEM(xme_core_nodeManager, 0)
	XME_COMPONENT_LIST_ITEM(xme_adv_loginServer, 0)
	XME_COMPONENT_LIST_ITEM(xme_prim_ipLoginClientProxy, 0)
	XME_COMPONENT_LIST_ITEM(xme_adv_voter, 0)
	XME_COMPONENT_LIST_ITEM(xme_adv_testHeartbeat, 0)
	XME_COMPONENT_LIST_ITEM(xme_adv_heartbeat, 0)
	XME_COMPONENT_LIST_ITEM(xme_adv_testConsistency, 0)
	XME_COMPONENT_LIST_ITEM(xme_adv_testConsistency, 1)
	XME_COMPONENT_LIST_ITEM(xme_adv_healthmonitor, 0)
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

xme_core_status_t
testFunction() {
	printf("I am a callback function\n");
	return XME_CORE_STATUS_SUCCESS;
}

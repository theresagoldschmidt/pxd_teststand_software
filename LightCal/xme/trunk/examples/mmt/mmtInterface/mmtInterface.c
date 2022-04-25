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
 *         Interface to XME for the Multifunk Modeling Tool (MMT).
 *
 * \author
 *         Benjamin Wiesmueller <wiesmueller@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/prim/ipLoginServerProxy.h"

#include "xme/core/componentList.h"
#include "xme/core/core.h"
#include "xme/core/defines.h"
#include "xme/core/nodeManager.h"

#include "xme/hal/sharedPtr.h"

#include "components/mmtUpdater.h"
#include "components/mmtUpdaterDummy.h"
#include "components/mmtCommander.h"
#include "components/mmtLogger.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define XME_CORE_NODEMANAGER_DEVICE_TYPE ((xme_core_device_type_t)0x000000000415D045)
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

xme_prim_ipLoginServerProxy_configStruct_t xme_prim_ipLoginServerProxy_config[1];

//xme_adv_mmtUpdater_configStruct_t xme_adv_mmtUpdater_config[1];

//xme_adv_mmtUpdater_configStruct_t xme_adv_mmtUpdaterDummy_config[1];

//xme_adv_mmtCommander_configStruct_t xme_adv_mmtCommander_config[1];

xme_adv_mmtLogger_configStruct_t xme_adv_mmtLogger_config[1];

/******************************************************************************/
/***   Component descriptor                                                 ***/
/******************************************************************************/
XME_COMPONENT_LIST_BEGIN
	XME_COMPONENT_LIST_ITEM(xme_core_nodeManager, 0)
	XME_COMPONENT_LIST_ITEM(xme_prim_ipLoginServerProxy, 0)
	//XME_COMPONENT_LIST_ITEM(xme_adv_mmtUpdater, 0)
	//XME_COMPONENT_LIST_ITEM(xme_adv_mmtCommander, 0)
	//XME_COMPONENT_LIST_ITEM(xme_adv_mmtUpdaterDummy, 0)
	XME_COMPONENT_LIST_ITEM(xme_adv_mmtLogger, 0)
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

/**
 * \brief  Start the MMT<->XME Interface Node. Exported in DLL.
 *
 *         Starts a XME node. Takes function pointers as additional parameters
 *         These pointers are used to access queues which are used for
 *         communication between the MMT and the XME node.
 *         The function pointers are passed to several special XME components
 *         (via their configuration). These components do the actual communication.
 *
 * \param  mmt_updCallback Send an update message to the MMT (synchronous, returns true when message hase been received successfully).
 * \param  mmt_cmdCallback Receive a command message from the MMT (blocking call).
 * \param  mmt_logCallback Send a log message to the MMT (synchronous, returns true when message hase been received successfully).
 * \param  mmt_updCallback Print a message to the MMT console (useful for debugging).
 * \return Error code.
 */
#ifdef WIN32
	__declspec(dllexport) // Only needed for MSVC when creating a DLL
#endif
int
start
(
	  bool (*mmt_updCallback) (mmt_upd_t),
	  mmt_cmd_t *(*mmt_cmdCallback) (void), 
	  bool (*mmt_logCallback) (mmt_log_t),
	  void (*mmt_println)(const char*)
) 
{
	// Initiliaize configuration structures of components with function pointers 
	// for communication with MMT
	xme_adv_mmtLogger_config[0].mmt_logCallback = mmt_logCallback;
	xme_adv_mmtLogger_config[0].mmt_println = mmt_println;

	//xme_adv_mmtUpdater_config[0].mmt_updCallback = mmt_updCallback;
	//xme_adv_mmtUpdater_config[0].mmt_println = mmt_println;

	//xme_adv_mmtUpdaterDummy_config[0].mmt_updCallback = mmt_updCallback;
	//xme_adv_mmtUpdaterDummy_config[0].mmt_println = mmt_println;

	//xme_adv_mmtCommander_config[0].mmt_cmdCallback = mmt_cmdCallback;
	//xme_adv_mmtCommander_config[0].mmt_println = mmt_println;

	XME_CHECK_MSG
	(
		XME_CORE_STATUS_SUCCESS == xme_core_init(),
		1,
		XME_LOG_FATAL,
		"Fatal error during XME initialization!\n"
	);

	xme_core_run();

	mmt_println("XME: End of main loop.");

	xme_core_fini();

	return 0;
}

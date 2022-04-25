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
 *         Core runtime system.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/core.h"

#include "xme/core/dcc.h"
#include "xme/core/broker.h"
#include "xme/core/defines.h"
#include "xme/core/resourceManager.h"
#include "xme/core/nodeManager.h"
#include "xme/core/interfaceManager.h"
#include "xme/core/routingTable.h"

#include "xme/hal/energy.h"
#include "xme/hal/net.h"
#include "xme/hal/random.h"
#include "xme/hal/sched.h"
#include "xme/hal/sync.h"
#include "xme/hal/tls.h"

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_core_init()
{
	xme_core_config.state = XME_CORE_STATE_UNDEF;

	// Initialize mandatory system components
	XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_hal_random_init(), XME_CORE_STATUS_OUT_OF_RESOURCES);
	XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_hal_sync_init(), XME_CORE_STATUS_OUT_OF_RESOURCES);
	XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_hal_sharedPtr_init(), XME_CORE_STATUS_OUT_OF_RESOURCES);
	XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_hal_tls_init(), XME_CORE_STATUS_OUT_OF_RESOURCES);
	XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_hal_net_init(), XME_CORE_STATUS_OUT_OF_RESOURCES);
	XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_hal_sched_init(), XME_CORE_STATUS_OUT_OF_RESOURCES);
	XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_interfaceManager_init(), XME_CORE_STATUS_OUT_OF_RESOURCES);
	XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_dcc_init(), XME_CORE_STATUS_OUT_OF_RESOURCES);
	XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_rr_init(), XME_CORE_STATUS_OUT_OF_RESOURCES);
	XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_md_init(), XME_CORE_STATUS_OUT_OF_RESOURCES);
	XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_init(), XME_CORE_STATUS_OUT_OF_RESOURCES);
	XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_broker_init(), XME_CORE_STATUS_OUT_OF_RESOURCES);

	// Initialize the resource manager. This should be the last system component to
	// initialize, since it will initialize the user-defined components.
	XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_resourceManager_init(), XME_CORE_STATUS_OUT_OF_RESOURCES);

	xme_core_config.state = XME_CORE_STATE_INIT;

	// Activate all components
	XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_resourceManager_activateComponents(), XME_CORE_STATUS_INTERNAL_ERROR);

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_core_run()
{
	if (XME_CORE_STATE_INIT != xme_core_config.state)
	{
		return;
	}

	while (!xme_core_isShutdownInProgress())
	{
		// If there is no task available, go to sleep mode
		/*while (xme_core_resourceManager_idle())*/
		{
			xme_hal_energy_sleep();
		}

		// Dispatch a task
		xme_core_resourceManager_runTask();
	}
}

void
xme_core_shutdown()
{
	xme_core_config.state = XME_CORE_STATE_SHUTDOWN;
}

bool
xme_core_isShutdownInProgress()
{
	return (XME_CORE_STATE_SHUTDOWN == xme_core_config.state);
}

void
xme_core_fini()
{
	// Deactivate all components
	xme_core_resourceManager_deactivateComponents();

	// Free the resource manager. This should be the first system component to
	// finalize, since it will finalize the user-defined components.
	xme_core_resourceManager_fini();

	// Free mandatory system components (in opposite order of initialization)
	xme_core_broker_fini();
	xme_core_routingTable_fini();
	xme_core_md_fini();
	xme_core_rr_fini();
	xme_core_dcc_fini();
	xme_core_interfaceManager_fini();
	xme_hal_sched_fini();
	xme_hal_net_fini();
	xme_hal_tls_fini();
	xme_hal_sharedPtr_fini();
	xme_hal_sync_fini();
	xme_hal_random_fini();

	xme_core_config.state = XME_CORE_STATE_UNDEF;
}

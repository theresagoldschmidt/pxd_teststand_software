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
 *         Communication interface abstraction.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/interface.h"

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_hal_net_interfaceDescr_t*
xme_core_interface_getInterface
(
	xme_core_interface_interfaceId_t interfaceId
)
{
	// TODO: How to manage interfaces? See ticket #746
	static xme_hal_net_interfaceDescr_t intf =
	{
		XME_CORE_INTERFACEMANAGER_INVALID_INTERFACE_ID, // interfaceID

		NULL, // init
		NULL, // fini

		NULL, // read_non_blocking
		NULL, // read_blocking

		NULL, // write_non_blocking
		NULL, // write_blocking

		NULL, // join_channel
		NULL, // leave_channel

		NULL, // get_available_data_size
		NULL, // wait_for_state_change

		NULL, // register_callback
		NULL, // clear_callback

		NULL, // provide_channel
		NULL // unprovide_channel
	};

	return &intf;
}

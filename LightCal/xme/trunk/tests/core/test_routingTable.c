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
 *         Routing Table testsuite.
 *
 * \author
 *         Stephan Sommer <sommer@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/


#include "tests/tests.h"
#include "xme/core/broker.h"
#include "xme/core/dcc.h"
#include "xme/core/md.h"
#include "xme/core/topic.h"
#include "xme/core/routingTable.h"

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/

int
xme_tests_core_routingTable()
{
	
	XME_TESTSUITE_BEGIN();
	{	
		/* This can't work because it relies on some magic ports to exist.
		   Please fix.

		xme_core_interface_address_t addr;
		addr.data[0]=123;
		XME_TEST(XME_CORE_STATUS_SUCCESS == (xme_core_routingTable_addLocalSourceRoute(4712, 4, 2)));
		XME_TEST(XME_CORE_STATUS_SUCCESS == (xme_core_routingTable_addLocalDestinationRoute(4712, 4, 5)));
		
		
		
		XME_TEST(XME_CORE_STATUS_SUCCESS == (xme_core_routingTable_removeLocalDestinationRoute(4712, 4, 5)));

		

		XME_TEST(XME_CORE_STATUS_SUCCESS == (xme_core_routingTable_addOutboundRoute(4712,1,&addr)));
		XME_TEST(XME_CORE_STATUS_SUCCESS == (xme_core_routingTable_removeOutboundRoute(4712,1,&addr)));

		*/
	}
	
	XME_TESTSUITE_END();
}

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
 *         Testsuite invoker.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "tests/tests.h"

#include "tests/core/test_broker.h"
#include "tests/core/test_core.h"
#include "tests/core/test_dcc.h"
#include "tests/core/test_defines.h"
#include "tests/core/test_directory.h"
#include "tests/core/test_login.h"
#include "tests/core/test_md.h"
#include "tests/core/test_routingTable.h"

#include "tests/hal/test_linkedList.h"
#include "tests/hal/test_math.h"
#include "tests/hal/test_net.h"
#include "tests/hal/test_random.h"
#include "tests/hal/test_shm.h"
#include "tests/hal/test_table.h"
#include "tests/hal/test_time.h"

#include "xme/core/componentList.h"
#include "xme/core/directory.h"
#include "xme/core/nodeManager.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define XME_COMPONENT_CONTEXT_TEST_DUMMY ((xme_core_component_t)0x0004)
#define XME_CORE_NODEMANAGER_DEVICE_TYPE ((xme_core_device_type_t)0x1234567890ABCDEF)
#define XME_CORE_NODEMANAGER_DEVICE_GUID ((xme_core_device_guid_t)0x0000000000001E51)

/******************************************************************************/
/***   Component configurations                                             ***/
/******************************************************************************/
XME_COMPONENT_CONFIG_INSTANCE(xme_core_nodeManager) =
{
	// public
	XME_CORE_NODEMANAGER_DEVICE_TYPE, // deviceType
	XME_CORE_NODEMANAGER_DEVICE_GUID // deviceGuid
};

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
xme_core_status_t
xme_test_dummy_create(xme_core_componentConfigStruct_t* config);

/******************************************************************************/
/***   Component descriptor                                                 ***/
/******************************************************************************/
XME_COMPONENT_LIST_BEGIN
	XME_COMPONENT_LIST_ITEM(xme_core_nodeManager, 0)
	XME_COMPONENT_LIST_ITEM_MANUAL("xme_test_dummy", &xme_test_dummy_create, NULL, NULL, NULL, NULL) // Dummy component for testsuite. Must be at position number XME_COMPONENT_CONTEXT_TEST_DUMMY
XME_COMPONENT_LIST_END;

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_test_dummy_create(xme_core_componentConfigStruct_t* config)
{
	return XME_CORE_STATUS_SUCCESS;
}

int
main (int argc, char* argv[])
{
	XME_TESTSUITE_BEGIN();
	XME_TESTSUITE(xme_tests_core_init(), "xme_core_init testsuite failed!");
	XME_TESTSUITE(xme_tests_hal_shm(), "xme_hal_shm testsuite failed!");
	XME_TESTSUITE(xme_tests_hal_math(), "xme_hal_math testsuite failed!");
	XME_TESTSUITE(xme_tests_hal_random(), "xme_hal_random testsuite failed!");
	XME_TESTSUITE(xme_tests_hal_linkedList(), "xme_hal_linkedList testsuite failed!");
	XME_TESTSUITE(xme_tests_hal_table(), "xme_hal_table testsuite failed!");
	XME_TESTSUITE(xme_tests_hal_time(), "xme_hal_table testsuite failed!");
//	XME_TESTSUITE(xme_tests_hal_net(), "xme_hal_net testsuite failed!");
	xme_core_resourceManager_setCurrentComponentId(XME_COMPONENT_CONTEXT_TEST_DUMMY);
	XME_TESTSUITE(xme_tests_core_defines(), "xme_core_defines testsuite failed!");
	XME_TESTSUITE(xme_tests_core_dcc(), "xme_core_dcc testsuite failed!");
	XME_TESTSUITE(xme_tests_core_login(), "xme_core_login testsuite failed!");
	XME_TESTSUITE(xme_tests_core_md(), "xme_core_md testsuite failed!");
	XME_TESTSUITE(xme_tests_core_directory(), "xme_core_directory testsuite failed!");
//	XME_TESTSUITE(xme_tests_core_broker(), "xme_core_broker testsuite failed!");
	XME_TESTSUITE(xme_tests_core_routingTable(), "xme_core_routingTable testsuite failed!");
	XME_TESTSUITE(xme_tests_core_fini(), "xme_core_fini testsuite failed!");
	XME_TESTSUITE_END();
}

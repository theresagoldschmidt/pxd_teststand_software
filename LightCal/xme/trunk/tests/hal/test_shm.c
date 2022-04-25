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
 *         SHM testsuite.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Stephan Sommer <sommer@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "tests/hal/test_shm.h"

#include "tests/tests.h"
#include "xme/core/core.h"
#include "xme/hal/sharedPtr.h"

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
int
xme_tests_hal_shm()
{
	XME_TESTSUITE_BEGIN();
	{
		xme_hal_sharedPtr_t dataHandle, dataHandle2;
		uint8_t test_buffer[DATA_BUFFER_SIZE];
		uint8_t test_buffer2[DATA_BUFFER_SIZE];
		test_buffer[0] = 0x01;
		test_buffer[DATA_BUFFER_SIZE-1] = 0x99;
		test_buffer2[0] = 0x02;
		test_buffer2[DATA_BUFFER_SIZE-1] = 0x98;

		dataHandle = xme_hal_sharedPtr_createFromPointer(DATA_BUFFER_SIZE, test_buffer);
		XME_TEST(DATA_BUFFER_SIZE == xme_hal_sharedPtr_getSize(dataHandle));
		XME_TEST(1 == xme_hal_sharedPtr_getReferenceCount(dataHandle));
		XME_TEST(DATA_BUFFER_SIZE == xme_hal_sharedPtr_getSize(dataHandle));

		dataHandle2 = xme_hal_sharedPtr_createFromPointer(DATA_BUFFER_SIZE, test_buffer2);
		XME_TEST(XME_HAL_SHAREDPTR_INVALID_POINTER != dataHandle2);
		XME_TEST(1 == xme_hal_sharedPtr_getReferenceCount(dataHandle2));

		XME_TEST(DATA_BUFFER_SIZE == xme_hal_sharedPtr_getSize(dataHandle2));

		XME_TEST(XME_HAL_SHAREDPTR_INVALID_POINTER != dataHandle);
		XME_TEST(1 == xme_hal_sharedPtr_getReferenceCount(dataHandle));

		XME_TEST(dataHandle == xme_hal_sharedPtr_retain(dataHandle));
		XME_TEST(2 == xme_hal_sharedPtr_getReferenceCount(dataHandle));

		XME_TEST(dataHandle == xme_hal_sharedPtr_retain(dataHandle));
		XME_TEST(3 == xme_hal_sharedPtr_getReferenceCount(dataHandle));

		xme_hal_sharedPtr_destroy(dataHandle);
		XME_TEST(2 == xme_hal_sharedPtr_getReferenceCount(dataHandle));

		XME_TEST(dataHandle == xme_hal_sharedPtr_retain(dataHandle));
		XME_TEST(3 == xme_hal_sharedPtr_getReferenceCount(dataHandle));

		xme_hal_sharedPtr_destroy(dataHandle);
		xme_hal_sharedPtr_destroy(dataHandle);
		xme_hal_sharedPtr_destroy(dataHandle);
		
		XME_TEST(XME_HAL_SHAREDPTR_INVALID_POINTER == xme_hal_sharedPtr_getReferenceCount(dataHandle));
	}

	XME_TESTSUITE_END();
}

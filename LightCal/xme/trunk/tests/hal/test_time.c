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
 *         Time testsuite.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "tests/hal/test_time.h"

#include "tests/tests.h"

#include "xme/hal/sleep.h"
#include "xme/hal/time.h"

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
int
xme_tests_hal_time()
{
	xme_hal_time_handle_t th1, th2;
	xme_hal_time_interval_t iv;
	int i;

	// On some platforms, sleeping will wait a shorter time than specified
	// because of low system tick resolution. The minOffset and maxOffset
	// variables try to define some reasonable values to compensate for this.
	const unsigned int intervalsMs[] = {10, 25, 100, 1000};
	const int minOffset = -10;
	const int maxOffset = 20;

	XME_TESTSUITE_BEGIN();

	// This should (probably) not raise a fatal error.
	// If it will ever do, handle the error or remove this test.
	xme_hal_time_destroyHandle(0);
	xme_hal_time_destroyHandle(1234);

	for (i=0; i<sizeof(intervalsMs)/sizeof(unsigned int); i++)
	{
		th1 = xme_hal_time_getCurrentTime();
		xme_hal_sleep_sleep(intervalsMs[i]);
		th2 = xme_hal_time_getCurrentTime();
		XME_TEST(th2 - th1 > intervalsMs[i] + minOffset);

		XME_TEST(xme_hal_time_compareTime(th1, th2) < 0);

		iv = xme_hal_time_getIntervalBetween(th1, th2);
		XME_TEST(iv >= intervalsMs[i]+minOffset && iv <= intervalsMs[i]+maxOffset);
		printf("xme_hal_time_getIntervalBetween(): %dms <= %dms <= %dms\n", intervalsMs[i]+minOffset, iv, intervalsMs[i]+maxOffset);

		xme_hal_time_destroyHandle(th1);
		xme_hal_time_destroyHandle(th2);

		th1 = xme_hal_time_getCurrentTime();
		xme_hal_sleep_sleep(intervalsMs[i]);
		iv = xme_hal_time_getInterval(&th1, true);
		XME_TEST(iv >= intervalsMs[i]+minOffset && iv <= intervalsMs[i]+maxOffset);
		printf("xme_hal_time_getInterval() w/ reset: %dms <= %dms <= %dms\n", intervalsMs[i]+minOffset, iv, intervalsMs[i]+maxOffset);

		xme_hal_time_destroyHandle(th1);

		th1 = xme_hal_time_getCurrentTime();
		xme_hal_sleep_sleep(intervalsMs[i]);
		xme_hal_time_getInterval(&th1, false);
		xme_hal_sleep_sleep(intervalsMs[i]);
		iv = xme_hal_time_getInterval(&th1, false);
		XME_TEST(iv >= 2*(intervalsMs[i]+minOffset) && iv <= 2*(intervalsMs[i]+maxOffset));
		printf("xme_hal_time_getInterval() w/o reset: %dms <= %dms <= %dms\n", 2*(intervalsMs[i]+minOffset), iv, 2*(intervalsMs[i]+maxOffset));

		xme_hal_time_destroyHandle(th1);
	}

	XME_TESTSUITE_END();
}

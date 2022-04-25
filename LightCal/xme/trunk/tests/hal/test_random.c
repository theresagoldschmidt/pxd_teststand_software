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
 *         Random number generator testsuite.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "tests/hal/test_random.h"

#include "tests/tests.h"

#include "xme/hal/random.h"
#include "xme/hal/mem.h"

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
int
xme_tests_hal_random()
{
	uint32_t i;

	XME_TESTSUITE_BEGIN();

	// According to C standard library, RAND_MAX must be at least 32767.
	// We should stick to this convention.
	XME_TEST(XME_HAL_RANDOM_RAND_MAX >= 32767);

	// Test range and distribution of xme_hal_random_rand()
	{
		double sum = 0;
		uint16_t distribution[XME_HAL_RANDOM_RAND_MAX+1];

		// Zero memory
		for (i=0; i<=XME_HAL_RANDOM_RAND_MAX; i++)
		{
			distribution[i] = 0;
		}

		for (i=0; i<=16*65536; i++)
		{
			uint16_t r = xme_hal_random_rand();

			// Prevent overflow
			if (distribution[r] < 0xFFFF)
			{
				distribution[r]++;
			}

			sum += r;
		}

		for (i=0; i<=XME_HAL_RANDOM_RAND_MAX; i++)
		{
			// In a truly uniform distribution, each item would be incremented
			// (16*65536)/(XME_HAL_RANDOM_RAND_MAX+1) times. These are safety bounds
			// that should always be true, given a somehow uniform distribution.
			XME_TEST(distribution[i] >= (16*65536)/(XME_HAL_RANDOM_RAND_MAX+1) / 3);
			XME_TEST(distribution[i] <= (16*65536)/(XME_HAL_RANDOM_RAND_MAX+1) * 3);
		}

		// Check whether mean over all random number
		// is almost the mean of the interval
		{
			double mean = sum / XME_HAL_RANDOM_RAND_MAX / 0x100000;
			XME_TEST(0.499 < mean && mean < 0.501);
		}
	}

	// Test range and distribution of xme_hal_random_rand_range()
	{
		double biasedSum = 0;
		uint16_t distribution[0xFFFF+1];

		uint16_t min = xme_hal_random_rand();
		uint16_t max = xme_hal_random_rand();

		// Swap min and max if max < min
		if (max < min)
		{
			uint16_t temp = min;
			min = max;
			max = temp;
		}

		// Zero memory
		for (i=0; i<=0xFFFF; i++)
		{
			distribution[i] = 0;
		}

		for (i=0; i<=16*65536; i++)
		{
			uint16_t r = xme_hal_random_randRange(min, max);

			// Prevent overflow
			if (distribution[r] < 0xFFFF)
			{
				distribution[r]++;
			}

			biasedSum += r - min;
		}

		for (i=0; i<=0xFFFF; i++)
		{
			if (i < min || i > max)
			{
				XME_TEST(0 == distribution[i]);
			}
			else
			{
				// In a truly uniform distribution, each item would be incremented
				// (16*65536)/(max-min+1) times. These are safety bounds that should
				// always be true, given a somehow uniform distribution.
				XME_TEST(distribution[i] >= (16*65536)/(max-min+1) / 3);
				XME_TEST(distribution[i] <= (16*65536)/(max-min+1) * 3);
			}
		}

		// Check whether mean over all random number
		// is almost the mean of the interval
		{
			double mean = biasedSum / (max-min) / 0x100000;
			XME_TEST(0.499 < mean && mean < 0.501);
		}
	}

	// Test xme_hal_random_rand_range() with min == max
	{
		for (i=0; i<=0xFFFF; i++)
		{
			XME_TEST(i == xme_hal_random_randRange(i, i));
		}
	}

	XME_TESTSUITE_END();
}

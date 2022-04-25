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
 *         Math testsuite.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "tests/hal/test_math.h"

#include "tests/tests.h"
#include "xme/hal/math.h"

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
int
xme_tests_hal_math()
{
	XME_TESTSUITE_BEGIN();

	XME_TEST(XME_HAL_MATH_IS_POWER_OF_2( 0, 32) == 0);
	XME_TEST(XME_HAL_MATH_IS_POWER_OF_2( 1, 32) == 1);
	XME_TEST(XME_HAL_MATH_IS_POWER_OF_2( 2, 32) == 1);
	XME_TEST(XME_HAL_MATH_IS_POWER_OF_2( 3, 32) == 0);
	XME_TEST(XME_HAL_MATH_IS_POWER_OF_2( 4, 32) == 1);
	XME_TEST(XME_HAL_MATH_IS_POWER_OF_2( 5, 32) == 0);
	XME_TEST(XME_HAL_MATH_IS_POWER_OF_2( 6, 32) == 0);
	XME_TEST(XME_HAL_MATH_IS_POWER_OF_2( 8, 32) == 1);
	XME_TEST(XME_HAL_MATH_IS_POWER_OF_2(12, 32) == 0);
	XME_TEST(XME_HAL_MATH_IS_POWER_OF_2(16, 32) == 1);

	XME_TEST(XME_HAL_MATH_MIN(0, 0) == 0);
	XME_TEST(XME_HAL_MATH_MIN(1, 1) == 1);
	XME_TEST(XME_HAL_MATH_MIN(1, 2) == 1);
	XME_TEST(XME_HAL_MATH_MIN(2, 1) == 1);
	XME_TEST(XME_HAL_MATH_MIN(-1, 2) == -1);
	XME_TEST(XME_HAL_MATH_MIN(1.5, 2.3) == 1.5);
	XME_TEST(XME_HAL_MATH_MIN(-1.5, 2.3) == -1.5);
	XME_TEST(2 + XME_HAL_MATH_MIN(3, 4) == 5);
	XME_TEST(XME_HAL_MATH_MIN(3, 4) + 5 == 8);
	XME_TEST(XME_HAL_MATH_MIN(-1.5, XME_HAL_MATH_MIN(-1.6, 2.3)) == -1.6);

	XME_TEST(XME_HAL_MATH_MAX(0, 0) == 0);
	XME_TEST(XME_HAL_MATH_MAX(1, 1) == 1);
	XME_TEST(XME_HAL_MATH_MAX(1, 2) == 2);
	XME_TEST(XME_HAL_MATH_MAX(2, 1) == 2);
	XME_TEST(XME_HAL_MATH_MAX(-1, 2) == 2);
	XME_TEST(XME_HAL_MATH_MAX(1.5, 2.3) == 2.3);
	XME_TEST(XME_HAL_MATH_MAX(-1.5, 2.3) == 2.3);
	XME_TEST(2 + XME_HAL_MATH_MAX(3, 4) == 6);
	XME_TEST(XME_HAL_MATH_MAX(3, 4) + 5 == 9);
	XME_TEST(XME_HAL_MATH_MAX(-1.5, XME_HAL_MATH_MAX(1.6, 2.3)) == 2.3);

	XME_TEST(xme_hal_math_ceilPowerOfTwo(0) == 0);
	XME_TEST(xme_hal_math_ceilPowerOfTwo(1) == 1);
	XME_TEST(xme_hal_math_ceilPowerOfTwo(2) == 2);
	XME_TEST(xme_hal_math_ceilPowerOfTwo(3) == 4);
	XME_TEST(xme_hal_math_ceilPowerOfTwo(4) == 4);
	XME_TEST(xme_hal_math_ceilPowerOfTwo(5) == 8);
	XME_TEST(xme_hal_math_ceilPowerOfTwo(7) == 8);
	XME_TEST(xme_hal_math_ceilPowerOfTwo(8) == 8);
	XME_TEST(xme_hal_math_ceilPowerOfTwo(1025) == 2048);
	XME_TEST(xme_hal_math_ceilPowerOfTwo(16384) == 16384);
	XME_TEST(xme_hal_math_ceilPowerOfTwo(262143) == 262144);
	XME_TEST(xme_hal_math_ceilPowerOfTwo(1073741825) == 2147483648);
	XME_TEST(xme_hal_math_ceilPowerOfTwo(2147483647) == 2147483648);
	XME_TEST(xme_hal_math_ceilPowerOfTwo(2147483648) == 2147483648);
	XME_TEST(xme_hal_math_ceilPowerOfTwo(2147483649) == 0);

	XME_TEST(xme_hal_math_floorPowerOfTwo(0) == 0);
	XME_TEST(xme_hal_math_floorPowerOfTwo(1) == 1);
	XME_TEST(xme_hal_math_floorPowerOfTwo(2) == 2);
	XME_TEST(xme_hal_math_floorPowerOfTwo(3) == 2);
	XME_TEST(xme_hal_math_floorPowerOfTwo(4) == 4);
	XME_TEST(xme_hal_math_floorPowerOfTwo(5) == 4);
	XME_TEST(xme_hal_math_floorPowerOfTwo(7) == 4);
	XME_TEST(xme_hal_math_floorPowerOfTwo(8) == 8);
	XME_TEST(xme_hal_math_floorPowerOfTwo(1023) == 512);
	XME_TEST(xme_hal_math_floorPowerOfTwo(16384) == 16384);
	XME_TEST(xme_hal_math_floorPowerOfTwo(262145) == 262144);
	XME_TEST(xme_hal_math_floorPowerOfTwo(1073741825) == 1073741824);
	XME_TEST(xme_hal_math_floorPowerOfTwo(2147483647) == 1073741824);
	XME_TEST(xme_hal_math_floorPowerOfTwo(2147483648) == 2147483648);
	XME_TEST(xme_hal_math_floorPowerOfTwo(2147483649) == 2147483648);

	XME_TESTSUITE_END();
}

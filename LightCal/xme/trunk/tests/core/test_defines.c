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
 *         Defines testsuite.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "tests/core/test_defines.h"

#include "tests/tests.h"
#include "xme/core/defines.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define XME_ASSERT_SIDE_EFFECT_TEST(condition) \
	xme_assert_checkForSideEffects("XME_ASSERT_TEST", #condition, __FILE__, __LINE__, (condition) ? 0 : 0)

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
static
int s()
{
	return 2;
}

static
int s2()
{
	return 2;
}

static
int s2s()
{
	return 2;
}

static
int _s()
{
	return 2;
}

static
int _s2()
{
	return 2;
}

static
int _s2s()
{
	return 2;
}

static
int s_()
{
	return 2;
}

static
int s2_()
{
	return 2;
}

static
int s2s_()
{
	return 2;
}

int
xme_tests_core_defines_assert()
{
	XME_TESTSUITE_BEGIN();

#ifdef NDEBUG
	printf("Warning: xme_tests_core_defines_assert test suite will only work in debug builds!");
#else // #ifdef NDEBUG

	{
		int abc = 1;
		int def = 1;
		int ghi = 2;

		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(abc == def));
		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(abc <= ghi));
		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(abc < ghi));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(abc = def));
		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(XME_ASSERT_NO_SIDE_EFFECTS(abc = def)));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(   abc = def));
		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(   XME_ASSERT_NO_SIDE_EFFECTS(abc = def)));
		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(abc == 1));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(abc = 1));
		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(abc != 2));

		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(1 < 2));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 < s()));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 < s2()));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 < s2s()));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 < _s()));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 < _s2()));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 < _s2s()));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 < s_()));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 < s2_()));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 < s2s_()));

		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 < s ()));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 < s2 ()));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 < s2s ()));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 < _s ()));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 < _s2 ()));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 < _s2s ()));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 < s_ ()));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 < s2_ ()));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 < s2s_ ()));



		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(1+2==3));
		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(1 + 2==3));
		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(1 + 2 == 3));
		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(1+2 == 3));

		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(1+ +2==3));
		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(1 + +2==3));
		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(1 + +2 == 3));
		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(1+ +2 == 3));

		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1+2==abc++));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 + 2==abc++));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 + 2 == abc++));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1+2 == abc++));

		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1+2==++abc));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 + 2==++abc));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 + 2 == ++abc));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1+2 == ++abc));

		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(abc++));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1+abc++));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 + abc++));

		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(++abc));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1+ ++abc));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 + ++abc));



		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(1-2==3));
		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(1 - 2==3));
		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(1 - 2 == 3));
		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(1-2 == 3));

		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(1- -2==3));
		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(1 - -2==3));
		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(1 - -2 == 3));
		XME_TEST(0 == XME_ASSERT_SIDE_EFFECT_TEST(1- -2 == 3));

		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1-2==abc--));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 - 2==abc--));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 - 2 == abc--));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1-2 == abc--));

		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1-2==--abc));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 - 2==--abc));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 - 2 == --abc));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1-2 == --abc));

		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(abc--));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1+abc--));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 + abc--));

		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(--abc));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1- --abc));
		XME_TEST(0 != XME_ASSERT_SIDE_EFFECT_TEST(1 - --abc));
	}

#endif // #ifdef NDEBUG

	XME_TESTSUITE_END();
}

int
xme_tests_core_defines()
{
	XME_TESTSUITE_BEGIN();
	XME_TESTSUITE(xme_tests_core_defines_assert(), "xme_tests_core_defines_assert testsuite failed!");
	XME_TESTSUITE_END();
}

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
 *         Testsuite defines.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdio.h>
#define WIN32 1
/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define XME_TESTSUITE_BEGIN() \
	{ \
		unsigned int tests_count = 0; \
		unsigned int tests_failed = 0; \
		const char* tests_name = __FUNCTION__; \
		printf("\n----- %s BEGIN -----\n", tests_name)

#define XME_TEST(condition) \
		do \
		{ \
			tests_count++; \
			if (!(condition)) \
			{ \
				fprintf(stderr, "Test failed: " # condition "\n"); \
				tests_failed++; \
			} \
		} while (0)

#define XME_TEST_MSG(condition, message, ...) \
		do \
		{ \
			tests_count++; \
			if (!(condition)) \
			{ \
				fprintf(stderr, message "\n", ##__VA_ARGS__); \
				tests_failed++; \
			} \
		} while (0)

#define XME_TEST_MSG_REC(condition, recovery, message, ...) \
		do \
		{ \
			tests_count++; \
			if (!(condition)) \
			{ \
				fprintf(stderr, message "\n", ##__VA_ARGS__); \
				tests_failed++; \
				recovery; \
			} \
		} while (0)

#define XME_TESTSUITE_STOP() \
	return tests_failed

#define XME_TESTSUITE(condition, message, ...) \
		do \
		{ \
			tests_count++; \
			if (condition) \
			{ \
				fprintf(stderr, message "\n", ##__VA_ARGS__); \
				tests_failed++; \
			} \
		} while (0)

#define XME_TESTSUITE_END() \
		printf("----- %s END (%d of %d succeeded, %d failed) -----\n\n", \
			tests_name, tests_count-tests_failed, tests_count, tests_failed); \
		XME_TESTSUITE_STOP(); \
	}

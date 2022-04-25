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
 *         Core initialization/finalization testsuite.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "tests/core/test_core.h"

#include "tests/tests.h"
#include "xme/core/core.h"

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
int
xme_tests_core_init()
{
	XME_TESTSUITE_BEGIN();
	XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_init());
	XME_TESTSUITE_END();
}

int
xme_tests_core_fini()
{
	XME_TESTSUITE_BEGIN();
	xme_core_fini();
	XME_TESTSUITE_END();
}

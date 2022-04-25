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
 *         Data centric communication testsuite.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "tests/core/test_dcc.h"

#include "tests/tests.h"
#include "xme/core/dcc.h"
#include "xme/core/md.h"
#include "xme/core/topic.h"

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
int
xme_tests_core_dcc_topicAnnouncement()
{
	XME_TESTSUITE_BEGIN();

	// TODO: Verify the number of announced topics after every test. See ticket #800

	{
		XME_TEST(XME_CORE_DCC_INVALID_PUBLICATION_HANDLE == xme_core_dcc_publishTopic(XME_CORE_TOPIC_INVALID_TOPIC, XME_CORE_MD_EMPTY_META_DATA, true, NULL));
		XME_TEST(XME_CORE_DCC_INVALID_PUBLICATION_HANDLE == xme_core_dcc_publishTopic(XME_CORE_TOPIC_LOGIN_REQUEST, 1234, true, NULL));

		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_dcc_unpublishTopic(0));
		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_dcc_unpublishTopic(1234));
	}

	{
		xme_core_dcc_publicationHandle_t h1, h2, h3, h4;

		XME_TEST(h1 = xme_core_dcc_publishTopic(XME_CORE_TOPIC_LOGIN_REQUEST, XME_CORE_MD_EMPTY_META_DATA, true, NULL));
		XME_TEST(h2 = xme_core_dcc_publishTopic(XME_CORE_TOPIC_LOGIN_RESPONSE, XME_CORE_MD_EMPTY_META_DATA, true, NULL));
		XME_TEST(h1 != 0); // Handles should be valid
		XME_TEST(h2 != 0); // Handles should be valid
		XME_TEST(h1 != h2); // Handles should be unique
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_dcc_unpublishTopic(h2));
		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_dcc_unpublishTopic(h2));
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_dcc_unpublishTopic(h1));
		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_dcc_unpublishTopic(h1));

		XME_TEST(h3 = xme_core_dcc_publishTopic(XME_CORE_TOPIC_LOGIN_REQUEST, XME_CORE_MD_EMPTY_META_DATA, true, NULL));
		XME_TEST(h4 = xme_core_dcc_publishTopic(XME_CORE_TOPIC_LOGIN_RESPONSE, XME_CORE_MD_EMPTY_META_DATA, true, NULL));
		XME_TEST(h3 != 0); // Handles should be valid
		XME_TEST(h4 != 0); // Handles should be valid
		XME_TEST(h3 != h4); // Handles should be unique
		XME_TEST(h1 == h3); // Handles should be reused
		XME_TEST(h2 == h4); // Handles should be reused
		XME_TEST(h1 != h4); // Handles should be unique
		XME_TEST(h2 != h3); // Handles should be unique
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_dcc_unpublishTopic(h3));
		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_dcc_unpublishTopic(h3)); // Handle should be invalidated
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_dcc_unpublishTopic(h4));
		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_dcc_unpublishTopic(h4)); // Handle should be invalidated
	}

	{
		xme_core_dcc_publicationHandle_t h[17];
		uint8_t i;

		for (i=0; i<17; i++)
		{
			XME_TEST(h[i] = xme_core_dcc_publishTopic(XME_CORE_TOPIC_LOGIN_REQUEST, XME_CORE_MD_EMPTY_META_DATA, true, NULL));
			XME_TEST(h[i] != 0); // Handles should be valid
		}
		for (i=0; i<17; i++)
		{
			XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_dcc_unpublishTopic(h[i]));
			XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_dcc_unpublishTopic(h[i]));
		}
	}

	{
		xme_core_dcc_publicationHandle_t h1, h2, h3, h4;

		XME_TEST(h1 = xme_core_dcc_publishTopic(XME_CORE_TOPIC_LOGIN_REQUEST, XME_CORE_MD_EMPTY_META_DATA, true, NULL));
		XME_TEST(h2 = xme_core_dcc_publishTopic(XME_CORE_TOPIC_LOGIN_RESPONSE, XME_CORE_MD_EMPTY_META_DATA, true, NULL));
		XME_TEST(h3 = xme_core_dcc_publishTopic(XME_CORE_TOPIC_LOGIN_REQUEST, XME_CORE_MD_EMPTY_META_DATA, true, NULL));
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_dcc_unpublishTopic(h2));
		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_dcc_unpublishTopic(h2)); // Handle should be invalidated
		XME_TEST(h4 = xme_core_dcc_publishTopic(XME_CORE_TOPIC_LOGIN_RESPONSE, XME_CORE_MD_EMPTY_META_DATA, true, NULL));
		XME_TEST(h2 == h4); // Handles should be reused
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_dcc_unpublishTopic(h1));
		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_dcc_unpublishTopic(h1)); // Handle should be invalidated
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_dcc_unpublishTopic(h4));
		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_dcc_unpublishTopic(h4)); // Handle should be invalidated
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_dcc_unpublishTopic(h3));
		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_dcc_unpublishTopic(h3)); // Handle should be invalidated
	}

	XME_TESTSUITE_END();
}

int
xme_tests_core_dcc()
{
	XME_TESTSUITE_BEGIN();
	XME_TESTSUITE(xme_tests_core_dcc_topicAnnouncement(), "xme_core_dcc_topicAnnouncement testsuite failed!");
	XME_TESTSUITE_END();
}

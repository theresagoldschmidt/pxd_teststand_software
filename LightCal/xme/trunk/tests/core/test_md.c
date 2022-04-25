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
 *         Meta data abstraction testsuite.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "tests/core/test_md.h"

#include "tests/tests.h"
#include "xme/core/md.h"

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
int xme_tests_core_md()
{
	XME_TESTSUITE_BEGIN();

	{
		XME_TEST(0 == xme_core_md_topicMetaDataPropertyCount(XME_CORE_MD_EMPTY_META_DATA));
		XME_TEST(0 == xme_core_md_topicMetaDataPropertyCount((xme_core_md_topicMetaDataHandle_t)1234));

		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_md_setTopicMetaDataString(XME_CORE_MD_EMPTY_META_DATA, "invalid-handle", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, "value"));
		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_md_setTopicMetaDataString((xme_core_md_topicMetaDataHandle_t)1234, "invalid-handle", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, "value"));

		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_md_setTopicMetaDataNumeric(XME_CORE_MD_EMPTY_META_DATA, "invalid-handle", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, -1234));
		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_md_setTopicMetaDataNumeric((xme_core_md_topicMetaDataHandle_t)1234, "invalid-handle", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, -1234));

		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_md_setTopicMetaDataUnsigned(XME_CORE_MD_EMPTY_META_DATA, "invalid-handle", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, 12345678));
		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_md_setTopicMetaDataUnsigned((xme_core_md_topicMetaDataHandle_t)1234, "invalid-handle", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, 12345678));

		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_md_setTopicMetaDataDecimal(XME_CORE_MD_EMPTY_META_DATA, "invalid-handle", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, -12.345678));
		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_md_setTopicMetaDataDecimal((xme_core_md_topicMetaDataHandle_t)1234, "invalid-handle", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, -12.345678));

		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_md_destroyTopicMetaData(XME_CORE_MD_EMPTY_META_DATA));
		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_md_destroyTopicMetaData((xme_core_md_topicMetaDataHandle_t)1234));
	}

	{
		xme_core_md_topicMetaDataHandle_t md;

		XME_TEST(XME_CORE_MD_EMPTY_META_DATA != (md = xme_core_md_createTopicMetaData()));
		XME_TEST(0 == xme_core_md_topicMetaDataPropertyCount(md));
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_md_setTopicMetaDataString(md, "location", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, "room_one"));
		XME_TEST(1 == xme_core_md_topicMetaDataPropertyCount(md));
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_md_setTopicMetaDataNumeric(md, "tolerance", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, -1234));
		XME_TEST(2 == xme_core_md_topicMetaDataPropertyCount(md));
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_md_setTopicMetaDataUnsigned(md, "serial", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, 12345678));
		XME_TEST(3 == xme_core_md_topicMetaDataPropertyCount(md));
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_md_setTopicMetaDataDecimal(md, "offset", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, -12.345678));
		XME_TEST(4 == xme_core_md_topicMetaDataPropertyCount(md));

		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_md_setTopicMetaDataString(md, "location", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, "room_two"));
		XME_TEST(4 == xme_core_md_topicMetaDataPropertyCount(md));

		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_md_setTopicMetaDataNumeric(md, "tolerance", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, -2345));
		XME_TEST(4 == xme_core_md_topicMetaDataPropertyCount(md));

		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_md_setTopicMetaDataUnsigned(md, "serial", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, 23456789));
		XME_TEST(4 == xme_core_md_topicMetaDataPropertyCount(md));

		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_md_setTopicMetaDataDecimal(md, "offset", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, -23.456789));
		XME_TEST(4 == xme_core_md_topicMetaDataPropertyCount(md));

		XME_TEST(XME_CORE_STATUS_NO_SUCH_VALUE == xme_core_md_removeTopicMetaData(md, "invalid"));
		XME_TEST(4 == xme_core_md_topicMetaDataPropertyCount(md));

		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_md_removeTopicMetaData(md, "location"));
		XME_TEST(3 == xme_core_md_topicMetaDataPropertyCount(md));

		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_md_removeTopicMetaData(md, "tolerance"));
		XME_TEST(2 == xme_core_md_topicMetaDataPropertyCount(md));

		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_md_removeTopicMetaData(md, "serial"));
		XME_TEST(1 == xme_core_md_topicMetaDataPropertyCount(md));

		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_md_removeTopicMetaData(md, "offset"));
		XME_TEST(0 == xme_core_md_topicMetaDataPropertyCount(md));

		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_md_destroyTopicMetaData(md));

		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_md_setTopicMetaDataString(md, "use-after-destroy", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, "value"));
	}

	{
		xme_core_md_topicMetaDataHandle_t md = xme_core_md_createTopicMetaData();

		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_md_setTopicMetaDataString(md, "location", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, "room_one"));
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_md_setTopicMetaDataNumeric(md, "tolerance", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, -1234));
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_md_setTopicMetaDataUnsigned(md, "serial", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, 12345678));
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_md_setTopicMetaDataDecimal(md, "offset", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, -12.345678));

		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_md_destroyTopicMetaData(md));
		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_md_destroyTopicMetaData(md));

		XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == xme_core_md_setTopicMetaDataString(md, "use-after-destroy", XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS, "value"));
	}

	XME_TESTSUITE_END();
}

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
 *         Directory testsuite.
 *
 * \author
 *         Gerd Kainz <kainz@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "tests/core/test_directory.h"

#include "tests/tests.h"
#include "xme/core/component.h"
#include "xme/core/directory.h"
#include "xme/core/md.h"
#include "xme/core/node.h"
#include "xme/core/topic.h"

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
int
xme_tests_core_directory()
{
	
	XME_TESTSUITE_BEGIN();
	{
#if 0
		#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
			xme_core_directory_neighbor_t neighbors[1];

			// Add nodes
			XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_directory_addNode((xme_core_node_nodeId_t)11));
			XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_directory_addNode((xme_core_node_nodeId_t)12));

			// Update neigbors of nodes
			neighbors[0].sendInterface = (xme_core_interface_interfaceId_t)1;
			neighbors[0].node = (xme_core_node_nodeId_t)12;
			neighbors[0].receiveInterface = (xme_core_interface_interfaceId_t)1;
			XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_directory_updateNeigborhood((xme_core_node_nodeId_t)11,neighbors, 1));
			neighbors[0].sendInterface = (xme_core_interface_interfaceId_t)1;
			neighbors[0].node = (xme_core_node_nodeId_t)11;
			neighbors[0].receiveInterface = (xme_core_interface_interfaceId_t)1;
			XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_directory_updateNeigborhood((xme_core_node_nodeId_t)12,neighbors, 1));

			// Add publications and subscriptions
			XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_directory_addPublication((xme_core_node_nodeId_t)11, (xme_core_component_t)1, (xme_core_component_port_t)1, (xme_core_topic_t)((int)XME_CORE_TOPIC_USER + 22), XME_CORE_MD_EMPTY_META_DATA));
			XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_directory_addSubscription((xme_core_node_nodeId_t)12, (xme_core_component_t)1, (xme_core_component_port_t)1, (xme_core_topic_t)((int)XME_CORE_TOPIC_USER + 22), XME_CORE_MD_EMPTY_META_DATA));
			XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_directory_addPublication((xme_core_node_nodeId_t)11, (xme_core_component_t)1, (xme_core_component_port_t)2, (xme_core_topic_t)((int)XME_CORE_TOPIC_USER + 22), XME_CORE_MD_EMPTY_META_DATA));
			XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_directory_addSubscription((xme_core_node_nodeId_t)12, (xme_core_component_t)1, (xme_core_component_port_t)2, (xme_core_topic_t)((int)XME_CORE_TOPIC_USER + 22), XME_CORE_MD_EMPTY_META_DATA));

			// Add request handler and requests
			XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_directory_addRequestHandler((xme_core_node_nodeId_t)12, (xme_core_component_t)2, (xme_core_component_port_t)1, (xme_core_component_port_t)2, (xme_core_topic_t)((int)XME_CORE_TOPIC_USER + 23), XME_CORE_MD_EMPTY_META_DATA, (xme_core_topic_t)((int)XME_CORE_TOPIC_USER + 24), XME_CORE_MD_EMPTY_META_DATA));
			XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_directory_addRequest((xme_core_node_nodeId_t)11, (xme_core_component_t)2, (xme_core_component_port_t)1, (xme_core_component_port_t)2, (xme_core_topic_t)((int)XME_CORE_TOPIC_USER + 23), XME_CORE_MD_EMPTY_META_DATA));
			XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_directory_addRequest((xme_core_node_nodeId_t)11, (xme_core_component_t)2, (xme_core_component_port_t)3, (xme_core_component_port_t)4, (xme_core_topic_t)((int)XME_CORE_TOPIC_USER + 23), XME_CORE_MD_EMPTY_META_DATA));
		#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER
#endif
	}
	XME_TESTSUITE_END();
}

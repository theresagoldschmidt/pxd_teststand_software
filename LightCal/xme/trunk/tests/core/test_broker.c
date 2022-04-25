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
 *         Broker testsuite.
 *
 * \author
 *         Stephan Sommer <sommer@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "tests/core/test_broker.h"

#include "tests/tests.h"
#include "xme/core/broker.h"
#include "xme/core/dcc.h"
#include "xme/core/md.h"
#include "xme/core/topic.h"
#include "xme/core/routingTable.h"
#include "xme/hal/sharedPtr.h"

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
uint8_t userData = 1;
uint8_t *big_data;
uint8_t callbackExecuted = 0;
uint8_t callback2Executed = 0;
uint8_t callbackForTopic2Executed = 0;

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
void
dcc_receiveTopicCallback2(xme_hal_sharedPtr_t dataHandle, void* userData)
{
	callback2Executed++;
//	printf("callback called by broker\n");
}

void
dcc_receiveTopicCallback(xme_hal_sharedPtr_t dataHandle, void* userData)
{
	callbackExecuted++;
//	printf("callback called by broker\n");
}

void
dcc_receiveTopicCallbackForTopic2(xme_hal_sharedPtr_t dataHandle, void* userData)
{
	callbackForTopic2Executed++;
}

bool
xme_tests_core_broker_createSubscriptionAndAddRoute
(
	xme_core_dcc_subscriptionHandle_t *inputTopicHandle, 
	xme_core_topic_t myTopic,
	xme_core_md_topicMetaDataHandle_t myMetaData, 
	xme_core_dataChannel_t dataChannel,
	xme_core_dcc_receiveTopicCallback_t dcc_receiveTopicCallback,
	uint8_t *userData
)
{
	XME_TESTSUITE_BEGIN();
	{
		XME_TEST(XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE != (*inputTopicHandle = xme_core_dcc_subscribeTopic(myTopic, myMetaData, true, dcc_receiveTopicCallback, userData)));

		XME_TEST(XME_CORE_STATUS_SUCCESS == 
			xme_core_routingTable_addLocalDestinationRoute
				(
				dataChannel, 
				xme_core_dcc_getComponentFromSubscriptionHandle(*inputTopicHandle), 
				xme_core_dcc_getPortFromSubscriptionHandle(*inputTopicHandle)
				)
			);
		}
	XME_TESTSUITE_END();
}

bool
xme_tests_core_broker_createTopicAndAddRoute
(
	xme_core_dcc_publicationHandle_t *outputTopicHandle, 
	xme_core_topic_t myTopic, 
	xme_core_md_topicMetaDataHandle_t myMetaData, 
	xme_core_dataChannel_t dataChannel
)
{
	// Publish TOPIC and prepare Routing to deliver data

	XME_TESTSUITE_BEGIN();
	{
		XME_TEST(XME_CORE_DCC_INVALID_PUBLICATION_HANDLE != (*outputTopicHandle = xme_core_dcc_publishTopic(myTopic, myMetaData, true, NULL)));
	
		XME_TEST(XME_CORE_STATUS_SUCCESS == 
			xme_core_routingTable_addLocalSourceRoute
				(
				dataChannel, 
				xme_core_dcc_getComponentFromPublicationHandle(*outputTopicHandle), 
				xme_core_dcc_getPortFromPublicationHandle(*outputTopicHandle)
				)
			);
		
	}
	XME_TESTSUITE_END();
	
}

bool
xme_tests_core_broker_unsubscribeAndRemoveRoute
(
	xme_core_dcc_subscriptionHandle_t inputTopicHandle,
	xme_core_dataChannel_t dataChannel
)
{
	XME_TESTSUITE_BEGIN();
	{
		// remove route
		XME_TEST(XME_CORE_STATUS_SUCCESS == 
			xme_core_routingTable_removeLocalDestinationRoute
				(
					dataChannel, 
					xme_core_dcc_getComponentFromSubscriptionHandle(inputTopicHandle), 
					xme_core_dcc_getPortFromSubscriptionHandle(inputTopicHandle)
				)
			);
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_dcc_unsubscribeTopic(inputTopicHandle));
	}
	XME_TESTSUITE_END();
}

int
xme_tests_core_broker()
{
	
	XME_TESTSUITE_BEGIN();
	{
		xme_core_topic_t myTopic1;
		xme_core_topic_t myTopic2;
		xme_core_topic_t myTopic3;

		xme_core_dcc_publicationHandle_t outputTopicHandle1;
		xme_core_dcc_publicationHandle_t outputTopicHandle2;
		xme_core_dcc_publicationHandle_t outputTopicHandle3;

		xme_core_dcc_subscriptionHandle_t inputTopicHandle1;
		xme_core_dcc_subscriptionHandle_t inputTopicHandle2;
		xme_core_dcc_subscriptionHandle_t inputTopicHandle3;

		xme_core_dcc_subscriptionHandle_t inputTopicHandleTranslation;

		xme_core_md_topicMetaDataHandle_t myMetaData;

		uint8_t data[] = "123456";
		
		myTopic1 = XME_CORE_TOPIC_USER+100;
		myTopic2 = XME_CORE_TOPIC_USER+101;
		myTopic3 = XME_CORE_TOPIC_USER+102;
		
		myMetaData = xme_core_md_createTopicMetaData();



// PUBLICATIONS
	
		// Publish TOPIC1; 
		xme_tests_core_broker_createTopicAndAddRoute(&outputTopicHandle1, myTopic1, myMetaData, TOPIC1_DATA_CHANNEL);
		// Publish TOPIC2; 
		 xme_tests_core_broker_createTopicAndAddRoute(&outputTopicHandle2, myTopic2, myMetaData, TOPIC2_DATA_CHANNEL);
		// Publish TOPIC3; 
		 xme_tests_core_broker_createTopicAndAddRoute(&outputTopicHandle3, myTopic3, myMetaData, TOPIC3_DATA_CHANNEL);
	

// SUBSCRIPTIONS
	
		// Subscribe dcc_receiveTopicCallback to Topic1; 
		xme_tests_core_broker_createSubscriptionAndAddRoute(&inputTopicHandle1, myTopic1, myMetaData, TOPIC1_DATA_CHANNEL, dcc_receiveTopicCallback, &userData);
		// Subscribe dcc_receiveTopicCallback2 to Topic1; 
		xme_tests_core_broker_createSubscriptionAndAddRoute(&inputTopicHandle2, myTopic1, myMetaData, TOPIC1_DATA_CHANNEL, dcc_receiveTopicCallback2, &userData);

		
// SEND DATA TO TOPICS

		// send topic data
		XME_TEST(XME_CORE_STATUS_SUCCESS == (xme_core_dcc_sendTopicData(outputTopicHandle1,(void*)data, sizeof(data))));

		// check if data is received
		XME_TEST(1==callbackExecuted);
		XME_TEST(1==callback2Executed);

		// send second topic data
		big_data = (uint8_t*)xme_hal_mem_alloc(10024);
		XME_TEST(XME_CORE_STATUS_SUCCESS == (xme_core_dcc_sendTopicData(outputTopicHandle1,big_data, sizeof(big_data))));
		xme_hal_mem_free(big_data);

		// test if data is received
		XME_TEST(2==callbackExecuted);
		XME_TEST(2==callback2Executed);


// REMOVE SUBSCRIPTIONS

	// Unsubscribe topic2
		xme_tests_core_broker_unsubscribeAndRemoveRoute(inputTopicHandle2, TOPIC1_DATA_CHANNEL);


// SEND DATA TO TOPICS

		// send data
		big_data = (uint8_t*)xme_hal_mem_alloc(10024);
		XME_TEST(XME_CORE_STATUS_SUCCESS == (xme_core_dcc_sendTopicData(outputTopicHandle1,big_data, sizeof(big_data))));
		xme_hal_mem_free(big_data);
				
		// test if data is received
		XME_TEST(3==callbackExecuted);
		XME_TEST(2==callback2Executed);


// PUBLICATIONS for Second TOPIC:
		
		// Subscribe to data published to myTopic2
		xme_tests_core_broker_createSubscriptionAndAddRoute(&inputTopicHandle3, myTopic2, myMetaData, TOPIC2_DATA_CHANNEL, dcc_receiveTopicCallbackForTopic2, &userData);


// SEND DATA TO TOPICS

		// send data
		big_data = (uint8_t*)xme_hal_mem_alloc(1024);
		XME_TEST(XME_CORE_STATUS_SUCCESS == (xme_core_dcc_sendTopicData(outputTopicHandle2, big_data, sizeof(big_data))));
		xme_hal_mem_free(big_data);
				
		// test if data is received
		XME_TEST(1==callbackForTopic2Executed);

		
// TEST TRANSLATIONS:

		// Subscribe to data published to myTopic1
		xme_tests_core_broker_createSubscriptionAndAddRoute(&inputTopicHandleTranslation, myTopic1, myMetaData, TOPIC1_TR_DATA_CHANNEL, dcc_receiveTopicCallback2, &userData);
		XME_TEST(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addTranslationRoute(TOPIC1_DATA_CHANNEL, TOPIC1_TR_DATA_CHANNEL));


// SEND DATA TO TOPICS

		// send data
		big_data = (uint8_t*)xme_hal_mem_alloc(1024);
		XME_TEST(XME_CORE_STATUS_SUCCESS == (xme_core_dcc_sendTopicData(outputTopicHandle1,big_data, sizeof(big_data))));
		xme_hal_mem_free(big_data);
		
		// test if data is received
		XME_TEST(4==callbackExecuted);
		XME_TEST(3==callback2Executed);	
	}
	
	XME_TESTSUITE_END();
}

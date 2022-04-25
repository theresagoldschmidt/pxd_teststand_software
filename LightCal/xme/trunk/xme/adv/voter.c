/*
 * Copyright (c) 2011, fortiss GmbH.
 * All rights reserved.
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
 *         voter component. (Proof of concept - voters very application specific
          and should be generated. The following aspects have to be taken into
		  account: voting algorithm, data type, timeout, trigger-event, input
		  data structure, mapping of input data to node ids, ...)
 *
 * \author
 *         Dominik Sojer <sojer@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/

#include "voter.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/

XME_HAL_TABLE
(
	xme_adv_voter_votableData_t,
	xme_adv_voter_valueTable,
	3 // TODO: derive from tool. See ticket #835
);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/

xme_core_status_t 
xme_adv_voter_create(xme_adv_voter_configStruct_t* config)
{
	XME_HAL_TABLE_INIT( xme_adv_voter_valueTable );
	debugTableSort(); // TODO: delete


	// Create subscription
	config->subHandle = xme_core_dcc_subscribeTopic( 
		config->inputTopic, 
		XME_CORE_MD_EMPTY_META_DATA, 
		false,
		receive_value, 
		NULL 
	);

	if ( config->subHandle == XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Create publisher
	config->pubHandle = xme_core_dcc_publishTopic( 
		config->outputTopic, 
		XME_CORE_MD_EMPTY_META_DATA, 
		false,
		NULL
	);

	if ( config->pubHandle == XME_CORE_DCC_INVALID_PUBLICATION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	} 

	// Create publisher for error message topic
	config->errorHandle = xme_core_dcc_publishTopic( 
		ERROR_MESSAGE_TOPIC_ID, 
		XME_CORE_MD_EMPTY_META_DATA, 
		false,
		NULL
	);

	if ( config->errorHandle == XME_CORE_DCC_INVALID_PUBLICATION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	} 

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_adv_voter_activate(xme_adv_voter_configStruct_t* config)
{
	xme_hal_sched_addTask(config->startMs, config->periodMs, 0, vote, NULL);
	return XME_CORE_STATUS_SUCCESS;
}


void xme_adv_voter_deactivate(xme_adv_voter_configStruct_t* config)
{
}

void xme_adv_voter_destroy(xme_adv_voter_configStruct_t* config)
{
	if ( xme_core_dcc_unsubscribeTopic(config->subHandle) != XME_CORE_STATUS_SUCCESS )
	{
		XME_LOG(XME_LOG_WARNING, "Failure xme_core_dcc_unsubscribeTopic\n");
	}
	if ( xme_core_dcc_unpublishTopic(config->pubHandle) != XME_CORE_STATUS_SUCCESS )
	{
		XME_LOG(XME_LOG_WARNING, "Failure xme_core_dcc_unsubscribeTopic\n");
	}
	if ( xme_core_dcc_unpublishTopic(config->errorHandle) != XME_CORE_STATUS_SUCCESS )
	{
		XME_LOG(XME_LOG_WARNING, "Failure xme_core_dcc_unsubscribeTopic\n");
	}
}

void receive_value
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
)
{
	xme_hal_table_rowHandle_t handle;
	xme_adv_voter_votableData_t* inRow;

	xme_adv_voter_votableData_t* value;

	// TODO: derive typecast from tool. See ticket #835
	value = (xme_adv_voter_votableData_t*)xme_hal_sharedPtr_getPointer(dataHandle);
	
	handle = XME_HAL_TABLE_ADD_ITEM(xme_adv_voter_valueTable);
	inRow = XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_adv_voter_valueTable, handle );

	inRow->data = value->data;
	inRow->sourceNode = value->sourceNode;
	inRow->type = value->type;
}

void
vote(void* userData)
{
	xme_adv_voter_configStruct_t* config;
	int resultInt;
	void *resultBinary;
	
	config = (xme_adv_voter_configStruct_t*)userData;

	switch (config->dataType) 
	{
	case XME_ADV_VOTER_DATATYPE_INT:
		
		// init integer result
		resultInt = -1;
		
		switch (config->algorithm) 
		{
		case XME_ADV_VOTER_ALGORITHM_MEAN:
			resultInt = vote_meanInt(config);
			break;

		case XME_ADV_VOTER_ALGORITHM_MEDIAN:
			resultInt = vote_medianInt(config);
			break;

		default:
			XME_LOG(XME_LOG_FATAL, "algorithm of voter is not defined! \n");
		}

		// publish integer result
		xme_core_dcc_sendTopicData(config->pubHandle, &resultInt, sizeof(resultInt));
		break;
		
	case XME_ADV_VOTER_DATATYPE_BINARY:

		// init binary result
		resultBinary = (void *) -1;

		switch (config->algorithm)
		{
		default:
			XME_LOG(XME_LOG_FATAL, "algorithm of voter is not defined! \n");
		}

		// publish binary result
		xme_core_dcc_sendTopicData(config->pubHandle, resultBinary, sizeof(resultBinary));
		break;

	default:
		XME_LOG(XME_LOG_FATAL, "data type of voter is not defined! \n");
	}

	// publish result
	xme_core_dcc_sendTopicData(config->pubHandle, &resultInt, sizeof(resultInt)); // TODO: adapt data type
}

int vote_meanInt(void* userData)
{
	int result; 
	
	result = 0;

	XME_HAL_TABLE_ITERATE
	(
		xme_adv_voter_valueTable,
		xme_hal_table_rowHandle_t, rowHandle,
		xme_adv_voter_votableData_t, rowItem,
		{
			result += (int)rowItem->data;
		}
	);

	// clear value table
	XME_HAL_TABLE_FINI(xme_adv_voter_valueTable);
	XME_HAL_TABLE_INIT(xme_adv_voter_valueTable);

	return result;
}

int
vote_medianInt(void* userData)
{
	xme_adv_voter_configStruct_t* config;
	int result;
	
	result = 0;
	config = (xme_adv_voter_configStruct_t*)userData;

	// TODO: implement calculation of median. See ticket #836
	XME_HAL_TABLE_BUBBLESORT(xme_adv_voter_valueTable, xme_adv_voter_votableData_t, 0, int);

	// clear value table
	XME_HAL_TABLE_FINI(xme_adv_voter_valueTable);
	XME_HAL_TABLE_INIT(xme_adv_voter_valueTable);

	return result;
}

void debugTableSort()
{
	xme_adv_voter_votableData_t value[3];
	//xme_adv_voter_namedInt_t temp;
	//int i, j;
	//xme_adv_voter_namedInt_t *base1;
	//xme_adv_voter_namedInt_t* base2;
	//int offset = 0;
	//int *comp1;
	//int *comp2;
	xme_hal_table_rowHandle_t handle;
	xme_adv_voter_votableData_t* inRow;


	value[0].data = (int *)3;
	value[1].data = (int *)2;
	value[2].data = (int *)1;

	handle = XME_HAL_TABLE_ADD_ITEM(xme_adv_voter_valueTable);
	inRow = XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_adv_voter_valueTable, handle );
	inRow->data = value[0].data;

	handle = XME_HAL_TABLE_ADD_ITEM(xme_adv_voter_valueTable);
	inRow = XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_adv_voter_valueTable, handle );
	inRow->data = value[1].data;

	handle = XME_HAL_TABLE_ADD_ITEM(xme_adv_voter_valueTable);
	inRow = XME_HAL_TABLE_ITEM_FROM_HANDLE( xme_adv_voter_valueTable, handle );
	inRow->data = value[2].data;

		printf("TABLESORT TEST \n");
	printf("TABLESORT TEST \n");
	printf("TABLESORT TEST \n");
	printf("TABLESORT TEST \n");
	printf("TABLESORT TEST \n");
	printf("TABLESORT TEST \n");

	// debug output 1
	XME_HAL_TABLE_ITERATE
	(
		xme_adv_voter_valueTable,
		xme_hal_table_rowHandle_t, rowHandle,
		xme_adv_voter_votableData_t, rowItem,
		{
			printf("value %d \n", rowItem->data);
		}
	);

	XME_HAL_TABLE_BUBBLESORT(xme_adv_voter_valueTable, xme_adv_voter_votableData_t, 0, int);

	// debug output 2
	XME_HAL_TABLE_ITERATE
	(
		xme_adv_voter_valueTable,
		xme_hal_table_rowHandle_t, rowHandle,
		xme_adv_voter_votableData_t, rowItem,
		{
			printf("value %d \n", rowItem->data);
		}
	);
}
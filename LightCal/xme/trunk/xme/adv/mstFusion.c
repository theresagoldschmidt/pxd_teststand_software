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
 *         mstFusion component.
 *
 * \author
 *         Marcel Flesch <fleschmarcel@mytum.de>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/adv/mstFusion.h"



#define DEBUGOUTPUT 0

/******************************************************************************/
/***   Forwaerd declarations                                                ***/
/******************************************************************************/

void
xme_adv_mstFusion_receiveTempTopicCallback
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
);

void
xme_adv_mstFusion_receiveSystemStateTopicCallback
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
);

xme_core_status_t
xme_adv_mstFusion_createFusionLists
(
	void* userData, 
	xme_adv_mstManager_systemState_t* systemState,
	uint16_t mstNum
);


void
xme_adv_mstFusion_fusionTaskCallback
(
	void* userData
);

xme_core_status_t
xme_adv_mstFusion_fusionOverPosition
(
	void* userData
);


xme_core_status_t
xme_adv_mstFusion_sendTopicTempFusion
(
	void* userData
);


/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/

xme_core_status_t
xme_adv_mstFusion_create(xme_adv_mstFusion_configStruct_t* config)
{
	// TODO: Initialize component state
	config->currentFusionTaskCount = 0;
	config->flagDataComplete = 0;

	// get current componentID
	config->componentID = xme_core_resourceManager_getCurrentComponentId();
	
	// fusion for temperature data or pressure data?
	switch(config->listenTopicSensorType)
	{
	case XME_CORE_TOPIC_TEMPERATURE: 
		printf("mstFusion with component ID #%d: I'm responsable for mst temperature data\n", config->componentID);
		
		// publish topic XME_CORE_TOPIC_TEMPFUSION
		XME_CHECK
		(
			XME_CORE_DCC_INVALID_PUBLICATION_HANDLE !=
			(
				config->fusionTopicPublicationHandle = xme_core_dcc_publishTopic
				(
					config->publicationtopic = XME_CORE_TOPIC_TEMPFUSION,
					XME_CORE_MD_EMPTY_META_DATA,
					false,
					NULL
				)
			),
			XME_CORE_STATUS_OUT_OF_RESOURCES
		);
		// set mst type
		config->mstType = TEMPERATURE;
		break;

	case XME_CORE_TOPIC_PRESSURE: 
		printf("mstFusion with component ID #%d: I'm responsable for mst pressure data\n", config->componentID);

		// publish topic XME_CORE_TOPIC_PRESFUSION
		XME_CHECK
		(
			XME_CORE_DCC_INVALID_PUBLICATION_HANDLE !=
			(
				config->fusionTopicPublicationHandle = xme_core_dcc_publishTopic
				(
					config->publicationtopic = XME_CORE_TOPIC_PRESFUSION,
					XME_CORE_MD_EMPTY_META_DATA,
					false,
					NULL
				)
			),
			XME_CORE_STATUS_OUT_OF_RESOURCES
		);

		// set mst type
		config->mstType = PRESSURE;

		break;
	}

	// subscribe listenTopic XME_CORE_TOPIC_TEMPERATURE or XME_CORE_TOPIC_PRESSURE
	XME_CHECK
	(
		XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE !=
		(
			config->listenTopicSensorTypeHandle = xme_core_dcc_subscribeTopic
			(
				config->listenTopicSensorType,
				XME_CORE_MD_EMPTY_META_DATA,
				false,
				&xme_adv_mstFusion_receiveTempTopicCallback,
				config
			)
		), XME_CORE_STATUS_SUCCESS
	);
	
	// subscribe Topic XME_CORE_TOPIC_SYSTEM_STATE
	XME_CHECK
	(
		XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE !=
		(
			config->listenTopicSystemStateHandle = xme_core_dcc_subscribeTopic
			(
				XME_CORE_TOPIC_SYSTEM_STATE,
				XME_CORE_MD_EMPTY_META_DATA,
				false,
				&xme_adv_mstFusion_receiveSystemStateTopicCallback,
				config
			)
		), XME_CORE_STATUS_SUCCESS
	);


	// publish topic XME_CORE_TOPIC_SYSTEM_ERROR
	XME_CHECK
	(
		XME_CORE_DCC_INVALID_PUBLICATION_HANDLE !=
		(
			config->errorTopicPublicationHandle = xme_core_dcc_publishTopic
			(
				XME_CORE_TOPIC_SYSTEM_ERROR,
				XME_CORE_MD_EMPTY_META_DATA,
				false,
				NULL
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// create handle for critical section
	config->criticalSectionHandle = xme_hal_sync_createCriticalSection();

	// init task for fusionize data
	XME_CHECK
	(
	 	XME_HAL_SCHED_INVALID_TASK_HANDLE !=
		(
			config->fusionTaskHandle = xme_core_resourceManager_scheduleTask
			(
				XME_HAL_SCHED_TASK_INITIALLY_SUSPENDED,
				config->fusionTaskPeriode,
				XME_HAL_SCHED_PRIORITY_NORMAL,
				&xme_adv_mstFusion_fusionTaskCallback,
				config
			)
		), XME_CORE_STATUS_OUT_OF_RESOURCES
	);
	
	//// Check for errors
	//if (XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE == config->subscriptionHandle)
	//{
	//	// Unpublish topic since subscription does not work
	//	xme_core_dcc_unpublishTopic(config->publicationHandle);
	//	return XME_CORE_STATUS_INTERNAL_ERROR;
	//}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_adv_mstFusion_activate(xme_adv_mstFusion_configStruct_t* config)
{
	// Start Task
	XME_CHECK
	(
		XME_CORE_STATUS_SUCCESS == xme_hal_sched_setTaskExecutionState(config->fusionTaskHandle, true),
		XME_CORE_STATUS_INTERNAL_ERROR
	);

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_adv_mstFusion_deactivate(xme_adv_mstFusion_configStruct_t* config)
{

	// Example: Remove the task:
	xme_core_resourceManager_killTask(config->taskHandle);
}

void
xme_adv_mstFusion_destroy(xme_adv_mstFusion_configStruct_t* config)
{


	// Example: Unsubscribe the topic
	xme_core_dcc_unsubscribeTopic(config->listenTopicSensorTypeHandle);

	// Example: Unpublish the topic
	//xme_core_dcc_unpublishTopic(config->publicationHandle);
}


void
xme_adv_mstFusion_receiveTempTopicCallback
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
)
{
	uint16_t* data;	
	xme_adv_mstFusion_configStruct_t* config = (xme_adv_mstFusion_configStruct_t*)userData;
	uint16_t sensorCount = 0;
	uint16_t numAvailableData = 0;
	
	// get Topic Data from specific topic
	data = (uint16_t*)xme_hal_sharedPtr_getPointer(dataHandle);

#if DEBUGOUTPUT
	printf("mstFusion with component ID #%d: received <SensorType>Topic from installPositionInstance %d \n", config->componentID, *(uint16_t*)data);
#endif

	// enter critical section
	xme_hal_sync_enterCriticalSection(config->criticalSectionHandle);

	// add sensor data to the mstFusionSensorList
	// First Version: replace old data in the list
	XME_HAL_LINKEDLIST_ITERATE
	(
		config->mstfusionSensorList, 
		xme_adv_mstFusion_fusionSensorListItem_t, 
		item,
		{
			if ( item->installPositionInstance == *(uint16_t*)data )
			{
				// insert the first sensor
				item->dataAvailable = 1;
				item->value = data[1+sensorCount];
				item->variance = 100;
				sensorCount++;
			}
			// summ up numer of available data
			if (item->dataAvailable) numAvailableData++;
		}
	);
	
	// check, if all data has received
	if (config->mstfusionSensorList.count == numAvailableData) config->flagDataComplete=1;

	// leave critical section
	xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);
};


void
xme_adv_mstFusion_receiveSystemStateTopicCallback
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
)
{
	xme_adv_mstFusion_configStruct_t* config = (xme_adv_mstFusion_configStruct_t*)userData;
	xme_adv_mstManager_systemState_t* systemState;
	uint16_t topicBufferSize;
	uint16_t mstNum;
	xme_core_status_t status;

	// get Topic Data from 'systemState' topic
	systemState = (xme_adv_mstManager_systemState_t*)xme_hal_sharedPtr_getPointer(dataHandle);

	// check dataSize
	topicBufferSize = xme_hal_sharedPtr_getSize(dataHandle);

	// calculate number of mstSystems
	mstNum = topicBufferSize / sizeof(xme_adv_mstManager_systemState_t);

#if DEBUGOUTPUT
	printf("mstFusion with component ID #%d: received SystemState with %d systems\n", config->componentID, mstNum);
#endif

	// create fusionList
	status = xme_adv_mstFusion_createFusionLists(userData, systemState, mstNum);

#if DEBUGOUTPUT
	printf("mstFusion with component ID #%d: added %d sensor(s) to my mstFusionSensorList\n", config->componentID, config->mstfusionSensorList.count); 
#endif

};

xme_core_status_t
xme_adv_mstFusion_createFusionLists
(
	void* userData, 
	xme_adv_mstManager_systemState_t* systemState,
	uint16_t mstNum
)
{
	xme_adv_mstFusion_fusionSensorListItem_t* itemSensorList = NULL;
	xme_adv_mstFusion_fusionDataListItem_t* itemDataList = NULL;
	int i = 0;
	int sensorNum = 0;
	uint8_t isInList = 0;
	uint8_t isEqualCount = 0;

	// Retrieve configuration structure
	xme_adv_mstFusion_configStruct_t* config = (xme_adv_mstFusion_configStruct_t*)userData;

	// destroy old lists, if an old list exist
	if (NULL != config->mstfusionSensorList.head) XME_HAL_LINKEDLIST_FINI(config->mstfusionSensorList);
	if (NULL != config->mstfusionDataList.head) XME_HAL_LINKEDLIST_FINI(config->mstfusionDataList);

	// create singly linked lists for sensors and data
	XME_HAL_LINKEDLIST_INIT(config->mstfusionSensorList);
	XME_HAL_LINKEDLIST_INIT(config->mstfusionDataList);

	// enter critical section
	xme_hal_sync_enterCriticalSection(config->criticalSectionHandle);

	// allocate memory for every sensor in every mst system
	for (i=0; i < mstNum; i++)
	{
			//printf("\n start with installPositionInstance %d ", systemState[i].installPositionInstance);
			// sensorTypeID, interfaceTypeRef, assignedID, numChannels, installPositionInstanceID, installPositionRef
			// check every list element if it is the right mstType (e.g. Temperature or Pressure)
			if ( config->mstType == systemState[i].sensorType )
			{
				//printf("-> is a tempSensor\n");
				// ****************************
				// CREATE mstFusionSensorList
				// ****************************
				// for every sensor in this mst system do
				for (sensorNum=0; sensorNum < systemState[i].numChannels; sensorNum++)
				{
					// allocate memory for item
					XME_CHECK
					(
						NULL !=
						(
							itemSensorList = (xme_adv_mstFusion_fusionSensorListItem_t*)xme_hal_mem_alloc
							(
								sizeof(xme_adv_mstFusion_fusionSensorListItem_t)
							)
						),
						XME_CORE_STATUS_INTERNAL_ERROR
					);

					//read from topic "system state" and insert into item
					itemSensorList->installPositionInstance = systemState[i].installPositionInstance;
					itemSensorList->installPosition = systemState[i].installPosition;
					itemSensorList->dataAvailable = 0;
					itemSensorList->sensorInstance = systemState[i].installPositionInstance*10 + sensorNum;
					itemSensorList->value = 0;
					itemSensorList->variance = 0;

					// add item to mstSystemList
					XME_HAL_SINGLYLINKEDLIST_ADD_ITEM(config->mstfusionSensorList, itemSensorList);
					//printf("\nadded item to mstfusionList:\nPos: %d Inst.: %d SenInst. %d\n",itemSensorList->installPosition, itemSensorList->installPositionInstance, itemSensorList->sensorInstance);
				}

				// ****************************
				// CREATE mstFusionDataList
				// ****************************
				// if list is empty, allocate memory, insert installPosition of the current mstSystem
				// if not check, check the current installPosition in the list and insert the installPosition of the current mstSystem if it is not in the list
				//printf("instance with sensorNum %d, fusionList.count: %d\n", sensorNum, config->mstfusionDataList.count);
				if ( 0 == config->mstfusionDataList.count)
				{
					XME_CHECK
					(
						NULL !=
						(
							itemDataList = (xme_adv_mstFusion_fusionDataListItem_t*)xme_hal_mem_alloc
							(
								sizeof(xme_adv_mstFusion_fusionDataListItem_t)
							)
						),
						XME_CORE_STATUS_INTERNAL_ERROR
					);
					itemDataList->installPosition = systemState[i].installPosition;
					itemDataList->value = 0;
					itemDataList->variance = 0;
					itemDataList->dataAvailable = 0;
					// add item to fusionDataList
					XME_HAL_SINGLYLINKEDLIST_ADD_ITEM(config->mstfusionDataList, itemDataList);
					//printf("\nadded item to mstDataList:\nPos: %d Val.: %d Var. %d\n\n",itemDataList->installPosition, itemDataList->value, itemDataList->variance);
				}
				else	// there is at least one item in the fusionDataList
				{	
					isInList = 1;
					isEqualCount = 0;
					// check if this install position is allready in the list
					//printf("iterate fusionDataList, isEqualCount = %d\n", isEqualCount);
					XME_HAL_LINKEDLIST_ITERATE
					(
						config->mstfusionDataList, 
						xme_adv_mstFusion_fusionDataListItem_t, 
						itemDataList,
						{
							//printf("systemState Instanz: %d; systemState Pos: %d; item.pos %d", systemState[i].installPositionInstance, systemState[i].installPosition, itemDataList->installPosition);
							// if the installPosition of the current mstSystem is in List, increment isEqualCount
							if ( systemState[i].installPosition == itemDataList->installPosition ) 
							{
								isInList = 0;
								isEqualCount++;
							}
							//printf(" -> isEqualCount: %d\n", isEqualCount);
						}
					);
					// if installPosition was not in list, add it
					if ( 0 == isEqualCount )
					{
						// allocate memory for item
						XME_CHECK
						(
							NULL !=
							(
								itemDataList = (xme_adv_mstFusion_fusionDataListItem_t*)xme_hal_mem_alloc
								(
									sizeof(xme_adv_mstFusion_fusionDataListItem_t)
								)
							),
							XME_CORE_STATUS_INTERNAL_ERROR
						);
						itemDataList->installPosition = systemState[i].installPosition;
						itemDataList->value = 0;
						itemDataList->variance = 0;
						itemDataList->dataAvailable = 0;
						// add item to fusionDataList
						XME_HAL_SINGLYLINKEDLIST_ADD_ITEM(config->mstfusionDataList, itemDataList);
						//printf("added item to mstDataList:\nPos: %d Val.: %d Var. %d\n\n",itemDataList->installPosition, itemDataList->value, itemDataList->variance);
					}
				}
			}
		}

	// leave critical section
	xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);

	return XME_CORE_STATUS_SUCCESS;
};


void
xme_adv_mstFusion_fusionTaskCallback
(
	void* userData
)
{
	// Retrieve configuration structure
	xme_adv_mstFusion_configStruct_t* config = (xme_adv_mstFusion_configStruct_t*)userData;
	xme_core_status_t status = XME_CORE_STATUS_INTERNAL_ERROR;

	config->currentFusionTaskCount++;

	// if the fusion periode is full, fusionize the data
	if ( config->fusionTaskCount == config->currentFusionTaskCount )
	{

		// fusionize data, delete them afterwards and set the flag flagDataComplete and the corresponding dataAvailable flag in the sensor list
		status = xme_adv_mstFusion_fusionOverPosition(config);

		// send topic, delete data from data list
		//if (status) status = xme_adv_mstFusion_sendTopicTempFusion(config);
		xme_adv_mstFusion_sendTopicTempFusion(config);

		// set count
		config->currentFusionTaskCount = 0;
	}
};


xme_core_status_t
xme_adv_mstFusion_fusionOverPosition
(
	void* userData
)
{
	uint8_t currentDataAvailable = 0;

	// Retrieve configuration structure
	xme_adv_mstFusion_configStruct_t* config = (xme_adv_mstFusion_configStruct_t*)userData;

	// enter critical section
	xme_hal_sync_enterCriticalSection(config->criticalSectionHandle);

	// take every item of the sensor list, test if there are data available and add them to the fusionized data in the dataList
	XME_HAL_LINKEDLIST_ITERATE
	(
		config->mstfusionSensorList, 
		xme_adv_mstFusion_fusionSensorListItem_t, 
		itemSensorList,
		{			
			// test if on the current install position of the item from Sensorlist are data available in the dataList
			if ( itemSensorList->dataAvailable )
			{
				// find the corresponding install position in the dataList
				XME_HAL_LINKEDLIST_ITERATE
				(
					config->mstfusionDataList, 
					xme_adv_mstFusion_fusionDataListItem_t, 
					itemDataList,
					{
						// if the install position of the item from the sensor list matches the item of the dataList...
						if ( itemDataList->installPosition == itemSensorList->installPosition )
						{
							// check if there are data available on the current position of the item of the sensor list
							if ( 1 == itemDataList->dataAvailable ) currentDataAvailable = 1;
							else currentDataAvailable = 0;

							// if there are data available in the item of the dataList on the current position of the sensorList
							//		- fusionize the two tuples (value1 from sensorList with value2 from dataList with their according variance)
							//		- set the flag dataAvailable in the dataList (remove and add ??? )
							if ( currentDataAvailable )
							{
								// TODO: do the real fusion in a spezific function
								itemDataList->value = ( itemDataList->value + itemSensorList->value )/2;
								itemDataList->variance = ( itemDataList->variance + itemSensorList->variance )/3;
								itemDataList->dataAvailable = 1;
							}
							// if there are no data available in the item of the dataList on the current position of the sensorList
							//		- add the data and set the flag dataAvailable in the dataList (remove and add item???)
							else
							{
								itemDataList->value = itemSensorList->value;
								itemDataList->variance = itemSensorList->variance;
								itemDataList->dataAvailable = 1;
							}
						}
					}
				);
			}
			// reset the item in sensorList
			itemSensorList->value = 0;
			itemSensorList->variance = 0;
			itemSensorList->dataAvailable = 0;
		}
	);

	// leave critical section
	xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);

	// set flag dataComplete
	config->flagDataComplete = 0;
	
	return XME_CORE_STATUS_SUCCESS;
};


xme_core_status_t
xme_adv_mstFusion_sendTopicTempFusion
(
	void* userData
)
{
	void* topicData;
	uint16_t* copyTopicData;
	uint16_t sizeofTopicData = 0;
	
	// Retrieve configuration structure
	xme_adv_mstFusion_configStruct_t* config = (xme_adv_mstFusion_configStruct_t*)userData;

	
	// allocate memory for topicData
	sizeofTopicData = 3*sizeof(uint16_t);
	topicData = xme_hal_mem_alloc( sizeofTopicData );	

	// enter critical section
	xme_hal_sync_enterCriticalSection(config->criticalSectionHandle);

	// send fusionData per every install position
	XME_HAL_LINKEDLIST_ITERATE
	(
		config->mstfusionDataList, 
		xme_adv_mstFusion_fusionDataListItem_t, 
		itemDataList,
		{
			// add data from itemDataList to topicData
			copyTopicData = topicData;
			*copyTopicData = itemDataList->installPosition;
			copyTopicData++;
			*copyTopicData = itemDataList->value;
			copyTopicData++;
			*copyTopicData = itemDataList->variance;
						
			// send Topic 
			XME_CHECK
			(
				XME_CORE_STATUS_SUCCESS == 
				xme_core_dcc_sendTopicData(config->fusionTopicPublicationHandle, topicData, sizeofTopicData ),
				XME_CORE_STATUS_INTERNAL_ERROR
			);

			// clear data of item data list
			itemDataList->value = 0;
			itemDataList->variance = 0;
			itemDataList->dataAvailable = 0;
		}
	);

	// leave critical section
	xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);
	
	return XME_CORE_STATUS_SUCCESS;
};
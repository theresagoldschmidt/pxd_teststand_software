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
 *         Modbus Simulator component.
 *
 * \author
 *         Marcel Flesch <fleschmarcel@mytum.de>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/prim/modbusMaster.h"
#include "xme/prim/modbusSimulator.h"


// For Debug Mode
#define DEBUG 0


/******************************************************************************/
/***   Forward declarations                                                 ***/
/******************************************************************************/
/**
 * \brief  Callback function for processing Modbus requests.
 *
 * \see    xme_hal_sched_taskCallback_t
 */
void
xme_prim_modbusSimulator_taskCallback(void* userData);

// rr-SERVER
xme_core_rr_responseStatus_t
xme_prim_modbusSimulator_receiveRequestCallback
(
	xme_core_topic_t requestTopic,
	void* requestData,
	xme_core_rr_responseInstanceHandle_t mstResponseInstanceHandle,
	void* responseData,
	uint16_t* responseSize,
	xme_hal_time_interval_t responseTimeoutMs,
	void* userData
);

// modbusSimulator_create dynamic sensor list
xme_prim_modbusMaster_status_t
xme_prim_modbusSimulator_createDynamicSensorList
(
	void* userData
);


// modbusSimulator_create static sensor list
xme_prim_modbusMaster_status_t
xme_prim_modbusSimulator_createStaticSensorList
(
	void* userData
);

// modbusSimulator_sendModbusSimulatorRequest
xme_prim_modbusMaster_status_t
xme_prim_modbusSimulator_getGaussianRandomNumber
(
	void* userData,
	xme_prim_modbusSimulator_priorityQueueItem_t* item,
	void* responseData,
	double meanValue,
	double variance
);

xme_prim_modbusMaster_status_t
xme_prim_modbusSimulator_checkId247
(
	void* userData,
	xme_prim_modbusSimulator_priorityQueueItem_t* priorityQueueItem,
	void* responseData,
	uint16_t responseDataSize,
	uint16_t* copyResponseData
	
);

xme_prim_modbusMaster_status_t
xme_prim_modbusSimulator_prepareResponse
(
	void* userData,
	xme_prim_modbusSimulator_priorityQueueItem_t* priorityQueueItem,
	void* responseData,
	uint16_t* responseDataSize,
	uint16_t* responseDataField
);

xme_prim_modbusMaster_status_t
xme_prim_modbusSimulator_checkDynamicSensorList
(
	void* userData,
	xme_prim_modbusSimulator_priorityQueueItem_t* priorityQueueItem,
	xme_prim_modbusSimulator_dynamicSensorListItem_t* dynamicSensorListItem,
	void* responseData,
	uint16_t responseDataSize,
	uint16_t* responseDataField
);

xme_prim_modbusMaster_status_t
xme_prim_modbusSimulator_checkStaticSensorList
(
	void* userData,
	xme_prim_modbusSimulator_priorityQueueItem_t* priorityQueueItem,
	xme_prim_modbusSimulator_dynamicSensorListItem_t* dynamicSensorListItem,
	void* responseData,
	uint16_t responseDataSize,
	uint16_t* responseDataField
);

xme_prim_modbusMaster_status_t
xme_prim_modbusSimulator_sendDefaultResponse
(
	void* userData,
	xme_prim_modbusSimulator_priorityQueueItem_t* priorityQueueItem,
	void* responseData,
	uint16_t responseDataSize,
	uint16_t* responseDataField
);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_prim_modbusSimulator_create(xme_prim_modbusSimulator_configStruct_t* config)
{
	XME_ASSERT(NULL != config);
	
	// Initialize configuration structure
	config->criticalSectionHandle = XME_HAL_SYNC_INVALID_CRITICAL_SECTION_HANDLE;
	config->requestHandlerHandle = XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE;
	config->taskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;
	
	// create singly linked list for priorityQueue
	XME_HAL_LINKEDLIST_INIT(config->priorityQueue);

	// create handle for critical section
	config->criticalSectionHandle = xme_hal_sync_createCriticalSection();

	// publish request handler
	XME_CHECK
	(
		XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE !=
		(
			config->requestHandlerHandle = xme_core_rr_publishRequestHandler
			(
				XME_CORE_TOPIC_MODBUS_REQUEST,
				XME_CORE_MD_EMPTY_META_DATA,
				XME_CORE_TOPIC_MODBUS_RESPONSE,
				XME_CORE_MD_EMPTY_META_DATA,
				false,
				&xme_prim_modbusSimulator_receiveRequestCallback,
				config
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// create task xme_prim_modbusSimulator_taskCallback
	XME_CHECK
	(
	 	XME_HAL_SCHED_INVALID_TASK_HANDLE !=
		(
			// This task suspends itself if it is not used, hence we can set
			// the period to the minimum time to make sure that the task
			// executes as soon as possible after resume
			config->taskHandle = xme_core_resourceManager_scheduleTask
			(
				XME_HAL_SCHED_TASK_INITIALLY_SUSPENDED,
				1,
				XME_HAL_SCHED_PRIORITY_NORMAL,
				&xme_prim_modbusSimulator_taskCallback,
				config
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// get current componentID
	config->componentID = xme_core_resourceManager_getCurrentComponentId();
	printf("modbusSimulator with component ID #%d: I'm responsable for modbus simulation interface\n", config->componentID);

	xme_prim_modbusSimulator_createDynamicSensorList(config);
	xme_prim_modbusSimulator_createStaticSensorList(config);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_prim_modbusSimulator_activate(xme_prim_modbusSimulator_configStruct_t* config)
{
	// TODO: remove this, if the schedlunging problem in xme is solved...	
	xme_hal_sched_setTaskExecutionState(config->taskHandle, true);

	config->localTimeCount = 0;
	config->defaultListFlag = 1;
	// reset loginMode
	config->loginMode = 0;

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_prim_modbusSimulator_deactivate(xme_prim_modbusSimulator_configStruct_t* config)
{
	// TODO: Delete pending requests?

	xme_core_status_t result = xme_hal_sched_setTaskExecutionState(config->taskHandle, false);
	XME_ASSERT_NORVAL(XME_CORE_STATUS_SUCCESS == result);
}

void
xme_prim_modbusSimulator_destroy(xme_prim_modbusSimulator_configStruct_t* config)
{
	// TODO: Delete pending requests!
	xme_core_status_t result;

	// Clear the list
 	XME_HAL_LINKEDLIST_FINI(config->priorityQueue);

	// close task xme_prim_modbusSimulator_taskCallback
	XME_ASSERT_NORVAL(XME_CORE_STATUS_SUCCESS == xme_hal_sched_removeTask(config->taskHandle));
	config->taskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;

	// unpublish request handler
	result = xme_core_rr_unpublishRequestHandler(config->requestHandlerHandle);
	XME_ASSERT_NORVAL(XME_CORE_STATUS_SUCCESS == result);
	config->requestHandlerHandle = XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE;

	// destroy critical section
	xme_hal_sync_destroyCriticalSection(config->criticalSectionHandle);
	config->criticalSectionHandle = XME_HAL_SYNC_INVALID_CRITICAL_SECTION_HANDLE;
}



xme_core_rr_responseStatus_t 
xme_prim_modbusSimulator_receiveRequestCallback
(
	xme_core_topic_t requestTopic,
	void* requestData,
	xme_core_rr_responseInstanceHandle_t responseInstanceHandle,
	void* responseData,
	uint16_t* responseSize,
	xme_hal_time_interval_t responseTimeoutMs,
	void* userData
)
{
	xme_prim_modbusSimulator_configStruct_t* config = (xme_prim_modbusSimulator_configStruct_t*)userData;
	xme_prim_modbusSimulator_priorityQueueItem_t* priorityQueueItem;
	//xme_prim_modbusSimulator_defaultSensorListItem_t* defaultSensorListItem;
	//xme_prim_modbusSimulator_dynamicSensorListItem_t* dynamicSensorListItem;
	xme_prim_modbusMaster_requestData_t* copyRequestData;
	uint16_t countOfDynamicSensors = 0;
	uint8_t requestedSensorExistsFlag = 0;

#if DEBUG
	printf("modbusSimulator with component ID #%d: received request\n   modbusInterfaceType: %d; slaveID: %d \n", config->componentID, ((xme_prim_modbusMaster_requestData_t*)requestData)->interfaceType, ((xme_prim_modbusMaster_requestData_t*)requestData)->slaveID );
#endif

	copyRequestData = (xme_prim_modbusMaster_requestData_t*)requestData;
	
	// check interface type of the request
	if ( config->modbusType != copyRequestData->interfaceType )
	{
		return (xme_core_rr_responseStatus_t)XME_PRIM_MODBUSMASTER_STATUS_NOT_RESPONSIBLE;
	}
	
	// enter critical section -> update priority queue
	xme_hal_sync_enterCriticalSection(config->criticalSectionHandle);
	{

		///////////////////////////////////////////
		// put into request List
		//////////////////////////////////////////

		// allocate memory for item
		XME_CHECK_REC
		(
			NULL !=
			(
				priorityQueueItem = (xme_prim_modbusSimulator_priorityQueueItem_t*)xme_hal_mem_alloc
				(
					sizeof(xme_prim_modbusSimulator_priorityQueueItem_t)
				)
			),
			XME_CORE_RR_STATUS_SERVER_ERROR,
			{
				xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);
			}
		);

		// read request data an write it to local memory "item"
		// TODO: remove the meta data hack!
		priorityQueueItem->priority = ((xme_prim_modbusMaster_requestData_t*)requestData)->priority;
		priorityQueueItem->responseInstanceHandle = responseInstanceHandle;
		priorityQueueItem->requestType = ((xme_prim_modbusMaster_requestData_t*)requestData)->requestType;
		priorityQueueItem->registerType = ((xme_prim_modbusMaster_requestData_t*)requestData)->registerType;
		priorityQueueItem->slaveID = ((xme_prim_modbusMaster_requestData_t*)requestData)->slaveID;
		priorityQueueItem->componentIdOfSender = ((xme_prim_modbusMaster_requestData_t*)requestData)->componentId;
		priorityQueueItem->count = ((xme_prim_modbusMaster_requestData_t*)requestData)->count;
		priorityQueueItem->requestData = ((xme_prim_modbusMaster_requestData_t*)requestData)->data;
		priorityQueueItem->mstType = ((xme_prim_modbusMaster_requestData_t*)requestData)->mstType;

		// add new item into priorityQueue
		XME_HAL_SINGLYLINKEDLIST_ADD_ITEM_SORTED(config->priorityQueue, priorityQueueItem, offsetof(xme_prim_modbusMaster_requestData_t, priority), uint16_t);
	
		/*
		//////////////////////////////////////////////////////////
		// put into defaultSensorList or in dynamicSensorList
		//////////////////////////////////////////////////////////

		// check if this sensor is in the default sensor list
		// reset requested exists Flag
		requestedSensorExistsFlag = 0;
		XME_HAL_LINKEDLIST_ITERATE
		(
			config->defaultSensorList, 
			xme_prim_modbusSimulator_defaultSensorListItem_t, 
			defaultSensorListItem,
			{
				if ( defaultSensorListItem->slaveId == ((xme_prim_modbusMaster_requestData_t*)requestData)->slaveID )
				{
					// sensor is in defaultSensorList
					// set requested sensor exists Flag
					requestedSensorExistsFlag = 1;
				}
				else
				{	
					// sensor is not in defaultSensorList
				}
			}
		);
		*/
		/*
		// check if this sensor is in the dynamic sensor list
		// slaveId 247 is never a dynamic sensor slaveId !
		if ( 0 == requestedSensorExistsFlag && 247 != priorityQueueItem->slaveID )
		{
			XME_HAL_LINKEDLIST_ITERATE
			(
				config->dynamicSensorList, 
				xme_prim_modbusSimulator_dynamicSensorListItem_t, 
				dynamicSensorListItem,
				{
					if ( dynamicSensorListItem->slaveId == ((xme_prim_modbusMaster_requestData_t*)requestData)->slaveID )
					{
						// sensor is in the dynamicSensorList
						requestedSensorExistsFlag = 1;
					}
					else
					{	
						// sensor is not in the dynamicSensorList and not in defaultSensorList -> insert into defaultSensorList if not ID 247
						// sensor doesn't exist
						requestedSensorExistsFlag = 0;
					}
				}
			);	
		}
		*/
		/*
		// insert into defaultSensorList, if it's not a dynamic sensor
		if (config->defaultListFlag && requestedSensorExistsFlag == 0)
		{
			// slaveId 247 can also be inserted into this list, because this avoids confusions
			// allocate memory for item
			XME_CHECK
			(
				NULL !=
				(
					defaultSensorListItem = (xme_prim_modbusSimulator_defaultSensorListItem_t*)xme_hal_mem_alloc
					(
						sizeof(xme_prim_modbusSimulator_defaultSensorListItem_t)
					)
				),
				XME_CORE_RR_STATUS_SERVER_ERROR
			);
		
			defaultSensorListItem->slaveId = ((xme_prim_modbusMaster_requestData_t*)requestData)->slaveID;

			// add new item to sorted list
			XME_HAL_SINGLYLINKEDLIST_ADD_ITEM_SORTED(config->defaultSensorList, defaultSensorListItem, offsetof(xme_prim_modbusSimulator_defaultSensorListItem_t, slaveId), uint16_t);
		}
		else if (config->defaultListFlag == 0 && requestedSensorExistsFlag == 0)
		{
			// insert into dynamicSensorList, if the requested slaveId is not in the default sensor list
				
			// set default list flag
			config->defaultListFlag = 1;
		}
	*/
	}
		
	// leave critical section
	xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);
	
	return XME_CORE_RR_STATUS_RESPONSE_DELAYED;
}


// modbusSimulator_taskCallback
void
xme_prim_modbusSimulator_taskCallback(void* userData)
{
	xme_prim_modbusSimulator_configStruct_t* config = (xme_prim_modbusSimulator_configStruct_t*)userData;
	xme_prim_modbusSimulator_priorityQueueItem_t* priorityQueueItem = NULL;
	xme_prim_modbusSimulator_dynamicSensorListItem_t* dynamicSensorListItem = NULL;
	xme_prim_modbusMaster_status_t status = XME_PRIM_MODBUSMASTER_STATUS_SERVER_ERROR;
	void* responseData = NULL;
	uint16_t* responseDataField = NULL;
	uint16_t responseDataSize = 0;
	
	// increment time counter
	config->localTimeCount++;
	if ( 30000 <= config->localTimeCount)
	{
		printf("\n\n\n\n---------------------------------- G A M E ---- O V E R ----------------------------------\n");
		printf("\n\n                                ->     INSERT COIN      <-\n\n");	
		printf("");
	}

	// check, if there is a request in the request list
	// enter critical section
	xme_hal_sync_enterCriticalSection(config->criticalSectionHandle);

	if ( config->priorityQueue.count )
	{
		// prepare modbus response
		//status = xme_prim_modbusSimulator_prepareResponse( config, priorityQueueItem, responseData, &responseDataSize, responseDataField );
		xme_prim_modbusSimulator_configStruct_t* config = (xme_prim_modbusSimulator_configStruct_t*)userData;
		xme_prim_modbusMaster_status_t status = XME_PRIM_MODBUSMASTER_STATUS_SUCCESS;

		// set dataRequestedFlag
		config->dataRequestedFlag = 1;
			
		// get item with highest priority
		XME_CHECK_REC
		(
			NULL != 
			( priorityQueueItem = (xme_prim_modbusSimulator_priorityQueueItem_t*)XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(config->priorityQueue, 0) ), ,
			{
				xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);
				status = XME_PRIM_MODBUSMASTER_STATUS_SERVER_ERROR;
			}
		);

		// leave critical Section
		xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);


		// allocate memory for response data
		// responeData: <slaveId>, <requestData/ sensorNumber>, <registerType>, <sizeOfResponseData> = 6Byte + <count>*2
		responseDataSize = sizeof(xme_prim_modbusMaster_requestMetaData_t) + priorityQueueItem->count*2;
		XME_CHECK_REC
		(
			NULL != 
			( responseData = xme_hal_mem_alloc(responseDataSize) ), ,
			status = XME_PRIM_MODBUSMASTER_STATUS_SERVER_ERROR;
		);

		// add response meta Data to the response data
		// TODO: remove "responseMetaData-Hack"	
		((uint16_t*)responseData)[0] = priorityQueueItem->componentIdOfSender;
		((uint16_t*)responseData)[1]= priorityQueueItem->slaveID;
		((uint16_t*)responseData)[2] = priorityQueueItem->requestData;
		((uint16_t*)responseData)[3] = priorityQueueItem->registerType;
		((uint16_t*)responseData)[4] = priorityQueueItem->mstType;

		// TODO: remove "responseMetaData-Hack"	
		responseDataField = &((uint16_t*)responseData)[5];


		// check, if there is a request on id 247
		if ( 247 == priorityQueueItem->slaveID && config->dataRequestedFlag && XME_PRIM_MODBUSMASTER_STATUS_SUCCESS == status)
		{
			status = xme_prim_modbusSimulator_checkId247( config, priorityQueueItem, responseData, responseDataSize, responseDataField);
		}
				
		// check, if this is a request for a static sensor
		if ( (config->staticSensorList.count) && (config->dataRequestedFlag)  && XME_PRIM_MODBUSMASTER_STATUS_SUCCESS == status )
		{
			status = xme_prim_modbusSimulator_checkStaticSensorList( config, priorityQueueItem, dynamicSensorListItem, responseData, responseDataSize, responseDataField);
			
		}
				
		// check if this is a request for a dynamic sensor
		if ( (config->dynamicSensorList.count) && (config->dataRequestedFlag)  && XME_PRIM_MODBUSMASTER_STATUS_SUCCESS == status )
		{
			status = xme_prim_modbusSimulator_checkDynamicSensorList( config, priorityQueueItem, dynamicSensorListItem, responseData, responseDataSize, responseDataField);
		}

		// this is a request for a default sensor
		if ( config->dataRequestedFlag && (247 != priorityQueueItem->slaveID) )
		{
			status = xme_prim_modbusSimulator_sendDefaultResponse( config, priorityQueueItem, responseData, responseDataSize, responseDataField);
		}
				
		// enter critical section
		xme_hal_sync_enterCriticalSection(config->criticalSectionHandle);

		// remove item from list
		XME_CHECK_REC
		(
			1 == 
			(XME_HAL_SINGLYLINKEDLIST_REMOVE_ITEM(config->priorityQueue, priorityQueueItem, false)), ,
			{
				xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);
				status = XME_PRIM_MODBUSMASTER_STATUS_SERVER_ERROR;
			}
		);
		// leave critical section
		xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);

		// free memory for resoponse data
		if ( NULL != responseData) xme_hal_mem_free(responseData);
	}
	else
	{
		// leave critical section
		xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);
	}
}



// modbusSimulator_create dynamic sensor list
xme_prim_modbusMaster_status_t
xme_prim_modbusSimulator_createDynamicSensorList
(
	void* userData
)
{
	xme_prim_modbusSimulator_configStruct_t* config = (xme_prim_modbusSimulator_configStruct_t*)userData;
	xme_prim_modbusSimulator_dynamicSensorListItem_t* dynamicSensorListItem;
	int i;
	
	// enter critical section
	xme_hal_sync_enterCriticalSection(config->criticalSectionHandle);
	{
		for ( i=0; i<config->numberOfDynamicSensors; i++ )
		{
			// allocate memory for item
			XME_CHECK_REC
			(
				NULL !=
				(
					dynamicSensorListItem = (xme_prim_modbusSimulator_dynamicSensorListItem_t*)xme_hal_mem_alloc
					(
						sizeof(xme_prim_modbusSimulator_dynamicSensorListItem_t)
					)
				),
				XME_CORE_RR_STATUS_SERVER_ERROR,
				{
					xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);
				}
			);

			dynamicSensorListItem->slaveId = 0;
			memcpy((void*)dynamicSensorListItem->dynamicSensorSerialNumber,(void*)config->dynamicSensorSerialNumber[i], 6);
			dynamicSensorListItem->dynamicSensorLoginTime = config->dynamicSensorLoginTime[i];
			dynamicSensorListItem->dynamicSensorLogoutTime = config->dynamicSensorLogoutTime[i];
			dynamicSensorListItem->dynamicSensorMeanValue = config->dynamicSensorMeanValue[i];
			dynamicSensorListItem->dynamicSensorVariance = config->dynamicSensorVariance[i];
			dynamicSensorListItem->activeFlag = 0;
			dynamicSensorListItem->dead = 0;

			// add new item to sorted list
			XME_HAL_SINGLYLINKEDLIST_ADD_ITEM_SORTED(config->dynamicSensorList, dynamicSensorListItem, offsetof(xme_prim_modbusSimulator_dynamicSensorListItem_t, slaveId), uint16_t);
		}
	}
	// leave critical section
	xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);

	return XME_PRIM_MODBUSMASTER_STATUS_SUCCESS;
}


// modbusSimulator_create static sensor list
xme_prim_modbusMaster_status_t
xme_prim_modbusSimulator_createStaticSensorList
(
	void* userData
)
{
	xme_prim_modbusSimulator_configStruct_t* config = (xme_prim_modbusSimulator_configStruct_t*)userData;
	xme_prim_modbusSimulator_staticSensorListItem_t* staticSensorListItem;
	int i;

	// enter critical section
	xme_hal_sync_enterCriticalSection(config->criticalSectionHandle);
	{
		for ( i=0; i<config->numberOfStaticSensors; i++ )
		{
			// allocate memory for item
			XME_CHECK_REC
			(
				NULL !=
				(
					staticSensorListItem = (xme_prim_modbusSimulator_staticSensorListItem_t*)xme_hal_mem_alloc
					(
						sizeof(xme_prim_modbusSimulator_staticSensorListItem_t)
					)
				),
				XME_CORE_RR_STATUS_SERVER_ERROR,
				{
					xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);
				}
			);

			staticSensorListItem->slaveId = config->slaveIdsOfStaticSensors[i];
			staticSensorListItem->staticSensorMeanValue = config->staticSensorMeanValue[i];
			staticSensorListItem->staticSensorVariance = config->staticSensorVariance[i];

			// add new item to sorted list
			XME_HAL_SINGLYLINKEDLIST_ADD_ITEM_SORTED(config->staticSensorList, staticSensorListItem, offsetof(xme_prim_modbusSimulator_staticSensorListItem_t, slaveId), uint16_t);
		}
	}
	// leave critical section
	xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);

	return XME_PRIM_MODBUSMASTER_STATUS_SUCCESS;
};

xme_prim_modbusMaster_status_t
xme_prim_modbusSimulator_getGaussianRandomNumber
(
	void* userData,
	xme_prim_modbusSimulator_priorityQueueItem_t* item,
	void* responseData,
	double meanValue,
	double variance
)
{
	xme_prim_modbusSimulator_configStruct_t* config = (xme_prim_modbusSimulator_configStruct_t*)userData;
	xme_prim_modbusMaster_status_t status = XME_PRIM_MODBUSMASTER_STATUS_MODBUS_ERROR;
	uint16_t* copyResponseData = NULL;
	int j;
	int i;
	uint16_t randomNumber[2] = {0,0};
	long randomNumberCum = 0;

	// interprete request
switch(item->registerType) 
				   {
					case XME_PRIM_MODBUSMASTER_REGISTER_TYPE_INPUT_REGISTERS:

						// generate gaussian distributed number for every sensor in this mstSystem
						// related to the "central limit theorem"
						for(i=0; i < item->count; i++)
						{
							randomNumber[0] = 0;
							for(j=0; j<=12; j++)
							{
								// Vorsicht, da werden uint16_t generiert... in diesem Zuge könnte man auch gleich die Linearisierung rein bringen, oder???
								randomNumber[0] = xme_hal_random_randRange(0, 100);
								randomNumberCum += randomNumber[0];
							}
							((uint16_t*)responseData)[i] = (uint16_t)(meanValue + sqrt( variance )*( randomNumberCum/100 - 6));
						}
					
						status = XME_PRIM_MODBUSMASTER_STATUS_SUCCESS;
						break;
					case XME_PRIM_MODBUSMASTER_REGISTER_TYPE_HOLDING_REGISTERS:
						// generate gaussian distributed number for adc value
						// related to the "central limit theorem"
						randomNumber[0] = 0;
						for(j=0; j<=12; j++)
						{
							randomNumber[0] = xme_hal_random_randRange(0, 1000);
							randomNumberCum += randomNumber[0];
						}
						// change high and lowbyte to be consistence with real data

						*(long*)randomNumber = (long)(meanValue + sqrt( variance )*( randomNumberCum/1000 - 6));
						((uint16_t*)responseData)[0] = randomNumber[1];
						((uint16_t*)responseData)[1] = randomNumber[0];

						status = XME_PRIM_MODBUSMASTER_STATUS_SUCCESS;
						break;
					default: status = XME_PRIM_MODBUSMASTER_STATUS_UNEXPECTED_REGISTERTYPE;
					}
	return status;
}


// check if there is a login, logout or nothing
xme_prim_modbusMaster_status_t
xme_prim_modbusSimulator_checkId247
(
	void* userData,
	xme_prim_modbusSimulator_priorityQueueItem_t* priorityQueueItem,
	void* responseData,
	uint16_t responseDataSize,
	uint16_t* responseDataField
)
{
	xme_prim_modbusSimulator_configStruct_t* config = (xme_prim_modbusSimulator_configStruct_t*)userData;
	xme_prim_modbusMaster_status_t status = XME_PRIM_MODBUSMASTER_STATUS_SUCCESS;

	// check, if there are sonsors to login or to logout. If there is a login, send serial number
	// if not, send response "no sensor at slave Id 247"

	// iterate dynamic sensor list and check login and logout time with local time count
	XME_HAL_LINKEDLIST_ITERATE
	(
		config->dynamicSensorList, 
		xme_prim_modbusSimulator_dynamicSensorListItem_t, 
		dynamicSensorListItem,
		{
			// time for sensor login? ( if it is the right time, if the spezific sensor is not activated and not dead, and if the programm is not allready in login mode (because there can just be one sensor in login mode)
			if ( dynamicSensorListItem->dynamicSensorLoginTime <= config->localTimeCount && dynamicSensorListItem->activeFlag == 0 && dynamicSensorListItem->dead == 0 && 0 == config->loginMode)
			{
				// sensor is not active, but has to be logged in
				// reset default flag
				config->defaultListFlag = 0;
				// reset data requested flag
				config->dataRequestedFlag = 0;

				// get serial number
				// TODO: remove "metaData-Hack"
				memcpy( responseDataField, (void*)dynamicSensorListItem->dynamicSensorSerialNumber, 6);
				// send response
				printf("\nmodbusSimulator: it's time to login a dynamic sensor\n");
				xme_core_rr_sendResponse(XME_CORE_RR_STATUS_SUCCESS, config->requestHandlerHandle, priorityQueueItem->responseInstanceHandle, responseData, responseDataSize);

				// set activation flag of this dynamic sensor
				dynamicSensorListItem->activeFlag = 1;
				// mark this sensor with slaveId 247
				dynamicSensorListItem->slaveId = 247;
				// it's just possible to login one sensor at time
				config->loginMode = 1;
			}

			// time for sensor logout?
			else if ( dynamicSensorListItem->dynamicSensorLogoutTime <= config->localTimeCount && dynamicSensorListItem->activeFlag == 1 )
			{
				// deactivate this sensor, the responsable mstLogger will log out this sensor later

				// sensor is active, but have to be logged out
				// reset active flag of this sensor
				printf("\nmodbusSimulator: it's time to logout a dynamic sensor\n");
							
				// reset acive flag of this sensor
				dynamicSensorListItem->activeFlag = 0;
				// make shure the sensor won't login again
				dynamicSensorListItem->dead = 1;

				// the data requested flag remains one
				config->dataRequestedFlag = 1;
			}
		}
	);
		
	// if it's a request on 247 to write data to holding register: this is the new slave id for a dynamic sensor
	if ( 247 == priorityQueueItem->slaveID && XME_PRIM_MODBUSMASTER_WRITE == priorityQueueItem->requestType )
	{
		printf("modbusSimulator: the new slave Id for my dynamic sensor is %d\n", priorityQueueItem->requestData);
		// iterate dynamic sensor list and check for the dynamic sensor, that should be logged into the system (marked with slaveId 247 )
		XME_HAL_LINKEDLIST_ITERATE
		(
			config->dynamicSensorList, 
			xme_prim_modbusSimulator_dynamicSensorListItem_t, 
			dynamicSensorListItem,
			{
				if ( 247 == dynamicSensorListItem->slaveId ) 
				{
					dynamicSensorListItem->slaveId = priorityQueueItem->requestData;
					// reset loginMode
					config->loginMode = 0;
				}
			}
		);
		// reset data requested flag
		config->dataRequestedFlag = 0;
	}

	// if it's not time to login a sensor, there is no sensor on id 247
	else if ( 247 == priorityQueueItem->slaveID && config->dataRequestedFlag)
	{
		xme_core_rr_sendResponse
		(	
			XME_PRIM_MODBUSMASTER_STATUS_CONNECTION_FAILD, 
			config->requestHandlerHandle, 
			priorityQueueItem->responseInstanceHandle, 
			responseData, 
			responseDataSize
		);
		// reset data requested flag
		config->dataRequestedFlag = 0;
	}
	return status;
}


xme_prim_modbusMaster_status_t
xme_prim_modbusSimulator_prepareResponse
(
	void* userData,
	xme_prim_modbusSimulator_priorityQueueItem_t* priorityQueueItem,
	void* responseData,
	uint16_t* responseDataSize,
	uint16_t* responseDataField
)
{

	xme_prim_modbusSimulator_configStruct_t* config = (xme_prim_modbusSimulator_configStruct_t*)userData;
	xme_prim_modbusMaster_status_t status = XME_PRIM_MODBUSMASTER_STATUS_SUCCESS;

	// set dataRequestedFlag
	config->dataRequestedFlag = 1;
			
	// get item with highest priority
	XME_CHECK
	(
		NULL != 
		( priorityQueueItem = (xme_prim_modbusSimulator_priorityQueueItem_t*)XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(config->priorityQueue, 0) ),
		status = XME_PRIM_MODBUSMASTER_STATUS_SERVER_ERROR
	);

	// allocate memory for response data
	// responeData: <slaveId>, <requestData/ sensorNumber>, <registerType>, <sizeOfResponseData> = 6Byte + <count>*2
	*responseDataSize = sizeof(xme_prim_modbusMaster_requestMetaData_t) + priorityQueueItem->count*2;

	XME_CHECK
	(
		NULL != 
		( responseData = xme_hal_mem_alloc(*responseDataSize) ),
		status = XME_PRIM_MODBUSMASTER_STATUS_SERVER_ERROR
	);

	// add response meta Data to the response data
	// TODO: remove "responseMetaData-Hack"	
	((uint16_t*)responseData)[0] = priorityQueueItem->componentIdOfSender;
	((uint16_t*)responseData)[1]= priorityQueueItem->slaveID;
	((uint16_t*)responseData)[2] = priorityQueueItem->requestData;
	((uint16_t*)responseData)[3] = priorityQueueItem->registerType;
	((uint16_t*)responseData)[4] = priorityQueueItem->mstType;

	// TODO: remove "responseMetaData-Hack"	
	responseDataField = &((uint16_t*)responseData)[5];

	return 0;
}


xme_prim_modbusMaster_status_t
xme_prim_modbusSimulator_checkDynamicSensorList
(
	void* userData,
	xme_prim_modbusSimulator_priorityQueueItem_t* priorityQueueItem,
	xme_prim_modbusSimulator_dynamicSensorListItem_t* dynamicSensorListItem,
	void* responseData,
	uint16_t responseDataSize,
	uint16_t* responseDataField
)
{
	xme_prim_modbusSimulator_configStruct_t* config = (xme_prim_modbusSimulator_configStruct_t*)userData;
	xme_prim_modbusMaster_status_t status = XME_PRIM_MODBUSMASTER_STATUS_SUCCESS;

	XME_HAL_LINKEDLIST_ITERATE
	(
		config->dynamicSensorList, 
		xme_prim_modbusSimulator_dynamicSensorListItem_t, 
		dynamicSensorListItem,
		{
			// check if the requested sensor is in list and activated
			if ( dynamicSensorListItem->slaveId == priorityQueueItem->slaveID && dynamicSensorListItem->activeFlag == 1 )
			{
				// simulate responseData for input and holding register
				// switch ( priorityQueueItem->mstType );
				if ( XME_PRIM_MODBUSMASTER_REGISTER_TYPE_INPUT_REGISTERS == priorityQueueItem->registerType )
				{
					status = xme_prim_modbusSimulator_getGaussianRandomNumber( config, priorityQueueItem, responseDataField, dynamicSensorListItem->dynamicSensorMeanValue, dynamicSensorListItem->dynamicSensorVariance);
				}
				else if ( XME_PRIM_MODBUSMASTER_REGISTER_TYPE_HOLDING_REGISTERS == priorityQueueItem->registerType )
				{
					printf("SIMULATOR: dynamic sensor slaveId %d\n", priorityQueueItem->slaveID);
					status = xme_prim_modbusSimulator_getGaussianRandomNumber( config, priorityQueueItem, responseDataField, dynamicSensorListItem->dynamicSensorMeanValue, dynamicSensorListItem->dynamicSensorVariance);
				}
				else 
				{ 
					status = XME_PRIM_MODBUSMASTER_STATUS_SERVER_ERROR;
				}

				// send response
				xme_core_rr_sendResponse((xme_core_rr_responseStatus_t)status, config->requestHandlerHandle, priorityQueueItem->responseInstanceHandle, responseData, ( responseDataSize ));
				// reset data requested flag
				config->dataRequestedFlag = 0;	
			}
			else if ( dynamicSensorListItem->slaveId == priorityQueueItem->slaveID && dynamicSensorListItem->activeFlag == 0 )
			{
				// sensor is not activated, though set data requested flag to zero and send responds
				config->dataRequestedFlag = 0;
				status = XME_PRIM_MODBUSMASTER_STATUS_CONNECTION_FAILD;
				printf("SIMULATOR:no reply from dynamic sensor %d due to deactivation\n", priorityQueueItem->slaveID);
				// send response
				xme_core_rr_sendResponse((xme_core_rr_responseStatus_t)status, config->requestHandlerHandle, priorityQueueItem->responseInstanceHandle, responseData, ( responseDataSize ));
				// reset data requested flag
				config->dataRequestedFlag = 0;	
			}
		}
	);

	return status;
}


xme_prim_modbusMaster_status_t
xme_prim_modbusSimulator_checkStaticSensorList
(
	void* userData,
	xme_prim_modbusSimulator_priorityQueueItem_t* priorityQueueItem,
	xme_prim_modbusSimulator_dynamicSensorListItem_t* staticSensorListItem,
	void* responseData,
	uint16_t responseDataSize,
	uint16_t* responseDataField
)
{
	xme_prim_modbusSimulator_configStruct_t* config = (xme_prim_modbusSimulator_configStruct_t*)userData;
	xme_prim_modbusMaster_status_t status = XME_PRIM_MODBUSMASTER_STATUS_SUCCESS;

	XME_HAL_LINKEDLIST_ITERATE
	(
		config->staticSensorList, 
		xme_prim_modbusSimulator_staticSensorListItem_t, 
		staticSensorListItem,
		{
			// check if the requested sensor is in list
			if ( staticSensorListItem->slaveId == priorityQueueItem->slaveID )
			{
				// simulate responseData for input and holding register
				// switch ( priorityQueueItem->mstType );
				if ( XME_PRIM_MODBUSMASTER_REGISTER_TYPE_INPUT_REGISTERS == priorityQueueItem->registerType )
				{
					status = xme_prim_modbusSimulator_getGaussianRandomNumber( config, priorityQueueItem, responseDataField, staticSensorListItem->staticSensorMeanValue, staticSensorListItem->staticSensorVariance);
				}
				else if ( XME_PRIM_MODBUSMASTER_REGISTER_TYPE_HOLDING_REGISTERS == priorityQueueItem->registerType )
				{
					printf("SIMULATOR: static sensor slaveId %d\n", priorityQueueItem->slaveID);
					status = xme_prim_modbusSimulator_getGaussianRandomNumber( config, priorityQueueItem, responseDataField, staticSensorListItem->staticSensorMeanValue, staticSensorListItem->staticSensorVariance);
				}
				else 
				{ 
					status = XME_PRIM_MODBUSMASTER_STATUS_SERVER_ERROR; 
				}

				// send response
				xme_core_rr_sendResponse((xme_core_rr_responseStatus_t)status, config->requestHandlerHandle, priorityQueueItem->responseInstanceHandle, responseData, ( responseDataSize ));
				// reset data requested flag
				config->dataRequestedFlag = 0;
			}
		}
	);

	return status;
}


xme_prim_modbusMaster_status_t
xme_prim_modbusSimulator_sendDefaultResponse
(
	void* userData,
	xme_prim_modbusSimulator_priorityQueueItem_t* priorityQueueItem,
	void* responseData,
	uint16_t responseDataSize,
	uint16_t* responseDataField
)
{
	xme_prim_modbusSimulator_configStruct_t* config = (xme_prim_modbusSimulator_configStruct_t*)userData;
	xme_prim_modbusMaster_status_t status = XME_PRIM_MODBUSMASTER_STATUS_SUCCESS;

	// simulate responseData for input and holding register
	// switch ( priorityQueueItem->mstType );
		if ( XME_PRIM_MODBUSMASTER_REGISTER_TYPE_INPUT_REGISTERS == priorityQueueItem->registerType )
		{
			status = xme_prim_modbusSimulator_getGaussianRandomNumber( config, priorityQueueItem, responseDataField, config->defaultSensorMeanValueInputRegister, config->defaultSensorVarianceInputRegister);
		}
		else if ( XME_PRIM_MODBUSMASTER_REGISTER_TYPE_HOLDING_REGISTERS == priorityQueueItem->registerType )
		{
			printf("SIMULATOR: default sensor slaveId %d\n", priorityQueueItem->slaveID);
			status = xme_prim_modbusSimulator_getGaussianRandomNumber( config, priorityQueueItem, responseDataField, config->defaultSensorMeanValueHoldingRegister, config->defaultSensorVarianceHoldingRegister);
		}
		else 
		{ 
			status = XME_PRIM_MODBUSMASTER_STATUS_SERVER_ERROR; 
		}

	// send response
	xme_core_rr_sendResponse((xme_core_rr_responseStatus_t)status, config->requestHandlerHandle, priorityQueueItem->responseInstanceHandle, responseData, ( responseDataSize ));
	// reset data requested flag
	config->dataRequestedFlag = 0;	

	return XME_PRIM_MODBUSMASTER_STATUS_SUCCESS;
}


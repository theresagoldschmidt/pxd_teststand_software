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
 *         MST Logger.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Marcel Flesch <fleschmarcel@mytum.de>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/adv/mstLogger.h"

#include "xme/core/log.h"
#include "xme/core/rr.h"

#include "xme/core/component.h"
#include "xme/core/core.h"
#include "xme/core/defines.h"
#include "xme/core/nodeManager.h"
#include "xme/core/interfaceManager.h"
#include "xme/core/broker.h"
#include "xme/core/topic.h"

#include "xme/core/dcc.h"
#include "xme/core/defines.h"
#include "xme/hal/random.h"

#include "xme/prim/modbusMaster.h"

#define DEBUG 0

/******************************************************************************/
/***   Forwaerd declarations                                                ***/
/******************************************************************************/
void 
xme_adv_mstLogger_dcc_taskCallback
(
	void* userData
);

xme_core_rr_responseStatus_t
receiveTopicSystemStateCallback
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
);

void
xme_adv_mstLogger_receiveResponseCallback
(
	xme_core_rr_responseStatus_t status,
	xme_core_rr_requestHandle_t request,
	xme_core_rr_requestInstanceHandle_t requestInstance,
	xme_core_topic_t responseTopic,
	void* responseData,
	xme_core_md_topicMetaDataHandle_t responseMetaData,
	void* userData,
	void* instanceUserData
);

xme_core_status_t
xme_adv_mstLogger_createSystemStateList
(
	void* userData,
	xme_adv_mstManager_systemState_t* systemState,
	int mstNum
);


xme_core_status_t
xme_adv_mstLogger_deleteItemfromSystemStateList
(
	void* userData,
	xme_adv_mstManager_systemState_t* systemState,
	int mstNum
);

xme_core_status_t
xme_adv_mstLogger_sendAdcValuesAsTopic
(
	void* userData,
	xme_adv_mstLogger_mstSystemRequestListItem_t* item,
	void* responseDataField
);

xme_core_status_t
xme_adv_mstLogger_sendDigitalReadingValuesAsTopic
(
	void* userData,
	xme_adv_mstLogger_mstSystemRequestListItem_t* item,
	void* responseDataField
);

xme_core_status_t
xme_adv_mstLogger_linearizeTempData
(
	void* userData,
	long* data,
	xme_adv_mstLogger_topicData_t* topicData
);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_adv_mstLogger_create(xme_adv_mstLogger_configStruct_t* config)
{
	// Initialize configuration structure
	config->dccPublicationHandle = XME_CORE_DCC_INVALID_PUBLICATION_HANDLE;
	config->dccTaskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;
	config->rrRequestHandle = XME_CORE_RR_INVALID_REQUEST_HANDLE;
	config->rrTaskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;

	// listen topic "systemState"
	config->topicHandle = xme_core_dcc_subscribeTopic
	(
		XME_CORE_TOPIC_SYSTEM_STATE,
		XME_CORE_MD_EMPTY_META_DATA,
		false,
		&receiveTopicSystemStateCallback,
		config
	);

	// publish topic XME_CORE_TOPIC_SYSTEM_ERROR
	XME_CHECK
	(
		XME_CORE_DCC_INVALID_PUBLICATION_HANDLE !=
		(
			config->errorDccPublicationHandle = xme_core_dcc_publishTopic
			(
				XME_CORE_TOPIC_SYSTEM_ERROR,
				XME_CORE_MD_EMPTY_META_DATA,
				false,
				NULL
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// get current componentID
	config->componentID = xme_core_resourceManager_getCurrentComponentId();

	// define topic acording to the mstType in the configuration
	switch(config->mstType)
	{
	case XME_ADV_MSTLOGGER_TEMPERATURE:
		{
			printf("mstLogger with component ID #%d: I'm responsable for mst temperature systems with register type %d \n", config->componentID, config->mstDataType);
			config->topic = XME_CORE_TOPIC_TEMPERATURE;
		}
		break;
	case XME_ADV_MSTLOGGER_PRESSURE:
		{
			printf("mstLogger with component ID #%d: I'm responsable for mst pressure systems with register type %d \n", config->componentID, config->mstDataType);
			config->topic = XME_CORE_TOPIC_PRESSURE;
		}
		break;
	default: return XME_CORE_STATUS_INVALID_CONFIGURATION;	
	}


	// publish topic for temperatur or pressure data
	XME_CHECK
	(
		XME_CORE_DCC_INVALID_PUBLICATION_HANDLE !=
		(
			config->dccPublicationHandle = xme_core_dcc_publishTopic
			(
				config->topic,
				XME_CORE_MD_EMPTY_META_DATA,
				false,
				NULL
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// init task for polling data
	XME_CHECK
	(
	 	XME_HAL_SCHED_INVALID_TASK_HANDLE !=
		(
			config->dccTaskHandle = xme_core_resourceManager_scheduleTask
			(
				XME_HAL_SCHED_TASK_INITIALLY_SUSPENDED,
				config->intervalMs,
				XME_HAL_SCHED_PRIORITY_NORMAL,
				&xme_adv_mstLogger_dcc_taskCallback,
				config
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// publish request XME_CORE_TOPIC_MST_REQUEST
	XME_CHECK
	(
		XME_CORE_RR_INVALID_REQUEST_HANDLE !=
		(
			config->rrRequestHandle = xme_core_rr_publishRequest
			(
				XME_CORE_TOPIC_MODBUS_REQUEST,
				XME_CORE_MD_EMPTY_META_DATA,
				XME_CORE_TOPIC_MODBUS_RESPONSE,
				XME_CORE_MD_EMPTY_META_DATA,
				true,
				false,
				&xme_adv_mstLogger_receiveResponseCallback,
				config
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// create handle for critical section
	config->criticalSectionHandle = xme_hal_sync_createCriticalSection();

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_adv_mstLogger_activate(xme_adv_mstLogger_configStruct_t* config)
{
	// Start Task for dcc
	XME_CHECK
	(
		XME_CORE_STATUS_SUCCESS == xme_hal_sched_setTaskExecutionState(config->dccTaskHandle, true),
		XME_CORE_STATUS_INTERNAL_ERROR
	);

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_adv_mstLogger_deactivate(xme_adv_mstLogger_configStruct_t* config)
{
	XME_ASSERT_NORVAL(XME_CORE_STATUS_SUCCESS == xme_hal_sched_setTaskExecutionState(config->dccTaskHandle, false));
	XME_ASSERT_NORVAL(XME_CORE_STATUS_SUCCESS == xme_hal_sched_setTaskExecutionState(config->rrTaskHandle, false));
}

void
xme_adv_mstLogger_destroy
(
	xme_adv_mstLogger_configStruct_t* config
)

{
	xme_hal_sync_destroyCriticalSection(config->criticalSectionHandle);
	config->criticalSectionHandle = XME_HAL_SYNC_INVALID_CRITICAL_SECTION_HANDLE;

	// TODO
	//// Unpublish RequestHandler
	//xme_core_rr_unpublishRequestHandler (
	//	xme_core_rr_requestHandlerHandle_t requestHandlerHandle
	//);

	// TODO: mstSystemStateList löschen!

	XME_ASSERT_NORVAL(XME_CORE_STATUS_SUCCESS == xme_hal_sched_removeTask(config->dccTaskHandle));
	config->dccTaskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;

	XME_ASSERT_NORVAL(XME_CORE_STATUS_SUCCESS == xme_core_dcc_unpublishTopic(config->dccPublicationHandle));
	config->dccPublicationHandle = XME_CORE_DCC_INVALID_PUBLICATION_HANDLE;
}

void
xme_adv_mstLogger_dcc_taskCallback
(
	void* userData
)
{
	xme_adv_mstLogger_configStruct_t* config = (xme_adv_mstLogger_configStruct_t*)userData;
	xme_prim_modbusMaster_requestData_t mstRequestData;

	// enter critical section
	xme_hal_sync_enterCriticalSection(config->criticalSectionHandle);

	if (XME_HAL_LINKEDLIST_ITEM_COUNT(config->mstSystemRequestList) )
	{
		#if DEBUG
			printf("mstLogger with component ID #%d: Send request list to modbusMaster\n", config->componentID);
		#endif		
		
		XME_HAL_LINKEDLIST_ITERATE
		(
			config->mstSystemRequestList, 
			xme_adv_mstLogger_mstSystemRequestListItem_t, 
			item,
			{
				mstRequestData.mstType = item->mstType;
				mstRequestData.componentId = item->componentId;
				mstRequestData.interfaceType = item->interfaceType;
				mstRequestData.priority = item->priority;
				mstRequestData.requestType = item->requestType;
				mstRequestData.registerType = item->registerType;
				mstRequestData.slaveID = item->slaveID;
				mstRequestData.startRegister = item->startRegister;
				mstRequestData.count = item->count;
				mstRequestData.data = item->data;

				xme_core_rr_sendRequest
				( 
					config->rrRequestHandle,  
					&mstRequestData,  
					sizeof(xme_prim_modbusMaster_requestData_t),
					config,  
					500
				 );
			}
		);
	};

	// leave critical section
	xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);
}


void
xme_adv_mstLogger_receiveResponseCallback
(
	xme_core_rr_responseStatus_t status,
	xme_core_rr_requestHandle_t request,
	xme_core_rr_requestInstanceHandle_t requestInstance,
	xme_core_topic_t responseTopic,
	void* responseData,
	xme_core_md_topicMetaDataHandle_t responseMetaData,
	void* userData,
	void* instanceUserData
)
{
	xme_adv_mstLogger_configStruct_t* config = (xme_adv_mstLogger_configStruct_t*)userData;
	uint16_t componentIdOfSender = 0;
	uint16_t slaveId = 0;
	uint16_t sensorNumber = 0;
	void* responseDataField = NULL;
	xme_prim_modbusMaster_status_t modbusStatus;

	modbusStatus = (xme_prim_modbusMaster_status_t)status;

	// check status of modbus response
	switch ( modbusStatus )
	{
	case XME_PRIM_MODBUSMASTER_STATUS_SUCCESS:
		{
			// TODO: remove "responseMetaData-Hack"
			componentIdOfSender = ((uint16_t*)responseData)[0];
			slaveId = ((uint16_t*)responseData)[1];
			sensorNumber = ((uint16_t*)responseData)[2];
			responseDataField = &((uint16_t*)responseData)[5];

			// check, if this response is for this component
			if ( config->componentID == componentIdOfSender )
			{
				#if DEBUG
					if(status == XME_CORE_RR_STATUS_SUCCESS )
						printf("mstLogger with component ID #%d: received response from modbusMaster with slaveId %d and sensorNumber: %d\n", config->componentID, slaveId, sensorNumber);
					else
						printf("mstLogger with component ID #%d: received response from modbusMaster from slaveId %d with errorNum %d\n", config->componentID, slaveId, status);
				#endif
				
				// find item with the same slaveID in the request list, build topicData and send topic
				// enter critical section
				xme_hal_sync_enterCriticalSection(config->criticalSectionHandle);

				XME_HAL_LINKEDLIST_ITERATE
				(
					config->mstSystemRequestList, 
					xme_adv_mstLogger_mstSystemRequestListItem_t, 
					item,
					{
						switch ( config->mstDataType )
						{
						case XME_ADV_MSTLOGGER_ADC_VALUE:	// ADC Value
							if ( item->slaveID == slaveId && item->data == sensorNumber ) 
							{
								xme_adv_mstLogger_sendAdcValuesAsTopic(config, item, responseDataField);
								//printf("\n adc-value mit slave %d und sensor %d\n", slaveId, sensorNumber);
							}
							break;
						case XME_ADV_MSTLOGGER_READING_VALUE:	// Digital Read Value
							//printf("\n dig-value mit slave %d und sensor %d\n", slaveId, sensorNumber);
							if ( slaveId == item->slaveID )
							{
								xme_adv_mstLogger_sendDigitalReadingValuesAsTopic(config, item, responseDataField);
								//printf("\n dig-value mit slave %d und sensor %d\n", slaveId, sensorNumber);
							}
							break;
						}
					}
				);
				// leave critical section
				xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);
			}

		} break;
	case XME_PRIM_MODBUSMASTER_STATUS_CONNECTION_FAILD:
		{
			// sensor does not react, log it out
			// allocate memory for error topic
			XME_CHECK
			(
				NULL !=
				(
				config->systemError = (xme_adv_mstManager_systemError_t*)xme_hal_mem_alloc(sizeof(xme_adv_mstManager_systemError_t))
				),
			);

			// add error code and additional data
			config->systemError->status = XME_ADV_MSTMANAGER_STATUS_LOGOUT;
			// additional data is the slaveID of the modbus System with error
			// TODO: remove "responseMetaData-Hack"
			config->systemError->data = (uint8_t)((uint16_t*)responseData)[1];

			// send topic "SYSTEM_ERROR"
			if ( NULL != config->systemError )
			{
				xme_core_dcc_sendTopicData
				(
					config->errorDccPublicationHandle,
					(void*)config->systemError,
					sizeof(xme_adv_mstManager_systemError_t)
				);	
				xme_hal_mem_free(config->systemError);
			}
		} break;
	case XME_PRIM_MODBUSMASTER_STATUS_NOT_RESPONSIBLE:
		{
			// don't react...
		} break;
	}
}



// send adc-values as topic
xme_core_status_t
xme_adv_mstLogger_sendAdcValuesAsTopic
(
	void* userData,
	xme_adv_mstLogger_mstSystemRequestListItem_t* item,
	void* responseDataField
)
{
	xme_adv_mstLogger_configStruct_t* config = (xme_adv_mstLogger_configStruct_t*)userData;	
	xme_adv_mstLogger_topicData_t* topicData = NULL;
	uint16_t adcValue[2];
	double help;
	double adc_double;

	// read out high and lowbyte
	adcValue[0] = ((uint16_t*)responseDataField)[1];
	adcValue[1] = ((uint16_t*)responseDataField)[0];

	// allocate memory for topicData
	// Topic: <installPositionInstance>, <SensorNumber/ SensorInstance>, <Value>
	topicData = (xme_adv_mstLogger_topicData_t*)xme_hal_mem_alloc( sizeof(xme_adv_mstLogger_topicData_t) );

	if ( XME_ADV_MSTLOGGER_TEMPERATURE == config->mstType )
	{
		// linearize temperatur data
		xme_adv_mstLogger_linearizeTempData( config, (long*)adcValue, topicData );
	}
	else if ( XME_ADV_MSTLOGGER_PRESSURE == config->mstType )
	{
		// calculate pressure from adc values in mbar
		help = (double)5000000/(2<<23);
		adc_double = (double)(*(long*)adcValue);
		topicData->data = (long)(help*adc_double);
	}

	// insert installPositionInstance and sensorNumber
	topicData->installPositionInstance = item->installPositionInstance;
	topicData->sensorNumber = item->startRegister;

#if DEBUG					
	printf("mstLogger with component ID #%d: send Sensor %d topic %d\n", config->componentID, item->data, config->topic);
#endif
	// send Topic
	XME_CHECK_REC
	(
		XME_CORE_STATUS_SUCCESS == 
		xme_core_dcc_sendTopicData(config->dccPublicationHandle, topicData, sizeof(xme_adv_mstLogger_topicData_t) ),
		XME_CORE_STATUS_INTERNAL_ERROR,
		printf("mstLogger with component ID #%d: Error while publishing Topic %d\n", config->componentID, config->topic)
	);
	xme_hal_mem_free(topicData);
	
	return XME_CORE_STATUS_SUCCESS;
};

// send digital-reading-values as topic
xme_core_status_t
xme_adv_mstLogger_sendDigitalReadingValuesAsTopic
(
	void* userData,
	xme_adv_mstLogger_mstSystemRequestListItem_t* item,
	void* responseDataField
)
{
	xme_adv_mstLogger_configStruct_t* config = (xme_adv_mstLogger_configStruct_t*)userData;
	int i;
	uint16_t* copyResponseData = NULL;
	xme_adv_mstLogger_topicData_t* topicData = NULL;
	uint16_t data;
	
	// send TopicData for every Sensor in this mstSystem
	copyResponseData = (uint16_t*)responseDataField;

	// get value for every sensor and send it as topic
	for ( i=0; i < item->count; i++ )
	{
		// allocate memory for topicData
		// Topic: <installPositionInstance>, <SensorNumber/ SensorInstance>, <Value>
		topicData = (xme_adv_mstLogger_topicData_t*)xme_hal_mem_alloc( sizeof(xme_adv_mstLogger_topicData_t) );				
		
		topicData->installPositionInstance = item->installPositionInstance;
		topicData->sensorNumber = i;

		// scale value with 10
		data = copyResponseData[i];
		topicData->data = 10*((long)data);

		// send Topic
		XME_CHECK_REC
		(
			XME_CORE_STATUS_SUCCESS == 
			xme_core_dcc_sendTopicData(config->dccPublicationHandle, topicData, sizeof(xme_adv_mstLogger_topicData_t) ),
			XME_CORE_STATUS_INTERNAL_ERROR,
			printf("mstLogger with component ID #%d: Error while publishing Topic %d\n", config->componentID, config->topic)
		);
		xme_hal_mem_free(topicData);
	}
	
	return XME_CORE_STATUS_SUCCESS;
};


xme_core_rr_responseStatus_t
receiveTopicSystemStateCallback
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
)
{
	xme_adv_mstManager_systemState_t* systemState;
	uint16_t topicBufferSize;
	int mstNum;
	
	// Retrieve configuration structure
	xme_adv_mstLogger_configStruct_t* config = (xme_adv_mstLogger_configStruct_t*)userData;

	// get Topic Data from 'systemState' topic
	systemState = (xme_adv_mstManager_systemState_t*)xme_hal_sharedPtr_getPointer(dataHandle);

	// check dataSize
	topicBufferSize = xme_hal_sharedPtr_getSize(dataHandle);

	// calculate number of mstSystems
	mstNum = topicBufferSize / sizeof(xme_adv_mstManager_systemState_t);

	#if DEBUG
		printf("mstLogger with component ID #%d: received topic callback 'systemState' with %d mstSystems\n", config->componentID, mstNum);
	#endif

	// create systemStateList
	XME_CHECK
	(
		XME_CORE_STATUS_SUCCESS == 
			( 
				xme_adv_mstLogger_createSystemStateList(userData, systemState, mstNum)
			),
		XME_CORE_RR_STATUS_SERVER_ERROR
	);

	return XME_CORE_RR_STATUS_SUCCESS;
}


xme_core_status_t
xme_adv_mstLogger_createSystemStateList
(
	void* userData,
	xme_adv_mstManager_systemState_t* systemState,
	int mstNum
)
{
	xme_adv_mstLogger_mstSystemRequestListItem_t* item = NULL;
	int i;
	int sensorNum;
	uint8_t requestCount = 0;

	// Retrieve configuration structure
	xme_adv_mstLogger_configStruct_t* config = (xme_adv_mstLogger_configStruct_t*)userData;

	// destroy old list, if an old list exist
	if (NULL != config->mstSystemRequestList.head) XME_HAL_LINKEDLIST_FINI(config->mstSystemRequestList);

	// create singly linked list for mst system request list
	XME_HAL_LINKEDLIST_INIT(config->mstSystemRequestList);

	// enter critical section
	xme_hal_sync_enterCriticalSection(config->criticalSectionHandle);
#if DEBUG
	printf("\nlogger %d\n",config->componentID);
#endif

	for (i=0; i < mstNum; i++)
	{
		// Topic SystemState: sensorTypeID, interfaceTypeRef, assignedID, numChannels, installPositionInstanceID, installPositionRef
		// check every list element if it is the right mstType (e.g. Temperature or Pressure)
		if ( config->mstType == systemState[i].sensorType )
		{
			// Two possible modes:
			// ADC-Mode:		There have to be a modbus request for every sensor in a mst system (because the according holding registers
			//					are not next to each other (9005+9006; 9013+9014; ...) -> requestCount = number of channels of the mstSystem
			// Reading register: There have to be just one modbus request per mst system requestCount = 1

			requestCount = systemState[i].numChannels;
			if ( config->mstDataType == XME_ADV_MSTLOGGER_READING_VALUE ) requestCount = 1;
			
			for ( sensorNum=0; sensorNum < requestCount; sensorNum++)
			{			
				// allocate memory for item
				XME_CHECK
				(
					NULL !=
					(
						item = (xme_adv_mstLogger_mstSystemRequestListItem_t*)xme_hal_mem_alloc
						(
							sizeof(xme_adv_mstLogger_mstSystemRequestListItem_t)
						)
					),
					XME_CORE_STATUS_INTERNAL_ERROR
				);

				//read from topic "system state" and insert into item
				item->mstType = config->mstType;
				item->componentId = config->componentID;
				item->interfaceType = (xme_prim_modbusMaster_interfaceType_t)systemState[i].interfaceType;
				item->slaveID = systemState[i].slaveID;
				item->installPosition = systemState[i].installPosition;
				item->installPositionInstance = systemState[i].installPositionInstance;
				item->priority = 1;				
				item->requestType = XME_PRIM_MODBUSMASTER_READ;
				// TODO: add the sensor number to the metadata of rr if available
				item->data = sensorNum;

				// Add holding register number for adc values or input register number for readings
				if ( config->mstDataType == XME_ADV_MSTLOGGER_ADC_VALUE ) 
				{
					item->startRegister = 9005 + sensorNum*8;
					item->registerType = XME_PRIM_MODBUSMASTER_REGISTER_TYPE_HOLDING_REGISTERS;
					// count of uint16_t; one adc-holding register is uint16_t with hight and lowbyte = long -> count = 2
					item->count = 2;
				}
				else 
				{
					item->startRegister = sensorNum;
					item->registerType = XME_PRIM_MODBUSMASTER_REGISTER_TYPE_INPUT_REGISTERS;
					item->count = systemState[i].numChannels;
				}
#if DEBUG
				printf("add request for instance %d with start Reg %d\n", item->installPositionInstance, item->startRegister);
#endif
				// add item to mstSystemList
				XME_HAL_SINGLYLINKEDLIST_ADD_ITEM(config->mstSystemRequestList, item);
			}
		}
	}

	// leave critical section
	xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);

	#if DEBUG
		printf("mstLogger with component ID #%d: added %d modbusMaster request(s) to my mstSystemRequestList\n", config->componentID, config->mstSystemRequestList.count); 
	#endif

	return XME_CORE_STATUS_SUCCESS;
}


xme_core_status_t
xme_adv_mstLogger_linearizeTempData
(
	void* userData,
	long* data,
	xme_adv_mstLogger_topicData_t* topicData
)
{
	// parameter for liniearisazion
	double r0 = 100;
	double tFixWtp = 0.01;
	double tFixGa = 29.7646;
	// a =	3.91*10^-03;
	double a = 0.00391;
	// b =	-5.78*10^-07;
	double b = -0.000000578;
	// estimated !!!!
	double rMessWtp = 100.005;
	double rMessGa = 111.38;

	// variables for results
	double rMess = 0;
	double uAdc = 0;
	double tEqu = 0;
	double tEquRmessWtp = 0;
	double tEquRmessGa = 0;
	double c, d, temp = 0;

	long help = 0;
	help = *data;

	// U_ADC = 5V * ADC_VAL / 2^24;
	uAdc = 5*(double)help/(2<<23);
	// rMess = 5V * R_Pt100 / (R_PT100 + 2*10k)
	rMess = -20000*uAdc/( uAdc -5 );

	// TODO: optimaize calculations... z.b. precalculate some const. values, function for tEqu<x>...
	tEqu = ( -r0*a+ sqrt( r0*r0*a*a-4*r0*b*(r0-rMess) ))/(2*r0*b);
	tEquRmessWtp = ( -r0*a+ sqrt( r0*r0*a*a-4*r0*b*(r0-rMessWtp) ))/(2*r0*b);
	tEquRmessGa =  ( -r0*a+ sqrt( r0*r0*a*a-4*r0*b*(r0-rMessGa) ))/(2*r0*b);
	c = tFixWtp - tEquRmessWtp;
	d = ( tFixGa - c - tEquRmessGa )/(rMessGa-rMessWtp);
	temp = tEqu+c+d*(rMess-rMessWtp);

	topicData->data = (long)(temp*1000);

	return XME_CORE_STATUS_SUCCESS;
};

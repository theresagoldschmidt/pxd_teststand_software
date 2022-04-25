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
 *         Modbus Master.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Marcel Flesch <fleschmarcel@hotmail.com>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/prim/modbusMaster.h"

#include "xme/core/defines.h"
#include "xme/core/packet.h"
#include "xme/core/topic.h"

#include "xme/hal/linkedList.h"
#include "xme/hal/linkedList_arch.h"
#include "xme/hal/random.h"
#include "xme/hal/table.h"

// For Debug Mode
#define DEBUG 0
#define meanValue 2500		// meanValue *100
#define varianceValue 0.05	// variance *100
#define meanValueAdc 90843		// meanValue for adc values
#define varianceValueAdc 8.5569	// variance for adc values

/******************************************************************************/
/***   Forward declarations                                                 ***/
/******************************************************************************/
/**
 * \brief  Callback function for processing Modbus requests.
 *
 * \see    xme_hal_sched_taskCallback_t
 */
void
xme_prim_modbusMaster_taskCallback
(
	void* userData
);

// rr-SERVER
xme_core_rr_responseStatus_t
xme_prim_modbusMaster_receiveRequestCallback
(
	xme_core_topic_t requestTopic,
	void* requestData,
	xme_core_rr_responseInstanceHandle_t mstResponseInstanceHandle,
	void* responseData,
	uint16_t* responseSize,
	xme_hal_time_interval_t responseTimeoutMs,
	void* userData
);

// send modbus serial request
xme_prim_modbusMaster_status_t
xme_prim_modbusMaster_sendModbusSerialRequest
(
	void* userData,
	xme_prim_modbusMaster_priorityQueueItem_t* item,
	void* responseData
);

// send modbus simulator request
xme_prim_modbusMaster_status_t
xme_prim_modbusMaster_sendModbusSimulatorRequest
(
	void* userData,
	xme_prim_modbusMaster_priorityQueueItem_t* item,
	void* responseData
);

// send modbus tcp request
xme_prim_modbusMaster_status_t
xme_prim_modbusMaster_sendModbusTcpRequest
(
	void* userData,
	xme_prim_modbusMaster_priorityQueueItem_t* item,
	void* responseData
);

// read modbus serial input register
xme_prim_modbusMaster_status_t
xme_prim_modbusMaster_readSerialInputRegister
(
	void* userData,
	xme_prim_modbusMaster_priorityQueueItem_t* item,
	void* responseData
);

// read modbus serial holding register
xme_prim_modbusMaster_status_t
xme_prim_modbusMaster_readSerialHoldingRegister
(
	void* userData,
	xme_prim_modbusMaster_priorityQueueItem_t* item,
	void* responseData
);

// write modbus serial holding register
xme_prim_modbusMaster_status_t
xme_prim_modbusMaster_writeSerialRegister
(
	void* userData,
	xme_prim_modbusMaster_priorityQueueItem_t* item
);


/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_prim_modbusMaster_create(xme_prim_modbusMaster_configStruct_t* config)
{
	XME_ASSERT(NULL != config);
	
	// Initialize configuration structure
	config->criticalSectionHandle = XME_HAL_SYNC_INVALID_CRITICAL_SECTION_HANDLE;
	config->requestHandlerHandle = XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE;
	config->taskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;
	config->mb = NULL;
	
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
				&xme_prim_modbusMaster_receiveRequestCallback,
				config
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// create task xme_prim_modbusMaster_taskCallback
	XME_CHECK
	(
	 	XME_HAL_SCHED_INVALID_TASK_HANDLE !=
		(
			// This task suspends itself if it is not used, hence we can set
			// the period to the minimum time to make sure that the task
			// executes as soon as possible after resume
			// TODO: If the scheduling problem in xme is solved, set the period back 1ms
			// TODO: and change the code back (aktivate task in rr-callback, deaktivate it
			// TODO: in the task itself, if the request list is empty
			config->taskHandle = xme_core_resourceManager_scheduleTask
			(
				XME_HAL_SCHED_TASK_INITIALLY_SUSPENDED,
				1,
				XME_HAL_SCHED_PRIORITY_NORMAL,
				&xme_prim_modbusMaster_taskCallback,
				config
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// get current componentID
	config->componentID = xme_core_resourceManager_getCurrentComponentId();

	// Verzweigung
	switch(config->modbusType)
	{
		case XME_PRIM_MODBUSMASTER_SERIAL:	printf("modbusMaster with component ID #%d: I'm responsable for modbus serial interface\n", config->componentID);
											// check config
											XME_CHECK
											( 
												(config->comPort != NULL) && (config->baudrate != 0) && (config->parity != 0) && (config->dataBits != 0),
												XME_CORE_STATUS_OUT_OF_RESOURCES
											);

											// create modbus serial connection
											config->mb = modbus_new_rtu(config->comPort, config->baudrate, config->parity, config->dataBits, config->stopBits);
											XME_CHECK
											( 
												0 == modbus_connect(config->mb),
												XME_CORE_STATUS_OUT_OF_RESOURCES
											);
		break;
		case XME_PRIM_MODBUSMASTER_TCP:		printf("modbusMaster with component ID #%d: I'm responsable for modbus TCP interface\n", config->componentID);
											// TODO: check config
											// TODO: create modbus TCP connection
		break;
		case XME_PRIM_MODBUSMASTER_SIMULATOR:		printf("modbusMaster with component ID #%d: I'm responsable for modbus simulation\n", config->componentID); 
		break;
		default:							printf("modbusMaster with component ID #%d: I'm not sure, what type of modbus interface I'm responsable for\n", config->componentID);
	}  

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_prim_modbusMaster_activate(xme_prim_modbusMaster_configStruct_t* config)
{
	// TODO: remove this, if the schedlunging problem in xme is solved...
	xme_hal_sched_setTaskExecutionState(config->taskHandle, true);

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_prim_modbusMaster_deactivate(xme_prim_modbusMaster_configStruct_t* config)
{
	// TODO: Delete pending requests?

	xme_core_status_t result = xme_hal_sched_setTaskExecutionState(config->taskHandle, false);
	XME_ASSERT_NORVAL(XME_CORE_STATUS_SUCCESS == result);
}

void
xme_prim_modbusMaster_destroy(xme_prim_modbusMaster_configStruct_t* config)
{
	// TODO: Delete pending requests!
	xme_core_status_t result;

	// Clear the list
 	XME_HAL_LINKEDLIST_FINI(config->priorityQueue);

	// close modbus connection
	modbus_close(config->mb);
	modbus_free(config->mb);
	config->mb = NULL;

	// close task xme_prim_modbusMaster_taskCallback
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
xme_prim_modbusMaster_receiveRequestCallback
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
	xme_prim_modbusMaster_configStruct_t* config = (xme_prim_modbusMaster_configStruct_t*)userData;
	xme_prim_modbusMaster_priorityQueueItem_t* item;
	xme_prim_modbusMaster_requestData_t* copyRequestData;

#if DEBUG
	printf("modbusMaster with component ID #%d: received request\n   modbusInterfaceType: %d; slaveID: %d \n", config->componentID, ((xme_prim_modbusMaster_requestData_t*)requestData)->interfaceType, ((xme_prim_modbusMaster_requestData_t*)requestData)->slaveID );
#endif
	
	copyRequestData = (xme_prim_modbusMaster_requestData_t*)requestData;

	// check interface type of the request
	if ( config->modbusType != copyRequestData->interfaceType )
	{
		return (xme_core_rr_responseStatus_t)XME_PRIM_MODBUSMASTER_STATUS_NOT_RESPONSIBLE;
	}
	
	// allocate memory for item
	XME_CHECK
	(
		NULL !=
		(
			item = (xme_prim_modbusMaster_priorityQueueItem_t*)xme_hal_mem_alloc
			(
				sizeof(xme_prim_modbusMaster_priorityQueueItem_t)
			)
		),
		XME_CORE_RR_STATUS_SERVER_ERROR
	);

	// TODO: remove the meta data hack!
	// read request data an write it to local memory "item"
	item->componentIdOfSender = copyRequestData->componentId;
	item->priority = copyRequestData->priority;
	item->responseInstanceHandle = responseInstanceHandle;
	item->requestType = copyRequestData->requestType;
	item->registerType = copyRequestData->registerType;
	item->slaveID = copyRequestData->slaveID;
	item->startRegister = copyRequestData->startRegister;
	item->count = copyRequestData->count;
	item->requestData = copyRequestData->data;
	item->mstType = copyRequestData->mstType;

	// enter critical section -> update priority queue
	xme_hal_sync_enterCriticalSection(config->criticalSectionHandle);
	{			
		// add new item to sorted list
		XME_HAL_SINGLYLINKEDLIST_ADD_ITEM_SORTED(config->priorityQueue, item, offsetof(xme_prim_modbusMaster_requestData_t, priority), uint16_t);

		//activate xme_prim_modbusMaster_taskCallback for executing requests out of the priority queue
		// TODO: aktivate this, if the schedlunging problem in xme is solved
		//xme_hal_sched_setTaskExecutionState(config->taskHandle, true);
	}
	xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);
			
	return XME_CORE_RR_STATUS_RESPONSE_DELAYED;
}


// modbusMaster_taskCallback
void
xme_prim_modbusMaster_taskCallback
(	
	void* userData
)
{
	/*
	- test if a item is in queue
		- if yes: allocate response memory, take element with highest priority, remove it from queue, send modbus request, set exe-state of this task
		- if not: set exe-state of this task to sleep
	*/
		
	xme_prim_modbusMaster_configStruct_t* config = (xme_prim_modbusMaster_configStruct_t*)userData;
	xme_prim_modbusMaster_priorityQueueItem_t* item = NULL;
	xme_core_rr_responseStatus_t status;
	void* responseData = NULL;
	uint16_t sizeOfResponseData = 0;

	// TODO: remove the if-statement, if the scheduling problem in xme is solved
	if( XME_HAL_LINKEDLIST_ITEM_COUNT(config->priorityQueue) )
	{

		// enter critical section
		xme_hal_sync_enterCriticalSection(config->criticalSectionHandle);
		{
			// get item with highest priority
			XME_CHECK_REC
			(
				NULL != 
				( item = (xme_prim_modbusMaster_priorityQueueItem_t*)XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(config->priorityQueue, 0) ), ,
				status = XME_CORE_RR_STATUS_SERVER_ERROR
			);

			// remove item from list
			XME_CHECK_REC
			(
				1 == 
				XME_HAL_SINGLYLINKEDLIST_REMOVE_ITEM(config->priorityQueue, item, false), ,
				status = XME_CORE_RR_STATUS_SERVER_ERROR
			);

			// test if there are still elements left in queue, otherwise set exe-state of this task to false
			// TODO: aktivate this, if the scheduling problem in xme is solved...
			/*
			if ( !(XME_HAL_LINKEDLIST_ITEM_COUNT(config->priorityQueue)) )
			{
				XME_CHECK_REC
				(
					XME_CORE_STATUS_SUCCESS == 
					xme_hal_sched_setTaskExecutionState(config->taskHandle, false), ,
					status = XME_CORE_RR_STATUS_SERVER_ERROR
				);
			}
			*/
		}
		xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);

		// allocate memory for response: <xme_prim_modbusMaster_requestMetaData_t>, <requestedData>
		sizeOfResponseData = sizeof(xme_prim_modbusMaster_requestMetaData_t) + item->count*sizeof(uint16_t);
		XME_CHECK_REC
		(
			NULL !=
			( responseData = xme_hal_mem_alloc( sizeOfResponseData )), ,
			status = XME_CORE_RR_STATUS_SERVER_ERROR
		);

		// add response meta Data to the response data
		// TODO: remove "metaData-Hack"
		((uint16_t*)responseData)[0] = item->componentIdOfSender;
		((uint16_t*)responseData)[1] = item->slaveID;
		((uint16_t*)responseData)[2] = item->requestData;
		((uint16_t*)responseData)[3] = item->registerType;
		((uint16_t*)responseData)[4] = item->mstType;

		// send modbus response according to modbus interface type
		switch(config->modbusType)
		{
			case XME_PRIM_MODBUSMASTER_SERIAL:	status = (xme_core_rr_responseStatus_t)xme_prim_modbusMaster_sendModbusSerialRequest(config, item, responseData);
			break;
			case XME_PRIM_MODBUSMASTER_TCP:		
			{	
				status = XME_CORE_RR_STATUS_SERVER_ERROR;	// TODO: add function for modbus tcp
			}
			break;
			case XME_PRIM_MODBUSMASTER_SIMULATOR: 
				// Not supported any more, because now there is a component modbusSimulator
				status = XME_CORE_RR_STATUS_SERVER_ERROR;
				//status = (xme_core_rr_responseStatus_t)xme_prim_modbusMaster_sendModbusSimulatorRequest(config, item, responseData);
			break;
			default: status = XME_CORE_RR_STATUS_SERVER_ERROR;
		}
			
		// send response
		xme_core_rr_sendResponse(status, config->requestHandlerHandle, item->responseInstanceHandle, responseData, ( sizeOfResponseData ));

		// free memory of item
		xme_hal_mem_free(item);

		// free memory for resoponse data
		xme_hal_mem_free(responseData);
	}

}


// modbusMaster_sendModbusSerialRequest
xme_prim_modbusMaster_status_t
xme_prim_modbusMaster_sendModbusSerialRequest
(
	void* userData,
	xme_prim_modbusMaster_priorityQueueItem_t* item,
	void* responseData
)
{
	xme_prim_modbusMaster_configStruct_t* config = (xme_prim_modbusMaster_configStruct_t*)userData;
	xme_prim_modbusMaster_status_t status = XME_PRIM_MODBUSMASTER_STATUS_MODBUS_ERROR;
	
	// interprete request
	switch(item->requestType) 
	{
      case XME_PRIM_MODBUSMASTER_WRITE: 
		   {
			   xme_prim_modbusMaster_writeSerialRegister(config, item);
			   break;
		   }
      case XME_PRIM_MODBUSMASTER_READ:
		   {
				switch(item->registerType) 
				   {
					case XME_PRIM_MODBUSMASTER_REGISTER_TYPE_INPUT_BITS:			
						// TODO
						status = XME_PRIM_MODBUSMASTER_STATUS_SUCCESS;
						break;
					case XME_PRIM_MODBUSMASTER_REGISTER_TYPE_INPUT_REGISTERS:		
						status = xme_prim_modbusMaster_readSerialInputRegister(config, item, responseData);
						break;
					case XME_PRIM_MODBUSMASTER_REGISTER_TYPE_BITS:
						// TODO
						status = XME_PRIM_MODBUSMASTER_STATUS_SUCCESS;
						break;
					case XME_PRIM_MODBUSMASTER_REGISTER_TYPE_HOLDING_REGISTERS:
						status = xme_prim_modbusMaster_readSerialHoldingRegister(config, item, responseData);
						break;
					default: status = XME_PRIM_MODBUSMASTER_STATUS_UNEXPECTED_REGISTERTYPE;
					}
				break;
		   }
	  default:	status = XME_PRIM_MODBUSMASTER_STATUS_UNEXPECTED_REQUESTTYPE;
     }

	return status;
}  


// modbusMaster_sendModbusSimulatorRequest
xme_prim_modbusMaster_status_t
xme_prim_modbusMaster_sendModbusSimulatorRequest
(
	void* userData,
	xme_prim_modbusMaster_priorityQueueItem_t* item,
	void* responseData
)
{
	xme_prim_modbusMaster_configStruct_t* config = (xme_prim_modbusMaster_configStruct_t*)userData;
	xme_prim_modbusMaster_status_t status = XME_PRIM_MODBUSMASTER_STATUS_MODBUS_ERROR;
	uint16_t* copyResponseData = NULL;
	int j;
	int i;
	uint16_t randomNumber[2] = {0,0};
	long randomNumberCum = 0;

	// TODO: remove "metaData-Hack"
	copyResponseData = &((uint16_t*)responseData)[5];

	// interprete request
	switch(item->requestType) 
	{
      case XME_PRIM_MODBUSMASTER_WRITE: 
		   {
			   // not supported for Simulator
			   break;
		   }
      case XME_PRIM_MODBUSMASTER_READ:
		   {
				switch(item->registerType) 
				   {
					case XME_PRIM_MODBUSMASTER_REGISTER_TYPE_INPUT_BITS:			
						// not supported for Simulator
						status = XME_PRIM_MODBUSMASTER_STATUS_SUCCESS;
						break;
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
							copyResponseData[i] = (uint16_t)(meanValue + sqrt( (double)varianceValue )*( randomNumberCum/100 - 6));
						}
					
						status = XME_PRIM_MODBUSMASTER_STATUS_SUCCESS;
						break;
					case XME_PRIM_MODBUSMASTER_REGISTER_TYPE_BITS:
						// not supported for Simulator
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

						*(long*)randomNumber = (long)(meanValueAdc + sqrt( (double)varianceValueAdc )*( randomNumberCum/1000 - 6));
						// TODO: remove "metaData-Hack"
						copyResponseData[0] = randomNumber[1];
						copyResponseData[1] = randomNumber[0];

						status = XME_PRIM_MODBUSMASTER_STATUS_SUCCESS;
						break;
					default: status = XME_PRIM_MODBUSMASTER_STATUS_UNEXPECTED_REGISTERTYPE;
					}
				break;
		   }
	  default:	status = XME_PRIM_MODBUSMASTER_STATUS_UNEXPECTED_REQUESTTYPE;
     }
	return status;
}   


//modubsMaster_readSerialInputRegister
xme_prim_modbusMaster_status_t
xme_prim_modbusMaster_readSerialInputRegister
(
	void* userData,
	xme_prim_modbusMaster_priorityQueueItem_t* item,
	void* responseData
)
{
	xme_prim_modbusMaster_configStruct_t* config = (xme_prim_modbusMaster_configStruct_t*)userData;
	xme_prim_modbusMaster_status_t status = XME_PRIM_MODBUSMASTER_STATUS_MODBUS_ERROR;
	int dataSize = -1;
	uint16_t* copyResponseData = NULL;

	// TODO: remove "metaData-Hack"	
	copyResponseData = &((uint16_t*)responseData)[5];

#if DEBUG
	void* copyRespData;
	int i;
	printf("\nmodbusMaster: received reading request: slaveID: %d; startRegister: %d; count: %d\n", item->slaveID, item->startRegister, item->count  );
#endif

	// set modbus slave	
	XME_CHECK
	(
		XME_PRIM_MODBUSMASTER_STATUS_SUCCESS ==
		(
			modbus_set_slave
			(
				config->mb,
				item->slaveID
			)
		),
		status = XME_PRIM_MODBUSMASTER_STATUS_CONNECTION_FAILD;
	);

	// read input registers
	dataSize = modbus_read_input_registers(config->mb, item->startRegister, item->count, copyResponseData);

#if DEBUG
	copyRespData = copyResponseData;
	printf("modbusMaster: ");
	for(i=0; i<dataSize; i++)
	{
		printf("Reg%d: %0.2f ", i, (float)*(uint16_t*)copyRespData/100);
		copyRespData = (char*)copyRespData + sizeof(uint16_t);
	}
	printf("dataSize: %d\n\n", dataSize);
#endif

	// Catch Errors
	if ( -1 == dataSize ) 
	{
		status = XME_PRIM_MODBUSMASTER_STATUS_CONNECTION_FAILD;
	}
	else if ( item->count != dataSize )
	{
		status = XME_PRIM_MODBUSMASTER_STATUS_UNEXPECTED_DATASIZE;
	}
	else
	{
		status = XME_PRIM_MODBUSMASTER_STATUS_SUCCESS;
	}

	return status;
}


// modbusMaster_readSerialHoldingRegister
xme_prim_modbusMaster_status_t
xme_prim_modbusMaster_readSerialHoldingRegister
(
	void* userData,
	xme_prim_modbusMaster_priorityQueueItem_t* item,
	void* responseData
)
{
	xme_prim_modbusMaster_configStruct_t* config = (xme_prim_modbusMaster_configStruct_t*)userData;
	xme_prim_modbusMaster_status_t status = XME_PRIM_MODBUSMASTER_STATUS_MODBUS_ERROR;
	int dataSize = -1;
	uint16_t* copyResponseData = NULL;

	// TODO: remove "metaData-Hack"	
	copyResponseData = &((uint16_t*)responseData)[5];


#if DEBUG
	uint16_t* copyResponseDataInteger;
	char* copyResponseDataChar;
	int i;
	printf("\nmodbusMaster: received reading request: slaveID: %d; startRegister: %d; count: %d\n", item->slaveID, item->startRegister, item->count  );
#endif

	// set modbus slave	
	XME_CHECK
	(
		XME_PRIM_MODBUSMASTER_STATUS_SUCCESS ==
		(
			modbus_set_slave
			(
				config->mb,
				item->slaveID
			)
		),
		status = XME_PRIM_MODBUSMASTER_STATUS_CONNECTION_FAILD;
	);


	// read holding register
	dataSize = modbus_read_registers(config->mb, item->startRegister, item->count, copyResponseData);

	// Catch Errors
	if ( -1 == dataSize ) 
	{
		status = XME_PRIM_MODBUSMASTER_STATUS_CONNECTION_FAILD;
	}
	else if ( item->count != dataSize )
	{
		status = XME_PRIM_MODBUSMASTER_STATUS_UNEXPECTED_DATASIZE;
	}
	else
	{
		status = XME_PRIM_MODBUSMASTER_STATUS_SUCCESS;
	}


#if DEBUG
	copyResponseDataChar = (char*)copyResponseData;
	printf("\nmodbusMaster: red as string ");
	// read ASCII-Data
	for(i=0; i<dataSize*2; i++)
	{
		printf("%c",*copyResponseDataChar);
		copyResponseDataChar++;
	}
	printf(" ;dataSize: %d\n", dataSize);

	copyResponseDataInteger = copyResponseData;
	printf("modbusMaster: red as integer ");
	// read uint16-Data
	for(i=0; i<dataSize; i++)
	{
		printf("%d ", *copyResponseDataInteger);
		copyResponseDataInteger++;
	}
	printf(" ;dataSize: %d\n\n", dataSize);
#endif

	return status;
}


// modbusMaster_writeSerialRegister
xme_prim_modbusMaster_status_t
xme_prim_modbusMaster_writeSerialRegister
(
	void* userData,
	xme_prim_modbusMaster_priorityQueueItem_t* item
)
{
	xme_prim_modbusMaster_configStruct_t* config = (xme_prim_modbusMaster_configStruct_t*)userData;
	xme_prim_modbusMaster_status_t status = XME_PRIM_MODBUSMASTER_STATUS_MODBUS_ERROR;

#if DEBUG
	printf("\nmodbusMaster: received writing request: slaveID: %d; startRegister: %d; count: %d; data: %d\n", item->slaveID, item->startRegister, item->count, item->requestData);
#endif

	// set modbus slave	
	XME_CHECK
	(
		XME_PRIM_MODBUSMASTER_STATUS_SUCCESS ==
		(
			modbus_set_slave
			(
				config->mb,
				item->slaveID
			)
		),
		status = XME_PRIM_MODBUSMASTER_STATUS_CONNECTION_FAILD;
	);


	// write holding register
	XME_CHECK
	(
		item->count ==
		(
			modbus_write_register
			(
				config->mb,
				item->startRegister,
				item->requestData
			)
		),
		XME_PRIM_MODBUSMASTER_STATUS_WRITING_ERROR;
	);

	return XME_PRIM_MODBUSMASTER_STATUS_SUCCESS;
}



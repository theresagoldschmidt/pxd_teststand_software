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
 *         MST Manager.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Marcel Flesch <fleschmarcel@mytum.de>
 */


/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/adv/mstManager.h"
#include "xme/core/dcc.h"

// For Debug Mode
#define DEBUG 1

#define INSTALLPOSITION 2
/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Callback function for incoming data.
 *
 * \see    xme_core_dcc_receiveTopicCallback_t
 */

xme_core_status_t
xme_adv_mstManager_readDatabaseSystemState
(
	xme_adv_mstManager_configStruct_t* config
);

void
xme_adv_mstManager_checkForNewDeviceTask
(
	void* userData
);


xme_core_rr_responseStatus_t
xme_adv_mstManager_receiveTopicSystemErrorCallback
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
);


void
xme_adv_mstManager_receiveModbusMasterResponseCallback
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
xme_adv_mstManager_checkInventory
(
	void* userData,
	char* mstSystemType,
	uint8_t* mstSystemInterface
);

xme_core_status_t
xme_adv_mstManager_createInstanceAndSlaveId
(
	void* userData
);

xme_core_status_t
xme_adv_mstManager_loginNewDevice
(
	void* userData
);



xme_core_status_t
xme_adv_mstManager_addSystemToDatabase
(
	void* userData
);


xme_core_status_t
xme_adv_mstManager_removeSystemFromDatabase
(
	void* userData,
	uint8_t slaveId
);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_adv_mstManager_create(xme_adv_mstManager_configStruct_t* config)
{	
	int status = -1;
	// Initialize configuration structure
	config->topicHandle = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;
	config->dbConnection = NULL;

	// Check configuration
	XME_CHECK(XME_CORE_TOPIC_INVALID_TOPIC != config->listenTopic, XME_CORE_STATUS_INVALID_PARAMETER);

	// get current componentID
	config->componentID = xme_core_resourceManager_getCurrentComponentId();

	// set state checkForNewDeviceState
	config->checkForNewDeviceState = XME_ADV_MSTMANAGER_CHECK_SERIAL_MODBUSINTERFACE;

	printf("mstManager with component ID #%d: connect to database...\n", config->componentID);

	// create db-connection
	XME_CHECK
	(
	 	NULL != (config->dbConnection = mysql_init(NULL)),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// connect to database
	XME_CHECK_REC
	(
		NULL !=
		(
		 	mysql_real_connect(
				config->dbConnection,
				config->dbAddress,
				config->dbUserName,
				config->dbPassword,
				config->dbDatabase,
				0, NULL, 0
			)
		),
		XME_CORE_STATUS_CONNECTION_REFUSED,
		{
			mysql_close(config->dbConnection);
			config->dbConnection = NULL;
		}
	);

	// retreive system status from database
	XME_CHECK
	(
	 	XME_CORE_STATUS_SUCCESS == 
		(
			xme_adv_mstManager_readDatabaseSystemState
			(
				config
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// publish topic XME_CORE_TOPIC_SYSTEM_STATE
	XME_CHECK
	(
		XME_CORE_DCC_INVALID_PUBLICATION_HANDLE !=
		(
			config->dccPublicationHandle = xme_core_dcc_publishTopic
			(
				XME_CORE_TOPIC_SYSTEM_STATE,
				XME_CORE_MD_EMPTY_META_DATA,
				false,
				NULL
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);


	// publish request XME_CORE_TOPIC_MODBUS_REQUEST
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
				&xme_adv_mstManager_receiveModbusMasterResponseCallback,
				config
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);


	// create task checkForNewDevice
	XME_CHECK
	(
	 	XME_HAL_SCHED_INVALID_TASK_HANDLE !=
		(
			config->checkForNewDeviceTaskHandle = xme_core_resourceManager_scheduleTask
			(
				XME_HAL_SCHED_TASK_INITIALLY_SUSPENDED,
				config->checkForNewDeviceintervalMs,
				XME_HAL_SCHED_PRIORITY_NORMAL,
				&xme_adv_mstManager_checkForNewDeviceTask,
				config
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// listen topic "SYSTEM_ERROR"
	config->topicHandle = xme_core_dcc_subscribeTopic
	(
		XME_CORE_TOPIC_SYSTEM_ERROR,
		XME_CORE_MD_EMPTY_META_DATA,
		false,
		&xme_adv_mstManager_receiveTopicSystemErrorCallback,
		config
	);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_adv_mstManager_activate(xme_adv_mstManager_configStruct_t* config)
{
	// send Topic "systemState"
	#if DEBUG
		printf("\nmstManager: send topic 'systemState'\n");
	#endif

	XME_CHECK
	(
	XME_CORE_STATUS_SUCCESS ==
		(
			xme_core_dcc_sendTopicData
			(
				config->dccPublicationHandle,
				config->systemState,
				config->sizeofSystemState
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);	

	// start task checkForNewDevice
	XME_CHECK
	(
		XME_CORE_STATUS_SUCCESS == xme_hal_sched_setTaskExecutionState(config->checkForNewDeviceTaskHandle, true),
		XME_CORE_STATUS_INTERNAL_ERROR
	);
	config->checkForNewDeviceFlag = 1;

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_adv_mstManager_deactivate(xme_adv_mstManager_configStruct_t* config)
{
	// Nothing to do
}

void
xme_adv_mstManager_destroy(xme_adv_mstManager_configStruct_t* config)
{
	// stop task checkForNewDevice
	// TODO

	// free systemState
	
	// Free statement
	//xme_adv_mstManager_closeStatement(config);

	// Unsubscribe topic
	//xme_core_dcc_unsubscribeTopic(config->topicHandle);
	//config->topicHandle = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;

	// Disconnect from database
	mysql_close(config->dbConnection);
	config->dbConnection = NULL;
}

// readDatabaseSystemState
xme_core_status_t
xme_adv_mstManager_readDatabaseSystemState
(
	xme_adv_mstManager_configStruct_t* config
)
{
	char* query;
	int error;
	MYSQL_RES *result;
	MYSQL_ROW row;
	int num_fields;
	int num_rows;
	int len;
	int i;

	// define query for mySQL as string
	# define string	"\
					SELECT sensorTypeID, interfaceTypeRef, assignedID, numChannels, installPositionInstanceID, installPositionRef \
					FROM SystemState, InstallPositionInstances, Inventory, SensorTypes \
					WHERE	SystemState.installPositionInstanceRef = InstallPositionInstances.installPositionInstanceID \
					AND		Inventory.serialNumID = InstallPositionInstances.serialNumRef \
					AND		SensorTypes.sensorTypeID = Inventory.sensorTypeRef \
					"

	// allocate memory for query with size len + stringtermination
	len = sizeof(string);
	XME_CHECK
	(
	 	NULL !=
		(
			query = (char*)xme_hal_mem_alloc
			(
				len+1
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);
	
	// create query
	strcpy(query, string);

	#if DEBUG	
		printf("\nmstManager: get systemState from database -> ");
	#endif

	// send query to mysql
	mysql_query(config->dbConnection, query);
	// read out error
	error = mysql_errno(config->dbConnection);
	// save result
	result = mysql_store_result(config->dbConnection);
	// read out number of mstSystems
	num_rows = (int)mysql_num_rows(result);
	// read out numer of parameters in systemState
	num_fields = mysql_num_fields(result);

	#if DEBUG
		printf("dbErrorNum: %d\n", error );
	#endif

	// calculate sizeof systemState
	config->sizeofSystemState = num_rows * sizeof(xme_adv_mstManager_systemState_t);

	// allocate memory for systemState
	XME_CHECK
	(
		NULL !=
		(
		config->systemState = (xme_adv_mstManager_systemState_t*)xme_hal_mem_alloc(config->sizeofSystemState)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);
	
	// read mysql result and insert into systemState
	#if DEBUG
		printf("\nmstManager: sensorType, interfType, slaveID, numCh, instPosInst, instPos\n");
	#endif
	for (i=0; i<num_rows; i++)
	{
		// read result per row
		row = mysql_fetch_row(result);
		// insert system state
		config->systemState[i].sensorType = atoi( (const char*)row[0]);
		config->systemState[i].interfaceType = atoi( (const char*)row[1]);
		config->systemState[i].slaveID = atoi( (const char*)row[2]);
		config->systemState[i].numChannels = atoi( (const char*)row[3]);
		config->systemState[i].installPositionInstance = atoi( (const char*)row[4]);
		config->systemState[i].installPosition = atoi( (const char*)row[5]);
		#if DEBUG
			printf("mstManager: %s %s %s %s %s %s\n", row[0], row[1], row[2], row[3], row[4], row[5]);
		#endif
	}
	#if DEBUG
		printf("\n");
	#endif
	
	mysql_free_result(result);
	xme_hal_mem_free(query);

	return XME_CORE_STATUS_SUCCESS;
}


void
xme_adv_mstManager_checkForNewDeviceTask
(
	void* userData
)
{
	xme_adv_mstManager_configStruct_t* config = (xme_adv_mstManager_configStruct_t*)userData;
	xme_prim_modbusMaster_requestData_t mstRequestData;

	if ( config->checkForNewDeviceFlag )
	{
		// send modbusMaster request: test if there is a device on modbus ID 247

		// set modbus interface according to current state
		switch ( config->checkForNewDeviceState )
		{
			case	XME_ADV_MSTMANAGER_CHECK_SERIAL_MODBUSINTERFACE: 
					mstRequestData.interfaceType = XME_PRIM_MODBUSMASTER_SERIAL;
					printf("mstManager: send modbusMaster serial request checkForNewDevice\n");
					break;
			case	XME_ADV_MSTMANAGER_CHECK_TCP_MODBUSINTERFACE: 
					mstRequestData.interfaceType = XME_PRIM_MODBUSMASTER_TCP;
					printf("mstManager: send modbusMaster tcp request checkForNewDevice\n");
					break;			
			case	XME_ADV_MSTMANAGER_CHECK_SIMULATOR_MODBUSINTERFACE: 
					mstRequestData.interfaceType = XME_PRIM_MODBUSMASTER_SIMULATOR;
					printf("mstManager: send modbusMaster simulator request checkForNewDevice\n");
					break;
		};

		mstRequestData.componentId = config->componentID;
		mstRequestData.registerType = XME_PRIM_MODBUSMASTER_REGISTER_TYPE_HOLDING_REGISTERS;
		mstRequestData.requestType = XME_PRIM_MODBUSMASTER_READ;
		mstRequestData.slaveID = 247;
		mstRequestData.priority = 1;
		mstRequestData.count = 3;
		mstRequestData.data = config->checkForNewDeviceState;
		mstRequestData.startRegister = 223;
		
		// reset check for new device flag until the first responds from modbus
		config->checkForNewDeviceFlag = 0;

		// send request to modbusMaster
		xme_core_rr_sendRequest
		( 
			config->rrRequestHandle,  
			&mstRequestData,  
			sizeof(xme_prim_modbusMaster_requestData_t),
			config,  
			500
		);
	}	
};


void
xme_adv_mstManager_receiveModbusMasterResponseCallback
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
	xme_adv_mstManager_configStruct_t* config = (xme_adv_mstManager_configStruct_t*)userData;
	char* copyResponseData = NULL;
	xme_core_status_t loginStatus;
	xme_prim_modbusMaster_status_t modbusStatus;

	modbusStatus = (xme_prim_modbusMaster_status_t)status;


	// check status of modbus response
	switch ( modbusStatus )
	{
	case XME_PRIM_MODBUSMASTER_STATUS_SUCCESS:
		{
			if ( config->componentID == ((xme_prim_modbusMaster_requestMetaData_t*)responseData)->componentId && 247 == ((xme_prim_modbusMaster_requestMetaData_t*)responseData)->slaveId )
			{
				// there was an response on Id 247
				// stopp task checkForNewDevice until the new Sensor is logged into the system
				//xme_hal_sched_setTaskExecutionState(config->checkForNewDeviceTaskHandle, false);
				config->checkForNewDeviceFlag = 0;
				printf("mstManager: deaktivate checkForNewDevice task\n");

				// TODO: remove "responseMetaData-Hack"	
				// xme_prim_modbusMaster_requestMetaData_t ... are 5*uint16_t
				memcpy((void*)config->serNum , (void*)&((uint16_t*)responseData)[5], 6);
				config->serNum[6] = 0;

				// login new mstSystem
				loginStatus = xme_adv_mstManager_loginNewDevice(userData);

				if ( XME_CORE_STATUS_SUCCESS != loginStatus ) printf("mstManager: login failed\n");

				// set modbus interface according to current state
				switch ( config->checkForNewDeviceState )
				{
					case	XME_ADV_MSTMANAGER_CHECK_SERIAL_MODBUSINTERFACE:
						{	// set interface for the next check
							// TODO: set it to modbus_tcp, if available
							config->checkForNewDeviceState = XME_ADV_MSTMANAGER_CHECK_SIMULATOR_MODBUSINTERFACE;
						} break;
					case	XME_ADV_MSTMANAGER_CHECK_TCP_MODBUSINTERFACE:
						{
							config->checkForNewDeviceState = XME_ADV_MSTMANAGER_CHECK_SIMULATOR_MODBUSINTERFACE;
						} break;			
					case	XME_ADV_MSTMANAGER_CHECK_SIMULATOR_MODBUSINTERFACE:
						{
							config->checkForNewDeviceState = XME_ADV_MSTMANAGER_CHECK_SERIAL_MODBUSINTERFACE;
						} break;
				};
				// start task checkForNewDevice again
				config->checkForNewDeviceFlag = 1;
				printf("mstManager: aktivate checkForNewDevice task\n");
				//xme_hal_sched_setTaskExecutionState(config->checkForNewDeviceTaskHandle, true);
			}
		} break;
	case XME_PRIM_MODBUSMASTER_STATUS_CONNECTION_FAILD:
		{
			printf("mstManager: there is no device on Id247\n");
			// there is no new sensor on slave Id 247
			// set the check for new device flag again
			config->checkForNewDeviceFlag = 1;

			// set modbus interface according to current state
			switch ( config->checkForNewDeviceState )
			{
				case	XME_ADV_MSTMANAGER_CHECK_SERIAL_MODBUSINTERFACE:
					{	// set interface for the next check
						// TODO: set it to modbus_tcp, if available
						config->checkForNewDeviceState = XME_ADV_MSTMANAGER_CHECK_SIMULATOR_MODBUSINTERFACE;
					} break;
				case	XME_ADV_MSTMANAGER_CHECK_TCP_MODBUSINTERFACE:
					{
						config->checkForNewDeviceState = XME_ADV_MSTMANAGER_CHECK_SIMULATOR_MODBUSINTERFACE;
					} break;			
				case	XME_ADV_MSTMANAGER_CHECK_SIMULATOR_MODBUSINTERFACE:
					{
						config->checkForNewDeviceState = XME_ADV_MSTMANAGER_CHECK_SERIAL_MODBUSINTERFACE;
					} break;
			};

		} break;
	case XME_PRIM_MODBUSMASTER_STATUS_NOT_RESPONSIBLE:
		{
			printf("mstManager: not responsible response\n");
			// this is an answer from a component, which is not responsible for this request. Ignore it.
		} break;
	//default: // this status code is not handled by the manager
	}
}


xme_core_status_t
xme_adv_mstManager_checkInventory
(
	void* userData,
	char* mstSystemType,
	uint8_t* mstSystemInterface
)
{
	char* query;
	char* pos;
	int error;
	int len;
	MYSQL_RES* mySqlResult;
	MYSQL_ROW row;
	xme_adv_mstManager_configStruct_t* config = (xme_adv_mstManager_configStruct_t*)userData;

	// define query for mySQL as string
	# define strQuery1	"SELECT Inventory.sensorTypeRef, SensorTypes.typeName, Inventory.interfaceTypeRef FROM Inventory, SensorTypes WHERE Inventory.serialNumID = "
	# define strQuery2	" AND Inventory.sensorTypeRef = SensorTypes.sensorTypeID"

	// allocate memory for query
	XME_CHECK
	(
		NULL !=
		(
			query = (char*)xme_hal_mem_alloc( sizeof(strQuery1) + 6*sizeof(char) + sizeof(strQuery2) + 1 )
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// create query
	strcpy(query, strQuery1);
	pos = query;
	pos += strlen(strQuery1);
	strcpy(pos, config->serNum);
	pos += strlen(config->serNum);
	strcpy(pos, strQuery2);

	
	// send query to mysql
	mysql_query(config->dbConnection, query);
	// read out error
	error = mysql_errno(config->dbConnection);
	// save result
	mySqlResult = mysql_store_result(config->dbConnection);
	// read result per row
	if ( mySqlResult != NULL ) row = mysql_fetch_row(mySqlResult);

	// check for errors
	XME_CHECK
	(
		XME_CORE_STATUS_SUCCESS ==
		(
			(error != 0 ) || ( mysql_num_rows(mySqlResult) != 1)
		),
		XME_CORE_STATUS_INTERNAL_ERROR
	);

	// copy mstSystemType string from database
	len = strlen(mstSystemType);
	strcpy(mstSystemType, row[1]);
	// make shure the string from database is not to long with termination of the last character
	mstSystemType[len] = 0;
	// get mstSystemInterface
	*mstSystemInterface = atoi(row[2]);

	xme_hal_mem_free(query);
	mysql_free_result(mySqlResult);
	return XME_CORE_STATUS_SUCCESS;
};


xme_core_status_t
xme_adv_mstManager_createInstanceAndSlaveId
(
	void* userData
)
{
	char* query;
	char* pos;
	char installPosition[5];
	int error;
	MYSQL_RES* mySqlResult;
	MYSQL_ROW row;
	xme_adv_mstManager_configStruct_t* config = (xme_adv_mstManager_configStruct_t*)userData;
	
	// Query to ask Database for the new slave ID and the new installposition Instance
	# define queryInstanceAndId1 "SELECT 	case	when ( maxPrefID <= maxNotPrefID ) then maxPrefID \
													else maxNotPrefID \
													end as newSlaveID, \
											case	when ( ( 	SELECT max(installPositionInstanceID) as varMaxInstance \
																FROM InstallPositionInstances ) IS NULL  ) then 1 \
													else ( 		SELECT max(installPositionInstanceID) as varMaxInstance \
																FROM InstallPositionInstances ) +1 \
																end as newInstance \
									FROM \
									( \
										SELECT	case 	when ( max(assignedID) IS NULL ) then (	SELECT preferredIDFrom as varPreferredIDFrom \
																								FROM InstallPositions \
																								WHERE installPositionID = " // insert installpositon from user
	# define queryInstanceAndID2 ")						else max(assignedID) +1 \
														end  as maxPrefID \
										FROM SystemState \
										WHERE \
										( \
										assignedID >=	( 	SELECT preferredIDFrom as varPreferredIDFrom \
															FROM InstallPositions \
															WHERE installPositionID = "	// insert installpositon from user
	# define queryInstanceAndID3 ") 		) \
										AND \
										( \
										assignedID <	(	SELECT preferredIDTo as varPreferredIDTo \
															FROM InstallPositions \
															WHERE installPositionID = " // insert installpositon from user
	#define queryInstanceAndID4 ") \
										) \
									)tmp0, \
									( \
										SELECT	case when ( max(assignedID) IS NULL ) then ( 	SELECT max( preferredIDTo ) as varMaxPrefID \
																								FROM InstallPositions ) +1 \
													else max(assignedID) +1 \
													end  as maxNotPrefID \
										FROM SystemState \
										WHERE assignedID > ( 	SELECT max( preferredIDTo ) as varMaxPrefID \
																FROM InstallPositions ) \
									)tmp1;"


	// allocate memory for query
	XME_CHECK
	(
		NULL !=
		(
			// size of query: queryInstanceAndId1 + "<Number of Installposition>" = 1  + queryInstanceAndId2 + "<Number of Installposition>" = 1 + ... + termination of string
			query = (char*)xme_hal_mem_alloc( sizeof(queryInstanceAndId1) + sizeof(queryInstanceAndID2) + sizeof(queryInstanceAndID3) + sizeof(queryInstanceAndID4) +3 +1 )
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// convert config->newInstallPosition as uint8_t to string
	_itoa(config->newInstallPosition, installPosition, 10);
	
	////create query
	pos = query;
	// string1
	strcpy(pos, queryInstanceAndId1);
	pos += strlen(queryInstanceAndId1);
	// user input: install position
	strcpy(pos, installPosition);
	pos += strlen(installPosition);
	// string2
	strcpy(pos, queryInstanceAndID2);
	pos += strlen(queryInstanceAndID2);
	// user input: install position
	strcpy(pos, installPosition);
	pos += strlen(installPosition);
	// string3
	strcpy(pos, queryInstanceAndID3);
	pos += strlen(queryInstanceAndID3);
	// user input: install position
	strcpy(pos, installPosition);
	pos += strlen(installPosition);
	// string4
	strcpy(pos, queryInstanceAndID4);
	pos += strlen(queryInstanceAndID4);
	// termination of string
	strcpy(pos, "\0");

	//printf("\n\n%s\n\n", query);
	
	// send query to mysql
	mysql_query(config->dbConnection, query);
	// read out error
	error = mysql_errno(config->dbConnection);
	// save result
	mySqlResult = mysql_store_result(config->dbConnection);
	// read result per row
	row = mysql_fetch_row(mySqlResult);


	config->newSlaveId = atoi(row[0]);
	config->newInstallPositionInstance = atoi(row[1]);

	#if DEBUG
		printf("\nThe new Slave ID and Instance is:\n%d %d\n\n", config->newSlaveId, config->newInstallPositionInstance);
	#endif

	return XME_CORE_STATUS_SUCCESS;
};

xme_core_status_t
xme_adv_mstManager_loginNewDevice
(
	void* userData
)
{
	xme_adv_mstManager_configStruct_t* config = (xme_adv_mstManager_configStruct_t*)userData;
	char mstSystemType[20];
	//char getcharStr[2];
	uint8_t mstSystemInterface;
	xme_prim_modbusMaster_requestData_t mstRequestData;

	mstSystemType[19] = 0;
	
	// check if this sensor is in db "inventory", if yes give the sensortype back
	XME_CHECK_REC
	(
		XME_CORE_STATUS_SUCCESS ==
		(
			xme_adv_mstManager_checkInventory(userData, mstSystemType, &mstSystemInterface)
		),
			XME_CORE_STATUS_INVALID_CONFIGURATION,
			printf("mstManager: mstSystem with serial number %s is not in table inventory of the database\n", config->serNum);
	);

	// inform user:
	printf("\nmstManager: a mstSystem with serNum %s and sensorType %s logged into the system\n", config->serNum, mstSystemType);
	// ask user for install position of the new mstSystem
	printf("\nmstManager: install position number of the new device...?\n");

	// TODO: scanf seems to produce deadlocks sometimes...

	//scanf("%c",scanfStr);
	/*
	getcharStr[0] = getchar();
	getcharStr[1] = 0;
	config->newInstallPosition = (uint8_t)atoi(getcharStr);
	// deleate buffer
	do {getcharStr[0] = getchar();} while ( getcharStr[0] != '\n' );
	*/
	config->newInstallPosition = INSTALLPOSITION;


	#if DEBUG
		printf("\nmstManager: ask database for install position instance and slave ID for the new System on install position %u...\n", config->newInstallPosition);
	#endif

	// ask database to make an install position instance and a slave ID
	XME_CHECK_REC
	(
		XME_CORE_STATUS_SUCCESS ==
		(
			xme_adv_mstManager_createInstanceAndSlaveId(userData)
		),
		XME_CORE_STATUS_INVALID_CONFIGURATION,
		printf("\nmstManager: Error while creating intance of install position and create slaveID\n")
	);


	#if DEBUG
		printf("\nmstManager: add new System to database\n");
	#endif

	// add new System to database
	XME_CHECK_REC
	(
		XME_CORE_STATUS_SUCCESS ==
		(
			xme_adv_mstManager_addSystemToDatabase(userData)
		),
		XME_CORE_STATUS_INTERNAL_ERROR,
		printf("\nmstManager: Error: new system wasn't added to the system!\n")
	);


	// retreive system state from database
	XME_CHECK_REC
	(
	 	XME_CORE_STATUS_SUCCESS == 
		(
			xme_adv_mstManager_readDatabaseSystemState
			(
				config
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES,
		printf("\nmstManager: Error: systemstate wasn't received from database!\n")
	);

	mstRequestData.mstType = 1; // the mstType doesn't matter in this case, because the response is the serial numer anyway
	mstRequestData.interfaceType = mstSystemInterface;
	mstRequestData.registerType = XME_PRIM_MODBUSMASTER_REGISTER_TYPE_HOLDING_REGISTERS;
	mstRequestData.requestType = XME_PRIM_MODBUSMASTER_WRITE;
	mstRequestData.slaveID = 247;
	mstRequestData.priority = 1;
	mstRequestData.count = 1;
	mstRequestData.data = config->newSlaveId;
	mstRequestData.startRegister = 200;

	// send new slaveId to modbus
	xme_core_rr_sendRequest
	( 
		config->rrRequestHandle,  
		&mstRequestData,  
		sizeof(xme_prim_modbusMaster_requestData_t),
		config,  
		500
	);


	#if DEBUG
		printf("\nmstManager: Assign new Slave ID to the new System\n");
	#endif

	// free memory of responseData if it was allocated
	if (config->responseData != NULL)
	{
		xme_hal_mem_free( config->responseData );
		config->responseData = NULL;
	}


	#if DEBUG
		printf("\nmstManager: publish topic XME_CORE_TOPIC_SYSTEM_STATE\n");
	#endif

	// send topic XME_CORE_TOPIC_SYSTEM_STATE
	XME_CHECK
	(
	XME_CORE_STATUS_SUCCESS ==
		(
			xme_core_dcc_sendTopicData
			(
				config->dccPublicationHandle,
				config->systemState,
				config->sizeofSystemState
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);
	
	return XME_CORE_STATUS_SUCCESS;
};


xme_core_status_t
xme_adv_mstManager_addSystemToDatabase
(
	void* userData
)
{
	char* query;
	char* pos;
	int error;
	xme_adv_mstManager_configStruct_t* config = (xme_adv_mstManager_configStruct_t*)userData;

	
	// define query for mySQL as string
	# define systemStateQuery1	"INSERT INTO SystemState VALUES ("
	# define systemStateQuery2 " );"
	# define instanceQuery1	"INSERT INTO InstallPositionInstances  VALUES ("
	# define instanceQuery2 ");"


	//////////////////////////////////////////////////////////////////
	//INSERT INTO InstallPositionInstances  VALUES (12, 1, 0)
	//////////////////////////////////////////////////////////////////
	

	// allocate memory for query systemStateQuery
	XME_CHECK
	(
		NULL !=
		(	
			// uint8_t are at max 256, that means a uint16_t has at max 3 digits -> 3*sizeof(char)  
			query = (char*)xme_hal_mem_alloc( sizeof(systemStateQuery1) + 3*sizeof(char) +2 + 3*sizeof(char) + sizeof(systemStateQuery1) + 1 )
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// create query systemStateQuery
	pos = query;
	strcpy(query, systemStateQuery1);
	pos += strlen(systemStateQuery1);
	// add "<slaveID>, <installPositionInstance>"
	strcpy(pos, _itoa(config->newSlaveId, pos, 10) );
	pos += strlen( _itoa(config->newSlaveId, pos, 10) );
	strcpy(pos, ", ");
	pos += strlen(", ");
	strcpy(pos, _itoa( config->newInstallPositionInstance, pos, 10) );
	pos += strlen( _itoa(config->newInstallPositionInstance, pos, 10) );
	strcpy(pos, systemStateQuery2);

	// send query to mysql
	mysql_query(config->dbConnection, query);
	// read out error
	XME_CHECK
	( 
		0 == (error = mysql_errno(config->dbConnection) ),
		XME_CORE_STATUS_INTERNAL_ERROR
	);
	
	xme_hal_mem_free(query);

	//////////////////////////////////////////////////////////////////
	// INSERT INTO InstallPositionInstances  VALUES (12, 1, 0)
	//////////////////////////////////////////////////////////////////

	// allocate memory for query InstallPositionInstances
	XME_CHECK
	(
		NULL !=
		(	
			// uint8_t are at max 256, that means a uint16_t has at max 3 digits -> 3*sizeof(char)  
			query = (char*)xme_hal_mem_alloc( sizeof(instanceQuery1) + 3*sizeof(char) +2 + 3*sizeof(char) +2 + 3*sizeof(char) + sizeof(instanceQuery2) + 1 )
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// create query installPositionInstancesQuery
	pos = query;
	strcpy(query, instanceQuery1);
	pos += strlen(instanceQuery1);
	// add "<installPositionInstanceID>, <installPositionRef>, <serialNumRef>"
	strcpy(pos, _itoa(config->newInstallPositionInstance, pos, 10) );
	pos += strlen( _itoa(config->newInstallPositionInstance, pos, 10) );
	strcpy(pos, ", ");
	pos += strlen(", ");
	strcpy(pos, _itoa( config->newInstallPosition, pos, 10) );
	pos += strlen( _itoa(config->newInstallPosition, pos, 10) );
	strcpy(pos, ", ");
	pos += strlen(", ");
	strcpy(pos, config->serNum );
	pos += strlen( config->serNum );
	strcpy(pos, systemStateQuery2);

	// send query to mysql
	mysql_query(config->dbConnection, query);
	// read out error
	XME_CHECK
	( 
		0 == (error = mysql_errno(config->dbConnection) ),
		XME_CORE_STATUS_INTERNAL_ERROR
	);
	
	xme_hal_mem_free(query);



	return XME_CORE_STATUS_SUCCESS;
};


xme_core_rr_responseStatus_t
xme_adv_mstManager_receiveTopicSystemErrorCallback
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
)
{
	xme_adv_mstManager_configStruct_t* config = (xme_adv_mstManager_configStruct_t*)userData;
	xme_adv_mstManager_systemError_t* systemError;
	uint8_t slaveId;
	
	// get Topic Data from 'systemError' topic
	systemError = (xme_adv_mstManager_systemError_t*)xme_hal_sharedPtr_getPointer(dataHandle);


	slaveId = systemError->data;

	#if DEBUG
		printf("\n\nmstManager: a systemError occurred with errorNum: %d\n", systemError->status);
		printf("mstManager: a mstSystem with slaveID: %d has disconnected from modbus\n", systemError->data);
	#endif


	#if DEBUG
		printf("mstManager: remove system from database\n");
	#endif

	// remove system from database
	XME_CHECK
	(
		XME_CORE_STATUS_SUCCESS ==
		(
			xme_adv_mstManager_removeSystemFromDatabase
			(
				config, 
				(uint8_t)slaveId
			)
		),
		XME_CORE_RR_STATUS_SERVER_ERROR
	);

	
	// retreive system status from database
	XME_CHECK
	(
	 	XME_CORE_STATUS_SUCCESS == 
		(
			xme_adv_mstManager_readDatabaseSystemState
			(
				config
			)
		),
		XME_CORE_RR_STATUS_SERVER_ERROR
	);


	#if DEBUG
		printf("mstManager: publish topic XME_CORE_TOPIC_SYSTEM_STATE\n");
	#endif

	// send topic XME_CORE_TOPIC_SYSTEM_STATE
	XME_CHECK
	(
	XME_CORE_STATUS_SUCCESS ==
		(
			xme_core_dcc_sendTopicData
			(
				config->dccPublicationHandle,
				config->systemState,
				config->sizeofSystemState
			)
		),
		XME_CORE_RR_STATUS_SERVER_ERROR
	);

	return XME_CORE_RR_STATUS_SUCCESS;
};


xme_core_status_t
xme_adv_mstManager_removeSystemFromDatabase
(
	void* userData,
	uint8_t slaveId
)
{
	char* query;
	char* pos;
	int error;
	xme_adv_mstManager_configStruct_t* config = (xme_adv_mstManager_configStruct_t*)userData;

	
	// define query for mySQL as string
	# define removeSystemQuery1	"DELETE FROM InstallPositionInstances, SystemState \
								USING InstallPositionInstances, SystemState \
								WHERE ( assignedID = " // add slave ID of the system
	# define removeSystemQuery2 " AND installPositionInstanceRef = installPositionInstanceID ) \
								 AND ( assignedID = " // add slave ID of the system
	# define removeSystemQuery3 " );"



	// allocate memory for query removeSystem
	XME_CHECK
	(
		NULL !=
		(	
			// slaveID is uint8_t (at max 256), that means a uint16_t has at max 3 digits -> 3*sizeof(char); 1: termination of string
			query = (char*)xme_hal_mem_alloc( sizeof(removeSystemQuery1) + 2*3*sizeof(char) + sizeof(removeSystemQuery2) + sizeof(removeSystemQuery3) + 1 )
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// create query removeSystem
	pos = query;
	strcpy(pos, removeSystemQuery1);
	pos += strlen(removeSystemQuery1);
	// add "<slaveID>"
	strcpy(pos, _itoa(slaveId, pos, 10) );
	pos += strlen( _itoa(slaveId, pos, 10) );
	strcpy(pos, removeSystemQuery2);
	pos += strlen(removeSystemQuery2);
	// add "<slaveID>"
	strcpy(pos, _itoa(slaveId, pos, 10) );
	pos += strlen( _itoa(slaveId, pos, 10) );
	strcpy(pos, removeSystemQuery3);
	pos += strlen(removeSystemQuery3);

	// send query to mysql
	mysql_query(config->dbConnection, query);
	// read out error
	XME_CHECK
	( 
		0 == (error = mysql_errno(config->dbConnection) ),
		XME_CORE_STATUS_INTERNAL_ERROR
	);
	
	xme_hal_mem_free(query);

	return XME_CORE_STATUS_SUCCESS;
};
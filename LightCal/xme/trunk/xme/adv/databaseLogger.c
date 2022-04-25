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
 *         Database Logger.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Marcel Flesch <fleschmarcel@mytum.de>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/adv/databaseLogger.h"
#include "xme/core/dcc.h"

// For Debug Mode
#define DEBUG 0

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Callback function for incoming data.
 *
 * \see    xme_core_dcc_receiveTopicCallback_t
 */
void
xme_adv_databaseLogger_receiveTopicCallback
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
);

xme_core_status_t
xme_adv_databaseLogger_dataToMysql
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
);

xme_core_status_t
xme_adv_databaseLogger_initStatement
(
	xme_adv_databaseLogger_configStruct_t* config
);

void
xme_adv_databaseLogger_closeStatement
(
	xme_adv_databaseLogger_configStruct_t* config
);

xme_core_status_t
xme_adv_databaseLogger_createTable
(
	xme_adv_databaseLogger_configStruct_t* config
);



/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_adv_databaseLogger_create(xme_adv_databaseLogger_configStruct_t* config)
{
	// Initialize configuration structure
	config->topicHandle = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;
	config->dbConnection = NULL;
	config->columnCount = 0;
	config->insertStatement = NULL;
	config->insertBind = NULL;
	config->insertLengths = NULL;

	// Check configuration
	XME_CHECK(XME_CORE_TOPIC_INVALID_TOPIC != config->listenTopic, XME_CORE_STATUS_INVALID_PARAMETER);
	XME_CHECK(NULL != config->tableName, XME_CORE_STATUS_INVALID_PARAMETER);
	XME_CHECK(NULL != config->columnNames, XME_CORE_STATUS_INVALID_PARAMETER);
	XME_CHECK(NULL != config->columnTypes, XME_CORE_STATUS_INVALID_PARAMETER);

	
	// get current componentID
	config->componentID = xme_core_resourceManager_getCurrentComponentId();


	if( config->listenTopic == XME_CORE_TOPIC_TEMPERATURE) printf("databaseLogger with component ID #%d: I'm responsable for temperature data; create database connection...\n", config->componentID);
	else if ( config->listenTopic == XME_CORE_TOPIC_PRESSURE) printf("databaseLogger with component ID #%d: I'm responsable for pressure data; create database connection...\n", config->componentID);
	else if ( config->listenTopic == XME_CORE_TOPIC_TEMPFUSION) printf("databaseLogger with component ID #%d: I'm responsable for temperatur fusion data; create database connection...\n", config->componentID);
	else if ( config->listenTopic == XME_CORE_TOPIC_PRESFUSION) printf("databaseLogger with component ID #%d: I'm responsable for pressure fusion data; create database connection...\n", config->componentID);		
	
	// create db-connection
	XME_CHECK
	(
	 	NULL != (config->dbConnection = mysql_init(NULL)),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// subscribe topic
	XME_CHECK_REC
	(
		XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE !=
		(
			config->topicHandle = xme_core_dcc_subscribeTopic
			(
				config->listenTopic,
				XME_CORE_MD_EMPTY_META_DATA,
				false,
				&xme_adv_databaseLogger_receiveTopicCallback,
				config
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES,
		{
			mysql_close(config->dbConnection);
			config->dbConnection = NULL;
		}
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
	
	// create handle for critical section
	config->criticalSectionHandle = xme_hal_sync_createCriticalSection();

	// Create table if it doesn't exist
	XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_adv_databaseLogger_createTable(config), XME_CORE_STATUS_INTERNAL_ERROR);

	// Prepare insert query statement
	XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_adv_databaseLogger_initStatement(config), XME_CORE_STATUS_INTERNAL_ERROR);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_adv_databaseLogger_activate(xme_adv_databaseLogger_configStruct_t* config)
{
	// Nothing to do
	return XME_CORE_STATUS_SUCCESS;
}

void
xme_adv_databaseLogger_deactivate(xme_adv_databaseLogger_configStruct_t* config)
{
	// Nothing to do
}

void
xme_adv_databaseLogger_destroy(xme_adv_databaseLogger_configStruct_t* config)
{
	// Free statement
	xme_adv_databaseLogger_closeStatement(config);

	// Unsubscribe topic
	xme_core_dcc_unsubscribeTopic(config->topicHandle);
	config->topicHandle = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;

	// Disconnect from database
	mysql_close(config->dbConnection);
	config->dbConnection = NULL;
}

void
xme_adv_databaseLogger_receiveTopicCallback
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
)
{
	// Write data to database
	xme_adv_databaseLogger_dataToMysql(dataHandle, userData);
}

xme_core_status_t
xme_adv_databaseLogger_initStatement
(
	xme_adv_databaseLogger_configStruct_t* config
)
{
	char* query;
	unsigned int len;
	unsigned int timestampCount;
	char* pos;
	const char* posColTyp;
	unsigned int i;

	// Assert preconditions
	XME_ASSERT(NULL != config->dbConnection);
	XME_ASSERT(NULL == config->insertStatement);
	XME_ASSERT(NULL == config->insertBind);
	XME_ASSERT(config->columnCount > 0);

	// QUERY: "INSERT INTO `table` VALUES (?, ?)"
	
	// allocate memory for query
	// 13 = "INSERT INTO `"; ...; 9 = "` VALUES ("; ...; *2=", "; 1=")"
	len = 13 + strlen(config->tableName) + 10 + config->columnCount + (config->columnCount-1)*2 +1;

	// check, if there are timestamp columns, and if, add for every timestamp column 4 Bytes to the query allocation memory ( ?=1 Byte; NOW()=5 Byte, 4 Bytes are missing...)
	pos = (char*)config->columnTypes;
	timestampCount = 0;
	for (i=0; i< config->columnCount; i++)
	{
		if( strncmp(pos, "timestamp", 9) == 0 ) 
		{
			len += 4;
			timestampCount++;
		}
		pos += strlen(pos)+1;
	}

	// allocate memory
	query = (char*)xme_hal_mem_alloc(len+1);	
	
	// create query
	pos = query;
	strcpy(pos, "INSERT INTO `");
	pos += strlen("INSERT INTO `");
	strcpy(pos, config->tableName);
	pos += strlen(config->tableName);
	strcpy(pos, "` VALUES (");
	pos += strlen("` VALUES (");
	posColTyp = config->columnTypes;
	for (i=0; i<config->columnCount; i++)
	{
		if (i > 0)
		{
			strcpy(pos, ", ");
			pos += strlen(", ");
		}
		
		// insert ? if config->columnNames != TIMESTAMP
		if ( (strncmp(posColTyp, "timestamp", 9)) == 0 )
		{
			// insert "NOW()" into the query for current timestamp in database
			strcpy(pos, "NOW()");
			pos += strlen("NOW()");
		}
		else
		{
			strcpy(pos, "?");
			pos += strlen("?");
		}
		posColTyp += strlen(posColTyp)+1; 		
	}
	strcpy(pos, ")");
	pos += strlen(")");

	// enter critical section
	xme_hal_sync_enterCriticalSection(config->criticalSectionHandle);

	
	// Initialize the statement
	XME_CHECK_REC
	(
		NULL != (config->insertStatement = mysql_stmt_init(config->dbConnection)),
		XME_CORE_STATUS_INTERNAL_ERROR,
		{
			xme_hal_mem_free(query);
		}
	);

	// Prepare the statement
	XME_CHECK_REC
	(
		0 == mysql_stmt_prepare(config->insertStatement, query, strlen(query)),
		XME_CORE_STATUS_INTERNAL_ERROR,
		{
			xme_adv_databaseLogger_closeStatement(config);
			xme_hal_mem_free(query);
		}
	);
	xme_hal_mem_free(query);

	// enter critical section
	xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);

	// Bind data for parameters
	config->insertBind = (MYSQL_BIND*)xme_hal_mem_alloc( (config->columnCount-timestampCount)*sizeof(MYSQL_BIND));
	memset(config->insertBind, 0, (config->columnCount-timestampCount)*sizeof(MYSQL_BIND));

	// Allocate memory for length pointers in bind structure
	config->insertLengths = (unsigned long*)xme_hal_mem_alloc(config->columnCount*sizeof(unsigned long));
	memset(config->insertLengths, 0, config->columnCount*sizeof(unsigned long));
	
	return XME_CORE_STATUS_SUCCESS;
}

void
xme_adv_databaseLogger_closeStatement
(
	xme_adv_databaseLogger_configStruct_t* config
)
{
	// Assert preconditions
	XME_ASSERT_NORVAL(NULL != config->dbConnection);
	XME_ASSERT_NORVAL(NULL != config->insertStatement);
	XME_ASSERT_NORVAL(NULL != config->insertBind);

	xme_hal_mem_free(config->insertLengths);
	config->insertLengths = NULL;

	xme_hal_mem_free(config->insertBind);
	config->insertBind = NULL;

	mysql_stmt_close(config->insertStatement);
	config->insertStatement = NULL;
}

xme_core_status_t
xme_adv_databaseLogger_createTable
(
	xme_adv_databaseLogger_configStruct_t* config
)
{
	char* query;
	char* pos;
	int err=0;
	int len = 0;
	int thisLen = 0;
	int colCount = 0;
	const char* colName;
	const char* colTyp;
	int i;

	// Assert preconditions
	XME_ASSERT(NULL != config->dbConnection);
	XME_ASSERT(NULL != config->tableName);
	XME_ASSERT(NULL != config->columnTypes);

	// Calc strlen( <columnNames> + <columnTypes> ) for query-string
	colName = config->columnNames;
	colTyp = config->columnTypes;
	colCount = 0;
	while( strlen(colName) != 0 )
	{
		// get lenght of column Name
		thisLen = strlen(colName);
		len += thisLen;
		colName += thisLen + 1;
		// get lenght of column Type
		thisLen = strlen(colTyp);
		len += thisLen;
		colTyp += thisLen +1;
		colCount++;
	}
	// allocate memory for query
	// 14 = "CREATE TABLE `"; ...; 4 = "` ( "; ...; ...; 3="`"+"` "; 2= " )"; ...; + 2=" ," + 12= "TYPE=innodb"
	len = 14 + strlen(config->tableName) + 4 + len + colCount*3 + 2 + (colCount-1)*2 +12;
	query = (char*)xme_hal_mem_alloc(len+1);

	// Apply column count into configuratiom structure
	config->columnCount = colCount;

	//create query
	pos = query;
	strcpy(pos, "CREATE TABLE `");
	pos += strlen("CREATE TABLE `");
	strcpy(pos, config->tableName);
	pos += strlen(config->tableName);
	strcpy(pos, "` ( ");
	pos += strlen("` ( ");
	// add colName and colType to query
	colName = config->columnNames;
	colTyp = config->columnTypes;
	for (i=0; i < colCount; i++)
	{
		// insert colName
		strcpy(pos, "`");
		pos += strlen("`");
		strcpy(pos, colName);
		pos += strlen(colName);
		colName += strlen(colName) +1;
		strcpy(pos, "` ");
		pos += strlen("` ");
		// insert colTyp
		strcpy(pos, colTyp);
		pos += strlen(colTyp);
		colTyp += strlen(colTyp) +1;
		// insert comma between col Type and new colName
		if (i < (colCount-1) )
		{
			strcpy(pos, ", ");
			pos += strlen(", ");
		}
	}
	strcpy(pos, " ) TYPE=innodb");

	// enter critical section
	xme_hal_sync_enterCriticalSection(config->criticalSectionHandle);

	if ( mysql_query(config->dbConnection, query) )
	{
		if ( (1050 == mysql_errno(config->dbConnection) ))
		{
			//printf("Error 1050: Table '%s' already exists\n", config->tableName);
		}
		else if( (1064 == mysql_errno(config->dbConnection) ))
		{
			//printf("Error 1064: You have an error in your SQL syntax\n");
		}
		else 
		{
			printf("Error %d: unknown MySQL Error-Code\n", mysql_errno(config->dbConnection) );
			xme_hal_mem_free(query);
			return XME_CORE_STATUS_INTERNAL_ERROR;
		}
	}

	// leave critical section
	xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);
	
	xme_hal_mem_free(query);
	return XME_CORE_STATUS_SUCCESS;
}


xme_core_status_t
xme_adv_databaseLogger_dataToMysql
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
)
{
	unsigned int i;
	unsigned int timestampCount;
	void* data;
	const char* pos;
	unsigned long TopicBufferSize;
	unsigned long ReadoutBufferSize;
	
	// Retrieve configuration structure
	xme_adv_databaseLogger_configStruct_t* config = (xme_adv_databaseLogger_configStruct_t*)userData;

	// Assert preconditions
	XME_ASSERT_RVAL(NULL != config->dbConnection, XME_CORE_STATUS_INTERNAL_ERROR);
	XME_ASSERT_RVAL(NULL != config->tableName, XME_CORE_STATUS_INTERNAL_ERROR);
	XME_ASSERT_RVAL(NULL != config->columnTypes, XME_CORE_STATUS_INTERNAL_ERROR);
	XME_ASSERT_RVAL(NULL != config->insertStatement, XME_CORE_STATUS_INTERNAL_ERROR);
	XME_ASSERT_RVAL(NULL != config->insertBind, XME_CORE_STATUS_INTERNAL_ERROR);

	// get Topic Data from specific topic
	data = xme_hal_sharedPtr_getPointer(dataHandle);
	// check dataSize
	TopicBufferSize = xme_hal_sharedPtr_getSize(dataHandle);
	// check data-type and bind data
	pos = config->columnTypes;
	ReadoutBufferSize = 0;
	timestampCount = 0;
	for (i=0; i < config->columnCount-timestampCount; i++)
	{
		if (strncmp(pos, "int(", 4) == 0 && (ReadoutBufferSize < TopicBufferSize) )
		{
			// integer-type
			config->insertLengths[i] = 2;
			config->insertBind[i].buffer_type = MYSQL_TYPE_SHORT;
			config->insertBind[i].buffer = (uint16_t*)data;
			config->insertBind[i].buffer_length = config->insertLengths[i];
			config->insertBind[i].length = &config->insertLengths[i];
			config->insertBind[i].is_null = 0;
			ReadoutBufferSize += config->insertLengths[i];
			data = (char*)data + config->insertLengths[i];
		}
		else if ( (strncmp(pos, "varchar(", 8) == 0 ) && (ReadoutBufferSize < TopicBufferSize) )
		{
			// string-type
			config->insertLengths[i] = strlen((const char*)data);
			config->insertBind[i].buffer_type = MYSQL_TYPE_STRING;
			config->insertBind[i].buffer = (char*)data;
			config->insertBind[i].buffer_length = config->insertLengths[i]+1;
			config->insertBind[i].length = &config->insertLengths[i];
			config->insertBind[i].is_null = 0;
			ReadoutBufferSize += config->insertLengths[i]+1;
			data = (char*)data + config->insertLengths[i]+1;
		}
		else if ( (strncmp(pos, "timestamp", 9) == 0) && (ReadoutBufferSize < TopicBufferSize) )
		{
			// timestamp-type
			// timestamp from database, no data have to be bind
			timestampCount++;
			i--;
		}
		else
		{
			// ERROR
			printf("databaseLogger with component ID #%d: Error, database seems to be inconsistent; unsupported column data type or buffer size of topic data\n", config->componentID);
			mysql_close(config->dbConnection);
			config->dbConnection = NULL;
			return XME_CORE_STATUS_INTERNAL_ERROR;	
			
		}
		pos += strlen(pos)+1;
	}

	// enter critical section
	xme_hal_sync_enterCriticalSection(config->criticalSectionHandle);

	// bind structure
	mysql_stmt_bind_param(config->insertStatement, config->insertBind);

	XME_CHECK_REC
	(
		!mysql_errno(config->dbConnection),
		XME_CORE_STATUS_INTERNAL_ERROR,
		{
			printf("MySQL-Error: %d\n", mysql_errno(config->dbConnection) );
			mysql_close(config->dbConnection);
			config->dbConnection = NULL;
		}
	);

	// Execute the statement
#if DEBUG
	printf("databaseLogger with component ID #%d: send data to database\n", config->componentID);
#endif
	mysql_stmt_execute(config->insertStatement);

	XME_CHECK_REC
	(
		!mysql_errno(config->dbConnection),
		XME_CORE_STATUS_INTERNAL_ERROR,
		{
			printf("MySQL-Error: %d\n", mysql_errno(config->dbConnection) );
			mysql_stmt_close(config->insertStatement);
			mysql_close(config->dbConnection);
			config->dbConnection = NULL;
		}
	);

	// leave critical section
	xme_hal_sync_leaveCriticalSection(config->criticalSectionHandle);

	return XME_CORE_STATUS_SUCCESS;
}

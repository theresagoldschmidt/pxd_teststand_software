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

#ifndef XME_ADV_DATABASELOGGER_H
#define XME_ADV_DATABASELOGGER_H

// TODO: This is currently required for MySQL to compile on VS10
#define _CRT_SECURE_NO_WARNINGS

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/dcc.h"
#include "xme/core/resourceManager.h"

#include <my_global.h>
#include <mysql.h>

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_adv_databaseLogger_configStruct_t
 *
 * \brief  Database logger configuration structure.
 */
typedef struct
{
	// public
	xme_core_topic_t listenTopic;
	const char* dbAddress;
	const char* dbDatabase;
	const char* dbUserName;
	const char* dbPassword;
	const char* tableName;
	const char* columnNames;
	const char* columnTypes;
	// private
	xme_core_dcc_subscriptionHandle_t topicHandle;
	MYSQL* dbConnection;
	unsigned int columnCount;
	unsigned int mysqlError;
	MYSQL_STMT* insertStatement;
	MYSQL_BIND* insertBind;
	unsigned long* insertLengths;
	uint16_t componentID;
	xme_hal_sync_criticalSectionHandle_t criticalSectionHandle;					///< critical section handle
}
xme_adv_databaseLogger_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates a database logger component.
 */
xme_core_status_t
xme_adv_databaseLogger_create(xme_adv_databaseLogger_configStruct_t* config);

/**
 * \brief  Activates a database logger component.
 */
xme_core_status_t
xme_adv_databaseLogger_activate(xme_adv_databaseLogger_configStruct_t* config);

/**
 * \brief  Deactivates a database logger component.
 */
void
xme_adv_databaseLogger_deactivate(xme_adv_databaseLogger_configStruct_t* config);

/**
 * \brief  Destroys a database logger component.
 */
void
xme_adv_databaseLogger_destroy(xme_adv_databaseLogger_configStruct_t* config);

#endif // #ifndef XME_ADV_DATABASELOGGER_H

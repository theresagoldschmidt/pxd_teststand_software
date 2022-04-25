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

#ifndef XME_ADV_mstManager_H
#define XME_ADV_mstManager_H

// TODO: This is currently required for MySQL to compile on VS10
#define _CRT_SECURE_NO_WARNINGS

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/dcc.h"

#include "xme/core/resourceManager.h"
#include "xme/core/rr.h"
#include "xme/core/topic.h"
#include "xme/prim/modbusMaster.h"


//#include <my_global.h>
#include <mysql.h>



/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/

/**
 * \typedef xme_adv_mstManager_systemState_t
 *
 * \brief  mst logger system state structure.
 */
typedef enum
{
	XME_ADV_MSTMANAGER_STATUS_SUCCESS = XME_CORE_STATUS_SUCCESS,				
	XME_ADV_MSTMANAGER_STATUS_LOGOUT,	///< a device logged out of the system
	XME_ADV_MSTMANAGER_STATUS_LOGIN,	///< a device logged into the system
	XME_ADV_MSTMANAGER_STATUS_ERROR		///< error
}
xme_adv_mstManager_status_t;


typedef enum
{
	XME_ADV_MSTMANAGER_CHECK_SERIAL_MODBUSINTERFACE,				
	XME_ADV_MSTMANAGER_CHECK_TCP_MODBUSINTERFACE,
	XME_ADV_MSTMANAGER_CHECK_SIMULATOR_MODBUSINTERFACE
}
xme_adv_mstManager_checkForNewDeviceState_t;


/**
 * \typedef xme_adv_mstManager_systemError_t
 *
 * \brief  mst manager system error structure.
 */
typedef struct
{
	xme_adv_mstManager_status_t status;
	uint8_t data;
}
xme_adv_mstManager_systemError_t;



/**
 * \typedef xme_adv_mstManager_systemState_t
 *
 * \brief  mst logger system state structure.
 */
typedef struct
{
	// sensorTypeID, interfaceTypeRef, assignedID, numChannels, installPositionInstanceID, installPositionRef
	uint8_t sensorType;
	uint8_t interfaceType;
	uint8_t slaveID;
	uint8_t numChannels;
	uint8_t installPositionInstance;
	uint8_t installPosition;
}
xme_adv_mstManager_systemState_t;

/**
 * \typedef xme_adv_mstManager_configStruct_t
 *
 * \brief  mst logger configuration structure.
 */
typedef struct
{
	// public
	xme_core_topic_t listenTopic;
	const char* dbAddress;
	const char* dbDatabase;
	const char* dbUserName;
	const char* dbPassword;
	xme_hal_time_interval_t checkForNewDeviceintervalMs;
	// private
	xme_core_dcc_subscriptionHandle_t topicHandle;
	MYSQL* dbConnection;
	unsigned int mysqlError;
	xme_core_dcc_publicationHandle_t dccPublicationHandle;
	xme_adv_mstManager_systemState_t* systemState;
	uint16_t sizeofSystemState;
	xme_core_resourceManager_taskHandle_t checkForNewDeviceTaskHandle;
	xme_core_rr_requestHandle_t rrRequestHandle;
	void* responseData;
	uint8_t responseDataSize;
	uint8_t newSlaveId;
	uint8_t newInstallPositionInstance;
	uint8_t newInstallPosition;
	char serNum[7];
	uint16_t componentID;
	uint8_t checkForNewDeviceFlag;
	xme_adv_mstManager_checkForNewDeviceState_t checkForNewDeviceState;
}
xme_adv_mstManager_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates a mstManager component.
 */
xme_core_status_t
xme_adv_mstManager_create(xme_adv_mstManager_configStruct_t* config);

/**
 * \brief  Activates a mstManager component.
 */
xme_core_status_t
xme_adv_mstManager_activate(xme_adv_mstManager_configStruct_t* config);

/**
 * \brief  Deactivates a mstManager component.
 */
void
xme_adv_mstManager_deactivate(xme_adv_mstManager_configStruct_t* config);

/**
 * \brief  Destroys a mstManager component.
 */
void
xme_adv_mstManager_destroy(xme_adv_mstManager_configStruct_t* config);

#endif // #ifndef XME_ADV_mstManager_H

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

#ifndef XME_ADV_MSTFUSION_H
#define XME_ADV_MSTFUSION_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/component.h"
#include "xme/core/dcc.h"
#include "xme/core/resourceManager.h"
#include "xme/hal/linkedList.h"
#include "xme/hal/linkedList_arch.h"
#include "xme/adv/mstManager.h"
#include "xme/prim/modbusMaster.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
typedef enum
{
	TEMPERATURE = 1,	///< temperatur mst system
	PRESSURE = 2,		///< pressure mst system
}
xme_adv_mstFusion_mstType_t;

typedef struct
{
	uint8_t installPosition;	///< install position
	uint16_t value;				///< value
	uint16_t variance;			///< variance
}
xme_adv_mstFusion_responseData_t;

typedef struct
{
	uint8_t installPositionInstance;	///< install position instance of the mst system
	uint8_t installPosition;	///< install position of the mst System
	uint32_t sensorInstance;	///< instance of the sensor, related to the installPositionInstance of the mst System
	uint8_t dataAvailable;		///< flag, if the data of this sensors has arrived
	uint16_t value;				///< value of the sensor						
	uint16_t variance;			///< variance of the sensor
}
xme_adv_mstFusion_fusionSensorListItem_t;


typedef struct
{
	uint8_t installPosition;	///< install position of the sensor group
	uint16_t value;				///< fusionized value
	uint16_t variance;			///< variance of the fusionized value
	uint8_t dataAvailable;		///< flag, if there are allready data on this position
}
xme_adv_mstFusion_fusionDataListItem_t;


typedef struct
{
	// public
	xme_core_topic_t listenTopicSensorType;			///< listen topic
	uint16_t fusionTaskPeriode;						///< fusion periode of the fusion task
	uint16_t fusionTaskCount;						///< Counter for the fusionTask, timer functionallity
	// private
	uint16_t componentID;							///< component Id
	xme_core_dcc_subscriptionHandle_t listenTopicSensorTypeHandle;				///< listen topic handle for data from mstLogger, (TEMPERATURE or PRESSURE)
	xme_core_dcc_subscriptionHandle_t listenTopicSystemStateHandle;				///< listen topic handle system state
	xme_core_resourceManager_taskHandle_t taskHandle;							///< Task handle.
	xme_core_dcc_publicationHandle_t errorTopicPublicationHandle;				///< Publication handle.
	xme_core_dcc_publicationHandle_t fusionTopicPublicationHandle;				///< Publication handle.
	XME_HAL_SINGLYLINKEDLIST(xme_adv_mstFusion_fusionSensorListItem_t, mstfusionSensorList, 1);	///< list for fusion sensors and data
	XME_HAL_SINGLYLINKEDLIST(xme_adv_mstFusion_fusionDataListItem_t, mstfusionDataList, 1);	///< list for fusion sensors and data
	xme_hal_sync_criticalSectionHandle_t criticalSectionHandle;					///< critical section handle
	xme_adv_mstFusion_mstType_t mstType;										///< type of the mst system temperature=1 or pressure=2
	xme_core_resourceManager_taskHandle_t fusionTaskHandle;						///< fusion task handle
	uint16_t currentFusionTaskCount;											///< counts the periodes of the fusion task
	uint8_t flagDataComplete;													///< flag data complete	
	xme_core_topic_t publicationtopic;											///< publication topic
}
xme_adv_mstFusion_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates a mstFusion component.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            initialized.
 *          - XME_CORE_STATUS_INTERNAL_ERROR on error.
 */
xme_core_status_t
xme_adv_mstFusion_create(xme_adv_mstFusion_configStruct_t* config);

/**
 * \brief  Activates a mstFusion component.
 */
xme_core_status_t
xme_adv_mstFusion_activate(xme_adv_mstFusion_configStruct_t* config);

/**
 * \brief  Deactivates a mstFusion component.
 */
void
xme_adv_mstFusion_deactivate(xme_adv_mstFusion_configStruct_t* config);

/**
 * \brief  Destroys a mstFusion component.
 */
void
xme_adv_mstFusion_destroy(xme_adv_mstFusion_configStruct_t* config);

#endif // #ifndef XME_ADV_MSTFUSION_H

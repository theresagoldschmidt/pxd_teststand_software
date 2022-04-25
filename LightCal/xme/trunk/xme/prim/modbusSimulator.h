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

#ifndef XME_PRIM_MODBUS_SIMULATOR_H
#define XME_PRIM_MODBUS_SIMULATOR_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/component.h"
#include "xme/core/dcc.h"
#include "xme/core/resourceManager.h"

#include "xme/core/device.h"
#include "xme/core/nodeManager.h"
#include "xme/core/packet.h"
#include "xme/core/rr.h"

#include "xme/hal/net.h"
#include "xme/hal/linkedList.h"
#include "xme/hal/linkedList_arch.h"
#include "xme/hal/sched.h"
#include "xme/hal/random.h"
#include "xme/hal/sync.h"
#include "xme/hal/table.h"

#include "xme/prim/modbusMaster.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/

/**
 * \typedef xme_prim_modbusSimulator_priorityQueueItem_t
 *
 * \brief  Modbus Simulator priority queue item type.
 */
typedef struct
{
	uint16_t priority;
	xme_core_rr_responseInstanceHandle_t responseInstanceHandle;
	xme_prim_modbusMaster_requestType_t requestType;
	xme_prim_modbusMaster_registerType_t registerType;
	uint16_t slaveID;
	uint16_t componentIdOfSender;
	uint16_t count;
	uint16_t requestData;
	uint16_t mstType;
}
xme_prim_modbusSimulator_priorityQueueItem_t;


/**
 * \typedef xme_prim_modbusSimulator_defaultSensorListItem_t
 *
 * \brief  Modbus Simulator default sensor list item type.
 */
typedef struct
{
	uint16_t slaveId;
}
xme_prim_modbusSimulator_defaultSensorListItem_t;


/**
 * \typedef xme_prim_modbusSimulator_dynamicSensorListItem_t
 *
 * \brief  Modbus Simulator dynamic sensor list item type.
 */
typedef struct
{
	uint16_t slaveId;
	char dynamicSensorSerialNumber[6];
	unsigned long dynamicSensorLoginTime;
	unsigned long dynamicSensorLogoutTime;
	double dynamicSensorMeanValue;
	double dynamicSensorVariance;
	uint8_t activeFlag;
	uint8_t dead;
}
xme_prim_modbusSimulator_dynamicSensorListItem_t;


/**
 * \typedef xme_prim_modbusSimulator_staticSensorListItem_t
 *
 * \brief  Modbus Simulator static sensor list item type.
 */
typedef struct
{
	uint16_t slaveId;
	double staticSensorMeanValue;
	double staticSensorVariance;
}
xme_prim_modbusSimulator_staticSensorListItem_t;


/**
 * \typedef xme_prim_modbusSimulator_configStruct_t
 *
 * \brief  Modbus Simulator configuration structure.
 */
typedef struct
{
	// public
	xme_prim_modbusMaster_interfaceType_t modbusType;
	double defaultSensorMeanValueInputRegister;
	double defaultSensorVarianceInputRegister;
	double defaultSensorMeanValueHoldingRegister;
	double defaultSensorVarianceHoldingRegister;
	uint8_t numberOfDynamicSensors;
	char dynamicSensorSerialNumber[5][8];
	unsigned long dynamicSensorLoginTime[5];
	unsigned long dynamicSensorLogoutTime[5];
	double dynamicSensorMeanValue[5];
	double dynamicSensorVariance[5];
	uint8_t numberOfStaticSensors;
	uint16_t slaveIdsOfStaticSensors[5];
	double staticSensorMeanValue[5];
	double staticSensorVariance[5];
	// private
	xme_hal_sync_criticalSectionHandle_t criticalSectionHandle;
	xme_core_rr_requestHandlerHandle_t requestHandlerHandle;
	xme_hal_sched_taskHandle_t taskHandle;
	XME_HAL_SINGLYLINKEDLIST(xme_prim_modbusSimulator_priorityQueueItem_t, priorityQueue, 3);
	XME_HAL_SINGLYLINKEDLIST(xme_prim_modbusSimulator_defaultSensorListItem_t, defaultSensorList, 3);
	XME_HAL_SINGLYLINKEDLIST(xme_prim_modbusSimulator_dynamicSensorListItem_t, dynamicSensorList, 3);
		XME_HAL_SINGLYLINKEDLIST(xme_prim_modbusSimulator_staticSensorListItem_t, staticSensorList, 3);
	uint8_t defaultListFlag;
	unsigned long localTimeCount;
	uint16_t componentID;
	uint8_t loginMode;
	uint8_t dataRequestedFlag;
}
xme_prim_modbusSimulator_configStruct_t;


/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates a Modbus Simulator component.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            initialized.
 *          - XME_CORE_STATUS_INTERNAL_ERROR on error.
 */
xme_core_status_t
xme_prim_modbusSimulator_create(xme_prim_modbusSimulator_configStruct_t* config);

/**
 * \brief  Activates a Modbus Simulator component.
 */
xme_core_status_t
xme_prim_modbusSimulator_activate(xme_prim_modbusSimulator_configStruct_t* config);

/**
 * \brief  Deactivates a Modbus Simulator component.
 */
void
xme_prim_modbusSimulator_deactivate(xme_prim_modbusSimulator_configStruct_t* config);

/**
 * \brief  Destroys a Modbus Simulator component.
 */
void
xme_prim_modbusSimulator_destroy(xme_prim_modbusSimulator_configStruct_t* config);

#endif // #ifndef XME_PRIM_MODBUS_SIMULATOR_H

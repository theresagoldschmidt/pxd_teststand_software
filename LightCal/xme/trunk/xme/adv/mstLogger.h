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

#ifndef XME_ADV_MSTLOGGER_H
#define XME_ADV_MSTLOGGER_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/adv/mstManager.h"
#include "xme/prim/modbusMaster.h"
#include "xme/core/dcc.h"
#include "xme/core/resourceManager.h"
#include "xme/core/rr.h"
#include "xme/hal/linkedList.h"
#include "xme/hal/linkedList_arch.h"
#include "xme/hal/net.h"
#include "xme/hal/time.h"
#include "xme/hal/sched.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_adv_mstLogger_mstType_t
 *
 * \brief  mst system type.
 */
typedef enum
{
	XME_ADV_MSTLOGGER_TEMPERATURE = 1,	///< temperatur mst system
	XME_ADV_MSTLOGGER_PRESSURE = 2,		///< pressure mst system
}
xme_adv_mstLogger_mstType_t;

/**
 * \typedef xme_adv_mstLogger_mstSystemRequestListItem_t
 *
 * \brief  mst system request item.
 */
typedef struct
{
	uint8_t priority;										///< priority of modbus request
	xme_prim_modbusMaster_interfaceType_t interfaceType;	///< modbus interface typ (serial/ tcp)
	xme_prim_modbusMaster_requestType_t requestType;		///< modbus request typ (read/ write)
	xme_prim_modbusMaster_registerType_t registerType;		///< modbus register typ (holding, coil, ...)
	uint8_t slaveID;			///< modbus address
	uint16_t startRegister;		///< address of start register of the modbus slave
	uint8_t count;				///< count of registers
	uint16_t data;				///< data for writing
	uint8_t installPositionInstance;	///< install position instance of the mstSystem
	uint8_t installPosition;				///< install position of the mstSystem
	uint16_t componentId;		///< component id
	uint16_t mstType;			///< mstType of mst System/ sensor
}
xme_adv_mstLogger_mstSystemRequestListItem_t;


typedef enum
{
	XME_ADV_MSTLOGGER_READING_VALUE = XME_PRIM_MODBUSMASTER_REGISTER_TYPE_INPUT_REGISTERS,	///< reading register
	XME_ADV_MSTLOGGER_ADC_VALUE = XME_PRIM_MODBUSMASTER_REGISTER_TYPE_HOLDING_REGISTERS,	///< adc register
}
xme_adv_mstLogger_mstDataType_t;

typedef struct
{
	uint16_t installPositionInstance;
	uint16_t sensorNumber;
	uint32_t data;
}
xme_adv_mstLogger_topicData_t;


/**
 * \typedef xme_adv_mstLogger_configStruct
 *
 * \brief  mst logger configuration structure.
 */
typedef struct
{
	// public
	xme_hal_time_interval_t intervalMs;
	xme_adv_mstLogger_mstType_t mstType;
	xme_adv_mstLogger_mstDataType_t mstDataType;
	// private
	xme_core_topic_t topic;
	xme_core_interface_interfaceId_t interfaceId;
	xme_core_dcc_publicationHandle_t dccPublicationHandle;
	xme_core_dcc_publicationHandle_t errorDccPublicationHandle;
	xme_core_rr_requestHandle_t rrRequestHandle;
	xme_core_resourceManager_taskHandle_t dccTaskHandle;
	xme_core_resourceManager_taskHandle_t rrTaskHandle;
	xme_hal_net_socketHandle_t rrServerSocket;
	xme_core_dcc_subscriptionHandle_t topicHandle;
	uint16_t componentID;
	XME_HAL_SINGLYLINKEDLIST(xme_adv_mstLogger_mstSystemRequestListItem_t, mstSystemRequestList, 1);
	xme_hal_sync_criticalSectionHandle_t criticalSectionHandle;
	xme_core_dcc_publicationHandle_t dccPublicationHandle_XME_CORE_TOPIC_SYSTEM_ERROR;
	xme_adv_mstManager_systemError_t* systemError;
}
xme_adv_mstLogger_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
xme_core_status_t
xme_adv_mstLogger_create(xme_adv_mstLogger_configStruct_t* config);

xme_core_status_t
xme_adv_mstLogger_activate(xme_adv_mstLogger_configStruct_t* config);

void
xme_adv_mstLogger_deactivate(xme_adv_mstLogger_configStruct_t* config);

void
xme_adv_mstLogger_destroy(xme_adv_mstLogger_configStruct_t* config);

#endif // #ifndef XME_ADV_MSTLOGGER_H

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

// TODO: Rename to "Modbus Master over Serial" or similar

#ifndef XME_PRIM_MODBUSMASTER_H
#define XME_PRIM_MODBUSMASTER_H

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define NO_BOOST

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/device.h"
#include "xme/core/nodeManager.h"
#include "xme/core/packet.h"
#include "xme/core/rr.h"

#include "xme/hal/net.h"
#include "xme/hal/linkedList.h"
#include "xme/hal/linkedList_arch.h"
#include "xme/hal/sched.h"
#include "xme/hal/sync.h"
#include "xme/hal/table.h"
#include <math.h>

#include "modbus.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_prime_modbusMaster_status_t
 *
 * \brief  Modbus Master status.
 */
typedef enum
{
	XME_PRIM_MODBUSMASTER_STATUS_SUCCESS = XME_CORE_RR_STATUS_SUCCESS,				
	XME_PRIM_MODBUSMASTER_STATUS_UNEXPECTED_REQUESTTYPE = XME_CORE_RR_STATUS_USER,	///< Error: This request type is not supported
	XME_PRIM_MODBUSMASTER_STATUS_UNEXPECTED_REGISTERTYPE,	///< Error: This register type is not supported
	XME_PRIM_MODBUSMASTER_STATUS_NOT_RESPONSIBLE,			///< Error: This component is not responsible for this request
	XME_PRIM_MODBUSMASTER_STATUS_MODBUS_ERROR,				///< Error: Modbus while modbus communication
	XME_PRIM_MODBUSMASTER_STATUS_SERVER_ERROR,				///< Error: Error in the modbus response handler component
	XME_PRIM_MODBUSMASTER_STATUS_UNEXPECTED_DATASIZE,		///< Error: The requested datasize equals not the received datasize
	XME_PRIM_MODBUSMASTER_STATUS_CONNECTION_FAILD,			///< Error: Modbus connection or slave is not available
	XME_PRIM_MODBUSMASTER_STATUS_WRITING_ERROR				///< Error: Modbus connection or slave is not available
}
xme_prim_modbusMaster_status_t;


/**
 * \typedef xme_prim_modbusMaster_requestType_t
 *
 * \brief  Modbus Master request type.
 */
typedef enum
{
	XME_PRIM_MODBUSMASTER_WRITE = 0,          ///< writing request
	XME_PRIM_MODBUSMASTER_READ = 1,           ///< reading request
}
xme_prim_modbusMaster_requestType_t;


/**
 * \typedef xme_prim_modbusMaster_interfaceType_t
 *
 * \brief  Modbus Master interface type.
 */
typedef enum
{
	XME_PRIM_MODBUSMASTER_SERIAL = 1,	///< modbus serial
	XME_PRIM_MODBUSMASTER_TCP = 2,		///< modbus tcp
	XME_PRIM_MODBUSMASTER_SIMULATOR = 3,		///< modbus simulator
}
xme_prim_modbusMaster_interfaceType_t;


/**
 * \typedef xme_prim_modbusMaster_registerType_t
 *
 * \brief  Modbus Master register type.
 */
typedef enum
{
	XME_PRIM_MODBUSMASTER_REGISTER_TYPE_BITS = 1,       ///< register bits
	XME_PRIM_MODBUSMASTER_REGISTER_TYPE_HOLDING_REGISTERS,      ///< 16-bit holding registers
	XME_PRIM_MODBUSMASTER_REGISTER_TYPE_INPUT_BITS,     ///< input register bits
	XME_PRIM_MODBUSMASTER_REGISTER_TYPE_INPUT_REGISTERS ///< 16-bit input registers
}
xme_prim_modbusMaster_registerType_t;


/**
 * \typedef xme_prim_modbusMaster_priorityQueueItem_t
 *
 * \brief  Modbus Master priority queue item type.
 */
typedef struct
{
	uint16_t priority;
	xme_core_rr_responseInstanceHandle_t responseInstanceHandle;
	xme_prim_modbusMaster_requestType_t requestType;
	xme_prim_modbusMaster_registerType_t registerType;
	uint16_t slaveID;
	uint16_t startRegister;
	uint16_t count;
	uint16_t requestData;
	uint16_t componentIdOfSender;
	uint16_t mstType;
}
xme_prim_modbusMaster_priorityQueueItem_t;

typedef struct
{
	uint16_t componentId;
	uint16_t slaveId;
	uint16_t requestData;
	uint16_t registerType;
	uint16_t mstType;
}
xme_prim_modbusMaster_requestMetaData_t;



/**
 * \typedef xme_prim_modbusMaster_requestData_t
 *
 * \brief  Data that are being transmitted along with a mst sensor request.
 */
#pragma pack(push, 1)
typedef struct
{
	uint8_t priority;									///< priority of the modbus request
	xme_prim_modbusMaster_interfaceType_t interfaceType;	///< interface type of modbus (serial, tcp, simulator)
	xme_prim_modbusMaster_requestType_t requestType;	///< request type
	xme_prim_modbusMaster_registerType_t registerType;	///< register type
	uint8_t slaveID;									///< slave address
	uint16_t startRegister;								///< start register
	uint8_t count;										///< qualtity of register
	uint16_t data;										///< data for writing
	uint16_t componentId;								///< id of the requesting component
	uint16_t mstType;									///< mstType of mst System/ sensor
}
xme_prim_modbusMaster_requestData_t;
#pragma pack(pop)


/**
 * \typedef xme_prim_modbusMaster_configStruct_t
 *
 * \brief  Modbus Master configuration structure.
 */
typedef struct
{
	// public
	xme_prim_modbusMaster_interfaceType_t modbusType;
	char comPort [5];
	unsigned int baudrate;
	char parity;
	uint16_t dataBits;
	uint16_t stopBits;
	// private
	uint16_t componentID;
	xme_hal_sync_criticalSectionHandle_t criticalSectionHandle;
	xme_core_rr_requestHandlerHandle_t requestHandlerHandle;
	xme_hal_sched_taskHandle_t taskHandle;
	modbus_t *mb;
	XME_HAL_SINGLYLINKEDLIST(xme_prim_modbusMaster_priorityQueueItem_t, priorityQueue, 3);
}
xme_prim_modbusMaster_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates a Modbus Master component.
 */
xme_core_status_t
xme_prim_modbusMaster_create(xme_prim_modbusMaster_configStruct_t* config);

/**
 * \brief  Activates a Modbus Master component.
 */
xme_core_status_t
xme_prim_modbusMaster_activate(xme_prim_modbusMaster_configStruct_t* config);

/**
 * \brief  Deactivates a Modbus Master component.
 */
void
xme_prim_modbusMaster_deactivate(xme_prim_modbusMaster_configStruct_t* config);

/**
 * \brief  Destroys a Modbus Master component.
 */
void
xme_prim_modbusMaster_destroy(xme_prim_modbusMaster_configStruct_t* config);

#endif // #ifndef XME_PRIM_MODBUSMASTER_H

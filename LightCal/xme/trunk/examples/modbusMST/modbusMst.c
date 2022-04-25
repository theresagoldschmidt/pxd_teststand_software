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
 *         Database example project.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *		   Marcel Flesch <fleschmarcel@mytum.de>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/adv/databaseLogger.h"
#include "xme/adv/loginServer.h"
#include "xme/adv/mstFusion.h"
#include "xme/adv/mstManager.h"
#include "xme/adv/mstLogger.h"

#include "xme/prim/ipLoginClientProxy.h"
#include "xme/prim/modbusSimulator.h"
#include "xme/prim/modbusMaster.h"

#include "xme/core/componentList.h"
#include "xme/core/core.h"
#include "xme/core/defines.h"

#include "xme/hal/sched.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define XME_CORE_NODEMANAGER_DEVICE_TYPE ((xme_core_device_type_t)0x000000000415D045)
#define XME_CORE_NODEMANAGER_DEVICE_GUID ((xme_core_device_guid_t)0x0000000000011841)

/******************************************************************************/
/***   Component configurations                                             ***/
/******************************************************************************/
XME_COMPONENT_CONFIG_INSTANCE(xme_core_nodeManager) =
{
	// public
	XME_CORE_NODEMANAGER_DEVICE_TYPE, // deviceType
	XME_CORE_NODEMANAGER_DEVICE_GUID // deviceGuid
};

XME_COMPONENT_CONFIG_INSTANCE(xme_adv_loginServer) =
{
	// public
	XME_CORE_NODE_LOCAL_NODE_ID+1 // nextNodeId
};

XME_COMPONENT_CONFIG_INSTANCE(xme_prim_ipLoginClientProxy) =
{
	// public
	(xme_core_interface_interfaceId_t)0 // interfaceId
};

XME_COMPONENT_CONFIG_INSTANCE(xme_prim_modbusMaster) =
{
	// public
	XME_PRIM_MODBUSMASTER_SERIAL,	// modbus type
	"COM3",		// com-port
	38400,		// baudrate
	'N',		// parity
	8,			// dataBits;
	1			// stopBits;	
};

XME_COMPONENT_CONFIG_INSTANCE(xme_prim_modbusSimulator) =
{
	// public
	XME_PRIM_MODBUSMASTER_SIMULATOR,	// modbus type
	3000,								// default sensor mean value
	5,									// default sensor variance
	90843,								// default sensor mean value adc
	8.5569,								// default sensor variance adc
	2,									// number of dynamic sensors
	{ {"110000\n"}, {"110001\n"}, {"000000\n"}, {"000000\n"}, {"000000\n"} }, // serial numbers of dynamic sensors
	{4000, 7000, 0, 0, 0},				// dynamic sensor login time in ms
	{20000, 23000, 0, 0, 0},			// dynamic sensor logout time in ms
	{92000, 8900, 0, 0, 0},				// mean value of dynamic sensors in 100°C or adc values
	{500, 100, 0, 0, 0},				// variance of dynamic sensors in 100°C or adc values
	2,									// number of static sensors
	{1, 2, 0, 0, 0},					// slave Ids of static sensors
	{90843, 8389, 0, 0, 0},				// mean value of static sensors in 100°C or adc values
	{330, 70, 0, 0, 0}					// variance of static sensors in 100°C or adc values
};

XME_COMPONENT_CONFIG_INSTANCE(xme_adv_mstManager) =
{
	// public
	XME_CORE_TOPIC_UNSIGNED_INTEGER,		 // listenTopic
	"192.168.17.164",	// dbAddress
	"random",			// dbDatabase
	"random",			// dbUserName
	"random",			// dbPassword
	2000				// interval in ms for the task checkForNewDevice
};


XME_COMPONENT_CONFIG_INSTANCE(xme_adv_mstFusion) =
{
	// public
	XME_CORE_TOPIC_TEMPERATURE,			// listenTopic
	3000,								// fusion periode
	10									// count of fusion periode to fusionize data
};

XME_COMPONENT_CONFIG_INSTANCE(xme_adv_databaseLogger) =
{
	{
	// public
	XME_CORE_TOPIC_TEMPERATURE,			// listenTopic
	"192.168.17.164",					// dbAddress
	"random",							// dbDatabase
	"random",							// dbUserName
	"random",							// dbPassword
	"measureTemperature",				// tableName
	"time\0installPositionInstanceRef\0sensorNumber\0temperature\0",	// columnNames
	"timestamp\0int(11)\0int(11)\0int(11)\0"					// columnTypes
	},
	{
	// public
	XME_CORE_TOPIC_PRESSURE,			// listenTopic
	"192.168.17.164",					// dbAddress
	"random",							// dbDatabase
	"random",							// dbUserName
	"random",							// dbPassword
	"measurePressure",					// tableName
	"time\0installPositionInstanceRef\0sensorNumber\0pressure\0",	// columnNames
	"timestamp\0int(11)\0int(11)\0int(11)\0"					// columnTypes	
	},
	{
	// public
	XME_CORE_TOPIC_TEMPFUSION,			// listenTopic
	"192.168.17.164",					// dbAddress
	"random",							// dbDatabase
	"random",							// dbUserName
	"random",							// dbPassword
	"InstallPositionDataTemp",			// tableName
	"time\0installPositionRef\0tempEstimate\0tempVariance\0",	// columnNames
	"timestamp\0int(11)\0int(11)\0int(11)\0"					// columnTypes	
	}
};


XME_COMPONENT_CONFIG_INSTANCE(xme_adv_mstLogger) =
{
	{
		// public
		3000,							// intervalMs
		XME_ADV_MSTLOGGER_TEMPERATURE,	// unit of mstSystem
		XME_ADV_MSTLOGGER_ADC_VALUE		// ADC Value or reading register
	},
	{
		// public
		3000,							// intervalMs
		XME_ADV_MSTLOGGER_TEMPERATURE,	// unit of mstSystem
		XME_ADV_MSTLOGGER_READING_VALUE	// ADC Value or reading register
	},
	{
		// public
		3000,							// intervalMs
		XME_ADV_MSTLOGGER_PRESSURE,		// unit of mstSystem
		XME_ADV_MSTLOGGER_ADC_VALUE		// ADC Value or reading register
	},
	{
		// public
		3000,							// intervalMs
		XME_ADV_MSTLOGGER_PRESSURE,		// unit of mstSystem
		XME_ADV_MSTLOGGER_READING_VALUE	// ADC Value or reading register
	}
};



/******************************************************************************/
/***   Component descriptor                                                 ***/
/******************************************************************************/
XME_COMPONENT_LIST_BEGIN
	// system components of chromosome
	XME_COMPONENT_LIST_ITEM(xme_core_nodeManager, 0)
	XME_COMPONENT_LIST_ITEM(xme_adv_loginServer, 0)
	XME_COMPONENT_LIST_ITEM(xme_prim_ipLoginClientProxy, 0)
	// mst components
	XME_COMPONENT_LIST_ITEM(xme_adv_databaseLogger, 0)	// temp
	XME_COMPONENT_LIST_ITEM(xme_adv_databaseLogger, 1)	// pres
	XME_COMPONENT_LIST_ITEM(xme_adv_databaseLogger, 2) // fusion
	XME_COMPONENT_LIST_ITEM(xme_prim_modbusMaster, 0) 
	XME_COMPONENT_LIST_ITEM(xme_prim_modbusSimulator, 0)
	XME_COMPONENT_LIST_ITEM(xme_adv_mstLogger, 0) // temp adc
	XME_COMPONENT_LIST_ITEM(xme_adv_mstLogger, 1) // temp dig
	XME_COMPONENT_LIST_ITEM(xme_adv_mstLogger, 2) // pres adc
	XME_COMPONENT_LIST_ITEM(xme_adv_mstLogger, 3) // pres dig
	XME_COMPONENT_LIST_ITEM(xme_adv_mstFusion, 0) 
	XME_COMPONENT_LIST_ITEM(xme_adv_mstManager, 0)
XME_COMPONENT_LIST_END;

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
int
main (int argc, char* argv[])
{
	XME_CHECK_MSG
	(
		XME_CORE_STATUS_SUCCESS == xme_core_init(),
		1,
		XME_LOG_FATAL,
		"Fatal error during XME initialization!\n"
	);

	xme_core_run();

	return 0;
}

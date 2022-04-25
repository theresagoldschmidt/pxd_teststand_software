/*
 * Copyright (c) 2011, fortiss GmbH.
 * All rights reserved.
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
 *        Belle II GUI XME component file.
 *
 * \author
 *         Hauke Stähle <staehle@fortiss.org>
 *         Nadine Keddis <keddis@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/componentList.h"
#include "xme/core/core.h"
#include "xme/core/defines.h"
#include "xme/core/directory.h"
#include "xme/core/nodeManager.h"
#include "xme/core/interfaceManager.h"
#include "xme/core/broker.h"
#include "xme/hal/time.h"
#include "belle_II_GUI_XME_Component.h"
#include "logdataDefines.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "wrapperHeader.h"
#ifdef __cplusplus
}
#endif

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
// Structure that keeps the status info of all nodes
XME_HAL_TABLE
(
	status_info_nodes_t,
	status_info_nodes_table,
	NUMBER_OF_BELLE_NODES
);
/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
xme_core_dcc_subscriptionHandle_t subHandle;
xme_core_dcc_publicationHandle_t pubHandleCommand;
xme_core_dcc_publicationHandle_t pubHandleCalibrate;
xme_core_dcc_publicationHandle_t pubHandleRecalibrate;
xme_core_dcc_publicationHandle_t pubHandleCommand;
xme_core_dcc_publicationHandle_t pubHandleSWRegisterSetDone;
xme_core_dcc_subscriptionHandle_t subHandleMonitor;
xme_core_dcc_subscriptionHandle_t subHandleSWRegisterSet;

#define COMMAND_TOPIC_ID (xme_core_topic_t)300
#define COMMAND_CHANNEL (xme_core_dataChannel_t)123

#define SET_CAHNNEL_VALUE_ID (xme_core_topic_t)302
#define CALIBRATE_CHANNEL (xme_core_dataChannel_t)125

#define SET_CHANNEL_CALIBRATION_DATA_ID (xme_core_topic_t)303
#define RECALIBRATE_CHANNEL (xme_core_dataChannel_t)126

#define LOGDATA_TOPIC_ID (xme_core_topic_t)305
#define LOGDATA_CHANNEL (xme_core_dataChannel_t)128

#define MONITOR_MESSAGE_TOPIC_ID (xme_core_topic_t)778
#define MONITOR_MESSAGE_CHANNEL (xme_core_dataChannel_t)129

#define SW_REG_SET_TOPIC_ID (xme_core_topic_t)306
#define SW_REG_SET_CHANNEL (xme_core_dataChannel_t)130

#define SW_REG_SET_DONE_TOPIC_ID (xme_core_topic_t)307
#define SW_REG_SET_DONE_CHANNEL (xme_core_dataChannel_t)131

#define LOCAL_PORT_COMMAND 33211
#define LOCAL_PORT_CALIBRATE 33213
#define LOCAL_PORT_RECALIBRATE 33214

// Publisher name (PS Node Name == Ip address) used for tcp interface for the SW_REG_SET topic.
#define PUBLISHER_NAME_PS "192.168.17.145"

// TCP Port used for the SW_REG_SET
#define PUBLISHER_PORT_PS 33218

// TCP Port used for the SW_REG_SET_DONE topic
#define LOCAL_PORT_PS 33215

#define START 0
#define STOP 1 
#define CALIBRATE 2
#define BOOTLOAD 3
#define RECALIBRATE 4
#define COMMAND 5
#define EMERGENCY_SHUTDOWN 6

#define 	PS_STATE_READY  0
#define 	PS_STATE_START_UP 1
#define 	PS_STATE_SHUT_DOWN 2
#define 	PS_STATE_MONITOR 3
#define 	PS_STATE_CALIBRATE  4
#define 	PS_STATE_RECALIBRATE 5
#define 	PS_STATE_BOOTLOAD  6
#define 	PS_STATE_EMERGENCY_SHUTDOWN  7
#define 	PS_STATE_NORMAL_OPERATION 8
#define 	PS_STATE_INITILIZE  9

// maxTimestampAge timeout when the status of allnodes should be checked.
xme_hal_time_interval_t maxTimestampAge = 900;
xme_hal_sched_taskHandle_t statusNodesTaskHandle;
/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
void
logdata_subscription
(
		xme_hal_sharedPtr_t dataHandle,
		void* userData
)
{
	int i;
	log_error_information_t *log_data=(log_error_information_t*) xme_hal_sharedPtr_getPointer(dataHandle);
	switch(log_data->code_number){
		case 1:
			// Write the received log data in the log file
			updateLogFile(log_data->log_info.nodeNumber, "Node number: ",log_data->log_info.nodeNumber);
			for(i = 0; i < NUMBER_OF_CALIBRATION_CHANNELS; ++i){
				updateLogFile(log_data->log_info.nodeNumber, "Channel number: ", i);
				updateLogFile(log_data->log_info.nodeNumber, "Voltage at Regulator: ", log_data->log_info.log_node_info_all_channels[i].voltage_at_regulator);
				updateLogFile(log_data->log_info.nodeNumber, "Voltage at Load: ", log_data->log_info.log_node_info_all_channels[i].voltage_at_load);
				updateLogFile(log_data->log_info.nodeNumber, "Current: ", log_data->log_info.log_node_info_all_channels[i].current);
				updateLogFile(log_data->log_info.nodeNumber, "Regulator Status: ", log_data->log_info.log_node_info_all_channels[i].regulator_status_bit);
			}
			break;
		case 2:
			// Display the log data on the GUI and write the them in the log file
			displayLogdata(log_data->emergency_info.nodeNumber, "Node number: ",log_data->emergency_info.nodeNumber);
			for(i = 0; i < NUMBER_OF_CALIBRATION_CHANNELS; ++i){
				if (log_data->emergency_info.channel_current_limit[i] == 0)
				displayLogdata(log_data->emergency_info.nodeNumber, "Channel in current limit: ", i);
			}
			if (log_data->emergency_info.ups_status == 0)
				displayLogdata(log_data->emergency_info.nodeNumber, "UPS status is: ", log_data->emergency_info.ups_status);
			if (log_data->emergency_info.thermal_status == 0)
				displayLogdata(log_data->emergency_info.nodeNumber, "Thermal status is: ", log_data->emergency_info.ups_status);
			if (log_data->emergency_info.ovp_fast_status == 0)
				displayLogdata(log_data->emergency_info.nodeNumber, "OVP status is: ", log_data->emergency_info.ups_status);
			break;
		case 3:
			// Write the received log data in the log file
			updateLogFile(log_data->log_info.nodeNumber, "Node number: ",log_data->log_info.nodeNumber);
			updateLogFile(log_data->log_info.nodeNumber, "Mode of operation: ", log_data->mode_of_node);
			break;

	}
}

void
monitorMessage_subscription
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
)
{
	monitor_information_node_t *data = (monitor_information_node_t*) xme_hal_sharedPtr_getPointer(dataHandle);
	XME_HAL_TABLE_ITERATE
	(
		status_info_nodes_table,
		xme_hal_table_rowHandle_t, statusInfoNodeHandle,
		status_info_nodes_t, statusInfoNodeItem,
		{
			if (data->nodeNumber == statusInfoNodeItem->nodeNumber)
			{
				statusInfoNodeItem->lastNodeStatusTimeStamp = xme_hal_time_getCurrentTime();
				statusInfoNodeItem->nodeStatusCode =  data->node_status;
			}

		}
	);
	updateStatus(data->nodeNumber, data->node_status);
	updateMonitor(data->nodeNumber, *data);
}

void
SWRegisterDone_subscription
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
)
{
	char message_DAQ[] = "SW Register SET";
	char message_compare[] = "Configure SW Register ";
	char *message_PS;
	message_PS = (char*) xme_hal_sharedPtr_getPointer( dataHandle );

	if (strcmp(message_compare,message_PS)== 0)
	{
	  xme_core_dcc_sendTopicData(pubHandleSWRegisterSetDone, &message_DAQ, sizeof(message_DAQ));
	}

}

xme_core_status_t
belle_II_GUI_XME_Component_create(xme_core_nodeManager_configStruct_t* config)
{
	// Initialized the table that keeps information about the status of all nodes
	XME_HAL_TABLE_INIT(status_info_nodes_table);
	initStatusInfoNodesTable();

	// Create publisher for start topic
	pubHandleCommand = xme_core_dcc_publishTopic( 
		COMMAND_TOPIC_ID, 
		XME_CORE_MD_EMPTY_META_DATA,
		false,
		NULL
	);

	if ( pubHandleCommand == XME_CORE_DCC_INVALID_PUBLICATION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Create publisher for calibrate topic
	pubHandleCalibrate = xme_core_dcc_publishTopic( 
		SET_CAHNNEL_VALUE_ID, 
		XME_CORE_MD_EMPTY_META_DATA, 
		false,
		NULL
	);

	if ( pubHandleCalibrate == XME_CORE_DCC_INVALID_PUBLICATION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Create publisher for recalibrate topic
	pubHandleRecalibrate = xme_core_dcc_publishTopic( 
		SET_CHANNEL_CALIBRATION_DATA_ID, 
		XME_CORE_MD_EMPTY_META_DATA, 
		false,
		NULL
	);

	if ( pubHandleRecalibrate == XME_CORE_DCC_INVALID_PUBLICATION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	//// Create publisher for SW_REG_SET_DONE topic
	//pubHandleSWRegisterSetDone = xme_core_dcc_publishTopic( 
	//	SW_REG_SET_DONE_TOPIC_ID, 
	//	XME_CORE_MD_EMPTY_META_DATA,
	//	false,
	//	NULL
	//);

	//if ( pubHandleSWRegisterSetDone == XME_CORE_DCC_INVALID_PUBLICATION_HANDLE )
	//{
	//	return XME_CORE_STATUS_INTERNAL_ERROR;
	//}

	

	// Create subscription to logdata topic
	subHandle = xme_core_dcc_subscribeTopic( 
		LOGDATA_TOPIC_ID, 
		XME_CORE_MD_EMPTY_META_DATA, 
		false,
		logdata_subscription, 
		NULL 
	);

	if ( subHandle == XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Create subscription to monitor message topic
	subHandleMonitor = xme_core_dcc_subscribeTopic( 
		MONITOR_MESSAGE_TOPIC_ID, 
		XME_CORE_MD_EMPTY_META_DATA, 
		false,
		monitorMessage_subscription, 
		NULL 
	);

	if ( subHandleMonitor == XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	//// Create subscription to SW_REG_SETtopic
	//subHandleSWRegisterSet = xme_core_dcc_subscribeTopic( 
	//	SW_REG_SET_TOPIC_ID, 
	//	XME_CORE_MD_EMPTY_META_DATA, 
	//	false,
	//	SWRegisterDone_subscription, 
	//	NULL 
	//);

	//if ( subHandleSWRegisterSet == XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE )
	//{
	//	return XME_CORE_STATUS_INTERNAL_ERROR;
	//}


	// Local channel from chat component for topic COMMAND
	if ( xme_core_routingTable_addLocalSourceRoute( 
		COMMAND_CHANNEL, 
		xme_core_dcc_getComponentFromPublicationHandle(pubHandleCommand),
		xme_core_dcc_getPortFromPublicationHandle(pubHandleCommand) 
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Local channel from gui component for topic CALIBRATE
	if ( xme_core_routingTable_addLocalSourceRoute( 
		CALIBRATE_CHANNEL, 
		xme_core_dcc_getComponentFromPublicationHandle(pubHandleCalibrate),
		xme_core_dcc_getPortFromPublicationHandle(pubHandleCalibrate) 
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Local channel from gui component for topic RECALIBRATE
	if ( xme_core_routingTable_addLocalSourceRoute( 
		RECALIBRATE_CHANNEL, 
		xme_core_dcc_getComponentFromPublicationHandle(pubHandleRecalibrate),
		xme_core_dcc_getPortFromPublicationHandle(pubHandleRecalibrate) 
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	//// Local channel from chat component for topic SW_REG_SET_DONE
	//if ( xme_core_routingTable_addLocalSourceRoute( 
	//	SW_REG_SET_DONE_CHANNEL, 
	//	xme_core_dcc_getComponentFromPublicationHandle(pubHandleSWRegisterSetDone),
	//	xme_core_dcc_getPortFromPublicationHandle(pubHandleSWRegisterSetDone) 
	//) != XME_CORE_STATUS_SUCCESS ) 	
	//{
	//	return XME_CORE_STATUS_INTERNAL_ERROR;
	//}

	// Local channel to chat component for topic LOGDATA
	if ( xme_core_routingTable_addLocalDestinationRoute(
		LOGDATA_CHANNEL,
		xme_core_dcc_getComponentFromSubscriptionHandle(subHandle),
		xme_core_dcc_getPortFromSubscriptionHandle(subHandle)
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Local channel to chat component for topic MONITOR
	if ( xme_core_routingTable_addLocalDestinationRoute(
		MONITOR_MESSAGE_CHANNEL,
		xme_core_dcc_getComponentFromSubscriptionHandle(subHandleMonitor),
		xme_core_dcc_getPortFromSubscriptionHandle(subHandleMonitor)
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	//// Local channel to chat component for topic SW_REG_SET
	//if ( xme_core_routingTable_addLocalDestinationRoute(
	//	SW_REG_SET_CHANNEL,
	//	xme_core_dcc_getComponentFromSubscriptionHandle(subHandleSWRegisterSet),
	//	xme_core_dcc_getPortFromSubscriptionHandle(subHandleSWRegisterSet)
	//) != XME_CORE_STATUS_SUCCESS ) 	
	//{
	//	return XME_CORE_STATUS_INTERNAL_ERROR;
	//};

	// Add channel to address mapping for tcp interface
	{
		xme_core_interface_address_t address;
		PORT_TO_GENERIC_ADDRESS_TCP(address, LOCAL_PORT_COMMAND);
		HOST_TO_GENERIC_ADDRESS_TCP(address, NULL);
		if ( xme_core_interfaceManager_add_channel_to_phy_address_mapping(
			(xme_core_interface_interfaceId_t) 2,
			COMMAND_CHANNEL,
			&address ) != XME_CORE_STATUS_SUCCESS )
		{
			return XME_CORE_STATUS_INTERNAL_ERROR;
		}
	}
	// Remote channel from chat component for topic COMMAND
	// Data is sent over TCP (reliable)
	if ( xme_core_routingTable_addOutboundRoute(
		COMMAND_CHANNEL,
		(xme_core_interface_interfaceId_t) 2
		) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Required only for testing the handshaking between PS and DAQ during startup phase.
	//// Add channel to address mapping for tcp interface
	//{
	//	xme_core_interface_address_t address;
	//	PORT_TO_GENERIC_ADDRESS_TCP(address, LOCAL_PORT_PS);
	//	HOST_TO_GENERIC_ADDRESS_TCP(address, NULL);
	//	if ( xme_core_interfaceManager_add_channel_to_phy_address_mapping(
	//		(xme_core_interface_interfaceId_t) 2,
	//		SW_REG_SET_DONE_CHANNEL,
	//		&address ) != XME_CORE_STATUS_SUCCESS )
	//	{
	//		return XME_CORE_STATUS_INTERNAL_ERROR;
	//	}
	//}
	//// Remote channel from chat component for SW_REG_SET_DONE toppic
	//// Data is sent over TCP (reliable)
	//if ( xme_core_routingTable_addOutboundRoute(
	//	SW_REG_SET_DONE_CHANNEL,
	//	(xme_core_interface_interfaceId_t) 2
	//	) != XME_CORE_STATUS_SUCCESS ) 	
	//{
	//	return XME_CORE_STATUS_INTERNAL_ERROR;
	//};

	// Add channel to address mapping for tcp interface
	{
		xme_core_interface_address_t address;
		PORT_TO_GENERIC_ADDRESS_TCP(address, LOCAL_PORT_CALIBRATE);
		HOST_TO_GENERIC_ADDRESS_TCP(address, NULL);
		if ( xme_core_interfaceManager_add_channel_to_phy_address_mapping(
			(xme_core_interface_interfaceId_t) 2,
			CALIBRATE_CHANNEL,
			&address ) != XME_CORE_STATUS_SUCCESS )
		{
			return XME_CORE_STATUS_INTERNAL_ERROR;
		}
	}
	// Remote channel from gui component for topic CALIBRATE
	// Data is sent over TCP (reliable)
	if ( xme_core_routingTable_addOutboundRoute(
		CALIBRATE_CHANNEL,
		(xme_core_interface_interfaceId_t) 2
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Add channel to address mapping for tcp interface
	{
		xme_core_interface_address_t address;
		PORT_TO_GENERIC_ADDRESS_TCP(address, LOCAL_PORT_RECALIBRATE);
		HOST_TO_GENERIC_ADDRESS_TCP(address, NULL);
		if ( xme_core_interfaceManager_add_channel_to_phy_address_mapping(
			(xme_core_interface_interfaceId_t) 2,
			RECALIBRATE_CHANNEL,
			&address ) != XME_CORE_STATUS_SUCCESS )
		{
			return XME_CORE_STATUS_INTERNAL_ERROR;
		}
	}
	// Remote channel from gui component for topic RECALIBRATE
	// Data is sent over TCP (reliable)
	if ( xme_core_routingTable_addOutboundRoute(
		RECALIBRATE_CHANNEL,
		(xme_core_interface_interfaceId_t) 2
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Remote channel to gui component for topic LOGDATA
	// Data is sent over UDP (not reliable)
	if( xme_core_routingTable_addInboundRoute(
		LOGDATA_CHANNEL,
		(xme_core_interface_interfaceId_t) 1
	) != XME_CORE_STATUS_SUCCESS )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Remote channel to gui component for topic MONITOR
	// Data is sent over UDP (not reliable)
	if( xme_core_routingTable_addInboundRoute(
		MONITOR_MESSAGE_CHANNEL,
		(xme_core_interface_interfaceId_t) 1
	) != XME_CORE_STATUS_SUCCESS )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Required only for testing the handshaking between PS and DAQ suring startup phase.
	//// Add channel to address mapping for tcp interface
	//{
	//	xme_core_interface_address_t address;
	//	PORT_TO_GENERIC_ADDRESS_TCP(address, PUBLISHER_PORT_PS);
	//	HOST_TO_GENERIC_ADDRESS_TCP(address, PUBLISHER_NAME_PS);
	//	if ( xme_core_interfaceManager_add_channel_to_phy_address_mapping(
	//		(xme_core_interface_interfaceId_t) 2,
	//		SW_REG_SET_CHANNEL,
	//		&address ) != XME_CORE_STATUS_SUCCESS )
	//	{
	//		return XME_CORE_STATUS_INTERNAL_ERROR;
	//	}
	//}
	//// Remote channel from chat component for SW_REG_SET toppic
	//// Data is sent over TCP (reliable)
	//if ( xme_core_routingTable_addInboundRoute(
	//	SW_REG_SET_CHANNEL,
	//	(xme_core_interface_interfaceId_t) 2
	//	) != XME_CORE_STATUS_SUCCESS ) 	
	//{
	//};

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
belle_II_GUI_XME_Component_activate(xme_core_nodeManager_configStruct_t* config)
{
	statusNodesTaskHandle = xme_hal_sched_addTask(maxTimestampAge, maxTimestampAge, 0 , node_status_callback, NULL);
	return XME_CORE_STATUS_SUCCESS;
}

void
belle_II_GUI_XME_Component_deactivate(xme_core_nodeManager_configStruct_t* config)
{
	xme_hal_sched_removeTask(statusNodesTaskHandle);
	XME_HAL_TABLE_FINI(status_info_nodes_table);
}

void
belle_II_GUI_XME_Component_destroy(xme_core_nodeManager_configStruct_t* config)
{
	if ( xme_core_routingTable_removeLocalSourceRoute(COMMAND_CHANNEL) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeLocalSourceRoute");
	}
	if ( xme_core_routingTable_removeLocalSourceRoute(CALIBRATE_CHANNEL) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeLocalSourceRoute");
	}
	if ( xme_core_routingTable_removeLocalSourceRoute(RECALIBRATE_CHANNEL) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeLocalSourceRoute");
	}

	if ( xme_core_routingTable_removeLocalDestinationRoute(
		LOGDATA_CHANNEL,
		xme_core_dcc_getComponentFromSubscriptionHandle(subHandle), 
		xme_core_dcc_getPortFromSubscriptionHandle(subHandle) ) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeLocalDestinationRoute");
	}

	if ( xme_core_routingTable_removeLocalDestinationRoute(
		MONITOR_MESSAGE_CHANNEL,
		xme_core_dcc_getComponentFromSubscriptionHandle(subHandleMonitor), 
		xme_core_dcc_getPortFromSubscriptionHandle(subHandleMonitor) ) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeLocalDestinationRoute");
	}

	if ( xme_core_routingTable_removeOutboundRoute(
		COMMAND_CHANNEL, 
		(xme_core_interface_interfaceId_t) 2) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeOutboundRoute");
	}
	if ( xme_core_routingTable_removeOutboundRoute(
		CALIBRATE_CHANNEL, 
		(xme_core_interface_interfaceId_t) 2) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeOutboundRoute");
	}
	if ( xme_core_routingTable_removeOutboundRoute(
		RECALIBRATE_CHANNEL, 
		(xme_core_interface_interfaceId_t) 2) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeOutboundRoute");
	}

	if ( 
		xme_core_routingTable_removeInboundRoute(
		LOGDATA_CHANNEL, 
		(xme_core_interface_interfaceId_t) 1
		) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeInboundRoute");
	}
	if ( 
		xme_core_routingTable_removeInboundRoute(
		MONITOR_MESSAGE_CHANNEL, 
		(xme_core_interface_interfaceId_t) 1
		) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeInboundRoute");
	}

	if ( xme_core_dcc_unsubscribeTopic(subHandle) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_dcc_unsubscribeTopic");
	}

	if ( xme_core_dcc_unsubscribeTopic(subHandleMonitor) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_dcc_unsubscribeTopic");
	}

	if ( xme_core_dcc_unpublishTopic(pubHandleCommand) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_dcc_unpublishTopic");
	}
	if ( xme_core_dcc_unpublishTopic(pubHandleCalibrate) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_dcc_unpublishTopic");
	}
	if ( xme_core_dcc_unpublishTopic(pubHandleRecalibrate) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_dcc_unpublishTopic");
	}

	/*if ( xme_core_routingTable_removeLocalSourceRoute(SW_REG_SET_DONE_CHANNEL) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeLocalSourceRoute");
	}
*/
	// Required only for testing the handshaking between PS and DAQ during startup phase.
	//if ( xme_core_routingTable_removeLocalDestinationRoute(
	//	SW_REG_SET_DONE_CHANNEL,
	//	xme_core_dcc_getComponentFromPublicationHandle(pubHandleSWRegisterSetDone), 
	//	xme_core_dcc_getPortFromPublicationHandle(pubHandleSWRegisterSetDone) ) != XME_CORE_STATUS_SUCCESS )
	//{
	//	printf("Failure xme_core_broker_removeLocalDestinationRoute");
	//}

	//if ( 
	//	xme_core_routingTable_removeOutboundRoute(
	//	SW_REG_SET_DONE_CHANNEL, 
	//	(xme_core_interface_interfaceId_t) 2 ) != XME_CORE_STATUS_SUCCESS )
	//{
	//	printf("Failure xme_core_broker_removeOutboundRoute");
	//}

	//if ( xme_core_dcc_unpublishTopic(pubHandleSWRegisterSetDone) != XME_CORE_STATUS_SUCCESS )
	//{
	//	printf("Failure xme_core_dcc_unpublishTopic");
	//}

	//if ( xme_core_routingTable_removeLocalDestinationRoute(
	//	SW_REG_SET_CHANNEL,
	//	xme_core_dcc_getComponentFromSubscriptionHandle(subHandleSWRegisterSet),
	//	xme_core_dcc_getPortFromSubscriptionHandle(subHandleSWRegisterSet) ) != XME_CORE_STATUS_SUCCESS )
	//{

	//}

	//{
	//	if ( xme_core_routingTable_removeInboundRoute(
	//		SW_REG_SET_CHANNEL,
	//		(xme_core_interface_interfaceId_t) 2
	//	) != XME_CORE_STATUS_SUCCESS )
	//	{
	//		
	//	};
	//}

	//if ( xme_core_dcc_unsubscribeTopic(subHandleSWRegisterSet) != XME_CORE_STATUS_SUCCESS )
	//{

	//}
}

// Wrapper function that starts the GUI. This is required since this function has to be called from other files/windows.
void callGui(int argc, char *argv[]){
	startGui(argc, argv);
}

// Wrapper function that sends data on specific toppic by using the Chromsome function xme_core_dcc_sendTopicData(). This is required since this function has to be called from other files/windows.
void sendData(int topic, void *data, uint16_t size){
	switch(topic){
      case COMMAND:
		  xme_core_dcc_sendTopicData(pubHandleCommand, data, size);
		  break;
      case CALIBRATE:
		  xme_core_dcc_sendTopicData(pubHandleCalibrate, data, size);
		  break;
	  case RECALIBRATE:
		  xme_core_dcc_sendTopicData(pubHandleRecalibrate, data, size);
		  break;
    }
}

// Wrapper function that shuts-down Chromsome. This is required since this function has to be called from other files/windows.
void shutdown(){
	xme_core_fini();
}
// Wrapper function that initializes Chromsome. This is required since this function has to be called from other files/windows.
void startup(){
	xme_core_init();
}

void node_status_callback (void* userData)
{
	xme_hal_time_interval_t timeStampAge;

	XME_HAL_TABLE_ITERATE
	(
		status_info_nodes_table,
		xme_hal_table_rowHandle_t, statusInfoNodeHandle,
		status_info_nodes_t, statusInfoNodeItem,
		{
			if(-1 != statusInfoNodeItem->lastNodeStatusTimeStamp) {
				// check age of timestamp
				timeStampAge = xme_hal_time_getInterval(&statusInfoNodeItem->lastNodeStatusTimeStamp, false);

				if (timeStampAge >= maxTimestampAge) {
					statusInfoNodeItem->nodeStatusCode = NO_CONNECTION;
					updateStatus(statusInfoNodeItem->nodeNumber,statusInfoNodeItem->nodeStatusCode);
				}

			}
		}

	);
}

void initStatusInfoNodesTable()
{
	xme_hal_table_rowHandle_t handle;
	status_info_nodes_t* inRow;
	int i;

	for(i=0; i <NUMBER_OF_BELLE_NODES ; i++) {
		handle = XME_HAL_TABLE_ADD_ITEM(status_info_nodes_table);
		inRow = XME_HAL_TABLE_ITEM_FROM_HANDLE( status_info_nodes_table, handle );
		inRow->lastNodeStatusTimeStamp = -1;
		inRow->nodeNumber = i;
		inRow->nodeStatusCode = ERROR;
	}

}

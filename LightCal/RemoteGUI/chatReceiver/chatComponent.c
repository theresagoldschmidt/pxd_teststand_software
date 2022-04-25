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
 *         Chat example project.
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
#include "chatComponent.h"
#include "logdataDefines.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define CALIBRATE_VALUE 0
#define NOT_CALIBRATE_VALUE 1
/******************************************************************************/
/***   Component configurations                                             ***/
/******************************************************************************/

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_dcc_subscriptionHandle_t subHandleCommand;
xme_core_dcc_subscriptionHandle_t subHandleCalibrate;
xme_core_dcc_subscriptionHandle_t subHandleRecalibrate;
xme_core_dcc_publicationHandle_t pubHandle;
xme_core_dcc_publicationHandle_t pubHandleError;

#define COMMAND_TOPIC_ID (xme_core_topic_t)300
#define COMMAND_CHANNEL (xme_core_dataChannel_t)123

#define SET_CHANNEL_VALUE_ID (xme_core_topic_t)302
#define CALIBRATE_CHANNEL (xme_core_dataChannel_t)125

#define SET_CHANNEL_CALIBRATION_DATA_ID (xme_core_topic_t)303
#define RECALIBRATE_CHANNEL (xme_core_dataChannel_t)126

#define LOGDATA_TOPIC_ID (xme_core_topic_t)305
#define LOGDATA_CHANNEL (xme_core_dataChannel_t)128

#define ERROR_MESSAGE_TOPIC_ID (xme_core_topic_t)778
#define ERROR_MESSAGE_CHANNEL (xme_core_dataChannel_t)129

//Change this IP address to the IP address of the sending component
#define PUBLISHER_NAME "192.168.17.48"
//#define PUBLISHER_NAME "192.168.17.36"
#define PUBLISHER_PORT_COMMAND 33211
#define PUBLISHER_PORT_CALIBRATE 33213
#define PUBLISHER_PORT_RECALIBRATE 33214

typedef struct{
	uint8_t nodeNumber;
	uint8_t commandType;
}command_data_t;

#define EEPROM_NUMBER_OF_CAL_UNITS_PER_CHANNEL 5

typedef struct{
	uint8_t nodeNumber;
	uint8_t channelNumber;
	uint8_t calibrate_bool;
	int32_t voltage;
	int32_t current;
}calibration_input_data_t;

typedef struct{
	uint8_t nodeNumber;
	uint8_t channelNumber;
	calibration_channel_t calibration_page[EEPROM_NUMBER_OF_CAL_UNITS_PER_CHANNEL];
}recalibration_data_t;

void
command_subscription
(
		xme_hal_sharedPtr_t dataHandle,
		void* userData
)
{
	command_data_t *data = (command_data_t*) xme_hal_sharedPtr_getPointer( dataHandle );
	printf("Received on command topic: Node %d\n", data->nodeNumber);
	switch(data->commandType){
	case 0:
		printf("Command Start\n");
		break;
	case 1:
		printf("Command Stop\n");
		break;
	case 3:
		printf("Command Bootload\n");
		break;
	case 6:
		printf("Command Emergency Shutdown\n");
		break;
	}
}

void
calibrate_subscription
(
		xme_hal_sharedPtr_t dataHandle,
		void* userData
)
{
	calibration_input_data_t *data = (calibration_input_data_t*) xme_hal_sharedPtr_getPointer( dataHandle ); 
	printf("Received on calibrate topic for node: %i\n", data->nodeNumber);
	printf("For channel number: %i\n", data->channelNumber);
	printf("Voltage: %d\n", data->voltage);
	printf("Current: %d\n", data->current);
}

void
recalibrate_subscription
(
		xme_hal_sharedPtr_t dataHandle,
		void* userData
)
{
	recalibration_data_t *data = (recalibration_data_t*) xme_hal_sharedPtr_getPointer( dataHandle );
	printf("Received on recalibrate topic for node: %i\n", data->nodeNumber );
	printf("For channel number: %i\n", data->channelNumber);
	printf("DAC U gain: %d\n", data->calibration_page[0].gain);
	printf("DAC U offset: %d\n", data->calibration_page[0].offset);
	printf("DAC I gain: %d\n", data->calibration_page[1].gain);
	printf("DAC I offset: %d\n", data->calibration_page[1].offset);
	printf("ADC I gain: %d\n", data->calibration_page[2].gain);
	printf("ADC I offset: %d\n", data->calibration_page[2].offset);
	printf("ADC U out gain: %d\n", data->calibration_page[3].gain);
	printf("ADC U out offset: %d\n", data->calibration_page[3].offset);
	printf("ADC U load gain: %d\n", data->calibration_page[4].gain);
	printf("ADC U load offset: %d\n", data->calibration_page[4].offset);
}

xme_core_status_t
chatComponent_create(xme_core_nodeManager_configStruct_t* config)
{
	// Create publisher for logdata topic
	pubHandle = xme_core_dcc_publishTopic( 
		LOGDATA_TOPIC_ID, 
		XME_CORE_MD_EMPTY_META_DATA, 
		false,
		NULL
	);

	if ( pubHandle == XME_CORE_DCC_INVALID_PUBLICATION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Create publisher for error message topic
	pubHandleError = xme_core_dcc_publishTopic( 
		ERROR_MESSAGE_TOPIC_ID, 
		XME_CORE_MD_EMPTY_META_DATA, 
		false,
		NULL
	);

	if ( pubHandleError == XME_CORE_DCC_INVALID_PUBLICATION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Create subscription to start topic
	subHandleCommand = xme_core_dcc_subscribeTopic( 
		COMMAND_TOPIC_ID, 
		XME_CORE_MD_EMPTY_META_DATA, 
		false,
		command_subscription, 
		NULL 
	);

	if ( subHandleCommand == XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Create subscription to calibrate topic
	subHandleCalibrate = xme_core_dcc_subscribeTopic( 
		SET_CHANNEL_VALUE_ID, 
		XME_CORE_MD_EMPTY_META_DATA, 
		false,
		calibrate_subscription, 
		NULL 
	);

	if ( subHandleCalibrate == XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Create subscription to recalibrate topic
	subHandleRecalibrate = xme_core_dcc_subscribeTopic( 
		SET_CHANNEL_CALIBRATION_DATA_ID, 
		XME_CORE_MD_EMPTY_META_DATA, 
		false,
		recalibrate_subscription, 
		NULL 
	);

	if ( subHandleRecalibrate == XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Local channel from chat component for topic logdata
	if ( xme_core_routingTable_addLocalSourceRoute( 
		LOGDATA_CHANNEL, 
		xme_core_dcc_getComponentFromPublicationHandle(pubHandle),
		xme_core_dcc_getPortFromPublicationHandle(pubHandle) 
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Local channel from chat component for topic error message
	if ( xme_core_routingTable_addLocalSourceRoute( 
		ERROR_MESSAGE_CHANNEL, 
		xme_core_dcc_getComponentFromPublicationHandle(pubHandleError),
		xme_core_dcc_getPortFromPublicationHandle(pubHandleError) 
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Local channel to chat component for topic start
	if ( xme_core_routingTable_addLocalDestinationRoute(
		COMMAND_CHANNEL,
		xme_core_dcc_getComponentFromSubscriptionHandle(subHandleCommand),
		xme_core_dcc_getPortFromSubscriptionHandle(subHandleCommand)
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Local channel to chat component for calibrate topic
	if ( xme_core_routingTable_addLocalDestinationRoute(
		CALIBRATE_CHANNEL,
		xme_core_dcc_getComponentFromSubscriptionHandle(subHandleCalibrate),
		xme_core_dcc_getPortFromSubscriptionHandle(subHandleCalibrate)
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Local channel to chat component for recalibrate topic
	if ( xme_core_routingTable_addLocalDestinationRoute(
		RECALIBRATE_CHANNEL,
		xme_core_dcc_getComponentFromSubscriptionHandle(subHandleRecalibrate),
		xme_core_dcc_getPortFromSubscriptionHandle(subHandleRecalibrate)
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Remote channel from chat component for logdata topic
	if ( xme_core_routingTable_addOutboundRoute(
		LOGDATA_CHANNEL,
		(xme_core_interface_interfaceId_t) 1
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Remote channel from chat component for error message topic
	if ( xme_core_routingTable_addOutboundRoute(
		ERROR_MESSAGE_CHANNEL,
		(xme_core_interface_interfaceId_t) 1
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	// Add channel to address mapping for tcp interface
	{
		xme_core_interface_address_t publisher;
		PORT_TO_GENERIC_ADDRESS_TCP(publisher, PUBLISHER_PORT_COMMAND);
		HOST_TO_GENERIC_ADDRESS_TCP(publisher, PUBLISHER_NAME);
		if ( xme_core_interfaceManager_add_channel_to_phy_address_mapping(
			(xme_core_interface_interfaceId_t) 2,
			COMMAND_CHANNEL,
			&publisher ) != XME_CORE_STATUS_SUCCESS )
		{
			return XME_CORE_STATUS_INTERNAL_ERROR;
		}
	}
	// Remote channel to chat component for topic start
	{
		if( xme_core_routingTable_addInboundRoute(
			COMMAND_CHANNEL,
			(xme_core_interface_interfaceId_t) 2
		) != XME_CORE_STATUS_SUCCESS )
		{
			/*
			 * If the publisher is not available, this command
			 * will fail.
			 *
			 * If it does fail, the system automatically tries
			 * to connect to the publisher until the connection
			 * is established.
			 *
			 * If the connection breaks, the system automatically
			 * tries to reconnect.
			 */
			printf("Connection to publisher failed.\n");
			printf("Retry in progress...\n");
		};
	}

	// Add channel to address mapping for tcp interface
	{
		xme_core_interface_address_t publisher;
		PORT_TO_GENERIC_ADDRESS_TCP(publisher, PUBLISHER_PORT_CALIBRATE);
		HOST_TO_GENERIC_ADDRESS_TCP(publisher, PUBLISHER_NAME);
		if ( xme_core_interfaceManager_add_channel_to_phy_address_mapping(
			(xme_core_interface_interfaceId_t) 2,
			CALIBRATE_CHANNEL,
			&publisher ) != XME_CORE_STATUS_SUCCESS )
		{
			return XME_CORE_STATUS_INTERNAL_ERROR;
		}
	}
	// Remote channel to chat component for topic calibrate
	{
		if( xme_core_routingTable_addInboundRoute(
			CALIBRATE_CHANNEL,
			(xme_core_interface_interfaceId_t) 2
		) != XME_CORE_STATUS_SUCCESS )
		{
			/*
			 * If the publisher is not available, this command
			 * will fail.
			 *
			 * If it does fail, the system automatically tries
			 * to connect to the publisher until the connection
			 * is established.
			 *
			 * If the connection breaks, the system automatically
			 * tries to reconnect.
			 */
			printf("Connection to publisher failed.\n");
			printf("Retry in progress...\n");
		};
	}

	// Add channel to address mapping for tcp interface
	{
		xme_core_interface_address_t publisher;
		PORT_TO_GENERIC_ADDRESS_TCP(publisher, PUBLISHER_PORT_RECALIBRATE);
		HOST_TO_GENERIC_ADDRESS_TCP(publisher, PUBLISHER_NAME);
		if ( xme_core_interfaceManager_add_channel_to_phy_address_mapping(
			(xme_core_interface_interfaceId_t) 2,
			RECALIBRATE_CHANNEL,
			&publisher ) != XME_CORE_STATUS_SUCCESS )
		{
			return XME_CORE_STATUS_INTERNAL_ERROR;
		}
	}
	// Remote channel to chat component for topic calibrate
	{
		if( xme_core_routingTable_addInboundRoute(
			RECALIBRATE_CHANNEL,
			(xme_core_interface_interfaceId_t) 2
		) != XME_CORE_STATUS_SUCCESS )
		{
			/*
			 * If the publisher is not available, this command
			 * will fail.
			 *
			 * If it does fail, the system automatically tries
			 * to connect to the publisher until the connection
			 * is established.
			 *
			 * If the connection breaks, the system automatically
			 * tries to reconnect.
			 */
			printf("Connection to publisher failed.\n");
			printf("Retry in progress...\n");
		};
	}

	return XME_CORE_STATUS_SUCCESS;
}

void
chatComponent_callback (void* userData)
{
	char temp[1024];
	//int count = 0;
	//print data;
	monitor_information_node_t dataToSend;
	//calibration_channel_values_t dataToSend;
	//channel_descr_t descr;

	printf("Chat client ready. Please enter some text. It will we displayed on other running clients. Multiple instances of this program may run at once, without need for reconfiguration.\n");

	printf("Messages will be printed twice, as they are locally as well as remotely routed.\n");

	dataToSend.nodeNumber = 1;
	dataToSend.node_status = 0;
	dataToSend.monitor_node_info_all_channels[0].current = 20;
	dataToSend.monitor_node_info_all_channels[0].voltage_at_regulator = 200;
	dataToSend.monitor_node_info_all_channels[0].voltage_at_load = 95;
	dataToSend.monitor_node_info_all_channels[0].regulator_status_bit = 0;
	
	// This can be used to test the LOG topic with the GUI under Windows.
	//data.logInfo.log_node_info_all_channels[5].current = 30;
	//data.logInfo.log_node_info_all_channels[5].voltage_at_regulator = 300;
	//data.logInfo.log_node_info_all_channels[5].voltage_at_load = 26;
	//data.logInfo.log_node_info_all_channels[5].regulator_status_bit = 1;

	///*dataToSend.calibration_channel_values[0].gain = 0;
	//dataToSend.calibration_channel_values[0].offset = 1;
	//dataToSend.calibration_channel_values[1].gain = 2;
	//dataToSend.calibration_channel_values[1].offset = 3;
	//dataToSend.calibration_channel_values[2].gain = 4;
	//dataToSend.calibration_channel_values[2].offset = 5;
	//dataToSend.calibration_channel_values[3].gain = 6;
	//dataToSend.calibration_channel_values[3].offset = 7;
	//dataToSend.calibration_channel_values[4].gain = 8;
	//dataToSend.calibration_channel_values[4].offset = 9;
	//dataToSend.voltage_limit = 10;
	//dataToSend.current_limit = 11;
	//dataToSend.step_size_t = 12;
	//dataToSend.delta_t = 13;

	//descr.channel_data = dataToSend;
	//descr.selection_config.adc_current_channel = (adc_selector_t) 0;
	//descr.selection_config.adc_load_channel = (adc_selector_t) 1;
	//descr.selection_config.adc_mux_select_current = (mux_selector_t) 2;
	//descr.selection_config.adc_mux_select_load = (mux_selector_t) 3;
	//descr.selection_config.adc_mux_select_regulator = (mux_selector_t) 4;
	//descr.selection_config.adc_regulator_channel = (adc_selector_t) 0;
	//descr.selection_config.board_selection = (board_selector_t) 2;
	//descr.selection_config.dac_1_channel = (dac_selector_t) 7;
	//descr.selection_config.dac_2_channel = (dac_selector_t) 6;*/

	//data.nodeNumber = 1;
	//data.channelNumber = 0;
	//data.code_number = 1;
	////data.channel_values = dataToSend;
	////data.channel = descr;

	//data.adc_value_mili_scale =3000;
	//
	//xme_core_dcc_sendTopicData(pubHandle, &data, sizeof(data));

	while( fgets(temp,1024,stdin) != NULL )
	{
		/*if ( strlen(temp) )
		{
			if ( temp[strlen(temp)-1] == '\n' ) temp[strlen(temp)-1] = '\0';
			xme_core_dcc_sendTopicData(pubHandle, temp, strlen(temp)+1);
		}
		/*if ( strlen(temp) && count == 0)
		{
			if ( temp[strlen(temp)-1] == '\n' ) temp[strlen(temp)-1] = '\0';
			//xme_core_dcc_sendTopicData(pubHandle, temp, strlen(temp)+1);
			data.code_number = 2;
			data.channelNumber = 5;
			data.nodeNumber = 19;
			xme_core_dcc_sendTopicData(pubHandle, &data, sizeof(data)); 
			count++;
		}
		else if ( strlen(temp) && count == 1)
		{
			if ( temp[strlen(temp)-1] == '\n' ) temp[strlen(temp)-1] = '\0';
			data.code_number = 4;
			data.channelNumber = 5;
			data.nodeNumber = 19;
			xme_core_dcc_sendTopicData(pubHandle,  &data, sizeof(data));
			count ++;
		}
		if ( strlen(temp) && count == 2)
		{

			if ( temp[strlen(temp)-1] == '\n' ) temp[strlen(temp)-1] = '\0';
			//xme_core_dcc_sendTopicData(pubHandleError, temp, strlen(temp)+1);
			xme_core_dcc_sendTopicData(pubHandleError, &dataToSend, sizeof(dataToSend));
			count = 0;
		}*/
		if ( strlen(temp))
		{

			if ( temp[strlen(temp)-1] == '\n' ) temp[strlen(temp)-1] = '\0';
			//xme_core_dcc_sendTopicData(pubHandleError, temp, strlen(temp)+1);
			xme_core_dcc_sendTopicData(pubHandleError, &dataToSend, sizeof(dataToSend));
		}
	}
}

xme_core_status_t
chatComponent_activate(xme_core_nodeManager_configStruct_t* config)
{
	xme_hal_sched_addTask(0, 0, 0, chatComponent_callback, NULL);
	return XME_CORE_STATUS_SUCCESS;
}

void
chatComponent_deactivate(xme_core_nodeManager_configStruct_t* config)
{
	// Nothing to do here
}

void
chatComponent_destroy(xme_core_nodeManager_configStruct_t* config)
{
	if ( xme_core_routingTable_removeLocalSourceRoute(LOGDATA_CHANNEL) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeLocalSourceRoute");
	}

	if ( xme_core_routingTable_removeLocalSourceRoute(ERROR_MESSAGE_CHANNEL) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeLocalSourceRoute");
	}

	if ( xme_core_routingTable_removeLocalDestinationRoute(
		COMMAND_CHANNEL,
		xme_core_dcc_getComponentFromSubscriptionHandle(subHandleCommand), 
		xme_core_dcc_getPortFromSubscriptionHandle(subHandleCommand) ) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeLocalDestinationRoute");
	}
	if ( xme_core_routingTable_removeLocalDestinationRoute(
		CALIBRATE_CHANNEL,
		xme_core_dcc_getComponentFromSubscriptionHandle(subHandleCalibrate), 
		xme_core_dcc_getPortFromPublicationHandle(pubHandle) ) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeLocalDestinationRoute");
	}
	if ( xme_core_routingTable_removeLocalDestinationRoute(
		RECALIBRATE_CHANNEL,
		xme_core_dcc_getComponentFromSubscriptionHandle(subHandleRecalibrate), 
		xme_core_dcc_getPortFromPublicationHandle(pubHandle) ) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeLocalDestinationRoute");
	}

	if ( 
		xme_core_routingTable_removeOutboundRoute(
		LOGDATA_CHANNEL, 
		(xme_core_interface_interfaceId_t) 1
		) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeOutboundRoute");
	}

	if ( 
		xme_core_routingTable_removeOutboundRoute(
		ERROR_MESSAGE_CHANNEL, 
		(xme_core_interface_interfaceId_t) 1
		) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeOutboundRoute");
	}

	if ( xme_core_routingTable_removeInboundRoute(
		COMMAND_CHANNEL, 
		(xme_core_interface_interfaceId_t) 2
		) != XME_CORE_STATUS_SUCCESS )
		{
			printf("Failure xme_core_broker_removeInboundRoute");
		}
	if ( xme_core_routingTable_removeInboundRoute(
		CALIBRATE_CHANNEL, 
		(xme_core_interface_interfaceId_t) 2
		) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeInboundRoute");
	}
	if ( xme_core_routingTable_removeInboundRoute(
		RECALIBRATE_CHANNEL, 
		(xme_core_interface_interfaceId_t) 2
		) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeInboundRoute");
	}

	if ( xme_core_dcc_unsubscribeTopic(subHandleCommand) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_dcc_unsubscribeTopic");
	}
	if ( xme_core_dcc_unsubscribeTopic(subHandleCalibrate) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_dcc_unsubscribeTopic");
	}
	if ( xme_core_dcc_unsubscribeTopic(subHandleRecalibrate) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_dcc_unsubscribeTopic");
	}

	if ( xme_core_dcc_unpublishTopic(pubHandle) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_dcc_unpublishTopic");
	}

	if ( xme_core_dcc_unpublishTopic(pubHandleError) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_dcc_unpublishTopic");
	}

}
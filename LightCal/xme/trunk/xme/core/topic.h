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
 *         Preliminary topic registry.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Marcel Flesch <fleschmarcel@mytum.de>
 */

#ifndef XME_CORE_TOPIC_H
#define XME_CORE_TOPIC_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/component.h"
#include "xme/core/dataChannel.h"
#include "xme/core/device.h"
#include "xme/core/interface.h"
#include "xme/core/node.h"
#include "xme/core/log.h"

#include <stdint.h>

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_core_topic_t
 *
 * \brief  Unique numeric identifier for a topic.
 *         Every topic name maps to a topic identifier.
 */
typedef enum
{
	// Important note:
	// Extend checks in XME_CORE_DCC_ASSERT_REFUSE_INVALID_DCC_TOPIC to ensure that only valid DCC topics can be published or subscribed.
	// Extend checks in XME_CORE_DCC_ASSERT_REFUSE_INTERNAL_PUBLICATION_TOPIC to ensure that no internal publication topics can be published.
	// Extend checks in XME_CORE_DCC_ASSERT_REFUSE_INTERNAL_SUBSCRIPTION_TOPIC to ensure that no internal subscription topics can be subscribed.
	// Extend checks in XME_CORE_RR_ASSERT_REFUSE_INVALID_REQUEST_TOPIC to ensure that only valid request topics can be requested.
	// Extend checks in XME_CORE_RR_ASSERT_REFUSE_INTERNAL_REQUEST_TOPIC to ensure that no internal request topics can be requested.
	// Extend checks in XME_CORE_RR_ASSERT_REFUSE_INVALID_RESPONSE_TOPIC to ensure that only valid response topics can be responsed.
	// Extend checks in XME_CORE_RR_ASSERT_REFUSE_INTERNAL_RESPONSE_TOPIC to ensure that no internal response topics can be responsed.

	XME_CORE_TOPIC_INVALID_TOPIC = 0x00000000, ///< Invalid topic identifier.

	XME_CORE_TOPIC_LOCAL_ANNOUNCEMENT = 0x00000001, ///< XME local announcement topic.
	XME_CORE_TOPIC_REMOTE_ANNOUNCEMENT, ///< XME remote announcement topic.
	XME_CORE_TOPIC_REMOTE_MODIFY_ROUTING_TABLE, ///< XME remote modify routing table topic.
	XME_CORE_TOPIC_NEW_NODE_REQUEST, ///< New node request topic.
	XME_CORE_TOPIC_NEW_NODE_RESPONSE, ///< New node response topic.
	XME_CORE_TOPIC_MANAGEMENT_CHANNELS_TO_EDGE_NODE, ///< Topic containing management channels to edge node.
	XME_CORE_TOPIC_MANAGEMENT_CHANNELS_TO_NEW_NODE, ///< Topic containing management channels to edge node.

	XME_CORE_TOPIC_LOGIN_REQUEST = 0x00000010, ///< Login request topic.
	XME_CORE_TOPIC_LOGIN_RESPONSE, ///< Login response topic.

	XME_CORE_TOPIC_LOG_MESSAGE = 0x00000050, ///< Log message topic.

	XME_CORE_TOPIC_COMMON = 0x00000100, ///< Minimum topic identifier for common (not reserved for XME itself) topics.
	XME_CORE_TOPIC_EVENT, ///< Notification event (with no actual data).
	XME_CORE_TOPIC_FLAG, ///< Boolean flag.
	XME_CORE_TOPIC_INTEGER, ///< Integer number.
	XME_CORE_TOPIC_UNSIGNED_INTEGER, ///< Unsigned integer number.
	XME_CORE_TOPIC_DECIMAL, ///< Decimal or integer number.
	XME_CORE_TOPIC_STRING, ///< String.

	XME_CORE_TOPIC_USER = 0x00001000, ///< Minimum topic identifier for user-defined (not reserved for XME itself) topics.

	// multifunk related topics
	XME_CORE_TOPIC_MODBUS_REQUEST,	///< Modbus request, used by multifunk
	XME_CORE_TOPIC_MODBUS_RESPONSE, ///< Modbus response, used by multifunk
	XME_CORE_TOPIC_SYSTEM_STATE,	///< System State of mst-network, used by multifunk
	XME_CORE_TOPIC_TEMPERATURE,		///< Temperature from mst sensors, used by multifunk
	XME_CORE_TOPIC_PRESSURE,		///< Pressure from mst sensors, used by multifunk
	XME_CORE_TOPIC_TEMPFUSION,		///< Fusionized temperature data from mst sensors, used by multifunk
	XME_CORE_TOPIC_PRESFUSION,		///< Fusionized pressure data from mst sensors, used by multifunk
	XME_CORE_TOPIC_SYSTEM_ERROR,	///< Channel for System errors, like disconnectet sensors

	XME_CORE_TOPIC_ARRAY_OF_DATA, ///< data array.

	XME_CORE_TOPIC_MAX = 0xFFFFFFFF ///< Maximum valid topic identifier.
}
xme_core_topic_t;

/**
 * \typedef xme_core_topic_newNodeRequestData_t
 *
 * \brief  Data that are being transmitted along with a new node request.
 */
#pragma pack(push, 1)
typedef struct
{
	xme_core_node_nodeId_t newNodeId; ///< Unique node identifier if assigned or XME_CORE_NODEMANAGER_INVALID_NODE_ID otherwise.
	xme_core_interface_interfaceId_t newNodeInterface; ///< Network interface of new node to reach edge node.
	xme_core_node_nodeId_t edgeNodeId; ///< Unique node identifier of the node that first forwarded the login request.
	xme_core_interface_interfaceId_t edgeNodeInterface; ///< Network interface of edge node to reach new node.

}
xme_core_topic_newNodeRequestData_t;
#pragma pack(pop)

/**
 * \typedef xme_core_topic_newNodeResponseData_t
 *
 * \brief  Data that are being transmitted along with a new node response.
 */
#pragma pack(push, 1)
typedef struct
{
	xme_core_dataChannel_t remoteAnnouncementDataChannel; ///< Data channel that should be used by the local directory of the new node to send announcements to its master directory.
	xme_core_dataChannel_t remoteModifyRoutingTableDataChannel; ///< Data channel used by the master directory to send modify routing table commands to the new node.
}
xme_core_topic_newNodeResponseData_t;
#pragma pack(pop)

/**
 * \typedef xme_core_topic_managementChannelsPacket_t
 *
 * \brief  Data that are being transmitted to construct the management channels during login.
 */
#pragma pack(push, 1)
typedef struct
{
	xme_core_interface_interfaceId_t interfaceId; ///< Node unique identifier of the interface used for the management channels.
	xme_core_dataChannel_t remoteAnnouncementDataChannel; ///< Data channel that should be used by the local directory of the new node to send announcements to its master directory.
	xme_core_dataChannel_t remoteModifyRoutingTableDataChannel; ///< Data channel used by the master directory to send modify routing table commands to the new node.
}
xme_core_topic_managementChannelsPacket_t;
#pragma pack(pop)

/**
 * \typedef xme_core_topic_loginRequestData_t
 *
 * \brief  Data that are being transmitted along with a login request.
 */
#pragma pack(push, 1)
typedef struct
{
	xme_core_device_type_t deviceType; ///< Device type.
	xme_core_device_guid_t deviceGuid; ///< Globally unique device identifier (e.g., serial number, MAC address).
	xme_core_node_nodeId_t edgeNodeId; ///< Unique node identifier of the node that first forwarded the login request.
	xme_core_interface_interfaceId_t newNodeInterfaceId; ///< Node unique identifier of the interface on the new node where the login request originated from.
	xme_core_interface_interfaceId_t edgeNodeInterfaceId; ///< Node unique identifier of the interface on the edge node that first received the login request.
}
xme_core_topic_loginRequestData_t;
#pragma pack(pop)

/**
 * \typedef xme_core_topic_loginResponseData_t
 *
 * \brief  Data that are being transmitted along with a login response.
 */
#pragma pack(push, 1)
typedef struct
{
	// TODO (See ticket #766): The first two members are actually not necessary, because other RR mechanisms permit the association of the request to the response.
	//                         However, the respective members *are* necessary in the "last hop" to the node to be logged in, which typically does not use data centric communication.
	xme_core_device_type_t deviceType; ///< Device type (corresponds to the value of the respective request).
	xme_core_device_guid_t deviceGuid; ///< Globally unique device identifier (e.g., serial number, MAC address, corresponds to the value of the respective request).
	xme_core_node_nodeId_t newNodeId; ///< Assigned unique node identifier.
	xme_core_node_nodeId_t edgeNodeId; ///< Unique node identifier of the node that first forwarded the login request.
	xme_core_interface_interfaceId_t edgeNodeInterfaceId; ///< Node unique identifier of the interface on the edge node that first received the login request.
	xme_core_dataChannel_t remoteAnnouncementDataChannel; ///< Data channel that should be used by the local directory of the new node to send announcements to its master directory.
	xme_core_dataChannel_t remoteModifyRoutingTableDataChannel; ///< Data channel used by the master directory to send modify routing table commands to the new node.
}
xme_core_topic_loginResponseData_t;
#pragma pack(pop)

/**
 * \typedef xme_core_announcementType_t
 *
 * \brief  Announcement types.
 */
typedef enum
{
	XME_CORE_ANNOUNCEMENT_PUBLISH_TOPIC = 1, ///< A new topic has been published.
	XME_CORE_ANNOUNCEMENT_UNPUBLISH_TOPIC, ///< A topic has been unpublished.
	XME_CORE_ANNOUNCEMENT_SUBSCRIBE_TOPIC = 4, ///< A new topic subscription has been issued.
	XME_CORE_ANNOUNCEMENT_UNSUBSCRIBE_TOPIC, ///< A topic subscription has been discontinued.
	XME_CORE_ANNOUNCEMENT_PUBLISH_REQUEST = 8, ///< A new request (client) has been published.
	XME_CORE_ANNOUNCEMENT_UNPUBLISH_REQUEST, ///< A request (client) has been unpublished.
	XME_CORE_ANNOUNCEMENT_PUBLISH_REQUEST_HANDLER, ///< A new request handler (server) has been published.
	XME_CORE_ANNOUNCEMENT_UNPUBLISH_REQUEST_HANDLER ///< A request handler (server) has been unpublished.
}
xme_core_announcementType_t;

/**
 * \typedef xme_core_topic_announcementPacket_t
 *
 * \brief  Data that are being transmitted along with a announcement.
 *
 *         An announcement is used to notify the global directory of changes
 *         in a component's configuration with respect to its ports.
 */
#pragma pack(push, 1)
typedef struct
{
	xme_core_announcementType_t announcementType; ///< Announcement type. The union member is to be interpreted depending on the value of this field.
	xme_core_node_nodeId_t nodeId; ///< Node identifier of the node issuing this announcement.
	xme_core_component_t componentId; ///< Locally valid identifier of the component issuing this announcement.
	xme_core_component_port_t portId; ///< Locally valid identifier of the port issuing this announcement.
	union
	{
		// Publish topic
		struct
		{
			xme_core_topic_t publicationTopic; ///< Topic that has been published.
			// TODO: meta data. See ticket #646
			bool onlyLocal; ///< Defines if publication is limited to the local node or not.
		}
		publishTopic;

		// Unpublish topic
		// (no further parameters)

		// Subscribe topic
		struct
		{
			xme_core_topic_t subscriptionTopic; ///< Topic that has been subscribed to.
			// TODO: Filter meta data. See ticket #646
			bool onlyLocal; ///< Defines if subscription is limited to the local node or not.
		}
		subscribeTopic;

		// Unpublish subscription
		// (no further parameters)

		// Publish request (client)
		struct
		{
			xme_core_component_port_t responseHandlingPort; ///< Response handling port.
			xme_core_topic_t requestTopic; ///< Request topic that has been published.
			// TODO: meta data. See ticket #646
			xme_core_topic_t responseTopic; ///< Response topic that is expected by request publisher.
			// TODO: meta data. See ticket #646
			bool manyResponses; ///< Defines if request wants to get only one or as many as possilbe responses.
			bool onlyLocal; ///< Defines if request is limited to the local node or not.
		}
		publishRequest;

		// Unpublish request (client)
		// (no further parameters)

		// Publish request handler (server)
		struct
		{
			xme_core_component_port_t responseSendingPort; ///< Response sending port.
			xme_core_topic_t requestTopic; ///< Request topic that has been published.
			// TODO: meta data. See ticket #646
			xme_core_topic_t responseTopic; ///< Response topic that has been published.
			// TODO: meta data. See ticket #646
			bool onlyLocal; ///< Defines if request handler is limited to the local node or not.
		}
		publishRequestHandler;

		// Unpublish request handler (server)
		// (no further parameters)
	};
}
xme_core_topic_announcementPacket_t;
#pragma pack(pop)

/**
 * \typedef xme_core_modifyRoutingTableType_t
 *
 * \brief  Routing table modification types.
 */
typedef enum
{
	XME_CORE_MODIFY_ROUTING_TABLE_TYPE_ADD_LOCAL_SOURCE_ROUTE = 0, ///< Add local source route.
	XME_CORE_MODIFY_ROUTING_TABLE_TYPE_REMOVE_LOCAL_SOURCE_ROUTE = 1, ///< Remove local source route.
	XME_CORE_MODIFY_ROUTING_TABLE_TYPE_ADD_LOCAL_DESTINATION_ROUTE = 2, ///< Add local destination route.
	XME_CORE_MODIFY_ROUTING_TABLE_TYPE_REMOVE_LOCAL_DESTINATION_ROUTE = 3, ///< Remove local destination route.
	XME_CORE_MODIFY_ROUTING_TABLE_TYPE_ADD_INBOUND_ROUTE = 4, ///< Add inbound route.
	XME_CORE_MODIFY_ROUTING_TABLE_TYPE_REMOVE_INBOUND_ROUTE = 5, ///< Remove inbound route.
	XME_CORE_MODIFY_ROUTING_TABLE_TYPE_ADD_OUTBOUND_ROUTE = 6, ///< Add outbound route.
	XME_CORE_MODIFY_ROUTING_TABLE_TYPE_REMOVE_OUTBOUND_ROUTE = 7, ///< Remove outbound route.
	XME_CORE_MODIFY_ROUTING_TABLE_TYPE_ADD_TRANSLATION_ROUTE = 8, ///< Add translation route.
	XME_CORE_MODIFY_ROUTING_TABLE_TYPE_REMOVE_TRANSLATION_ROUTE = 9 ///< Remove translation route.
}
xme_core_modifyRoutingTableType_t;

/**
 * \typedef xme_core_topic_modifyRoutingTablePacket_t
 *
 * \brief  Data that are being transmitted along with a routing table modification request.
 *
 *         A routing table modification is used to request the routing table change.
 */
#pragma pack(push, 1)
typedef struct
{
	xme_core_modifyRoutingTableType_t modifyRoutingTableType; ///< Routing table modification type. The union member is to be interpreted depending on the value of this field.
	xme_core_dataChannel_t dataChannel; ///< Data channel of route.
	union
	{
		// Add/remove local source route
		struct
		{
			xme_core_component_t component; ///< Locally valid identifier of component.
			xme_core_component_port_t port; ///< Locally valid identifier of port.
		}
		localSourceRoute;

		// Add/remove local destination route
		struct
		{
			xme_core_component_t component; ///< Locally valid identifier of component.
			xme_core_component_port_t port; ///< Locally valid identifier of port.
		}
		localDestinationRoute;

		// Add/remove outbound route
		struct
		{
			xme_core_interface_interfaceId_t networkInterface; ///< Locally valid identifier of network interface.
		}
		outboundRoute;

		// Add/remove inbound route
		struct
		{
			xme_core_interface_interfaceId_t networkInterface; ///< Locally valid identifier of network interface.
		}
		inboundRoute;

		// Add/remove inbound route
		struct
		{
			xme_core_dataChannel_t newDataChannel; ///< New data channel of route.
		}
		translationRoute;
	};
}
xme_core_topic_modifyRoutingTablePacket_t;
#pragma pack(pop)

/**
 * \typedef xme_core_topic_flagData_t
 *
 * \brief  Data that are being transmitted along with a flag topic.
 */
#pragma pack(push, 1)
typedef struct
{
	bool flag; ///< Boolean flag.
}
xme_core_topic_flagData_t;
#pragma pack(pop)

/**
 * \typedef xme_core_topic_integerData_t
 *
 * \brief  Data that are being transmitted along with an integer topic.
 */
#pragma pack(push, 1)
typedef struct
{
	int number; ///< Number.
}
xme_core_topic_integerData_t;
#pragma pack(pop)

/**
 * \typedef xme_core_topic_decimalData_t
 *
 * \brief  Data that are being transmitted along with a decimal topic.
 */
#pragma pack(push, 1)
typedef struct
{
	float decimal; ///< Decimal number.
}
xme_core_topic_decimalData_t;
#pragma pack(pop)
typedef struct
{
	char str[10];
	uint16_t number;
}
xme_core_topic_arrayData_t;
typedef struct
{
	char str[10]; ///< string.
}
xme_core_topic_stringData_t;

/**
 * \typedef xme_core_topic_logMsgData_t
 *
 * \brief  Data that are being transmitted along with a log message topic (XME_CORE_TOPIC_LOG_MESSAGE).
 */
// TODO: Change log message data definition when metadata filtering is available (see ticket #665).
//       The struct itself will only contain the message size, all other information will be delivered
//       via metadata.
//       Candidates for metadata: severity, componentId/name, deviceGuid, timestamp
#pragma pack(push, 1)
typedef struct
{
	xme_core_device_guid_t deviceGuid; ///< Device guid of node that sent this message.
	xme_core_device_type_t deviceType; ///< Device type of node that sent this message.
	xme_core_component_t componentId;  ///< Local id of sending component.
	xme_log_severity_t severity;       ///< Severity.
	uint16_t msgSize;      ///< Size of message string in bytes.
	uint16_t compNameSize; ///< Size of component name string in bytes.
	
	// The following data must be appended in the given order after the struct when sending log message data:
	//
	// char[msgSize] msg           ///< Log message.
	// char[compNameSize] compName ///< Name of component which sent the message.
}
xme_core_topic_logMsgData_t;
#pragma pack(pop)

#endif // #ifndef XME_CORE_TOPIC_H

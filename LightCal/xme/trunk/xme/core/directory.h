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
 *         Directory.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Gerd Kainz <kainz@fortiss.org>
 *         Nadine Keddis <keddis@fortiss.org>
 */

#ifndef XME_CORE_DIRECTORY_H
#define XME_CORE_DIRECTORY_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdbool.h>
#include <stdint.h>

#include "xme/core/component.h"
#include "xme/core/core.h"
#include "xme/core/dcc.h"
#include "xme/core/interface.h"
#include "xme/core/md.h"
#include "xme/core/nodeManager.h"
#include "xme/core/topic.h"

#include "xme/hal/table.h"

#ifndef XME_CORE_DIRECTORY_TYPE_LOCAL
	#ifndef XME_CORE_DIRECTORY_TYPE_MASTER
		#error Directory type is not defined! Please define either XME_CORE_DIRECTORY_TYPE_LOCAL or XME_CORE_DIRECTORY_TYPE_MASTER!
	#endif // #ifndef XME_CORE_DIRECTORY_TYPE_MASTER
#endif // #ifndef XME_CORE_DIRECTORY_TYPE_LOCAL

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/*
 * \typedef xme_core_directory_dataChannelMapping_t
 *
 * \brief   Mapping between topic and meta data combination and data channel.
 */
typedef struct
{
	xme_core_topic_t topic; ///< Topic.
	xme_core_md_topicMetaDataHandle_t metaData; ///< Meta data belonging to topic.
	xme_core_dataChannel_t dataChannel; ///< Data channel assigned to topic and meta data combination.
}
xme_core_directory_dataChannelMapping_t;

#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
	/*
	 * \typedef xme_core_directory_node_t
	 *
	 * \brief   Stores information about a node.
	 */
	typedef struct
	{
		xme_core_node_nodeId_t node; ///< Node identifier.
		xme_core_dataChannel_t remoteAnnouncementDataChannel; ///<  Data channel for receiving announcements of node.
		xme_core_dataChannel_t remoteModifyRoutingTableDataChannel; ///< Data channel for modifying the node's routing table.
	}
	xme_core_directory_node_t;

	/*
	 * \typedef xme_core_directory_neighbor_t
	 *
	 * \brief   Neighbor of a node.
	 */
	typedef struct
	{
		xme_core_interface_interfaceId_t sendInterface; ///< Send interface of node to reach neighbor.
		xme_core_node_nodeId_t node; ///< Neighbor.
		xme_core_interface_interfaceId_t receiveInterface; ///< Receive interface of neighbor.
	}
	xme_core_directory_neighbor_t;

	/*
	 * \typedef xme_core_directory_communicationLink_t
	 *
	 * \brief   Communication link representing a communication relation between two nodes.
	 */
	typedef struct
	{
		xme_core_node_nodeId_t node1; ///< First node.
		xme_core_interface_interfaceId_t interfaceNode1; ///< Communication interface of first node.
		xme_core_node_nodeId_t node2; ///< Second node.
		xme_core_interface_interfaceId_t interfaceNode2; ///< Communication interface of second node.
	}
	xme_core_directory_communicationLink_t;


	/*
	 * \typedef xme_core_directory_link_t
	 *
	 * \brief   Communication link of a data channel.
	 */
	typedef struct
	{
		xme_core_node_nodeId_t sourceNode; ///< Source node of communication link.
		xme_core_interface_interfaceId_t sourceInterface; ///< Used communication interface of source node.
		xme_core_node_nodeId_t sinkNode; ///< Sink node of communication link.
		xme_core_interface_interfaceId_t sinkInterface; ///< Used communication interface of sink node.
		xme_core_dataChannel_t dataChannel; ///< Data channel to which communication link belongs.
		uint8_t count; ///< Number of connections realized by link.
	}
	xme_core_directory_link_t;

	/*
	 * \typedef xme_core_directory_datachannelTranslation_t
	 *
	 * \brief   Translation of data channel.
	 */
	typedef struct
	{
		xme_core_node_nodeId_t node; ///< Node at which translation happens.
		xme_core_dataChannel_t oldDataChannel; ///< Old data channel.
		xme_core_dataChannel_t newDataChannel; ///< New data channel.
	}
	xme_core_directory_dataChannelTranslation_t;
#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER

/*
 * \typedef xme_core_directory_endpoint_t
 *
 * \brief   Communication endpoint.
 */
typedef struct
{
	xme_core_node_nodeId_t node; ///< Node of communication endpoint.
	xme_core_component_t component; ///< Component of communication endpoint.
	xme_core_component_port_t port; ///< Port of communication endpoint.
}
xme_core_directory_endpoint_t;

typedef struct
{
	xme_core_directory_endpoint_t* source; ///< Source endpoint of communication relation.
	xme_core_dataChannel_t localDataChannel; ///< Local data channel used for communication.
	xme_core_dataChannel_t globalDataChannel; ///< Global data channel used for communication.
	xme_core_directory_endpoint_t* sink; ///< Sink endpoint of communication relation.
}
xme_core_directory_communicationRelation_t;

#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
	/*
	 * \typedef xme_core_directory_endpointType_t
	 *
	 * \brief   Type of communication endpoint.
	 */
	typedef enum
	{
		XME_CORE_DIRECTORY_ENDPOINT_TYPE_SOURCE = 0, ///< Source (out port).
		XME_CORE_DIRECTORY_ENDPOINT_TYPE_SINK = 1 ///< Sink (in port).
	}
	xme_core_directory_endpointType_t;

	/*
	 * \typedef xme_core_directory_endpointLink_t
	 *
	 * \brief   Communication link to endpoint of a data channel.
	 */
	typedef struct
	{
		xme_core_directory_endpoint_t endpoint; ///< Endpoint of communication link.
		xme_core_directory_endpointType_t type; ///< Type of endpoint.
		xme_core_dataChannel_t dataChannel; ///< Data channel to which communication link belongs.
		uint8_t count; ///< Number of connections realized by link.
	}
	xme_core_directory_endpointLink_t;

	/*
	 * \typedef xme_core_directory_dataChannelToNode_t
	 *
	 * \brief   Stores data channel connected to a specific note.
	 */
	typedef struct
	{
		xme_core_dataChannel_t dataChannel; ///< Data channel to reach node.
		xme_core_node_nodeId_t node; ///< Node which is reached via the data channel.
	}
	xme_core_directory_dataChannelToNode_t;
#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER

/*
 * \typedef xme_core_directory_publicationItem_t
 *
 * \brief   Publication.
 */
typedef struct
{
	xme_core_directory_endpoint_t publisher; ///< Publisher.
	xme_core_topic_t topic; ///< Published topic.
	xme_core_md_topicMetaDataHandle_t metaData; ///< Meta data belonging to publication.
	bool onlyLocal; ///< Defines if publication is limited to the local node or not.
}
xme_core_directory_publicationItem_t;

/*
 * \typedef xme_core_directory_subscriptionItem_t
 *
 * \brief   Subscription.
 */
typedef struct
{
	xme_core_directory_endpoint_t subscriber; ///< Subscriber.
	xme_core_topic_t topic; ///< Subscribed topic.
	xme_core_md_topicMetaDataHandle_t metaData; ///< Meta data belonging to subscription.
	bool onlyLocal; ///< Defines if subscription is limited to the local node or not.
}
xme_core_directory_subscriptionItem_t;

/*
 * \typedef xme_core_directory_requestItem_t
 *
 * \brief   Request.
 */
typedef struct
{
	xme_core_directory_endpoint_t requester; ///< Requester.
	xme_core_topic_t requestTopic; ///< Request topic.
	xme_core_md_topicMetaDataHandle_t requestMetaData; ///< Meta data belonging to request.
	xme_core_topic_t responseTopic; ///< Response topic.
	xme_core_md_topicMetaDataHandle_t responseMetaDataFilter; ///< Meta data filter belonging to response.
	xme_core_directory_endpoint_t responseHandler; ///< Response handler.
	bool manyResponses; ///< Defines if request wants to get only one or as many as possilbe responses.
	bool onlyLocal; ///< Defines if request is limited to the local node or not.
}
xme_core_directory_requestItem_t;

/*
 * \typedef xme_core_directory_requestHandlerItem_t
 *
 * \brief   Request handler.
 */
typedef struct
{
	xme_core_directory_endpoint_t requestHandler; ///< Request handler.
	xme_core_topic_t requestTopic; ///< Request topic.
	xme_core_md_topicMetaDataHandle_t requestMetaDataFilter; ///< Meta data filter belonging to request.
	xme_core_directory_endpoint_t responseSender; ///< Response sender.
	xme_core_topic_t responseTopic; ///< Response topic.
	xme_core_md_topicMetaDataHandle_t responseMetaData; ///< Meta data belonging to response.
	bool onlyLocal; ///< Defines if response is limited to the local node or not.
}
xme_core_directory_requestHandlerItem_t;

/**
 * \typedef xme_core_directory_configStruct_t
 *
 * \brief  Directory configuration structure.
 */
#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
	XME_COMPONENT_CONFIG_STRUCT
	(
		xme_core_directory,
		// public
		xme_core_dataChannel_t baseDataChannel; // 1 in MSB means that the base data channel is a locally assigned data channel
		xme_core_dataChannel_t dataChannelMask;

		// private
		xme_core_dcc_subscriptionHandle_t announcementSubscription;

		XME_HAL_TABLE(xme_core_directory_dataChannelMapping_t, dataChannelMappings, XME_HAL_DEFINES_MAX_DIRECTORY_DATACHANNELMAPPING_ITEMS);

		XME_HAL_TABLE(xme_core_directory_node_t, nodes, XME_HAL_DEFINES_MAX_DIRECTORY_NODE_ITEMS);
		XME_HAL_TABLE(xme_core_directory_communicationLink_t, communicationLinks, XME_HAL_DEFINES_MAX_DIRECTORY_COMMUNICATION_LINKS_ITEMS);

		xme_core_rr_requestHandlerHandle_t newNodeRequestHandlerHandle;

		xme_core_dcc_subscriptionHandle_t localAnnouncementHandle;
		xme_core_dataChannel_t localAnnouncementDataChannel;

		xme_core_dcc_subscriptionHandle_t remoteAnnouncementHandle;
		xme_core_dataChannel_t remoteAnnouncementDataChannel;

		xme_core_dcc_publicationHandle_t remoteModifyRoutingTableHandle;
		XME_HAL_TABLE(xme_core_directory_dataChannelToNode_t, remoteModifyRoutingTableDataChannels, XME_HAL_DEFINES_MAX_DIRECTORY_REMOTE_MODIFY_ROUTING_TABLE_DATACHANNEL_ITEMS);

		xme_core_dcc_subscriptionHandle_t managementChannelsToEdgeNodeHandle;
		xme_core_dataChannel_t managementChannelsToEdgeNodeDataChannel;

		xme_core_dcc_subscriptionHandle_t managementChannelsToNewNodeHandle;
		xme_core_dataChannel_t managementChannelsToNewNodeDataChannel;

		XME_HAL_TABLE(xme_core_directory_link_t, links, XME_HAL_DEFINES_MAX_DIRECTORY_LINK_ITEMS);
		XME_HAL_TABLE(xme_core_directory_dataChannelTranslation_t, dataChannelTranslations, XME_HAL_DEFINES_MAX_DIRECTORY_DATACHANNEL_TRANSLATION_ITEMS);
		XME_HAL_TABLE(xme_core_directory_endpointLink_t, endpointLinks, XME_HAL_DEFINES_MAX_DIRECTORY_ENDPOINT_LINK_ITEMS);

		XME_HAL_TABLE(xme_core_directory_publicationItem_t, publications, XME_HAL_DEFINES_MAX_DIRECTORY_PUBLICATION_ITEMS);
		XME_HAL_TABLE(xme_core_directory_subscriptionItem_t, subscriptions, XME_HAL_DEFINES_MAX_DIRECTORY_SUBSCRIPTION_ITEMS);
		XME_HAL_TABLE(xme_core_directory_requestItem_t, requests, XME_HAL_DEFINES_MAX_DIRECTORY_REQUEST_ITEMS);
		XME_HAL_TABLE(xme_core_directory_requestHandlerItem_t, requestHandlers, XME_HAL_DEFINES_MAX_DIRECTORY_REQUEST_HANDLER_ITEMS);

		XME_HAL_TABLE(xme_core_directory_communicationRelation_t, communicationRelations, XME_HAL_DEFINES_MAX_DIRECTORY_REQUEST_HANDLER_ITEMS);
	);
#else // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER
	XME_COMPONENT_CONFIG_STRUCT
	(
		xme_core_directory,
		// public
		xme_core_dataChannel_t baseDataChannel; // 1 in MSB means that the base data channel is a locally assigned data channel
		xme_core_dataChannel_t dataChannelMask;

		// private
		xme_core_dcc_subscriptionHandle_t announcementSubscription;

		XME_HAL_TABLE(xme_core_directory_dataChannelMapping_t, dataChannelMappings, XME_HAL_DEFINES_MAX_DIRECTORY_DATACHANNELMAPPING_ITEMS);

		xme_core_dcc_subscriptionHandle_t localAnnouncementHandle;
		xme_core_dataChannel_t localAnnouncementDataChannel;

		xme_core_dcc_publicationHandle_t remoteAnnouncementHandle;
		xme_core_dataChannel_t remoteAnnouncementDataChannel;

		xme_core_dcc_subscriptionHandle_t remoteModifyRoutingTableHandle;
		xme_core_dataChannel_t remoteModifyRoutingTableDataChannel;

		xme_core_dcc_subscriptionHandle_t managementChannelsToEdgeNodeHandle;
		xme_core_dataChannel_t managementChannelsToEdgeNodeDataChannel;

		xme_core_dcc_subscriptionHandle_t managementChannelsToNewNodeHandle;
		xme_core_dataChannel_t managementChannelsToNewNodeDataChannel;

		XME_HAL_TABLE(xme_core_directory_publicationItem_t, publications, XME_HAL_DEFINES_MAX_DIRECTORY_PUBLICATION_ITEMS);
		XME_HAL_TABLE(xme_core_directory_subscriptionItem_t, subscriptions, XME_HAL_DEFINES_MAX_DIRECTORY_SUBSCRIPTION_ITEMS);
		XME_HAL_TABLE(xme_core_directory_requestItem_t, requests, XME_HAL_DEFINES_MAX_DIRECTORY_REQUEST_ITEMS);
		XME_HAL_TABLE(xme_core_directory_requestHandlerItem_t, requestHandlers, XME_HAL_DEFINES_MAX_DIRECTORY_REQUEST_HANDLER_ITEMS);

		XME_HAL_TABLE(xme_core_directory_communicationRelation_t, communicationRelations, XME_HAL_DEFINES_MAX_DIRECTORY_COMMUNICATION_RELATION_ITEMS);

		bool connectedToMasterDirectory;
	);
#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates a directory component.
 *         Exactly one component of this type must be present on every node.
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            initialized.
 *          - XME_CORE_STATUS_INVALID_CONFIGURATION if a component of this
 *            type has already been initialized. Exactly one component of this
 *            type must be present on every node.
 */
xme_core_status_t
xme_core_directory_create(xme_core_directory_configStruct_t* config);

/**
 * \brief  Activates a directory component.
 */
xme_core_status_t
xme_core_directory_activate(xme_core_directory_configStruct_t* config);

/**
 * \brief  Deactivates a directory component.
 */
void
xme_core_directory_deactivate(xme_core_directory_configStruct_t* config);

/**
 * \brief  Destroys a directory component.
 *         Exactly one component of this type must be present on every node.
 */
void
xme_core_directory_destroy(xme_core_directory_configStruct_t* config);

// TODO: Remove functions from public API into c file (ticket #656)

/**
 * \brief  Returns the data channel for a given topic with its meta data.
 *
 * \param  topic Topic for whicht to return the corresponding data channel.
 * \param  metaData Meta data associated with the topic.
 * \param  requestResponse States whether topic is belonging to a request
 *         response or not.
 *
 * \return Returns the data channel associated with the given topic and its
 *         meta data.
 */
xme_core_dataChannel_t
xme_core_directory_getDataChannel
(
	xme_core_topic_t topic,
	xme_core_md_topicMetaDataHandle_t metaData,
	bool requestResponse
);

#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
	/**
	 * \brief  Adds a node to the collection of available nodes.
	 *
	 * \param  node Node which shall be added.
	 * \param  remoteAnnouncementDataChannel Data channel which is used for receiving announcements of node.
	 * \param  remoteModifyRoutingTableDataChannel Data channel to modify routing table of node.
	 *
	 * \return Returns whether add of node has been successful or not.
	 */
	xme_core_status_t
	xme_core_directory_addNode
	(
		xme_core_node_nodeId_t node,
		xme_core_dataChannel_t remoteAnnouncementDataChannel,
		xme_core_dataChannel_t remoteModifyRoutingTableDataChannel
	);

	/**
	 * \brief  Updates the available neighbors of the given node.
	 *
	 * \param  node Node which neighors shall be updated.
	 * \param  neighbors List of current neigbors of node.
	 * \param  neighborsCount Count of given neighbors.
	 * \param  complete Defines if list of neighbors is complete or just an increment.
	 *
	 * \return Returns whether update of neighbors has been successful or not.
	 */
	xme_core_status_t
	xme_core_directory_updateNeigborhood
	(
		xme_core_node_nodeId_t node,
		xme_core_directory_neighbor_t neighbors[],
		uint16_t neighborsCount,
		bool complete
	);
#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER


/**
 * \brief  Registers a new publication from a software component and
 *         establishes routes to matching subscribtions.
 *
 *         This function can either be called from the local node or from a
 *         network component that forwards a publication from a different
 *         node.
 *
 * \param  publishingNode Global logical identifier of the node where the publication
 *         was issued.
 * \param  publishingComponent Local identifier of the software component on the node
 *         that issued the publication.
 * \param  publishingPort Local identifier of the software component port on the node
 *         that issued the publication.
 * \param  topic Published topic.
 * \param  metaData Meta data associated with the topic.
 * \param  onlyLocal Defines if publication shall be limited to the local node or not.
 *
 * \return Returns whether register of publication has been successful or not.
 */
xme_core_status_t
xme_core_directory_addPublication
(
	xme_core_node_nodeId_t publishingNode,
	xme_core_component_t publishingComponent,
	xme_core_component_port_t publishingPort,
	xme_core_topic_t topic,
	xme_core_md_topicMetaDataHandle_t metaData,
	bool onlyLocal
);

/**
 * \brief  Unregisters a publication of a software component and
 *         destroys established routes belonging to publication.
 *
 *         This function can either be called from the local node or from a
 *         network component that forwards a unregister publication from a
 *         different node.
 *
 * \param  publishingNode Global logical identifier of the node where the publication
 *         was issued.
 * \param  publishingComponent Local identifier of the software component on the node
 *         that issued the publication.
 * \param  publishingPort Local identifier of the software component port on the node
 *         that issued the publication.
 *
 * \return Returns whether unregister of publication has been successful or not.
 */
xme_core_status_t
xme_core_directory_removePublication
(
	xme_core_node_nodeId_t publishingNode,
	xme_core_component_t publishingComponent,
	xme_core_component_port_t publishingPort
);

/**
 * \brief  Registers a new subscription from a software component and
 *         establishes routes to matching publications.
 *
 *         This function can either be called from the local node or from a
 *         network component that forwards a subscription from a different
 *         node.
 *
 * \param  subscribingNode Global logical identifier of the node where the subscription
 *         was issued.
 * \param  subsribingComponent Local identifier of the software component on the node
 *         that issued the subscription.
 * \param  subscribingPort Local identifier of the software component port on the node
 *         that issued the subscription.
 * \param  topic Subsribed topic.
 * \param  metaData Meta data associated with the topic.
 * \param  onlyLocal Defines if subscription shall be limited to the local node or not.
 *
 * \return Returns whether register of subscription has been successful or not.
 */
xme_core_status_t
xme_core_directory_addSubscription
(
	xme_core_node_nodeId_t subscribingNode,
	xme_core_component_t subscribingComponent,
	xme_core_component_port_t subscribingPort,
	xme_core_topic_t topic,
	xme_core_md_topicMetaDataHandle_t metaData,
	bool onlyLocal
);

/**
 * \brief  Unegisters a subscription of a software component and
 *         destroys established routes belonging to subscription.
 *
 *         This function can either be called from the local node or from a
 *         network component that forwards a unregister subscription from a
 *         different node.
 *
 * \param  subscribingNode Global logical identifier of the node where the subscription
 *         was issued.
 * \param  subsribingComponent Local identifier of the software component on the node
 *         that issued the subscription.
 * \param  subscribingPort Local identifier of the software component port on the node
 *         that issued the subscription.
 *
 * \return Returns whether unregister of subscription has been successful or not.
 */
xme_core_status_t
xme_core_directory_removeSubscription
(
	xme_core_node_nodeId_t subscribingNode,
	xme_core_component_t subscribingComponent,
	xme_core_component_port_t subscribingPort
);

/**
 * \brief  Registers a new request handler from a software component and
 *         establishes routes to matching requests.
 *
 *         This function can either be called from the local node or from a
 *         network component that forwards a request handler announcement from
 *         a different node.
 *
 * \param  requestHandlingNode Global logical identifier of the node where the
 *         request handler was issued.
 * \param  requestHandlingComponent Local identifier of the software component
 *         on the node that issued the request handler.
 * \param  requestHandlingPort Local identifier of the software component port
 *         on the node that issued the request handler.
 * \param  responseSendingPort Local identifier of the software component port
 *         on the node that sends responses to the request.
 * \param  requestTopic Request topic.
 * \param  requestMetaDataFilter Meta data filter to apply to the request topic.
 * \param  responseTopic Response topic.
 * \param  responseMetaData Meta data associated with the response topic.
 * \param  onlyLocal Defines if request handler shall be limited to the local
 *         node or not.
 *
 * \return Returns whether register of request handler has been successful or not.
 */
xme_core_status_t
xme_core_directory_addRequestHandler
(
	xme_core_node_nodeId_t requestHandlingNode,
	xme_core_component_t requestHandlingComponent,
	xme_core_component_port_t requestHandlingPort,
	xme_core_component_port_t responseSendingPort,
	xme_core_topic_t requestTopic,
	xme_core_md_topicMetaDataHandle_t requestMetaDataFilter,
	xme_core_topic_t responseTopic,
	xme_core_md_topicMetaDataHandle_t responseMetaData,
	bool onlyLocal
);

/**
 * \brief  Unregisters a request handler of a software component and
 *         destroys established routes belonging to request handler.
 *
 *         This function can either be called from the local node or from a
 *         network component that forwards a unregister request handler
 *         announcement from a different node.
 *
 * \param  requestHandlingNode Global logical identifier of the node where the
 *         request handler was issued.
 * \param  requestHandlingComponent Local identifier of the software component
 *         on the node that issued the request handler.
 * \param  requestHandlingPort Local identifier of the software component port
 *         on the node that issued the request handler.
 *
 * \return Returns whether unregister of request handler has been successful or not.
 */xme_core_status_t
xme_core_directory_removeRequestHandler
(
	xme_core_node_nodeId_t requestHandlingNode,
	xme_core_component_t requestHandlingComponent,
	xme_core_component_port_t requestHandlingPort
);

/**
 * \brief  Registers a new request from a software component and
 *         establishes routes to matching request handlers.
 *
 *         This function can either be called from the local node or from a
 *         network component that forwards a request from a different node.
 *
 * \param  requestingNode Global logical identifier of the node where the
 *         request was issued.
 * \param  requestingComponent Local identifier of the software component
 *         on the node that issued the request.
 * \param  requestingPort Local identifier of the software component port
 *         on the node that issued the request.
 * \param  responseHandlingPort Local identifier of the software component port
 *         on the node that handles responses to the request.
 * \param  requestTopic Request topic.
 * \param  requestMetaData Meta data associated with the request topic.
 * \param  responseTopic Exepected response topic.
 * \param  responseMetaDataFilter Meta data filter to apply to response topic.
 * \param  manyResponses Defines if request wants to get only one or as many as possilbe responses.
 * \param  onlyLocal Defines if request shall be limited to the local node or not.
 *
 * \return Returns whether register of request has been successful or not.
 */
xme_core_status_t
xme_core_directory_addRequest
(
	xme_core_node_nodeId_t requestingNode,
	xme_core_component_t requestingComponent,
	xme_core_component_port_t requestingPort,
	xme_core_component_port_t responseHandlingPort,
	xme_core_topic_t requestTopic,
	xme_core_md_topicMetaDataHandle_t requestMetaData,
	xme_core_topic_t responseTopic,
	xme_core_md_topicMetaDataHandle_t responseMetaDataFilter,
	bool manyResponses,
	bool onlyLocal
);

/**
 * \brief  Registers a request of a software component and
 *         destroys established routes belonging to request.
 *
 *         This function can either be called from the local node or from a
 *         network component that forwards a unregister request from a different node.
 *
 * \param  requestingNode Global logical identifier of the node where the
 *         request was issued.
 * \param  requestingComponent Local identifier of the software component
 *         on the node that issued the request.
 * \param  requestingPort Local identifier of the software component port
 *         on the node that issued the request.
 *
 * \return Returns whether unregister of request has been successful or not.
 */
xme_core_status_t
xme_core_directory_removeRequest
(
	xme_core_node_nodeId_t requestingNode,
	xme_core_component_t requestingComponent,
	xme_core_component_port_t requestingPort
);

#endif // #ifndef XME_CORE_DIRECTORY_H

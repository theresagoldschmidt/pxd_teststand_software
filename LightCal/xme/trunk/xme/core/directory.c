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

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/directory.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "xme/core/broker.h"
#include "xme/core/component.h"
#include "xme/core/defines.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
	/*
	 * \typedef xme_core_directory_path_t
	 *
	 * \brief   Communication path.
	 */
	typedef XME_HAL_TABLE(xme_core_directory_communicationLink_t, xme_core_directory_path_t, XME_HAL_DEFINES_MAX_DIRECTORY_PATH_ITEMS);

	/*
	 * \typedef xme_core_directory_communicationPath_t
	 *
	 * \brief   Communication path.
	 */
	typedef XME_HAL_TABLE(xme_core_directory_link_t*, xme_core_directory_communicationPath_t, XME_HAL_DEFINES_MAX_DIRECTORY_PATH_ITEMS);
#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
static xme_core_directory_configStruct_t* xme_core_directory_config = NULL; ///< Directory configuration (singleton).

/******************************************************************************/
/***   Forward declarations                                                 ***/
/******************************************************************************/
// The Directory uses some internal functions from DCC and RR,
// which are forward-declared here.

// from dcc.c
xme_core_dcc_publicationHandle_t
_xme_core_dcc_publishTopicWithoutAnnouncement
(
	xme_core_topic_t topic,
	xme_core_md_topicMetaDataHandle_t topicMetaDataHandle,
	xme_core_dcc_demandCallback_t demandCallback
);

// from dcc.c
xme_core_status_t
_xme_core_dcc_unpublishTopicWithoutAnnouncement
(
	xme_core_dcc_publicationHandle_t publicationHandle,
	xme_core_component_t* publishingComponent,
	xme_core_component_port_t* publishingPort
);

// from dcc.c
xme_core_dcc_subscriptionHandle_t
_xme_core_dcc_subscribeTopicWithoutAnnouncement
(
	xme_core_topic_t topic,
	xme_core_md_topicMetaDataHandle_t topicFilter,
	xme_core_dcc_receiveTopicCallback_t callback,
	void* userData
);

// from rr.c
xme_core_rr_requestHandle_t
_xme_core_rr_publishRequestWithoutAnnouncement
(
	xme_core_topic_t requestTopic,
	xme_core_md_topicMetaDataHandle_t requestMetaDataHandle,
	xme_core_topic_t responseTopic,
	xme_core_md_topicMetaDataHandle_t responseMetaDataFilter,
	xme_core_rr_receiveResponseCallback_t receiveResponseCallback,
	void* userData
);

// from rr.c
xme_core_status_t
_xme_core_rr_unpublishRequestWithoutAnnouncement
(
	xme_core_rr_requestHandle_t requestHandle,
	xme_core_component_t* requestingComponent,
	xme_core_component_port_t* requestingPort
);

// from dcc.c
xme_core_status_t
_xme_core_dcc_unsubscribeTopicWithoutAnnouncement
(
	xme_core_dcc_subscriptionHandle_t subscriptionHandle,
	xme_core_component_t* subscribingComponent,
	xme_core_component_port_t* subscribingPort
);

// from rr.c
xme_core_rr_requestHandlerHandle_t
_xme_core_rr_publishRequestHandlerWithoutAnnouncement
(
	xme_core_topic_t requestTopic,
	xme_core_md_topicMetaDataHandle_t requestFilter,
	xme_core_topic_t responseTopic,
	xme_core_md_topicMetaDataHandle_t responseMetaData,
	xme_core_rr_receiveRequestCallback_t receiveRequestCallback,
	void* userData
);

// from rr.c
xme_core_status_t
_xme_core_rr_unpublishRequestHandlerWithoutAnnouncement
(
	xme_core_rr_requestHandlerHandle_t requestHandlerHandle,
	xme_core_component_t* requestHandlingComponent,
	xme_core_component_port_t* requestHandlingPort
);

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Callback function for receiving an announcement at the directory.
 *
 * \see    xme_core_dcc_receiveTopicCallback_t
 */
static
void
xme_core_directory_receiveAnnouncementCallback
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
);

/**
 * \brief  Callback function for receiving management channels to the edge node
 *         at the directory.
 *
 * \see    xme_core_dcc_receiveTopicCallback_t
 */
static
void
xme_core_directory_receiveManagementChannelsToEdgeNode
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
);

/**
 * \brief  Callback function for receiving management channels to new node
 *         at the directory.
 *
 * \see    xme_core_dcc_receiveTopicCallback_t
 */
static
void
xme_core_directory_receiveManagementChannelsToNewNode
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
);

#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
	/**
	 * \brief  Compares to pathes and returns their relation.
	 *
	 * \param  path1 First communication path.
	 * \param  path2 Second communication path.
	 *
	 * \return Returns the relation between the given pathes:
	 *          - -1 if path1 is better than path2
	            - 0 if path1 is equal to path2
	            - 1 if path1 is worse than path2
	 */	
	static
	int8_t
	xme_core_directory_cmp_pathes
	(
		xme_core_directory_path_t* path1,
		xme_core_directory_path_t* path2
	);

	/**
	 * \brief  Searches for a communicaton path between the given nodes.
	 *
	 * \param  source Source node of communication path.
	 * \param  sink Sink node of communication path.
	 * \param  path Buffer for storing found communication path
	 *
	 * \return Returns one of the following status codes:
	 *          - XME_CORE_STATUS_SUCCESS if a communication path has been found
	 *          - XME_CORE_STATUS_NOT_FOUND if no communication path could be found
	 */
	static
	xme_core_status_t
	xme_core_directory_findPath
	(
		xme_core_node_nodeId_t source,
		xme_core_node_nodeId_t sink,
		xme_core_directory_path_t* path
	);

	/**
	 * \brief  Establish data channel between the given communication partners
	 *         along the given communication path.
	 *
	 * \param  publication Publication which is the source of the communication path.
	 * \param  path Commucation path on which data channel shall be established.
	 * \param  subscription Subscription which is the sink of the communication path.
	 *
	 * \return Returns whether establishment of data channel has been successful or not.
	 */
	static
	xme_core_status_t
	xme_core_directory_establishDataChannel
	(
		xme_core_directory_publicationItem_t* publication,
		xme_core_directory_path_t* path,
		xme_core_directory_subscriptionItem_t* subscription
	);

	/**
	 * \brief  Establish request response data channels between the given
	 *         communication partners along the given communication path.
	 *
	 * \param  request Request which is the source/sink of the communication paths.
	 * \param  path Commucation path on which data channel shall be established.
	 * \param  requestHandler Request handler which is the sink/source of the
	 *         communication paths.
	 *
	 * \return Returns whether establishment of data channels has been successful or not.
	 */
	static
	xme_core_status_t
	xme_core_directory_establishRRDataChannels
	(
		xme_core_directory_requestItem_t* request,
		xme_core_directory_path_t* path,
		xme_core_directory_requestHandlerItem_t* requestHandler
	);

	/**
	 * \brief  Constructs the remote announcemant and remote modify routing table data channel between the given communication partners.
	 *
	 * \param  directory Node hosting the directory.
	 * \param  node Node which shall be connected to the directory.
	 * \param  remoteModifyRoutingTableDataChannel Remote modify routing table data channel which shall be constructed.
	 *
	 * \return Returns whether construction of data channels has been successful or not.
	 */
	static
	xme_core_status_t
	xme_core_directory_constructManagementChannels
	(
		xme_core_node_nodeId_t directory,
		xme_core_node_nodeId_t node,
		xme_core_dataChannel_t remoteModifyRoutingTableDataChannel
	);

	/**
	 * \brief  Returns the remote modify routing table data channel for the given node.
	 *
	 * \param  nodeId Node identifier of node for which to return remote modify routing table data channel.
	 *
	 * \return Remote modify routing table data channel for the given node.
	 */
	static
	xme_core_dataChannel_t
	xme_core_directory_getRemoteModifyRoutingTableDataChannelForNode
	(
		xme_core_node_nodeId_t nodeId
	);
#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER

/**
 * \brief  Tries to construct a data channel between the given communication partners.
 *
 * \param  publication Publication which is the source of the data channel.
 * \param  subscription Subscription which is the sink of the data channel.
 *
 * \return Returns whether construction of data channel has been successful or not.
 */
static
xme_core_status_t
xme_core_directory_constructDataChannel
(
	xme_core_directory_publicationItem_t* publication,
	xme_core_directory_subscriptionItem_t* subscription
);

#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
	/**
	 * \brief  Returns the communication path between source and sink.
	 *
	 * \param  source Source node of communication path.
	 * \param  sink Sink node of communication path.
	 * \param  dataChannel Data channel used on communication path between source and sink.
	 * \param  path Communication path between source and sink.
	 *
	 * \return Returns whether finding the communication path has been successful or not.
	 */
	static
	xme_core_status_t
	xme_core_directory_getCommunicationPath
	(
		xme_core_node_nodeId_t source,
		xme_core_node_nodeId_t sink,
		xme_core_dataChannel_t dataChannel,
		xme_core_directory_communicationPath_t* path
	);

	/**
	 * \brief  Deconstructs data channel between the given communication partners
	 *         if existing.
	 *
	 * \param  source Source of the data channel.
	 * \param  sink Sink of the data channel.
	 * \param  dataChannel Data channel used for communication between source and sink.
	 *
	 * \return Returns whether deconstruction of data channel has been successful or not.
	 */
	static
	xme_core_status_t
	xme_core_directory_deconstructDataChannel
	(
		xme_core_directory_endpoint_t* source,
		xme_core_directory_endpoint_t* sink,
		xme_core_dataChannel_t dataChannel
	);
#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER

/**
 * \brief  Checks if request response data channels exist between the given
 *         communication partners.
 *
 * \param  request Request which is the source/sink of the data channels.
 * \param  requestHandler Request handler which is the sink/source of the
 *         data channels.
 *
 * \return Returns whether request response data channes exist between the
 *         given communication partners or not.
 */
static
bool
xme_core_directory_existsRRDataChannels
(
	xme_core_directory_requestItem_t* request,
	xme_core_directory_requestHandlerItem_t* requestHandler
);

/**
 * \brief  Tries to construct request response data channels between the
 *         given communication partners.
 *
 * \param  request Request which is the source/sink of the data channels.
 * \param  requestHandler Request handler which is the sink/source of the
 *         data channels.
 *
 * \return Returns whether construction of request response data channels has
 *         been successful or not.
 */
static
xme_core_status_t
xme_core_directory_constructRRDataChannels
(
	xme_core_directory_requestItem_t* request,
	xme_core_directory_requestHandlerItem_t* requestHandler
);

#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
	/**
	 * \brief  Adds new element to endpoint links.
	 *
	 * \param  node Node where endpoint resists.
	 * \param  component Component to which endpoint belongs.
	 * \param  port Port representing endpoint.
	 * \param  type Type of endpoint.
	 * \param  dataChannel Data channel to which endpoint is connected.
	 *
	 * \return Returns whether adding of new endpoint link has been successful or not.
	 */
	static
	xme_core_status_t
	xme_core_directory_addEndpointLink
	(
		xme_core_directory_endpoint_t* endpoint,
		xme_core_directory_endpointType_t type,
		xme_core_dataChannel_t dataChannel
	);

	/**
	 * \brief Creates the in- and outbound routes for the given link.
	 *
	 * \param  pathElement Link for which the corresponding in- and outbound routes shall be created.
	 * \param  dataChannel Data channel which is used for the route.
	 *
	 * \return Returns whether the creation has been successful or not.
	 */
	static
	xme_core_status_t
	xme_core_directory_createInAndOutboundRoutes
	(
		xme_core_directory_communicationLink_t* pathElement,
		xme_core_dataChannel_t dataChannel
	);

	/**
	 * \brief  Adds new element to links.
	 *
	 * \param  sourceNode Source node of new link.
	 * \param  sourceInterface Source interface of new link.
	 * \param  sinkNode Sink node of new link.
	 * \param  sinkInterface Sink interface of new link.
	 * \param  dataChannel Data channel to which link belongs.
	 *
	 * \return Returns whether adding of new link has been successful or not.
	 */
	static
	xme_core_status_t
	xme_core_directory_addLink
	(
		xme_core_node_nodeId_t sourceNode,
		xme_core_interface_interfaceId_t sourceInterface,
		xme_core_node_nodeId_t sinkNode,
		xme_core_interface_interfaceId_t sinkInterface,
		xme_core_dataChannel_t dataChannel
	);

	/**
	 * \brief  Creates new link.
	 *
	 * \param  pathElement Information of link to create.
	 * \param  dataChannel Data channel to which link belongs.
	 *
	 * \return Returns whether creation of link has been successful or not.
	 */
	static
	xme_core_status_t
	xme_core_directory_createLink
	(
		xme_core_directory_communicationLink_t* pathElement,
		xme_core_dataChannel_t dataChannel
	);

	/**
	 * \brief  Callback function for indication of new node.
	 *
	 * \see    xme_core_rr_receiveRequestCallback_t
	 */
	static
	xme_core_rr_responseStatus_t
	xme_core_directory_receiveNewNodeRequest
	(
		xme_core_topic_t requestTopic,
		void* requestData,
		xme_core_rr_responseInstanceHandle_t responseInstanceHandle,
		void* responseData,
		uint16_t* responseSize,
		xme_hal_time_interval_t responseTimeoutMs,
		void* userData
	);
#else // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER
	/**
	 * \brief  Establishes the routes to the master directory. Afterwards the received
	           announcements are transferred to the master directory.
	 *
	 * \param  interfaceId Identifier of the interface which is used to communicate
	 *         with the new node.
	 * \param  remoteAnnouncementDataChannel DataChannel, which can be used to send 
	           announcements to master directory.
	 * \param  remoteModifyRoutingTableDataChannel DataChannel, which is used to receive
	           modify routing table command from master directory.
	 */
	static
	xme_core_status_t
	xme_core_directory_connectToMasterDirectory
	(
		xme_core_interface_interfaceId_t interfaceId,
		xme_core_dataChannel_t remoteAnnouncementDataChannel,
		xme_core_dataChannel_t remoteModifyRoutingTableDataChannel
	);

	/**
	 * \brief  Modifies the routing table of the broker according to the received
	 *         modification command.
	 *
	 * \see    xme_core_dcc_receiveTopicCallback_t
	 */
	static
	void
	xme_core_directory_receiveRemoteModifyRoutingTableCallback
	(
		xme_hal_sharedPtr_t dataHandle,
		void* userData
	);
#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER

/**
 * \brief  Creates new communication relation.
 *
 * \param  source Source endpoint of communicaiton relation.
 * \param  dataChannel Data channel used for communication.
 * \param  sink Sink endpoint of communication relation.
 *
 * \return Returns whether creation of communication relation has been
 *         successful or not.
 */
static
xme_core_status_t
xme_core_directory_addCommunicationRelation
(
	xme_core_directory_endpoint_t* source,
	xme_core_dataChannel_t dataChannel,
	xme_core_directory_endpoint_t* sink
);

/**
 * \brief  Destroyes a communication relation.
 *
 * \param  source Source endpoint of communicaiton relation.
 * \param  sink Sink endpoint of communication relation.
 * \param  dataChannel Data channel used for communication.
 *
 * \return Returns whether destruction of communication relation has been
 *         successful or not.
 */
static
xme_core_status_t
xme_core_directory_removeCommunicationRelation
(
	xme_core_directory_endpoint_t* source,
	xme_core_directory_endpoint_t* sink,
	xme_core_dataChannel_t* dataChannel
);

/******************************************************************************/
/***   Implementations                                                      ***/
/******************************************************************************/
xme_core_status_t
xme_core_directory_create(xme_core_directory_configStruct_t* config)
{
	XME_CHECK(NULL == xme_core_directory_config, XME_CORE_STATUS_INTERNAL_ERROR);
	xme_core_directory_config = config;

	XME_ASSERT(XME_ASSERT_NO_SIDE_EFFECTS(XME_CORE_COMPONENT_DIRECTORY_COMPONENT_ID == xme_core_resourceManager_getCurrentComponentId()));

	// Initialize configuration structure
	config->announcementSubscription = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;
	XME_HAL_TABLE_INIT(config->dataChannelMappings);
	config->localAnnouncementDataChannel = XME_CORE_DATACHANNEL_INVALID_DATACHANNEL;
	config->localAnnouncementHandle = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;
	config->remoteAnnouncementDataChannel = XME_CORE_DATACHANNEL_INVALID_DATACHANNEL;
	#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
		config->remoteAnnouncementHandle = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;
		XME_HAL_TABLE_INIT(config->remoteModifyRoutingTableDataChannels);
		config->remoteModifyRoutingTableHandle = XME_CORE_DCC_INVALID_PUBLICATION_HANDLE;
	#else // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER
		config->remoteAnnouncementHandle = XME_CORE_DCC_INVALID_PUBLICATION_HANDLE;
		config->remoteModifyRoutingTableDataChannel = XME_CORE_DATACHANNEL_INVALID_DATACHANNEL;
		config->remoteModifyRoutingTableHandle = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;
	#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER
	config->managementChannelsToEdgeNodeDataChannel= XME_CORE_DATACHANNEL_INVALID_DATACHANNEL;
	config->managementChannelsToEdgeNodeHandle = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;
	config->managementChannelsToNewNodeDataChannel= XME_CORE_DATACHANNEL_INVALID_DATACHANNEL;
	config->managementChannelsToNewNodeHandle = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;

	#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
		XME_HAL_TABLE_INIT(config->nodes);
		XME_HAL_TABLE_INIT(config->communicationLinks);

		config->newNodeRequestHandlerHandle = XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE;

		XME_HAL_TABLE_INIT(config->links);
		XME_HAL_TABLE_INIT(config->dataChannelTranslations);
		XME_HAL_TABLE_INIT(config->endpointLinks);
	#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER
	XME_HAL_TABLE_INIT(config->publications);
	XME_HAL_TABLE_INIT(config->subscriptions);
	XME_HAL_TABLE_INIT(config->requests);
	XME_HAL_TABLE_INIT(config->requestHandlers);

	// Create system internal data channel
	config->localAnnouncementDataChannel = xme_core_directory_getDataChannel
	(
		XME_CORE_TOPIC_LOCAL_ANNOUNCEMENT,
		XME_CORE_MD_EMPTY_META_DATA,
		false
	);
	XME_ASSERT(XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != config->localAnnouncementDataChannel);
	// TODO: Error handling! See ticket #721
	config->localAnnouncementHandle = _xme_core_dcc_subscribeTopicWithoutAnnouncement(XME_CORE_TOPIC_LOCAL_ANNOUNCEMENT, XME_CORE_MD_EMPTY_META_DATA, xme_core_directory_receiveAnnouncementCallback, NULL);
	xme_core_routingTable_addLocalDestinationRoute(config->localAnnouncementDataChannel, xme_core_resourceManager_getCurrentComponentId(), xme_core_resourceManager_getSubscriptionPort(config->localAnnouncementHandle));

	#ifdef XME_CORE_DIRECTORY_TYPE_LOCAL
		config->connectedToMasterDirectory = false;

		// TODO: Error handling! See ticket #721
		config->remoteAnnouncementHandle = _xme_core_dcc_publishTopicWithoutAnnouncement(XME_CORE_TOPIC_REMOTE_ANNOUNCEMENT, XME_CORE_MD_EMPTY_META_DATA, NULL);

		// TODO: Error handling! See ticket #721
		config->remoteModifyRoutingTableHandle = _xme_core_dcc_subscribeTopicWithoutAnnouncement(XME_CORE_TOPIC_REMOTE_MODIFY_ROUTING_TABLE, XME_CORE_MD_EMPTY_META_DATA, xme_core_directory_receiveRemoteModifyRoutingTableCallback, NULL);
	#else
		{
			xme_core_resourceManager_requestHandlerItem_t* item;

			// TODO: Error handling! See ticket #721
			config->newNodeRequestHandlerHandle = _xme_core_rr_publishRequestHandlerWithoutAnnouncement
			(
				XME_CORE_TOPIC_NEW_NODE_REQUEST,
				XME_CORE_MD_EMPTY_META_DATA,
				XME_CORE_TOPIC_NEW_NODE_RESPONSE,
				XME_CORE_MD_EMPTY_META_DATA,
				xme_core_directory_receiveNewNodeRequest,
				NULL
			);

			item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localRequestHandlers, config->newNodeRequestHandlerHandle);
			XME_ASSERT(NULL != item);

			xme_core_directory_addRequestHandler
			(
				XME_CORE_NODE_LOCAL_NODE_ID,
				xme_core_resourceManager_getCurrentComponentId(),
				item->requestHandlingPort,
				item->responseSendingPort,
				item->requestTopic,
				item->requestMetaDataFilter,
				item->responseTopic,
				item->responseStaticMetaData,
				true
			);
		}

		// TODO: Error handling! See ticket #721
		config->remoteAnnouncementDataChannel = xme_core_directory_getDataChannel
		(
			XME_CORE_TOPIC_REMOTE_ANNOUNCEMENT,
			XME_CORE_MD_EMPTY_META_DATA,
			false
		);
		XME_ASSERT(XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != config->remoteAnnouncementDataChannel);
		// TODO: Error handling! See ticket #721
		config->remoteAnnouncementHandle = _xme_core_dcc_subscribeTopicWithoutAnnouncement(XME_CORE_TOPIC_REMOTE_ANNOUNCEMENT, XME_CORE_MD_EMPTY_META_DATA, xme_core_directory_receiveAnnouncementCallback, NULL);
		xme_core_routingTable_addLocalDestinationRoute(config->remoteAnnouncementDataChannel, xme_core_resourceManager_getCurrentComponentId(), xme_core_resourceManager_getSubscriptionPort(config->remoteAnnouncementHandle));

		// TODO: Error handling! See ticket #721
		config->remoteModifyRoutingTableHandle = _xme_core_dcc_publishTopicWithoutAnnouncement(XME_CORE_TOPIC_REMOTE_MODIFY_ROUTING_TABLE, XME_CORE_MD_EMPTY_META_DATA, NULL);
	#endif // #ifdef XME_CORE_DIRECTORY_TYPE_LOCAL

	// Create system internal data channel
	config->managementChannelsToEdgeNodeDataChannel = xme_core_directory_getDataChannel
	(
		XME_CORE_TOPIC_MANAGEMENT_CHANNELS_TO_EDGE_NODE,
		XME_CORE_MD_EMPTY_META_DATA,
		false
	);
	XME_ASSERT(XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != config->managementChannelsToEdgeNodeDataChannel);
	{
		xme_core_resourceManager_subscriptionItem_t* item;

		// TODO: Error handling! See ticket #721
		config->managementChannelsToEdgeNodeHandle = _xme_core_dcc_subscribeTopicWithoutAnnouncement(XME_CORE_TOPIC_MANAGEMENT_CHANNELS_TO_EDGE_NODE, XME_CORE_MD_EMPTY_META_DATA, xme_core_directory_receiveManagementChannelsToEdgeNode, NULL);

		item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localSubscriptions, config->managementChannelsToEdgeNodeHandle);
		XME_ASSERT(NULL != item);

		xme_core_directory_addSubscription
		(
			XME_CORE_NODE_LOCAL_NODE_ID,
			xme_core_resourceManager_getCurrentComponentId(),
			item->subscribingPort,
			item->subscribedTopic,
			item->metaDataFilterHandle,
			true
		);
	}

	// Create system internal data channel
	config->managementChannelsToNewNodeDataChannel = xme_core_directory_getDataChannel
	(
		XME_CORE_TOPIC_MANAGEMENT_CHANNELS_TO_NEW_NODE,
		XME_CORE_MD_EMPTY_META_DATA,
		false
	);
	XME_ASSERT(XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != config->managementChannelsToNewNodeDataChannel);
	{
		xme_core_resourceManager_subscriptionItem_t* item;

		// TODO: Error handling! See ticket #721
		config->managementChannelsToNewNodeHandle = _xme_core_dcc_subscribeTopicWithoutAnnouncement(XME_CORE_TOPIC_MANAGEMENT_CHANNELS_TO_NEW_NODE, XME_CORE_MD_EMPTY_META_DATA, xme_core_directory_receiveManagementChannelsToNewNode, NULL);

		item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localSubscriptions, config->managementChannelsToNewNodeHandle);
		XME_ASSERT(NULL != item);

		xme_core_directory_addSubscription
		(
			XME_CORE_NODE_LOCAL_NODE_ID,
			xme_core_resourceManager_getCurrentComponentId(),
			item->subscribingPort,
			item->subscribedTopic,
			item->metaDataFilterHandle,
			true
		);
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_directory_activate(xme_core_directory_configStruct_t* config)
{
	// Nothing to do
	return XME_CORE_STATUS_SUCCESS;
}

void
xme_core_directory_deactivate(xme_core_directory_configStruct_t* config)
{
	// Nothing to do
}

void
xme_core_directory_destroy(xme_core_directory_configStruct_t* config)
{
	xme_core_directory_removeSubscription(xme_core_nodeManager_getNodeId(), xme_core_resourceManager_getCurrentComponentId(), xme_core_resourceManager_getSubscriptionPort(config->managementChannelsToNewNodeHandle));
	_xme_core_dcc_unsubscribeTopicWithoutAnnouncement(config->managementChannelsToNewNodeHandle, NULL, NULL);
	config->managementChannelsToNewNodeHandle = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;

	xme_core_directory_removeSubscription(xme_core_nodeManager_getNodeId(), xme_core_resourceManager_getCurrentComponentId(), xme_core_resourceManager_getSubscriptionPort(config->managementChannelsToEdgeNodeHandle));
	_xme_core_dcc_unsubscribeTopicWithoutAnnouncement(config->managementChannelsToEdgeNodeHandle, NULL, NULL);
	config->managementChannelsToEdgeNodeHandle = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;

	#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
		XME_HAL_TABLE_ITERATE
		(
			config->remoteModifyRoutingTableDataChannels,
			xme_hal_table_rowHandle_t, handle,
			xme_core_directory_dataChannelToNode_t, dataChannelToNode,
			{
				xme_core_routingTable_removeLocalSourceRoute(dataChannelToNode->dataChannel);
			}
		);
		XME_HAL_TABLE_FINI(config->remoteModifyRoutingTableDataChannels);
		_xme_core_dcc_unpublishTopicWithoutAnnouncement(config->remoteModifyRoutingTableHandle, NULL, NULL);
		config->remoteModifyRoutingTableHandle = XME_CORE_DCC_INVALID_PUBLICATION_HANDLE;

		xme_core_routingTable_removeLocalDestinationRoute(config->remoteAnnouncementDataChannel, xme_core_resourceManager_getCurrentComponentId(), xme_core_resourceManager_getSubscriptionPort(config->remoteAnnouncementHandle));
		_xme_core_dcc_unsubscribeTopicWithoutAnnouncement(config->remoteAnnouncementHandle, NULL, NULL);
		config->remoteAnnouncementHandle = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;

		xme_core_directory_removeRequestHandler(xme_core_nodeManager_getNodeId(), xme_core_resourceManager_getCurrentComponentId(), xme_core_resourceManager_getRequestHandlerPort(config->newNodeRequestHandlerHandle));
		_xme_core_rr_unpublishRequestHandlerWithoutAnnouncement(config->newNodeRequestHandlerHandle, NULL, NULL);
		config->newNodeRequestHandlerHandle = XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE;
	#else //#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
		if (XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != config->remoteModifyRoutingTableDataChannel)
		{
			xme_core_routingTable_removeLocalDestinationRoute(config->remoteModifyRoutingTableDataChannel, xme_core_resourceManager_getCurrentComponentId(), xme_core_resourceManager_getSubscriptionPort(config->remoteModifyRoutingTableHandle));
		}
		_xme_core_dcc_unsubscribeTopicWithoutAnnouncement(config->remoteModifyRoutingTableHandle, NULL, NULL);
		config->remoteModifyRoutingTableHandle = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;

		if (XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != config->remoteAnnouncementDataChannel)
		{
			xme_core_routingTable_removeLocalSourceRoute(config->remoteAnnouncementDataChannel);
		}
		_xme_core_dcc_unpublishTopicWithoutAnnouncement(config->remoteAnnouncementHandle, NULL, NULL);
		config->remoteAnnouncementHandle = XME_CORE_DCC_INVALID_PUBLICATION_HANDLE;
	#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER

	xme_core_routingTable_removeLocalDestinationRoute(config->localAnnouncementDataChannel, xme_core_resourceManager_getCurrentComponentId(), xme_core_resourceManager_getSubscriptionPort(config->localAnnouncementHandle));
	_xme_core_dcc_unsubscribeTopicWithoutAnnouncement(config->localAnnouncementHandle, NULL, NULL);
	config->localAnnouncementHandle = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;

	// Finalize configuration structure
	XME_HAL_TABLE_FINI(config->requestHandlers);
	XME_HAL_TABLE_FINI(config->requests);
	XME_HAL_TABLE_FINI(config->subscriptions);
	XME_HAL_TABLE_FINI(config->publications);
	#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
		XME_HAL_TABLE_FINI(config->endpointLinks);
		XME_HAL_TABLE_FINI(config->dataChannelTranslations);
		XME_HAL_TABLE_FINI(config->links);
		XME_HAL_TABLE_FINI(config->communicationLinks);
		XME_HAL_TABLE_FINI(config->nodes);
	#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER
	XME_HAL_TABLE_FINI(config->dataChannelMappings);
	config->announcementSubscription = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;
}


#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
	xme_core_status_t
	xme_core_directory_addNode(
		xme_core_node_nodeId_t node,
		xme_core_dataChannel_t remoteAnnouncementDataChannel,
		xme_core_dataChannel_t remoteModifyRoutingTableDataChannel
	)
	{
		xme_hal_table_rowHandle_t handle;
		xme_core_directory_node_t* item;

		// Parameter check
		XME_ASSERT(XME_CORE_NODE_INVALID_NODE_ID != node);

		//Check whether node has already been added
		XME_HAL_TABLE_ITERATE
		(
			xme_core_directory_config->nodes,
			xme_hal_table_rowHandle_t, handle,
			xme_core_directory_node_t, nodeItem,
			{
				XME_CHECK(nodeItem->node != node, XME_CORE_STATUS_ALREADY_EXIST);
			}
		);

		// Add the node to the list of nodes
		handle = XME_HAL_TABLE_ADD_ITEM(xme_core_directory_config->nodes);
		XME_CHECK(0 != handle, XME_CORE_STATUS_OUT_OF_RESOURCES);

		item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_directory_config->nodes, handle);
		XME_ASSERT(NULL != item);

		// TODO: This is not RR, although the last parameter in xme_core_directory_getDataChannel() is set to true! See ticket #732
		// Maybe rename the last parameter to "bool forceUniqueDataChannel" or similar?
		item->node = node;
		item->remoteAnnouncementDataChannel = remoteAnnouncementDataChannel;
		item->remoteModifyRoutingTableDataChannel = remoteModifyRoutingTableDataChannel;
	
		return XME_CORE_STATUS_SUCCESS;
	}

	xme_core_status_t
	xme_core_directory_updateNeigborhood
	(
		xme_core_node_nodeId_t node,
		xme_core_directory_neighbor_t neighbors[],
		uint16_t neighborsCount,
		bool complete
	)
	{
		xme_hal_table_rowHandle_t handle;
		xme_core_directory_communicationLink_t* item;
		uint16_t i;
		bool networkChanged;

		// Parameter check
		XME_ASSERT(XME_CORE_NODE_INVALID_NODE_ID != node);
		for (i = 0; i < neighborsCount; ++i)
		{
			XME_ASSERT(XME_CORE_INTERFACEMANAGER_INVALID_INTERFACE_ID != neighbors[i].sendInterface);
			XME_ASSERT(XME_CORE_NODE_INVALID_NODE_ID != neighbors[i].node);
			XME_ASSERT(XME_CORE_INTERFACEMANAGER_INVALID_INTERFACE_ID != neighbors[i].receiveInterface);
		}

		networkChanged = false;

		// Add new communication links
		for (i = 0; i < neighborsCount; ++i)
		{
			bool exists = false;

			XME_HAL_TABLE_ITERATE
			(
				xme_core_directory_config->communicationLinks,
				xme_hal_table_rowHandle_t, handle,
				xme_core_directory_communicationLink_t, communicationLink,
				{
					if (communicationLink->node1 == node && communicationLink->interfaceNode1 == neighbors[i].sendInterface && communicationLink->node2 == neighbors[i].node && communicationLink->interfaceNode2 == neighbors[i].receiveInterface) {
						exists = true;
						break;
					}
				}
			);

			// Check if communication link is new
			if (!exists) {
				// Add the communication link to the list of communication links
				handle = XME_HAL_TABLE_ADD_ITEM(xme_core_directory_config->communicationLinks);
				XME_CHECK(0 != handle, XME_CORE_STATUS_OUT_OF_RESOURCES);

				item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_directory_config->communicationLinks, handle);
				XME_ASSERT(NULL != item);

				item->node1 = node;
				item->interfaceNode1 = neighbors[i].sendInterface;
				item->node2 = neighbors[i].node;
				item->interfaceNode2 = neighbors[i].receiveInterface;

				networkChanged = true;
			}
		}

		if (complete)
		{
			// Delete missing communication links
			XME_HAL_TABLE_ITERATE
			(
				xme_core_directory_config->communicationLinks,
				xme_hal_table_rowHandle_t, handle,
				xme_core_directory_communicationLink_t, communicationLink,
				{
					if (communicationLink->node1 == node) {
						bool exists = false;

						for (i = 0; i < neighborsCount; ++i)
						{
							if (communicationLink->interfaceNode1 == neighbors[i].sendInterface && communicationLink->node2 == neighbors[i].node && communicationLink->interfaceNode2 == neighbors[i].receiveInterface) {
								exists = true;
								break;
							}
						}

						// Check if communication no longer exists
						if (!exists) {
							// Remove the communication link from the list of communication links
							XME_HAL_TABLE_REMOVE_ITEM(xme_core_directory_config->communicationLinks, handle);

							networkChanged = true;
						}
					}
				}
			);
		}

		// Check if communication network has changed
		if (networkChanged) {
			// TODO update data routes. See ticket #733
		}

		return XME_CORE_STATUS_SUCCESS;
	}
#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER


xme_core_dataChannel_t
xme_core_directory_getDataChannel
(
	xme_core_topic_t topic,
	xme_core_md_topicMetaDataHandle_t metaData,
	bool requestResponse
)
{
	xme_core_dataChannel_t dataChannel;
	xme_hal_table_rowHandle_t handle;
	xme_core_directory_dataChannelMapping_t* mapping;

	// Parameter check
	XME_ASSERT_RVAL(XME_CORE_TOPIC_INVALID_TOPIC != topic, XME_CORE_DATACHANNEL_INVALID_DATACHANNEL);
	// TODO: Check meta data. See ticket #646

	dataChannel = XME_CORE_DATACHANNEL_INVALID_DATACHANNEL;

	// Check if topic is not belonging to a request response
	if (!requestResponse)
	{
		// Search if topic and meta data combination has already a data channel assigned
		XME_HAL_TABLE_ITERATE
		(
			xme_core_directory_config->dataChannelMappings,
			xme_hal_table_rowHandle_t, handle,
			xme_core_directory_dataChannelMapping_t, mapping,
			{
				if (mapping->topic == topic && 0 == xme_core_md_compareMetData(mapping->metaData, metaData))
				{
					dataChannel = mapping->dataChannel;
					break;
				}
			}
		);
	}

	// Check if new data channel has to be assinged
	if (XME_CORE_DATACHANNEL_INVALID_DATACHANNEL == dataChannel)
	{
		// Add topic and meta data combination to mappings
		handle = XME_HAL_TABLE_ADD_ITEM(xme_core_directory_config->dataChannelMappings);
		XME_CHECK(0 != handle, XME_CORE_DATACHANNEL_INVALID_DATACHANNEL);

		mapping = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_directory_config->dataChannelMappings, handle);
		XME_CHECK(NULL != mapping, XME_CORE_DATACHANNEL_INVALID_DATACHANNEL);

		mapping->topic = topic;
		mapping->metaData = metaData;
		// Assign data channel
		mapping->dataChannel = (xme_core_dataChannel_t)(xme_core_directory_config->baseDataChannel + handle); // TODO: ensure that data channel doesn't overlap between local and master directory. See ticket #737

		dataChannel = mapping->dataChannel;
	}

	return dataChannel;
}

static
xme_core_status_t
xme_core_directory_addCommunicationRelation
(
	xme_core_directory_endpoint_t* source,
	xme_core_dataChannel_t dataChannel,
	xme_core_directory_endpoint_t* sink
)
{
	xme_hal_table_rowHandle_t handle;
	xme_core_directory_communicationRelation_t* communication;

	// Add the communication to the list of commication relations
	handle = XME_HAL_TABLE_ADD_ITEM(xme_core_directory_config->communicationRelations);
	XME_CHECK(0 != handle, XME_CORE_STATUS_OUT_OF_RESOURCES);

	communication = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_directory_config->communicationRelations, handle);
	XME_ASSERT(NULL != communication);

	communication->source = source;
	communication->localDataChannel = dataChannel;
	communication->globalDataChannel = XME_CORE_DATACHANNEL_INVALID_DATACHANNEL;
	communication->sink = sink;

	return XME_CORE_STATUS_SUCCESS;
}

static
xme_core_status_t
xme_core_directory_removeCommunicationRelation
(
	xme_core_directory_endpoint_t* source,
	xme_core_directory_endpoint_t* sink,
	xme_core_dataChannel_t* dataChannel
)
{
	xme_hal_table_rowHandle_t handle;
	xme_core_directory_communicationRelation_t* communication;

	// Find the corresponding row in the table
	handle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
	XME_HAL_TABLE_GET_NEXT
	(
		xme_core_directory_config->communicationRelations,
		xme_hal_table_rowHandle_t, handle,
		xme_core_directory_communicationRelation_t, communication,
		communication->source == source && communication->sink == sink
	);
	XME_ASSERT(XME_HAL_TABLE_INVALID_ROW_HANDLE != handle);
	XME_ASSERT(NULL != communication);

	if (NULL != dataChannel)
	{
		if (XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != communication->globalDataChannel)
		{
			*dataChannel = communication->globalDataChannel;
		}
		else
		{
			*dataChannel = communication->localDataChannel;
		}
	}

	//Remove the communication relation from directory
	XME_CHECK(XME_CORE_STATUS_SUCCESS == XME_HAL_TABLE_REMOVE_ITEM(xme_core_directory_config->communicationRelations, handle), XME_CORE_STATUS_INTERNAL_ERROR);
	
	return XME_CORE_STATUS_SUCCESS;
}

#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
	static
	int8_t
	xme_core_directory_cmp_pathes
	(
		xme_core_directory_path_t* path1,
		xme_core_directory_path_t* path2
	)
	{
		// TODO select better path. See ticket #734
		if (XME_HAL_TABLE_ITEM_COUNT(*path1) < XME_HAL_TABLE_ITEM_COUNT(*path2))
		{
			return -1;
		}
		else if (XME_HAL_TABLE_ITEM_COUNT(*path1) == XME_HAL_TABLE_ITEM_COUNT(*path2))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}

	static
	xme_core_status_t
	xme_core_directory_findPath
	(
		xme_core_node_nodeId_t source,
		xme_core_node_nodeId_t sink,
		xme_core_directory_path_t* path
	)
	{
		xme_hal_table_rowHandle_t handle;
		xme_core_directory_communicationLink_t* communicationLink;
		xme_core_status_t status;
		xme_hal_table_rowHandle_t pathHandle;
		xme_core_directory_communicationLink_t* pathElement;
		xme_core_directory_path_t path1;
		xme_core_directory_path_t path2;
		xme_core_directory_path_t* bestPath;
		xme_core_directory_path_t* currentPath;

		// Parameter check
		XME_ASSERT(NULL != path);

		// Check if source and sink are the same
		if (source == sink)
		{
			return XME_CORE_STATUS_SUCCESS;
		}

		XME_HAL_TABLE_INIT(path1);
		XME_HAL_TABLE_INIT(path2);
		currentPath = &path1;
		bestPath = &path2;

		XME_HAL_TABLE_ITERATE_BEGIN
		(
			*path,
			xme_hal_table_rowHandle_t, handle,
			xme_core_directory_communicationLink_t, communicationLink
		);
		{
			pathHandle = XME_HAL_TABLE_ADD_ITEM(*currentPath);
			XME_CHECK(0 != pathHandle, XME_CORE_STATUS_OUT_OF_RESOURCES);

			pathElement = XME_HAL_TABLE_ITEM_FROM_HANDLE(*currentPath, pathHandle);
			XME_ASSERT(NULL != pathElement);

			pathElement->node1 = communicationLink->node1;
			pathElement->interfaceNode1 = communicationLink->interfaceNode1;
			pathElement->node2 = communicationLink->node2;
			pathElement->interfaceNode2 = communicationLink->interfaceNode2;
		}
		XME_HAL_TABLE_ITERATE_END();

		// DFS over network graph
		handle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
		do
		{
			XME_HAL_TABLE_GET_NEXT
			(
				xme_core_directory_config->communicationLinks,
				xme_hal_table_rowHandle_t, handle,
				xme_core_directory_communicationLink_t, communicationLink,
				communicationLink->node1 == source
			);
			if (XME_HAL_TABLE_INVALID_ROW_HANDLE != handle)
			{
				// Check for path loop
				pathHandle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
				XME_HAL_TABLE_GET_NEXT
				(
					(*path),
					xme_hal_table_rowHandle_t, pathHandle,
					xme_core_directory_communicationLink_t, pathElement,
					pathElement->node1 == communicationLink->node2 || pathElement->node2 == communicationLink->node2
				);
				if (XME_HAL_TABLE_INVALID_ROW_HANDLE == pathHandle)
				{
					// Add communication link to current node to current communication path
					pathHandle = XME_HAL_TABLE_ADD_ITEM(*currentPath);
					XME_CHECK(0 != pathHandle, XME_CORE_STATUS_OUT_OF_RESOURCES);

					pathElement = XME_HAL_TABLE_ITEM_FROM_HANDLE(*currentPath, pathHandle);
					XME_ASSERT(NULL != pathElement);

					pathElement->node1 = communicationLink->node1;
					pathElement->interfaceNode1 = communicationLink->interfaceNode1;
					pathElement->node2 = communicationLink->node2;
					pathElement->interfaceNode2 = communicationLink->interfaceNode2;

					// Try to find path from current node to sink
					status = xme_core_directory_findPath(communicationLink->node2, sink, currentPath);
					switch (status)
					{
						case XME_CORE_STATUS_SUCCESS:
							if (0 == XME_HAL_TABLE_ITEM_COUNT(*bestPath) || xme_core_directory_cmp_pathes(currentPath, bestPath) < 0)
							{
								xme_core_directory_path_t* tmp;

								tmp = bestPath;
								bestPath = currentPath;
								currentPath = tmp;
							}
							
							XME_HAL_TABLE_FINI(*currentPath);
							XME_HAL_TABLE_INIT(*currentPath);
							XME_HAL_TABLE_ITERATE_BEGIN
							(
								*path,
								xme_hal_table_rowHandle_t, handle,
								xme_core_directory_communicationLink_t, communicationLink
							);
							{
								pathHandle = XME_HAL_TABLE_ADD_ITEM(*currentPath);
								XME_CHECK(0 != pathHandle, XME_CORE_STATUS_OUT_OF_RESOURCES);

								pathElement = XME_HAL_TABLE_ITEM_FROM_HANDLE(*currentPath, pathHandle);
								XME_ASSERT(NULL != pathElement);

								pathElement->node1 = communicationLink->node1;
								pathElement->interfaceNode1 = communicationLink->interfaceNode1;
								pathElement->node2 = communicationLink->node2;
								pathElement->interfaceNode2 = communicationLink->interfaceNode2;
							}
							XME_HAL_TABLE_ITERATE_END();
							break;

						case XME_CORE_STATUS_NOT_FOUND:
							XME_HAL_TABLE_REMOVE_ITEM(*currentPath, pathHandle);
							break;

						default:
							XME_HAL_TABLE_FINI(path2);
							XME_HAL_TABLE_FINI(path1);

							return status;
					}
				}
			}
		} while (XME_HAL_TABLE_INVALID_ROW_HANDLE != handle);

		if (0 < XME_HAL_TABLE_ITEM_COUNT(*bestPath))
		{
			XME_HAL_TABLE_ITERATE_BEGIN
			(
				*bestPath,
				xme_hal_table_rowHandle_t, handle,
				xme_core_directory_communicationLink_t, communicationLink
			);
			{
				pathHandle = XME_HAL_TABLE_ADD_ITEM(*path);
				XME_CHECK(0 != pathHandle, XME_CORE_STATUS_OUT_OF_RESOURCES);

				pathElement = XME_HAL_TABLE_ITEM_FROM_HANDLE(*path, pathHandle);
				XME_ASSERT(NULL != pathElement);

				pathElement->node1 = communicationLink->node1;
				pathElement->interfaceNode1 = communicationLink->interfaceNode1;
				pathElement->node2 = communicationLink->node2;
				pathElement->interfaceNode2 = communicationLink->interfaceNode2;
			}
			XME_HAL_TABLE_ITERATE_END();

			status = XME_CORE_STATUS_SUCCESS;
		}
		else
		{
			status = XME_CORE_STATUS_NOT_FOUND;
		}

		XME_HAL_TABLE_FINI(path2);
		XME_HAL_TABLE_FINI(path1);

		return status;
	}

	static
	xme_core_status_t
	xme_core_directory_addEndpointLink
	(
		xme_core_directory_endpoint_t* endpoint,
		xme_core_directory_endpointType_t type,
		xme_core_dataChannel_t dataChannel
	)
	{
		xme_hal_table_rowHandle_t handle;
		xme_core_directory_endpointLink_t* endpointLink;

		// Add the endpoint to the list of endpoint links
		handle = XME_HAL_TABLE_ADD_ITEM(xme_core_directory_config->endpointLinks);
		XME_CHECK(0 != handle, XME_CORE_STATUS_OUT_OF_RESOURCES);

		endpointLink = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_directory_config->endpointLinks, handle);
		XME_ASSERT(NULL != endpointLink);

		endpointLink->endpoint.node = endpoint->node;
		endpointLink->endpoint.component = endpoint->component;
		endpointLink->endpoint.port = endpoint->port;
		endpointLink->type = type;
		endpointLink->dataChannel = dataChannel;
		endpointLink->count = 1;

		return XME_CORE_STATUS_SUCCESS;
	}

	static
	xme_core_status_t
	xme_core_directory_addLink
	(
		xme_core_node_nodeId_t sourceNode,
		xme_core_interface_interfaceId_t sourceInterface,
		xme_core_node_nodeId_t sinkNode,
		xme_core_interface_interfaceId_t sinkInterface,
		xme_core_dataChannel_t dataChannel
	)
	{
		xme_hal_table_rowHandle_t handle;
		xme_core_directory_link_t* link;

		// Add the link to the list of links
		handle = XME_HAL_TABLE_ADD_ITEM(xme_core_directory_config->links);
		XME_CHECK(0 != handle, XME_CORE_STATUS_OUT_OF_RESOURCES);

		link = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_directory_config->links, handle);
		XME_ASSERT(NULL != link);

		link->sourceNode = sourceNode;
		link->sourceInterface = sourceInterface;
		link->sinkNode = sinkNode;
		link->sinkInterface = sinkInterface;
		link->dataChannel = dataChannel;
		link->count = 1;

		return XME_CORE_STATUS_SUCCESS;
	}

	static
	xme_core_status_t
	xme_core_directory_createInAndOutboundRoutes
	(
		xme_core_directory_communicationLink_t* pathElement,
		xme_core_dataChannel_t dataChannel
	)
	{
		// Add inbound route
		if (pathElement->node2 == xme_core_nodeManager_getNodeId())
		{
			XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addInboundRoute(dataChannel, pathElement->interfaceNode2), XME_CORE_STATUS_INTERNAL_ERROR);
		}
		else
		{
			xme_hal_sharedPtr_t dataHandle;
			xme_core_topic_modifyRoutingTablePacket_t* modifyRoutingTable;

			// TODO: Error handling! See ticket #721
			dataHandle = xme_hal_sharedPtr_create(sizeof(xme_core_topic_modifyRoutingTablePacket_t));
			modifyRoutingTable = (xme_core_topic_modifyRoutingTablePacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

			modifyRoutingTable->modifyRoutingTableType = XME_CORE_MODIFY_ROUTING_TABLE_TYPE_ADD_INBOUND_ROUTE;
			modifyRoutingTable->dataChannel = dataChannel;
			modifyRoutingTable->inboundRoute.networkInterface = pathElement->interfaceNode2;

			// TODO: Error handling x2! See ticket #721
			xme_core_broker_deliverData(xme_core_directory_getRemoteModifyRoutingTableDataChannelForNode(pathElement->node2), dataHandle, xme_core_resourceManager_getCurrentComponentId());

			xme_hal_sharedPtr_destroy(dataHandle);
		}

		// Add outbound route
		if (pathElement->node1 == xme_core_nodeManager_getNodeId())
		{
			XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addOutboundRoute(dataChannel, pathElement->interfaceNode1), XME_CORE_STATUS_INTERNAL_ERROR);
		}
		else
		{
			xme_hal_sharedPtr_t dataHandle;
			xme_core_topic_modifyRoutingTablePacket_t* modifyRoutingTable;

			// TODO: Error handling! See ticket #721
			dataHandle = xme_hal_sharedPtr_create(sizeof(xme_core_topic_modifyRoutingTablePacket_t));
			modifyRoutingTable = (xme_core_topic_modifyRoutingTablePacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

			modifyRoutingTable->modifyRoutingTableType = XME_CORE_MODIFY_ROUTING_TABLE_TYPE_ADD_OUTBOUND_ROUTE;
			modifyRoutingTable->dataChannel = dataChannel;
			modifyRoutingTable->outboundRoute.networkInterface = pathElement->interfaceNode1;

			// TODO: Error handling x2! See ticket #721
			xme_core_broker_deliverData(xme_core_directory_getRemoteModifyRoutingTableDataChannelForNode(pathElement->node1), dataHandle, xme_core_resourceManager_getCurrentComponentId());

			xme_hal_sharedPtr_destroy(dataHandle);
		}

		return XME_CORE_STATUS_SUCCESS;
	}

	static
	xme_core_status_t
	xme_core_directory_createLink
	(
		xme_core_directory_communicationLink_t* pathElement,
		xme_core_dataChannel_t dataChannel
	)
	{
		bool foundLink;

		// Check if current communication link already exist
		foundLink = false;
		XME_HAL_TABLE_ITERATE
		(
			xme_core_directory_config->links,
			xme_hal_table_rowHandle_t, handle,
			xme_core_directory_link_t, link,
			{
				if (link->dataChannel == dataChannel &&
					link->sourceNode == pathElement->node1 && link->sourceInterface == pathElement->interfaceNode1 &&
					link->sinkNode == pathElement->node2 && link->sinkInterface == pathElement->interfaceNode2)
				{
					link->count++;
					
					foundLink = true;
					
					break;
				}
			}
		);
		// Create communication link if it doesn't already exist
		if (!foundLink)
		{
			xme_core_status_t status;

			status = xme_core_directory_addLink(pathElement->node1, pathElement->interfaceNode1, pathElement->node2, pathElement->interfaceNode2, dataChannel);
			XME_CHECK(XME_CORE_STATUS_SUCCESS == status, status);

			status = xme_core_directory_createInAndOutboundRoutes(pathElement, dataChannel);
			XME_CHECK(XME_CORE_STATUS_SUCCESS == status, status);

		}

		return XME_CORE_STATUS_SUCCESS;
	}

	static
	xme_core_status_t
	xme_core_directory_establishDataChannel
	(
		xme_core_directory_publicationItem_t* publication,
		xme_core_directory_path_t* path,
		xme_core_directory_subscriptionItem_t* subscription
	)
	{
		xme_core_dataChannel_t dataChannel;
		bool foundSourceEndpoint;
		bool foundSinkEndpoint;

		// Get data channel for communication path
		dataChannel = xme_core_directory_getDataChannel(subscription->topic, subscription->metaData, false);
		XME_ASSERT(XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != dataChannel);

		// Add communication relation
		xme_core_directory_addCommunicationRelation(&publication->publisher, dataChannel, &subscription->subscriber);
		XME_LOG(XME_LOG_VERBOSE, "%d.%d.%d -> %d.%d.%d (%d)\n", publication->publisher.node, publication->publisher.component, publication->publisher.port, subscription->subscriber.node, subscription->subscriber.component, subscription->subscriber.port, dataChannel);

		// Check if source and sink endpoint already exist
		foundSourceEndpoint = false;
		foundSinkEndpoint = false;
		XME_HAL_TABLE_ITERATE_BEGIN
		(
			xme_core_directory_config->endpointLinks,
			xme_hal_table_rowHandle_t, handle,
			xme_core_directory_endpointLink_t, endpoint
		);
		{
			if (endpoint->dataChannel == dataChannel)
			{
				if (XME_CORE_DIRECTORY_ENDPOINT_TYPE_SOURCE == endpoint->type)
				{
					if (endpoint->endpoint.node == publication->publisher.node && endpoint->endpoint.component == publication->publisher.component && endpoint->endpoint.port == publication->publisher.port)
					{
						endpoint->count++;

						foundSourceEndpoint = true;

						if (foundSinkEndpoint)
						{
							break;
						}
					}
				}
				else //XME_CORE_DIRECTORY_ENDPOINT_TYPE_SINK
				{
					if (endpoint->endpoint.node == subscription->subscriber.node && endpoint->endpoint.component == subscription->subscriber.component && endpoint->endpoint.port == subscription->subscriber.port)
					{
						endpoint->count++;

						foundSinkEndpoint = true;

						if (foundSourceEndpoint)
						{
							break;
						}
					}
				}
			}
		}
		XME_HAL_TABLE_ITERATE_END();

		// Create source endpoint if it doesn't already exist
		if (!foundSourceEndpoint)
		{
			xme_core_status_t status;

			status = xme_core_directory_addEndpointLink(&publication->publisher, XME_CORE_DIRECTORY_ENDPOINT_TYPE_SOURCE, dataChannel);
			XME_CHECK(XME_CORE_STATUS_SUCCESS == status, status);

			// Add local source route
			XME_LOG(XME_LOG_VERBOSE, "LS: %d.%d.%d (%d)\n", publication->publisher.node, publication->publisher.component, publication->publisher.port, dataChannel);
			if (publication->publisher.node == xme_core_nodeManager_getNodeId())
			{
				XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addLocalSourceRoute(dataChannel, publication->publisher.component, publication->publisher.port), XME_CORE_STATUS_INTERNAL_ERROR);
			}
			else
			{
				xme_hal_sharedPtr_t dataHandle;
				xme_core_topic_modifyRoutingTablePacket_t* modifyRoutingTable;

				// TODO: Error handling! See ticket #721
				dataHandle = xme_hal_sharedPtr_create(sizeof(xme_core_topic_modifyRoutingTablePacket_t));
				modifyRoutingTable = (xme_core_topic_modifyRoutingTablePacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

				modifyRoutingTable->modifyRoutingTableType = XME_CORE_MODIFY_ROUTING_TABLE_TYPE_ADD_LOCAL_SOURCE_ROUTE;
				modifyRoutingTable->dataChannel = dataChannel;
				modifyRoutingTable->localSourceRoute.component = publication->publisher.component;
				modifyRoutingTable->localSourceRoute.port = publication->publisher.port;

				// TODO: Error handling x2! See ticket #721
				xme_core_broker_deliverData(xme_core_directory_getRemoteModifyRoutingTableDataChannelForNode(publication->publisher.node), dataHandle, xme_core_resourceManager_getCurrentComponentId());

				xme_hal_sharedPtr_destroy(dataHandle);
			}

			//printf("\tCreated source endpoint\n");
		}

		// Construct missing parts of communication path
		XME_HAL_TABLE_ITERATE_BEGIN
		(
			(*path),
			xme_hal_table_rowHandle_t, handle,
			xme_core_directory_communicationLink_t, pathElement
		);
		{
			xme_core_status_t status;

			status = xme_core_directory_createLink(pathElement, dataChannel);
			XME_CHECK(XME_CORE_STATUS_SUCCESS == status, status);
			XME_LOG(XME_LOG_VERBOSE, "L: %d -> %d (%d)\n", pathElement->node1, pathElement->node2, dataChannel);
		}
		XME_HAL_TABLE_ITERATE_END();

		// Create sink endpoint if it doesn't already exist
		if (!foundSinkEndpoint)
		{
			xme_core_status_t status;

			status = xme_core_directory_addEndpointLink(&subscription->subscriber, XME_CORE_DIRECTORY_ENDPOINT_TYPE_SINK, dataChannel);
			XME_CHECK(XME_CORE_STATUS_SUCCESS == status, status);
		
			// Add local destination route
			XME_LOG(XME_LOG_VERBOSE, "LD: %d.%d.%d (%d)\n", subscription->subscriber.node, subscription->subscriber.component, subscription->subscriber.port, dataChannel);
			if (subscription->subscriber.node == xme_core_nodeManager_getNodeId())
			{
				XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addLocalDestinationRoute(dataChannel, subscription->subscriber.component, subscription->subscriber.port), XME_CORE_STATUS_INTERNAL_ERROR);
			}
			else
			{
				xme_hal_sharedPtr_t dataHandle;
				xme_core_topic_modifyRoutingTablePacket_t* modifyRoutingTable;

				// TODO: Error handling! See ticket #721
				dataHandle = xme_hal_sharedPtr_create(sizeof(xme_core_topic_modifyRoutingTablePacket_t));
				modifyRoutingTable = (xme_core_topic_modifyRoutingTablePacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

				modifyRoutingTable->modifyRoutingTableType = XME_CORE_MODIFY_ROUTING_TABLE_TYPE_ADD_LOCAL_DESTINATION_ROUTE;
				modifyRoutingTable->dataChannel = dataChannel;
				modifyRoutingTable->localDestinationRoute.component = subscription->subscriber.component;
				modifyRoutingTable->localDestinationRoute.port = subscription->subscriber.port;

				// TODO: Error handling x2! See ticket #721
				xme_core_broker_deliverData(xme_core_directory_getRemoteModifyRoutingTableDataChannelForNode(subscription->subscriber.node), dataHandle, xme_core_resourceManager_getCurrentComponentId());

				xme_hal_sharedPtr_destroy(dataHandle);
			}

			//printf("\tCreated sink endpoint\n");
		}

		return XME_CORE_STATUS_SUCCESS;
	}

	static
	xme_core_status_t
	xme_core_directory_establishRRDataChannels
	(
		xme_core_directory_requestItem_t* request,
		xme_core_directory_path_t* path,
		xme_core_directory_requestHandlerItem_t* requestHandler
	)
	{
		xme_core_dataChannel_t requestDataChannel;
		xme_core_dataChannel_t responseDataChannel;
		xme_core_status_t status;

		// Get data channels for communication paths
		requestDataChannel = xme_core_directory_getDataChannel(request->requestTopic, request->requestMetaData, true);
		XME_ASSERT(XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != requestDataChannel);
		responseDataChannel = xme_core_directory_getDataChannel(requestHandler->responseTopic, requestHandler->responseMetaData, true);
		XME_ASSERT(XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != responseDataChannel);

		// Add communication relations
		xme_core_directory_addCommunicationRelation(&request->requester, requestDataChannel, &requestHandler->requestHandler);
		xme_core_directory_addCommunicationRelation(&requestHandler->responseSender, responseDataChannel, &request->responseHandler);

		//printf("Request Data Channel: %d\n", requestDataChannel);
		//printf("Response Data Channel: %d\n", responseDataChannel);
		//printf("Source: %d.%d.%d\n", request->requester.node, request->requester.component, request->requester.port);
		//printf("Sink: %d.%d.%d\n", requestHandler->requestHandler.node, requestHandler->requestHandler.component, requestHandler->requestHandler.port);

		status = xme_core_directory_addEndpointLink(&request->requester, XME_CORE_DIRECTORY_ENDPOINT_TYPE_SOURCE, requestDataChannel);
		XME_CHECK(XME_CORE_STATUS_SUCCESS == status, status);
		status = xme_core_directory_addEndpointLink(&request->responseHandler, XME_CORE_DIRECTORY_ENDPOINT_TYPE_SINK, responseDataChannel);
		XME_CHECK(XME_CORE_STATUS_SUCCESS == status, status);

		// Add local source route request
		if (request->requester.node == xme_core_nodeManager_getNodeId())
		{
			XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addLocalSourceRoute(requestDataChannel, request->requester.component, request->requester.port), XME_CORE_STATUS_INTERNAL_ERROR);
		}
		else
		{
			xme_hal_sharedPtr_t dataHandle;
			xme_core_topic_modifyRoutingTablePacket_t* modifyRoutingTable;

			// TODO: Error handling! See ticket #721
			dataHandle = xme_hal_sharedPtr_create(sizeof(xme_core_topic_modifyRoutingTablePacket_t));
			modifyRoutingTable = (xme_core_topic_modifyRoutingTablePacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

			modifyRoutingTable->modifyRoutingTableType = XME_CORE_MODIFY_ROUTING_TABLE_TYPE_ADD_LOCAL_SOURCE_ROUTE;
			modifyRoutingTable->dataChannel = requestDataChannel;
			modifyRoutingTable->localSourceRoute.component = request->requester.component;
			modifyRoutingTable->localSourceRoute.port = request->requester.port;

			// TODO: Error handling x2! See ticket #721
			xme_core_broker_deliverData(xme_core_directory_getRemoteModifyRoutingTableDataChannelForNode(request->requester.node), dataHandle, xme_core_resourceManager_getCurrentComponentId());

			xme_hal_sharedPtr_destroy(dataHandle);
		}

		// Add local destination route response handler
		if (request->requester.node == xme_core_nodeManager_getNodeId())
		{
			XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addLocalDestinationRoute(responseDataChannel, request->responseHandler.component, request->responseHandler.port), XME_CORE_STATUS_INTERNAL_ERROR);
		}
		else
		{
			xme_hal_sharedPtr_t dataHandle;
			xme_core_topic_modifyRoutingTablePacket_t* modifyRoutingTable;

			// TODO: Error handling! See ticket #721
			dataHandle = xme_hal_sharedPtr_create(sizeof(xme_core_topic_modifyRoutingTablePacket_t));
			modifyRoutingTable = (xme_core_topic_modifyRoutingTablePacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

			modifyRoutingTable->modifyRoutingTableType = XME_CORE_MODIFY_ROUTING_TABLE_TYPE_ADD_LOCAL_DESTINATION_ROUTE;
			modifyRoutingTable->dataChannel = responseDataChannel;
			modifyRoutingTable->localDestinationRoute.component = request->responseHandler.component;
			modifyRoutingTable->localDestinationRoute.port = request->responseHandler.port;

			// TODO: Error handling x2! See ticket #721
			xme_core_broker_deliverData(xme_core_directory_getRemoteModifyRoutingTableDataChannelForNode(request->requester.node), dataHandle, xme_core_resourceManager_getCurrentComponentId());

			xme_hal_sharedPtr_destroy(dataHandle);
		}

		//printf("\tCreated request endpoints\n");

		// Construct missing parts of communication path
		XME_HAL_TABLE_ITERATE
		(
			(*path),
			xme_hal_table_rowHandle_t, handle,
			xme_core_directory_communicationLink_t, pathElement,
			{
				xme_core_directory_communicationLink_t reversePathElement;

				//printf("Hop: %d -> %d\n", pathElement->node1, pathElement->node2);

				status = xme_core_directory_addLink(pathElement->node1, pathElement->interfaceNode1, pathElement->node2, pathElement->interfaceNode2, requestDataChannel);
				XME_CHECK(XME_CORE_STATUS_SUCCESS == status, status);
				status = xme_core_directory_addLink(pathElement->node2, pathElement->interfaceNode2, pathElement->node1, pathElement->interfaceNode1, responseDataChannel);
				XME_CHECK(XME_CORE_STATUS_SUCCESS == status, status);

				status = xme_core_directory_createInAndOutboundRoutes(pathElement, requestDataChannel);
				XME_CHECK(XME_CORE_STATUS_SUCCESS == status, status);

				reversePathElement.node1 = pathElement->node2;
				reversePathElement.interfaceNode1 = pathElement->interfaceNode2;
				reversePathElement.node2 = pathElement->node1;
				reversePathElement.interfaceNode2 = pathElement->interfaceNode1;
				status = xme_core_directory_createInAndOutboundRoutes(&reversePathElement, responseDataChannel);
				XME_CHECK(XME_CORE_STATUS_SUCCESS == status, status);

				//printf("\tCreated links\n");
			}
		);


		status = xme_core_directory_addEndpointLink(&requestHandler->requestHandler, XME_CORE_DIRECTORY_ENDPOINT_TYPE_SINK, requestDataChannel);
		XME_CHECK(XME_CORE_STATUS_SUCCESS == status, status);
		status = xme_core_directory_addEndpointLink(&requestHandler->responseSender, XME_CORE_DIRECTORY_ENDPOINT_TYPE_SOURCE, responseDataChannel);
		XME_CHECK(XME_CORE_STATUS_SUCCESS == status, status);

		// Add local destination route request handler
		if (requestHandler->requestHandler.node == xme_core_nodeManager_getNodeId())
		{
			XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addLocalDestinationRoute(requestDataChannel, requestHandler->requestHandler.component, requestHandler->requestHandler.port), XME_CORE_STATUS_INTERNAL_ERROR);
		}
		else
		{
			xme_hal_sharedPtr_t dataHandle;
			xme_core_topic_modifyRoutingTablePacket_t* modifyRoutingTable;

			// TODO: Error handling! See ticket #721
			dataHandle = xme_hal_sharedPtr_create(sizeof(xme_core_topic_modifyRoutingTablePacket_t));
			modifyRoutingTable = (xme_core_topic_modifyRoutingTablePacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

			modifyRoutingTable->modifyRoutingTableType = XME_CORE_MODIFY_ROUTING_TABLE_TYPE_ADD_LOCAL_DESTINATION_ROUTE;
			modifyRoutingTable->dataChannel = requestDataChannel;
			modifyRoutingTable->localDestinationRoute.component = requestHandler->requestHandler.component;
			modifyRoutingTable->localDestinationRoute.port = requestHandler->requestHandler.port;

			// TODO: Error handling x2! See ticket #721
			xme_core_broker_deliverData(xme_core_directory_getRemoteModifyRoutingTableDataChannelForNode(requestHandler->requestHandler.node), dataHandle, xme_core_resourceManager_getCurrentComponentId());

			xme_hal_sharedPtr_destroy(dataHandle);
		}


		// Add local source route response sender
		if (requestHandler->requestHandler.node == xme_core_nodeManager_getNodeId())
		{
			XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addLocalSourceRoute(responseDataChannel, requestHandler->responseSender.component, requestHandler->responseSender.port), XME_CORE_STATUS_INTERNAL_ERROR);
		}
		else
		{
			xme_hal_sharedPtr_t dataHandle;
			xme_core_topic_modifyRoutingTablePacket_t* modifyRoutingTable;

			// TODO: Error handling! See ticket #721
			dataHandle = xme_hal_sharedPtr_create(sizeof(xme_core_topic_modifyRoutingTablePacket_t));
			modifyRoutingTable = (xme_core_topic_modifyRoutingTablePacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

			modifyRoutingTable->modifyRoutingTableType = XME_CORE_MODIFY_ROUTING_TABLE_TYPE_ADD_LOCAL_SOURCE_ROUTE;
			modifyRoutingTable->dataChannel = responseDataChannel;
			modifyRoutingTable->localSourceRoute.component = requestHandler->responseSender.component;
			modifyRoutingTable->localSourceRoute.port = requestHandler->responseSender.port;

			// TODO: Error handling x2! See ticket #721
			xme_core_broker_deliverData(xme_core_directory_getRemoteModifyRoutingTableDataChannelForNode(requestHandler->requestHandler.node), dataHandle, xme_core_resourceManager_getCurrentComponentId());

			xme_hal_sharedPtr_destroy(dataHandle);
		}

		//printf("\tCreated request handler endpoints\n");

		return XME_CORE_STATUS_SUCCESS;
	}

	static
	xme_core_status_t
	xme_core_directory_constructManagementChannels
	(
		xme_core_node_nodeId_t directory,
		xme_core_node_nodeId_t node,
		xme_core_dataChannel_t remoteModifyRoutingTableDataChannel
	)
	{
		xme_hal_table_rowHandle_t handle;
		xme_core_directory_dataChannelToNode_t* item;
		xme_core_directory_path_t path;

		XME_HAL_TABLE_INIT(path);

		XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_directory_findPath(directory, node, &path), XME_CORE_STATUS_NOT_FOUND);

		// Add remote modify routing table data channel to list
		handle = XME_HAL_TABLE_ADD_ITEM(xme_core_directory_config->remoteModifyRoutingTableDataChannels);
		XME_CHECK(0 != handle, XME_CORE_STATUS_OUT_OF_RESOURCES);

		item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_directory_config->remoteModifyRoutingTableDataChannels, handle);
		XME_ASSERT(NULL != item);

		item->dataChannel = remoteModifyRoutingTableDataChannel;
		item->node = node;

		// Add local source route for remote modify routing table channel
		XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addLocalSourceRoute(remoteModifyRoutingTableDataChannel, xme_core_resourceManager_getCurrentComponentId(), xme_core_resourceManager_getPublicationPort(xme_core_directory_config->remoteModifyRoutingTableHandle)), XME_CORE_STATUS_INTERNAL_ERROR);

		// Construct missing parts of communication path
		XME_HAL_TABLE_ITERATE_BEGIN
		(
			path,
			xme_hal_table_rowHandle_t, handle,
			xme_core_directory_communicationLink_t, pathElement
		);
		{
			// Check if path element is not last hop (data routes of last hop will be constructed by node manager and login server client proxy)
			if (pathElement->node2 != node)
			{
				// Create remote announcement and modify routing table routes between nodes of path element
				xme_core_status_t status;
				xme_core_directory_communicationLink_t reversePathElement;

				status = xme_core_directory_createInAndOutboundRoutes(pathElement, remoteModifyRoutingTableDataChannel);
				XME_CHECK(XME_CORE_STATUS_SUCCESS == status, status);

				reversePathElement.node1 = pathElement->node2;
				reversePathElement.interfaceNode1 = pathElement->interfaceNode2;
				reversePathElement.node2 = pathElement->node1;
				reversePathElement.interfaceNode2 = pathElement->interfaceNode1;
				status = xme_core_directory_createInAndOutboundRoutes(&reversePathElement, xme_core_directory_config->remoteAnnouncementDataChannel);
				XME_CHECK(XME_CORE_STATUS_SUCCESS == status, status);
			}
		}
		XME_HAL_TABLE_ITERATE_END();

		XME_HAL_TABLE_FINI(path);

		return XME_CORE_STATUS_SUCCESS;
	}

	static
	xme_core_dataChannel_t
	xme_core_directory_getRemoteModifyRoutingTableDataChannelForNode
	(
		xme_core_node_nodeId_t nodeId
	)
	{
		XME_HAL_TABLE_ITERATE
		(
			xme_core_directory_config->remoteModifyRoutingTableDataChannels,
			xme_hal_table_rowHandle_t, loopHandle, xme_core_directory_dataChannelToNode_t, item,
			{
				if (item->node == nodeId)
				{
					return item->dataChannel;
				}
			}
		);

		return XME_CORE_DATACHANNEL_INVALID_DATACHANNEL;
	}
#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER

static
xme_core_status_t
xme_core_directory_constructDataChannel
(
	xme_core_directory_publicationItem_t* publication,
	xme_core_directory_subscriptionItem_t* subscription
)
{
	#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
		xme_core_directory_path_t path;

		XME_HAL_TABLE_INIT(path);
		// Check if communiation path exists
		if (XME_CORE_STATUS_SUCCESS == xme_core_directory_findPath(publication->publisher.node, subscription->subscriber.node, &path))
		{
			// Establish data channel
			XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_directory_establishDataChannel(publication, &path, subscription), XME_CORE_STATUS_INTERNAL_ERROR);
		}
		XME_HAL_TABLE_FINI(path);
	#else // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER
		xme_core_dataChannel_t dataChannel;

		// Get data channel for communication path
		dataChannel = xme_core_directory_getDataChannel(subscription->topic, subscription->metaData, false);
		XME_ASSERT(XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != dataChannel);

		// Add communication relation
		xme_core_directory_addCommunicationRelation(&publication->publisher, dataChannel, &subscription->subscriber);

		// Add local source route
		XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addLocalSourceRoute(dataChannel, publication->publisher.component, publication->publisher.port), XME_CORE_STATUS_INTERNAL_ERROR);

		// Add local destination route
		XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addLocalDestinationRoute(dataChannel, subscription->subscriber.component, subscription->subscriber.port), XME_CORE_STATUS_INTERNAL_ERROR);
	#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER

	return XME_CORE_STATUS_SUCCESS;
}
#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
	static
	xme_core_status_t
	xme_core_directory_getCommunicationPath
	(
		xme_core_node_nodeId_t source,
		xme_core_node_nodeId_t sink,
		xme_core_dataChannel_t dataChannel,
		xme_core_directory_communicationPath_t* path
	)
	{
		xme_hal_table_rowHandle_t pathHandle;
		xme_core_directory_link_t** pathElement;
		xme_core_status_t status;

		if (source == sink)
		{
			return XME_CORE_STATUS_SUCCESS;
		}
		
		XME_HAL_TABLE_ITERATE_BEGIN
		(
			xme_core_directory_config->links,
			xme_hal_table_rowHandle_t, linkHandle,
			xme_core_directory_link_t, link
		);
		{
			if (link->sourceNode == source && link->dataChannel == dataChannel)
			{
				// Check for path loop
				pathHandle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
				XME_HAL_TABLE_GET_NEXT
				(
					(*path),
					xme_hal_table_rowHandle_t, pathHandle,
					xme_core_directory_link_t, pathElement,
					(*pathElement)->sourceNode == link->sinkNode || (*pathElement)->sinkNode == link->sinkNode
				);
				if (XME_HAL_TABLE_INVALID_ROW_HANDLE == pathHandle)
				{
					// Add communication link to current node to current communication path
					pathHandle = XME_HAL_TABLE_ADD_ITEM(*path);
					XME_CHECK(0 != pathHandle, XME_CORE_STATUS_OUT_OF_RESOURCES);

					pathElement = XME_HAL_TABLE_ITEM_FROM_HANDLE(*path, pathHandle);
					XME_ASSERT(NULL != pathElement);

					*pathElement = link;

					// Try to find path from current node to sink
					status = xme_core_directory_getCommunicationPath(link->sinkNode, sink, dataChannel, path);
					switch (status)
					{
						case XME_CORE_STATUS_SUCCESS:
							return XME_CORE_STATUS_SUCCESS;

						case XME_CORE_STATUS_NOT_FOUND:
							XME_HAL_TABLE_REMOVE_ITEM(*path, pathHandle);
							break;

						default:
							XME_HAL_TABLE_REMOVE_ITEM(*path, pathHandle);
							return status;
					}
				}
			}
		}
		XME_HAL_TABLE_ITERATE_END();

		return XME_CORE_STATUS_NOT_FOUND;
	}

	static
	xme_core_status_t
	xme_core_directory_deconstructDataChannel
	(
		xme_core_directory_endpoint_t* source,
		xme_core_directory_endpoint_t* sink,
		xme_core_dataChannel_t dataChannel
	)
	{
		xme_hal_table_rowHandle_t linkHandle;
		xme_core_directory_endpointLink_t* endpointLink;
		xme_core_directory_communicationPath_t path;

		XME_HAL_TABLE_INIT(path);

		XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_directory_getCommunicationPath(source->node, sink->node, dataChannel, &path), XME_CORE_STATUS_INTERNAL_ERROR);

		linkHandle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
		XME_HAL_TABLE_GET_NEXT
		(
			xme_core_directory_config->endpointLinks,
			xme_hal_table_rowHandle_t, linkHandle,
			xme_core_directory_endpointLink_t, endpointLink,
			endpointLink->type == XME_CORE_DIRECTORY_ENDPOINT_TYPE_SOURCE && endpointLink->dataChannel == dataChannel &&
			endpointLink->endpoint.node == source->node && endpointLink->endpoint.component == source->component && endpointLink->endpoint.port == source->port
		);
		if (XME_HAL_TABLE_INVALID_ROW_HANDLE != linkHandle)
		{
			endpointLink->count--;
					
			if (0 == endpointLink->count)
			{
				// Remove local source route
				if (endpointLink->endpoint.node == xme_core_nodeManager_getNodeId())
				{
					XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_removeLocalSourceRoute(dataChannel), XME_CORE_STATUS_INTERNAL_ERROR);
				}
				else
				{
					xme_hal_sharedPtr_t dataHandle;
					xme_core_topic_modifyRoutingTablePacket_t* modifyRoutingTable;

					// TODO: Error handling! See ticket #721
					dataHandle = xme_hal_sharedPtr_create(sizeof(xme_core_topic_modifyRoutingTablePacket_t));
					modifyRoutingTable = (xme_core_topic_modifyRoutingTablePacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

					modifyRoutingTable->modifyRoutingTableType = XME_CORE_MODIFY_ROUTING_TABLE_TYPE_REMOVE_LOCAL_SOURCE_ROUTE;
					modifyRoutingTable->dataChannel = endpointLink->dataChannel;
					modifyRoutingTable-> localSourceRoute.component = endpointLink->endpoint.component;
					modifyRoutingTable->localSourceRoute.port = endpointLink->endpoint.port;

					// TODO: Error handling x2! See ticket #721
					xme_core_broker_deliverData(xme_core_directory_getRemoteModifyRoutingTableDataChannelForNode(endpointLink->endpoint.node), dataHandle, xme_core_resourceManager_getCurrentComponentId());

					xme_hal_sharedPtr_destroy(dataHandle);
				}

				XME_CHECK(XME_CORE_STATUS_SUCCESS == XME_HAL_TABLE_REMOVE_ITEM(xme_core_directory_config->endpointLinks, linkHandle), XME_CORE_STATUS_INTERNAL_ERROR);
			}
		}

		XME_HAL_TABLE_ITERATE_BEGIN
		(
			path,
			xme_hal_table_rowHandle_t, linkHandle,
			xme_core_directory_link_t*, link
		);
		{
			(*link)->count--;

			if (0 == (*link)->count)
			{
				// Add inbound route
				if ((*link)->sinkNode == xme_core_nodeManager_getNodeId())
				{
					XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_removeInboundRoute((*link)->dataChannel, (*link)->sinkInterface), XME_CORE_STATUS_INTERNAL_ERROR);
				}
				else
				{
					xme_hal_sharedPtr_t dataHandle;
					xme_core_topic_modifyRoutingTablePacket_t* modifyRoutingTable;

					// TODO: Error handling! See ticket #721
					dataHandle = xme_hal_sharedPtr_create(sizeof(xme_core_topic_modifyRoutingTablePacket_t));
					modifyRoutingTable = (xme_core_topic_modifyRoutingTablePacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

					modifyRoutingTable->modifyRoutingTableType = XME_CORE_MODIFY_ROUTING_TABLE_TYPE_REMOVE_INBOUND_ROUTE;
					modifyRoutingTable->dataChannel = (*link)->dataChannel;
					modifyRoutingTable->inboundRoute.networkInterface = (*link)->sinkInterface;

					// TODO: Error handling x2! See ticket #721
					xme_core_broker_deliverData(xme_core_directory_getRemoteModifyRoutingTableDataChannelForNode((*link)->sinkNode), dataHandle, xme_core_resourceManager_getCurrentComponentId());

					xme_hal_sharedPtr_destroy(dataHandle);
				}

				// Add outbound route
				if ((*link)->sourceNode == xme_core_nodeManager_getNodeId())
				{
					XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_removeOutboundRoute((*link)->dataChannel, (*link)->sourceInterface), XME_CORE_STATUS_INTERNAL_ERROR);
				}
				else
				{
					xme_hal_sharedPtr_t dataHandle;
					xme_core_topic_modifyRoutingTablePacket_t* modifyRoutingTable;

					// TODO: Error handling! See ticket #721
					dataHandle = xme_hal_sharedPtr_create(sizeof(xme_core_topic_modifyRoutingTablePacket_t));
					modifyRoutingTable = (xme_core_topic_modifyRoutingTablePacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

					modifyRoutingTable->modifyRoutingTableType = XME_CORE_MODIFY_ROUTING_TABLE_TYPE_REMOVE_OUTBOUND_ROUTE;
					modifyRoutingTable->dataChannel = (*link)->dataChannel;
					modifyRoutingTable->outboundRoute.networkInterface = (*link)->sourceInterface;

					// TODO: Error handling x2! See ticket #721
					xme_core_broker_deliverData(xme_core_directory_getRemoteModifyRoutingTableDataChannelForNode((*link)->sourceNode), dataHandle, xme_core_resourceManager_getCurrentComponentId());

					xme_hal_sharedPtr_destroy(dataHandle);
				}

				{
					xme_hal_table_rowHandle_t linkHandle2;
					xme_core_directory_link_t* link2;

					linkHandle2 = XME_HAL_TABLE_INVALID_ROW_HANDLE;
					XME_HAL_TABLE_GET_NEXT
					(
						xme_core_directory_config->links,
						xme_hal_table_rowHandle_t, linkHandle2,
						xme_core_directory_link_t, link2,
						link2 == *link
					);
					XME_ASSERT(XME_HAL_TABLE_INVALID_ROW_HANDLE != linkHandle2);
					XME_CHECK(XME_CORE_STATUS_SUCCESS == XME_HAL_TABLE_REMOVE_ITEM(xme_core_directory_config->links, linkHandle2), XME_CORE_STATUS_INTERNAL_ERROR);
				}
			}
		}
		XME_HAL_TABLE_ITERATE_END();

		linkHandle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
		XME_HAL_TABLE_GET_NEXT
		(
			xme_core_directory_config->endpointLinks,
			xme_hal_table_rowHandle_t, linkHandle,
			xme_core_directory_endpointLink_t, endpointLink,
			endpointLink->type == XME_CORE_DIRECTORY_ENDPOINT_TYPE_SINK && endpointLink->dataChannel == dataChannel &&
			endpointLink->endpoint.node == sink->node && endpointLink->endpoint.component == sink->component && endpointLink->endpoint.port == sink->port
		);
		if (XME_HAL_TABLE_INVALID_ROW_HANDLE != linkHandle)
		{
			endpointLink->count--;

			if (0 == endpointLink->count)
			{
				// Remove local destination route
				if (endpointLink->endpoint.node == xme_core_nodeManager_getNodeId())
				{
					XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_removeLocalDestinationRoute(dataChannel, endpointLink->endpoint.component, endpointLink->endpoint.port), XME_CORE_STATUS_INTERNAL_ERROR);
				}
				else
				{
					xme_hal_sharedPtr_t dataHandle;
					xme_core_topic_modifyRoutingTablePacket_t* modifyRoutingTable;

					// TODO: Error handling! See ticket #721
					dataHandle = xme_hal_sharedPtr_create(sizeof(xme_core_topic_modifyRoutingTablePacket_t));
					modifyRoutingTable = (xme_core_topic_modifyRoutingTablePacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

					modifyRoutingTable->modifyRoutingTableType = XME_CORE_MODIFY_ROUTING_TABLE_TYPE_REMOVE_LOCAL_DESTINATION_ROUTE;
					modifyRoutingTable->dataChannel = endpointLink->dataChannel;
					modifyRoutingTable->localDestinationRoute.component = endpointLink->endpoint.component;
					modifyRoutingTable->localDestinationRoute.port = endpointLink->endpoint.port;

					// TODO: Error handling x2! See ticket #721
					xme_core_broker_deliverData(xme_core_directory_getRemoteModifyRoutingTableDataChannelForNode(endpointLink->endpoint.node), dataHandle, xme_core_resourceManager_getCurrentComponentId());

					xme_hal_sharedPtr_destroy(dataHandle);
				}

				XME_CHECK(XME_CORE_STATUS_SUCCESS == XME_HAL_TABLE_REMOVE_ITEM(xme_core_directory_config->endpointLinks, linkHandle), XME_CORE_STATUS_INTERNAL_ERROR);
			}
		}

		XME_HAL_TABLE_FINI(path);// xme_core_broker_removeRoute
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}
#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER

static
bool
xme_core_directory_existsRRDataChannels
(
	xme_core_directory_requestItem_t* request,
	xme_core_directory_requestHandlerItem_t* requestHandler
)
{
	XME_HAL_TABLE_ITERATE
	(
		xme_core_directory_config->communicationRelations,
		xme_hal_table_rowHandle_t, handle,
		xme_core_directory_communicationRelation_t, communication,
		{
			if (&request->requester == communication->source && &requestHandler->requestHandler == communication->sink)
			{
				return true;
			}
		}
	);

	return false;
}

static
xme_core_status_t
xme_core_directory_constructRRDataChannels
(
	xme_core_directory_requestItem_t* request,
	xme_core_directory_requestHandlerItem_t* requestHandler
)
{
	#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
		xme_core_directory_path_t path;

		XME_HAL_TABLE_INIT(path);
		// Check if communiation path exists
		if (XME_CORE_STATUS_SUCCESS == xme_core_directory_findPath(request->requester.node, requestHandler->requestHandler.node, &path))
		{
			// Establish request response data channels
			XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_directory_establishRRDataChannels(request, &path, requestHandler), XME_CORE_STATUS_INTERNAL_ERROR);
		}
		XME_HAL_TABLE_FINI(path);
	#else // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER
		xme_core_dataChannel_t requestDataChannel;
		xme_core_dataChannel_t responseDataChannel;

		// Get data channels for communication paths
		requestDataChannel = xme_core_directory_getDataChannel(request->requestTopic, request->requestMetaData, true);
		XME_ASSERT(XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != requestDataChannel);
		responseDataChannel = xme_core_directory_getDataChannel(requestHandler->responseTopic, requestHandler->responseMetaData, true);
		XME_ASSERT(XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != responseDataChannel);

		// Add communication relations
		xme_core_directory_addCommunicationRelation(&request->requester, requestDataChannel, &requestHandler->requestHandler);
		xme_core_directory_addCommunicationRelation(&requestHandler->responseSender, responseDataChannel, &request->responseHandler);

		// Add local source route request
		XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addLocalSourceRoute(requestDataChannel, request->requester.component, request->requester.port), XME_CORE_STATUS_INTERNAL_ERROR);

		// Add local destination route request
		XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addLocalDestinationRoute(responseDataChannel, request->responseHandler.component, request->responseHandler.port), XME_CORE_STATUS_INTERNAL_ERROR);

		// Add local destination route request handler
		XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addLocalDestinationRoute(requestDataChannel, requestHandler->requestHandler.component, requestHandler->requestHandler.port), XME_CORE_STATUS_INTERNAL_ERROR);

		// Add local source route request handler
		XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addLocalSourceRoute(responseDataChannel, requestHandler->responseSender.component, requestHandler->responseSender.port), XME_CORE_STATUS_INTERNAL_ERROR);
	#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_directory_addPublication
(
	xme_core_node_nodeId_t publishingNode,
	xme_core_component_t publishingComponent,
	xme_core_component_port_t publishingPort,
	xme_core_topic_t topic,
	xme_core_md_topicMetaDataHandle_t metaData,
	bool onlyLocal
)
{
	xme_hal_table_rowHandle_t handle;
	xme_core_directory_publicationItem_t* item;

	// Parameter check
	XME_ASSERT(XME_CORE_NODE_INVALID_NODE_ID != publishingNode);
	XME_ASSERT(XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT != publishingComponent);
	XME_ASSERT(XME_CORE_COMPONENT_INVALID_COMPONENT_PORT != publishingPort);
	XME_ASSERT(XME_CORE_TOPIC_INVALID_TOPIC != topic);

	XME_LOG
	(
		XME_LOG_DEBUG,
		"DCC_PUB ADD N=%d C=%d P=%d T=%d%s\n",
		publishingNode,
		publishingComponent,
		publishingPort,
		topic,
		onlyLocal ? " LOCAL" : ""
	);

	//Check that no other publication has been already added for component port
	XME_HAL_TABLE_ITERATE
	(
		xme_core_directory_config->publications,
		xme_hal_table_rowHandle_t, handle,
		xme_core_directory_publicationItem_t, publication,
		{
			XME_ASSERT(publication->publisher.node != publishingNode || publication->publisher.component != publishingComponent || publication->publisher.port != publishingPort);
		}
	);

	// Add the publication to the list of publications
	handle = XME_HAL_TABLE_ADD_ITEM(xme_core_directory_config->publications);
	XME_CHECK(0 != handle, XME_CORE_STATUS_OUT_OF_RESOURCES);

	item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_directory_config->publications, handle);
	XME_ASSERT(NULL != item);

	item->publisher.node = publishingNode;
	item->publisher.component = publishingComponent;
	item->publisher.port = publishingPort;
	item->topic = topic;
	item->metaData = metaData;
	item->onlyLocal = onlyLocal;

	#ifdef XME_CORE_DIRECTORY_TYPE_LOCAL
		if (!item->onlyLocal && xme_core_directory_config->connectedToMasterDirectory)
		{
			xme_core_topic_announcementPacket_t announcement;

			announcement.announcementType = XME_CORE_ANNOUNCEMENT_PUBLISH_TOPIC;
			announcement.nodeId = item->publisher.node;
			announcement.componentId = item->publisher.component;
			announcement.portId = item->publisher.port;
			announcement.publishTopic.publicationTopic = item->topic;
			//TODO meta data. See ticket #646
			announcement.publishTopic.onlyLocal = item->onlyLocal;

			// Publish publication
			{
				xme_hal_sharedPtr_t dataHandle = xme_hal_sharedPtr_createFromPointer(sizeof(announcement), &announcement);
				// TODO: Error handling x2! See ticket #721
				xme_core_broker_deliverData(xme_core_directory_config->remoteAnnouncementDataChannel, dataHandle, xme_core_resourceManager_getCurrentComponentId());
				xme_hal_sharedPtr_destroy(dataHandle);
			}
		}
	#endif // #ifdef XME_CORE_DIRECTORY_TYPE_LOCAL

	// Check whether there is a component that subscribes to the respective topic
	XME_HAL_TABLE_ITERATE
	(
		xme_core_directory_config->subscriptions,
		xme_hal_table_rowHandle_t, handle,
		xme_core_directory_subscriptionItem_t, subscriptionItem,
		{
			if (subscriptionItem->topic == item->topic)
			{
				// Check whether publication and subcribtion are not already handled by local directory
				if (subscriptionItem->subscriber.node != item->publisher.node || xme_core_nodeManager_getNodeId() == subscriptionItem->subscriber.node)
				{
					// Check whether the meta data filter accepts the data being sent
					if (xme_core_md_filterAcceptsMetdata(subscriptionItem->metaData, item->metaData))
					{
						// Found a match, construct data channel
						XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_directory_constructDataChannel(item, subscriptionItem), XME_CORE_STATUS_INTERNAL_ERROR);
					}
				}
			}
		}
	);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_directory_removePublication
(
	xme_core_node_nodeId_t publishingNode,
	xme_core_component_t publishingComponent,
	xme_core_component_port_t publishingPort
)
{
	xme_hal_table_rowHandle_t rowHandle;
	xme_core_directory_publicationItem_t* publicationItem;

	// Parameter check
	XME_ASSERT(XME_CORE_NODE_INVALID_NODE_ID != publishingNode);
	XME_ASSERT(XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT != publishingComponent);
	XME_ASSERT(XME_CORE_COMPONENT_INVALID_COMPONENT_PORT != publishingPort);

	XME_LOG
	(
		XME_LOG_DEBUG,
		"DCC_PUB REMOVE N=%d C=%d P=%d\n",
		publishingNode,
		publishingComponent,
		publishingPort
	);

	// Find the corresponding row in the table
	rowHandle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
	XME_HAL_TABLE_GET_NEXT
	(
		xme_core_directory_config->publications,
		xme_hal_table_rowHandle_t, rowHandle,
		xme_core_directory_publicationItem_t, publicationItem,
		publicationItem->publisher.node == publishingNode && publicationItem->publisher.component == publishingComponent && publicationItem->publisher.port == publishingPort
	);
	XME_ASSERT(XME_HAL_TABLE_INVALID_ROW_HANDLE != rowHandle);
	XME_ASSERT(NULL != publicationItem);

	#ifdef XME_CORE_DIRECTORY_TYPE_LOCAL
		if (!publicationItem->onlyLocal && xme_core_directory_config->connectedToMasterDirectory)
		{
			xme_core_topic_announcementPacket_t announcement;

			announcement.announcementType = XME_CORE_ANNOUNCEMENT_UNPUBLISH_TOPIC;
			announcement.nodeId = publishingNode;
			announcement.componentId = publishingComponent;
			announcement.portId = publishingPort;

			// Unpublish publication
			{
				xme_hal_sharedPtr_t dataHandle = xme_hal_sharedPtr_createFromPointer(sizeof(announcement), &announcement);
				// TODO: Error handling x2! See ticket #721
				xme_core_broker_deliverData(xme_core_directory_config->remoteAnnouncementDataChannel, dataHandle, xme_core_resourceManager_getCurrentComponentId());
				xme_hal_sharedPtr_destroy(dataHandle);
			}
		}
	#endif // #ifdef XME_CORE_DIRECTORY_TYPE_LOCAL

	// Check whether there is a component that subscribes to the respective topic to remove the routes
	XME_HAL_TABLE_ITERATE_BEGIN
	(
		xme_core_directory_config->communicationRelations,
		xme_hal_table_rowHandle_t, handle,
		xme_core_directory_communicationRelation_t, communication
	);
	{
		if (communication->source->node == publishingNode && communication->source->component == publishingComponent && communication->source->port == publishingPort)
		{
			#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
				xme_core_directory_deconstructDataChannel(communication->source, communication->sink, communication->localDataChannel);
			#else // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER
				xme_core_dataChannel_t dataChannel;
				//xme_core_directory_communicationRelation_t* communication2;

				if (XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != communication->globalDataChannel)
				{
					dataChannel = communication->globalDataChannel;
				}
				else
				{
					dataChannel = communication->localDataChannel;
				}

				XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_removeLocalSourceRoute(dataChannel), XME_CORE_STATUS_INTERNAL_ERROR);

				// Remove local destination route
				XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_removeLocalDestinationRoute(dataChannel, communication->sink->component, communication->sink->port), XME_CORE_STATUS_INTERNAL_ERROR);
			#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER

			//Remove the communication relation from directory
			XME_CHECK(XME_CORE_STATUS_SUCCESS == XME_HAL_TABLE_REMOVE_ITEM(xme_core_directory_config->communicationRelations, handle), XME_CORE_STATUS_INTERNAL_ERROR);
		}
	}
	XME_HAL_TABLE_ITERATE_END();

	//Remove the publication from directory
	XME_CHECK(XME_CORE_STATUS_SUCCESS == XME_HAL_TABLE_REMOVE_ITEM(xme_core_directory_config->publications, rowHandle), XME_CORE_STATUS_INTERNAL_ERROR);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_directory_addSubscription
(
	xme_core_node_nodeId_t subscribingNode,
	xme_core_component_t subscribingComponent,
	xme_core_component_port_t subscribingPort,
	xme_core_topic_t topic,
	xme_core_md_topicMetaDataHandle_t metaData,
	bool onlyLocal
)
{
	xme_hal_table_rowHandle_t handle;
	xme_core_directory_subscriptionItem_t* item;

	// Parameter check
	XME_ASSERT(XME_CORE_NODE_INVALID_NODE_ID != subscribingNode);
	XME_ASSERT(XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT != subscribingComponent);
	XME_ASSERT(XME_CORE_COMPONENT_INVALID_COMPONENT_PORT != subscribingPort);
	XME_ASSERT(XME_CORE_TOPIC_INVALID_TOPIC != topic);

	XME_LOG
	(
		XME_LOG_DEBUG,
		"DCC_SUB ADD N=%d C=%d P=%d T=%d%s\n",
		subscribingNode,
		subscribingComponent,
		subscribingPort,
		topic,
		onlyLocal ? " LOCAL" : ""
	);

	// Add the subscription to the list of subscriptions
	handle = XME_HAL_TABLE_ADD_ITEM(xme_core_directory_config->subscriptions);
	XME_CHECK(0 != handle, XME_CORE_STATUS_OUT_OF_RESOURCES);

	item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_directory_config->subscriptions, handle);
	XME_ASSERT(NULL != item);

	item->subscriber.node = subscribingNode;
	item->subscriber.component = subscribingComponent;
	item->subscriber.port = subscribingPort;
	item->topic = topic;
	item->metaData = metaData;
	item->onlyLocal = onlyLocal;

	#ifdef XME_CORE_DIRECTORY_TYPE_LOCAL
		if (!item->onlyLocal && xme_core_directory_config->connectedToMasterDirectory)
		{
			xme_core_topic_announcementPacket_t announcement;

			announcement.announcementType = XME_CORE_ANNOUNCEMENT_SUBSCRIBE_TOPIC;
			announcement.nodeId = item->subscriber.node;
			announcement.componentId = item->subscriber.component;
			announcement.portId = item->subscriber.port;
			announcement.subscribeTopic.subscriptionTopic= item->topic;
			//TODO meta data. See ticket #646
			announcement.subscribeTopic.onlyLocal = item->onlyLocal;

			// Publish subscription
			{
				xme_hal_sharedPtr_t dataHandle = xme_hal_sharedPtr_createFromPointer(sizeof(announcement), &announcement);
				// TODO: Error handling x2! See ticket #721
				xme_core_broker_deliverData(xme_core_directory_config->remoteAnnouncementDataChannel, dataHandle, xme_core_resourceManager_getCurrentComponentId());
				xme_hal_sharedPtr_destroy(dataHandle);
			}
		}
	#endif // #ifdef XME_CORE_DIRECTORY_TYPE_LOCAL

	// Check whether there is a component that publishes the respective topic
	XME_HAL_TABLE_ITERATE_BEGIN
	(
		xme_core_directory_config->publications,
		xme_hal_table_rowHandle_t, handle,
		xme_core_directory_publicationItem_t, publicationItem
	);
		if (publicationItem->topic == item->topic)
		{
			// Check whether publication and subcribtion are not already handled by local directory
			if (publicationItem->publisher.node != item->subscriber.node || xme_core_nodeManager_getNodeId() == publicationItem->publisher.node)
			{
				// Check whether the meta data filter accepts the data being sent
				if (xme_core_md_filterAcceptsMetdata(item->metaData, publicationItem->metaData))
				{
					// Found a match, construct data channel
					XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_directory_constructDataChannel(publicationItem, item), XME_CORE_STATUS_INTERNAL_ERROR);
				}
			}
		}
	XME_HAL_TABLE_ITERATE_END();

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_directory_removeSubscription
(
	xme_core_node_nodeId_t subscribingNode,
	xme_core_component_t subscribingComponent,
	xme_core_component_port_t subscribingPort
)
{
	xme_hal_table_rowHandle_t rowHandle;
	xme_core_directory_subscriptionItem_t* subscriptionItem;

	// Parameter check
	XME_ASSERT(XME_CORE_NODE_INVALID_NODE_ID != subscribingNode);
	XME_ASSERT(XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT != subscribingComponent);
	XME_ASSERT(XME_CORE_COMPONENT_INVALID_COMPONENT_PORT != subscribingPort);

	XME_LOG
	(
		XME_LOG_DEBUG,
		"DCC_PUB REMOVE N=%d C=%d P=%d\n",
		subscribingNode,
		subscribingComponent,
		subscribingPort
	);

	// Find the corresponding row in the table
	rowHandle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
	XME_HAL_TABLE_GET_NEXT
	(
		xme_core_directory_config->subscriptions,
		xme_hal_table_rowHandle_t, rowHandle,
		xme_core_directory_subscriptionItem_t, subscriptionItem,
		subscriptionItem->subscriber.node == subscribingNode && subscriptionItem->subscriber.component == subscribingComponent && subscriptionItem->subscriber.port == subscribingPort
	);
	XME_ASSERT(XME_HAL_TABLE_INVALID_ROW_HANDLE != rowHandle);
	XME_ASSERT(NULL != subscriptionItem);

	#ifdef XME_CORE_DIRECTORY_TYPE_LOCAL
		if (!subscriptionItem->onlyLocal && xme_core_directory_config->connectedToMasterDirectory)
		{
			xme_core_topic_announcementPacket_t announcement;

			announcement.announcementType = XME_CORE_ANNOUNCEMENT_UNSUBSCRIBE_TOPIC;
			announcement.nodeId = subscribingNode;
			announcement.componentId = subscribingComponent;
			announcement.portId = subscribingPort;

			// Unpublish subscription
			{
				xme_hal_sharedPtr_t dataHandle = xme_hal_sharedPtr_createFromPointer(sizeof(announcement), &announcement);
				// TODO: Error handling x2! See ticket #721
				xme_core_broker_deliverData(xme_core_directory_config->remoteAnnouncementDataChannel, dataHandle, xme_core_resourceManager_getCurrentComponentId());
				xme_hal_sharedPtr_destroy(dataHandle);
			}
		}
	#endif // #ifdef XME_CORE_DIRECTORY_TYPE_LOCAL

	// Check whether there is a component that publishes the respective topic to remove the routes
	XME_HAL_TABLE_ITERATE_BEGIN
	(
		xme_core_directory_config->communicationRelations,
		xme_hal_table_rowHandle_t, handle,
		xme_core_directory_communicationRelation_t, communication
	);
	{
		if (communication->sink->node == subscribingNode && communication->sink->component == subscribingComponent && communication->sink->port == subscribingPort)
		{
			#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
				xme_core_directory_deconstructDataChannel(communication->source, communication->sink, communication->localDataChannel);
			#else // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER
				xme_core_dataChannel_t dataChannel;
				//xme_core_directory_communicationRelation_t* communication2;

				if (XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != communication->globalDataChannel)
				{
					dataChannel = communication->globalDataChannel;
				}
				else
				{
					dataChannel = communication->localDataChannel;
				}

				// Remove local destination route
				XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_removeLocalDestinationRoute(dataChannel, subscribingComponent, subscribingPort), XME_CORE_STATUS_INTERNAL_ERROR);

				// Remove local source route
				XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_removeLocalSourceRoute(dataChannel), XME_CORE_STATUS_INTERNAL_ERROR);
			#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER

			//Remove the communication relation from directory
			XME_CHECK(XME_CORE_STATUS_SUCCESS == XME_HAL_TABLE_REMOVE_ITEM(xme_core_directory_config->communicationRelations, handle), XME_CORE_STATUS_INTERNAL_ERROR);
		}
	}
	XME_HAL_TABLE_ITERATE_END();

	//Remove the subscription from directory
	XME_CHECK(XME_CORE_STATUS_SUCCESS == XME_HAL_TABLE_REMOVE_ITEM(xme_core_directory_config->subscriptions, rowHandle), XME_CORE_STATUS_INTERNAL_ERROR);

	return XME_CORE_STATUS_SUCCESS;
}

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
)
{
	xme_hal_table_rowHandle_t handle;
	xme_core_directory_requestHandlerItem_t* item;

	// Parameter check
	XME_ASSERT(XME_CORE_NODE_INVALID_NODE_ID != requestHandlingNode);
	XME_ASSERT(XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT != requestHandlingComponent);
	XME_ASSERT(XME_CORE_COMPONENT_INVALID_COMPONENT_PORT != requestHandlingPort);
	XME_ASSERT(XME_CORE_COMPONENT_INVALID_COMPONENT_PORT != responseSendingPort);
	XME_ASSERT(XME_CORE_TOPIC_INVALID_TOPIC != requestTopic);
	XME_ASSERT(XME_CORE_TOPIC_INVALID_TOPIC != responseTopic);

	XME_LOG
	(
		XME_LOG_DEBUG,
		"RR_RQH ADD N=%d C=%d P_RQH=%d T_RQ=%d P_RSS=%d T_RS=%d%s\n",
		requestHandlingNode,
		requestHandlingComponent,
		requestHandlingPort,
		requestTopic,
		responseSendingPort,
		responseTopic,
		onlyLocal ? " LOCAL" : ""
	);

	// Add the server to the list of request handlers
	handle = XME_HAL_TABLE_ADD_ITEM(xme_core_directory_config->requestHandlers);
	XME_CHECK(0 != handle, XME_CORE_STATUS_OUT_OF_RESOURCES);

	item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_directory_config->requestHandlers, handle);
	XME_ASSERT(NULL != item);

	item->requestHandler.node = requestHandlingNode;
	item->requestHandler.component = requestHandlingComponent;
	item->requestHandler.port = requestHandlingPort;
	item->responseSender.node = requestHandlingNode;
	item->responseSender.component = requestHandlingComponent;
	item->responseSender.port = responseSendingPort;
	item->requestTopic = requestTopic;
	item->requestMetaDataFilter = requestMetaDataFilter;
	item->responseTopic = responseTopic;
	item->responseMetaData = responseMetaData;
	item->onlyLocal = onlyLocal;

	#ifdef XME_CORE_DIRECTORY_TYPE_LOCAL
		if (!item->onlyLocal && xme_core_directory_config->connectedToMasterDirectory)
		{
			xme_core_topic_announcementPacket_t announcement;

			announcement.announcementType = XME_CORE_ANNOUNCEMENT_PUBLISH_REQUEST_HANDLER;
			announcement.nodeId = item->requestHandler.node;
			announcement.componentId = item->requestHandler.component;
			announcement.portId = item->requestHandler.port;
			announcement.publishRequestHandler.responseSendingPort = item->responseSender.port;
			announcement.publishRequestHandler.requestTopic= item->requestTopic;
			//TODO meta data. See ticket #646 
			announcement.publishRequestHandler.responseTopic= item->responseTopic;
			//TODO meta data. See ticket #646
			announcement.publishRequestHandler.onlyLocal = onlyLocal;

			// Publish request handler
			{
				xme_hal_sharedPtr_t dataHandle = xme_hal_sharedPtr_createFromPointer(sizeof(announcement), &announcement);
				// TODO: Error handling x2! See ticket #721
				xme_core_broker_deliverData(xme_core_directory_config->remoteAnnouncementDataChannel, dataHandle, xme_core_resourceManager_getCurrentComponentId());
				xme_hal_sharedPtr_destroy(dataHandle);
			}
		}
	#endif // #ifdef XME_CORE_DIRECTORY_TYPE_LOCAL

	// Check whether there is a client that might emit requests matching the server
	XME_HAL_TABLE_ITERATE
	(
		xme_core_directory_config->requests,
		xme_hal_table_rowHandle_t, handle,
		xme_core_directory_requestItem_t, requestSenderItem,
		{
			if (requestSenderItem->requestTopic == item->requestTopic && requestSenderItem->responseTopic == item->responseTopic)
			{
				// Check whether request publication and request handling are not already handled by local directory
				if (requestSenderItem->requester.node != item->requestHandler.node || xme_core_nodeManager_getNodeId() == requestSenderItem->requester.node)
				{
					// Check whether the meta data filter accepts the data being sent
					if (xme_core_md_filterAcceptsMetdata(item->requestMetaDataFilter, requestSenderItem->requestMetaData) && xme_core_md_filterAcceptsMetdata(requestSenderItem->responseMetaDataFilter, item->responseMetaData))
					{
						//Check if requester accepts more than one reply or does not have already a request handler assigned
						if (!requestSenderItem->manyResponses)
						{
							xme_hal_table_rowHandle_t handle2;
							xme_core_directory_communicationRelation_t* communication;

							handle2 = XME_HAL_TABLE_INVALID_ROW_HANDLE;
							XME_HAL_TABLE_GET_NEXT
							(
								xme_core_directory_config->communicationRelations,
								xme_hal_table_rowHandle_t, handle2,
								xme_core_directory_communicationRelation_t, communication,
								communication->source->node == requestSenderItem->requester.node && communication->source->component == requestSenderItem->requester.component  &&communication->source->port == requestSenderItem->requester.port
							);

							if (XME_HAL_TABLE_INVALID_ROW_HANDLE != handle2)
							{
								continue;
							}
						}

						// Found a match, construct request response data channels
						XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_directory_constructRRDataChannels(requestSenderItem, item), XME_CORE_STATUS_INTERNAL_ERROR);
					}
				}
			}
		}
	);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_directory_removeRequestHandler
(
	xme_core_node_nodeId_t requestHandlingNode,
	xme_core_component_t requestHandlingComponent,
	xme_core_component_port_t requestHandlingPort
)
{
	xme_hal_table_rowHandle_t rowHandle;
	xme_core_directory_requestHandlerItem_t* requestHandlerItem;

	// Parameter check
	XME_ASSERT(XME_CORE_NODE_INVALID_NODE_ID != requestHandlingNode);
	XME_ASSERT(XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT != requestHandlingComponent);
	XME_ASSERT(XME_CORE_COMPONENT_INVALID_COMPONENT_PORT != requestHandlingPort);

	XME_LOG
	(
		XME_LOG_DEBUG,
		"RR_RQH REMOVE N=%d C=%d P_RQH=%d\n",
		requestHandlingNode,
		requestHandlingComponent,
		requestHandlingPort
	);

	// Find the corresponding row in the table
	rowHandle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
	XME_HAL_TABLE_GET_NEXT
	(
		xme_core_directory_config->requestHandlers,
		xme_hal_table_rowHandle_t, rowHandle,
		xme_core_directory_requestHandlerItem_t, requestHandlerItem,
		requestHandlerItem->requestHandler.node == requestHandlingNode && requestHandlerItem->requestHandler.component == requestHandlingComponent && requestHandlerItem->requestHandler.port == requestHandlingPort
	);
	XME_ASSERT(XME_HAL_TABLE_INVALID_ROW_HANDLE != rowHandle);
	XME_ASSERT(NULL != requestHandlerItem);

	#ifdef XME_CORE_DIRECTORY_TYPE_LOCAL
		if (!requestHandlerItem->onlyLocal && xme_core_directory_config->connectedToMasterDirectory)
		{
			xme_core_topic_announcementPacket_t announcement;

			announcement.announcementType = XME_CORE_ANNOUNCEMENT_UNSUBSCRIBE_TOPIC;
			announcement.nodeId = requestHandlingNode;
			announcement.componentId = requestHandlingComponent;
			announcement.portId = requestHandlingPort;

			// Unpublish request handler
			{
				xme_hal_sharedPtr_t dataHandle = xme_hal_sharedPtr_createFromPointer(sizeof(announcement), &announcement);
				// TODO: Error handling x2! See ticket #721
				xme_core_broker_deliverData(xme_core_directory_config->remoteAnnouncementDataChannel, dataHandle, xme_core_resourceManager_getCurrentComponentId());
				xme_hal_sharedPtr_destroy(dataHandle);
			}
		}
	#endif // #ifdef XME_CORE_DIRECTORY_TYPE_LOCAL

	// Check whether there is a client that sends requests the respective topic to remove the routes
	XME_HAL_TABLE_ITERATE_BEGIN
	(
		xme_core_directory_config->communicationRelations,
		xme_hal_table_rowHandle_t, handle,
		xme_core_directory_communicationRelation_t, communication
	);
	{
		if ((communication->sink->node == requestHandlingNode && communication->sink->component == requestHandlingComponent && communication->sink->port == requestHandlingPort) ||
			(communication->source->node == requestHandlerItem->responseSender.node && communication->source->component == requestHandlerItem->responseSender.component && communication->source->port == requestHandlerItem->responseSender.port))
		{
			// TODO: Establish new RR route if response handler is unpublished and a requester relys on him (#903)

			#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
				xme_core_directory_deconstructDataChannel(communication->source, communication->sink, communication->localDataChannel);
			#else // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER
				xme_core_dataChannel_t dataChannel;

				if (XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != communication->globalDataChannel)
				{
					dataChannel = communication->globalDataChannel;
				}
				else
				{
					dataChannel = communication->localDataChannel;
				}

				if (communication->sink->node == requestHandlingNode && communication->sink->component == requestHandlingComponent && communication->sink->port == requestHandlingPort)
				{
					// Remove local source route
					XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_removeLocalSourceRoute(dataChannel), XME_CORE_STATUS_INTERNAL_ERROR);

					// Remove local destination route
					XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_removeLocalDestinationRoute(dataChannel, communication->sink->component, communication->sink->port), XME_CORE_STATUS_INTERNAL_ERROR);
				}
				else
				{
					// Remove local source route
					XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_removeLocalSourceRoute(dataChannel), XME_CORE_STATUS_INTERNAL_ERROR);

					// Remove local destination route
					XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_removeLocalDestinationRoute(dataChannel, communication->source->component, communication->source->port), XME_CORE_STATUS_INTERNAL_ERROR);
				}
			#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER

			//Remove the communication relation from directory
			XME_CHECK(XME_CORE_STATUS_SUCCESS == XME_HAL_TABLE_REMOVE_ITEM(xme_core_directory_config->communicationRelations, handle), XME_CORE_STATUS_INTERNAL_ERROR);
		}
	}
	XME_HAL_TABLE_ITERATE_END();

	//Remove the request handler from directory
	XME_CHECK(XME_CORE_STATUS_SUCCESS == XME_HAL_TABLE_REMOVE_ITEM(xme_core_directory_config->requestHandlers, rowHandle), XME_CORE_STATUS_INTERNAL_ERROR);

	return XME_CORE_STATUS_SUCCESS;
}

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
)
{
	xme_hal_table_rowHandle_t handle;
	xme_core_directory_requestItem_t* item;
	bool communicationRelationEstablished;

	// Parameter check
	XME_ASSERT(XME_CORE_NODE_INVALID_NODE_ID != requestingNode);
	XME_ASSERT(XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT != requestingComponent);
	XME_ASSERT(XME_CORE_COMPONENT_INVALID_COMPONENT_PORT != requestingPort);
	XME_ASSERT(XME_CORE_COMPONENT_INVALID_COMPONENT_PORT != responseHandlingPort);
	XME_ASSERT(XME_CORE_TOPIC_INVALID_TOPIC != requestTopic);
	XME_ASSERT(XME_CORE_TOPIC_INVALID_TOPIC != responseTopic);

	XME_LOG
	(
		XME_LOG_DEBUG,
		"RR_RQS ADD N=%d C=%d P_RQS=%d T_RQ=%d P_RSH=%d T_RS=%d%s%s\n",
		requestingNode,
		requestingComponent,
		requestingPort,
		requestTopic,
		responseHandlingPort,
		responseTopic,
		onlyLocal ? " LOCAL" : "",
		manyResponses ? " MANY" : " !MANY"
	);

	// Add the server to the list of requests
	handle = XME_HAL_TABLE_ADD_ITEM(xme_core_directory_config->requests);
	XME_CHECK(0 != handle, XME_CORE_STATUS_OUT_OF_RESOURCES);

	item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_directory_config->requests, handle);
	XME_ASSERT(NULL != item);

	item->requester.node = requestingNode;
	item->requester.component = requestingComponent;
	item->requester.port = requestingPort;
	item->responseHandler.node = requestingNode;
	item->responseHandler.component = requestingComponent;
	item->responseHandler.port = responseHandlingPort;
	item->requestTopic = requestTopic;
	item->requestMetaData = requestMetaData;
	item->responseTopic = responseTopic;
	item->responseMetaDataFilter = responseMetaDataFilter;
	item->manyResponses = manyResponses;
	item->onlyLocal = onlyLocal;

	// Check whether there is a server that might accepts requests matching the client
	communicationRelationEstablished = false;
	XME_HAL_TABLE_ITERATE
	(
		xme_core_directory_config->requestHandlers,
		xme_hal_table_rowHandle_t, handle,
		xme_core_directory_requestHandlerItem_t, requestHandlerItem,
		{
			if (requestHandlerItem->requestTopic == item->requestTopic && requestHandlerItem->responseTopic == item->responseTopic)
			{
				// Check whether publication and subcribtion are not already handled by local directory
				if (requestHandlerItem->requestHandler.node != item->requester.node || xme_core_nodeManager_getNodeId() == requestHandlerItem->requestHandler.node)
				{
					// Check whether the meta data filter accepts the data being sent
					if (xme_core_md_filterAcceptsMetdata(requestHandlerItem->requestMetaDataFilter, item->requestMetaData) && xme_core_md_filterAcceptsMetdata(item->responseMetaDataFilter, requestHandlerItem->responseMetaData))
					{
						// Found a match, construct request response data channels
						XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_directory_constructRRDataChannels(item, requestHandlerItem), XME_CORE_STATUS_INTERNAL_ERROR);
						communicationRelationEstablished = true;

						// Check if this request handler is the only one
						if (!item->manyResponses)
						{
							break;
						}
					}
				}
			}
		}
	);

	#ifdef XME_CORE_DIRECTORY_TYPE_LOCAL
		if (!item->onlyLocal && xme_core_directory_config->connectedToMasterDirectory)
		{
			if (item->manyResponses || !communicationRelationEstablished)
			{
				xme_core_topic_announcementPacket_t announcement;

				announcement.announcementType = XME_CORE_ANNOUNCEMENT_PUBLISH_REQUEST;
				announcement.nodeId = item->requester.node;
				announcement.componentId = item->requester.component;
				announcement.portId = item->requester.port;
				announcement.publishRequest.responseHandlingPort = item->responseHandler.port;
				announcement.publishRequest.requestTopic = item->requestTopic;
				//TODO meta data. See ticket #646
				announcement.publishRequest.responseTopic = item->responseTopic;
				//TODO meta data. See ticket #646
				announcement.publishRequest.manyResponses = item->manyResponses;
				announcement.publishRequest.onlyLocal = onlyLocal;

				// Publish request
				{
					xme_hal_sharedPtr_t dataHandle = xme_hal_sharedPtr_createFromPointer(sizeof(announcement), &announcement);
					// TODO: Error handling x2! See ticket #721
					xme_core_broker_deliverData(xme_core_directory_config->remoteAnnouncementDataChannel, dataHandle, xme_core_resourceManager_getCurrentComponentId());
					xme_hal_sharedPtr_destroy(dataHandle);
				}
			}
		}
	#endif // #ifdef XME_CORE_DIRECTORY_TYPE_LOCAL

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_directory_removeRequest
(
	xme_core_node_nodeId_t requestingNode,
	xme_core_component_t requestingComponent,
	xme_core_component_port_t requestingPort
)
{
	xme_hal_table_rowHandle_t rowHandle;
	xme_core_directory_requestItem_t* requestItem;

	// Parameter check
	XME_ASSERT(XME_CORE_NODE_INVALID_NODE_ID != requestingNode);
	XME_ASSERT(XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT != requestingComponent);
	XME_ASSERT(XME_CORE_COMPONENT_INVALID_COMPONENT_PORT != requestingPort);

	XME_LOG
	(
		XME_LOG_DEBUG,
		"RR_RQS REMOVE N=%d C=%d P_RQS=%d\n",
		requestingNode,
		requestingComponent,
		requestingPort
	);

	// Find the corresponding row in the table
	rowHandle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
	XME_HAL_TABLE_GET_NEXT
	(
		xme_core_directory_config->requests,
		xme_hal_table_rowHandle_t, rowHandle,
		xme_core_directory_requestItem_t, requestItem,
		requestItem->requester.node == requestingNode && requestItem->requester.component == requestingComponent && requestItem->requester.port == requestingPort
	);
	XME_ASSERT(XME_HAL_TABLE_INVALID_ROW_HANDLE != rowHandle);
	XME_ASSERT(NULL != requestItem);

	#ifdef XME_CORE_DIRECTORY_TYPE_LOCAL
		if (!requestItem->onlyLocal && xme_core_directory_config->connectedToMasterDirectory)
		{
			xme_core_topic_announcementPacket_t announcement;

			announcement.announcementType = XME_CORE_ANNOUNCEMENT_UNSUBSCRIBE_TOPIC;
			announcement.nodeId = requestingNode;
			announcement.componentId = requestingComponent;
			announcement.portId = requestingPort;

			// Unpublish request
			{
				xme_hal_sharedPtr_t dataHandle = xme_hal_sharedPtr_createFromPointer(sizeof(announcement), &announcement);
				// TODO: Error handling x2! See ticket #721
				xme_core_broker_deliverData(xme_core_directory_config->remoteAnnouncementDataChannel, dataHandle, xme_core_resourceManager_getCurrentComponentId());
				xme_hal_sharedPtr_destroy(dataHandle);
			}
		}
	#endif // #ifdef XME_CORE_DIRECTORY_TYPE_LOCAL

	// Check whether there is a server that handels requests to the respective topic to remove the routes
	XME_HAL_TABLE_ITERATE_BEGIN
	(
		xme_core_directory_config->communicationRelations,
		xme_hal_table_rowHandle_t, handle,
		xme_core_directory_communicationRelation_t, communication
	);
	{
		if ((communication->source->node == requestingNode && communication->source->component == requestingComponent && communication->source->port == requestingPort) ||
			(communication->sink->node == requestItem->responseHandler.node && communication->sink->component == requestItem->responseHandler.component && communication->sink->port == requestItem->responseHandler.port))
		{
			#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
				xme_core_directory_deconstructDataChannel(communication->source, communication->sink, communication->localDataChannel);
			#else // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER
				xme_core_dataChannel_t dataChannel;

				if (XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != communication->globalDataChannel)
				{
					dataChannel = communication->globalDataChannel;
				}
				else
				{
					dataChannel = communication->localDataChannel;
				}

				if (communication->source->node == requestingNode && communication->source->component == requestingComponent && communication->source->port == requestingPort)
				{
					// Remove local source route
					XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_removeLocalSourceRoute(dataChannel), XME_CORE_STATUS_INTERNAL_ERROR);

					// Remove local destination route
					XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_removeLocalDestinationRoute(dataChannel, communication->sink->component, communication->sink->port), XME_CORE_STATUS_INTERNAL_ERROR);
				}
				else
				{
					// Remove local source route
					XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_removeLocalSourceRoute(dataChannel), XME_CORE_STATUS_INTERNAL_ERROR);

					// Remove local destination route
					XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_removeLocalDestinationRoute(dataChannel, communication->source->component, communication->source->port), XME_CORE_STATUS_INTERNAL_ERROR);
				}
			#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER

			//Remove the communication relation from directory
			XME_CHECK(XME_CORE_STATUS_SUCCESS == XME_HAL_TABLE_REMOVE_ITEM(xme_core_directory_config->communicationRelations, handle), XME_CORE_STATUS_INTERNAL_ERROR);
		}
	}
	XME_HAL_TABLE_ITERATE_END();

	//Remove the request from directory
	XME_CHECK(XME_CORE_STATUS_SUCCESS == XME_HAL_TABLE_REMOVE_ITEM(xme_core_directory_config->requests, rowHandle), XME_CORE_STATUS_INTERNAL_ERROR);

	return XME_CORE_STATUS_SUCCESS;
}

static
void
xme_core_directory_receiveManagementChannelsToEdgeNode
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
)
{
	xme_core_topic_managementChannelsPacket_t* managementChannels;
	xme_core_node_nodeId_t nodeId;

	// Extract information required for the corresponding table
	managementChannels = (xme_core_topic_managementChannelsPacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

	// Update directory internal tables according to newly assigned
	// node ID. Since we do not assign a new node ID when we already
	// have one, this update is only performed when only local routes
	// can exist. This means that we can directly replace all node
	// identifiers that are present until this time with the newly
	// assigned node ID.
	nodeId = xme_core_nodeManager_getNodeId();
	XME_ASSERT_NORVAL(XME_CORE_NODE_INVALID_NODE_ID != nodeId);
	XME_ASSERT_NORVAL(XME_CORE_NODE_LOCAL_NODE_ID != nodeId);

	// TODO: This should probably be replaced by the following approach (See ticket #736):
	//       In local directories, do not keep an indication of which
	//       node a publication or subscription belongs to -- always
	//       assume that it's a local publication/subscription.
	//       In master directories, remember which node a request
	//       originated from (or have it set to the local ID if it
	//       originates from the node where the master directory is on).

	#ifdef XME_CORE_DIRECTORY_TYPE_MASTER
		XME_HAL_TABLE_ITERATE_BEGIN
		(
			xme_core_directory_config->endpointLinks,
			xme_hal_table_rowHandle_t, handle,
			xme_core_directory_endpointLink_t, endpointLink
		);
		{
			if (XME_CORE_NODE_LOCAL_NODE_ID == endpointLink->endpoint.node)
			{
				endpointLink->endpoint.node = nodeId;
			}
		}
		XME_HAL_TABLE_ITERATE_END();
	#endif // #ifdef XME_CORE_DIRECTORY_TYPE_MASTER

	XME_HAL_TABLE_ITERATE
	(
		xme_core_directory_config->publications,
		xme_hal_table_rowHandle_t, handle,
		xme_core_directory_publicationItem_t, publicationItem,
		{
			XME_ASSERT_NORVAL(XME_ASSERT_NO_SIDE_EFFECTS(XME_CORE_NODE_LOCAL_NODE_ID == publicationItem->publisher.node || xme_core_nodeManager_getNodeId() == publicationItem->publisher.node));
			publicationItem->publisher.node = nodeId;
		}
	);
	XME_HAL_TABLE_ITERATE
	(
		xme_core_directory_config->subscriptions,
		xme_hal_table_rowHandle_t, handle,
		xme_core_directory_subscriptionItem_t, subscriptionItem,
		{
			XME_ASSERT_NORVAL(XME_ASSERT_NO_SIDE_EFFECTS(XME_CORE_NODE_LOCAL_NODE_ID == subscriptionItem->subscriber.node || xme_core_nodeManager_getNodeId() == subscriptionItem->subscriber.node));
			subscriptionItem->subscriber.node = nodeId;
		}
	);
	XME_HAL_TABLE_ITERATE
	(
		xme_core_directory_config->requestHandlers,
		xme_hal_table_rowHandle_t, handle,
		xme_core_directory_requestHandlerItem_t, requestHandlerItem,
		{
			XME_ASSERT_NORVAL(XME_ASSERT_NO_SIDE_EFFECTS(XME_CORE_NODE_LOCAL_NODE_ID == requestHandlerItem->requestHandler.node || xme_core_nodeManager_getNodeId() == requestHandlerItem->requestHandler.node));
			XME_ASSERT_NORVAL(XME_ASSERT_NO_SIDE_EFFECTS(XME_CORE_NODE_LOCAL_NODE_ID == requestHandlerItem->responseSender.node || xme_core_nodeManager_getNodeId() == requestHandlerItem->responseSender.node));
			requestHandlerItem->requestHandler.node = nodeId;
			requestHandlerItem->responseSender.node = nodeId;
		}
	);
	XME_HAL_TABLE_ITERATE
	(
		xme_core_directory_config->requests,
		xme_hal_table_rowHandle_t, handle,
		xme_core_directory_requestItem_t, requesterItem,
		{
			XME_ASSERT_NORVAL(XME_ASSERT_NO_SIDE_EFFECTS(XME_CORE_NODE_LOCAL_NODE_ID == requesterItem->requester.node || xme_core_nodeManager_getNodeId() == requesterItem->requester.node));
			XME_ASSERT_NORVAL(XME_ASSERT_NO_SIDE_EFFECTS(XME_CORE_NODE_LOCAL_NODE_ID == requesterItem->responseHandler.node || xme_core_nodeManager_getNodeId() == requesterItem->responseHandler.node));
			requesterItem->requester.node = nodeId;
			requesterItem->responseHandler.node = nodeId;
		}
	);

	#ifdef XME_CORE_DIRECTORY_TYPE_LOCAL
		XME_ASSERT_NORVAL(XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != managementChannels->remoteAnnouncementDataChannel);
		XME_ASSERT_NORVAL(XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != managementChannels->remoteModifyRoutingTableDataChannel);

		XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_directory_connectToMasterDirectory(managementChannels->interfaceId, managementChannels->remoteAnnouncementDataChannel, managementChannels->remoteModifyRoutingTableDataChannel), );
	#else // #ifdef XME_CORE_DIRECTORY_TYPE_LOCAL
		XME_ASSERT_NORVAL(XME_CORE_DATACHANNEL_INVALID_DATACHANNEL == managementChannels->remoteAnnouncementDataChannel);
		XME_ASSERT_NORVAL(XME_CORE_DATACHANNEL_INVALID_DATACHANNEL == managementChannels->remoteModifyRoutingTableDataChannel);
	#endif // #ifdef XME_CORE_DIRECTORY_TYPE_LOCAL
}

static
void
xme_core_directory_receiveManagementChannelsToNewNode
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
)
{
	xme_core_topic_managementChannelsPacket_t* managementChannels;

	// Extract information required for the corresponding table
	managementChannels = (xme_core_topic_managementChannelsPacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

	XME_ASSERT_NORVAL(XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != managementChannels->remoteAnnouncementDataChannel);
	XME_ASSERT_NORVAL(XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != managementChannels->remoteModifyRoutingTableDataChannel);

	// TODO: Create own remote announcement data channel for each not or find a way to establish channel multiple times (#894)
	XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addInboundRoute(managementChannels->remoteAnnouncementDataChannel, managementChannels->interfaceId), );
	XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addOutboundRoute(managementChannels->remoteModifyRoutingTableDataChannel, managementChannels->interfaceId), );
}

static
void
xme_core_directory_receiveAnnouncementCallback
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
)
{
	xme_core_topic_announcementPacket_t* announcement;

	xme_core_node_nodeId_t node;
	xme_core_component_t component;
	xme_core_component_port_t port;
	xme_core_topic_t topic;
	xme_core_component_port_t port2;
	xme_core_md_topicMetaDataHandle_t metaData;
	xme_core_topic_t responseTopic;
	xme_core_md_topicMetaDataHandle_t responseMetaData;
	bool manyResponses;
	bool onlyLocal;

	// Extract information required for the corresponding table
	announcement = (xme_core_topic_announcementPacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

	//XME_LOG(XME_LOG_VERBOSE, "Received announcement (type: %d)\n", announcement->announcementType);

	// Make sure that node ID assignments that happended in between the
	// announcement being sent and processed are considered appropriately.
	node = announcement->nodeId;
	if (XME_CORE_NODE_LOCAL_NODE_ID == node)
	{
		node = xme_core_nodeManager_getNodeId();
	}
	component = announcement->componentId;
	port = announcement->portId;

	// Add/remove the information to/from the corresponding table
	switch (announcement->announcementType)
	{
		case XME_CORE_ANNOUNCEMENT_PUBLISH_TOPIC:
			topic = announcement->publishTopic.publicationTopic;
			metaData = XME_CORE_MD_EMPTY_META_DATA;
			onlyLocal = announcement->publishTopic.onlyLocal;
			xme_core_directory_addPublication(node, component, port, topic, metaData, onlyLocal);
			break;
		case XME_CORE_ANNOUNCEMENT_UNPUBLISH_TOPIC:
			xme_core_directory_removePublication(node, component, port);
			break;
		case XME_CORE_ANNOUNCEMENT_SUBSCRIBE_TOPIC:
			topic = announcement->subscribeTopic.subscriptionTopic;
			metaData = XME_CORE_MD_EMPTY_META_DATA;
			onlyLocal = announcement->subscribeTopic.onlyLocal;
			xme_core_directory_addSubscription(node, component, port, topic, metaData, onlyLocal);
			break;
		case XME_CORE_ANNOUNCEMENT_UNSUBSCRIBE_TOPIC:
			xme_core_directory_removeSubscription(node, component, port);
			break;
		case XME_CORE_ANNOUNCEMENT_PUBLISH_REQUEST:
			port2 = announcement->publishRequest.responseHandlingPort;
			topic = announcement->publishRequest.requestTopic;
			metaData = XME_CORE_MD_EMPTY_META_DATA;
			responseTopic = announcement->publishRequest.responseTopic;
			responseMetaData = XME_CORE_MD_EMPTY_META_DATA;
			manyResponses = announcement->publishRequest.manyResponses;
			onlyLocal = announcement->publishRequest.onlyLocal;
			xme_core_directory_addRequest(node, component, port, port2, topic, metaData, responseTopic, responseMetaData, manyResponses, onlyLocal);
			break;
		case XME_CORE_ANNOUNCEMENT_UNPUBLISH_REQUEST:
			xme_core_directory_removeRequest(node, component, port);
			break;
		case XME_CORE_ANNOUNCEMENT_PUBLISH_REQUEST_HANDLER:
			port2 = announcement->publishRequestHandler.responseSendingPort;
			topic = announcement->publishRequestHandler.requestTopic;
			metaData = XME_CORE_MD_EMPTY_META_DATA;
			responseTopic = announcement->publishRequestHandler.responseTopic;
			responseMetaData = XME_CORE_MD_EMPTY_META_DATA;
			onlyLocal = announcement->publishRequestHandler.onlyLocal;
			xme_core_directory_addRequestHandler(node, component, port, port2, topic, metaData, responseTopic, responseMetaData, onlyLocal);
			break;
		case XME_CORE_ANNOUNCEMENT_UNPUBLISH_REQUEST_HANDLER:
			xme_core_directory_removeRequestHandler(node, component, port);
			break;
	}
}

#ifdef XME_CORE_DIRECTORY_TYPE_LOCAL
	static
	xme_core_status_t
	xme_core_directory_connectToMasterDirectory
	(
		xme_core_interface_interfaceId_t interfaceId,
		xme_core_dataChannel_t remoteAnnouncementDataChannel,
		xme_core_dataChannel_t remoteModifyRoutingTableDataChannel
	)
	{
		xme_core_topic_announcementPacket_t announcement;

		xme_core_directory_config->connectedToMasterDirectory = true;
		
		xme_core_directory_config->remoteAnnouncementDataChannel = remoteAnnouncementDataChannel;
		xme_core_directory_config->remoteModifyRoutingTableDataChannel = remoteModifyRoutingTableDataChannel;

		XME_LOG
		(
			XME_LOG_DEBUG,
			"Connected to Master Directory (RADC=%d, RMRTDC=%d)\n",
			xme_core_directory_config->remoteAnnouncementDataChannel,
			xme_core_directory_config->remoteModifyRoutingTableDataChannel
		);

		// Add local source and outbound route for remote announcement channel
		XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addLocalSourceRoute(xme_core_directory_config->remoteAnnouncementDataChannel, xme_core_resourceManager_getCurrentComponentId(), xme_core_resourceManager_getPublicationPort(xme_core_directory_config->remoteAnnouncementHandle)), XME_CORE_STATUS_INTERNAL_ERROR);
		XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addOutboundRoute(remoteAnnouncementDataChannel, interfaceId), XME_CORE_STATUS_INTERNAL_ERROR);

		// Add local destination and inbound route for remote modify routing table channel
		XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addLocalDestinationRoute(xme_core_directory_config->remoteModifyRoutingTableDataChannel, xme_core_resourceManager_getCurrentComponentId(), xme_core_resourceManager_getSubscriptionPort(xme_core_directory_config->remoteModifyRoutingTableHandle)), XME_CORE_STATUS_INTERNAL_ERROR);
		XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_routingTable_addInboundRoute(remoteModifyRoutingTableDataChannel, interfaceId), XME_CORE_STATUS_INTERNAL_ERROR);

		XME_LOG
		(
			XME_LOG_DEBUG,
			"Sending local DCC_PUB items to Master Directory (count=%d)\n",
			XME_HAL_TABLE_ITEM_COUNT(xme_core_directory_config->publications)
		);

		XME_HAL_TABLE_ITERATE
		(
			xme_core_directory_config->publications,
			xme_hal_table_rowHandle_t, handle,
			xme_core_directory_publicationItem_t, publicationItem,
			{
				XME_LOG
				(
					XME_LOG_DEBUG,
					"[DCC_PUB #%d] N=%d C=%d P=%d T=%d%s\n",
					handle,
					publicationItem->publisher.node,
					publicationItem->publisher.component,
					publicationItem->publisher.port,
					publicationItem->topic,
					publicationItem->onlyLocal ? " LOCAL" : ""
				);

				if (!publicationItem->onlyLocal)
				{
					announcement.announcementType = XME_CORE_ANNOUNCEMENT_PUBLISH_TOPIC;
					announcement.nodeId = publicationItem->publisher.node;
					announcement.componentId = publicationItem->publisher.component;
					announcement.portId = publicationItem->publisher.port;
					announcement.publishTopic.publicationTopic = publicationItem->topic;
					//TODO meta data. See ticket #646
					announcement.publishTopic.onlyLocal = publicationItem->onlyLocal;

					// Publish publication
					{
						xme_hal_sharedPtr_t dataHandle = xme_hal_sharedPtr_createFromPointer(sizeof(announcement), &announcement);
						// TODO: Error handling x2! See ticket #721
						xme_core_broker_deliverData(xme_core_directory_config->remoteAnnouncementDataChannel, dataHandle, xme_core_resourceManager_getCurrentComponentId());
						xme_hal_sharedPtr_destroy(dataHandle);
					}
				}
			}
		);

		XME_LOG
		(
			XME_LOG_DEBUG,
			"Sending local DCC_SUB items to Master Directory (count=%d)\n",
			XME_HAL_TABLE_ITEM_COUNT(xme_core_directory_config->subscriptions)
		);

		XME_HAL_TABLE_ITERATE
		(
			xme_core_directory_config->subscriptions,
			xme_hal_table_rowHandle_t, handle,
			xme_core_directory_subscriptionItem_t, subscriptionItem,
			{
				XME_LOG
				(
					XME_LOG_DEBUG,
					"[DCC_SUB #%d] N=%d C=%d P=%d T=%d%s\n",
					handle,
					subscriptionItem->subscriber.node,
					subscriptionItem->subscriber.component,
					subscriptionItem->subscriber.port,
					subscriptionItem->topic,
					subscriptionItem->onlyLocal ? " LOCAL" : ""
				);

				if (!subscriptionItem->onlyLocal)
				{
					announcement.announcementType = XME_CORE_ANNOUNCEMENT_SUBSCRIBE_TOPIC;
					announcement.nodeId = subscriptionItem->subscriber.node;
					announcement.componentId = subscriptionItem->subscriber.component;
					announcement.portId = subscriptionItem->subscriber.port;
					announcement.subscribeTopic.subscriptionTopic = subscriptionItem->topic;
					//TODO meta data. See ticket #646
					announcement.subscribeTopic.onlyLocal = subscriptionItem->onlyLocal;

					// Publish subscription
					{
						xme_hal_sharedPtr_t dataHandle = xme_hal_sharedPtr_createFromPointer(sizeof(announcement), &announcement);
						// TODO: Error handling x2! See ticket #721
						xme_core_broker_deliverData(xme_core_directory_config->remoteAnnouncementDataChannel, dataHandle, xme_core_resourceManager_getCurrentComponentId());
						xme_hal_sharedPtr_destroy(dataHandle);
					}
				}
			}
		);

		XME_LOG
		(
			XME_LOG_DEBUG,
			"Sending local RR_RQH items to Master Directory (count=%d)\n",
			XME_HAL_TABLE_ITEM_COUNT(xme_core_directory_config->requestHandlers)
		);

		XME_HAL_TABLE_ITERATE
		(
			xme_core_directory_config->requestHandlers,
			xme_hal_table_rowHandle_t, handle,
			xme_core_directory_requestHandlerItem_t, requestHandlerItem,
			{
				XME_LOG
				(
					XME_LOG_DEBUG,
					"[RR_RQH #%d] N=%d C=%d P_RQH=%d T_RQ=%d P_RSS=%d T_RS=%d%s\n",
					handle,
					requestHandlerItem->requestHandler.node,
					requestHandlerItem->requestHandler.component,
					requestHandlerItem->requestHandler.port,
					requestHandlerItem->requestTopic,
					requestHandlerItem->responseSender.port,
					requestHandlerItem->responseTopic,
					requestHandlerItem->onlyLocal ? " LOCAL" : ""
				);

				if (!requestHandlerItem->onlyLocal)
				{
					announcement.announcementType = XME_CORE_ANNOUNCEMENT_PUBLISH_REQUEST_HANDLER;
					announcement.nodeId = requestHandlerItem->requestHandler.node;
					announcement.componentId = requestHandlerItem->requestHandler.component;
					announcement.portId = requestHandlerItem->requestHandler.port;
					announcement.publishRequestHandler.requestTopic = requestHandlerItem->requestTopic;
					//TODO meta data. See ticket #646
					announcement.publishRequestHandler.responseSendingPort = requestHandlerItem->responseSender.port;
					announcement.publishRequestHandler.responseTopic = requestHandlerItem->responseTopic;
					//TODO meta data. See ticket #646
					announcement.publishRequestHandler.onlyLocal = requestHandlerItem->onlyLocal;

					// Publish request handler
					{
						xme_hal_sharedPtr_t dataHandle = xme_hal_sharedPtr_createFromPointer(sizeof(announcement), &announcement);
						// TODO: Error handling x2! See ticket #721
						xme_core_broker_deliverData(xme_core_directory_config->remoteAnnouncementDataChannel, dataHandle, xme_core_resourceManager_getCurrentComponentId());
						xme_hal_sharedPtr_destroy(dataHandle);
					}
				}
			}
		);

		XME_LOG
		(
			XME_LOG_DEBUG,
			"Sending local RR_RQS items to Master Directory (count=%d)\n",
			XME_HAL_TABLE_ITEM_COUNT(xme_core_directory_config->requestHandlers)
		);

		XME_HAL_TABLE_ITERATE
		(
			xme_core_directory_config->requests,
			xme_hal_table_rowHandle_t, handle,
			xme_core_directory_requestItem_t, requesterItem,
			{
				XME_LOG
				(
					XME_LOG_DEBUG,
					"[RR_RQS #%d] N=%d C=%d P_RQS=%d T_RQ=%d P_RSH=%d T_RS=%d%s%s\n",
					handle,
					requesterItem->requester.node,
					requesterItem->requester.component,
					requesterItem->requester.port,
					requesterItem->requestTopic,
					requesterItem->responseHandler.port,
					requesterItem->responseTopic,
					requesterItem->onlyLocal ? " LOCAL" : "",
					requesterItem->manyResponses ? " MANY" : " !MANY"
				);

				if (!requesterItem->onlyLocal)
				{
					//Check if requester accepts more than one reply or does not have already a request handler assigned
					if (!requesterItem->manyResponses)
					{
						xme_hal_table_rowHandle_t handle2;
						xme_core_directory_communicationRelation_t* communication;

						handle2 = XME_HAL_TABLE_INVALID_ROW_HANDLE;
						XME_HAL_TABLE_GET_NEXT
						(
							xme_core_directory_config->communicationRelations,
							xme_hal_table_rowHandle_t, handle2,
							xme_core_directory_communicationRelation_t, communication,
							communication->source->node == requesterItem->requester.node && communication->source->component == requesterItem->requester.component  &&communication->source->port == requesterItem->requester.port
						);

						if (XME_HAL_TABLE_INVALID_ROW_HANDLE != handle2)
						{
							XME_LOG
							(
								XME_LOG_DEBUG,
								"This request is resolved locally!\n"
							);

							continue;
						}
					}

					announcement.announcementType = XME_CORE_ANNOUNCEMENT_PUBLISH_REQUEST;
					announcement.nodeId = requesterItem->requester.node;
					announcement.componentId = requesterItem->requester.component;
					announcement.portId = requesterItem->requester.port;
					announcement.publishRequest.requestTopic = requesterItem->requestTopic;
					//TODO meta data. See ticket #646
					announcement.publishRequest.responseTopic = requesterItem->responseTopic;
					//TODO meta data. See ticket #646
					announcement.publishRequest.responseHandlingPort = requesterItem->responseHandler.port;
					announcement.publishRequest.onlyLocal = requesterItem->onlyLocal;

					// Publish request
					{
						xme_hal_sharedPtr_t dataHandle = xme_hal_sharedPtr_createFromPointer(sizeof(announcement), &announcement);
						// TODO: Error handling x2! See ticket #721
						xme_core_broker_deliverData(xme_core_directory_config->remoteAnnouncementDataChannel, dataHandle, xme_core_resourceManager_getCurrentComponentId());
						xme_hal_sharedPtr_destroy(dataHandle);
					}
				}
			}
		);

		return XME_CORE_STATUS_SUCCESS;
	}

	static void
	xme_core_directory_receiveRemoteModifyRoutingTableCallback
	(
		xme_hal_sharedPtr_t dataHandle,
		void* userData
	)
	{
		xme_core_topic_modifyRoutingTablePacket_t* modifyRoutingTable;

		xme_core_dataChannel_t dataChannel;
		xme_core_component_t component;
		xme_core_component_port_t port;
		xme_core_interface_interfaceId_t networkInterface;
		xme_core_dataChannel_t newDataChannel;

		// Extract information required for the corresponding table
		modifyRoutingTable = (xme_core_topic_modifyRoutingTablePacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

		//XME_LOG(XME_LOG_VERBOSE, "Received modify routing table (type: %d)\n", modifyRoutingTable->modifyRoutingTableType);

		dataChannel = modifyRoutingTable->dataChannel;

		// TODO: Find a better way to prevent that global routes are installed additionally to the local routes and thereby trigger doublication of transmitted data (ticket #679)
		switch (modifyRoutingTable->modifyRoutingTableType)
		{
			case XME_CORE_MODIFY_ROUTING_TABLE_TYPE_ADD_LOCAL_SOURCE_ROUTE:
				component = modifyRoutingTable->localSourceRoute.component;
				port = modifyRoutingTable->localSourceRoute.port;

				XME_LOG
				(
					XME_LOG_DEBUG,
					"RMRT ADD LOCAL_SOURCE DC=0x%X C=%d P=%d\n",
					dataChannel, component, port
				);

				xme_core_routingTable_addLocalSourceRoute(dataChannel, component, port);

				{
					xme_hal_table_rowHandle_t handle;
					xme_core_directory_communicationRelation_t* communication;

					handle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
					XME_HAL_TABLE_GET_NEXT
					(
						xme_core_directory_config->communicationRelations,
						xme_hal_table_rowHandle_t, handle,
						xme_core_directory_communicationRelation_t, communication,
						component == communication->source->component && port == communication->source->port
					);

					if (XME_HAL_TABLE_INVALID_ROW_HANDLE != handle)
					{
						xme_core_directory_dataChannelMapping_t* mapping;

						communication->globalDataChannel = dataChannel;
						xme_core_routingTable_removeLocalSourceRoute(communication->localDataChannel);
						xme_core_routingTable_addLocalSourceRoute(dataChannel, component, port);

						handle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
						XME_HAL_TABLE_GET_NEXT
						(
							xme_core_directory_config->dataChannelMappings,
							xme_hal_table_rowHandle_t, handle,
							xme_core_directory_dataChannelMapping_t, mapping,
							mapping->dataChannel == communication->localDataChannel
						);
						if (XME_HAL_TABLE_INVALID_ROW_HANDLE != handle)
						{
							mapping->dataChannel = dataChannel;
						}
					}

				}
				break;
			case XME_CORE_MODIFY_ROUTING_TABLE_TYPE_REMOVE_LOCAL_SOURCE_ROUTE:
				component = modifyRoutingTable->localSourceRoute.component;
				port = modifyRoutingTable->localSourceRoute.port;

				XME_LOG
				(
					XME_LOG_DEBUG,
					"RMRT REMOVE LOCAL_SOURCE DC=0x%X C=%d P=%d\n",
					dataChannel, component, port
				);

				xme_core_routingTable_removeLocalSourceRoute(dataChannel);
				break;
			case XME_CORE_MODIFY_ROUTING_TABLE_TYPE_ADD_LOCAL_DESTINATION_ROUTE:
				component = modifyRoutingTable->localDestinationRoute.component;
				port = modifyRoutingTable->localDestinationRoute.port;

				XME_LOG
				(
					XME_LOG_DEBUG,
					"RMRT ADD LOCAL_DESTINATION DC=0x%X C=%d P=%d\n",
					dataChannel, component, port
				);

				xme_core_routingTable_addLocalDestinationRoute(dataChannel, component, port);

				{
					xme_hal_table_rowHandle_t handle;
					xme_core_directory_communicationRelation_t* communication;

					handle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
					XME_HAL_TABLE_GET_NEXT
					(
						xme_core_directory_config->communicationRelations,
						xme_hal_table_rowHandle_t, handle,
						xme_core_directory_communicationRelation_t, communication,
						component == communication->sink->component && port == communication->sink->port
					);

					if (XME_HAL_TABLE_INVALID_ROW_HANDLE != handle)
					{
						xme_core_directory_dataChannelMapping_t* mapping;

						communication->globalDataChannel = dataChannel;
						xme_core_routingTable_removeLocalDestinationRoute(communication->localDataChannel, component, port);
						xme_core_routingTable_addLocalDestinationRoute(communication->globalDataChannel, component, port);

						handle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
						XME_HAL_TABLE_GET_NEXT
						(
							xme_core_directory_config->dataChannelMappings,
							xme_hal_table_rowHandle_t, handle,
							xme_core_directory_dataChannelMapping_t, mapping,
							mapping->dataChannel == communication->localDataChannel
						);
						if (XME_HAL_TABLE_INVALID_ROW_HANDLE != handle)
						{
							mapping->dataChannel = dataChannel;
						}
					}
				}
				break;
			case XME_CORE_MODIFY_ROUTING_TABLE_TYPE_REMOVE_LOCAL_DESTINATION_ROUTE:
				component = modifyRoutingTable->localDestinationRoute.component;
				port = modifyRoutingTable->localDestinationRoute.port;

				XME_LOG
				(
					XME_LOG_DEBUG,
					"RMRT REMOVE LOCAL_DESTINATION DC=0x%X C=%d P=%d\n",
					dataChannel, component, port
				);

				xme_core_routingTable_removeLocalDestinationRoute(dataChannel, component, port);
				break;
			case XME_CORE_MODIFY_ROUTING_TABLE_TYPE_ADD_OUTBOUND_ROUTE:
				networkInterface = modifyRoutingTable->outboundRoute.networkInterface;

				XME_LOG
				(
					XME_LOG_DEBUG,
					"RMRT ADD OUTBOUND DC=0x%X I=%d\n",
					dataChannel, networkInterface
				);

				xme_core_routingTable_addOutboundRoute(dataChannel, networkInterface);
				break;
			case XME_CORE_MODIFY_ROUTING_TABLE_TYPE_REMOVE_OUTBOUND_ROUTE:
				networkInterface = modifyRoutingTable->outboundRoute.networkInterface;

				XME_LOG
				(
					XME_LOG_DEBUG,
					"RMRT REMOVE OUTBOUND DC=0x%X I=%d\n",
					dataChannel, networkInterface
				);

				xme_core_routingTable_removeOutboundRoute(dataChannel, networkInterface);
				break;
			case XME_CORE_MODIFY_ROUTING_TABLE_TYPE_ADD_INBOUND_ROUTE:
				networkInterface = modifyRoutingTable->inboundRoute.networkInterface;

				XME_LOG
				(
					XME_LOG_DEBUG,
					"RMRT ADD INBOUND DC=0x%X I=%d\n",
					dataChannel, networkInterface
				);

				xme_core_routingTable_addInboundRoute(dataChannel, networkInterface);
				break;
			case XME_CORE_MODIFY_ROUTING_TABLE_TYPE_REMOVE_INBOUND_ROUTE:
				networkInterface = modifyRoutingTable->inboundRoute.networkInterface;

				XME_LOG
				(
					XME_LOG_DEBUG,
					"RMRT REMOVE INBOUND DC=0x%X I=%d\n",
					dataChannel, networkInterface
				);

				xme_core_routingTable_removeInboundRoute(dataChannel, networkInterface);
				break;
			case XME_CORE_MODIFY_ROUTING_TABLE_TYPE_ADD_TRANSLATION_ROUTE:
				newDataChannel = modifyRoutingTable->translationRoute.newDataChannel;

				XME_LOG
				(
					XME_LOG_DEBUG,
					"RMRT ADD TRANSLATION DC=0x%X NDC=%d\n",
					dataChannel, newDataChannel
				);

				xme_core_routingTable_addTranslationRoute(dataChannel, newDataChannel);
				break;
			case XME_CORE_MODIFY_ROUTING_TABLE_TYPE_REMOVE_TRANSLATION_ROUTE:
				newDataChannel = modifyRoutingTable->translationRoute.newDataChannel;

				XME_LOG
				(
					XME_LOG_DEBUG,
					"RMRT REMOVE TRANSLATION DC=0x%X NDC=%d\n",
					dataChannel, newDataChannel
				);

				xme_core_routingTable_removeTranslationRoute(dataChannel, newDataChannel);
				break;
		}
	}
#else // #ifdef XME_CORE_DIRECTORY_TYPE_LOCAL
	static
	xme_core_rr_responseStatus_t
	xme_core_directory_receiveNewNodeRequest
	(
		xme_core_topic_t requestTopic,
		void* requestData,
		xme_core_rr_responseInstanceHandle_t responseInstanceHandle,
		void* responseData,
		uint16_t* responseSize,
		xme_hal_time_interval_t responseTimeoutMs,
		void* userData
	)
	{
		xme_core_topic_newNodeRequestData_t* newNodeRequest;
		xme_core_topic_newNodeResponseData_t* newNodeResponse;
		xme_hal_table_rowHandle_t nodeHandle;
		xme_core_directory_node_t* node;
		xme_core_dataChannel_t remoteAnnouncementDataChannel;
		xme_core_dataChannel_t remoteModifyRoutingTableDataChannel;
		xme_core_directory_neighbor_t neighbor;

		XME_ASSERT_RVAL(XME_CORE_TOPIC_NEW_NODE_REQUEST == requestTopic, XME_CORE_RR_STATUS_SERVER_ERROR);
		XME_ASSERT_RVAL(responseSize != NULL, XME_CORE_RR_STATUS_SERVER_ERROR);

		{
			uint16_t oldSize = *responseSize;

			// Set response size
			*responseSize = sizeof(xme_core_topic_newNodeResponseData_t);

			XME_CHECK(oldSize >= sizeof(xme_core_topic_newNodeResponseData_t), XME_CORE_RR_STATUS_BUFFER_TOO_SMALL);
		}

		// Set response size
		*responseSize = sizeof(xme_core_topic_newNodeResponseData_t);

		newNodeRequest = (xme_core_topic_newNodeRequestData_t*)requestData;
		newNodeResponse = (xme_core_topic_newNodeResponseData_t*)responseData;

		// Check if node has not already been added
		nodeHandle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
		XME_HAL_TABLE_GET_NEXT
		(
			xme_core_directory_config->nodes,
			xme_hal_table_rowHandle_t, nodeHandle,
			xme_core_directory_node_t, node,
			node->node == newNodeRequest->newNodeId
		);
		if (XME_HAL_TABLE_INVALID_ROW_HANDLE == nodeHandle)
		{
			// XME_CORE_NODE_INVALID_NODE_ID as edgeNodeId means that the new node
			// is the local node. In this case, no remote announcement and modify
			// routing table data channels will be created.

			// Get data channel for communication path
			if (XME_CORE_NODE_INVALID_NODE_ID != newNodeRequest->edgeNodeId)
			{
				remoteAnnouncementDataChannel = xme_core_directory_config->remoteAnnouncementDataChannel;
				remoteModifyRoutingTableDataChannel = xme_core_directory_getDataChannel(XME_CORE_TOPIC_REMOTE_MODIFY_ROUTING_TABLE, XME_CORE_MD_EMPTY_META_DATA, true);
				XME_ASSERT_RVAL(XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != remoteModifyRoutingTableDataChannel, XME_CORE_RR_STATUS_SERVER_ERROR);
			}
			else
			{
				remoteAnnouncementDataChannel = XME_CORE_DATACHANNEL_INVALID_DATACHANNEL;
				remoteModifyRoutingTableDataChannel = XME_CORE_DATACHANNEL_INVALID_DATACHANNEL;
			}

			// Update network topology
			xme_core_directory_addNode(newNodeRequest->newNodeId, remoteAnnouncementDataChannel, remoteModifyRoutingTableDataChannel);

			if (XME_CORE_NODE_INVALID_NODE_ID != newNodeRequest->edgeNodeId)
			{
				// TODO: Remove when ticket #561 has been implemented.
				XME_HAL_TABLE_ITERATE_BEGIN
				(
					xme_core_directory_config->nodes,
					xme_hal_table_rowHandle_t, nodeHandle,
					xme_core_directory_node_t, node
				);
				{
					if (node->node != newNodeRequest->newNodeId)
					{
						neighbor.sendInterface = newNodeRequest->newNodeInterface;
						neighbor.node = node->node;
						neighbor.receiveInterface = 1; // TODO: replace with real interface ID! (cmp. ticket #536) See #547
						xme_core_directory_updateNeigborhood(newNodeRequest->newNodeId, &neighbor, 1, false);

						neighbor.sendInterface = 1; // TODO: replace with real interface ID! (cmp. ticket #536) See #547
						neighbor.node = newNodeRequest->newNodeId;
						neighbor.receiveInterface = newNodeRequest->newNodeInterface;
						xme_core_directory_updateNeigborhood(node->node, &neighbor, 1, false);
					}
				}
				XME_HAL_TABLE_ITERATE_END();

				XME_CHECK(XME_CORE_STATUS_SUCCESS == xme_core_directory_constructManagementChannels(xme_core_nodeManager_getNodeId(), newNodeRequest->newNodeId, remoteModifyRoutingTableDataChannel), XME_CORE_RR_STATUS_SERVER_ERROR);
			}
		}
		else
		{
			remoteAnnouncementDataChannel = node->remoteAnnouncementDataChannel;
			remoteModifyRoutingTableDataChannel = node->remoteModifyRoutingTableDataChannel;
		}

		// Prepare response
		newNodeResponse->remoteAnnouncementDataChannel = remoteAnnouncementDataChannel;
		newNodeResponse->remoteModifyRoutingTableDataChannel = remoteModifyRoutingTableDataChannel;

		return XME_CORE_RR_STATUS_SUCCESS;
	}
#endif // #ifdef XME_CORE_DIRECTORY_TYPE_LOCAL

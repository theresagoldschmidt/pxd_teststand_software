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
 *         Data centric communication (DCC).
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/dcc.h"

#include "xme/core/broker.h"
#include "xme/core/defines.h"
#include "xme/core/resourceManager.h"
#include "xme/core/directory.h"
#include "xme/core/nodeManager.h"

#include "xme/hal/sharedPtr.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
/**
 * \def    XME_CORE_DCC_ASSERT_REFUSE_INVALID_DCC_TOPIC
 *
 * \brief  Returns from the current function with the given return value if the
 *         given topic is not a valid DCC topic.
 *
 * \param  topic Topic, which shall be check.
 * \param  rval Return value to pass to the caller of the current function
 *         in case the topic is no valid DCC topic.
 */
#define XME_CORE_DCC_ASSERT_REFUSE_INVALID_DCC_TOPIC(topic, rval) \
	do { \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_INVALID_TOPIC != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_NEW_NODE_REQUEST != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_NEW_NODE_RESPONSE != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_LOGIN_REQUEST != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_LOGIN_RESPONSE != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_MODBUS_REQUEST != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_MODBUS_RESPONSE != topic, rval); \
	} while (0)

/**
 * \def    XME_CORE_DCC_ASSERT_REFUSE_INTERNAL_PUBLICATION_TOPIC
 *
 * \brief  Returns from the current function with the given return value if the
 *         given topic is an internal publication topic.
 *
 * \param  topic Topic, which shall be check.
 * \param  rval Return value to pass to the caller of the current function
 *         in case the topic is an internal publication topic.
 */
#define XME_CORE_DCC_ASSERT_REFUSE_INTERNAL_PUBLICATION_TOPIC(topic, rval) \
	do { \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_LOCAL_ANNOUNCEMENT != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_REMOTE_ANNOUNCEMENT != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_REMOTE_MODIFY_ROUTING_TABLE != topic, rval); \
	} while (0)

/**
 * \def    XME_CORE_DCC_ASSERT_REFUSE_INTERNAL_SUBSCRIPTION_TOPIC
 *
 * \brief  Returns from the current function with the given return value if the
 *         given topic is an internal subscription topic.
 *
 * \param  topic Topic, which shall be check.
 * \param  rval Return value to pass to the caller of the current function
 *         in case the topic is an internal subscription topic.
 */
#define XME_CORE_DCC_ASSERT_REFUSE_INTERNAL_SUBSCRIPTION_TOPIC(topic, rval) \
	do { \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_LOCAL_ANNOUNCEMENT != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_REMOTE_ANNOUNCEMENT != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_REMOTE_MODIFY_ROUTING_TABLE != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_MANAGEMENT_CHANNELS_TO_EDGE_NODE != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_MANAGEMENT_CHANNELS_TO_NEW_NODE != topic, rval); \
	} while (0)

/******************************************************************************/
/***   Helper functions                                                     ***/
/******************************************************************************/
/**
 * \brief  Makes the runtime system aware that the calling component intends
 *         to publish (i.e., send) data of the given topic. In contrast to
 *         xme_core_dcc_publishTopic(), this function will not send an
 *         announcement to the directory.
 *
 *         This function does not indicate when data of the given topic are
 *         sent. This can happen at any point in time after the call to this
 *         function. When a component does not publish the topic any more, the
 *         xme_core_dcc_unpublishTopicWithoutAnnouncement() function should be
 *         called.
 *
 * \param  topic Topic to publish/announce.
 * \param  topicMetaDataHandle Handle of the meta data to associate with the
 *         topic. These can be informational meta data (e.g., topographic
 *         information) or quality of service parameters.
 * \param  demandCallback Function to be called by the runtime system when the
 *         demand to generate the given data changes. This is an optimization
 *         measure used to decide whether data whose generation is very
 *         expensive (regarding computation time, energy consumption or
 *         resource allication) should actually be generated. Furthermore,
 *         the runtime system might call this function to optimize data
 *         generation, for example to announce the optimal frequency at which
 *         data are to be generated. Specifying NULL for this parameter is
 *         allowed and indicates that the announced data will always be
 *         generated with the properties specified in the meta data handle.
 *         If a non-NULL value is specified, the runtime system will call the
 *         given function after the component has been initialized to specify
 *         the intended behavior.
 *         Note that this concept slightly breaks the semantics of data driven
 *         communication, where components usually have a "fire & forget"
 *         philosophy. However, for some applications, a hybrid approach is
 *         necessary to ensure system performance.
 *
 * \return Returns a non-zero publication handle for providing data of the
 *         given topic on success and XME_CORE_DCC_INVALID_PUBLICATION_HANDLE
 *         on error.
 */
xme_core_dcc_publicationHandle_t
_xme_core_dcc_publishTopicWithoutAnnouncement
(
	xme_core_topic_t topic,
	xme_core_md_topicMetaDataHandle_t topicMetaDataHandle,
	xme_core_dcc_demandCallback_t demandCallback
)
{
	xme_core_dcc_publicationHandle_t handle;
	xme_core_component_port_t port;
	xme_core_resourceManager_publicationItem_t* item;
	xme_core_component_t currentComponentId;

	XME_CORE_DCC_ASSERT_REFUSE_INVALID_DCC_TOPIC(topic, XME_CORE_DCC_INVALID_PUBLICATION_HANDLE);

	// Determine the calling component
	currentComponentId = xme_core_resourceManager_getCurrentComponentId();

	// Only components can publish topics
	XME_CHECK
	(
		XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT != currentComponentId,
		XME_CORE_DCC_INVALID_PUBLICATION_HANDLE
	);

	// Check input parameters
	XME_CHECK
	(
		XME_CORE_TOPIC_INVALID_TOPIC != topic,
		XME_CORE_DCC_INVALID_PUBLICATION_HANDLE
	);

	// Verify topic meta data handle. It is valid for the meta data handle to
	// be XME_CORE_DCC_INVALID_TOPIC_META_DATA_HANDLE (=0), in which case no
	// meta data is used.
	if (XME_CORE_MD_EMPTY_META_DATA != topicMetaDataHandle)
	{
		XME_CHECK
		(
			xme_core_md_isTopicMetaDataHandleValid(topicMetaDataHandle),
			XME_CORE_DCC_INVALID_PUBLICATION_HANDLE
		);
	}

	// Allocate a unique publication handle
	XME_CHECK
	(
		XME_HAL_TABLE_INVALID_ROW_HANDLE !=
		(
			handle = (xme_core_dcc_publicationHandle_t)XME_HAL_TABLE_ADD_ITEM(xme_core_resourceManager_config.localPublications)
		),
		XME_CORE_DCC_INVALID_PUBLICATION_HANDLE
	);

	// Allocate a port number
	XME_CHECK_REC
	(
		XME_CORE_COMPONENT_INVALID_COMPONENT_PORT !=
		(
			port = xme_core_resourceManager_addPublicationPort(currentComponentId)
		),
		XME_CORE_DCC_INVALID_PUBLICATION_HANDLE,
		{
			XME_HAL_TABLE_REMOVE_ITEM(xme_core_resourceManager_config.localPublications, (xme_hal_table_rowHandle_t)handle);
		}
	);

	item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localPublications, handle);
	item->publishingComponent = currentComponentId;
	item->publishingPort = port;
	item->publishedTopic = topic;
	item->topicMetaDataHandle = topicMetaDataHandle;

	return handle;
}

/**
 * \brief  Makes the runtime system aware that the calling component
 *         discontinues emission of the given topic. In contrast to
 *         xme_core_dcc_unpublishTopic(), this function will not send an
 *         announcement to the directory.
 *
 * \param  publicationHandle Handle of the topic that is to be
 *         unpublished/discontinued.
 * \param  publishingComponent Component corresponding to topic to unpublish.
 * \param  publishingPort Port corresponding to topic to unpublish.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the topic has been successfully
 *            discontinued. In this case, publicationHandle is now an invalid
 *            handle and should be discarded.
 *          - XME_CORE_STATUS_PERMISSION_DENIED if the calling component is
 *            not allowed to perform this action.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the given publication handle
 *            was invalid.
 */
xme_core_status_t
_xme_core_dcc_unpublishTopicWithoutAnnouncement
(
	xme_core_dcc_publicationHandle_t publicationHandle,
	xme_core_component_t* publishingComponent,
	xme_core_component_port_t* publishingPort
)
{
	xme_core_component_t currentComponentId;
	xme_core_resourceManager_publicationItem_t* item;

	// Retrieve the publication descriptor
	XME_CHECK
	(
		NULL != (item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localPublications, publicationHandle)),
		XME_CORE_STATUS_INVALID_HANDLE
	);

	// Determine the calling component
	currentComponentId = xme_core_resourceManager_getCurrentComponentId();

	// Only the component that published a port can unpublish it
	XME_CHECK
	(
		item->publishingComponent == currentComponentId,
		XME_CORE_STATUS_PERMISSION_DENIED
	);

	// Remove the port
	{
		xme_core_status_t rval = xme_core_resourceManager_removePort(currentComponentId, item->publishingPort);
		XME_ASSERT(XME_CORE_STATUS_SUCCESS == rval);
	}

	//Save information to send unregister announcement
	if (NULL != publishingComponent)
	{
		*publishingComponent = item->publishingComponent;
	}
	if (NULL != publishingPort)
	{
		*publishingPort = item->publishingPort;
	}

	// Remove the publication
	{
		xme_core_status_t rval = XME_HAL_TABLE_REMOVE_ITEM(xme_core_resourceManager_config.localPublications, (xme_hal_table_rowHandle_t)publicationHandle);
		XME_ASSERT(XME_CORE_STATUS_SUCCESS == rval);
	}

	return XME_CORE_STATUS_SUCCESS;
}

/**
 * \brief  Subscribes the calling component to the given topic.
 *         In contrast to xme_core_dcc_subscribeTopic(), this function will
 *         not send an announcement to the directory.
 *         The topicFilter parameter can be used to specify criteria that have
 *         to be met for the data associated to the topic to be relevant to the
 *         receiver. Whenever a transmission passes the filter, the specified
 *         callback function will be invoked and the given userData will be
 *         passed to the callback function along with the data received.
 *
 * \param  topic Topic to subscribe to.
 * \param  topicFilter Handle pointing to filter criteria that have to be met
 *         for the data to be relevant to the caller.
 * \param  callback Callback function to invoke when new data arrive.
 * \param  userData User-specified data to be passed to the callback function.
 *
 * \return Returns a non-zero subscription handle for receiving data of the
 *         given topic on success and XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE
 *         on error.
 */
xme_core_dcc_subscriptionHandle_t
_xme_core_dcc_subscribeTopicWithoutAnnouncement
(
	xme_core_topic_t topic,
	xme_core_md_topicMetaDataHandle_t topicFilter,
	xme_core_dcc_receiveTopicCallback_t callback,
	void* userData
)
{
	xme_core_dcc_subscriptionHandle_t handle;
	xme_core_component_port_t port;
	xme_core_resourceManager_subscriptionItem_t* item;
	xme_core_component_t currentComponentId;

	XME_CORE_DCC_ASSERT_REFUSE_INVALID_DCC_TOPIC(topic, XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE);

	// Determine the calling component
	currentComponentId = xme_core_resourceManager_getCurrentComponentId();

	// Only components can subscribe to topics
	XME_CHECK
	(
		XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT != currentComponentId,
		XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE
	);

	// Check input parameters
	XME_CHECK
	(
		XME_CORE_TOPIC_INVALID_TOPIC != topic,
		XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE
	);

	// Verify topic meta data handle. It is valid for the meta data handle to
	// be XME_CORE_DCC_INVALID_TOPIC_META_DATA_HANDLE (=0), in which case no
	// meta data is used.
	if (XME_CORE_MD_EMPTY_META_DATA != topicFilter)
	{
		XME_CHECK
		(
			xme_core_md_isTopicMetaDataHandleValid(topicFilter),
			XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE
		);
	}

	// Allocate a unique publication handle
	XME_CHECK
	(
		XME_HAL_TABLE_INVALID_ROW_HANDLE !=
		(
			handle = (xme_core_dcc_subscriptionHandle_t)XME_HAL_TABLE_ADD_ITEM(xme_core_resourceManager_config.localSubscriptions)
		),
		XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE
	);

	// Allocate a port number
	XME_CHECK_REC
	(
		XME_CORE_COMPONENT_INVALID_COMPONENT_PORT !=
		(
			port = xme_core_resourceManager_addSubscriptionPort(currentComponentId, callback, userData)
		),
		XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE,
		{
			XME_HAL_TABLE_REMOVE_ITEM(xme_core_resourceManager_config.localSubscriptions, (xme_hal_table_rowHandle_t)handle);
		}
	);

	item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localSubscriptions, handle);
	item->subscribingComponent = currentComponentId;
	item->subscribingPort = port;
	item->subscribedTopic = topic;
	item->metaDataFilterHandle = topicFilter;

	return handle;
}

/**
 * \brief  Unsubscribes the calling component from the given topic.
 *         In contrast to xme_core_dcc_unsubscribeTopic(), this function will
 *         not send an announcement to the directory.
 *         The parameters passed to this function must meet the ones specified
 *         in the call to xme_core_dcc_subscribeTopic() that was used to
 *         subscribe to the respective topic.
 *
 * \note   Note that unsubscriptions do not have to be performed as part of
 *         component destruction. Any topic subscriptions of destroyed
 *         components will automatically be canceled.
 *
 * \param  subscriptionHandle Subscription handle describing the topic to
 *         unsubscribe from.
 * \param  subscribeingComponent Component corresponding to topic to unsubsribe.
 * \param  subscribeingPort Port corresponding to topic to unsubscribe.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the topic has been successfully
 *            unsubscribed from. In this case, subscriptionHandle is now an
 *            invalid handle and should be discarded.
 *          - XME_CORE_STATUS_PERMISSION_DENIED if the calling component is
 *            not allowed to perform this action.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the given subscription handle
 *            was invalid.
 */
xme_core_status_t
_xme_core_dcc_unsubscribeTopicWithoutAnnouncement
(
	xme_core_dcc_subscriptionHandle_t subscriptionHandle,
	xme_core_component_t* subscribingComponent,
	xme_core_component_port_t* subscribingPort
)
{
	xme_core_component_t currentComponentId;
	xme_core_resourceManager_subscriptionItem_t* item;

	// Retrieve the subscription descriptor
	XME_CHECK
	(
		NULL != (item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localSubscriptions, subscriptionHandle)),
		XME_CORE_STATUS_INVALID_HANDLE
	);

	// Determine the calling component
	currentComponentId = xme_core_resourceManager_getCurrentComponentId();

	// Only the component that published a port can unpublish it
	XME_CHECK
	(
		item->subscribingComponent == currentComponentId,
		XME_CORE_STATUS_PERMISSION_DENIED
	);

	// Remove the port
	{
		xme_core_status_t rval = xme_core_resourceManager_removePort(currentComponentId, item->subscribingPort);
		XME_ASSERT(XME_CORE_STATUS_SUCCESS == rval);
	}

	//Save information to send unregister announcement
	if (NULL != subscribingComponent)
	{
		*subscribingComponent = item->subscribingComponent;
	}
	if (NULL != subscribingPort)
	{
		*subscribingPort = item->subscribingPort;
	}

	// Remove the publication
	{
		xme_core_status_t rval = XME_HAL_TABLE_REMOVE_ITEM(xme_core_resourceManager_config.localSubscriptions, (xme_hal_table_rowHandle_t)subscriptionHandle);
		XME_ASSERT(XME_CORE_STATUS_SUCCESS == rval);
	}

	return XME_CORE_STATUS_SUCCESS;
}

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_core_dcc_init()
{
	// Nothing to do

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_core_dcc_fini()
{
	// Nothing to do
}

xme_core_dcc_publicationHandle_t
xme_core_dcc_publishTopic
(
	xme_core_topic_t topic,
	xme_core_md_topicMetaDataHandle_t topicMetaDataHandle,
	bool onlyLocal,
	xme_core_dcc_demandCallback_t demandCallback
)
{
	xme_core_resourceManager_publicationItem_t* item;
	xme_core_dcc_publicationHandle_t publicationHandle;

	XME_CORE_DCC_ASSERT_REFUSE_INTERNAL_PUBLICATION_TOPIC(topic, XME_CORE_DCC_INVALID_PUBLICATION_HANDLE);

	publicationHandle =
		_xme_core_dcc_publishTopicWithoutAnnouncement
		(
			topic,
			topicMetaDataHandle,
			demandCallback
		);
	XME_CHECK(XME_CORE_DCC_INVALID_PUBLICATION_HANDLE != publicationHandle, XME_CORE_DCC_INVALID_PUBLICATION_HANDLE);

	item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localPublications, publicationHandle);
	XME_ASSERT_RVAL(NULL != item, XME_CORE_DCC_INVALID_PUBLICATION_HANDLE);

	// Build and send a port announcement
	{
		xme_hal_sharedPtr_t dataHandle;
		xme_core_topic_announcementPacket_t* announcement;

		// TODO: Error handling! See ticket #721
		dataHandle = xme_hal_sharedPtr_create(sizeof(xme_core_topic_announcementPacket_t));
		announcement = (xme_core_topic_announcementPacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

		announcement->announcementType = XME_CORE_ANNOUNCEMENT_PUBLISH_TOPIC;
		announcement->nodeId = xme_core_nodeManager_getNodeId();
		announcement->componentId = item->publishingComponent;
		announcement->portId = item->publishingPort;
		announcement->publishTopic.publicationTopic = item->publishedTopic;
		// TODO: meta data. See ticket #646
		announcement->publishTopic.onlyLocal = onlyLocal;

		// TODO: Error handling! See ticket #721
		xme_core_broker_deliverData(xme_core_resourceManager_config.localAnnouncementDataChannel, dataHandle, item->publishingComponent);

		xme_hal_sharedPtr_destroy(dataHandle);
	}

	// Return the unique publication handle
	return publicationHandle;
}

xme_core_status_t
xme_core_dcc_unpublishTopic
(
	xme_core_dcc_publicationHandle_t publicationHandle
)
{
	xme_core_component_t publishingComponent;
	xme_core_component_port_t publishingPort;

	{
		xme_core_status_t rval = _xme_core_dcc_unpublishTopicWithoutAnnouncement(publicationHandle, &publishingComponent, &publishingPort);
		XME_CHECK(XME_CORE_STATUS_SUCCESS == rval, rval);
	}

	// Build and send a port announcement
	{
		xme_hal_sharedPtr_t dataHandle;
		xme_core_topic_announcementPacket_t* announcement;

		// TODO: Error handling! See ticket #721
		dataHandle = xme_hal_sharedPtr_create(sizeof(xme_core_topic_announcementPacket_t));
		announcement = (xme_core_topic_announcementPacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

		announcement->announcementType = XME_CORE_ANNOUNCEMENT_UNPUBLISH_TOPIC;
		announcement->nodeId = xme_core_nodeManager_getNodeId();
		announcement->componentId = publishingComponent;
		announcement->portId = publishingPort;

		// TODO: Error handling! See ticket #721
		xme_core_broker_deliverData(xme_core_resourceManager_config.localAnnouncementDataChannel, dataHandle, publishingComponent);

		xme_hal_sharedPtr_destroy(dataHandle);
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_component_t
xme_core_dcc_getComponentFromSubscriptionHandle
(
	xme_core_dcc_subscriptionHandle_t subscriptionHandle
)
{
	xme_core_resourceManager_subscriptionItem_t* item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localSubscriptions, subscriptionHandle);

	XME_CHECK(NULL != item, XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT);

	return item->subscribingComponent;
}

xme_core_component_port_t
xme_core_dcc_getPortFromSubscriptionHandle
(
	xme_core_dcc_subscriptionHandle_t subscriptionHandle
)
{
	xme_core_resourceManager_subscriptionItem_t* item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localSubscriptions, subscriptionHandle);

	XME_CHECK(NULL != item, XME_CORE_COMPONENT_INVALID_COMPONENT_PORT);

	return item->subscribingPort;
}

xme_core_component_t
xme_core_dcc_getComponentFromPublicationHandle
(
	xme_core_dcc_publicationHandle_t publicationHandle
)
{
	xme_core_resourceManager_publicationItem_t* item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localPublications, publicationHandle);

	XME_CHECK(NULL != item, XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT);

	return item->publishingComponent;
}

xme_core_component_port_t
xme_core_dcc_getPortFromPublicationHandle
(
	xme_core_dcc_publicationHandle_t publicationHandle
)
{
	xme_core_resourceManager_publicationItem_t* item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localPublications, publicationHandle);

	XME_CHECK(NULL != item, XME_CORE_COMPONENT_INVALID_COMPONENT_PORT);

	return item->publishingPort;
}

xme_core_status_t
xme_core_dcc_sendTopicData
(
	xme_core_dcc_publicationHandle_t publicationHandle,
	void* data,
	uint16_t size
)
{
	xme_core_status_t status = XME_CORE_STATUS_INVALID_HANDLE;
	xme_hal_sharedPtr_t dataHandle;
	xme_core_component_t sourceComponent;
	xme_core_component_port_t sourcePort;

	// Validate publication handle

	// Check data (if possible)
	// Note: data could also be NULL in case of a plain notification event

	// Send announcements over all relevant interfaces (or schedule them for sending)

	// Store the data in reference counted memory
	XME_CHECK
	(
		XME_HAL_SHAREDPTR_INVALID_POINTER != (dataHandle = xme_hal_sharedPtr_createFromPointer(size, data)),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	sourceComponent = xme_core_dcc_getComponentFromPublicationHandle(publicationHandle);
	if (XME_CORE_STATUS_INVALID_HANDLE != (sourcePort = xme_core_dcc_getPortFromPublicationHandle(publicationHandle)))
	{
		status = xme_core_broker_dccSendTopicData(sourceComponent, sourcePort, dataHandle);
	}

	// Free the data in reference counted memory
	xme_hal_sharedPtr_destroy(dataHandle);

	// A publisher should not care its publication not to be forwarded
	if (XME_CORE_STATUS_NOT_FOUND == status)
	{
		status = XME_CORE_STATUS_SUCCESS;
	}

	return status;
}

xme_core_dcc_subscriptionHandle_t
xme_core_dcc_subscribeTopic
(
	xme_core_topic_t topic,
	xme_core_md_topicMetaDataHandle_t topicFilter,
	bool onlyLocal,
	xme_core_dcc_receiveTopicCallback_t callback,
	void* userData
)
{
	xme_core_resourceManager_subscriptionItem_t* item;
	xme_core_dcc_subscriptionHandle_t subscriptionHandle;

	XME_CORE_DCC_ASSERT_REFUSE_INTERNAL_SUBSCRIPTION_TOPIC(topic, XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE);

	subscriptionHandle =
		_xme_core_dcc_subscribeTopicWithoutAnnouncement
		(
			topic,
			topicFilter,
			callback,
			userData
		);
	XME_CHECK(XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE != subscriptionHandle, XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE);

	item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localSubscriptions, subscriptionHandle);
	XME_ASSERT_RVAL(NULL != item, XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE);

	// Build and send a port announcement
	{
		xme_hal_sharedPtr_t dataHandle;
		xme_core_topic_announcementPacket_t* announcement;

		// TODO: Error handling! See ticket #721
		dataHandle = xme_hal_sharedPtr_create(sizeof(xme_core_topic_announcementPacket_t));
		announcement = (xme_core_topic_announcementPacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

		announcement->announcementType = XME_CORE_ANNOUNCEMENT_SUBSCRIBE_TOPIC;
		announcement->nodeId = xme_core_nodeManager_getNodeId();
		announcement->componentId = item->subscribingComponent;
		announcement->portId = item->subscribingPort;
		announcement->subscribeTopic.subscriptionTopic = item->subscribedTopic;
		// TODO: Filter meta data. See ticket #646
		announcement->subscribeTopic.onlyLocal = onlyLocal;

		// TODO: Error handling! See ticket #721
		xme_core_broker_deliverData(xme_core_resourceManager_config.localAnnouncementDataChannel, dataHandle, item->subscribingComponent);

		xme_hal_sharedPtr_destroy(dataHandle);
	}

	return subscriptionHandle;
}

xme_core_status_t
xme_core_dcc_unsubscribeTopic
(
	xme_core_dcc_subscriptionHandle_t subscriptionHandle
)
{
	xme_core_component_t subscribingComponent;
	xme_core_component_port_t subscribingPort;

	{
		xme_core_status_t rval = _xme_core_dcc_unsubscribeTopicWithoutAnnouncement(subscriptionHandle, &subscribingComponent, &subscribingPort);
		XME_CHECK(XME_CORE_STATUS_SUCCESS == rval, rval);
	}

	// Build and send a port announcement
	{
		xme_hal_sharedPtr_t dataHandle;
		xme_core_topic_announcementPacket_t* announcement;

		// TODO: Error handling! See ticket #721
		dataHandle = xme_hal_sharedPtr_create(sizeof(xme_core_topic_announcementPacket_t));
		announcement = (xme_core_topic_announcementPacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

		announcement->announcementType = XME_CORE_ANNOUNCEMENT_UNSUBSCRIBE_TOPIC;
		announcement->nodeId = xme_core_nodeManager_getNodeId();
		announcement->componentId = subscribingComponent;
		announcement->portId = subscribingPort;

		// TODO: Error handling! See ticket #721
		xme_core_broker_deliverData(xme_core_resourceManager_config.localAnnouncementDataChannel, dataHandle, subscribingComponent);

		xme_hal_sharedPtr_destroy(dataHandle);
	}

	return XME_CORE_STATUS_SUCCESS;
}

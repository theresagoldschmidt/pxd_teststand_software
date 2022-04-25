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
 *         Request/response communication (RR).
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Gerd Kainz <kainz@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/rr.h"

#include "xme/core/broker.h"
#include "xme/core/dcc.h"
#include "xme/core/defines.h"
#include "xme/core/resourceManager.h"
#include "xme/core/directory.h"
#include "xme/core/packet.h"

#include "xme/hal/sharedPtr.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
/**
 * \def    XME_CORE_RR_ASSERT_REFUSE_INVALID_REQUEST_TOPIC
 *
 * \brief  Returns from the current function with the given return value if the
 *         given topic is not a valid request topic.
 *
 * \param  topic Topic, which shall be check.
 * \param  rval Return value to pass to the caller of the current function
 *         in case the topic is no valid request topic.
 */
#define XME_CORE_RR_ASSERT_REFUSE_INVALID_REQUEST_TOPIC(topic, rval) \
	do { \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_INVALID_TOPIC != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_LOCAL_ANNOUNCEMENT != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_REMOTE_ANNOUNCEMENT != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_REMOTE_MODIFY_ROUTING_TABLE != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_NEW_NODE_RESPONSE != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_MANAGEMENT_CHANNELS_TO_EDGE_NODE != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_MANAGEMENT_CHANNELS_TO_NEW_NODE != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_LOGIN_RESPONSE != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_COMMON != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_FLAG != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_INTEGER != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_UNSIGNED_INTEGER != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_DECIMAL != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_STRING != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_USER != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_MODBUS_RESPONSE != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_ARRAY_OF_DATA != topic, rval); \
	} while (0)

/**
 * \def    XME_CORE_RR_ASSERT_REFUSE_INTERNAL_REQUEST_TOPIC
 *
 * \brief  Returns from the current function with the given return value if the
 *         given topic is an internal request topic.
 *
 * \param  topic Topic, which shall be check.
 * \param  rval Return value to pass to the caller of the current function
 *         in case the topic is an internal request topic.
 */
#define XME_CORE_RR_ASSERT_REFUSE_INTERNAL_REQUEST_TOPIC(topic, rval) \
	do { \
	} while (0)

/**
 * \def    XME_CORE_RR_ASSERT_REFUSE_INVALID_RESPONSE_TOPIC
 *
 * \brief  Returns from the current function with the given return value if the
 *         given topic is not a valid response topic.
 *
 * \param  topic Topic, which shall be check.
 * \param  rval Return value to pass to the caller of the current function
 *         in case the topic is no valid response topic.
 */
#define XME_CORE_RR_ASSERT_REFUSE_INVALID_RESPONSE_TOPIC(topic, rval) \
	do { \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_INVALID_TOPIC != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_LOCAL_ANNOUNCEMENT != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_REMOTE_ANNOUNCEMENT != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_REMOTE_MODIFY_ROUTING_TABLE != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_NEW_NODE_REQUEST != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_MANAGEMENT_CHANNELS_TO_EDGE_NODE != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_MANAGEMENT_CHANNELS_TO_NEW_NODE != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_LOGIN_REQUEST != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_COMMON != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_FLAG != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_INTEGER != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_UNSIGNED_INTEGER != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_DECIMAL != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_STRING != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_USER != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_MODBUS_REQUEST != topic, rval); \
		XME_ASSERT_RVAL(XME_CORE_TOPIC_ARRAY_OF_DATA != topic, rval); \
	} while (0)

/**
 * \def    XME_CORE_RR_ASSERT_REFUSE_INTERNAL_RESPONSE_TOPIC
 *
 * \brief  Returns from the current function with the given return value if the
 *         given topic is an internal response topic.
 *
 * \param  topic Topic, which shall be check.
 * \param  rval Return value to pass to the caller of the current function
 *         in case the topic is an internal response topic.
 */
#define XME_CORE_RR_ASSERT_REFUSE_INTERNAL_RESPONSE_TOPIC(topic, rval) \
	do { \
	} while (0)

/******************************************************************************/
/***   Helper functions                                                     ***/
/******************************************************************************/
/**
 * \brief  Makes the runtime system aware that the calling component intends
 *         to publish (i.e., send) the given request topic and expects the
 *         given response topic as reply.
 *         In contrast to xme_core_rr_publishRequest(), this function will not
 *         send an announcement to the directory.
 *
 *         This function does not indicate when data of the given topic are
 *         sent. This can happen at any point in time after the call to this
 *         function. When a component does not intend to send requests under
 *         this topic any more, the xme_core_rr_unpublishRequestWithoutAnnouncement()
 *         function should be called.
 *
 * \param  requestTopic Topic of the request.
 * \param  requestMetaDataHandle Meta data associated to the request.
 * \param  responseTopic Topic of the response.
 * \param  responseMetaDataFilter Meta data filter to apply to the response.
 * \param  receiveResponseCallback Callback function to be called when the
 *         response arrives or when the timeout interval specified in
 *         the timeoutMs parameter of the respective call to
 *         xme_core_rr_sendRequest() elapses.
 * \param  userData User-specified data to pass to the callback function.
 *
 * \return On success, returns a nonzero request handle, otherwise returns
 *         XME_CORE_RR_INVALID_REQUEST_HANDLE.
 */
xme_core_rr_requestHandle_t
_xme_core_rr_publishRequestWithoutAnnouncement
(
	xme_core_topic_t requestTopic,
	xme_core_md_topicMetaDataHandle_t requestMetaDataHandle,
	xme_core_topic_t responseTopic,
	xme_core_md_topicMetaDataHandle_t responseMetaDataFilter,
	xme_core_rr_receiveResponseCallback_t receiveResponseCallback,
	void* userData
)
{
	xme_core_rr_requestHandle_t handle;
	xme_core_component_t currentComponentId;
	xme_core_resourceManager_requestSenderItem_t* item;
	xme_core_component_port_t outPort;
	xme_core_component_port_t inPort;

	XME_CORE_RR_ASSERT_REFUSE_INVALID_REQUEST_TOPIC(requestTopic, XME_CORE_RR_INVALID_REQUEST_HANDLE);
	XME_CORE_RR_ASSERT_REFUSE_INVALID_RESPONSE_TOPIC(responseTopic, XME_CORE_RR_INVALID_REQUEST_HANDLE);

	// Determine the calling component
	currentComponentId = xme_core_resourceManager_getCurrentComponentId();

	// Only components can publish requests
	XME_CHECK
	(
		XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT != currentComponentId,
		XME_CORE_RR_INVALID_REQUEST_HANDLE
	);

	// Check input parameters
	XME_CHECK
	(
		XME_CORE_TOPIC_INVALID_TOPIC != requestTopic,
		XME_CORE_RR_INVALID_REQUEST_HANDLE
	);
	XME_CHECK
	(
		XME_CORE_TOPIC_INVALID_TOPIC != responseTopic,
		XME_CORE_RR_INVALID_REQUEST_HANDLE
	);

	// Verify topic meta data handles. It is valid for the meta data handles to
	// be XME_CORE_DCC_INVALID_TOPIC_META_DATA_HANDLE (=0), in which case no
	// meta data is used.
	if (XME_CORE_MD_EMPTY_META_DATA != requestMetaDataHandle)
	{
		XME_CHECK
		(
			xme_core_md_isTopicMetaDataHandleValid(requestMetaDataHandle),
			XME_CORE_RR_INVALID_REQUEST_HANDLE
		);
	}
	if (XME_CORE_MD_EMPTY_META_DATA != responseMetaDataFilter)
	{
		XME_CHECK
		(
			xme_core_md_isTopicMetaDataHandleValid(responseMetaDataFilter),
			XME_CORE_RR_INVALID_REQUEST_HANDLE
		);
	}

	// Allocate a unique request publication handle
	XME_CHECK
	(
		XME_HAL_TABLE_INVALID_ROW_HANDLE !=
		(
			handle = (xme_core_rr_requestHandle_t)XME_HAL_TABLE_ADD_ITEM(xme_core_resourceManager_config.localRequestSenders)
		),
		XME_CORE_RR_INVALID_REQUEST_HANDLE
	);

	// Allocate output port
	XME_CHECK_REC
	(
		XME_CORE_COMPONENT_INVALID_COMPONENT_PORT !=
		(
			outPort = xme_core_resourceManager_addRequestSenderPort(currentComponentId)
		),
		XME_CORE_RR_INVALID_REQUEST_HANDLE,
		{
			XME_HAL_TABLE_REMOVE_ITEM(xme_core_resourceManager_config.localRequestSenders, (xme_hal_table_rowHandle_t)handle);
		}
	);

	// Allocate input port
	XME_CHECK_REC
	(
		XME_CORE_COMPONENT_INVALID_COMPONENT_PORT !=
		(
			inPort = xme_core_resourceManager_addResponseHandlerPort(currentComponentId, receiveResponseCallback, userData)
		),
		XME_CORE_RR_INVALID_REQUEST_HANDLE,
		{
			xme_core_resourceManager_removePort(currentComponentId, outPort);
			XME_HAL_TABLE_REMOVE_ITEM(xme_core_resourceManager_config.localRequestSenders, (xme_hal_table_rowHandle_t)handle);
		}
	);

	item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localRequestSenders, handle);
	item->requestingComponent = currentComponentId;
	item->requestingPort = outPort;
	item->responseHandlingPort = inPort;
	item->requestTopic = requestTopic;
	item->responseTopic = responseTopic;
	item->requestStaticMetaData = requestMetaDataHandle;

	return handle;
}

/**
 * \brief  Makes the runtime system aware that the calling component does not
 *         intend to send requests of the given type any more.
 *         But no announcement will be sent to the directory.
 *
 * \param  requestHandle Handle of the request to unpublish.
 * \param  requestingComponent Component corresponding to request to unpublish.
 * \param  requestingPort Port corresponding to request to unpublish.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the request was unpublished
 *            successfully.
 *          - XME_CORE_STATUS_PERMISSION_DENIED if the calling component is
 *            not allowed to perform this action.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the given request handle was
 *            invalid.
 */
xme_core_status_t
_xme_core_rr_unpublishRequestWithoutAnnouncement
(
	xme_core_rr_requestHandle_t requestHandle,
	xme_core_component_t* requestingComponent,
	xme_core_component_port_t* requestingPort
)
{
	xme_core_component_t currentComponentId;
	xme_core_resourceManager_requestSenderItem_t* item;
	xme_core_resourceManager_componentInfo_t* componentInfo;

	// Retrieve the request descriptor
	XME_CHECK
	(
		NULL != (item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localRequestSenders, requestHandle)),
		XME_CORE_STATUS_INVALID_HANDLE
	);

	// Determine the calling component
	currentComponentId = xme_core_resourceManager_getCurrentComponentId();

	// Only the component that published a request can unpublish it
	XME_CHECK
	(
		item->requestingComponent == currentComponentId,
		XME_CORE_STATUS_PERMISSION_DENIED
	);

	// Retrieve the component information structure for the current component
	componentInfo = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.components, currentComponentId);
	XME_ASSERT(NULL != componentInfo);

	// Remove input port
	{
		xme_core_status_t rval = xme_core_resourceManager_removePort(currentComponentId, item->responseHandlingPort);
		XME_ASSERT(XME_CORE_STATUS_SUCCESS == rval);
	}

	// Remove output port
	{
		xme_core_status_t rval = xme_core_resourceManager_removePort(currentComponentId, item->requestingPort);
		XME_ASSERT(XME_CORE_STATUS_SUCCESS == rval);
	}

	//Save information to send unregister announcement
	if (NULL != requestingComponent)
	{
		*requestingComponent = item->requestingComponent;
	}
	if (NULL != requestingPort)
	{
		*requestingPort = item->requestingPort;
	}

	// Remove the publication
	{
		xme_core_status_t rval = XME_HAL_TABLE_REMOVE_ITEM(xme_core_resourceManager_config.localRequestSenders, (xme_hal_table_rowHandle_t)requestHandle);
		XME_ASSERT(XME_CORE_STATUS_SUCCESS == rval);
	}

	return XME_CORE_STATUS_SUCCESS;
}

/**
 * \brief  Makes the runtime system aware that the calling component can
 *         handle requests of the given type and, upon success, will respond
 *         to them with the given response topic.
 *         But no announcement will be sent to the directory.
 *
 * \param  requestTopic Topic of the request that is accepted by the component.
 * \param  requestFilter Meta data filter for incoming requests.
 * \param  responseTopic Topic of the response the component will send after
 *         processing a request matching the given parameters.
 * \param  responseMetaData Static meta data of the response topic.
 * \param  receiveRequestCallback Callback function for handling the given
 *         request.
 * \param  userData User-defined data to pass to the callback function.
 *
 * \return On success, returns a nonzero request/response handle, otherwise
 *         returns XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE.
 */
xme_core_rr_requestHandlerHandle_t
_xme_core_rr_publishRequestHandlerWithoutAnnouncement
(
	xme_core_topic_t requestTopic,
	xme_core_md_topicMetaDataHandle_t requestFilter,
	xme_core_topic_t responseTopic,
	xme_core_md_topicMetaDataHandle_t responseMetaData,
	xme_core_rr_receiveRequestCallback_t receiveRequestCallback,
	void* userData
)
{
	xme_core_rr_requestHandlerHandle_t handle;
	xme_core_component_t currentComponentId;
	xme_core_resourceManager_requestHandlerItem_t* item;
	xme_core_component_port_t inPort;
	xme_core_component_port_t outPort;

	XME_CORE_RR_ASSERT_REFUSE_INVALID_REQUEST_TOPIC(requestTopic, XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE);
	XME_CORE_RR_ASSERT_REFUSE_INVALID_RESPONSE_TOPIC(responseTopic, XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE);

	// Determine the calling component
	currentComponentId = xme_core_resourceManager_getCurrentComponentId();

	// Only components can publish request handlers
	XME_CHECK
	(
		XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT != currentComponentId,
		XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE
	);

	// Check input parameters
	XME_CHECK
	(
		XME_CORE_TOPIC_INVALID_TOPIC != requestTopic,
		XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE
	);

	XME_CHECK
	(
		XME_CORE_TOPIC_INVALID_TOPIC != responseTopic,
		XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE
	);

	// Verify topic meta data handles. It is valid for the meta data handles to
	// be XME_CORE_DCC_INVALID_TOPIC_META_DATA_HANDLE (=0), in which case no
	// meta data is used.
	if (XME_CORE_MD_EMPTY_META_DATA != requestFilter)
	{
		XME_CHECK
		(
			xme_core_md_isTopicMetaDataHandleValid(requestFilter),
			XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE
		);
	}
	if (XME_CORE_MD_EMPTY_META_DATA != responseMetaData)
	{
		XME_CHECK
		(
			xme_core_md_isTopicMetaDataHandleValid(responseMetaData),
			XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE
		);
	}

	// Allocate a unique request publication handle
	XME_CHECK
	(
		XME_HAL_TABLE_INVALID_ROW_HANDLE !=
		(
			handle = (xme_core_rr_requestHandlerHandle_t)XME_HAL_TABLE_ADD_ITEM(xme_core_resourceManager_config.localRequestHandlers)
		),
		XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE
	);

	// Allocate input port
	XME_CHECK_REC
	(
		XME_CORE_COMPONENT_INVALID_COMPONENT_PORT !=
		(
			inPort = xme_core_resourceManager_addRequestHandlerPort(currentComponentId, receiveRequestCallback, userData)
		),
		XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE,
		{
			XME_HAL_TABLE_REMOVE_ITEM(xme_core_resourceManager_config.localRequestHandlers, (xme_hal_table_rowHandle_t)handle);
		}
	);

	// Allocate output port
	XME_CHECK_REC
	(
		XME_CORE_COMPONENT_INVALID_COMPONENT_PORT !=
		(
			outPort = xme_core_resourceManager_addResponseSenderPort(currentComponentId)
		),
		XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE,
		{
			xme_core_resourceManager_removePort(currentComponentId, inPort);
			XME_HAL_TABLE_REMOVE_ITEM(xme_core_resourceManager_config.localRequestHandlers, (xme_hal_table_rowHandle_t)handle);
		}
	);

	item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localRequestHandlers, handle);
	item->requestHandlingComponent = currentComponentId;
	item->requestHandlingPort = inPort;
	item->responseSendingPort = outPort;
	item->requestTopic = requestTopic;
	item->requestMetaDataFilter = requestFilter;
	item->responseTopic = responseTopic;
	item->responseStaticMetaData = responseMetaData;

	return handle;
}

/**
 * \brief  Makes the runtime system aware that the calling component
 *         discontinues handling the given request.
 *         But no announcement will be sent to the directory.
 *
 * \param  requestHandlerHandle handling to discontinue.
 * \param  requestHandlingComponent Component corresponding to request handler to unpublish.
 * \param  requestHandlingPort Port corresponding to request hanlder to unpublish.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the request handling capability has
 *            been successfully discontinued.
 *          - XME_CORE_STATUS_PERMISSION_DENIED if the calling component is
 *            not allowed to perform this action.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the given request/response
 *            handle was not valid.
 */
xme_core_status_t
_xme_core_rr_unpublishRequestHandlerWithoutAnnouncement
(
	xme_core_rr_requestHandlerHandle_t requestHandlerHandle,
	xme_core_component_t* requestHandlingComponent,
	xme_core_component_port_t* requestHandlingPort
)
{
	xme_core_component_t currentComponentId;
	xme_core_resourceManager_requestHandlerItem_t* item;

	// Retrieve the request handler descriptor
	XME_CHECK
	(
		NULL != (item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localRequestHandlers, requestHandlerHandle)),
		XME_CORE_STATUS_INVALID_HANDLE
	);

	// Determine the calling component
	currentComponentId = xme_core_resourceManager_getCurrentComponentId();

	// Only the component that published a request handler can unpublish it
	XME_CHECK
	(
		item->requestHandlingComponent == currentComponentId,
		XME_CORE_STATUS_PERMISSION_DENIED
	);

	// Remove output port
	{
		xme_core_status_t rval = xme_core_resourceManager_removePort(currentComponentId, item->responseSendingPort);
		XME_ASSERT(XME_CORE_STATUS_SUCCESS == rval);
	}

	// Remove input port
	{
		xme_core_status_t rval = xme_core_resourceManager_removePort(currentComponentId, item->requestHandlingPort);
		XME_ASSERT(XME_CORE_STATUS_SUCCESS == rval);
	}

	//Save information to send unregister announcement
	if (NULL != requestHandlingComponent)
	{
		*requestHandlingComponent = item->requestHandlingComponent;
	}
	if (NULL != requestHandlingPort)
	{
		*requestHandlingPort = item->requestHandlingPort;
	}

	// Remove the publication
	{
		xme_core_status_t rval = XME_HAL_TABLE_REMOVE_ITEM(xme_core_resourceManager_config.localRequestHandlers, (xme_hal_table_rowHandle_t)requestHandlerHandle);
		XME_ASSERT(XME_CORE_STATUS_SUCCESS == rval);
	}

	return XME_CORE_STATUS_SUCCESS;
}

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_core_rr_init()
{
	// Nothing to do

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_core_rr_fini()
{
	// Nothing to do
}

xme_core_rr_requestHandle_t
xme_core_rr_publishRequest
(
	xme_core_topic_t requestTopic,
	xme_core_md_topicMetaDataHandle_t requestMetaDataHandle,
	xme_core_topic_t responseTopic,
	xme_core_md_topicMetaDataHandle_t responseMetaDataFilter,
	bool manyResponses,
	bool onlyLocal,
	xme_core_rr_receiveResponseCallback_t receiveResponseCallback,
	void* userData
)
{
	xme_core_resourceManager_requestSenderItem_t* item;
	xme_core_rr_requestHandle_t requestHandle;

	XME_CORE_RR_ASSERT_REFUSE_INTERNAL_REQUEST_TOPIC(requestTopic, XME_CORE_RR_INVALID_REQUEST_HANDLE);
	XME_CORE_RR_ASSERT_REFUSE_INTERNAL_RESPONSE_TOPIC(responseTopic, XME_CORE_RR_INVALID_REQUEST_HANDLE);

	requestHandle =
		_xme_core_rr_publishRequestWithoutAnnouncement
		(
			requestTopic,
			requestMetaDataHandle,
			responseTopic,
			responseMetaDataFilter,
			receiveResponseCallback,
			userData
		);
	XME_CHECK(XME_CORE_RR_INVALID_REQUEST_HANDLE != requestHandle, XME_CORE_RR_INVALID_REQUEST_HANDLE);

	item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localRequestSenders, requestHandle);
	XME_ASSERT_RVAL(NULL != item, XME_CORE_RR_INVALID_REQUEST_HANDLE);

	// Build and send a port announcement
	{
		xme_hal_sharedPtr_t dataHandle;
		xme_core_topic_announcementPacket_t* announcement;

		// TODO: Error handling! See ticket #721
		dataHandle = xme_hal_sharedPtr_create(sizeof(xme_core_topic_announcementPacket_t));
		announcement = (xme_core_topic_announcementPacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

		announcement->announcementType = XME_CORE_ANNOUNCEMENT_PUBLISH_REQUEST;
		announcement->nodeId = xme_core_nodeManager_getNodeId();
		announcement->componentId = item->requestingComponent;
		announcement->portId = item->requestingPort;
		announcement->publishRequest.responseHandlingPort = item->responseHandlingPort;
		announcement->publishRequest.requestTopic = item->requestTopic;
		// TODO: meta data. See ticket #646
		announcement->publishRequest.responseTopic = item->responseTopic;
		// TODO: meta data. See ticket #646
		announcement->publishRequest.manyResponses = manyResponses;
		announcement->publishRequest.onlyLocal = onlyLocal;

		// TODO: Error handling! See ticket #721
		xme_core_broker_deliverData(xme_core_resourceManager_config.localAnnouncementDataChannel, dataHandle, item->requestingComponent);

		xme_hal_sharedPtr_destroy(dataHandle);
	}

	// Return the unique request handle
	return requestHandle;
}

xme_core_status_t
xme_core_rr_unpublishRequest
(
	xme_core_rr_requestHandle_t requestHandle
)
{
	xme_core_component_t requestingComponent;
	xme_core_component_port_t requestingPort;

	{
		xme_core_status_t rval = _xme_core_rr_unpublishRequestWithoutAnnouncement(requestHandle, &requestingComponent, &requestingPort);
		XME_CHECK(XME_CORE_STATUS_SUCCESS == rval, rval);
	}

	// Build and send a port announcement
	{
		xme_hal_sharedPtr_t dataHandle;
		xme_core_topic_announcementPacket_t* announcement;

		// TODO: Error handling! See ticket #721
		dataHandle = xme_hal_sharedPtr_create(sizeof(xme_core_topic_announcementPacket_t));
		announcement = (xme_core_topic_announcementPacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

		announcement->announcementType = XME_CORE_ANNOUNCEMENT_UNPUBLISH_REQUEST;
		announcement->nodeId = xme_core_nodeManager_getNodeId();
		announcement->componentId = requestingComponent;
		announcement->portId = requestingPort;

		// TODO: Error handling! See ticket #721
		xme_core_broker_deliverData(xme_core_resourceManager_config.localAnnouncementDataChannel, dataHandle, requestingComponent);

		xme_hal_sharedPtr_destroy(dataHandle);
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_rr_requestHandlerHandle_t
xme_core_rr_publishRequestHandler
(
	xme_core_topic_t requestTopic,
	xme_core_md_topicMetaDataHandle_t requestFilter,
	xme_core_topic_t responseTopic,
	xme_core_md_topicMetaDataHandle_t responseMetaData,
	bool onlyLocal,
	xme_core_rr_receiveRequestCallback_t receiveRequestCallback,
	void* userData
)
{
	xme_core_resourceManager_requestHandlerItem_t* item;
	xme_core_rr_requestHandlerHandle_t requestHandlerHandle;

	XME_CORE_RR_ASSERT_REFUSE_INTERNAL_REQUEST_TOPIC(requestTopic, XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE);
	XME_CORE_RR_ASSERT_REFUSE_INTERNAL_RESPONSE_TOPIC(responseTopic, XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE);

	requestHandlerHandle =
		_xme_core_rr_publishRequestHandlerWithoutAnnouncement
		(
			requestTopic,
			requestFilter,
			responseTopic,
			responseMetaData,
			receiveRequestCallback,
			userData
		);
	XME_CHECK(XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE != requestHandlerHandle, XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE);

	item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localRequestHandlers, requestHandlerHandle);
	XME_ASSERT_RVAL(NULL != item, XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE);

	// Build and send a port announcement
	{
		xme_hal_sharedPtr_t dataHandle;
		xme_core_topic_announcementPacket_t* announcement;

		// TODO: Error handling! See ticket #721
		dataHandle = xme_hal_sharedPtr_create(sizeof(xme_core_topic_announcementPacket_t));
		announcement = (xme_core_topic_announcementPacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

		announcement->announcementType = XME_CORE_ANNOUNCEMENT_PUBLISH_REQUEST_HANDLER;
		announcement->nodeId = xme_core_nodeManager_getNodeId();
		announcement->componentId = item->requestHandlingComponent;
		announcement->portId = item->requestHandlingPort;
		announcement->publishRequestHandler.responseSendingPort = item->responseSendingPort;
		announcement->publishRequestHandler.requestTopic = item->requestTopic;
		// TODO: meta data. See ticket #646
		announcement->publishRequestHandler.responseTopic = item->responseTopic;
		// TODO: meta data. See ticket #646
		announcement->publishRequestHandler.onlyLocal = onlyLocal;

		// TODO: Error handling! See ticket #721
		xme_core_broker_deliverData(xme_core_resourceManager_config.localAnnouncementDataChannel, dataHandle, item->requestHandlingComponent);

		xme_hal_sharedPtr_destroy(dataHandle);
	}

	// Return the unique request handler handle
	return requestHandlerHandle;
}

xme_core_status_t
xme_core_rr_unpublishRequestHandler
(
	xme_core_rr_requestHandlerHandle_t requestHandlerHandle
)
{
	xme_core_component_t requestHandlingComponent;
	xme_core_component_port_t requestHandlingPort;

	{
		xme_core_status_t rval = _xme_core_rr_unpublishRequestHandlerWithoutAnnouncement(requestHandlerHandle, &requestHandlingComponent, &requestHandlingPort);
		XME_CHECK(XME_CORE_STATUS_SUCCESS == rval, rval);
	}

	// Build and send a port announcement
	{
		xme_hal_sharedPtr_t dataHandle;
		xme_core_topic_announcementPacket_t* announcement;

		// TODO: Error handling! See ticket #721
		dataHandle = xme_hal_sharedPtr_create(sizeof(xme_core_topic_announcementPacket_t));
		announcement = (xme_core_topic_announcementPacket_t*)xme_hal_sharedPtr_getPointer(dataHandle);

		announcement->announcementType = XME_CORE_ANNOUNCEMENT_UNPUBLISH_REQUEST_HANDLER;
		announcement->nodeId = xme_core_nodeManager_getNodeId();
		announcement->componentId = requestHandlingComponent;
		announcement->portId = requestHandlingPort;

		// TODO: Error handling! See ticket #721
		xme_core_broker_deliverData(xme_core_resourceManager_config.localAnnouncementDataChannel, dataHandle, requestHandlingComponent);

		xme_hal_sharedPtr_destroy(dataHandle);
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_rr_requestInstanceHandle_t
xme_core_rr_sendRequest
(
	xme_core_rr_requestHandle_t request,
	void* requestData,
	uint16_t requestSize,
	void* instanceUserData,
	xme_hal_time_interval_t timeoutMs
)
{
	xme_core_component_t currentComponentId;
	xme_core_resourceManager_requestSenderItem_t* item;
	xme_hal_sharedPtr_t dataHandle;
	xme_core_rr_requestInstanceHandle_t requestInstanceHandle;
	xme_core_packet_requestContext_t* requestContext;

	// Retrieve the request descriptor
	XME_CHECK
	(
		NULL != (item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localRequestSenders, request)),
		XME_CORE_RR_INVALID_REQUEST_INSTANCE_HANDLE
	);

	// Determine the calling component
	currentComponentId = xme_core_resourceManager_getCurrentComponentId();

	// Only the component that registered the request can send the request
	XME_CHECK
	(
		item->requestingComponent == currentComponentId,
		XME_CORE_RR_INVALID_REQUEST_INSTANCE_HANDLE
	);

	// TODO: timeoutMs! See ticket #797

	// Generate a unique request instance handle
	requestInstanceHandle = 0x10D0; // TODO: See ticket #798

	// Reserve some extra space for the request context data
	XME_CHECK
	(
		XME_HAL_SHAREDPTR_INVALID_POINTER != (dataHandle = xme_hal_sharedPtr_create(requestSize + sizeof(xme_core_packet_requestContext_t))),
		XME_CORE_RR_INVALID_REQUEST_INSTANCE_HANDLE
	);

	requestContext = (xme_core_packet_requestContext_t*)xme_hal_sharedPtr_getPointer(dataHandle);
	XME_ASSERT_RVAL(NULL != requestContext, XME_CORE_RR_INVALID_REQUEST_INSTANCE_HANDLE);

	// Set up the request context
	requestContext->localRequestHandle = request;
	requestContext->localRequestInstance = requestInstanceHandle;
	requestContext->instanceUserData = (uint32_t)instanceUserData;

	// Copy over the actual request data
	memcpy(((char*)requestContext) + sizeof(xme_core_packet_requestContext_t), requestData, requestSize);

	XME_CHECK_REC
	(
		XME_CORE_STATUS_SUCCESS == xme_core_broker_rrSendRequest(item->requestingComponent, item->requestingPort, dataHandle),
		XME_CORE_RR_INVALID_REQUEST_INSTANCE_HANDLE,
		{
			// Clean up request instance handle
			// TODO! See ticket #799

			xme_hal_sharedPtr_destroy(dataHandle);
		}
	);

	xme_hal_sharedPtr_destroy(dataHandle);

	return requestInstanceHandle;
}

xme_core_status_t
xme_core_rr_sendResponse
(
	xme_core_rr_responseStatus_t status,
	xme_core_rr_requestHandlerHandle_t requestHandler,
	xme_core_rr_responseInstanceHandle_t responseInstanceHandle,
	void* responseData,
	uint16_t responseSize
)
{
	xme_core_component_t currentComponentId;
	xme_core_resourceManager_requestHandlerItem_t* item;
	xme_core_resourceManager_responseInstanceItem_t* reponseInstanceItem;
	xme_hal_sharedPtr_t dataHandle;
	xme_core_packet_responseContext_t* responseContext;
	xme_core_status_t rval;

	// Retrieve the request handler descriptor
	XME_CHECK
	(
		NULL != (item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localRequestHandlers, requestHandler)),
		XME_CORE_STATUS_INVALID_PARAMETER
	);

	// Determine the calling component
	currentComponentId = xme_core_resourceManager_getCurrentComponentId();

	// Only the component that registered the request handler can send the response
	XME_CHECK
	(
		item->requestHandlingComponent == currentComponentId,
		XME_CORE_STATUS_PERMISSION_DENIED
	);

	// Retrieve the response instance structure
	XME_CHECK
	(
		NULL != (reponseInstanceItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localResponseInstances, responseInstanceHandle)),
		XME_CORE_STATUS_INVALID_HANDLE
	);

	// Reserve some extra space for the response context data
	XME_CHECK
	(
		XME_HAL_SHAREDPTR_INVALID_POINTER != (dataHandle = xme_hal_sharedPtr_create(responseSize + sizeof(xme_core_packet_responseContext_t))),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// Set up the response context
	responseContext = (xme_core_packet_responseContext_t*)xme_hal_sharedPtr_getPointer(dataHandle);
	XME_ASSERT(NULL != responseContext);

	responseContext->localRequestHandle = reponseInstanceItem->localRequestHandle;
	responseContext->localRequestInstance = reponseInstanceItem->localRequestInstance;
	responseContext->responseTopic = item->responseTopic;
	responseContext->responseStatus = status;

	memcpy(((char*)responseContext) + sizeof(xme_core_packet_responseContext_t), responseData, responseSize);

	rval = xme_core_broker_rrSendResponse(reponseInstanceItem->responseDataChannel, item->requestHandlingComponent, item->responseSendingPort, dataHandle);

	// Delete the response data
	xme_hal_sharedPtr_destroy(dataHandle);

	// Invalidate the response instance handle
	XME_HAL_TABLE_REMOVE_ITEM(xme_core_resourceManager_config.localResponseInstances, (xme_hal_table_rowHandle_t)responseInstanceHandle);

	return rval;
}

xme_core_status_t
xme_core_rr_cancelRequest
(
	xme_core_rr_requestInstanceHandle_t requestInstanceHandle
)
{
	// TODO: See ticket #770

	return XME_CORE_STATUS_SUCCESS;
}

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
 *         Login server.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/adv/loginServer.h"

#include "xme/core/defines.h"

#ifndef XME_CORE_DIRECTORY_TYPE_MASTER
	#error Login Server can only be used on a node that has XME_CORE_DIRECTORY_TYPE_MASTER defined!
#endif // #ifndef XME_CORE_DIRECTORY_TYPE_MASTER

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Callback function for incoming login requests.
 *
 * \see    xme_core_rr_receiveRequestCallback_t
 */
static
xme_core_rr_responseStatus_t
xme_adv_loginServer_receiveLoginRequest
(
	xme_core_topic_t requestTopic,
	void* requestData,
	xme_core_rr_responseInstanceHandle_t responseInstanceHandle,
	void* responseData,
	uint16_t* responseSize,
	xme_hal_time_interval_t responseTimeoutMs,
	void* userData
);

/**
 * \brief  Callback function for incoming new node responses.
 *
 * \see    xme_core_rr_receiveResponseCallback_t
 */
static
void
xme_adv_loginServer_receiveNewNodeResponse
(
	xme_core_rr_responseStatus_t status,
	xme_core_rr_requestHandle_t request,
	xme_core_rr_requestInstanceHandle_t requestInstance,
	xme_core_topic_t responseTopic,
	void* responseData,
	xme_core_md_topicMetaDataHandle_t responseMetaData,
	void* userData,
	void* instanceUserData
);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_adv_loginServer_create(xme_adv_loginServer_configStruct_t* config)
{
	// Initialize configuration structure
	XME_ASSERT(config->nextNodeId > XME_CORE_NODE_LOCAL_NODE_ID); // First node ID to be assigned must be at least XME_CORE_NODE_LOCAL_NODE_ID + 1
	XME_HAL_TABLE_INIT(config->nodeIdAssignments);
	config->loginRequestHandlerHandle = XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE;
	config->newNodeRequestHandle = XME_CORE_RR_INVALID_REQUEST_HANDLE;

	XME_CHECK
	(
		XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE !=
		(
			config->loginRequestHandlerHandle = xme_core_rr_publishRequestHandler
			(
				XME_CORE_TOPIC_LOGIN_REQUEST,
				XME_CORE_MD_EMPTY_META_DATA,
				XME_CORE_TOPIC_LOGIN_RESPONSE,
				XME_CORE_MD_EMPTY_META_DATA,
				false,
				&xme_adv_loginServer_receiveLoginRequest,
				config
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	XME_CHECK
	(
		XME_CORE_RR_INVALID_REQUEST_HANDLE !=
		(
			config->newNodeRequestHandle = xme_core_rr_publishRequest
			(
				XME_CORE_TOPIC_NEW_NODE_REQUEST,
				XME_CORE_MD_EMPTY_META_DATA,
				XME_CORE_TOPIC_NEW_NODE_RESPONSE,
				XME_CORE_MD_EMPTY_META_DATA,
				false,
				true,
				xme_adv_loginServer_receiveNewNodeResponse,
				config
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_adv_loginServer_activate(xme_adv_loginServer_configStruct_t* config)
{
	return XME_CORE_STATUS_SUCCESS;
}

void
xme_adv_loginServer_deactivate(xme_adv_loginServer_configStruct_t* config)
{
}

void
xme_adv_loginServer_destroy(xme_adv_loginServer_configStruct_t* config)
{
	xme_core_rr_unpublishRequest(config->newNodeRequestHandle);
	config->newNodeRequestHandle = XME_CORE_RR_INVALID_REQUEST_HANDLE;

	xme_core_rr_unpublishRequestHandler(config->loginRequestHandlerHandle);
	config->loginRequestHandlerHandle = XME_CORE_RR_INVALID_REQUEST_HANDLER_HANDLE;

	XME_HAL_TABLE_FINI(config->nodeIdAssignments);
}

static
xme_core_rr_responseStatus_t
xme_adv_loginServer_receiveLoginRequest
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
	xme_adv_loginServer_configStruct_t* config;
	xme_core_topic_loginRequestData_t* loginRequest;
	xme_hal_table_rowHandle_t nodeIdAssignmentHandle;
	xme_adv_loginServer_nodeIdAssignment_t* assignment;

	XME_ASSERT_RVAL(XME_CORE_TOPIC_LOGIN_REQUEST == requestTopic, XME_CORE_RR_STATUS_SERVER_ERROR);
	XME_ASSERT_RVAL(responseSize != NULL, XME_CORE_RR_STATUS_SERVER_ERROR);

	{
		uint16_t oldSize = *responseSize;

		// Set response size
		*responseSize = sizeof(xme_core_topic_loginResponseData_t);

		XME_CHECK(oldSize >= sizeof(xme_core_topic_loginResponseData_t), XME_CORE_RR_STATUS_BUFFER_TOO_SMALL);
	}

	// Set response size
	*responseSize = sizeof(xme_core_topic_loginResponseData_t);

	loginRequest = (xme_core_topic_loginRequestData_t*)requestData;
	config = (xme_adv_loginServer_configStruct_t*)userData;

	XME_LOG
	(
		XME_LOG_VERBOSE,
		"Received login request (DT=0x%08X%08X, GUID=0x%08X%08X, N=0:%d, E=%d:%d)\n",
		(uint32_t)(loginRequest->deviceType >> 32), (uint32_t)(loginRequest->deviceType & 0xFFFFFFFF),
		(uint32_t)(loginRequest->deviceGuid >> 32), (uint32_t)(loginRequest->deviceGuid & 0xFFFFFFFF),
		loginRequest->newNodeInterfaceId,
		loginRequest->edgeNodeId, loginRequest->edgeNodeInterfaceId
	);

	// Check whether we have served this request before
	nodeIdAssignmentHandle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
	assignment = NULL;
	XME_HAL_TABLE_GET_NEXT
	(
		config->nodeIdAssignments, xme_hal_table_rowHandle_t, nodeIdAssignmentHandle, xme_adv_loginServer_nodeIdAssignment_t, assignment,
		assignment->deviceType == loginRequest->deviceType && assignment->deviceGuid == loginRequest->deviceGuid
	);

	// TODO: Node ID assignments probably need to be persisted. See ticket #832

	if (XME_HAL_TABLE_INVALID_ROW_HANDLE != nodeIdAssignmentHandle)
	{
		XME_ASSERT_RVAL(NULL != assignment, XME_CORE_RR_STATUS_SERVER_ERROR);

		// TODO: See ticket #535!
		if (assignment->edgeNodeId != loginRequest->edgeNodeId || assignment->edgeNodeInterfaceId != loginRequest->edgeNodeInterfaceId)
		{
			// Abort
			XME_LOG
			(
				XME_LOG_VERBOSE, "Abandoning login request with different edge node or edge node interface (DT=0x%08X%08X, GUID=0x%08X%08X, N=%d, E=%d:%d, prevE=%d:%d, AC=%d, MC=%d)\n",
				(uint32_t)(loginRequest->deviceType >> 32), (uint32_t)(loginRequest->deviceType & 0xFFFFFFFF),
				(uint32_t)(loginRequest->deviceGuid >> 32), (uint32_t)(loginRequest->deviceGuid & 0xFFFFFFFF),
				assignment->nodeId,
				loginRequest->edgeNodeId, loginRequest->edgeNodeInterfaceId,
				assignment->edgeNodeId, assignment->edgeNodeInterfaceId,
				assignment->remoteAnnouncementDataChannel,
				assignment->remoteModifyRoutingTableDataChannel
			);

			return XME_CORE_RR_STATUS_SERVER_ERROR;
		}

		// XME_CORE_RR_INVALID_RESPONSE_INSTANCE_HANDLE for responseInstanceHandle means that
		// there is no pending request for login of this node at the directory.
		if (XME_CORE_RR_INVALID_RESPONSE_INSTANCE_HANDLE == assignment->responseInstanceHandle)
		{
			xme_core_topic_loginResponseData_t* loginResponse = (xme_core_topic_loginResponseData_t*)responseData;

			// We already send a node identifier assignment to that node and the
			// original directory request w.r.t. data management channels was successful.
			// Immediately send a subsequent node identifier assignment message.
			XME_LOG
			(
				XME_LOG_VERBOSE, "Sending subsequent node identifier assignment (DT=0x%08X%08X, GUID=0x%08X%08X, N=%d, E=%d:%d, AC=%d, MC=%d)\n",
				(uint32_t)(loginRequest->deviceType >> 32), (uint32_t)(loginRequest->deviceType & 0xFFFFFFFF),
				(uint32_t)(loginRequest->deviceGuid >> 32), (uint32_t)(loginRequest->deviceGuid & 0xFFFFFFFF),
				assignment->nodeId,
				assignment->edgeNodeId, assignment->edgeNodeInterfaceId,
				assignment->remoteAnnouncementDataChannel,
				assignment->remoteModifyRoutingTableDataChannel
			);

			// Prepare the reply
			loginResponse->deviceType = loginRequest->deviceType;
			loginResponse->deviceGuid = loginRequest->deviceGuid;
			loginResponse->newNodeId = assignment->nodeId;
			loginResponse->edgeNodeId = loginRequest->edgeNodeId;
			loginResponse->edgeNodeInterfaceId = loginRequest->edgeNodeInterfaceId;
			loginResponse->remoteAnnouncementDataChannel = assignment->remoteAnnouncementDataChannel;
			loginResponse->remoteModifyRoutingTableDataChannel = assignment->remoteModifyRoutingTableDataChannel;

			return XME_CORE_RR_STATUS_SUCCESS;
		}
	}
	else
	{
		xme_core_node_nodeId_t newNodeId;

		XME_ASSERT_RVAL(NULL == assignment, XME_CORE_RR_STATUS_SERVER_ERROR);

		nodeIdAssignmentHandle = XME_HAL_TABLE_ADD_ITEM(config->nodeIdAssignments);
		assignment = (xme_adv_loginServer_nodeIdAssignment_t*)XME_HAL_TABLE_ITEM_FROM_HANDLE(config->nodeIdAssignments, nodeIdAssignmentHandle);

		// Allocate a new node identifier
		newNodeId = config->nextNodeId++;

		assignment->deviceType = loginRequest->deviceType;
		assignment->deviceGuid = loginRequest->deviceGuid;
		assignment->nodeId = newNodeId;
		assignment->edgeNodeId = loginRequest->edgeNodeId;
		assignment->edgeNodeInterfaceId = loginRequest->edgeNodeInterfaceId;
		assignment->remoteAnnouncementDataChannel = XME_CORE_DATACHANNEL_INVALID_DATACHANNEL;
		assignment->remoteModifyRoutingTableDataChannel = XME_CORE_DATACHANNEL_INVALID_DATACHANNEL;
		assignment->responseInstanceHandle = XME_CORE_RR_INVALID_RESPONSE_INSTANCE_HANDLE;
	}

	// Actually, we would have to manage a list here that contains
	// all relevant response instance handles that correspond to
	// the given node (to guarantee that all requests are actually
	// replied to in case multiple requests for the same node appear
	// in fast succession). However, we simplify it here by only
	// remembering the most recent response instance handle.
	if (XME_CORE_RR_INVALID_RESPONSE_INSTANCE_HANDLE != assignment->responseInstanceHandle)
	{
		xme_core_rr_sendResponse(XME_CORE_RR_STATUS_SERVER_ERROR, config->loginRequestHandlerHandle, assignment->responseInstanceHandle, NULL, 0);
	}
	assignment->responseInstanceHandle = responseInstanceHandle;

	// Ask the directory for management channel numbers for the new node.
	// Since the response will be delivered asynchronously, we return from
	// this function and send our response at a later point in time.
	{
		xme_core_topic_newNodeRequestData_t newNodeRequest;
		newNodeRequest.newNodeId = assignment->nodeId;
		newNodeRequest.newNodeInterface = loginRequest->newNodeInterfaceId;
		newNodeRequest.edgeNodeId = loginRequest->edgeNodeId;
		newNodeRequest.edgeNodeInterface = loginRequest->edgeNodeInterfaceId;

		XME_CHECK
		(
			XME_CORE_RR_INVALID_REQUEST_INSTANCE_HANDLE !=
			(
				xme_core_rr_sendRequest(config->newNodeRequestHandle, &newNodeRequest, sizeof(newNodeRequest), assignment, 10000)
			),
			XME_CORE_RR_STATUS_SERVER_ERROR
		);
	}

	return XME_CORE_RR_STATUS_RESPONSE_DELAYED;
}

static
void
xme_adv_loginServer_receiveNewNodeResponse
(
	xme_core_rr_responseStatus_t status,
	xme_core_rr_requestHandle_t request,
	xme_core_rr_requestInstanceHandle_t requestInstance,
	xme_core_topic_t responseTopic,
	void* responseData,
	xme_core_md_topicMetaDataHandle_t responseMetaData,
	void* userData,
	void* instanceUserData
)
{
	// Prepare node identifier assignment
	xme_core_topic_loginResponseData_t loginResponse;
	xme_adv_loginServer_configStruct_t* config = (xme_adv_loginServer_configStruct_t*)userData;
	xme_adv_loginServer_nodeIdAssignment_t* assignment = (xme_adv_loginServer_nodeIdAssignment_t*)instanceUserData;
	xme_core_topic_newNodeResponseData_t* newNodeResponse = (xme_core_topic_newNodeResponseData_t*)responseData;

	XME_ASSERT_NORVAL(XME_CORE_RR_STATUS_SUCCESS == status);
	XME_CHECK(XME_CORE_RR_INVALID_RESPONSE_INSTANCE_HANDLE != assignment->responseInstanceHandle, );

	assignment->remoteAnnouncementDataChannel = newNodeResponse->remoteAnnouncementDataChannel;
	assignment->remoteModifyRoutingTableDataChannel = newNodeResponse->remoteModifyRoutingTableDataChannel;

	XME_LOG
	(
		XME_LOG_VERBOSE, "Sending initial node identifier assignment (DT=0x%08X%08X, GUID=0x%08X%08X, N*=%d, E=%d:%d, AC=%d, MC=%d)\n",
		(uint32_t)(assignment->deviceType >> 32), (uint32_t)(assignment->deviceType & 0xFFFFFFFF),
		(uint32_t)(assignment->deviceGuid >> 32), (uint32_t)(assignment->deviceGuid & 0xFFFFFFFF),
		assignment->nodeId,
		assignment->edgeNodeId, assignment->edgeNodeInterfaceId,
		assignment->remoteAnnouncementDataChannel,
		assignment->remoteModifyRoutingTableDataChannel
	);

	// Prepare the reply
	loginResponse.deviceType = assignment->deviceType;
	loginResponse.deviceGuid = assignment->deviceGuid;
	loginResponse.newNodeId = assignment->nodeId;
	loginResponse.edgeNodeId = assignment->edgeNodeId;
	loginResponse.edgeNodeInterfaceId = assignment->edgeNodeInterfaceId;
	loginResponse.remoteAnnouncementDataChannel = assignment->remoteAnnouncementDataChannel;
	loginResponse.remoteModifyRoutingTableDataChannel = assignment->remoteModifyRoutingTableDataChannel;

	xme_core_rr_sendResponse(XME_CORE_RR_STATUS_SUCCESS, config->loginRequestHandlerHandle, assignment->responseInstanceHandle, &loginResponse, sizeof(loginResponse));
	assignment->responseInstanceHandle = XME_CORE_RR_INVALID_RESPONSE_INSTANCE_HANDLE;
}

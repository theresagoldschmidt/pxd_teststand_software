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
 *         Node manager.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/nodeManager.h"

#include "xme/core/defines.h"
#include "xme/core/resourceManager.h"
#include "xme/core/topic.h"

#include "xme/hal/random.h"
#include "xme/hal/sharedPtr.h"

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
static xme_core_nodeManager_configStruct_t* xme_core_nodeManager_staticConfig = NULL;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Callback function for sending login requests.
 *
 * \see    xme_hal_sched_taskCallback_t
 */
void
xme_core_nodeManager_taskPerformLogin(void* userData);

/**
 * \brief  Callback function for login response handling.
 *
 * \see    xme_core_rr_receiveResponseCallback_t
 */
void
xme_core_nodeManager_receiveLoginResponse
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

/*
register()
 1. fordert für das "präferierte" Subnetz eine Transport-Adresse an basierend auf einer
    potenziell generierten physikalischen Adresse (z.B. MAC, Zufallszahl)
     - z.B. bei Verwendung von IP wird ein DHCP-Request gesendet (z.B. Ethernet, WLAN)
     - z.B. bei Zigbee-Funkknoten wird
 2. fordert globale NodeId an
 3. fordert für alle weiteren Subnetze, mit denen der Knoten verbunden ist, eine Transport-Adresse an,
    basierend auf einer potenziell generierten physikalischen Adresse und der zugewiesenen globalen
	NodeId
*/

/*
TODO
xme_core_status_t
xme_core_login_registerIpDhcp(interface_t interf, xme_core_dcc_nodeId* outNodeId)
{
	XME_ASSERT(0 != outNodeId);

	xme_hal_net_sendDhcpRequest(interf);

	wait_for_reply();

	*outNodeId = replyNodeId();
}

#define calculateRTManagementAddTopicChannelFromNodeId(nodeId) ((xme_core_dcc_topicChannel_t)(nodeId) << 16 + 0x0000)
#define calculateRTManagementRemTopicChannelFromNodeId(nodeId) ((xme_core_dcc_topicChannel_t)(nodeId) << 16 + 0x0001)

xme_core_status_t
xme_core_login_register()
{
	xme_core_dcc_nodeId nodeId;

	// IP-Netzwerk ist vorhanden und wird präferiert
	intf = eth1;
	while (XME_CORE_STATUS_SUCCESS != xme_core_login_registerTransportLayer(intf)) // z.B. DHCP
	{
		next = nextIntf();
	}

	if (!transportAddress)
	{
		// Try again later
	}

	xme_core_login_register(intf, &nodeId);

	// TODO: Unterscheidung zwischen Netzwerk-RT und Daten-RT

	// Beispiel:
	// NodeId = 0xAABB;
	// addId = 0xAABB0000;
	// remId = 0xAABB0001;

	// Jeder Knoten braucht Datenbank der lokalen "Funktionsnamen"
	// ("Ordinal", Listenindex) mit Funktionspointer

	// Topic-Datenformat für add und remove:
	// (TopicChannel, Ordinal)
	// Nächster zu verwendender "Ordinal"/Listenindex wird entweder
	// vor der Installation einer neuen Komponente abgefragt oder
	// ist an den zetralen Stelle, von der aus Komponenten nachgeladen
	// werden, bekannt.

	// Physikalische Adressvergabe: Zweistufiger Ansatz
	// z.B. bei IP-Netz:
	// 1. Node-ID ist initial Null. Lokale Nachrichten können damit zugestellt werden.
	//    Routing-Tabelle kann Einträge mit Nuller-High-Bytes enthalten (d.h. lokale Zustellung).
	// 2. DHCP-Anfrage (Broadcast) zur Zuweisung einer Transport-Adresse (IP-Adresse)
	// 3. Anfrage (Broadcast) zur Zuweisung einer Node-ID
	// 4. Routing-Tabelle wird aktualisiert: Einträge mit "lokaler" Zustellung (High-Bytes Null)
	//    werden auf entsprechende Node-Id geändert

	// 1. "XME over IP"-Komponente überprüft zyklisch die Interfaces auf ihren Zustand (up oder down)
	//    (zyklische Ausführung über Schedule im Broker o.?)
	// 2. Falls Interface-Zustand geändert, schickt diese ein entsprechendes Datum

	xme_core_routingTable_addEntry(calculateRTManagementAddIdFromNodeId(nodeId), &RTManagementComp.add);
	xme_core_routingTable_addEntry(calculateRTManagementRemIdFromNodeId(nodeId), &RTManagementComp.remove);

	for (...)
	{
		xme_core_login_registerIpDhcpWithNodeId(intf, nodeId);
	}
}
*/

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_core_nodeManager_create(xme_core_nodeManager_configStruct_t* config)
{
	XME_CHECK(NULL == xme_core_nodeManager_staticConfig, XME_CORE_STATUS_INVALID_CONFIGURATION);
	xme_core_nodeManager_staticConfig = config;

	// Initialize configuration structure
	config->nodeId = XME_CORE_NODE_LOCAL_NODE_ID;
	config->loginRequestHandle = XME_CORE_RR_INVALID_REQUEST_HANDLE;
	config->managementChannelsToEdgeNode = XME_CORE_DCC_INVALID_PUBLICATION_HANDLE;
	config->loginTaskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;

	// Generate random GUID if requested
	if (XME_CORE_DEVICE_GUID_RANDOM == config->deviceGuid)
	{
		config->deviceGuid =
			((xme_core_device_guid_t)xme_hal_random_randRange(0x0000, 0xFFFF)) << 48 |
			((xme_core_device_guid_t)xme_hal_random_randRange(0x0000, 0xFFFF)) << 32 |
			((xme_core_device_guid_t)xme_hal_random_randRange(0x0000, 0xFFFF)) << 16 |
			((xme_core_device_guid_t)xme_hal_random_randRange(0x0000, 0xFFFF));
	}

	// Publish login request
	XME_CHECK
	(
		XME_CORE_RR_INVALID_REQUEST_HANDLE !=
		(
			config->loginRequestHandle =
				xme_core_rr_publishRequest
				(
					XME_CORE_TOPIC_LOGIN_REQUEST,
					XME_CORE_MD_EMPTY_META_DATA,
					XME_CORE_TOPIC_LOGIN_RESPONSE,
					XME_CORE_MD_EMPTY_META_DATA,
					false,
					true,
					&xme_core_nodeManager_receiveLoginResponse,
					config
				)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// Publish managment channels to edge node topic
	XME_CHECK
	(
		XME_CORE_DCC_INVALID_PUBLICATION_HANDLE !=
		(
			config->managementChannelsToEdgeNode =
				xme_core_dcc_publishTopic
				(
					XME_CORE_TOPIC_MANAGEMENT_CHANNELS_TO_EDGE_NODE,
					XME_CORE_MD_EMPTY_META_DATA,
					true,
					NULL
				)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// Create login task
	XME_CHECK_REC
	(
		XME_HAL_SCHED_INVALID_TASK_HANDLE !=
		(
			// If sending the login request fails, retry every 10 seconds.
			// Otherwise, the task will get suspended until a request timeout occurrs (see below).
			config->loginTaskHandle =
				xme_core_resourceManager_scheduleTask
				(
					XME_HAL_SCHED_TASK_INITIALLY_SUSPENDED,
					10000,
					XME_HAL_SCHED_PRIORITY_NORMAL,
					&xme_core_nodeManager_taskPerformLogin,
					config
				)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES,
		{
			xme_core_rr_unpublishRequest(config->loginRequestHandle);
			config->loginRequestHandle = XME_CORE_RR_INVALID_REQUEST_HANDLE;
		}
	);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_nodeManager_activate(xme_core_nodeManager_configStruct_t* config)
{
	// Activate the login task
	if (XME_HAL_SCHED_INVALID_TASK_HANDLE != config->loginTaskHandle)
	{
		xme_hal_sched_setTaskExecutionState(config->loginTaskHandle, true);
	}

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_core_nodeManager_deactivate(xme_core_nodeManager_configStruct_t* config)
{
	// Suspend the login task
	if (XME_HAL_SCHED_INVALID_TASK_HANDLE != config->loginTaskHandle)
	{
		xme_hal_sched_setTaskExecutionState(config->loginTaskHandle, false);
	}
}

void
xme_core_nodeManager_destroy(xme_core_nodeManager_configStruct_t* config)
{
	xme_core_status_t rval;

	if (XME_HAL_SCHED_INVALID_TASK_HANDLE != config->loginTaskHandle)
	{
		rval = xme_hal_sched_removeTask(config->loginTaskHandle);
		XME_ASSERT_NORVAL(XME_CORE_STATUS_SUCCESS == rval);
	}

	rval = xme_core_dcc_unpublishTopic(config->managementChannelsToEdgeNode);
	XME_ASSERT_NORVAL(XME_CORE_STATUS_SUCCESS == rval);

	rval = xme_core_rr_unpublishRequest(config->loginRequestHandle);
	XME_ASSERT_NORVAL(XME_CORE_STATUS_SUCCESS == rval);

	// Do not reset Node Manager's static configuration on destruction,
	// otherwise subsequently destroyed components will not be able to
	// determine the node identifier any more. This issue should be fixed
	// by moving the node identifier storage from the Node Manager into
	// the Resource Manager (ticket #650).
	//xme_core_nodeManager_staticConfig = NULL;
}

void
xme_core_nodeManager_taskPerformLogin(void* userData)
{
	xme_core_topic_loginRequestData_t* loginRequest;
	xme_core_nodeManager_configStruct_t* config = (xme_core_nodeManager_configStruct_t*)userData;
	XME_ASSERT_NORVAL(NULL != config);
	XME_ASSERT_NORVAL(XME_HAL_SCHED_INVALID_TASK_HANDLE != config->loginTaskHandle);
	XME_ASSERT_NORVAL(XME_CORE_NODE_LOCAL_NODE_ID == config->nodeId);
	
	// Prepare login request
	XME_CHECK
	(
		NULL !=
		(
			loginRequest = (xme_core_topic_loginRequestData_t*)xme_hal_mem_alloc(sizeof(xme_core_topic_loginRequestData_t))
		),
	);

	loginRequest->deviceType = config->deviceType;
	loginRequest->deviceGuid = config->deviceGuid;
	loginRequest->edgeNodeId = XME_CORE_NODE_INVALID_NODE_ID;
	loginRequest->newNodeInterfaceId = 1; // TODO: replace with real interface ID! See #547
	loginRequest->edgeNodeInterfaceId = 1; // TODO: replace with real interface ID! See #547
	XME_ASSERT_NORVAL(XME_CORE_INTERFACEMANAGER_INVALID_INTERFACE_ID != loginRequest->newNodeInterfaceId);

	// Send login request
	// TODO: Node manager should request an address on the primary interface first! See ticket #749
	XME_CHECK_REC
	(
		XME_CORE_RR_INVALID_REQUEST_INSTANCE_HANDLE !=
		(
			// Use a request timeout of 5000ms
			xme_core_rr_sendRequest
			(
				config->loginRequestHandle,
				loginRequest,
				sizeof(*loginRequest),
				NULL,
				5000
			)
		),
		,
		{
			xme_hal_mem_free(loginRequest);
		}
	);

	xme_hal_mem_free(loginRequest);

	// Suspend the task to wait for responses or a timeout condition
	//xme_hal_sched_setTaskExecutionState(config->loginRequestHandle, false); // TODO! See ticket #762
}

void
xme_core_nodeManager_receiveLoginResponse
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
	xme_core_topic_loginResponseData_t* loginResponse;
	xme_core_nodeManager_configStruct_t* config = (xme_core_nodeManager_configStruct_t*)userData;
	XME_ASSERT_NORVAL(NULL != config);

	if (config->nodeId != XME_CORE_NODE_LOCAL_NODE_ID)
	{
		// We already have a node identifier assigned
		return;
	}

	if (XME_CORE_RR_STATUS_TIMEOUT == status)
	{
		// If a timeout has occurred, reschedule the task
		xme_hal_sched_setTaskExecutionState(config->loginTaskHandle, true);
		return;
	}

	if (XME_CORE_RR_STATUS_SUCCESS != status)
	{
		// Only consider successful responses
		return;
	}

	loginResponse = (xme_core_topic_loginResponseData_t*)responseData;

	XME_ASSERT_NORVAL(config->deviceType == loginResponse->deviceType);
	XME_ASSERT_NORVAL(config->deviceGuid == loginResponse->deviceGuid);

	// NOTE: loginResponse->remoteAnnouncementDataChannel and loginResponse->remoteModifyRoutingTableDataChannel
	//       might be invalid. This means that the local node has just completed login.

	XME_LOG
	(
		XME_LOG_VERBOSE,
		"Successfully logged into the network (N=%d)\n",
		loginResponse->newNodeId
	);

	config->nodeId = loginResponse->newNodeId;

	// Cancel login task
	XME_LOG_IF
	(
		XME_CORE_STATUS_SUCCESS != xme_hal_sched_removeTask(config->loginTaskHandle),
		XME_LOG_ERROR,
		"Unable to remove login task!\n"
	);
	config->loginTaskHandle = XME_HAL_SCHED_INVALID_TASK_HANDLE;

	// Notify directory to set up management channels for new node
	{
		xme_core_topic_managementChannelsPacket_t managementChannels;

		managementChannels.interfaceId = 1; // TODO: replace with real interface ID! See #547
		managementChannels.remoteAnnouncementDataChannel = loginResponse->remoteAnnouncementDataChannel;
		managementChannels.remoteModifyRoutingTableDataChannel = loginResponse->remoteModifyRoutingTableDataChannel;

		// Publish publication
		{
			xme_core_status_t rval = xme_core_dcc_sendTopicData(config->managementChannelsToEdgeNode, &managementChannels, sizeof(managementChannels));
			XME_ASSERT_NORVAL(XME_CORE_STATUS_SUCCESS == rval);
		}
	}
}

xme_core_node_nodeId_t
xme_core_nodeManager_getNodeId()
{
	XME_CHECK(NULL != xme_core_nodeManager_staticConfig, XME_CORE_NODE_LOCAL_NODE_ID);
	return xme_core_nodeManager_staticConfig->nodeId;
}

xme_core_device_guid_t
xme_core_nodeManager_getDeviceGuid()
{
	XME_ASSERT_RVAL(NULL != xme_core_nodeManager_staticConfig, 0);
	return xme_core_nodeManager_staticConfig->deviceGuid;
}

xme_core_device_type_t
xme_core_nodeManager_getDeviceType()
{
	XME_ASSERT_RVAL(NULL != xme_core_nodeManager_staticConfig, 0);
	return xme_core_nodeManager_staticConfig->deviceType;
}

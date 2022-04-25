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
 *         Resource manager.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

// TODO (ticket #757): Invalidate reponse instance handle after response timeout (i.e., when
//                     a component asks the resource manager to send a response later, but does not
//                     actually provide one (within acceptable time)).

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/resourceManager.h"

#include "xme/core/broker.h"
#include "xme/core/dcc.h"
#include "xme/core/defines.h"
#include "xme/core/directory.h"
#include "xme/core/component.h"
#include "xme/core/nodeManager.h"
#include "xme/core/packet.h"

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
xme_core_resourceManager_configStruct_t xme_core_resourceManager_config;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates a new port for the given component.
 *
 * \param  component Component to create the new port for.
 *
 * \return Port handle of the newly created port.
 */
static
xme_core_component_port_t
xme_core_resourceManager_createPort
(
	xme_core_component_t component,
	xme_core_component_portType_t portType,
	xme_core_resourceManager_componentPortInfo_t** portInfo
);

/**
 * \brief  Callback function for receiving an announcement request at the resource manager.
 */
static
void
xme_core_resourceManager_receiveAnnouncementRequestCallback
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_core_resourceManager_init()
{
	xme_core_component_t i;

	// Initialize configuration structure
	XME_HAL_TABLE_INIT(xme_core_resourceManager_config.components);
	XME_HAL_TABLE_INIT(xme_core_resourceManager_config.localPublications);
	XME_HAL_TABLE_INIT(xme_core_resourceManager_config.localSubscriptions);
	XME_HAL_TABLE_INIT(xme_core_resourceManager_config.localRequestSenders);
	XME_HAL_TABLE_INIT(xme_core_resourceManager_config.localRequestHandlers);
	XME_HAL_TABLE_INIT(xme_core_resourceManager_config.localResponseInstances);
	xme_core_resourceManager_config.localAnnouncementDataChannel = XME_CORE_DATACHANNEL_INVALID_DATACHANNEL;
	xme_core_resourceManager_config.localAnnouncementPort = XME_CORE_COMPONENT_INVALID_COMPONENT_PORT;
	xme_core_resourceManager_config.currentComponentIdTlsHandle = XME_HAL_TLS_INVALID_TLS_HANDLE;

	// Register thread local storage for the component context
	XME_CHECK
	(
		XME_HAL_TLS_INVALID_TLS_HANDLE != (xme_core_resourceManager_config.currentComponentIdTlsHandle = xme_hal_tls_alloc(sizeof(xme_core_component_t))),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	// Create software components
	for (i=(xme_core_component_t)0; i<xme_core_resourceManager_componentCount; i=(xme_core_component_t)(((unsigned short)i)+1))
	{
		// If this item is in use...
		if (xme_core_resourceManager_componentDescriptors[i].create)
		{
			// ...create an information structure for the component and call the creation routine
			xme_core_resourceManager_componentInfo_t* item;

			// TODO: On error, clean up components initialized properly so far! See ticket #758
			XME_CHECK
			(
				XME_HAL_TABLE_INVALID_ROW_HANDLE !=
				(
					xme_core_resourceManager_componentDescriptors[i].componentId = (xme_core_component_t)XME_HAL_TABLE_ADD_ITEM(xme_core_resourceManager_config.components)
				),
				XME_CORE_STATUS_OUT_OF_RESOURCES
			);

			XME_LOG
			(
				XME_LOG_VERBOSE,
				"Creating software component \"%s\" (#%d)\n",
				xme_core_resourceManager_componentDescriptors[i].componentName,
				xme_core_resourceManager_componentDescriptors[i].componentId
			);

			item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.components, xme_core_resourceManager_componentDescriptors[i].componentId);
			XME_ASSERT(NULL != item);

			XME_HAL_TABLE_INIT(item->componentPorts);

			// Switch to the component context
			XME_COMPONENT_CONTEXT
			(
				xme_core_resourceManager_componentDescriptors[i].componentId,
				{
					// Create the component
					// TODO: On error, clean up components initialized properly so far! See ticket #758
					XME_CHECK_MSG
					(
						XME_CORE_STATUS_SUCCESS == xme_core_resourceManager_componentDescriptors[i].create(xme_core_resourceManager_componentDescriptors[i].config),
						XME_CORE_STATUS_OUT_OF_RESOURCES,
						XME_LOG_FATAL,
						"Component #%d failed to initialize!\n",
						xme_core_resourceManager_componentDescriptors[i].componentId
					);
				}
			);
		}
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_resourceManager_create(xme_core_resourceManager_configStruct_t* config)
{
	// TODO (ticket #759): Currently, NULL is passed in the config parameter.
	//                     However, there should either be a separate config struct for the component part of the resource manager
	//                     or it should be the same struct that is already used in resourceManager_init().

	XME_ASSERT(XME_ASSERT_NO_SIDE_EFFECTS(XME_CORE_COMPONENT_RESOURCEMANAGER_COMPONENT_ID == xme_core_resourceManager_getCurrentComponentId()));

	// Create system internal data channel
	xme_core_resourceManager_config.localAnnouncementDataChannel = xme_core_directory_getDataChannel
	(
		XME_CORE_TOPIC_LOCAL_ANNOUNCEMENT,
		XME_CORE_MD_EMPTY_META_DATA,
		false
	);
	XME_ASSERT(XME_CORE_DATACHANNEL_INVALID_DATACHANNEL != xme_core_resourceManager_config.localAnnouncementDataChannel);

	// TODO: Error handling! See ticket #721
	xme_core_resourceManager_config.localAnnouncementPort = xme_core_resourceManager_addPublicationPort(xme_core_resourceManager_getCurrentComponentId());
	xme_core_routingTable_addLocalSourceRoute(xme_core_resourceManager_config.localAnnouncementDataChannel, xme_core_resourceManager_getCurrentComponentId(), xme_core_resourceManager_config.localAnnouncementPort);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_resourceManager_activate(xme_core_resourceManager_configStruct_t* config)
{
	// Nothing to do
	return XME_CORE_STATUS_SUCCESS;
}

void
xme_core_resourceManager_deactivate(xme_core_resourceManager_configStruct_t* config)
{
	// Nothing to do
}

void
xme_core_resourceManager_destroy(xme_core_resourceManager_configStruct_t* config)
{
	xme_core_routingTable_removeLocalSourceRoute(xme_core_resourceManager_config.localAnnouncementDataChannel);
	xme_core_resourceManager_removePort(xme_core_resourceManager_getCurrentComponentId(), xme_core_resourceManager_config.localAnnouncementPort);
}

xme_core_status_t
xme_core_resourceManager_activateComponents()
{
	xme_core_component_t i;

	// Activate software components
	for (i=(xme_core_component_t)0; i<xme_core_resourceManager_componentCount; i=(xme_core_component_t)(((unsigned short)i)+1))
	{
		// If this item is in use...
		if (xme_core_resourceManager_componentDescriptors[i].activate)
		{
			XME_COMPONENT_CONTEXT
			(
				xme_core_resourceManager_componentDescriptors[i].componentId,
				{
					// ...activate the respective component
					XME_CHECK_MSG
					(
						XME_CORE_STATUS_SUCCESS == xme_core_resourceManager_componentDescriptors[i].activate(xme_core_resourceManager_componentDescriptors[i].config),
						XME_CORE_STATUS_INTERNAL_ERROR,
						XME_LOG_FATAL,
						"Component #%d failed to activate!\n",
						xme_core_resourceManager_componentDescriptors[i].componentId
					);
				}
			);
		}
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_resourceManager_deactivateComponents()
{
	xme_core_component_t i;

	// Deactivate software components
	for (i=xme_core_resourceManager_componentCount; i>(xme_core_component_t)0; i=(xme_core_component_t)(((unsigned short)i)-1))
	{
		// If this item is in use...
		if (xme_core_resourceManager_componentDescriptors[i-1].deactivate)
		{
			XME_COMPONENT_CONTEXT
			(
				xme_core_resourceManager_componentDescriptors[i-1].componentId,
				{
					// ...deactivate the respective component
					xme_core_resourceManager_componentDescriptors[i-1].deactivate(xme_core_resourceManager_componentDescriptors[i-1].config);
				}
			);
		}
	}

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_core_resourceManager_fini()
{
	xme_core_component_t i;

	// Destroy software components
	for (i=xme_core_resourceManager_componentCount; i>(xme_core_component_t)0; i=(xme_core_component_t)(((unsigned short)i)-1))
	{
		// If this item is in use...
		if (xme_core_resourceManager_componentDescriptors[i-1].destroy)
		{
			// ...call the destruction routine and destroy the information structure for the component
			xme_core_resourceManager_componentInfo_t* item;

			XME_COMPONENT_CONTEXT
			(
				xme_core_resourceManager_componentDescriptors[i-1].componentId,
				{
					xme_core_resourceManager_componentDescriptors[i-1].destroy(xme_core_resourceManager_componentDescriptors[i-1].config);
				}
			);

			item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.components, xme_core_resourceManager_componentDescriptors[i-1].componentId);
			XME_ASSERT_NORVAL(NULL != item);

			XME_HAL_TABLE_FINI(item->componentPorts);
		}
	}

	// Clean up resource manager component
	{
		xme_core_resourceManager_componentInfo_t* item;

		xme_core_routingTable_removeLocalSourceRoute(xme_core_resourceManager_config.localAnnouncementDataChannel);
		xme_core_resourceManager_removePort(XME_CORE_COMPONENT_RESOURCEMANAGER_COMPONENT_ID, xme_core_resourceManager_config.localAnnouncementPort);
		xme_core_resourceManager_config.localAnnouncementPort = XME_CORE_COMPONENT_INVALID_COMPONENT_PORT;

		item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.components, XME_CORE_COMPONENT_RESOURCEMANAGER_COMPONENT_ID);
		XME_ASSERT_NORVAL(NULL != item);
		XME_HAL_TABLE_FINI(item->componentPorts);

		XME_HAL_TABLE_REMOVE_ITEM(xme_core_resourceManager_config.components, (xme_hal_table_rowHandle_t)XME_CORE_COMPONENT_RESOURCEMANAGER_COMPONENT_ID);
	}

	// Deregister thread local storage for the component context
	xme_hal_tls_free(xme_core_resourceManager_config.currentComponentIdTlsHandle);

	XME_HAL_TABLE_FINI(xme_core_resourceManager_config.localResponseInstances);
	XME_HAL_TABLE_FINI(xme_core_resourceManager_config.localRequestHandlers);
	XME_HAL_TABLE_FINI(xme_core_resourceManager_config.localRequestSenders);
	XME_HAL_TABLE_FINI(xme_core_resourceManager_config.localSubscriptions);
	XME_HAL_TABLE_FINI(xme_core_resourceManager_config.localPublications);
	XME_HAL_TABLE_FINI(xme_core_resourceManager_config.components);
}





void
xme_core_resourceManager_setCurrentComponentId
(
	xme_core_component_t componentId
)
{
	void* data = xme_hal_tls_get(xme_core_resourceManager_config.currentComponentIdTlsHandle);
	if (NULL == data && XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT == componentId) return;
	XME_ASSERT_NORVAL(NULL != data);
	*((xme_core_component_t*)data) = componentId;
}

xme_core_component_t
xme_core_resourceManager_getCurrentComponentId()
{
	void* data = xme_hal_tls_get(xme_core_resourceManager_config.currentComponentIdTlsHandle);
	return (NULL != data) ? *((xme_core_component_t*)data) : XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT;
}

const char*
xme_core_resourceManager_getComponentName
(
	xme_core_component_t componentId
)
{
	xme_core_component_t i;

	for (i=(xme_core_component_t)0; i<xme_core_resourceManager_componentCount; i=(xme_core_component_t)(((unsigned short)i)+1))
	{
		if (xme_core_resourceManager_componentDescriptors[i].componentId == componentId)
		{
			return xme_core_resourceManager_componentDescriptors[i].componentName;
		}
	}
	
	// When no component with given id has been found return NULL
	return NULL;
}




xme_core_resourceManager_taskHandle_t
xme_core_resourceManager_scheduleTask
(
	xme_hal_time_interval_t startMs,
	xme_hal_time_interval_t periodMs,
	uint8_t priority,
	xme_hal_sched_taskCallback_t callback,
	void* userData
)
{
	// TODO: See ticket #763
	//if (!feasible)
	//{
	//	return XME_CORE_RESOURCEMANAGER_TASK_INFEASIBLE;
	//}

	return xme_hal_sched_addTask(startMs, periodMs, priority, callback, userData);
}

xme_core_status_t
xme_core_resourceManager_killTask(xme_core_resourceManager_taskHandle_t taskHandle)
{
	return xme_hal_sched_removeTask(taskHandle);
}

bool
xme_core_resourceManager_idle()
{
	// TODO: See ticket #760
	return true;
}

void
xme_core_resourceManager_runTask()
{
}

xme_core_status_t
xme_core_resourceManager_notifyComponent
(
	xme_core_dataChannel_t sourceDataChannel,
	xme_core_component_t toComponent,
	xme_core_component_port_t toPort,
	xme_hal_sharedPtr_t dataHandle
)
{
	xme_core_resourceManager_componentInfo_t* componentInfo;
	xme_core_resourceManager_componentPortInfo_t* portInfo;

	componentInfo = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.components, toComponent);
	XME_ASSERT(NULL != componentInfo);

	portInfo = XME_HAL_TABLE_ITEM_FROM_HANDLE(componentInfo->componentPorts, toPort);
	XME_ASSERT(NULL != portInfo);

	XME_LOG
	(
		XME_LOG_VERBOSE,
		"Notify component #%d, port #%d (type %s) with data handle #%d\n",
		toComponent, toPort,
		XME_CORE_COMPONENT_PORTTYPE_DCC_SUBSCRIPTION == portInfo->portType ? "DCC_SUB" : (
			XME_CORE_COMPONENT_PORTTYPE_RR_REQUEST_HANDLER == portInfo->portType ? "RR_RQH" : (
			XME_CORE_COMPONENT_PORTTYPE_RR_RESPONSE_HANDLER == portInfo->portType ? "RR_RSH" : "INVALID"
			)
		),
		dataHandle
	);

	switch (portInfo->portType)
	{
		case XME_CORE_COMPONENT_PORTTYPE_DCC_SUBSCRIPTION:
			XME_COMPONENT_CONTEXT
			(
				toComponent,
				{
					// callback is an xme_core_dcc_receiveTopicCallback_t
					portInfo->subscriptionCallback.callback
					(
						dataHandle,
						portInfo->subscriptionCallback.userData
					);
				}
			);

			return XME_CORE_STATUS_SUCCESS;
			//break;

		case XME_CORE_COMPONENT_PORTTYPE_RR_REQUEST_HANDLER:
		{
			XME_HAL_TABLE_ITERATE_BEGIN
			(
				xme_core_resourceManager_config.localRequestHandlers,
				xme_hal_table_rowHandle_t, itemHandle,
				xme_core_resourceManager_requestHandlerItem_t, item
			);
			{
				if (item->requestHandlingComponent == toComponent && item->requestHandlingPort == toPort)
				{
					unsigned short responseSize = 32;
					xme_hal_sharedPtr_t responseHandle = XME_HAL_SHAREDPTR_INVALID_POINTER;
					xme_core_packet_requestContext_t* requestContext = (xme_core_packet_requestContext_t*)xme_hal_sharedPtr_getPointer(dataHandle);
					xme_core_rr_responseInstanceHandle_t responseInstanceHandle;
					xme_core_resourceManager_responseInstanceItem_t* reponseInstanceItem;

					xme_core_dataChannel_t responseDataChannel = (xme_core_dataChannel_t)(((xme_hal_table_rowHandle_t)sourceDataChannel)+1); // TODO: See ticket #544

					// Generate a unique response instance handle in case the response needs to be delayed
					XME_CHECK
					(
						XME_HAL_TABLE_INVALID_ROW_HANDLE != (responseInstanceHandle = (xme_core_rr_responseInstanceHandle_t)XME_HAL_TABLE_ADD_ITEM(xme_core_resourceManager_config.localResponseInstances)),
						XME_CORE_STATUS_OUT_OF_RESOURCES
					);

					// Set up the response instance structure
					reponseInstanceItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localResponseInstances, (xme_hal_table_rowHandle_t)responseInstanceHandle);
					XME_ASSERT(NULL != reponseInstanceItem);
					reponseInstanceItem->localRequestHandle = requestContext->localRequestHandle;
					reponseInstanceItem->localRequestInstance = requestContext->localRequestInstance;
					reponseInstanceItem->responseDataChannel = responseDataChannel;

					do
					{
						xme_core_packet_responseContext_t* responseContext;
						unsigned short newResponseSize = responseSize;

						responseHandle = xme_hal_sharedPtr_realloc(responseHandle, responseSize + sizeof(xme_core_packet_responseContext_t));
						XME_ASSERT(XME_HAL_SHAREDPTR_INVALID_POINTER != responseHandle);

						responseContext = (xme_core_packet_responseContext_t*)xme_hal_sharedPtr_getPointer(responseHandle);
						XME_ASSERT(NULL != responseContext);

						// Copy over the request context data to the response context
						responseContext->localRequestHandle = requestContext->localRequestHandle;
						responseContext->localRequestInstance = requestContext->localRequestInstance;
						responseContext->responseTopic = item->responseTopic;
						responseContext->instanceUserData = requestContext->instanceUserData;

						XME_COMPONENT_CONTEXT
						(
							toComponent,
							{
								// Callback is an xme_core_rr_receiveRequestCallback_t
								responseContext->responseStatus = portInfo->requestHandlerCallback.callback
								(
									item->requestTopic,
									((char*)requestContext) + sizeof(xme_core_packet_requestContext_t),
									responseInstanceHandle,
									((char*)responseContext) + sizeof(xme_core_packet_responseContext_t),
									&newResponseSize,
									0, // TODO: responseTimeoutMs. See ticket #764
									portInfo->requestHandlerCallback.userData
								);
							}
						);

						// It is not allowed for the receive request callback to return XME_CORE_RR_STATUS_TIMEOUT
						XME_ASSERT(XME_CORE_RR_STATUS_TIMEOUT != responseContext->responseStatus);

						// Either the buffer was too small and a larger buffer size has been specified
						// or it was enough and the response status indicates that it was not too small
						XME_ASSERT((XME_CORE_RR_STATUS_BUFFER_TOO_SMALL == responseContext->responseStatus) ^ (newResponseSize <= responseSize));
						responseSize = newResponseSize;

						switch (responseContext->responseStatus)
						{
							case XME_CORE_RR_STATUS_BUFFER_TOO_SMALL:
							{
								// Retry with a correctly-sized response buffer.
								// Response instance handle stays the same.
								break;
							}

							case XME_CORE_RR_STATUS_RESPONSE_DELAYED:
							{
								// Response is delayed, nothing special to do at this point in time.
								// Especially, do *not* delete the response instance handle.

								// Delete the response handle
								xme_hal_sharedPtr_destroy(responseHandle);

								return XME_CORE_STATUS_SUCCESS;
							}

							case XME_CORE_RR_STATUS_SERVER_ERROR:
								// Server error, send an empty response
								responseSize = 0;

								// fall-through

							case XME_CORE_RR_STATUS_SUCCESS:
								// fall-through

							default:
							{
								// Success or application defined status, send response

								// TODO: Error handling! See ticket #721
								xme_core_broker_rrSendResponse
								(
									responseDataChannel,
									item->requestHandlingComponent,
									item->responseSendingPort,
									responseHandle
								);

								// Delete the response handle
								xme_hal_sharedPtr_destroy(responseHandle);

								// Do not delete response instance handle.
								// It has already been removed by xme_core_rr_sendResponse().
								// TODO: Somehow this assertion fails. Debug this! MG. See ticket #750
								//XME_ASSERT(NULL == XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localResponseInstances, (xme_hal_table_rowHandle_t)responseInstanceHandle));

								return XME_CORE_STATUS_SUCCESS;
							}
						}
					}
					while (1);
				}
			}
			XME_HAL_TABLE_ITERATE_END();

			return XME_CORE_STATUS_INVALID_PARAMETER;
		}

		case XME_CORE_COMPONENT_PORTTYPE_RR_RESPONSE_HANDLER:
		{
			XME_HAL_TABLE_ITERATE
			(
				xme_core_resourceManager_config.localRequestSenders,
				xme_hal_table_rowHandle_t, itemHandle,
				xme_core_resourceManager_requestSenderItem_t, item,
				{
					if (item->requestingComponent == toComponent && item->responseHandlingPort == toPort)
					{
						xme_core_packet_responseContext_t* responseContext = (xme_core_packet_responseContext_t*)xme_hal_sharedPtr_getPointer(dataHandle);

						XME_COMPONENT_CONTEXT
						(
							toComponent,
							{
								// Callback is an xme_core_rr_receiveResponseCallback_t
								portInfo->responseHandlerCallback.callback
								(
									responseContext->responseStatus,
									responseContext->localRequestHandle,
									responseContext->localRequestInstance,
									responseContext->responseTopic,
									((char*)responseContext) + sizeof(xme_core_packet_responseContext_t),
									(xme_core_md_topicMetaDataHandle_t)0, //xme_core_md_topicMetaDataHandle_t responseMetaData, // TODO: See ticket #646
									portInfo->responseHandlerCallback.userData,
									(void*)responseContext->instanceUserData // TODO: See ticket #540
								);
							}
						);

						return XME_CORE_STATUS_SUCCESS;
					}
				}
			);

			return XME_CORE_STATUS_INVALID_PARAMETER;
		}

		default:
			// Raise an assertion failure
			XME_ASSERT
			(
				XME_CORE_COMPONENT_PORTTYPE_DCC_SUBSCRIPTION == portInfo->portType ||
				XME_CORE_COMPONENT_PORTTYPE_RR_REQUEST_HANDLER == portInfo->portType ||
				XME_CORE_COMPONENT_PORTTYPE_RR_RESPONSE_HANDLER == portInfo->portType
			);

			return XME_CORE_STATUS_INVALID_PARAMETER;
	}
}

static
xme_core_component_port_t
xme_core_resourceManager_createPort
(
	xme_core_component_t component,
	xme_core_component_portType_t portType,
	xme_core_resourceManager_componentPortInfo_t** portInfo
)
{
	xme_core_resourceManager_componentInfo_t* componentInfo;
	xme_core_resourceManager_componentPortInfo_t* portInfoTemp;
	xme_core_component_port_t port;

	componentInfo = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.components, component);
	XME_ASSERT_RVAL(NULL != componentInfo, XME_CORE_COMPONENT_INVALID_COMPONENT_PORT);

	XME_CHECK
	(
		XME_CORE_COMPONENT_INVALID_COMPONENT_PORT !=
		(
			port = (xme_core_component_port_t)XME_HAL_TABLE_ADD_ITEM(componentInfo->componentPorts)
		),
		XME_CORE_COMPONENT_INVALID_COMPONENT_PORT
	);

	portInfoTemp = XME_HAL_TABLE_ITEM_FROM_HANDLE(componentInfo->componentPorts, port);
	XME_ASSERT_RVAL(NULL != portInfoTemp, XME_CORE_COMPONENT_INVALID_COMPONENT_PORT);

	portInfoTemp->portType = portType;

	if (NULL != portInfo)
	{
		*portInfo = portInfoTemp;
	}

	return port;
}

xme_core_component_port_t
xme_core_resourceManager_addPublicationPort
(
	xme_core_component_t component
)
{
	return xme_core_resourceManager_createPort(component, XME_CORE_COMPONENT_PORTTYPE_DCC_PUBLICATION, NULL);
}

xme_core_component_port_t
xme_core_resourceManager_addSubscriptionPort
(
	xme_core_component_t component,
	xme_core_dcc_receiveTopicCallback_t receiveTopicCallback,
	void* receiveTopicUserData
)
{
	xme_core_resourceManager_componentPortInfo_t* portInfo;
	xme_core_component_port_t port = xme_core_resourceManager_createPort(component, XME_CORE_COMPONENT_PORTTYPE_DCC_SUBSCRIPTION, &portInfo);

	portInfo->subscriptionCallback.callback = receiveTopicCallback;
	portInfo->subscriptionCallback.userData = receiveTopicUserData;

	return port;
}

xme_core_component_port_t
xme_core_resourceManager_addRequestSenderPort
(
	xme_core_component_t component
)
{
	return xme_core_resourceManager_createPort(component, XME_CORE_COMPONENT_PORTTYPE_RR_REQUEST_SENDER, NULL);
}

xme_core_component_port_t
xme_core_resourceManager_addResponseHandlerPort
(
	xme_core_component_t component,
	xme_core_rr_receiveResponseCallback_t receiveResponseCallback,
	void* receiveResponseUserData
)
{
	xme_core_resourceManager_componentPortInfo_t* portInfo;
	xme_core_component_port_t port = xme_core_resourceManager_createPort(component, XME_CORE_COMPONENT_PORTTYPE_RR_RESPONSE_HANDLER, &portInfo);

	portInfo->responseHandlerCallback.callback = receiveResponseCallback;
	portInfo->responseHandlerCallback.userData = receiveResponseUserData;

	return port;
}

xme_core_component_port_t
xme_core_resourceManager_addRequestHandlerPort
(
	xme_core_component_t component,
	xme_core_rr_receiveRequestCallback_t receiveRequestCallback,
	void* receiveRequestUserData
)
{
	xme_core_resourceManager_componentPortInfo_t* portInfo;
	xme_core_component_port_t port = xme_core_resourceManager_createPort(component, XME_CORE_COMPONENT_PORTTYPE_RR_REQUEST_HANDLER, &portInfo);

	portInfo->requestHandlerCallback.callback = receiveRequestCallback;
	portInfo->requestHandlerCallback.userData = receiveRequestUserData;

	return port;
}

xme_core_component_port_t
xme_core_resourceManager_addResponseSenderPort
(
	xme_core_component_t component
)
{
	return xme_core_resourceManager_createPort(component, XME_CORE_COMPONENT_PORTTYPE_RR_RESPONSE_SENDER, NULL);
}

xme_core_status_t
xme_core_resourceManager_removePort
(
	xme_core_component_t component,
	xme_core_component_port_t port
)
{
	xme_core_resourceManager_componentInfo_t* componentInfo;

	componentInfo = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.components, component);
	XME_ASSERT(NULL != componentInfo);

	return XME_HAL_TABLE_REMOVE_ITEM(componentInfo->componentPorts, (xme_hal_table_rowHandle_t)port);
}

xme_core_component_portType_t
xme_core_resourceManager_getPortType
(
	xme_core_component_t component,
	xme_core_component_port_t port,
	bool isOutputPort
)
{
	// TODO: what is the isOutputPort parameter being used for? See ticket #765

	xme_core_resourceManager_componentInfo_t* componentInfo;
	xme_core_resourceManager_componentPortInfo_t* portInfo;

	componentInfo = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.components, component);
	XME_ASSERT_RVAL(NULL != componentInfo, XME_CORE_COMPONENT_PORTTYPE_INVALID);
	portInfo = XME_HAL_TABLE_ITEM_FROM_HANDLE(componentInfo->componentPorts, port);
	
	// TODO: Handle all different types of ports; adapt portInfo-structure!! See ticket #765
	XME_ASSERT_RVAL(NULL != portInfo, XME_CORE_COMPONENT_PORTTYPE_INVALID);

	return portInfo->portType;
}

xme_core_component_port_t
xme_core_resourceManager_getPublicationPort
(
	xme_core_dcc_publicationHandle_t publicationHandle
)
{
	xme_core_resourceManager_publicationItem_t* item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localPublications, publicationHandle);
	XME_ASSERT_RVAL(NULL != item, XME_CORE_COMPONENT_INVALID_COMPONENT_PORT);

	return item->publishingPort;
}

xme_core_component_port_t
xme_core_resourceManager_getSubscriptionPort
(
	xme_core_dcc_subscriptionHandle_t subscriptionHandle
)
{
	xme_core_resourceManager_subscriptionItem_t* item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localSubscriptions, subscriptionHandle);
	XME_ASSERT_RVAL(NULL != item, XME_CORE_COMPONENT_INVALID_COMPONENT_PORT);

	return item->subscribingPort;
}

xme_core_component_port_t
xme_core_resourceManager_getRequestSenderPort
(
	xme_core_rr_requestHandle_t requestHandle
)
{
	xme_core_resourceManager_requestSenderItem_t* item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localRequestSenders, requestHandle);
	XME_ASSERT_RVAL(NULL != item, XME_CORE_COMPONENT_INVALID_COMPONENT_PORT);

	return item->requestingPort;
}

xme_core_component_port_t
xme_core_resourceManager_getRequestHandlerPort
(
	xme_core_rr_requestHandlerHandle_t requestHandlerHandle
)
{
	xme_core_resourceManager_requestHandlerItem_t* item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_resourceManager_config.localRequestHandlers, requestHandlerHandle);
	XME_ASSERT_RVAL(NULL != item, XME_CORE_COMPONENT_INVALID_COMPONENT_PORT);

	return item->requestHandlingPort;
}

// TODO: Integrate into announcement handling (#934)
static
void
xme_core_resourceManager_receiveAnnouncementRequestCallback
(
	xme_hal_sharedPtr_t dataHandle,
	void* userData
)
{
	// TODO: Check whether appropriate resources are available! See ticket #763

	// TODO: Error handling! See ticket #721
	xme_core_broker_deliverData(xme_core_resourceManager_config.localAnnouncementDataChannel, dataHandle, xme_core_resourceManager_getCurrentComponentId());
}

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

#ifndef XME_CORE_RESOURCEMANAGER_H
#define XME_CORE_RESOURCEMANAGER_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/component.h"
#include "xme/core/core.h"
#include "xme/core/dataChannel.h"
#include "xme/core/dcc.h"
#include "xme/core/defines.h"
#include "xme/core/md.h"
#include "xme/core/rr.h"
#include "xme/core/topic.h"

#include "xme/hal/sched.h"
#include "xme/hal/sharedPtr.h"
#include "xme/hal/table.h"
#include "xme/hal/tls.h"

#include <stdint.h>

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define XME_CORE_RESOURCEMANAGER_TASK_INFEASIBLE ((xme_core_resourceManager_taskHandle_t)0) ///< Task schedule is not feasible. This must be defined as zero.

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_core_resourceManager_taskHandle_t
 *
 * \brief  Locally valid handle to a handle.
 */
typedef xme_hal_sched_taskHandle_t xme_core_resourceManager_taskHandle_t;

/**
 * \typedef xme_core_resourceManager_taskCallback_t
 *
 * \brief  Task callback function.
 */
typedef xme_hal_sched_taskCallback_t xme_core_resourceManager_taskCallback_t;

/**
 * \typedef xme_core_resourceManager_componentPortInfo_t
 *
 * \brief  Component port information structure.
 */
typedef struct
{
	xme_core_component_portType_t portType; ///< Port type.
	union
	{
		// Publication (XME_CORE_COMPONENT_PORTTYPE_DCC_PUBLICATION == portType)
		// (no further parameters)

		// Subsciption (XME_CORE_COMPONENT_PORTTYPE_DCC_SUBSCRIPTION == portType)
		struct
		{
			xme_core_dcc_receiveTopicCallback_t callback; ///< Callback function.
			void* userData; ///< User-defined data to pass to the callback function.
		}
		subscriptionCallback;

		// Request sender (XME_CORE_COMPONENT_PORTTYPE_RR_REQUEST_SENDER == portType)
		// (no further parameters)

		// Response handler (XME_CORE_COMPONENT_PORTTYPE_RR_RESPONSE_HANDLER == portType)
		struct
		{
			xme_core_rr_receiveResponseCallback_t callback; ///< Callback function for response delivery.
			void* userData; ///< User-defined data to pass to the callback function.
		}
		responseHandlerCallback;

		// Request handler (XME_CORE_COMPONENT_PORTTYPE_RR_REQUEST_HANDLER == portType)
		struct
		{
			xme_core_rr_receiveRequestCallback_t callback; ///< Callback function for request delivery.
			void* userData; ///< User-defined data to pass to the callback function.
		}
		requestHandlerCallback;

		// Response sender (XME_CORE_COMPONENT_PORTTYPE_RR_RESPONSE_SENDER == portType)
		// (no further parameters)
	};
}
xme_core_resourceManager_componentPortInfo_t;

/**
 * \typedef xme_core_resourceManager_componentInfo_t
 *
 * \brief  Component information structure.
 */
typedef struct
{
	XME_HAL_TABLE(xme_core_resourceManager_componentPortInfo_t, componentPorts, XME_HAL_DEFINES_MAX_COMPONENT_PORT_ITEMS);
}
xme_core_resourceManager_componentInfo_t;

typedef struct
{
	xme_core_component_t publishingComponent;
	xme_core_component_port_t publishingPort;
	xme_core_topic_t publishedTopic;
	xme_core_md_topicMetaDataHandle_t topicMetaDataHandle;
}
xme_core_resourceManager_publicationItem_t;

typedef struct
{
	xme_core_component_t subscribingComponent;
	xme_core_component_port_t subscribingPort;
	xme_core_topic_t subscribedTopic;
	xme_core_md_topicMetaDataHandle_t metaDataFilterHandle;
}
xme_core_resourceManager_subscriptionItem_t;

typedef struct
{
	xme_core_component_t requestingComponent;
	xme_core_component_port_t requestingPort;
	xme_core_component_port_t responseHandlingPort;
	xme_core_topic_t requestTopic;
	xme_core_topic_t responseTopic;
	xme_core_md_topicMetaDataHandle_t requestStaticMetaData;
}
xme_core_resourceManager_requestSenderItem_t;

typedef struct
{
	xme_core_component_t requestHandlingComponent;
	xme_core_component_port_t requestHandlingPort;
	xme_core_component_port_t responseSendingPort;
	xme_core_topic_t requestTopic;
	xme_core_md_topicMetaDataHandle_t requestMetaDataFilter;
	xme_core_topic_t responseTopic; // may be optional if it is asserted that responseTopic = reqestTopic+1
	xme_core_md_topicMetaDataHandle_t responseStaticMetaData;
}
xme_core_resourceManager_requestHandlerItem_t;

typedef struct
{
	xme_core_dataChannel_t responseDataChannel; ///< Data channel to send response to.
	xme_core_rr_requestHandle_t localRequestHandle; ///< Request handle (only valid at the request issuer side).
	xme_core_rr_requestInstanceHandle_t localRequestInstance; ///< Request instance identifier (only valid at the request issuer side).
}
xme_core_resourceManager_responseInstanceItem_t;

/**
 * \typedef xme_core_resourceManager_configStruct_t
 *
 * \brief  Service database configuration structure.
 */
XME_COMPONENT_CONFIG_STRUCT
(
	xme_core_resourceManager,
	// private
	XME_HAL_TABLE(xme_core_resourceManager_componentInfo_t, components, XME_HAL_DEFINES_MAX_RESOURCEMANAGER_COMPONENT_ITEMS);
	XME_HAL_TABLE(xme_core_resourceManager_publicationItem_t, localPublications, XME_HAL_DEFINES_MAX_RESOURCEMANAGER_LOCAL_PUBLICATION_ITEMS);
	XME_HAL_TABLE(xme_core_resourceManager_subscriptionItem_t, localSubscriptions, XME_HAL_DEFINES_MAX_RESOURCEMANAGER_LOCAL_SUBSCRIPTION_ITEMS);
	XME_HAL_TABLE(xme_core_resourceManager_requestSenderItem_t, localRequestSenders, XME_HAL_DEFINES_MAX_RESOURCEMANAGER_LOCAL_REQUEST_SENDER_ITEMS);
	XME_HAL_TABLE(xme_core_resourceManager_requestHandlerItem_t, localRequestHandlers, XME_HAL_DEFINES_MAX_RESOURCEMANAGER_LOCAL_REQUEST_HANDLER_ITEMS);
	XME_HAL_TABLE(xme_core_resourceManager_responseInstanceItem_t, localResponseInstances, XME_HAL_DEFINES_MAX_RESOURCEMANAGER_LOCAL_RESPONSE_INSTANCE_ITEMS);

	xme_hal_tls_handle_t currentComponentIdTlsHandle;

	xme_core_component_port_t localAnnouncementPort;
	xme_core_dataChannel_t localAnnouncementDataChannel;
);

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
extern xme_core_resourceManager_configStruct_t xme_core_resourceManager_config;
extern xme_core_component_t xme_core_resourceManager_componentCount;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Initializes the resource manager component.
 *         Exactly one component of this type must be present on every node.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_SUCCESS if the resource manager component has been properly
 *            initialized.
 *          - XME_CORE_STATUS_OUT_OF_RESOURCES if initialization failed.
 */
xme_core_status_t
xme_core_resourceManager_init();

/**
 * \brief  Creates a resource manager component.
 *         Exactly one component of this type must be present on every node.
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            initialized.
 *          - XME_CORE_STATUS_INVALID_CONFIGURATION if a component of this
 *            type has already been initialized. Exactly one component of this
 *            type must be present on every node.
 */
xme_core_status_t
xme_core_resourceManager_create(xme_core_resourceManager_configStruct_t* config);

/**
 * \brief  Activates a resource manager component.
 */
xme_core_status_t
xme_core_resourceManager_activate(xme_core_resourceManager_configStruct_t* config);

/**
 * \brief  Deactivates a resource manager component.
 */
void
xme_core_resourceManager_deactivate(xme_core_resourceManager_configStruct_t* config);

/**
 * \brief  Destroys a resource manager component.
 *         Exactly one component of this type must be present on every node.
 */
void
xme_core_resourceManager_destroy(xme_core_resourceManager_configStruct_t* config);

/**
 * \brief  Frees all resources occupied by the resource manager component.
 *         Exactly one component of this type must be present on every node.
 */
void
xme_core_resourceManager_fini();

xme_core_status_t
xme_core_resourceManager_activateComponents();

xme_core_status_t
xme_core_resourceManager_deactivateComponents();





void
xme_core_resourceManager_setCurrentComponentId
(
	xme_core_component_t componentId
);

xme_core_component_t
xme_core_resourceManager_getCurrentComponentId();

/**
 * \brief  Get name of component with given id.
 *
 * \param  componentId Locally valid component id.
 *
 * \return String with name of component or NULL when no component with
 *         given id exists.
 */
const char*
xme_core_resourceManager_getComponentName
(
	xme_core_component_t componentId
);





/**
 * \brief  Tries to schedule the task according to the given parameters.
 *         If a new task with the given parameters is not feasible, the
 *         function will fail.
 *
 * \param  startMs If non-zero, specifies the time in milliseconds from now
 *         when the task should be executed. A value of zero will cause the
 *         task to be started as soon as possible. A value of
 *         XME_HAL_SCHED_TASK_INITIALLY_SUSPENDED will cause the task to stay
 *         suspended until it is resumed using
 *         xme_hal_sched_setTaskExecutionState().
 * \param  periodMs If non-zero, specifies the period in milliseconds of the
 *         task. Otherwise, the task is a one-shot task and will be
 *         automatically removed after execution.
 * \param  priority Task priority (only meaningful for priority-based
 *         scheduler implementations). Higher values mean a higher priority.
 * \param  callback Callback function implementing the task to be executed.
 *         Note that on some platforms, this callback function might be run
 *         from a different execution context (e.g., a different thread),
 *         so synchronization mechanisms might be required if the task
 *         shares data with other tasks or the main program.
 * \param  userData User-defined data to be passed to the callback function.
 *
 * \return Returns a task handle on success. If the a task according to the
 *         given parameters is not feasible, the function fails and returns
 *         XME_CORE_RESOURCEMANAGER_TASK_INFEASIBLE.
 */
xme_core_resourceManager_taskHandle_t
xme_core_resourceManager_scheduleTask
(
	xme_hal_time_interval_t startMs,
	xme_hal_time_interval_t periodMs,
	uint8_t priority,
	xme_hal_sched_taskCallback_t callback,
	void* userData
);

/**
 * \brief  Removes the given task from the schedule. However, if the task is
 *         currently running, it will not be stopped. In case of a one-shot
 *         task, this function can be used to prevent starting of the task
 *         before it has been started. In case of a periodic task, this
 *         function will prevent subsequent invocations of the task.
 *
 * \param  taskHandle Handle of the task to remove.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the task has been successfully
 *            removed.
 *          - XME_CORE_STATUS_INVALID_PARAMETER if the given task handle
 *            is invalid.
 */
xme_core_status_t
xme_core_resourceManager_killTask(xme_core_resourceManager_taskHandle_t taskHandle);

// Returns whether there are currently no tasks to process
bool
xme_core_resourceManager_idle();

// Runs the next task from the ready set.
void
xme_core_resourceManager_runTask();


/**
 * \brief  Provided information to the resource manager to manage data transmission 
 *         from broker to the destination component port. Data delivery is 
 *         moved to the resource manager to encapsulate knowledge of potential context
 *         switch, scheduling...
 *
 * \param  sourceDataChannel Data channel from which the data originated.
 * \param  toComponent Component to which data needs to be delivered.
 * \param  toPort Port of component to which data needs to be delivered.
 * \param  dataHandle Data handle associated with data to be delivered
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the task has been successfully
 *            removed.
 *          - TODO: some errorcode if not. See ticket #721
 */

xme_core_status_t
xme_core_resourceManager_notifyComponent
(
	xme_core_dataChannel_t sourceDataChannel,
	xme_core_component_t toComponent,
	xme_core_component_port_t toPort,
	xme_hal_sharedPtr_t dataHandle
);

xme_core_component_port_t
xme_core_resourceManager_addPublicationPort
(
	xme_core_component_t component
);

xme_core_component_port_t
xme_core_resourceManager_addSubscriptionPort
(
	xme_core_component_t component,
	xme_core_dcc_receiveTopicCallback_t receiveTopicCallback,
	void* receiveTopicUserData
);

xme_core_component_port_t
xme_core_resourceManager_addRequestSenderPort
(
	xme_core_component_t component
);

xme_core_component_port_t
xme_core_resourceManager_addResponseHandlerPort
(
	xme_core_component_t component,
	xme_core_rr_receiveResponseCallback_t receiveResponseCallback,
	void* receiveResponseUserData
);

xme_core_component_port_t
xme_core_resourceManager_addRequestHandlerPort
(
	xme_core_component_t component,
	xme_core_rr_receiveRequestCallback_t receiveRequestCallback,
	void* receiveRequestUserData
);

xme_core_component_port_t
xme_core_resourceManager_addResponseSenderPort
(
	xme_core_component_t component
);

xme_core_status_t
xme_core_resourceManager_removePort
(
	xme_core_component_t component,
	xme_core_component_port_t port
);

xme_core_component_portType_t
xme_core_resourceManager_getPortType
(
	xme_core_component_t component,
	xme_core_component_port_t port,
	bool isOutputPort
);

xme_core_component_port_t
xme_core_resourceManager_getPublicationPort
(
	xme_core_dcc_publicationHandle_t publicationHandle
);

xme_core_component_port_t
xme_core_resourceManager_getSubscriptionPort
(
	xme_core_dcc_subscriptionHandle_t subscriptionHandle
);

xme_core_component_port_t
xme_core_resourceManager_getRequestSenderPort
(
	xme_core_rr_requestHandle_t requestHandle
);

xme_core_component_port_t
xme_core_resourceManager_getRequestHandlerPort
(
	xme_core_rr_requestHandlerHandle_t requestHandlerHandle
);
#endif // #ifndef XME_CORE_RESOURCEMANAGER_H

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
 *         Health monitor component.
 *
 * \author
 *         Dominik Sojer <sojer@fortiss.org>
 */

#ifndef HEALTHMONITOR_H
#define HEALTHMONITOR_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/

#include "xme/core/core.h"
#include "xme/core/defines.h"
#include "xme/core/broker.h"
#include "xme/hal/time.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
typedef struct
{
	// public
	xme_core_node_nodeId_t nodeId;
	xme_core_component_t componentId; // set to -1 to monitor complete node
	xme_core_dcc_subscriptionHandle_t subHandleErrors; // error messages
	xme_core_dcc_publicationHandle_t pubHandleErrors; // error messages
}
xme_adv_healthmonitor_configStruct_t;

#define HEARTBEAT_TOPIC_ID (xme_core_topic_t)777
#define ERROR_MESSAGE_TOPIC_ID (xme_core_topic_t)778
#define HEARTBEAT_CHANNEL (xme_core_dataChannel_t)742
#define ERROR_MESSAGE_CHANNEL (xme_core_dataChannel_t)743

#define XME_ADV_HEALTHMONITOR_ERRORS_MAX 5

typedef enum
{
	XME_ADV_HEALTHMONITOR_COMPONENT_OK = 0,
	XME_ADV_HEALTHMONITOR_COMPONENT_EXCEPTION,
	XME_ADV_HEALTHMONITOR_COMPONENT_UNKNOWN,
	XME_ADV_HEALTHMONITOR_TEST_OK, 
	XME_ADV_HEALTHMONITOR_TEST_UNKNOWN,
	XME_ADV_HEALTHMONITOR_TEST_FAILED, 
	XME_ADV_HEALTHMONITOR_NODE_OK, 
	XME_ADV_HEALTHMONITOR_NODE_UNKNOWN,
	XME_ADV_HEALTHMONITOR_NODE_EXCEPTION, 
	XME_ADV_HEALTHMONITOR_STATUS_UNKNOWN,
}
xme_adv_healthmonitor_status_t;

typedef enum
{
	XME_ADV_TEST_HEARTBEAT = 0,
	XME_ADV_TEST_CPU,
	XME_ADV_TEST_CONSISTENCY,
	XME_ADV_TEST_MEMORY,
	XME_ADV_TEST_NOTEST,
}
xme_adv_test_type_t;

typedef struct
{
	xme_core_component_t componentId;
	xme_core_device_guid_t nodeId; 
	xme_adv_test_type_t identifier; 
	xme_adv_healthmonitor_status_t status;
} xme_adv_healthmonitor_errorMessage_t;

typedef struct
{
	xme_core_device_guid_t nodeId; // id of the heartbeat's sender
	xme_hal_time_handle_t maxTimestampAge;
}
xme_adv_healthmonitor_heartbeat_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

/**
 * \brief  Creates a health monitor component.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            initialized.
 *          - XME_CORE_STATUS_INTERNAL_ERROR on error.
 */
xme_core_status_t 
xme_adv_healthmonitor_create(xme_adv_healthmonitor_configStruct_t* config);

/**
 * \brief  Activates a health monitor component.
 */
xme_core_status_t 
	xme_adv_healthmonitor_activate(xme_adv_healthmonitor_configStruct_t* config);

/**
 * \brief  Deactivates a health monitor component.
 */
void
xme_adv_healthmonitor_deactivate(xme_adv_healthmonitor_configStruct_t* config);

/**
 * \brief  Destroys a health monitor component.
 */
void
xme_adv_healthmonitor_destroy(xme_adv_healthmonitor_configStruct_t* config);

/**
 * \brief  Callback function that is executed when an error message is received
 *
 */
void
receive_errorMessage(xme_hal_sharedPtr_t dataHandle, void* userData);

/**
 * \brief  Callback function that performs consistency checks on the locally
 *         collected data, sends their results and sends meta-heartbeats.
 */
void 
healthmonitor_callback (void* userData);

/**
 * \brief  prints the textual expression of a status code
 */
char*
printHealthmonitorErrNo(xme_adv_healthmonitor_status_t error);

/**
 * \brief  create error message publication, subscription and routes
 */
xme_core_status_t
createHealthmonitorErrorMessages(xme_adv_healthmonitor_configStruct_t* config);

/**
 * \brief checks the health of the monitored entity, based on received error messages
 */
xme_core_status_t
checkHealth();

#endif
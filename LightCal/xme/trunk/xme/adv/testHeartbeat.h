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

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/

#include "healthmonitor.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/

typedef struct
{
	// public
	xme_hal_time_handle_t maxTimestampAge;
	bool catchAllHeartBeats;
	xme_core_status_t (*callback)(); // instant error reaction function
	xme_core_dcc_subscriptionHandle_t subHandle; // heartbeats
	xme_core_dcc_publicationHandle_t pubHandle; // error messages
}
xme_adv_testHeartbeat_configStruct_t;

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/

#define XME_ADV_TESTHEARTBEAT_HEARTBEATS_MAX 50

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

/**
 * \brief  Creates a heartbeat component.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            initialized.
 *          - XME_CORE_STATUS_INTERNAL_ERROR on error.
 */
xme_core_status_t
xme_adv_testHeartbeat_create(xme_adv_testHeartbeat_configStruct_t* config);


/**
 * \brief  Activates a heartbeat component.
 */
xme_core_status_t
xme_adv_testHeartbeat_activate(xme_adv_testHeartbeat_configStruct_t* config);


/**
 * \brief  Deactivates a heartbeat component.
 */
void
xme_adv_testHeartbeat_deactivate(xme_adv_testHeartbeat_configStruct_t* config);


/**
 * \brief  Destroys a heartbeat component.
 */
void
xme_adv_testHeartbeat_destroy(xme_adv_testHeartbeat_configStruct_t* config);

/**
 * \brief  Callback function that sends the sends the local node id
 */
void
testHeartbeat_callback (void* userData);

/**
 * \brief  Callback function that saves a received heartbeat
 */
void
receive_heartbeatSingleNode(xme_hal_sharedPtr_t dataHandle, void* userData);

/**
 * \brief  Callback function that saves a received heartbeat
 */
void
receive_heartbeatMultiNode(xme_hal_sharedPtr_t dataHandle, void* userData);


/**
 * \brief  Function that sets the node ids, which should be monitored. Project specific!
 */
void 
initHeartbeatTable ();

/**
 * \brief Debug function, printing the timestamp table
 */
void debugHeartbeatTable ();
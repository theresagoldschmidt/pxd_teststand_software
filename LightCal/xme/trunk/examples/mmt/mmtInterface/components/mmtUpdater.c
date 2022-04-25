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
 *         Multifunk Modeling Tool (MMT) updater component.
 *         Listens for changes in the XME network and sends updates to the MMT.
 *
 * \author
 *         Benjamin Wiesmueller <wiesmueller@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdbool.h>

#include "xme/core/core.h"
#include "xme/core/broker.h"
#include "xme/core/defines.h"
#include "xme/core/nodeManager.h"

#include "mmtUpdater.h"

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

/**
 * \brief  Callback function for receiving NewNode events.
 *
 * \see    xme_core_dcc_receiveTopicCallback_t
 */
void
xme_adv_mmtUpdater_receiveNewNodeTopicCallback
(
	xme_hal_sharedPtr_t dataHandle, 
	void* userData
);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/

xme_core_status_t 
xme_adv_mmtUpdater_create(xme_adv_mmtUpdater_configStruct_t* config)
{
	// Initialize the private part of the configuration structure
	config->newNodeSubscriptionHandlerHandle = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;

	// Subscribe to login requests (we want to pass information about new nodes to the Multifunk Tool)
	XME_CHECK
	(
		XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE !=
		(
			config->newNodeSubscriptionHandlerHandle = xme_core_dcc_subscribeTopic
			(
				XME_CORE_TOPIC_NEW_NODE,
				XME_CORE_MD_EMPTY_META_DATA,
				&xme_adv_mmtUpdater_receiveNewNodeTopicCallback,
				false,
				config
			)
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t 
xme_adv_mmtUpdater_activate(xme_adv_mmtUpdater_configStruct_t* config)
{
	return XME_CORE_STATUS_SUCCESS;
}

void 
xme_adv_mmtUpdater_deactivate(xme_adv_mmtUpdater_configStruct_t* config)
{
}

void 
xme_adv_mmtUpdater_destroy(xme_adv_mmtUpdater_configStruct_t* config)
{
	xme_core_dcc_unsubscribeTopic(config->newNodeSubscriptionHandlerHandle);
	config->newNodeSubscriptionHandlerHandle = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;
}

void
xme_adv_mmtUpdater_receiveNewNodeTopicCallback(xme_hal_sharedPtr_t dataHandle, void* userData) 
{
	xme_adv_mmtUpdater_configStruct_t* config;
	xme_core_topic_loginRequestData_t* requestData;
	mmt_upd_t update;
	
	config = (xme_adv_mmtUpdater_configStruct_t*)userData;

	if (xme_hal_sharedPtr_getSize(dataHandle) != sizeof(xme_core_topic_loginRequestData_t))
	{
		// Unexpected data type
		return;
	}
	requestData = (xme_core_topic_loginRequestData_t*)xme_hal_sharedPtr_getPointer(dataHandle);

	config->mmt_println("XME: Received login request. Sending NEW_NODE update.");
	update.type = NEW_NODE;
	update.len = sizeof(xme_core_topic_loginRequestData_t);
	update.data = (char*)requestData;

	config->mmt_updCallback(update);

	xme_hal_sharedPtr_destroy(dataHandle);
}

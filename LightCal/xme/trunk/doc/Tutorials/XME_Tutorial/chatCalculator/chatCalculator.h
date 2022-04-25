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
 *         Chat Calculator component.
 *
 * \author
 *         fortiss <chromosome@fortiss.org>
 */

#ifndef XME_ADV_CHAT_CALCULATOR_H
#define XME_ADV_CHAT_CALCULATOR_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/component.h"
#include "xme/core/dcc.h"
#include "xme/core/resourceManager.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
typedef struct
{
	// public
	xme_core_topic_t topic;
	// private
	xme_core_resourceManager_taskHandle_t taskHandle; ///< Task handle.
	xme_core_dcc_publicationHandle_t publicationHandle; ///< Publication handle.
	xme_core_dcc_subscriptionHandle_t subscriptionHandle; ///< Subscriptions handle.
}
xme_adv_chatCalculator_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates a Chat Calculator component.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            initialized.
 *          - XME_CORE_STATUS_INTERNAL_ERROR on error.
 */
xme_core_status_t
xme_adv_chatCalculator_create(xme_adv_chatCalculator_configStruct_t* config);

/**
 * \brief  Activates a Chat Calculator component.
 */
xme_core_status_t
xme_adv_chatCalculator_activate(xme_adv_chatCalculator_configStruct_t* config);

/**
 * \brief  Deactivates a Chat Calculator component.
 */
void
xme_adv_chatCalculator_deactivate(xme_adv_chatCalculator_configStruct_t* config);

/**
 * \brief  Destroys a Chat Calculator component.
 */
void
xme_adv_chatCalculator_destroy(xme_adv_chatCalculator_configStruct_t* config);

#endif // #ifndef XME_ADV_CHAT_CALCULATOR_H

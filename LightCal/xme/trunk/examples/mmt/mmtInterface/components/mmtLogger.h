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
 *         Multifunk Modeling Tool (MMT) logger component.
 *         Listens for log messages from the XME net and passes them to the 
 *         MMT.
 *
 * \author
 *         Benjamin Wiesmueller <wiesmueller@fortiss.org>
 */

#ifndef XME_ADV_MMTLOGGER_H
#define XME_ADV_MMTLOGGER_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/component.h"
#include "xme/core/dcc.h"
#include "xme/core/device.h"
#include "xme/core/log.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef  mmt_log_t
 *
 * \brief    Log message.
 */
typedef struct 
{
	xme_core_device_guid_t deviceGuid; ///< Device guid of node which sent this message.
	xme_log_severity_t severity;       ///< Severity of log message.
	xme_core_component_t componentId;  ///< Id of component which sent the message (locally valid on its node).
	const char* compName; ///< Pointer to C string, containing the name of the component which sent this message.
	const char* msg;      ///< Pointer to C string, containing the log message.
} 
mmt_log_t;

/**
 * \typedef xme_adv_mmtLogger_configStruct_t
 *
 * \brief   Multifunk logger configuration structure.
 */
typedef struct
{
	// public
	bool (*mmt_logCallback) (mmt_log_t); ///< Callback function for sending log messages to the MMT.
	void (*mmt_println) (const char*);	 ///< Callback function for printing to the console of the MMT.
	// private
	xme_core_dcc_subscriptionHandle_t logMsgSubscriptionHandlerHandle;
}
xme_adv_mmtLogger_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

/**
 * \brief  Creates a multifunk logger component.
 */
xme_core_status_t
xme_adv_mmtLogger_create(xme_adv_mmtLogger_configStruct_t* config);


/**
 * \brief  Activates a multifunk logger component.
 */
xme_core_status_t
xme_adv_mmtLogger_activate(xme_adv_mmtLogger_configStruct_t* config);


/**
 * \brief  Deactivates a multifunk logger component.
 */
void
xme_adv_mmtLogger_deactivate(xme_adv_mmtLogger_configStruct_t* config);


/**
 * \brief  Destroys a multifunk logger component.
 */
void
xme_adv_mmtLogger_destroy(xme_adv_mmtLogger_configStruct_t* config);

#endif // #ifndef XME_ADV_MMTLOGGER_H
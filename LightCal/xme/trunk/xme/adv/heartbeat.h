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
 *         Heartbeat sender component.
 *
 * \author
 *         Dominik Sojer <sojer@fortiss.org>
 */
 
#ifndef HEARTBEAT_H
#define HEARTBEAT_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/

#include "healthmonitor.h"

#include "xme/hal/sched.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/

typedef struct
{
	// public
	xme_hal_time_handle_t interval; ///< Frequency of heartbeating.
	// private
	xme_core_dcc_publicationHandle_t pubHandle;
	xme_hal_sched_taskHandle_t taskHandle;
}
xme_adv_heartbeat_configStruct_t;

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
xme_adv_heartbeat_create(xme_adv_heartbeat_configStruct_t* config);


/**
 * \brief  Activates a heartbeat component.
 */
xme_core_status_t
xme_adv_heartbeat_activate(xme_adv_heartbeat_configStruct_t* config);


/**
 * \brief  Deactivates a heartbeat component.
 */
void
xme_adv_heartbeat_deactivate(xme_adv_heartbeat_configStruct_t* config);


/**
 * \brief  Destroys a heartbeat component.
 */
void
xme_adv_heartbeat_destroy(xme_adv_heartbeat_configStruct_t* config);

/**
 * \brief  Callback function that sends the sends the local node id
 */
void
heartbeatComponent_callback (void* userData);

#endif
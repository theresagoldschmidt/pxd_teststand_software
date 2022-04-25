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

#ifndef XME_ADV_MMTUPDATER_H
#define XME_ADV_MMTUPDATER_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/

/**
 * \typedef  mmt_upd_type_t
 *
 * \brief    Enumerates all possible types of model update events.
 */
typedef enum { 
	NEW_NODE=0,
	NEW_COMP=1,
	DO_NOT_USE=75000 // 'force' 32 bit integer
}
mmt_upd_type_t;

/**
 * \typedef  mmt_upd_t
 *
 * \brief    Encapsulates information about model update.
 */
typedef struct 
{
	mmt_upd_type_t type; // Type of update event
	int len;            // Length of data stored at data pointer
	char *data;         // Pointer to actual data of update (e.g. a loginRequest)
} 
mmt_upd_t;

/**
 * \typedef xme_adv_mmtUpdater_configStruct_t
 *
 * \brief   Multifunk Updater configuration structure.
 */
typedef struct
{
	// public
	bool (*mmt_updCallback) (mmt_upd_t); // Callback function for model update events
	void (*mmt_println) (char*);	       // Callback function for printing to std out
	// private
	xme_core_dcc_subscriptionHandle_t newNodeSubscriptionHandlerHandle;
}
xme_adv_mmtUpdater_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

/**
 * \brief  Creates a multifunk updater component.
 */
xme_core_status_t
xme_adv_mmtUpdater_create(xme_adv_mmtUpdater_configStruct_t* config);


/**
 * \brief  Activates a multifunk updater component.
 */
xme_core_status_t
xme_adv_mmtUpdater_activate(xme_adv_mmtUpdater_configStruct_t* config);


/**
 * \brief  Deactivates a multifunk updater component.
 */
void
xme_adv_mmtUpdater_deactivate(xme_adv_mmtUpdater_configStruct_t* config);


/**
 * \brief  Destroys a multifunk updater component.
 */
void
xme_adv_mmtUpdater_destroy(xme_adv_mmtUpdater_configStruct_t* config);

#endif // #ifndef XME_ADV_MMTUPDATER_H

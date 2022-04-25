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
 *         Dummy implementation of mmtUpdater.
 *
 * \author
 *         Benjamin Wiesmueller <wiesmueller@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/sched.h"

#include "mmtUpdater.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/

/**
 * \typedef xme_adv_loginServer_configStruct_t
 *
 * \brief  Login server configuration structure.
 */
typedef struct
{
	// public
	bool (*mmt_updCallback) (mmt_upd_t);
	void (*mmt_println) (char*);	
	// private
	xme_hal_sched_taskHandle_t taskCallbackTaskHandle;
}
xme_adv_mmtUpdaterDummy_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

/**
 * \brief  Creates a multifunk updater component.
 */
xme_core_status_t
xme_adv_mmtUpdaterDummy_create(xme_adv_mmtUpdaterDummy_configStruct_t* config);


/**
 * \brief  Activates a multifunk updater component.
 */
xme_core_status_t
xme_adv_mmtUpdaterDummy_activate(xme_adv_mmtUpdaterDummy_configStruct_t* config);


/**
 * \brief  Deactivates a multifunk updater component.
 */
void
xme_adv_mmtUpdaterDummy_deactivate(xme_adv_mmtUpdaterDummy_configStruct_t* config);


/**
 * \brief  Destroys a multifunk updater component.
 */
void
xme_adv_mmtUpdaterDummy_destroy(xme_adv_mmtUpdaterDummy_configStruct_t* config);

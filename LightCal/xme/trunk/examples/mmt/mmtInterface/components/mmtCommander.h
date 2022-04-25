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
 *         Multifunk Modeling Tool (MMT) commander component.
 *         Receives commands from the MMT.
 *
 * \author
 *         Benjamin Wiesmueller <wiesmueller@fortiss.org>
 */

#ifndef XME_ADV_MMTCOMMANDER_H
#define XME_ADV_MMTCOMMANDER_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef  mmt_cmd_type_t
 *
 * \brief    Enumerates all possible types of commands.
 */
typedef enum { 
	NO_CMD=0,
	SHUTDOWN_MODELING_INTERFACE=1,
	UPDATE_ALL=2,
	MAX = 75000
}
mmt_cmd_type_t;

/**
 * \typedef  mmt_cmd_t
 *
 * \brief    Encapsulates information about a command.
 */
typedef struct 
{
	mmt_cmd_type_t type; // Type of command event
	int len;            // Length of additional data of command, stored at data pointer
	char *data;         // Pointer to actual data of update (e.g. a loginRequest)
} 
mmt_cmd_t;

/**
 * \typedef  xme_adv_mmtCommander_configStruct_t
 *
 * \brief    Multifunk Commander configuration structure.
 */
typedef struct
{
	// public
	mmt_cmd_t *(*mmt_cmdCallback) (void); // Callback function to Multifunk Tool for receiving commdands
	void (*mmt_println) (char*);	        // Callback function for printing to std out
	// private
	xme_hal_sched_taskHandle_t taskCallbackTaskHandle;
}
xme_adv_mmtCommander_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

/**
 * \brief  Creates a multifunk updater component.
 */
xme_core_status_t
xme_adv_mmtCommander_create(xme_adv_mmtCommander_configStruct_t* config);


/**
 * \brief  Activates a multifunk updater component.
 */
xme_core_status_t
xme_adv_mmtCommander_activate(xme_adv_mmtCommander_configStruct_t* config);


/**
 * \brief  Deactivates a multifunk updater component.
 */
void
xme_adv_mmtCommander_deactivate(xme_adv_mmtCommander_configStruct_t* config);


/**
 * \brief  Destroys a multifunk updater component.
 */
void
xme_adv_mmtCommander_destroy(xme_adv_mmtCommander_configStruct_t* config);

#endif // #ifndef XME_ADV_MMTCOMMANDER_H
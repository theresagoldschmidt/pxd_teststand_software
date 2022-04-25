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
 *         CPU test component.
 *
 * \author
 *         Dominik Sojer <sojer@foritss.org>
 */
 
#ifndef TESTCONSISTENCY_H
#define TESTCONSISTENCY_H

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
	xme_hal_time_handle_t startMs;
	xme_hal_time_handle_t periodMs;
}
xme_adv_testConsistency_configStruct_t;

typedef bool (*checkFunctionPointer_t)();

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/

#define XME_ADV_TESTCONSISTENCY_FUNCTIONS_MAX 10


/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

/**
 * \brief  Creates a consistency test component.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            initialized.
 *          - XME_CORE_STATUS_INTERNAL_ERROR on error.
 */
xme_core_status_t
xme_adv_testConsistency_create(xme_adv_testConsistency_configStruct_t* config);


/**
 * \brief  Activates a consistency component.
 */
xme_core_status_t
xme_adv_testConsistency_activate(xme_adv_testConsistency_configStruct_t* config);


/**
 * \brief  Deactivates a consistency component.
 */
void
xme_adv_testConsistency_deactivate(xme_adv_testConsistency_configStruct_t* config);


/**
 * \brief  Destroys a consistency component.
 */
void
xme_adv_testConsistency_destroy(xme_adv_testConsistency_configStruct_t* config);

/**
 * \brief  Callback function that executes consistency checks
 */
void
testConsistency_callback (void* userData);

/**
 * \brief  Function that adds the consistency checks, which should be executed. Project specific!
 */
void initFunctionsTable();

/**
 * \brief  example function 1
 */
bool func1();

/**
 * \brief  example function 2
 */
bool func2();

#endif
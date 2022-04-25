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
 *         memory test component.
 *
 * \author
 *         Dominik Sojer <sojer@fortiss.org>
 */

#ifndef TESTSTUB_H
#define TESTSTUB_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/

#include "healthmonitor.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/

typedef enum
{
	XME_ADV_TEST_MEMORY_ALGORITHM_WALKPATH = 0,
	XME_ADV_TEST_MEMORY_ALGORITHM_CHECKERBOARD,
	XME_ADV_TEST_MEMORY_ALGORITHM_GALPAT,
	XME_ADV_TEST_MEMORY_ALGORITHM_TRANSPARENT_GALPAT, 
	XME_ADV_TEST_MEMORY_ALGORITHM_INVERT, 
}
xme_adv_testmemory_algorithm_t;

typedef struct
{
	// public
	xme_hal_time_handle_t interval_ms; // activation interval
	int startAddress; // memory address where test should start
	int stopAddress; // memory address where test should stop
	xme_adv_testmemory_algorithm_t algorithm; // test algorithm
	xme_core_status_t (*callback)(); // instant error reaction function
	xme_core_dcc_publicationHandle_t pubHandle;
}
xme_adv_testMemory_configStruct_t;

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

/**
 * \brief  Creates a memory test component.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            initialized.
 *          - XME_CORE_STATUS_INTERNAL_ERROR on error.
 */
xme_core_status_t
xme_adv_testMemory_create(xme_adv_testMemory_configStruct_t* config);


/**
 * \brief  Activates a memory test component.
 */
xme_core_status_t
xme_adv_testMemory_activate(xme_adv_testMemory_configStruct_t* config);


/**
 * \brief  Deactivates a memory test component.
 */
void
xme_adv_testMemory_deactivate(xme_adv_testMemory_configStruct_t* config);


/**
 * \brief  Destroys a memory test component.
 */
void
xme_adv_testMemory_destroy(xme_adv_testMemory_configStruct_t* config);

/**
 * \brief  actual test function
 */
void
xme_adv_testMemory_test(void* userData);

/**
 * \brief  walkpath test function
 */
xme_core_status_t
xme_adv_testMemory_walkpath(int start, int stop);

/**
 * \brief  invert test function
 */
xme_core_status_t
xme_adv_testMemory_invert(int start, int stop);

#endif
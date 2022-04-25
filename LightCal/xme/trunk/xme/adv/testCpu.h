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
 *         Dominik Sojer <sojer@fortiss.org>
 */
 
#ifndef TESTCPU_H
#define TESTCPU_H


/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/

#include "healthmonitor.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/

typedef enum
{
	XME_ADV_TEST_MEMORY_ALGORITHM_LIMITED_NR_PATTERNS = 0,
	XME_ADV_TEST_CPU_ALGORITHM_WALKING_BIT,
}
xme_adv_testcpu_algorithm_t;

typedef struct
{
	// public
	xme_hal_time_handle_t interval_ms; // activation interval
	xme_adv_testcpu_algorithm_t algorithm; // test algorithm
	xme_core_status_t (*callback)(); // instant error reaction function
	int runlength; // width of data path
	xme_core_dcc_publicationHandle_t pubHandle;
}
xme_adv_testCpu_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

/**
 * \brief  Creates a cpu test component.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            initialized.
 *          - XME_CORE_STATUS_INTERNAL_ERROR on error.
 */
xme_core_status_t
xme_adv_testCpu_create(xme_adv_testCpu_configStruct_t* config);


/**
 * \brief  Activates a cpu test component.
 */
xme_core_status_t
xme_adv_testCpu_activate(xme_adv_testCpu_configStruct_t* config);


/**
 * \brief  Deactivates a cpu test component.
 */
void
xme_adv_testCpu_deactivate(xme_adv_testCpu_configStruct_t* config);


/**
 * \brief  Destroys a cpu test component.
 */
void
xme_adv_testCpu_destroy(xme_adv_testCpu_configStruct_t* config);

/**
 * \brief  test function
 */
void
testCpu_callback (void* userData);

/**
 * \brief  limited number of pattern test function
 */
xme_core_status_t
xme_adv_testCpu_limitedNrPatterns();


/**
 * \brief  walking bit test function
 */
xme_core_status_t
xme_adv_testCpu_walkingBit(unsigned int runlength);

#endif
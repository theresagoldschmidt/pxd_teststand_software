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
 *         This is the stub of a test component, based on the SAFE design. It
 *         can be used to create new test components very easily by making a
 *         copy of it and changing the sections, marked with 'TODO'. In addition,
 *         it might be necessary to extend xme_adv_testStub_configStruct_t, if
 *         additional information is required to execute the test. Please have
 *         a look at the heartbeatComponent and the heartbeatTestComponent for
 *         an example how tests with external evidence generators can be imple-
 *         mented.
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

typedef struct
{
	// public
	xme_hal_time_handle_t interval_ms; // activation interval
	xme_core_status_t (*callback)(); // instant error reaction function
	xme_core_dcc_publicationHandle_t pubHandle;
}
xme_adv_testStub_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

/**
 * \brief  Creates a stub test component.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            initialized.
 *          - XME_CORE_STATUS_INTERNAL_ERROR on error.
 */
xme_core_status_t
xme_adv_testStub_create(xme_adv_testStub_configStruct_t* config);


/**
 * \brief  Activates a stub test component.
 */
xme_core_status_t
xme_adv_testStub_activate(xme_adv_testStub_configStruct_t* config);


/**
 * \brief  Deactivates a stub test component.
 */
void
xme_adv_testStub_deactivate(xme_adv_testStub_configStruct_t* config);


/**
 * \brief  Destroys a stub test component.
 */
void
xme_adv_testStub_destroy(xme_adv_testStub_configStruct_t* config);

/**
 * \brief  test function
 */
void
test_callback (void* userData);


#endif
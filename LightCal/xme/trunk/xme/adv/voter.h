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
 *        voter component. (Proof of concept - voters very application specific
          and should be generated. The following aspects have to be taken into
		  account: voting algorithm, data type, timeout, trigger-event, input
		  data structure, mapping of input data to node ids, ...)
 *
 * \author
 *         Dominik Sojer <sojer@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/

#include "healthmonitor.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/

typedef enum
{
	XME_ADV_VOTER_ALGORITHM_MEDIAN = 0,
	XME_ADV_VOTER_ALGORITHM_MEAN,
}
xme_adv_voter_algorithm_t;

typedef enum
{
	XME_ADV_VOTER_DATATYPE_INT = 0,
	XME_ADV_VOTER_DATATYPE_BINARY,
}
xme_adv_voter_dataType_t;

typedef struct
{
	xme_adv_voter_dataType_t type;
	xme_core_node_nodeId_t sourceNode;
	int sizeOfBinaryData;
	void *data;
}
xme_adv_voter_votableData_t;

typedef struct
{
	xme_core_topic_t inputTopic;
	xme_core_topic_t outputTopic;
	xme_adv_voter_algorithm_t algorithm;
	xme_adv_voter_dataType_t dataType;
	xme_hal_time_handle_t startMs;
	xme_hal_time_handle_t periodMs;
	xme_core_dcc_subscriptionHandle_t subHandle; // collect data
	xme_core_dcc_publicationHandle_t pubHandle; // voted results
	xme_core_dcc_publicationHandle_t errorHandle; // error messages
}
xme_adv_voter_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

/**
 * \brief  Creates a voter component.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            initialized.
 *          - XME_CORE_STATUS_INTERNAL_ERROR on error.
 */
xme_core_status_t
xme_adv_voter_create(xme_adv_voter_configStruct_t* config);


/**
 * \brief  Activates a voter component.
 */
xme_core_status_t
xme_adv_voter_activate(xme_adv_voter_configStruct_t* config);


/**
 * \brief  Deactivates a voter component.
 */
void
xme_adv_voter_deactivate(xme_adv_voter_configStruct_t* config);


/**
 * \brief  Destroys a voter component.
 */
void
xme_adv_voter_destroy(xme_adv_voter_configStruct_t* config);

/**
 * \brief  calculates the median of the received values and publishes it.
 */
int
vote_medianInt(void* userData);

/**
 * \brief  calculates the mean of the received values.
 */
int
vote_meanInt(void* userData);

/**
 * \brief  votes the received values and publishes the result.
 */
void
vote(void* userData);

/**
 * \brief  stores received data.
 */
void
receive_value(xme_hal_sharedPtr_t dataHandle, void* userData);


void debugTableSort(); // TODO: delete
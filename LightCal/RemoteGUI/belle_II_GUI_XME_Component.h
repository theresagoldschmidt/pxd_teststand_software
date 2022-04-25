/*
 * Copyright (c) 2011, fortiss GmbH.
 * All rights reserved.
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
 *         Belle II GUI XME component file.
 *
 * \author
 *         Hauke Stähle <staehle@fortiss.org>
  *        Nadine Keddis <keddis@fortiss.org>
 */
#ifndef belle_II_GUI_XME_Component_H
#define belle_II_GUI_XME_Component_H
/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/component.h"
#include "xme/core/core.h"
#include "xme/core/defines.h"
#include "xme/core/nodeManager.h"
#include "xme/core/interfaceManager.h"
#include "xme/core/broker.h"
#include "xme/hal/time.h"
/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
#define OK 0
#define ERROR 1
#define NO_CONNECTION 2

#define NUMBER_OF_BELLE_NODES 200

// Structure that keeps the status info of each node
typedef struct
{
	int nodeNumber;										// Node number
	xme_hal_time_handle_t lastNodeStatusTimeStamp;		// Timestamp when the last Node status has been received 
	int nodeStatusCode;									// Status of the Node (OK, ERROR, NO CONNECTION)
}
status_info_nodes_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates Belle GUI  component.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            initialized.
 *          - XME_CORE_STATUS_INTERNAL_ERROR on error.
 */
xme_core_status_t
belle_II_GUI_XME_Component_create(xme_core_nodeManager_configStruct_t* config);

/**
 * \brief  Activates Belle GUI component.
 */
xme_core_status_t
belle_II_GUI_XME_Component_activate(xme_core_nodeManager_configStruct_t* config);

/**
 * \brief  Deactivates Belle GUI component.
 */
void
belle_II_GUI_XME_Component_deactivate(xme_core_nodeManager_configStruct_t* config);

/**
 * \brief  Destroys Belle GUI component.
 */
void
belle_II_GUI_XME_Component_destroy(xme_core_nodeManager_configStruct_t* config);

/**
 * \brief  Wrapper function that starts Belle II GUI.
 */
void callGui(int argc, char *argv[]);

/**
 * \brief  Wrapper function that calles a Chromosome specific function xme_core_dcc_sendTopicData for certain toppic.
 */
void sendData(int topic, void *data, uint16_t size);

/**
 * \brief  Wrapper function that finalizes ( xme_core_fini() - removes routs, unpublishes/unsubscribes toppics, delets tasks etc.) Chromosome.
 */
void shutdown();

/**
 * \brief  Wrapper function that initializes (xme_core_init()) Chromosome.
 */
void startup();

/**
 * \brief  Callback function that is called whenever the periodic task that checks the status of all nodes and updates the GUI accordingly is running.
 */

void node_status_callback (void* userData);

/**
 * \brief  Function that initializes the table that stores the latest status (OK,ERROR) of each node.
 */
void initStatusInfoNodesTable();

#endif //belle_II_GUI_XME_Component_H
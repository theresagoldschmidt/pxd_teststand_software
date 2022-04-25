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
 *         Topic dump component.
 *
 *         Subscribes to a (configurable) topic, creates a dump of the received 
 *         topic data and creates a log for the dump.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_PRIM_TOPICDUMP_H
#define XME_PRIM_TOPICDUMP_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/core.h"
#include "xme/core/dcc.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_prim_topicDump_configStruct
 *
 * \brief  Console logger configuration structure.
 */
typedef struct
{
	// public
	xme_core_topic_t subscribedTopic;
	// private
	xme_core_dcc_subscriptionHandle_t subscriptionHandle;
}
xme_prim_topicDump_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
xme_core_status_t
xme_prim_topicDump_create(xme_prim_topicDump_configStruct_t* config);

xme_core_status_t
xme_prim_topicDump_activate(xme_prim_topicDump_configStruct_t* config);

void
xme_prim_topicDump_deactivate(xme_prim_topicDump_configStruct_t* config);

void
xme_prim_topicDump_destroy(xme_prim_topicDump_configStruct_t* config);

#endif // #ifndef XME_PRIM_TOPICDUMP_H

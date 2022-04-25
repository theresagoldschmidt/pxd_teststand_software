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
 *         Chat example component.
 *
 * \author
 *         Hauke Stähle <staehle@fortiss.org>
 *         Dominik Sojer <sojer@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef CHATCOMPONENT_H
#define CHATCOMPONENT_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/component.h"
#include "xme/core/core.h"
#include "xme/core/defines.h"
#include "xme/core/nodeManager.h"
#include "xme/core/interfaceManager.h"
#include "xme/core/broker.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define NUM_CHAT_ROOMS 6 ///< Number of available chat rooms.
#define CHAT_TOPIC_BASE ((xme_core_topic_t)(XME_CORE_TOPIC_USER+100)) ///< Topic identifier for first chat room (susequent chat rooms use subsequent topic number).

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
/**
 * \brief  Names of the chat rooms.
 */
static const char* chatRoomNames[NUM_CHAT_ROOMS] =
{
	"Lobby",       ///< Lobby. The lobby will automatically be entered upon startup.
	"fortiss",     ///< Chat room for discussions about fortiss.
	"CHROMOSOME",  ///< Chat room for discussions about CHROMOSOME.
	"Weather",     ///< Chat room for discussions about the weather.
	"Sports",      ///< Chat room for discussions about sports.
	"Leisure"      ///< Chat room for discussions about leisure activities.
};

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
typedef struct
{
	char selectedChatRoom; ///< Currently selected chat room (-1 means no active chat room, 0 means first chat room, NUM_CHAT_ROOMS-1 means last chat room).
	xme_core_resourceManager_taskHandle_t taskHandle; ///< Task handle for user (chat message) input task.
	xme_core_dcc_subscriptionHandle_t subscriptionHandles[NUM_CHAT_ROOMS]; ///< Subscriptions for the respective chat rooms (invalid handle means that this chat room is not subscribed to).
	xme_core_dcc_publicationHandle_t publicationHandles[NUM_CHAT_ROOMS]; ///< Publications for the respective chat rooms (invalid handle means that this chat room is not published).
}
chatComponent_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates a chat component.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            initialized.
 *          - XME_CORE_STATUS_INTERNAL_ERROR on error.
 */
xme_core_status_t
chatComponent_create(chatComponent_configStruct_t* config);

/**
 * \brief  Activates a chat component.
 */
xme_core_status_t
chatComponent_activate(chatComponent_configStruct_t* config);

/**
 * \brief  Deactivates a chat component.
 */
void
chatComponent_deactivate(chatComponent_configStruct_t* config);

/**
 * \brief  Destroys a chat component.
 */
void
chatComponent_destroy(chatComponent_configStruct_t* config);

#endif // #ifndef CHATCOMPONENT_H

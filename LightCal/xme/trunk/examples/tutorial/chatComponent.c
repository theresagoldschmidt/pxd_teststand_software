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
 *         Chat example project.
 *
 * \author
 *         Hauke Stähle <staehle@fortiss.org>
 *         Dominik Sojer <sojer@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "chatComponent.h"
#include "xme/core/core.h"
#include "xme/core/dcc.h"
#include "xme/hal/sleep.h"

/******************************************************************************/
/***   Forward declarations                                                 ***/
/******************************************************************************/
static
void
_chatComponent_printChatRoomList(chatComponent_configStruct_t* config, bool enteredOnly);

static
xme_core_status_t
_chatComponent_enterChatRoom(chatComponent_configStruct_t* config, unsigned char chatRoom);

static
xme_core_status_t
_chatComponent_leaveChatRoom(chatComponent_configStruct_t* config, unsigned char chatRoom);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
static
void
_chatComponent_receiveChatMessageCallback(xme_hal_sharedPtr_t dataHandle, void* userData)
{
	unsigned int chatRoom = (unsigned int)userData - CHAT_TOPIC_BASE;
	const char* message = (const char*)xme_hal_sharedPtr_getPointer(dataHandle);

	printf("[%s] %s\n", chatRoomNames[chatRoom], message);
}

static
void
_chatComponent_taskCallback(void* userData)
{
	chatComponent_configStruct_t* config = (chatComponent_configStruct_t*)userData;
	char temp[1024];
	char message[256];
	size_t nameLength = 0;

	XME_LOG(XME_LOG_NOTE, "Please enter your name: ");

	while (fgets(temp, 40, stdin) != NULL || 0 == nameLength)
	{
		nameLength = strlen(temp);
		if (nameLength < 3)
		{
			XME_LOG(XME_LOG_NOTE, "Name too short. Please try again: ");
			nameLength = 0;
		}
		else if (temp[nameLength-1] == '\n')
		{
			temp[nameLength-1] = 0;
			break;
		}
	}

	XME_LOG(XME_LOG_NOTE, "\n");
	XME_LOG(XME_LOG_NOTE, "Welcome to the chat client tutorial, %s!\n", temp);
	XME_LOG(XME_LOG_NOTE, "Please enter some text, which will be sent to all persons in the selected chat\n");
	XME_LOG(XME_LOG_NOTE, "room (marked with (*) below). You can enter multiple chat rooms at the same\n");
	XME_LOG(XME_LOG_NOTE, "time, but send messages only to the selected chat room at any point in time.\n");

	XME_LOG(XME_LOG_NOTE, "\n");
	XME_LOG(XME_LOG_NOTE, "Available commands (chat room numbers can be 1-%d):\n", NUM_CHAT_ROOMS);
	XME_LOG(XME_LOG_NOTE, "- show available chat rooms: '!c'\n");
	XME_LOG(XME_LOG_NOTE, "- enter a chat room:         '!e x', where x is the chat room number\n");
	XME_LOG(XME_LOG_NOTE, "- leave a chat room:         '!l x', where x is the chat room number\n");
	XME_LOG(XME_LOG_NOTE, "- select a chat room:        '!s x', where x is an entered chat room number\n");
	XME_LOG(XME_LOG_NOTE, "- quit the application:      '!q'\n");

	XME_LOG(XME_LOG_NOTE, "\n");
	_chatComponent_printChatRoomList(config, false);

	temp[nameLength-1] = ':';
	temp[nameLength++] = ' ';

	XME_LOG(XME_LOG_NOTE, "\n");
	XME_LOG(XME_LOG_NOTE, "You are now talking in chat room \"%s\".\n", chatRoomNames[config->selectedChatRoom]);

	// Currently, we do not yet have a mechanism to wait for
	// route setup to complete. Hence, we wait here for a moment.
	// This will be fixed in future versions.
	// TODO: See ticket #989.
	xme_hal_sleep_sleep(1000);

	sprintf_s(message, 256, "%.*s has entered this chat room.", nameLength-2, temp);
	xme_core_dcc_sendTopicData(config->publicationHandles[0], message, strlen(message)+1);

	while (fgets(temp+nameLength, 1024-nameLength, stdin) != NULL)
	{
		size_t msgLength = strlen(temp) - nameLength;

		if (msgLength > 0 && temp[nameLength+msgLength-1] == '\n')
		{
			temp[nameLength+msgLength-1] = '\0';

			if (msgLength >= 3 && 0 == strcmp(temp+nameLength, "!c"))
			{
				_chatComponent_printChatRoomList(config, false);
			}
			else if (msgLength > 4 && 0 == strncmp(temp+nameLength, "!e ", 3))
			{
				const unsigned char chatRoom = temp[3+nameLength] - '1';
				if (chatRoom >= NUM_CHAT_ROOMS)
				{
					XME_LOG(XME_LOG_ERROR, "Invalid chat room number!\n");
				}
				else if (XME_CORE_STATUS_SUCCESS != _chatComponent_enterChatRoom(config, chatRoom))
				{
					XME_LOG(XME_LOG_ERROR, "Chat room could not be entered!\n");
				}
				else
				{
					char message[256];

					// Currently, we do not yet have a mechanism to wait for
					// route setup to complete. Hence, we wait here for a moment.
					// This will be fixed in future versions.
					// TODO: See ticket #989.
					xme_hal_sleep_sleep(1000);

					sprintf_s(message, 256, "%.*s has entered this chat room.", nameLength-2, temp);
					xme_core_dcc_sendTopicData(config->publicationHandles[chatRoom], message, strlen(message)+1);

					if (config->selectedChatRoom < 0)
					{
						config->selectedChatRoom = chatRoom;
						XME_LOG(XME_LOG_NOTE, "Selected chat room is now \"%s\". Here is list of entered chat rooms:\n", chatRoomNames[config->selectedChatRoom]);
						_chatComponent_printChatRoomList(config, true);
					}
					else
					{
						XME_LOG(XME_LOG_NOTE, "Here is the list of entered chat rooms:\n");
						_chatComponent_printChatRoomList(config, true);
					}
				}
			}
			else if (msgLength > 4 && 0 == strncmp(temp+nameLength, "!l ", 3))
			{
				// TODO: This will only work after issue #648 has been solved
				const unsigned char chatRoom = temp[3+nameLength] - '1';
				if (chatRoom >= NUM_CHAT_ROOMS)
				{
					XME_LOG(XME_LOG_ERROR, "Invalid chat room number!\n");
				}
				else
				{
					char message[256];
					sprintf_s(message, 256, "%.*s is leaving this chat room.", nameLength-2, temp);
					xme_core_dcc_sendTopicData(config->publicationHandles[chatRoom], message, strlen(message)+1);

					if (XME_CORE_STATUS_SUCCESS != _chatComponent_leaveChatRoom(config, chatRoom))
					{
						XME_LOG(XME_LOG_ERROR, "Chat room could not be left!\n");
					}
					else
					{
						// Try to select another chat room
						if (XME_CORE_DCC_INVALID_PUBLICATION_HANDLE != config->publicationHandles[0])
						{
							config->selectedChatRoom = 0;
							XME_LOG(XME_LOG_NOTE, "Selected chat room is now \"%s\". Here is the list of entered chat rooms:\n", chatRoomNames[config->selectedChatRoom]);
							_chatComponent_printChatRoomList(config, true);
						}
						else
						{
							config->selectedChatRoom = -1;
							XME_LOG(XME_LOG_NOTE, "No chat room selected. Here is the list of entered chat rooms:\n");
							_chatComponent_printChatRoomList(config, true);
						}
					}
				}
			}
			else if (msgLength > 4 && 0 == strncmp(temp+nameLength, "!s ", 3))
			{
				// TODO: This will only work after issue #648 has been solved
				const unsigned char chatRoom = temp[3+nameLength] - '1';
				if (chatRoom >= NUM_CHAT_ROOMS)
				{
					XME_LOG(XME_LOG_ERROR, "Invalid chat room number!\n");
				}
				else if (XME_CORE_DCC_INVALID_PUBLICATION_HANDLE == config->publicationHandles[chatRoom])
				{
					XME_LOG(XME_LOG_ERROR, "You can not select a chat room that you have not entered!\n");
				}
				else
				{
					config->selectedChatRoom = chatRoom;
					XME_LOG(XME_LOG_NOTE, "You are now talking in chat room \"%s\".\n", chatRoomNames[chatRoom]);
				}
			}
			else if (msgLength == 3 && 0 == strncmp(temp+nameLength, "!q", 2))
			{
				unsigned int i;
				char message[256];

				sprintf_s(message, 256, "%.*s is now offline.", nameLength-2, temp);

				for (i=0; i<NUM_CHAT_ROOMS; i++)
				{
					if (XME_CORE_DCC_INVALID_PUBLICATION_HANDLE != config->publicationHandles[i])
					{
						xme_core_dcc_sendTopicData(config->publicationHandles[i], message, strlen(message)+1);
					}
				}

				xme_core_shutdown();
				break;
			}
			else
			{
				if (config->selectedChatRoom >= 0)
				{
					// Send chat message
					xme_core_dcc_sendTopicData(config->publicationHandles[config->selectedChatRoom], temp, nameLength+msgLength+1);
				}
				else
				{
					XME_LOG(XME_LOG_NOTE, "No chat room selected. Here is the chat room list:\n");
					_chatComponent_printChatRoomList(config, false);
				}
			}
		}
	}
}

static
void
_chatComponent_printChatRoomList(chatComponent_configStruct_t* config, bool enteredOnly)
{
	unsigned int i;

	XME_LOG(XME_LOG_NOTE, "%s chat rooms, (+) = entered chat room, (*) = selected chat room:\n", enteredOnly ? "Entered" : "Available");
	for (i=0; i<NUM_CHAT_ROOMS; i++)
	{
		XME_LOG_IF
		(
			!enteredOnly || XME_CORE_DCC_INVALID_PUBLICATION_HANDLE != config->publicationHandles[i],
			XME_LOG_NOTE, "- %d: %s%s%s\n", i+1, chatRoomNames[i],
			XME_CORE_DCC_INVALID_PUBLICATION_HANDLE != config->publicationHandles[i] ? " (+)" : "",
			config->selectedChatRoom == i ? " (*)" : ""
		);
	}
}

static
xme_core_status_t
_chatComponent_enterChatRoom(chatComponent_configStruct_t* config, unsigned char chatRoom)
{
	if (XME_CORE_DCC_INVALID_PUBLICATION_HANDLE != config->publicationHandles[chatRoom])
	{
		XME_LOG(XME_LOG_ERROR, "You have already entered this chat room!\n");
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Create subscription to chat topic
	config->subscriptionHandles[chatRoom] = xme_core_dcc_subscribeTopic
	(
		(xme_core_topic_t)(CHAT_TOPIC_BASE + chatRoom),
		XME_CORE_MD_EMPTY_META_DATA,
		false,
		_chatComponent_receiveChatMessageCallback,
		(void*)(CHAT_TOPIC_BASE + chatRoom)
	);

	// Check for errors
	if (XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE == config->subscriptionHandles[chatRoom])
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Create publisher for chat topic
	config->publicationHandles[chatRoom] = xme_core_dcc_publishTopic
	(
		(xme_core_topic_t)(CHAT_TOPIC_BASE + chatRoom),
		XME_CORE_MD_EMPTY_META_DATA,
		false,
		NULL
	);

	// Check for errors
	if (XME_CORE_DCC_INVALID_PUBLICATION_HANDLE == config->publicationHandles[chatRoom])
	{
		xme_core_dcc_unsubscribeTopic(config->subscriptionHandles[chatRoom]);
		config->subscriptionHandles[chatRoom] = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;

		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	return XME_CORE_STATUS_SUCCESS;
}

static
xme_core_status_t
_chatComponent_leaveChatRoom(chatComponent_configStruct_t* config, unsigned char chatRoom)
{
	if (XME_CORE_DCC_INVALID_PUBLICATION_HANDLE == config->publicationHandles[chatRoom])
	{
		XME_LOG(XME_LOG_ERROR, "You did not enter this chat room!\n");
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// TODO: This will only work after issue #648 has been solved.
	xme_core_dcc_unsubscribeTopic(config->subscriptionHandles[chatRoom]);
	xme_core_dcc_unpublishTopic(config->publicationHandles[chatRoom]);

	config->publicationHandles[chatRoom] = XME_CORE_DCC_INVALID_PUBLICATION_HANDLE;
	config->subscriptionHandles[chatRoom] = XME_CORE_DCC_INVALID_SUBSCRIPTION_HANDLE;

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
chatComponent_create(chatComponent_configStruct_t* config)
{
	config->selectedChatRoom = -1;

	// Initially, enter the lobby
	if (XME_CORE_STATUS_SUCCESS == _chatComponent_enterChatRoom(config, 0))
	{
		config->selectedChatRoom = 0;
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
chatComponent_activate(chatComponent_configStruct_t* config)
{
	config->taskHandle =
		xme_core_resourceManager_scheduleTask
		(
		 	0, 0,
			XME_HAL_SCHED_PRIORITY_NORMAL,
			_chatComponent_taskCallback,
			config
		);

	// Check for errors
	if (XME_CORE_RESOURCEMANAGER_TASK_INFEASIBLE == config->taskHandle)
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	return XME_CORE_STATUS_SUCCESS;
}

void
chatComponent_deactivate(chatComponent_configStruct_t* config)
{
	xme_core_resourceManager_killTask(config->taskHandle);
}

void
chatComponent_destroy(chatComponent_configStruct_t* config)
{
	unsigned int i;
	for (i=0; i<NUM_CHAT_ROOMS; i++)
	{
		xme_core_dcc_unsubscribeTopic(config->subscriptionHandles[i]);
		xme_core_dcc_unpublishTopic(config->publicationHandles[i]);
	}
}

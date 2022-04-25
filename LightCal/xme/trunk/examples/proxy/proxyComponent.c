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
 *         Proxy component.
 *
 * \author
 *         Christian Fuchs <fuchsch@in.tum.de>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/component.h"
#include "xme/core/core.h"
#include "xme/core/defines.h"
#include "xme/core/nodeManager.h"
#include "xme/core/interfaceManager.h"
#include "xme/core/broker.h"
#include "xme/core/topic.h"
#include "proxyComponent.h"
// Libs for networking
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/

/******************************************************************************/
/***   Component configurations                                             ***/
/******************************************************************************/

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_dcc_publicationHandle_t pubHandle;
int udp_socket;

#define PROXY_TOPIC_ID (xme_core_topic_t)300
#define PROXY_CHANNEL 123

xme_core_status_t
proxyComponent_create(xme_core_nodeManager_configStruct_t* config)
{

	// Create publisher for proxy topic
	pubHandle = xme_core_dcc_publishTopic( 
		PROXY_TOPIC_ID, 
		XME_CORE_MD_EMPTY_META_DATA,
		false,
		NULL
	);

	if ( pubHandle == XME_CORE_DCC_INVALID_PUBLICATION_HANDLE )
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Local channel from proxy component
	if ( xme_core_routingTable_addLocalSourceRoute( 
		PROXY_CHANNEL, 
		xme_core_dcc_getComponentFromPublicationHandle(pubHandle),
		xme_core_dcc_getPortFromPublicationHandle(pubHandle) 
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	// Remote channel from proxy component
	if ( xme_core_routingTable_addOutboundRoute(
		PROXY_CHANNEL,
		(xme_core_interface_interfaceId_t) 1
	) != XME_CORE_STATUS_SUCCESS ) 	
	{
		return XME_CORE_STATUS_INTERNAL_ERROR;
	};

	return XME_CORE_STATUS_SUCCESS;
}

void
proxyComponent_callback (void* userData)
{

	/* Creating socket for IPv6 and IPv4 messages */
	char buf[1024],host[NI_MAXHOST],service[NI_MAXSERV];
	struct sockaddr_storage sockstor;
	socklen_t len = sizeof(sockstor);
	int aiErr, message_size;
	struct addrinfo *aiHead,*ai;
	struct addrinfo hints = { .ai_flags = AI_PASSIVE,
				  .ai_family = PF_INET6,
				  .ai_socktype = SOCK_DGRAM,
				  .ai_protocol = IPPROTO_UDP};

	aiErr = getaddrinfo("aaaa::1","5200",&hints,&aiHead);

	for(ai = aiHead; (ai!=NULL);ai=ai->ai_next) {
		aiErr = getnameinfo(ai->ai_addr,ai->ai_addrlen,
					host,sizeof(host),service,sizeof(service),
					NI_NUMERICHOST | NI_NUMERICSERV);
		printf("Setting up socket with address: %s:%s\n",host,service);

		if ((udp_socket = socket(ai->ai_family,ai->ai_socktype,ai->ai_protocol))<0) {
			printf("Error while creating socket\n");
			exit(1);
		}
		aiErr = bind(udp_socket,ai->ai_addr,ai->ai_addrlen);
		if (aiErr < 0) {
			printf("Error while bind %d\n",aiErr);
			exit(1);
		}
	}

	while( 1 )
	{
		
		len = sizeof(sockstor);
		message_size = recvfrom(udp_socket,buf,sizeof(buf),0,
					(struct sockaddr *)&sockstor,&len);
		// Print info about packet sender
		aiErr = getnameinfo((struct sockaddr *)&sockstor,len,
					host,sizeof(host),service,sizeof(service),
					NI_NUMERICHOST | NI_NUMERICSERV);
		printf("Packet from %s:%s\n",host,service);
		buf[message_size] = '\0';
		xme_core_dcc_sendTopicData(pubHandle, buf, message_size+1);
	
	}
}

xme_core_status_t
proxyComponent_activate(xme_core_nodeManager_configStruct_t* config)
{
	xme_hal_sched_addTask(0, 0, 0, proxyComponent_callback, NULL);
	return XME_CORE_STATUS_SUCCESS;
}

void
proxyComponent_deactivate(xme_core_nodeManager_configStruct_t* config)
{
	// Nothing to do here
}

void
proxyComponent_destroy(xme_core_nodeManager_configStruct_t* config)
{
	close(udp_socket);

	if ( xme_core_routingTable_removeLocalSourceRoute(PROXY_CHANNEL) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeLocalSourceRoute");
	}

	if ( 
		xme_core_routingTable_removeOutboundRoute(
		PROXY_CHANNEL, 
		(xme_core_interface_interfaceId_t) 1
		) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_broker_removeOutboundRoute");
	}

	if ( xme_core_dcc_unpublishTopic(pubHandle) != XME_CORE_STATUS_SUCCESS )
	{
		printf("Failure xme_core_dcc_unpublishTopic");
	}
}

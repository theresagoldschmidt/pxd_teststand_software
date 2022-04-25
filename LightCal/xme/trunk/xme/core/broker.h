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
 *         Broker.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Stephan Sommer <sommer@fortiss.org>
 */

#ifndef XME_CORE_BROKER_H
#define XME_CORE_BROKER_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/component.h"
#include "xme/core/core.h"
#include "xme/core/interface.h"
#include "xme/core/nodeManager.h"
#include "xme/core/routingTable.h"
#include "xme/hal/sharedPtr.h"
#include "xme/core/resourceManager.h"
#include "xme/core/dataChannel.h"

#include <stdint.h>

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
typedef uint16_t xme_core_broker_route_t;

typedef struct
{
	//xme_core_dcc_topicChannel_t topicChannel;
	//xme_core_broker_nextHop_callback callback;
	void *userData; // if local callback: user-defined data, otherwise: topic ID and node ID of next recipient
}
xme_core_broker_tableRow;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Forwards topic data from the given component port to receivers
 *         in the routing table.
 *
 * \param  sendingComponent Component that sent the topic data.
 * \param  fromPublicationPort Port that issued the topic data.
 * \param  dataHandle Handle to buffer with topic data to send.
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the topic data have been forwarded
 *            to at least one subscriber. Note that this does not necessarily
 *            imply that the topic data have actually been delivered. It just
 *            means that there exists a route in the local routing table that
 *            forwards the data.
 *          - XME_CORE_STATUS_NOT_FOUND if no subscriber could be found in
 *            the local routing table.
 */
xme_core_status_t
xme_core_broker_dccSendTopicData
(
	xme_core_component_t sendingComponent,
	xme_core_component_port_t fromPublicationPort,
	xme_hal_sharedPtr_t dataHandle
);

/**
 * \brief  Forwards request data from the given component port to receivers
 *         in the routing table.
 *
 * \param  sendingComponent Component that sent the request data.
 * \param  fromRequestPort Port that issued the request.
 * \param  dataHandle Handle to buffer with request data to send.
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the request data have been delivered
 *            to at least one request handler. Note that this does not
 *            necessarily imply that the request data have actually been
 *            delivered. It just means that there exists a route in the local
 *            routing table that forwards the data.
 *          - XME_CORE_STATUS_NOT_FOUND if no request handler could be found
 *            in the local routing table
 */
xme_core_status_t
xme_core_broker_rrSendRequest
(
	xme_core_component_t sendingComponent,
	xme_core_component_port_t fromRequestPort,
	xme_hal_sharedPtr_t dataHandle
);

/**
 * \brief  Forwards response data from the given component port to receivers
 *         in the routing table.
 *
 * \param  responseDataChannel Data channel to send response to.
 * \param  sendingComponent Component that sent the reponse data.
 * \param  fromRequestHandlerPort Request handler port that received the
 *         request.
 * \param  dataHandle Handle to buffer with response data to send.
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the response data have been delivered
 *            to at least one request issuer. Note that this does not
 *            necessarily imply that the response data have actually been
 *            delivered. It just means that there exists a route in the local
 *            routing table that forwards the data.
 *          - XME_CORE_STATUS_NOT_FOUND if no request issuer could be found
 *            in the local routing table.
 */
xme_core_status_t
xme_core_broker_rrSendResponse
(
	xme_core_dataChannel_t responseDataChannel,
	xme_core_component_t sendingComponent,
	xme_core_component_port_t fromRequestHandlerPort,
	xme_hal_sharedPtr_t dataHandle
);

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Initializes the broker component.
 *         Exactly one component of this type must be present on every node.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_SUCCESS if the broker component has been properly
 *            initialized.
 *          - XME_CORE_STATUS_OUT_OF_RESOURCES if initialization failed.
 */
xme_core_status_t
xme_core_broker_init();

/**
 * \brief  Frees all resources occupied by the broker component.
 *         Exactly one component of this type must be present on every node.
 */
void
xme_core_broker_fini();

/**
 * \brief  Callback function to handle data exchange between broker and network for
 *         incomming data. This method needs to be called by the network, was soon as data is
 *         ready at an interface. The data is then retrieved and stored into a 
 *         memory location referenced by a data handle and the delivery to the 
 *         recipients is triggered.
 *
 * \param  status Indicator why this callback was called.
 * \param  interfaceHandle Handle to interface at which data was received.
 */
static void
xme_core_broker_interfaceManager_callback
(
	xme_core_interface_state_t status,
	xme_core_interface_interfaceId_t interfaceHandle
);

/**
 * \brief  Manages data delivery in Broker-Component and handles delivery to right components
 *         like >xme_core_broker_deliverDataToComponent< and >xme_core_broker_deliverDataToNetwork<.
 *         In addition, the translation of routes is performed in this function.
 *
 * \param  dataChannel Globally unique data channel on which data is received for delivery.
 * \param  dataHandle Handle to data to be delivered to component.
 * \param  sendingComponent Handle to the (local) sending component. Set parameter to 
 *         XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT if sending component is unkown.
 * \return Returns one of the following status codes:
 *          - XME_CORE_SUCCESS if the function is executed.
 *          - TODO: handle errors!! See ticket #721
 */
xme_core_status_t
xme_core_broker_deliverData
(
	xme_core_dataChannel_t dataChannel,
	xme_hal_sharedPtr_t dataHandle,
	xme_core_component_t sendingComponent
);


/**
 * \brief  Delivers data from broker to channel transformation method. If a transformation
 *         is specified, xme_core_broker_deliverData is called with new channel information.
 *
 * \param  dataChannel Globally unique data channel on which data is received for delivery.
 * \param  dataHandle Handle to data to be delivered to component.
 * \param  sendingComponent Handle to the (local) sending component. Set parameter to 
 *         XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT if sending component is unkown.
 * \return Returns one of the following status codes:
 *          - XME_CORE_SUCCESS if the data is received by the networking component.
 *          - TODO: handle errors!! See ticket #721
 */
xme_core_status_t
xme_core_broker_deleiverDataToTransformation
(
	xme_core_dataChannel_t dataChannel,
	xme_hal_sharedPtr_t dataHandle,
	xme_core_component_t sendingComponent
);

/**
 * \brief  Delivers data from broker to local components.
 *
 * This function will not loop messages. This means, if a local component published on a
 * dataChannel it is also subscribed to, it will not get a copy of that message.
 *
 * \param  dataChannel Globally unique data channel on which data is received for delivery.
 * \param  dataHandle Handle to data to be delivered to component.
 * \param  sendingComponent Handle to the (local) sending component. Set parameter to 
 *         XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT if sending component is unkown.
 * \return Returns one of the following status codes:
 *          - XME_CORE_SUCCESS if the data is received by the networking component.
 *          - TODO: handle errors!! See ticket #721
 */
xme_core_status_t
xme_core_broker_deliverDataToComponent
(
	xme_core_dataChannel_t sourceDataChannel,
	xme_hal_sharedPtr_t dataHandle,
	xme_core_component_t sendingComponent
);

/**
 * \brief  Delivers data from broker to network.
 *
 * \param  dataChannel Globally unique data channel on which data will be delivered.
 * \param  dataHandle Handle to data to be delivered by the network.
 * \return Returns one of the following status codes:
 *          - XME_CORE_SUCCESS if the data is received by the networking component.
 *          - TODO: handle errors!! See ticket #721
 */
xme_core_status_t
xme_core_broker_deliverDataToNetwork
(
	xme_core_dataChannel_t dataChannel,
	xme_hal_sharedPtr_t dataHandle
);

#endif // #ifndef XME_CORE_BROKER_H

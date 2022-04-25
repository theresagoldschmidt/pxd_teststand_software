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
 *         Routing table.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/*
TODO: See ticket #767
- What happens if a route that is about to be added already exists in the routing table?
- What happens if a non-existing route is about to be deleted?
- Do routing information have to be persisted on the node?
*/

#ifndef XME_CORE_ROUTINGTABLE_H
#define XME_CORE_ROUTINGTABLE_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdbool.h>

#include "xme/core/component.h"
#include "xme/core/core.h"
#include "xme/core/dataChannel.h"
#include "xme/core/interface.h"
#include "xme/core/resourceManager.h"


/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
typedef struct
{
	xme_core_dataChannel_t dataChannel; ///< Data channel over which data are to be transmitted.
	xme_core_dataChannel_properties_t dataChannelProperties; ///< Properties of the data channel (e.g., reliability).
	xme_core_component_t fromComponent; ///< Component where data are originating from.
	xme_core_component_port_t fromPort; ///< Port where data are originating from.
	uint16_t lockCount; ///< Number of times this route has been established.
}
xme_core_routingTable_localSourceRoute_t;

typedef struct
{
	xme_core_dataChannel_t dataChannel; ///< Data channel over which data arrive.
	xme_core_dataChannel_properties_t dataChannelProperties; ///< Properties of the data channel (e.g., reliability).
	xme_core_component_t toComponent; ///< ID of component to deliver data to
	xme_core_component_port_t toPort; ///< Port to deliver data to.
	uint16_t lockCount; ///< Number of times this route has been established.
}
xme_core_routingTable_localDestinationRoute_t;

typedef struct
{
	xme_core_dataChannel_t dataChannel; ///< Data channel over which data is sent.
	xme_core_interface_interfaceId_t viaInterface; ///< Interface used to send data
	uint16_t lockCount; ///< Number of times this route has been established.
}
xme_core_routingTable_outboundRoute_t;

typedef struct
{
	xme_core_dataChannel_t dataChannel; ///< Data channel over which data arrive.
	xme_core_interface_interfaceId_t inInterface; ///< Interface used to receive data.
	uint16_t lockCount; ///< Number of times this route has been established.
}
xme_core_routingTable_inboundRoute_t;

typedef struct
{
	xme_core_dataChannel_t sourceDataChannel; ///< Data channel over which data arrive.
	xme_core_dataChannel_t destinationDataChannel; ///< Data channel over which data are to be transmitted.
	uint16_t lockCount; ///< Number of times this route has been established.
}
xme_core_routingTable_translationRoute_t;

typedef struct
{
	// TODO (See ticket #768): This should be changed to a type of table that allows efficient
	//                         browsing by data channel (i.e., dataChannel should be the primary
	//                         key of the respective table). Currently, we are simply searching
	//                         linearly for a matching item.
	XME_HAL_TABLE(xme_core_routingTable_localSourceRoute_t, localSourceRoutes, XME_HAL_DEFINES_MAX_LOCAL_SOURCE_ROUTE_ITEMS);
	XME_HAL_TABLE(xme_core_routingTable_localDestinationRoute_t, localDestinationRoutes, XME_HAL_DEFINES_MAX_LOCAL_DESTINATION_ROUTE_ITEMS);
	XME_HAL_TABLE(xme_core_routingTable_outboundRoute_t, outboundRoutes, XME_HAL_DEFINES_MAX_OUTBOUND_ROUTE_ITEMS);
	XME_HAL_TABLE(xme_core_routingTable_inboundRoute_t, inboundRoutes, XME_HAL_DEFINES_MAX_INBOUND_ROUTE_ITEMS);
	XME_HAL_TABLE(xme_core_routingTable_translationRoute_t, translationRoutes, XME_HAL_DEFINES_MAX_TRANSLATION_ROUTE_ITEMS);
}
xme_core_routingTable_configStruct_t;

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
extern xme_core_routingTable_configStruct_t xme_core_routingTable_config;



/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Initializes the routing table component.
 *         Exactly one component of this type must be present on every node.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_SUCCESS if the routing table component has been
 *            properly initialized.
 *          - XME_CORE_STATUS_OUT_OF_RESOURCES if initialization failed.
 */
xme_core_status_t
xme_core_routingTable_init();

/**
 * \brief  Frees all resources occupied by the routing table component.
 *         Exactly one component of this type must be present on every node.
 */
void
xme_core_routingTable_fini();

/**
 * \brief  Adds a local source route entry to the routing table.
 *
 *         A local source route is a route that originates from a component on
 *         the local node and defines the data channel that is associated to
 *         that route.
 *
 * \param  dataChannel Globally unique data channel to associate with the
 *         local source route.
 * \param  fromComponent Locally valid identifier of the component that
 *         publishes the data on the route.
 * \param  fromPort Locally valid identifier of the port where the data
 *         originate. This can either be a data centric publication or a
 *         request publication.
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the route has been added successfully.
 *          - XME_CORE_STATUS_INVALID_PARAMETER if fromComponent does not
 *            specify a valid component or fromPort does not specify a valid
 *            publication port.
 *          - XME_CORE_OUT_OF_RESOURCES if the route can not be added, because
 *            the routing table component is out of resources.
 */
xme_core_status_t
xme_core_routingTable_addLocalSourceRoute
(
	xme_core_dataChannel_t dataChannel,
	xme_core_component_t fromComponent,
	xme_core_component_port_t fromPort
);

/**
 * \brief  Adds a local destination route entry to the routing table.
 *
 *         A local destination route is a route that ends at a component on
 *         the local node and defines the data channel that is associated to
 *         that route.
 *
 * \param  dataChannel Globally unique data channel to associate with the
 *         local destination route.
 * \param  toComponent Locally valid identifier of the component that receives
 *         the data on the route.
 * \param  toPort Locally valid identifier of the port where the data are
 *         delivered to. This can either be a data centric subscription or a
 *         response subscription.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the route has been added successfully.
 *          - XME_CORE_STATUS_INVALID_PARAMETER if toComponent does not
 *            specify a valid component or toPort does not specify a valid
 *            subscription port.
 *          - XME_CORE_OUT_OF_RESOURCES if the route can not be added, because
 *            the routing table component is out of resources.
 */
xme_core_status_t
xme_core_routingTable_addLocalDestinationRoute
(
	xme_core_dataChannel_t dataChannel,
	xme_core_component_t toComponent,
	xme_core_component_port_t toPort
);

/**
 * \brief  Adds an outbound route entry to the routing table.
 *
 *         An outbound route is a route that begins at the local broker
 *         component and leads to the broker on a different node and defines
 *         the data channel that is associated to that route.
 *
 * \param  dataChannel Globally unique data channel to associate with the
 *         outbound route.
 * \param  toInterface Locally valid identifier of the interface that should
 *         be used to send the data.
 * \param  toAddress Interface-specific addressing data used to reach the next
 *         hop on the route.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the route has been added successfully.
 *          - XME_CORE_STATUS_INVALID_PARAMETER if toInterface does not
 *            specify a valid interface.
 *          - XME_CORE_OUT_OF_RESOURCES if the route can not be added, because
 *            the routing table component is out of resources.
 */
xme_core_status_t
xme_core_routingTable_addOutboundRoute
(
	xme_core_dataChannel_t dataChannel,
	xme_core_interface_interfaceId_t toInterface
);

/**
 * \brief  Adds an inbound route entry to the routing table.
 *
 *         An inbound route is a route that begins at a network interface and
 *         ends at the local broker. Inbound routes are neccessary to distinguish
 *         wanted from unwanted data and to prepare a network interface for
 *         the reception of data at some interface-specific address, slot
 *         and/or port.
 *
 *         In the case of reliable/TCP-based connections, an inbound route
 *         tells the network interface to open a specific port and listen
 *         for a connection.
 *
 *         In the case of unreliable/UDP-based connections, an inbound route
 *         tells the network interface for a certain type of multicast-traffic.
 *
 * \param  dataChannel Globally unique data channel to associate with the
 *         inbound route.
 * \param  inInterface Locally valid identifier of the interface that should
 *         be used to receive the data.
 * \param  inAddress Interface-specific addressing data the interface should
 *         forward.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the route has been added successfully.
 *          - XME_CORE_STATUS_INVALID_PARAMETER if toInterface does not
 *            specify a valid interface.
 *          - XME_CORE_OUT_OF_RESOURCES if the route can not be added, because
 *            the routing table component is out of resources.
 */
xme_core_status_t
xme_core_routingTable_addInboundRoute
(
	xme_core_dataChannel_t dataChannel,
	xme_core_interface_interfaceId_t inInterface
);

/**
 * \brief  Adds a translation route entry to the routing table.
 *
 *         A translation route is a mapping (renaming) between two data
 *         channels. Any data arriving at the node with their data channel
 *         set to sourceDataChannel are treated as if they were received
 *         with their data channel set to destDataChannel (in addition to
 *         any processing performed with them according to sourceDataChannel).
 *
 * \param  sourceDataChannel Globally unique data channel for incoming packets
 *         to be translated.
 * \param  destinationDataChannel Globally unique data channel for outgoing
 *         packets after "renaming" their data channel.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the route has been added successfully.
 *          - XME_CORE_STATUS_INVALID_PARAMETER if sourceDataChannel is equal
 *            to destinationDataChannel.
 *          - XME_CORE_OUT_OF_RESOURCES if the route can not be added, because
 *            the routing table component is out of resources.
 */
xme_core_status_t
xme_core_routingTable_addTranslationRoute
(
	xme_core_dataChannel_t sourceDataChannel,
	xme_core_dataChannel_t destinationDataChannel
);

/**
 * \brief  Removes a local source route from the routing table.
 *
 * \param  dataChannel Data channel identifying the local source route to
 *         remove.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the route has been successfully
 *            removed.
 *          - XME_CORE_STATUS_NO_SUCH_VALUE if dataChannel does not specify an
 *            existing local source route.
 */
xme_core_status_t
xme_core_routingTable_removeLocalSourceRoute
(
	xme_core_dataChannel_t dataChannel
);

/**
 * \brief  Removes a local destination route from the routing table.
 *
 * \param  dataChannel Data channel identifying the local destination route to
 *         remove.
 * \param toComponent Component subscribing to the data
 * \param toSubscription Port of component subscribing to the data
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the route has been successfully
 *            removed.
 *          - XME_CORE_STATUS_NO_SUCH_VALUE if dataChannel does not specify an
 *            existing local destination route.
 */
xme_core_status_t
xme_core_routingTable_removeLocalDestinationRoute
(
	xme_core_dataChannel_t dataChannel,
	xme_core_component_t toComponent,
	xme_core_component_port_t toSubscription
);

/**
 * \brief  Removes an outbound route from the routing table. All parameters are
 *         required to definetely identify the route to be deleted.
 *
 * \param  dataChannel Data channel identifying the outbound route to remove.
 * \param  toInterface Interface to deliver data to.
 * \param  toAddress Address  on interface to deliver data to.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the route has been successfully
 *            removed.
 *          - XME_CORE_STATUS_NO_SUCH_VALUE if dataChannel does not specify an
 *            existing outbound route.
 */
xme_core_status_t
xme_core_routingTable_removeOutboundRoute
(
	xme_core_dataChannel_t dataChannel,
	xme_core_interface_interfaceId_t toInterface
);

/**
 * \brief  Removes an inbound route from the routing table. All parameters are
 *         required to definetely identify the route to be deleted.
 *
 * \param  dataChannel Data channel identifying the inbound route to remove.
 * \param  inInterface Interface to receive data from..
 * \param  inAddress Address on interface to receive data from.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the route has been successfully
 *            removed.
 *          - XME_CORE_STATUS_NO_SUCH_VALUE if dataChannel does not specify an
 *            existing outbound route.
 */
xme_core_status_t
xme_core_routingTable_removeInboundRoute
(
	xme_core_dataChannel_t dataChannel,
	xme_core_interface_interfaceId_t inInterface
);

/**
 * \brief  Removes a translation route from the routing table.
 *
 * \param  sourceDataChannel Globally unique data channel for incoming packets
 *         to be translated.
 * \param  destinationDataChannel Globally unique data channel for outgoing
 *         packets after "renaming" their data channel.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the route has been successfully
 *            removed.
 *          - XME_CORE_STATUS_NO_SUCH_VALUE if the combination of
 *            sourceDataChannel and destDataChannel does not specify an
 *            existing translation route.
 */
xme_core_status_t
xme_core_routingTable_removeTranslationRoute
(
	xme_core_dataChannel_t sourceDataChannel,
	xme_core_dataChannel_t destinationDataChannel
);

#endif // #ifndef XME_CORE_ROUTINGTABLE_H

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
 *         Communication interface abstraction.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Hauke Stähle <staehle@fortiss.org>
 */

#ifndef XME_CORE_INTERFACE_H
#define XME_CORE_INTERFACE_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/table.h"
#include "xme/core/dataChannel.h"
#include "xme/hal/sharedPtr.h"
#include "xme/core/interface.h"

#include <stdint.h>

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_core_interface_t
 *
 * \brief  Locally unique interface identifier.
 */
typedef enum
{
	XME_CORE_INTERFACEMANAGER_INVALID_INTERFACE_ID = 0, ///< Invalid interface identifier.
	XME_CORE_INTERFACEMANAGER_MAX_INTERFACE_ID = XME_HAL_TABLE_MAX_ROW_HANDLE ///< Largest possible interface identifier.
}
xme_core_interface_interfaceId_t;

/**
 * \typedef xme_core_interface_address_t
 *
 * \brief  Interface-specific address data.
 */
typedef struct
{
	uint8_t data[16];
}
xme_core_interface_address_t;

/**
 * \typedef xme_core_interface_state_t
 *
 * \brief The state of an interface. Used for callbacks.
 */
typedef enum
{
	XME_CORE_INTERFACE_STATE_UNDEFINED = 0, ///< Interface in uninitialized.
	XME_CORE_INTERFACE_STATE_IDLE, ///< Interface ready to receive/send data.
	XME_CORE_INTERFACE_STATE_DATA_AVAILABLE, ///< New data is available.
	XME_CORE_INTERFACE_STATE_ERROR   ///< Error occured. Interface is not reliable anymore.
}
xme_core_interface_state_t;

/**
 * \typedef xme_core_interface_callback_t
 *
 * \brief  Callback function to notify about a status change within an interface.
 *
 * \param  status New state of interface.
 */
typedef void (*xme_core_interface_callback_t)
(
	xme_core_interface_state_t status,
	xme_core_interface_interfaceId_t interfaceID
);

/**
 * \typedef xme_hal_net_interfaceDescr_t
 *
 * \brief  Generic interface for communication device access.
 *         Every device has to implement the full set of these functions.
 */
typedef struct
{
	// public
	xme_core_interface_interfaceId_t interfaceID;

	xme_core_status_t (*init) (xme_core_interface_address_t *localAddress);
	xme_core_status_t (*fini) (void);

	uint16_t (*read_non_blocking) (void *buffer, uint16_t count, xme_core_dataChannel_t *channel);
	uint16_t (*read_blocking) (void *buffer, uint16_t count, xme_core_dataChannel_t *channel);

	uint16_t (*write_non_blocking) (const void *buffer, uint16_t count, xme_core_dataChannel_t channel);
	uint16_t (*write_blocking) (const void *buffer, uint16_t count, xme_core_dataChannel_t channel);

	xme_core_status_t (*join_channel) (xme_core_dataChannel_t channel);
	xme_core_status_t (*leave_channel) (xme_core_dataChannel_t channel);

	uint16_t (*get_available_data_size) (void);

	xme_core_interface_state_t (*wait_for_state_change) (void);

	void (*register_callback) (xme_core_interface_callback_t cb);
	void (*clear_callback) (void);

	xme_core_status_t (*provide_channel) (xme_core_dataChannel_t channel);
	xme_core_status_t (*unprovide_channel) (xme_core_dataChannel_t channel);

	xme_core_status_t (*add_channel_to_phy_address_mapping) (xme_core_dataChannel_t channel, xme_core_interface_address_t *address);
	xme_core_status_t (*remove_channel_to_phy_address_mapping) (xme_core_dataChannel_t channel, xme_core_interface_address_t *address);

	// private
}
xme_hal_net_interfaceDescr_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
xme_hal_net_interfaceDescr_t*
xme_core_interface_getInterface
(
	xme_core_interface_interfaceId_t interfaceId
);

/******************************************************************************/
/***   Defines	                                                            ***/
/******************************************************************************/
#define GENERIC_ADDRESS_TO_HOST_TCP(addr) \
	(*((char **)((addr).data)))

#define GENERIC_ADDRESS_TO_HOST_TCP_MALLOC_TCP(addr, charPtr) \
	do { \
		if ( GENERIC_ADDRESS_TO_HOST_TCP(addr) != NULL ) \
		{ \
			uint16_t size; \
			size = strlen( GENERIC_ADDRESS_TO_HOST_TCP(addr) ) + 1; \
			charPtr = (char *)xme_hal_mem_alloc( size ); \
			memcpy( charPtr, GENERIC_ADDRESS_TO_HOST_TCP(addr), size ); \
		} else { \
			charPtr = NULL; \
		} \
	} while(0);

#define GENERIC_ADDRESS_TO_PORT_TCP(addr, port) \
	do { \
		port = *(uint16_t *)(&((addr).data[4])); \
	} while(0);

#define PORT_TO_GENERIC_ADDRESS_TCP(addr, port) \
	do { \
		(*(uint16_t *)(&((addr).data[4]))) = port; \
	} while(0);

#define HOST_TO_GENERIC_ADDRESS_TCP(addr, host) \
	do { \
		*((char **)(addr).data) = host; \
	} while(0);

#endif // #ifndef XME_CORE_INTERFACE_H

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
 *         The interface manager.
 *
 * \author
 *         Hauke Stähle <staehle@fortiss.org>
 */

#ifndef XME_CORE_INTERFACE_MANAGER_H
#define XME_CORE_INTERFACE_MANAGER_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/table.h"
#include "xme/core/dataChannel.h"
#include "xme/core/interface.h"

#include <stdint.h>

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
#define XME_CORE_INTERFACEMANAGER_INTERFACES_MAX 10

/**
 * \typedef xme_core_interfaceManager_callback_t
 *
 * \brief  Callback function to notify about a status change of an interface.
 *
 * \param  status New state of interface.
 * \param  interfaceHandle Handle to interface with changed status.
 */
typedef void (*xme_core_interfaceManager_callback_t)
	(
		xme_core_interface_state_t status,
		xme_core_interface_interfaceId_t interfaceHandle
	);

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

/**
 * \brief  Initializes the interface manager including all involved interfaces.
 *
 * \return Returns one of the following values:
 *          - XME_CORE_STATUS_SUCCESS if all devices have been successfully
 *            initialized.
 *          - XME_CORE_STATUS_OUT_OF_RESOURCES if errors occured.
 */
xme_core_status_t
xme_core_interfaceManager_init();

/**
 * \brief  Frees all resources associated to the interface manager.
 */
void
xme_core_interfaceManager_fini();

/**
 * \brief  Reads data from a specific interface.
 *
 *         The behavior is non-blocking reading; i.e. the function will return
 *         immediately if no new data is available. The function will allocate
 *         a tracked shared memory area and copy the input into it if data is
 *         available.
 *
 * \param  interfaceRef Handle to the interface for reading.
 * \param  channel Pointer to store the channel number of the received data.
 *
 * \return Returns a handle to shared memory where the data is stored. The size
 *         of that area might be zero. Returns an invalid handle if an error
 *         occured.
 *
 */
xme_hal_sharedPtr_t 
xme_core_interfaceManager_read_non_blocking
(
	xme_core_interface_interfaceId_t interfaceRef,
	xme_core_dataChannel_t *channel
);

/**
 * \brief  Reads data from a specific interface.
 *
 *         The behavior is blocking reading; i.e. the function will wait until
 *         data is available. The function will allocate a tracked shared memory
 *         area and copy the input into it.
 *
 * \param  interfaceRef Handle to the interface for reading.
 * \param  channel Pointer to store the channel number of the received data.
 *
 * \return Returns a handle to the tracked shared memory area where the input
 *         data is stored. Returns an invalid handle on error.
 */
xme_hal_sharedPtr_t
xme_core_interfaceManager_read_blocking
(
	xme_core_interface_interfaceId_t interfaceRef,
	xme_core_dataChannel_t *channel
);

/**
 * \brief  Writes data to a specific interface.
 *
 *         The behavior is non-blocking writing; i.e. the function will
 *         copy the data of the buffer for sending and return immediately.
 *         The buffer can be reused right after the function call.
 *
 * \param  interfaceRef Handle to the interface for writing.
 * \param  channel Channel the data should be sent to.
 * \param  shmHandle Handle to the shared memory area.
 *
 * \return Returns the number of bytes that have been copied into the internal
 *         buffer. Might be zero if the internal buffer of the device is full.
 *         Returns 0 on error.
 */
uint16_t
xme_core_interfaceManager_write_non_blocking
(
	xme_core_interface_interfaceId_t interfaceRef,
	xme_core_dataChannel_t channel,
	xme_hal_sharedPtr_t shmHandle
);

/**
 * \brief   Writes data to a specific interface.
 *
 *          The behavior is blocking writing; i.e. the function will wait
 *          until the data is either put on a physical line or is successfully
 *          transfered into a hardware buffer. This function does not gurantee
 *          that the data actually reached the destination.
 *
 * \param  interfaceRef Handle to the interface for writing.
 * \param  channel Channel the data should be sent to.
 * \param  shmHandle Handle to the shared memory area.
 *
 * \return Returns the number of bytes that have actually been processed. If and only
 *         if an error occoured, the number of written bytes does not match the size
 *         of the shared memory area.
 */
uint16_t
xme_core_interfaceManager_write_blocking
(
	xme_core_interface_interfaceId_t interfaceRef,
	xme_core_dataChannel_t channel,
	xme_hal_sharedPtr_t shmHandle
);

/**
 * \brief Configures specific interface to join a specific channel.
 *        This function will configure the underlying protocol or hardware
 *        to receive data dedicated to a specific channel.
 *
 *        For connectionless protocols this means, that the underlying
 *        protocol will forward broadcast and/or multicast messages that
 *        correspond to a specific channel.
 *
 *        For connection-oriented protocols this means, that the underlying
 *        protocol will initiate a handshake to all hosts that provide
 *        this channel.
 *
 *        It is allowed to join a channel multiple times although this
 *        will not have any affect on the underlying interface.
 *
 * \param interfaceRef Handle to the interface to configure.
 * \param channelNumber The channel to join.
 *
 * \return Returns one of the following status codes:
 *         - XME_CORE_STATUS_SUCCESS The device was successfully 
 *           configured to listen to the channel.
 *         - XME_CORE_STATUS_OUT_OF_RESOURCES No ressources are
 *           available to manage listening to another channel.
 *         - XME_CORE_STATUS_INTERNAL_ERROR The configuration request
 *           failed for unspecific reasons.
 *         - XME_CORE_STATUS_NO_SUCH_VALUE Either interfaceRef or
 *           channelNumber are not valid.
 */
xme_core_status_t
xme_core_interfaceManager_join_channel
(
	xme_core_interface_interfaceId_t interfaceRef,
	xme_core_dataChannel_t channelNumber
);

/**
 * \brief Configures specific interface to drop membership of a specific channel.
 *        This function will configure the underlying protocol or hardware
 *        to drop the membership to a specific channel. It does not effect writing.
 *
 *        For connectionless protocols this means, that the underlying
 *        protocol will stop forwarding packets corresponding to a specific channel.
 *
 *        For connection-oriented protocols this means, that the underlying
 *        protocol will disconnect all connections made for the specific channel.
 *
 *        A channel can be left as many times as it was joined. When it is left more
 *        often, an error is triggered.
 *
 * \param interfaceRef Handle to the interface to configure.
 * \param channelNumber The channel to leave.
 *
 * \return Returns one of the following status codes:
 *         - XME_CORE_STATUS_SUCCESS The device was successfully 
 *           configured to stop listening to the channel.
 *         - XME_CORE_STATUS_INTERNAL_ERROR The configuration request
 *           failed for unspecific reasons.
 *         - XME_CORE_STATUS_NO_SUCH_VALUE Either interfaceRef or
 *           channelNumber are not valid.
 */
xme_core_status_t
xme_core_interfaceManager_leave_channel
(
	xme_core_interface_interfaceId_t interfaceHandle,
	xme_core_dataChannel_t channelNumber
);

/**
 * \brief Tells the underlying protocol that this host is going
 *        to send data over the specific channel.
 *
 *        This does not do anything for connectionless protocols.
 *
 *        For connection-oriented protocols, the underlying protocol
 *        will prepare for a connection to the specified channel (i.e.
 *        open and listen to a port for tcp).
 *
 * \param interfaceRef Handle to the interface to configure.
 * \param channelNumber The channel this host will provide.
 *
 * \return Returns one of the following status codes:
 *         - XME_CORE_STATUS_SUCCESS The device was successfully 
 *           configured to provide the channel.
 *         - XME_CORE_STATUS_OUT_OF_RESOURCES No ressources are
 *           available to manage providing the channel.
 *         - XME_CORE_STATUS_INTERNAL_ERROR The configuration request
 *           failed for unspecific reasons.
 */
xme_core_status_t
xme_core_interfaceManager_provide_channel
(
	xme_core_interface_interfaceId_t interfaceRef,
	xme_core_dataChannel_t channelNumber
);

/**
 * \brief Tells the underlying protocol that this host is not going
 *        to send data over the specified channel anymore.
 *
 *        This does not do anything for connectionless protocols.
 *
 *        For connection-oriented protocols, the underlying
 *        protocol will stop reacting on connection attemps to
 *        a specific channel.
 *
 * \param interfaceRef Handle to the interface to configure.
 * \param channelNumber The channel this host will not provide anymore.
 *
 * \return Returns one of the following status codes:
 *         - XME_CORE_STATUS_SUCCESS The device was successfully 
 *           configured to unprovide the channel.
 *         - XME_CORE_STATUS_INTERNAL_ERROR The configuration request
 *           failed for unspecific reasons.
 */
xme_core_status_t
xme_core_interfaceManager_unprovide_channel
(
	xme_core_interface_interfaceId_t interfaceRef,
	xme_core_dataChannel_t channelNumber
);

/**
 * \brief Adds interface to the interface manager.
 *        This function adds an interface to the manager. The interface has to
 *        be initiated before adding it.
 *
 * \param interfaceDesc The software interface to the physical device.
 *
 * \return Returns the ID of the interface. Returns -1 if adding was not successfull.
 */
xme_core_interface_interfaceId_t
xme_core_interfaceManager_addInterface
(
	xme_hal_net_interfaceDescr_t* interfaceDesc
);

/**
 * \brief Removes an interface from the manager.
 *
 * \param interfaceHandle Handle to the interface to be removed.
 */
void
xme_core_interfaceManager_removeInterface
(
	xme_core_interface_interfaceId_t interfaceHandle
);

/**
 * \brief Gets size of available data.
 *
 * \return Number of bytes in the buffer of the device.
 */
uint16_t
xme_core_interfaceManager_get_available_data_size
(
	xme_core_interface_interfaceId_t interfaceHandle
);

/**
 * \brief Registers callback for status change of an interface.
 *        Only one callback can be registered.
 *
 * \param cb Function to call when the status of an interface changes.
 */
void
xme_core_interfaceManager_registerCallback
(
	xme_core_interfaceManager_callback_t cb
);

/**
 * \brief Clears the callback for status changes of an interface.
 */
void
xme_core_interfaceManager_clearCallback(void);

// TODO. See ticket #722 and #729
xme_core_status_t
xme_core_interfaceManager_add_channel_to_phy_address_mapping 
(
	xme_core_interface_interfaceId_t interfaceRef,
	xme_core_dataChannel_t channel,
	xme_core_interface_address_t *address
);

// TODO. See ticket #722 and #729
xme_core_status_t
xme_core_interfaceManager_remove_channel_to_phy_address_mapping 
(
	xme_core_interface_interfaceId_t interfaceRef,
	xme_core_dataChannel_t channel,
	xme_core_interface_address_t *address
);

#endif // #ifndef XME_CORE_INTERFACE_MANAGER_H

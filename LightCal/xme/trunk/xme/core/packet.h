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
 *         Low-level communication packet format definitions.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_CORE_PACKET_H
#define XME_CORE_PACKET_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/device.h"
#include "xme/core/nodeManager.h"
#include "xme/core/topic.h"
#include "xme/core/nodeManager.h"

#include <stdint.h>

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define XME_CORE_PACKET_HEADER_MAGIC 0xD7
#define XME_CORE_PACKET_HEADER_VERSION 0x01

#define XME_CORE_PACKET_HEADER_TYPE_LOGINREQUEST 0x01
#define XME_CORE_PACKET_HEADER_TYPE_LOGINRESPONSE 0x02
#define XME_CORE_PACKET_HEADER_TYPE_SAMPLE 0x03

#define XME_CORE_PACKET_INIT(packet, type) \
	do { \
		(packet).packetHeader.magic = XME_CORE_PACKET_HEADER_MAGIC; \
		(packet).packetHeader.version = XME_CORE_PACKET_HEADER_VERSION; \
		(packet).packetHeader.packetType = (type); \
	} while (0)

#define XME_CORE_PACKET_VALID(packet) \
	((XME_CORE_PACKET_HEADER_MAGIC == (packet).packetHeader.magic) && (XME_CORE_PACKET_HEADER_VERSION == (packet).packetHeader.version))

#define XME_CORE_PACKET_TYPE(packet) \
	(packet).packetHeader.packetType

#define XME_CORE_PACKET_PAYLOAD(packet) \
	(packet).data

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
#pragma pack(push, 1)
typedef struct
{
	uint8_t magic; ///< Magic byte.
	uint8_t version; ///< Protocol version.
	uint8_t packetType; ///< Packet type.
}
xme_core_packet_header_t;
#pragma pack(pop)

/**
 * \typedef xme_core_login_request_t
 *
 * \brief  Login request packet.
 */
#pragma pack(push, 1)
typedef struct
{
	xme_core_packet_header_t packetHeader; ///< Packet header.
	xme_core_topic_loginRequestData_t data; ///< Login request data.
}
xme_core_packet_loginRequest_t;
#pragma pack(pop)

/**
 * \typedef xme_core_login_response_t
 *
 * \brief  Login response packet.
 */
#pragma pack(push, 1)
typedef struct
{
	xme_core_packet_header_t packetHeader; ///< Packet header.
	xme_core_topic_loginResponseData_t data; /// Login response data.
}
xme_core_packet_loginResponse_t;
#pragma pack(pop)

/**
 * \typedef xme_core_packet_requestContext_t
 *
 * \brief  Data that are always transmitted along with a request packet.
 */
#pragma pack(push, 1)
typedef struct
{
	xme_core_rr_requestHandle_t localRequestHandle; ///< Request handle (only valid at the request issuer side).
	xme_core_rr_requestInstanceHandle_t localRequestInstance; ///< Request instance identifier (only valid at the request issuer side).
	uint32_t instanceUserData; // TODO: See ticket #540
}
xme_core_packet_requestContext_t;
#pragma pack(pop)

/**
 * \typedef xme_core_packet_responseContext_t
 *
 * \brief  Data that are always transmitted along with a response packet.
 */
#pragma pack(push, 1)
typedef struct
{
	xme_core_rr_requestHandle_t localRequestHandle; ///< Request handle (only valid at the request issuer side).
	xme_core_rr_requestInstanceHandle_t localRequestInstance; ///< Request instance identifier (only valid at the request issuer side).
	xme_core_topic_t responseTopic; ///< Response topic. Note: although the request sender specifies which topic he want to rceive as response, we might want to relax that semantic later on and hence this member is still useful.
	xme_core_rr_responseStatus_t responseStatus; ///< Response status code.
	// TODO: Response meta data. See ticket #646
	uint32_t instanceUserData; // TODO: See ticket #540
}
xme_core_packet_responseContext_t;
#pragma pack(pop)

typedef xme_hal_sharedPtr_t xme_core_packet_sample_t;

#pragma pack(push, 1)
typedef struct
{
	xme_core_packet_header_t packetHeader; ///< Common XME packet header.
	// TODO: We should reduce the number of bytes for this field or implement an alternative mechanism for detecting own network packets. See ticket #787.
	xme_core_device_guid_t senderGuid; ///< Globally unique identifier of packet sender.
/**

	Unused up to now

	uint16_t topicID; ///< Topic ID.
	uint16_t publisherIDSender; ///< ID of publisher.
	uint16_t totalSize; ///< Total size of packet.
	uint16_t dataOffset; ///< Offset of fragment within packet.
	uint16_t sampleNumber; ///< Sample number.
*/
}
xme_core_packet_sample_header_t;
#pragma pack(pop)

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

/**
 * \brief Creates and initializes a sample packet.
 *
 * \param size Size of the payload.
 *
 * \return Handle to sample packet.
 */
xme_core_packet_sample_t xme_core_packet_sample_create_payloadSize(uint16_t dataSize);

/**
 * \brief Creates and initializes a sample packet.
 *
 * \param size Size of complete packet.
 *
 * \return Handle to sample packet.
 */
xme_core_packet_sample_t xme_core_packet_sample_create_totalSize(uint16_t totalSize);

/**
 * \brief Retrieve size of payload within sample packet.
 *
 * \param sample Handle to sample packet.
 * 
 * \return Size of payload.
 */
uint16_t xme_core_packet_sample_getPayloadSize(xme_core_packet_sample_t sample);

/**
 * \brief Retrieve total size of sample packet, including headers.
 *
 * \param sample Handle to sample packet.
 * 
 * \return Total size of sample packet.
 */
uint16_t xme_core_packet_sample_getCompleteSize(xme_core_packet_sample_t sample);

/**
 * \brief Extracts header from sample packet.
 *
 * \param sample Handle to sample packet.
 *
 * \return Pointer to header of sample packet.
 */
xme_core_packet_sample_header_t* xme_core_packet_sample_header(xme_core_packet_sample_t sample);

/**
 * \brief Extracts payload from sample packet.
 *
 * \param sample Handle to sample packet.
 *
 * \return Pointer to payload.
 */
void* xme_core_packet_sample_payload(xme_core_packet_sample_t sample);

/**
 * \brief Frees memory of sample packet.
 *
 * \param sample Handle to sample packet.
 */
void xme_core_packet_sample_destroy(xme_core_packet_sample_t sample);

#endif // #ifndef XME_CORE_PACKET_H

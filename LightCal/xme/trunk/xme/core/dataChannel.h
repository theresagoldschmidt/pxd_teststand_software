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
 *         Data channel abstraction.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_CORE_DATACHANNEL_H
#define XME_CORE_DATACHANNEL_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdint.h>

#include "xme/hal/table.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
// TODO: See ticket #722
typedef enum
{
	XME_CORE_DATACHANNEL_INVALID_DATACHANNEL = 0, ///< Invalid data channel.
	XME_CORE_DATACHANNEL_LOCAL_DATACHANNEL_PREFIX = 1 << (sizeof(XME_HAL_TABLE_MAX_ROW_HANDLE) * 8 - 1), ///< Prefix marking locally assigned data channels.
	XME_CORE_DATACHANNEL_MASK = 1 << (sizeof(XME_HAL_TABLE_MAX_ROW_HANDLE) * 8 - 1), ///< Data channel mask supporting local data channels.
	XME_CORE_DATACHANNEL_MAX_DATACHANNEL = XME_HAL_TABLE_MAX_ROW_HANDLE ///< Largest possible data channel.
}
xme_core_dataChannel_t;

// TODO: See ticket #722
typedef struct
{
	bool isReliable; ///< Whether the data channel is reliable (i.e., reception of data is guaranteed or an error is thrown on the sender side).
	//bool useSequenceNumbers;
	//bool useConsumptionTime;
}
xme_core_dataChannel_properties_t;

#endif // #ifndef XME_CORE_DATACHANNEL_H

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
 *         Shared memory abstraction (platform specific part: FreeRTOS
 *         based implementation).
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Stephan Sommer <sommer@fortiss.org>
 *         Hauke Staehle <staehle@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/sharedPtr.h"
#include "xme/hal/mem.h"

#include "xme/core/defines.h"

// From FreeRTOS
#include "FreeRTOS.h"

#include <stdlib.h>

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
// TODO: Inline all these functions on this platform! See ticket #822
xme_core_status_t
xme_hal_sharedPtr_init()
{
	return XME_CORE_STATUS_SUCCESS;
}

void
xme_hal_sharedPtr_fini()
{
}

xme_hal_sharedPtr_bufferRow_t xme_hal_sharedPtr_buffer[XME_CORE_BROKER_PACKET_BUFFER];


xme_hal_sharedPtr_t
xme_hal_sharedPtr_create
(
	uint16_t size
)
{
	xme_hal_sharedPtr_t sharedPointer;

	for (sharedPointer = (xme_hal_sharedPtr_t)0; sharedPointer < XME_CORE_BROKER_PACKET_BUFFER; sharedPointer = (xme_hal_sharedPtr_t)(((uint16_t)sharedPointer)+1))
	{
		if (xme_hal_sharedPtr_buffer[sharedPointer].referenceCount == 0)
		{
			if (NULL == (xme_hal_sharedPtr_buffer[sharedPointer].slot_data = xme_hal_mem_alloc(size)))
			{
				// Out of memory
				break;
			}

			xme_hal_sharedPtr_buffer[sharedPointer].referenceCount = 1;
			xme_hal_sharedPtr_buffer[sharedPointer].size = size;

			return (xme_hal_sharedPtr_t)(((uint16_t)sharedPointer)+1);
		}
	}

	return XME_HAL_SHAREDPTR_INVALID_POINTER;
}

xme_hal_sharedPtr_t
xme_hal_sharedPtr_realloc
(
	xme_hal_sharedPtr_t sharedPointer,
	uint16_t size
)
{
	uint16_t oldSize;
	xme_hal_sharedPtr_t newBuffer;

	if (XME_HAL_SHAREDPTR_INVALID_POINTER == sharedPointer)
	{
		return xme_hal_sharedPtr_create(size);
	}

	XME_ASSERT_RVAL(sharedPointer < XME_CORE_BROKER_PACKET_BUFFER, XME_HAL_SHAREDPTR_INVALID_POINTER);

	oldSize = xme_hal_sharedPtr_buffer[sharedPointer].size;
	newBuffer = xme_hal_sharedPtr_createFromPointer( oldSize > size ? size : oldSize, xme_hal_sharedPtr_getPointer(sharedPointer) );
	xme_hal_sharedPtr_destroy( sharedPointer );

	return newBuffer;
}

xme_hal_sharedPtr_t
xme_hal_sharedPtr_retain
(
	xme_hal_sharedPtr_t sharedPointer
)
{
	if (sharedPointer <= 0)
	{
		return XME_HAL_SHAREDPTR_INVALID_POINTER;
	}
	else
	{
		if (xme_hal_sharedPtr_buffer[sharedPointer-1].referenceCount > 0)
		{
			xme_hal_sharedPtr_buffer[sharedPointer-1].referenceCount++;
			
			return sharedPointer;
		}
		return XME_HAL_SHAREDPTR_INVALID_POINTER;
	}

	return XME_HAL_SHAREDPTR_INVALID_POINTER;
}

void
xme_hal_sharedPtr_destroy
(
	xme_hal_sharedPtr_t sharedPointer
)
{
	if (sharedPointer <= 0)
	{
		return;
	}
	else
	{
		xme_hal_sharedPtr_buffer[sharedPointer-1].referenceCount--;
		if (xme_hal_sharedPtr_buffer[sharedPointer-1].referenceCount == 0)
		{
			xme_hal_mem_free(xme_hal_sharedPtr_buffer[sharedPointer-1].slot_data);
			xme_hal_sharedPtr_buffer[sharedPointer-1].size = 0;
		}
	}
}

xme_hal_sharedPtr_t
xme_hal_sharedPtr_createFromPointer
(
	uint16_t size,
	void *data
)
{
	xme_hal_sharedPtr_t sharedPointer;

	if (XME_HAL_SHAREDPTR_INVALID_POINTER != (sharedPointer = xme_hal_sharedPtr_create(size)))
	{
		memcpy((xme_hal_sharedPtr_buffer[sharedPointer-1].slot_data), data, size);

		return sharedPointer;
	}

	return XME_HAL_SHAREDPTR_INVALID_POINTER;
}

uint16_t
xme_hal_sharedPtr_getSize
(
	xme_hal_sharedPtr_t sharedPointer
)
{
	return xme_hal_sharedPtr_buffer[sharedPointer-1].size;
};


xme_hal_sharedPtr_referenceCount_t
xme_hal_sharedPtr_getReferenceCount
(
	xme_hal_sharedPtr_t sharedPointer
)
{
	if (xme_hal_sharedPtr_buffer[sharedPointer-1].referenceCount > 0)
	{
		return xme_hal_sharedPtr_buffer[sharedPointer-1].referenceCount;
	}
	else
	{
		return XME_HAL_SHAREDPTR_INVALID_POINTER;
	}

	return XME_HAL_SHAREDPTR_INVALID_POINTER;
}

void*
xme_hal_sharedPtr_getPointer
(
	xme_hal_sharedPtr_t sharedPointer
)
{
	return xme_hal_sharedPtr_buffer[sharedPointer-1].slot_data;
}


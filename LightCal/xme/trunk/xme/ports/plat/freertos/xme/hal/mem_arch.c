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
#include "xme/hal/mem.h"

#include "xme/core/defines.h"

// From FreeRTOS
#include "FreeRTOS.h"

#include <stdlib.h>

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
// TODO: Inline all these functions on this platform! See ticket #827

void*
xme_hal_mem_alloc
(
	uint16_t size
)
{
	return pvPortMalloc(size);
}

void*
xme_hal_mem_realloc
(
	void* mem,
	uint16_t size
)
{
	for (;;);
	return NULL;
}

void
xme_hal_mem_free
(
	void* mem
)
{
	return vPortFree(mem);
}

void*
xme_hal_mem_set
(
	void* mem,
	uint8_t value,
	size_t num
)
{
	// This implementation is only efficient on 8-bit controllers!

	uint8_t* m = (uint8_t*)mem;
	uint8_t* l = m + num;

	while (m < l)
	{
		*m++ = value;
	}

	return mem;
}

void*
xme_hal_mem_copy
(
	void* destination,
	const void* source,
	size_t num
)
{
	// This implementation is only efficient on 8-bit controllers!

	uint8_t* m = (uint8_t*)destination;
	uint8_t* s = (uint8_t*)source;
	uint8_t* l = m + num;

	while (m < l)
	{
		*m++ = *s++;
	}

	return destination;	
}

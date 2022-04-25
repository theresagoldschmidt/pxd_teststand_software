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
 *         Memory abstraction (architecture specific part: generic OS based
 *         implementation).
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Stephan Sommer <sommer@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/mem.h"

#include <stdlib.h>
#include <string.h>

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
void*
xme_hal_mem_alloc
(
	uint16_t size
)
{
	return malloc(size);
}

void*
xme_hal_mem_realloc
(
	void* mem,
	uint16_t size
)
{
	return realloc(mem, size);
}

void
xme_hal_mem_free
(
	void* mem
)
{
	free(mem);
}

void*
xme_hal_mem_set
(
	void* mem,
	uint8_t value,
	size_t num
)
{
	return memset(mem, value, num);
}

void*
xme_hal_mem_copy
(
	void* destination,
	const void* source,
	size_t num
)
{
	return memcpy(destination, source, num);
}

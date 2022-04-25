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
 *         Memory abstraction.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Stephan Sommer <sommer@fortiss.org>
 */

#ifndef XME_HAL_MEM_H
#define XME_HAL_MEM_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdint.h>
#include <stddef.h>
/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \brief  Allocates a block of size bytes of memory, returning a pointer to the
 *         beginning of the block.
 *
 *         The content of the newly allocated block of memory is not initialized,
 *         remaining with indeterminate values.
 *
 * \param  size Size of the memory block, in bytes.
 *
 * \return On success, returns a non-NULL pointer to the memory block allocated
 *         by the function. If the function failed to allocate the requested
 *         block of memory, a NULL pointer is returned.
 */
void*
xme_hal_mem_alloc
(
	uint16_t size
);

/**
 * \brief  Changes the size of the given memory block, expanding or reducing
 *         the amount of memory available in the block. The function may move
 *         the memory block to a new location, in which case the new location
 *         is returned. The content of the memory block is preserved up to the
 *         smaller of the current and the new size even if the block is moved.
 *
 * \note   NULL may be passed in mem. In this case, the function behaves
 *         exactly as xme_hal_mem_alloc(). In case NULL is passed, a new
 *         block is allocated and a pointer to it is returned by the function.
 *
 * \param  mem Pointer to a memory block previously allocated with
 *         xme_hal_mem_alloc() to be reallocated.
 * \param  size New size for the memory block, in bytes. If 0 is passed and
 *         mem is non-NULL, the memory block pointed by mem is deallocated
 *         and a NULL pointer is returned.
 *
 * \return A pointer to the reallocated memory block, which may be one of the
 *         following:
 *          - A memory block with the same content as the mdm block passed
 *            in memory, but with the given new size, if mem was non-NULL and
 *            size non-zero.
 *          - A newly allocated memory block in case mem was NULL and size
 *            was non-zero.
 *          - NULL if size was zero.
 */
void*
xme_hal_mem_realloc
(
	void* mem,
	uint16_t size
);

/**
 * \brief  Releases the given block of memory.
 *
 * \param  mem Block of memory to release.
 */
void
xme_hal_mem_free
(
	void* mem
);

/**
 * \brief  Sets the first num bytes of the block of memory pointed by mem
 *         to the specified value (interpreted as an unsigned char).
 *
 * \param  mem Pointer to the block of memory to fill.
 * \param  value Value to be set.
 * \param  num Number of bytes to be set to the value.
 *
 * \return Returns mem.
 */
void*
xme_hal_mem_set
(
	void* mem,
	uint8_t value,
	size_t num
);

/**
 * \brief  Copies the values of num bytes from the location pointed by source
 *         directly to the memory block pointed by destination.
 *
 *         The arrays pointed by both the destination and source parameters
 *         shall be at least num bytes and should not overlap.
 *
 * \param  destination Pointer to the destination array where the content is to
 *         be copied, type-casted to a pointer of type void*.
 * \param  source Pointer to the source of data to be copied, type-casted to
 *         a pointer of type void*.
 * \param  num Number of bytes to copy.
 *
 * \return Returns destination.
 */
void*
xme_hal_mem_copy
(
	void* destination,
	const void* source,
	size_t num
);

/******************************************************************************/
/***   Platform-specific includes                                           ***/
/******************************************************************************/
#include "xme/hal/mem_arch.h"

#endif // #ifndef XME_HAL_MEM_H

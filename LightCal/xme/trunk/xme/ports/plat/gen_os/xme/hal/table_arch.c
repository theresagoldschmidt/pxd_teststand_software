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
 *         Table abstraction (architecture specific part: generic OS based
 *         implementation).
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/table.h"

#include "xme/core/defines.h"
#include "xme/core/log.h"
#include "xme/hal/math.h"
#include "xme/hal/sharedPtr.h"
#include "xme/hal/mem.h"

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
void
xme_hal_table_fini(xme_hal_table_arrayStruct_t* table)
{
	xme_hal_table_rowHandleBaseType_t i;

	XME_ASSERT_NORVAL(NULL != table);
	XME_ASSERT_NORVAL(table->count <= table->maxHandle);

	// Free all items. A NULL argument won't hurt xme_hal_mem_free().
	for (i=0; i<table->maxHandle; i++)
	{
		xme_hal_mem_free(table->array[i]);
	}

	// Free the array itself. Again, a NULL argument won't hurt xme_hal_mem_free().
	xme_hal_mem_free(table->array);

	// Leave the descriptor in a deterministic state.
	// In case of system shutdown, this additional overhead won't hurt,
	// and when the table is destroyed during runtime, it will prevent
	// future accesses to free'd memory.
	XME_HAL_TABLE_INIT((*table));
}

xme_hal_table_rowHandle_t
xme_hal_table_addItem(xme_hal_table_arrayStruct_t* table, uint16_t itemSize)
{
	xme_hal_table_rowHandle_t handle = XME_HAL_TABLE_INVALID_ROW_HANDLE;
	void* item;

	XME_ASSERT_RVAL(NULL != table, XME_HAL_TABLE_INVALID_ROW_HANDLE);
	XME_ASSERT_RVAL(0 != itemSize, XME_HAL_TABLE_INVALID_ROW_HANDLE);
	XME_ASSERT_RVAL(table->count <= table->maxHandle, XME_HAL_TABLE_INVALID_ROW_HANDLE);

	// Allocate memory for the new item
	XME_CHECK
	(
		NULL != (item = xme_hal_mem_alloc(itemSize)),
		XME_HAL_TABLE_INVALID_ROW_HANDLE
	);

	// If the memory is fragmented, we search for the first empty item
	// and return a handle to that item. The memory is fragmented when
	// the number of items in the table is less than the highest valid
	// handle (which is a one-based index into the array of table items).
	if (table->count != table->maxHandle)
	{
		// Search the fragmented array for the next free item.
		// Actually, the "-1" should be added in the bounds check of the
		// for loop, but it only complicates the calculation and doesn't
		// change behavior because the above assertion ensures that the
		// last table item is never unused when we reach this code, so we
		// can safely omit it.
		xme_hal_table_rowHandleBaseType_t i;
		for (i=0; i<table->maxHandle/*-1*/; i++)
		{
			// If the table item is unused...
			if (NULL == table->array[i])
			{
				// ...use its index as base for the handle and return
				table->array[i] = item;
				table->count++;
				return (xme_hal_table_rowHandle_t)(i+1);
			}
		}

		// We should never reach this location, because that would indicate
		// an inconsistent table state
		XME_ASSERT_RVAL(false, XME_HAL_TABLE_INVALID_ROW_HANDLE);

		xme_hal_mem_free(item);
		XME_ASSERT_RVAL(XME_HAL_TABLE_INVALID_ROW_HANDLE != handle, XME_HAL_TABLE_INVALID_ROW_HANDLE);
	}

	// Add a new item to the table array. If the current size is larger than zero
	// and a power of two (e.g., 1, 2, 4, 8, ...), this means the currently available
	// space is full. We then double the size of the container. Otherwise, we skip
	// the reallocation, as there is still some space remaining.
	if (XME_HAL_MATH_IS_POWER_OF_2(table->count, 32) || 0 == table->count)
	{
		void** memory;

		if (NULL != (memory = (void**)xme_hal_mem_realloc(table->array, sizeof(void*)*XME_HAL_MATH_MAX(2*table->count, 1))))
		{
			xme_hal_table_rowHandleBaseType_t i;

			table->array = memory;
			handle = (xme_hal_table_rowHandle_t)(++table->count);
			table->maxHandle = handle;
			table->array[handle-1] = item;
			table->power++;

			// Make sure the remaining array items are zero.
			// Maybe this can be replaced with something as follows,
			// but beware of memory alignment issues:
			// memset(&table->array[handle], 0, sizeof(*table->array)*(2*(table->count-1)-handle));
			for (i=handle; i<2*(table->count-1); i++)
			{
				table->array[i] = NULL;
			}
		}
		else
		{
			// Allocation failed, release the previously allocated memory for the new item.
			// In this case, handle is still zero, so we will return zero.
			xme_hal_mem_free(item);
		}
	}
	else
	{
		// Use the next available item in the already allocated list
		handle = (xme_hal_table_rowHandle_t)(++table->count);
		table->maxHandle = handle;
		table->array[handle-1] = item;
	}

	return handle;
}

xme_core_status_t
xme_hal_table_removeItem(xme_hal_table_arrayStruct_t* table, xme_hal_table_rowHandle_t handle, uint16_t itemSize)
{
	XME_ASSERT(NULL != table);
	XME_ASSERT(0 != itemSize);
	XME_CHECK(0 < table->count, XME_CORE_STATUS_INVALID_HANDLE);
	XME_ASSERT(0 < table->maxHandle);
	XME_ASSERT(0 < table->power);
	XME_ASSERT(table->count <= table->maxHandle);
	XME_CHECK(XME_HAL_TABLE_INVALID_ROW_HANDLE != handle, XME_CORE_STATUS_INVALID_HANDLE);
	XME_CHECK(handle <= table->maxHandle, XME_CORE_STATUS_INVALID_HANDLE);

	// Free the table item
	table->count--;
	xme_hal_mem_free(table->array[handle-1]);

	// Check whether the highest handle value has been removed.
	// If this is the case, we might have to resize the memory
	// occupied by the item array.
	if (table->maxHandle == handle)
	{
		uint32_t newSize;

		// Determine the new highest valid handle value
		if (0 == table->count)
		{
			// If the list is empty, the highest valid handle value is zero
			table->maxHandle = (xme_hal_table_rowHandle_t)0;
		}
		else
		{
			// Otherwise, we have to iterate through the list
			xme_hal_table_rowHandleBaseType_t i;
			for (i=table->maxHandle-2; ; i--)
			{
				if (NULL != table->array[i])
				{
					table->maxHandle = (xme_hal_table_rowHandle_t)(i+1);
					break;
				}
				if (0 == i)
				{
					break;
				}
			}
		}

		// Check whether we can resize the item array. This is the case if the
		// least power of two value that is larger or equal to the current
		// maximum handle value is smaller than the current size of the table.
		newSize = (table->maxHandle > 0) ? xme_hal_math_ceilPowerOfTwo(table->maxHandle) : 0;

		// Since the table->power field contains the power of two plus one, we
		// subtract one from it to obtain the real size. A value of zero for
		// newSize means that the list is about to become empty.
		if (newSize < pow(2.0, table->power-1))
		{
			if (0 == newSize)
			{
				// The list has become empty. Free all associated memory.
				xme_hal_mem_free(table->array);
				table->array = NULL;
				table->power = 0;
			}
			else
			{
				// Shrink the amount of memory occupied by the item array. It is not fatal
				// if realloc() returns zero, although this should never happen here.
				void** memory = (void**)xme_hal_mem_realloc(table->array, sizeof(*table->array)*newSize);
				if (memory)
				{
					table->array = memory;
					table->power = (uint8_t)(log((double)newSize) / log(2.0)) +1;
				}
			}

			return XME_CORE_STATUS_SUCCESS;
		}
	}

	// Invalidate the table item
	table->array[handle-1] = NULL;

	return XME_CORE_STATUS_SUCCESS;
}

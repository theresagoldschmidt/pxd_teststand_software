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
 *         Table abstraction (architecture specific part: generic embedded
 *         implementation).
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/table.h"

#include "xme/core/defines.h"
#include "xme/core/log.h"
#include "xme/hal/math.h"

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_hal_table_rowHandle_t
xme_hal_table_addItem(xme_hal_table_arrayStruct_t* table, uint16_t capacity)
{
	xme_hal_table_rowHandle_t handle = XME_HAL_TABLE_INVALID_ROW_HANDLE;

	// Table is allocated statically, so return an error if it's full.
	if (table->count == capacity)
	{
		return handle;
	}

	XME_ASSERT_RVAL(NULL != table, XME_HAL_TABLE_INVALID_ROW_HANDLE);
	XME_ASSERT_RVAL(table->count <= table->maxHandle, XME_HAL_TABLE_INVALID_ROW_HANDLE);
	XME_ASSERT_RVAL(table->maxHandle <= capacity, XME_HAL_TABLE_INVALID_ROW_HANDLE);
//	XME_ASSERT_RVAL(table->bitmap != NULL, XME_HAL_TABLE_INVALID_ROW_HANDLE);

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
			if (XME_HAL_TABLE_ARCH_ROW_AVAILABLE == table->bitmap[i])
			{
				// ...use its index as base for the handle and return
				table->bitmap[i] = XME_HAL_TABLE_ARCH_ROW_OCCUPIED;
				table->count++;
				return (xme_hal_table_rowHandle_t)(i+1);
			}
		}

		// We should never reach this location, because that would indicate
		// an inconsistent table state
		XME_ASSERT_RVAL(XME_HAL_TABLE_INVALID_ROW_HANDLE != handle, XME_HAL_TABLE_INVALID_ROW_HANDLE);
	}
	else
	{
		// Table is contiguously filled: Use the next available item
		// at the back of the table.
		handle = (xme_hal_table_rowHandle_t)(++table->count);
		
		// Table is allocated statically, so respect it's capacity.
		if (table->maxHandle < capacity)
		{
			table->maxHandle = handle;
		}

		// Mark item as occupied
		table->bitmap[handle-1] = XME_HAL_TABLE_ARCH_ROW_OCCUPIED;
	}

	return handle;
}

xme_core_status_t
xme_hal_table_removeItem(xme_hal_table_arrayStruct_t* table, xme_hal_table_rowHandle_t handle, uint16_t capacity)
{
	XME_ASSERT(NULL != table);
	XME_ASSERT(0 < table->count);
	XME_ASSERT(0 < table->maxHandle);
	XME_ASSERT(table->count <= table->maxHandle);
	XME_ASSERT_RVAL(table->count <= table->maxHandle, XME_HAL_TABLE_INVALID_ROW_HANDLE);
	XME_ASSERT_RVAL(table->maxHandle <= capacity, XME_HAL_TABLE_INVALID_ROW_HANDLE);
	//XME_ASSERT_RVAL(table->bitmap != NULL, XME_HAL_TABLE_INVALID_ROW_HANDLE);
	XME_CHECK(XME_HAL_TABLE_INVALID_ROW_HANDLE != handle, XME_CORE_STATUS_INVALID_HANDLE);
	XME_CHECK(handle <= table->maxHandle, XME_CORE_STATUS_INVALID_HANDLE);

	// Free the table item
	table->bitmap[handle-1] = XME_HAL_TABLE_ARCH_ROW_AVAILABLE;
	--table->count;

	// Check whether the highest handle value has been removed.
	// If this is the case, we can update maxHandle to reflect
	// the status of contiguously used space.
	//
	// "Holes" in the table will be filled by subsequent calls
	// of xme_hal_table_removeItem() in O(n).
	//
	// We do not try to detect the fact that the table has
	// become fully contiguous by multiple calls of xme_hal_table_removeItem()
	// in order to keep this function O(1).
	if (table->maxHandle == handle)
	{
		--table->maxHandle;
	}

	return XME_CORE_STATUS_SUCCESS;
}

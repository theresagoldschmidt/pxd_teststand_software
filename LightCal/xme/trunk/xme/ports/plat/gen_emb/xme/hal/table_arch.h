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

#ifndef XME_HAL_TABLE_ARCH_H
#define XME_HAL_TABLE_ARCH_H

#ifndef XME_HAL_TABLE_H
	#error This architecture-specific header file should not be included directly. Include the generic header file (usually without "_arch" suffix) instead.
#endif // #ifndef XME_HAL_TABLE_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/core.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/*
 * \typedef xme_hal_table_rowHandleBaseType_t
 *
 * \brief  Numeric base type used for table handle arithmetic.
 *
 *         It must be asserted that this type is able to express at least the
 *         values XME_HAL_TABLE_INVALID_ROW_HANDLE and XME_HAL_TABLE_MAX_ROW_HANDLE.
 */
typedef uint16_t xme_hal_table_rowHandleBaseType_t;
typedef bool xme_hal_table_bitmap_t;

#define XME_HAL_TABLE_ARCH_ROW_OCCUPIED		1
#define XME_HAL_TABLE_ARCH_ROW_AVAILABLE	0

#define XME_HAL_TABLE_STATIC_ALLOC

/*
 * \typedef xme_hal_table_arrayStruct_t
 *
 * \brief  Table structure.
 *
 *         The table structure is the internal representation of a table.
 *
 * \note   Note that the definition of xme_hal_table_arrayStruct_t must match
 *         the definition of the XME_HAL_TABLE() macro (except for the type of
 *         the array element).
 */
typedef struct
{
	uint16_t count;                      ///< Number of items in the table.
	xme_hal_table_rowHandle_t maxHandle; ///< Highest valid handle value of an item in the table.
	xme_hal_table_bitmap_t bitmap[];     ///< Bitmap indicating which fields in the table are currently
                                         ///< occupied or available.
}
xme_hal_table_arrayStruct_t;

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
/**
 * \def XME_HAL_TABLE
 *
 * \brief  Defines an table with a given maximum size.
 *
 * \note   Note that the above definition of this macro must match the
 *         definition of the xme_hal_table_arrayStruct_t type (except for the
 *         type of the array element).
 *
 * \param  itemType Type of the items in the table.
 * \param  name Name of the table.
 * \param  maxElements Maximum number of elements in the table.
 *         On platforms without dynamic memory allocation, this parameter
 *         defines the maximum number of items in the table. The memory
 *         required to hold these items will be statically allocated.
 */
#define XME_HAL_TABLE(itemType, name, maxElements) \
	struct \
	{ \
		uint16_t count; \
		xme_hal_table_rowHandle_t maxHandle; \
		xme_hal_table_bitmap_t bitmap[maxElements]; \
		itemType array[maxElements]; \
	} name

/**
 * \def XME_HAL_TABLE_INIT
 *
 * \brief  Initializes the given table.
 *         This macro must be called for a table before it can be used.
 *
 * \param  name Name of the table to initialize.
 */
#define XME_HAL_TABLE_INIT(name) \
	do { \
		(name).count = 0; \
		(name).maxHandle = XME_HAL_TABLE_INVALID_ROW_HANDLE; \
		memset((name).bitmap, XME_HAL_TABLE_ARCH_ROW_AVAILABLE, sizeof((name).bitmap)); \
	} while (0)

/**
 * \def XME_HAL_TABLE_FINI
 *
 * \brief  Destroys the given table and frees all associated memory.
 *
 * \param  name Name of the table to finalize.
 */
#define XME_HAL_TABLE_FINI(name) \
	do { \
		(name).count = 0; \
		(name).maxHandle = XME_HAL_TABLE_INVALID_ROW_HANDLE; \
	} while (0)

/**
 * \def XME_HAL_TABLE_ADD_ITEM
 *
 * \brief  Adds a new item to the given table, returning the handle to that
 *         item. On systems with dynamic memory allocation, this function
 *         will reserve the memory required to hold the new item and resize
 *         the table to fit the new item. On systems with static memory
 *         allocation, it will use the next free slot and return a handle
 *         to that slot.
 *
 * \param  name Name of the table to add an item to.
 *
 * \return Returns the table row handle for the newly created item.
 */
#define XME_HAL_TABLE_ADD_ITEM(name) \
	xme_hal_table_addItem((xme_hal_table_arrayStruct_t*)&(name), sizeof((name).bitmap)/sizeof(xme_hal_table_bitmap_t))

/**
 * \def XME_HAL_TABLE_REMOVE_ITEM
 *
 * \brief  Removes the item corresponding to the given handle from the table.
 *
 * \param  name Name of the table.
 * \param  handle Table row handle of the item to remove.
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the item has been successfully removed.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the given handle was invalid.
 */
#define XME_HAL_TABLE_REMOVE_ITEM(name, handle) \
	xme_hal_table_removeItem((xme_hal_table_arrayStruct_t*)&(name), handle, sizeof((name).bitmap)/sizeof(xme_hal_table_bitmap_t))

/**
 * \def XME_HAL_TABLE_ITEM_COUNT
 *
 * \brief  Returns the number of items in the given table.
 *
 * \param  name Name of the table.
 *
 * \return Number of items in the given table.
 */
#define XME_HAL_TABLE_ITEM_COUNT(name) \
	((name).count)

/**
 * \def XME_HAL_TABLE_ITEM_FROM_HANDLE
 *
 * \brief  Returns a pointer to the table item for the given table row handle
 *         or NULL if no such item exists.
 *
 * \param  name Name of the table.
 * \param  handle Table row handle of the item to retrieve.
 *
 * \return Returns a pointer to the item corresponding to the given table row
 *         handle or NULL if no such item exists.
 */
#define XME_HAL_TABLE_ITEM_FROM_HANDLE(name, handle) \
	((handle > 0 && handle <= (name).maxHandle && (name).bitmap[handle-1] == XME_HAL_TABLE_ARCH_ROW_OCCUPIED) ? &(name).array[handle-1] : NULL)

/**
 * \def    XME_HAL_TABLE_ITERATE_BEGIN
 *
 * \brief  Begins a block for iterating over all items of the given table.
 *
 * \note   A block started with XME_HAL_TABLE_ITERATE_BEGIN() must be ended
 *         with XME_HAL_TABLE_ITERATE_END() at the same scope.
 *
 * \param  name Name of the table to iterate over.
 * \param  loopHandleType Type of the loop handle variable, usually
 *         xme_hal_table_rowHandleBaseType_t or a type defined to it.
 * \param  loopHandle Name of the loop handle variable. This variable can be
 *         used from within the iterator body.
 * \param  loopItemType Base type of the items that are iterated over.
 *         This parameter should be the type of the table items, not a pointer
 *         to them.
 * \param  loopItem Name of the loop item variable. This variable can be used
 *         from within the iterator body.
 */
#define XME_HAL_TABLE_ITERATE_BEGIN(name, loopHandleType, loopHandle, loopItemType, loopItem) \
	do \
	{ \
		loopHandleType loopHandle; \
		for (loopHandle=(loopHandleType)1; loopHandle<=(loopHandleType)(name).maxHandle; loopHandle = (loopHandleType)(((xme_hal_table_rowHandleBaseType_t)loopHandle)+1)) \
		{ \
			if (XME_HAL_TABLE_ARCH_ROW_OCCUPIED == (name).bitmap[loopHandle-1]) \
			{ \
				loopItemType* loopItem = &(name).array[loopHandle-1]; \
				{ \

/**
 * \def    XME_HAL_TABLE_ITERATE_END
 *
 * \brief  Ends a block for iterating over all items of the given table.
 *
 * \note   A block ended with XME_HAL_TABLE_ITERATE_END() must be started
 *         with XME_HAL_TABLE_ITERATE_BEGIN() at the same scope.
 */
#define XME_HAL_TABLE_ITERATE_END() \
				} \
			} \
		} \
	} while (0)

/**
 * \def    XME_HAL_TABLE_ITERATE_REVERSE_BEGIN
 *
 * \brief  Begins a block for iterating over all items of the given table in
 *         reverse order.
 *
 * \note   A block started with XME_HAL_TABLE_ITERATE_REVERSE_BEGIN() must be
 *         ended with XME_HAL_TABLE_ITERATE_REVERSE_END() at the same scope.
 *
 * \param  name Name of the table to iterate over.
 * \param  loopHandleType Type of the loop handle variable, usually
 *         xme_hal_table_rowHandleBaseType_t or a type defined to it.
 * \param  loopHandle Name of the loop handle variable. This variable can be
 *         used from within the iterator body.
 * \param  loopItemType Base type of the items that are iterated over.
 *         This parameter should be the type of the table items, not a pointer
 *         to them.
 * \param  loopItem Name of the loop item variable. This variable can be used
 *         from within the iterator body.
 */
#define XME_HAL_TABLE_ITERATE_REVERSE_BEGIN(name, loopHandleType, loopHandle, loopItemType, loopItem) \
	do \
	{ \
		loopHandleType loopHandle; \
		for (loopHandle=(loopHandleType)(name).maxHandle; loopHandle>=(loopHandleType)1; loopHandle = (loopHandleType)(((xme_hal_table_rowHandleBaseType_t)loopHandle)-1)) \
		{ \
			if (XME_HAL_TABLE_ARCH_ROW_OCCUPIED == (name).bitmap[loopHandle-1]) \
			{ \
				loopItemType* loopItem = &(name).array[loopHandle-1]; \
				{ \

/**
 * \def    XME_HAL_TABLE_ITERATE_REVERSE_END
 *
 * \brief  Ends a block for iterating over all items of the given table in
 *         reverse order.
 *
 * \note   A block ended with XME_HAL_TABLE_ITERATE_REVERSE_END() must be
 *         started with XME_HAL_TABLE_ITERATE_REVERSE_BEGIN() at the same
 *         scope.
 */
#define XME_HAL_TABLE_ITERATE_REVERSE_END() \
				} \
			} \
		} \
	} while (0)

/**
 * \def    XME_HAL_TABLE_GET_NEXT
 *
 * \brief  Search for the next element of the given table matching the given
 *         condition, starting at the element specified by loopHandle.
 *
 * \param  name Name of the table to iterate over.
 * \param  loopHandleType Type of the loop handle variable, usually
 *         xme_hal_table_rowHandleBaseType_t or a type defined to it.
 * \param  loopHandle Name of the loop handle variable. This variable has to
 *         be defined and initialized by the user and can be used from within
 *         the condition. The value of this variable determines at which
 *         element the scan over the table starts. To start at the beginning
 *         of the table, set the variable to XME_HAL_TABLE_INVALID_ROW_HANDLE.
 * \param  loopItemType Base type of the items that are iterated over.
 *         This parameter should be the type of the table items, not a pointer
 *         to them.
 * \param  loopItem Name of the loop item variable. This variable can be used
 *         from within the condition.
 * \param  condition Condition that the element shall satisfy. Must return a
 *         Boolean value. The variables specified in the loopHandle and
 *         loopItem parameters are available from within the condition.
 */
#define XME_HAL_TABLE_GET_NEXT(name, loopHandleType, loopHandle, loopItemType, loopItem, condition) \
	do \
	{ \
		while (loopHandle <= (loopHandleType)(name).maxHandle) \
		{ \
			loopHandle = (loopHandleType)(((xme_hal_table_rowHandleBaseType_t)loopHandle) + 1); \
			if (loopHandle <= (loopHandleType)(name).maxHandle && XME_HAL_TABLE_ARCH_ROW_OCCUPIED == (name).bitmap[loopHandle-1]) \
			{ \
				loopItem = &(name).array[loopHandle-1]; \
				if (condition) \
				{ \
					break; \
				} \
			} \
		} \
		if (XME_HAL_TABLE_INVALID_ROW_HANDLE == loopHandle || loopHandle > (loopHandleType)(name).maxHandle) \
		{ \
			loopHandle = (loopHandleType)XME_HAL_TABLE_INVALID_ROW_HANDLE; \
			loopItem = NULL; \
		} \
	} while (0)

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
//xme_hal_table_rowHandle_t
//xme_hal_table_addItem(xme_hal_table_arrayStruct_t* table, uint16_t capacity);

xme_core_status_t
xme_hal_table_removeItem(xme_hal_table_arrayStruct_t* table, xme_hal_table_rowHandle_t handle, uint16_t capacity);

#endif // #ifndef XME_HAL_TABLE_ARCH_H

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
 *         Table abstraction.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_HAL_TABLE_H
#define XME_HAL_TABLE_H

/**
 * \defgroup hal_table Table
 *
 * \brief  Table abstraction.
 *
 *         A table is a data structure that allows to store collections of
 *         items with equal structure. An item is uniquely identified by its
 *         index (i.e., row) in the table. The structure of a row (i.e., the
 *         columns of the table) are defined by a C struct.
 *         Functionality is provided for adding new items, retrieving the
 *         address of the struct for a given index, removing items as well
 *         as iterating over the items.
 *
 * \note   Tables are implementated differently depending on the architecture.
 *         This is why tables are covered by the hardware abstraction library.
 *         On some target platforms, table items are dynamically allocated and
 *         the number of items is only limited by the available system memory.
 *         On resource constrained platforms without efficient memory
 *         management, a static maximum size (in terms of number of items) is
 *         given for each table.
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/*
 * \typedef xme_hal_table_rowHandle_t
 *
 * \brief  Table row handle.
 *
 *         A table row handle is the index of a row in a table.
 *         Valid handles are non-zero and smaller or equal to
 *         XME_HAL_TABLE_MAX_ROW_HANDLE.
 */
typedef enum
{
	XME_HAL_TABLE_INVALID_ROW_HANDLE = 0, ///< Invalid table row handle.
	XME_HAL_TABLE_MAX_ROW_HANDLE = 0xFFFF ///< Largest possible table row handle.
}
xme_hal_table_rowHandle_t;

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
/**
 * \def    XME_HAL_TABLE_ITERATE
 *
 * \brief  Iterates over all items of the given table.
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
 * \param  body Iterator code body. The variables specified in the loopHandle
 *         and loopItem parameters are available from within the body.
 */
#define XME_HAL_TABLE_ITERATE(name, loopHandleType, loopHandle, loopItemType, loopItem, body) \
	do \
	{ \
		XME_HAL_TABLE_ITERATE_BEGIN(name, loopHandleType, loopHandle, loopItemType, loopItem); \
		body; \
		XME_HAL_TABLE_ITERATE_END(); \
	} while (0)

/**
 * \def    XME_HAL_TABLE_ITERATE_REVERSE
 *
 * \brief  Iterates over all items of the given table in reverse order.
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
 * \param  body Iterator code body. The variables specified in the loopHandle
 *         and loopItem parameters are available from within the body.
 */
#define XME_HAL_TABLE_ITERATE_REVERSE(name, loopHandleType, loopHandle, loopItemType, loopItem, body) \
	do \
	{ \
		XME_HAL_TABLE_ITERATE_REVERSE_BEGIN(name, loopHandleType, loopHandle, loopItemType, loopItem); \
		body; \
		XME_HAL_TABLE_ITERATE_REVERSE_END(); \
	} while (0)

/******************************************************************************/
/***   Platform-specific includes                                           ***/
/******************************************************************************/
#include "xme/hal/table_arch.h"

#endif // #ifndef XME_HAL_TABLE_H

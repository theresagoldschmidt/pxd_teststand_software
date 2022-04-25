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
 *         Linked list abstraction (architecture specific part: generic OS
 *         based implementation).
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_HAL_LINKEDLIST_ARCH_H
#define XME_HAL_LINKEDLIST_ARCH_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/core.h"

#include <stddef.h> // Make sure offsetof() is available for use in XME_HAL_*LINKEDLIST_ADD_ITEM_SORTED()
#include <stdint.h>

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/*
 * \typedef xme_hal_linkedList_descriptor_t
 *
 * \brief  Singly linked list descriptor.
 *
 *         Defines the internal representation of a singly linked list.
 */
typedef struct
{
	void* head;     ///< Link to the first element in the list or NULL if the list is empty.
	void* end;      ///< Link to the last element in the list or NULL if the list is empty.
	uint16_t count; ///< Number of elements in the list (for reasons of efficiency).
}
xme_hal_linkedList_descriptor_t;

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
/**
 * \def XME_HAL_SINGLYLINKEDLIST
 *
 * \brief  Defines a singly linked list with a given maximum size.
 *
 * \note   Note that the above definition of this macro must match the
 *         definition of the xme_hal_linkedList_descriptor_t type (except for
 *         the type of the head element).
 *
 * \param  itemType Type of the items in the linked list.
 * \param  name Name of the linked list.
 * \param  maxItems Maximum number of items in the linked list.
 *         On platforms without dynamic memory allocation, this parameter
 *         defines the maximum number of items in the linked list. The
 *         memory required to hold these items will then be statically
 *         allocated.
 */
#define XME_HAL_SINGLYLINKEDLIST(itemType, name, maxItems) \
	struct \
	{ \
		struct xme_hal_singlyLinkedList_element_##itemType \
		{ \
			struct xme_hal_singlyLinkedList_element_##itemType* next; \
			itemType* item; \
		}* head; \
		struct xme_hal_singlyLinkedList_element_##itemType* end; \
		uint16_t count; \
	} name

/**
 * \def XME_HAL_LINKEDLIST_DOUBLE
 *
 * \brief  Defines a doubly linked list with a given maximum size.
 *
 * \note   Note that the above definition of this macro must match the
 *         definition of the xme_hal_linkedList_descriptor_t type (except for
 *         the type of the head element).
 *
 * \param  itemType Type of the items in the linked list.
 * \param  name Name of the linked list.
 * \param  maxItems Maximum number of items in the linked list.
 *         On platforms without dynamic memory allocation, this parameter
 *         defines the maximum number of items in the linked list. The
 *         memory required to hold these items will then be statically
 *         allocated.
 */
#define XME_HAL_DOUBLYLINKEDLIST(itemType, name, maxItems) \
	struct \
	{ \
		struct xme_hal_doublyLinkedList_element_##itemType \
		{ \
			struct xme_hal_doublyLinkedList_element_##itemType* next; \
			itemType* item; \
			struct xme_hal_doublyLinkedList_element_##itemType* prev; /* This element must be put after item to allow common functions for both types of linked lists */ \
		}* head; \
		struct xme_hal_doublyLinkedList_element_##itemType* end; \
		uint16_t count; \
	} name

/**
 * \def XME_HAL_LINKEDLIST_INIT
 *
 * \brief  Initializes the given linked list.
 *         This macro must be called for a linked list before it can be used.
 *
 * \param  name Name of the linked list to initialize.
 */
#define XME_HAL_LINKEDLIST_INIT(name) \
	do { (name).head = (name).end = 0; (name).count = 0; } while (0)

/**
 * \def XME_HAL_LINKEDLIST_FINI
 *
 * \brief  Destroys the given linked list and frees all associated memory.
 *
 * \param  name Name of the linked list to finalize.
 */
#define XME_HAL_LINKEDLIST_FINI(name) \
	xme_hal_linkedList_fini((xme_hal_linkedList_descriptor_t*)&(name))

/**
 * \def XME_HAL_SINGLYLINKEDLIST_ADD_ITEM
 *
 * \brief  Adds the given item to the given singly linked list.
 *
 *         The function does not allocate the memory required to store the
 *         new item.
 *
 *         On platforms with dynamic memory allocation, this function will
 *         reserve memory for maintaining the new item within the linked list.
 *         On platforms with static memory allocation, it will use one of the
 *         free slots, if available.
 *
 * \param  name Name of the singly linked list to add the item to.
 * \param  item Item to add to the singly linked list.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the item has been added successfully.
 *          - XME_CORE_STATUS_OUT_OF_RESOURCES if memory allocation failed (on
 *            platforms with dynamic memory mamanagement) or the linked list
 *            has reached its maximum capacity (on platforms without dynamic
 *            memory management).
 */
#define XME_HAL_SINGLYLINKEDLIST_ADD_ITEM(name, item) \
	xme_hal_linkedList_addItem((xme_hal_linkedList_descriptor_t*)&(name), item, false, 0, 0)

/**
 * \def XME_HAL_SINGLYLINKEDLIST_ADD_ITEM_SORTED
 *
 * \brief  Adds the given item to the given singly linked list, respecting
 *         sort order.
 *
 *         Sort order is determined by the value of type compareType at
 *         the given offset within the structure. Items are always sorted
 *         starting from the smallest value at the head of the list.
 *
 *         The function does not allocate the memory required to store the
 *         new item.
 *
 *         On platforms with dynamic memory allocation, this function will
 *         reserve memory for maintaining the new item within the linked list.
 *         On platforms with static memory allocation, it will use one of the
 *         free slots, if available.
 *
 * \param  name Name of the singly linked list to add the item to.
 * \param  item Item to add to the singly linked list.
 * \param  offset Offset of the compare value within the item data structure.
 * \param  compareType Type of the attribute at the given offset within the
 *         item data structure.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the item has been added successfully.
 *          - XME_CORE_STATUS_OUT_OF_RESOURCES if memory allocation failed (on
 *            platforms with dynamic memory mamanagement) or the linked list
 *            has reached its maximum capacity (on platforms without dynamic
 *            memory management).
 */
#define XME_HAL_SINGLYLINKEDLIST_ADD_ITEM_SORTED(name, item, offset, compareType) \
	xme_hal_linkedList_addItem((xme_hal_linkedList_descriptor_t*)&(name), item, false, offset, sizeof(compareType))

/**
 * \def XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM
 *
 * \brief  Adds the given item to the given doubly linked list.
 *
 *         The function does not allocate the memory required to store the
 *         new item.
 *
 *         On platforms with dynamic memory allocation, this function will
 *         reserve memory for maintaining the new item within the linked list.
 *         On platforms with static memory allocation, it will use one of the
 *         free slots, if available.
 *
 * \param  name Name of the doubly linked list to add the item to.
 * \param  item Item to add to the doubly linked list.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the item has been added successfully.
 *          - XME_CORE_STATUS_OUT_OF_RESOURCES if memory allocation failed (on
 *            platforms with dynamic memory mamanagement) or the linked list
 *            has reached its maximum capacity (on platforms without dynamic
 *            memory management).
 */
#define XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM(name, item) \
	xme_hal_linkedList_addItem((xme_hal_linkedList_descriptor_t*)&(name), item, true, 0, 0)

/**
 * \def XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM_SORTED
 *
 * \brief  Adds the given item to the given doubly linked list, respecting
 *         sort order.
 *
 *         Sort order is determined by the value of type compareType at
 *         the given offset within the structure. Items are always sorted
 *         starting from the smallest value at the head of the list.
 *
 *         The function does not allocate the memory required to store the
 *         new item.
 *
 *         On platforms with dynamic memory allocation, this function will
 *         reserve memory for maintaining the new item within the linked list.
 *         On platforms with static memory allocation, it will use one of the
 *         free slots, if available.
 *
 * \param  name Name of the doubly linked list to add the item to.
 * \param  item Item to add to the doubly linked list.
 * \param  offset Offset of the compare value within the item data structure.
 * \param  compareType Type of the attribute at the given offset within the
 *         item data structure.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the item has been added successfully.
 *          - XME_CORE_STATUS_OUT_OF_RESOURCES if memory allocation failed (on
 *            platforms with dynamic memory mamanagement) or the linked list
 *            has reached its maximum capacity (on platforms without dynamic
 *            memory management).
 */
#define XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM_SORTED(name, item, offset, compareType) \
	xme_hal_linkedList_addItem((xme_hal_linkedList_descriptor_t*)&(name), item, true, offset, sizeof(compareType))

/**
 * \def XME_HAL_SINGLYLINKEDLIST_REMOVE_ITEM
 *
 * \brief  Removes the given item from the singly linked list.
 *
 *         The function does not free the memory associated with the item.
 *
 * \param  name Name of the singly linked list.
 * \param  item Address of the item to remove.
 * \param  all Flag indicating whether all matching items should be removed.
 *         If set to false, only the first one will be removed.
 * \return Returns the number of items removed.
 */
#define XME_HAL_SINGLYLINKEDLIST_REMOVE_ITEM(name, item, all) \
	xme_hal_singlyLinkedList_removeItem((xme_hal_linkedList_descriptor_t*)&(name), item, all)

/**
 * \def XME_HAL_DOUBLYLINKEDLIST_REMOVE_ITEM
 *
 * \brief  Removes the given item from the doubly linked list.
 *
 *         The function does not free the memory associated with the item.
 *
 * \param  name Name of the doubly linked list.
 * \param  item Address of the item to remove.
 * \param  all Flag indicating whether all matching items should be removed.
 *         If set to false, only the first one will be removed.
 * \return Returns the number of items removed.
 */
#define XME_HAL_DOUBLYLINKEDLIST_REMOVE_ITEM(name, item, all) \
	xme_hal_doublyLinkedList_removeItem((xme_hal_linkedList_descriptor_t*)&(name), item, all)

/**
 * \def XME_HAL_LINKEDLIST_ITEM_COUNT
 *
 * \brief  Returns the number of items in the given linked list.
 *
 * \param  name Name of the linked list.
 *
 * \return Number of items in the given linked list.
 */
#define XME_HAL_LINKEDLIST_ITEM_COUNT(name) \
	((name).count)

/**
 * \def XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX
 *
 * \brief  Returns a pointer to the item at the given index within the given
 *         singly linked list or NULL if no such item exists.
 *
 * \param  name Name of the singly linked list.
 * \param  index Zero-based index of the item within the singly linked list to
 *         retrieve.
 *
 * \return Returns a pointer to the item corresponding to the given index or
 *         NULL if no such item exists.
 */
#define XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(name, index) \
	xme_hal_linkedList_itemFromIndex((xme_hal_linkedList_descriptor_t*)&(name), index, false)

/**
 * \def XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX
 *
 * \brief  Returns a pointer to the item at the given index within the given
 *         doubly linked list or NULL if no such item exists.
 *
 * \param  name Name of the singly doubly list.
 * \param  index Zero-based index of the item within the doubly linked list to
 *         retrieve.
 *
 * \return Returns a pointer to the item corresponding to the given index or
 *         NULL if no such item exists.
 */
#define XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX(name, index) \
	xme_hal_linkedList_itemFromIndex((xme_hal_linkedList_descriptor_t*)&(name), index, true)

/**
 * \def    XME_HAL_LINKEDLIST_ITERATE
 *
 * \brief  Iterates over all items of the given singly or doubly linked list.
 *
 * \param  name Name of the singly or doubly linked list to iterate over.
 * \param  loopItemType Base type of the items that are iterated over.
 *         This parameter should be the type of the linked list items, not a
 *         pointer to them.
 * \param  loopItem Name of the loop item variable. This variable can be used
 *         from within the iterator body.
 * \param  body Iterator code body. The variable specified in the loopItem
 *         parameter is available from within the body.
 */
#define XME_HAL_LINKEDLIST_ITERATE(name, loopItemType, loopItem, body) \
	do \
	{ \
		struct xme_hal_linkedList_element_##loopItemType \
		{ \
			struct xme_hal_linkedList_element_##loopItemType* next; \
			loopItemType* item; \
			/* Doubly linked lists have one more member here, but this can be ignored here */ \
		}; \
		struct xme_hal_linkedList_element_##loopItemType* loopElement = (struct xme_hal_linkedList_element_##loopItemType*)(name).head; \
		while (loopElement) \
		{ \
			loopItemType* loopItem = loopElement->item; \
			{ \
				body; \
			} \
			loopElement = loopElement->next; \
		} \
	} while (0)

/**
 * \def    XME_HAL_DOUBLYLINKEDLIST_ITERATE_REVERSE
 *
 * \brief  Iterates over all items of the given doubly linked list in reverse order.
 *
 * \param  name Name of the doubly linked list to iterate over.
 * \param  loopItemType Base type of the items that are iterated over.
 *         This parameter should be the type of the linked list items, not a
 *         pointer to them.
 * \param  loopItem Name of the loop item variable. This variable can be used
 *         from within the iterator body.
 * \param  body Iterator code body. The variable specified in the loopItem
 *         parameter is available from within the body.
 */
#define XME_HAL_DOUBLYLINKEDLIST_ITERATE_REVERSE(name, loopItemType, loopItem, body) \
	do \
	{ \
		struct xme_hal_doublylinkedList_element_##loopItemType \
		{ \
			struct xme_hal_doublylinkedList_element_##loopItemType* next; \
			loopItemType* item; \
			struct xme_hal_doublylinkedList_element_##loopItemType* prev; \
		}; \
		struct xme_hal_doublylinkedList_element_##loopItemType* loopElement = (struct xme_hal_doublylinkedList_element_##loopItemType*)(name).end; \
		while (loopElement) \
		{ \
			loopItemType* loopItem = loopElement->item; \
			{ \
				body; \
			} \
			loopElement = loopElement->prev; \
		} \
	} while (0)

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
void
xme_hal_linkedList_fini
(
	xme_hal_linkedList_descriptor_t* linkedList
);

xme_core_status_t
xme_hal_linkedList_addItem
(
	xme_hal_linkedList_descriptor_t* linkedList,
	void* item,
	bool doublyLinked,
	uint32_t offset,
	uint8_t compareSize
);

uint16_t
xme_hal_singlyLinkedList_removeItem
(
	xme_hal_linkedList_descriptor_t* linkedList,
	void* item,
	bool all
);

uint16_t
xme_hal_doublyLinkedList_removeItem
(
	xme_hal_linkedList_descriptor_t* linkedList,
	void* item,
	bool all
);

void*
xme_hal_linkedList_itemFromIndex
(
	xme_hal_linkedList_descriptor_t* linkedList,
	uint16_t index,
	bool doublyLinked
);

#endif // #ifndef XME_HAL_LINKEDLIST_ARCH_H

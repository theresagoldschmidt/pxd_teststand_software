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

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/linkedList.h"

#include "xme/core/defines.h"
#include "xme/hal/mem.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
typedef struct xme_hal_singlyLinkedList_genericElement_t
{
	struct xme_hal_singlyLinkedList_genericElement_t* next;
	void* item;
}
xme_hal_singlyLinkedList_genericElement_t;

typedef struct xme_hal_doublyLinkedList_genericElement_t
{
	struct xme_hal_doublyLinkedList_genericElement_t* next;
	void* item;
	struct xme_hal_doublyLinkedList_genericElement_t* prev;
}
xme_hal_doublyLinkedList_genericElement_t;

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
void
xme_hal_linkedList_fini
(
	xme_hal_linkedList_descriptor_t* linkedList
)
{
	// We don't care whenther the list is singly or doubly linked
	xme_hal_singlyLinkedList_genericElement_t* current;

	XME_ASSERT_NORVAL(NULL != linkedList);

	current = (xme_hal_singlyLinkedList_genericElement_t*)linkedList->head;

	while (current)
	{
		xme_hal_singlyLinkedList_genericElement_t* thisElement = current;

		current = (xme_hal_singlyLinkedList_genericElement_t*)current->next;

		// Free the linked list element
		xme_hal_mem_free(thisElement);
	};

	// Leave the descriptor in a deterministic state.
	// In case of system shutdown, this additional overhead won't hurt,
	// and when the linked list is destroyed during runtime, it will prevent
	// future accesses to free'd memory.
	XME_HAL_LINKEDLIST_INIT((*linkedList));
}

xme_core_status_t
xme_hal_linkedList_addItem
(
	xme_hal_linkedList_descriptor_t* linkedList,
	void* item,
	bool doublyLinked,
	uint32_t offset,
	uint8_t compareSize
)
{
	xme_hal_doublyLinkedList_genericElement_t* element;
	xme_hal_doublyLinkedList_genericElement_t* prev = NULL;
	xme_hal_doublyLinkedList_genericElement_t* position = NULL;

	XME_ASSERT(NULL != linkedList);
	XME_ASSERT(compareSize <= 8);

	// Allocate memory for the new element
	XME_CHECK
	(
		NULL !=
		(
			element = (xme_hal_doublyLinkedList_genericElement_t*)xme_hal_mem_alloc(doublyLinked ? sizeof(xme_hal_doublyLinkedList_genericElement_t) : sizeof(xme_hal_singlyLinkedList_genericElement_t))
		),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	if (0 == compareSize)
	{
		// Insert the new element at the end of the linked list.
		// In case the list is originally empty, position will be NULL.
		position = (xme_hal_doublyLinkedList_genericElement_t*)linkedList->end;
	}
	else
	{
		uint64_t* newValue = (uint64_t*)((uint64_t)item + offset);

		// Determine insertion position
		xme_hal_doublyLinkedList_genericElement_t* loopElement = (xme_hal_doublyLinkedList_genericElement_t*)linkedList->head;
		while (loopElement)
		{
			uint64_t* value = (uint64_t*)((uint64_t)loopElement->item + offset);
			uint64_t mask = (((uint64_t)1) << (compareSize*8))-1;

			if ((*value & mask) > (*newValue & mask))
			{
				// Insert after the previous element
				position = prev;
				break;
			}

			prev = loopElement;
			loopElement = (xme_hal_doublyLinkedList_genericElement_t*)loopElement->next;

			if (NULL == loopElement)
			{
				// Insert at the end
				position = (xme_hal_doublyLinkedList_genericElement_t*)linkedList->end;
			}
		}
	}

	if (NULL == position)
	{
		// Insert as first element
		element->next = (xme_hal_doublyLinkedList_genericElement_t*)linkedList->head;
		if (doublyLinked)
		{
			element->prev = NULL;

			if (linkedList->head) ((xme_hal_doublyLinkedList_genericElement_t*)linkedList->head)->prev = element;
		}
		linkedList->head = element;
		if (NULL == linkedList->end) linkedList->end = element;
	}
	else
	{
		// Insert as next element after position
		element->next = position->next;
		if (doublyLinked)
		{
			element->prev = (xme_hal_doublyLinkedList_genericElement_t*)position;
			if (position->next) ((xme_hal_doublyLinkedList_genericElement_t*)position->next)->prev = element;
		}
		position->next = (xme_hal_doublyLinkedList_genericElement_t*)element;
		if (linkedList->end == position) linkedList->end = element;
	}

	element->item = item;
	linkedList->count++;

	return XME_CORE_STATUS_SUCCESS;
}

uint16_t
xme_hal_singlyLinkedList_removeItem
(
	xme_hal_linkedList_descriptor_t* linkedList,
	void* item,
	bool all
)
{
	xme_hal_singlyLinkedList_genericElement_t* current;
	xme_hal_singlyLinkedList_genericElement_t* prev = NULL;
	uint16_t origCount = linkedList->count;

	XME_ASSERT(NULL != linkedList);

	current = (xme_hal_singlyLinkedList_genericElement_t*)linkedList->head;

	while (current)
	{
		// Check whether we should remove this item
		if (item == current->item)
		{
			void* next;

			if (NULL != prev)
			{
				// An element exists in front of current
				next = prev->next = current->next;
			}
			else
			{
				// Current was the first element
				next = linkedList->head = current->next;
			}

			if (current == linkedList->end) linkedList->end = prev;

			// Free the memory allocated for maintaining the item
			xme_hal_mem_free(current);
			linkedList->count--;

			// Stop if we should only remove the first matching item
			if (!all)
			{
				return 1;
			}

			// Go to the next element
			current = (xme_hal_singlyLinkedList_genericElement_t*)next;
		}
		else
		{
			// Go to the next element
			prev = current;
			current = (xme_hal_singlyLinkedList_genericElement_t*)current->next;
		}
	};

	return origCount - linkedList->count;
}

uint16_t
xme_hal_doublyLinkedList_removeItem
(
	xme_hal_linkedList_descriptor_t* linkedList,
	void* item,
	bool all
)
{
	xme_hal_doublyLinkedList_genericElement_t* current;
	uint16_t origCount = linkedList->count;

	XME_ASSERT(NULL != linkedList);

	current = (xme_hal_doublyLinkedList_genericElement_t*)linkedList->head;

	while (current)
	{
		// Check whether we should remove this item
		if (item == current->item)
		{
			void* next = NULL;

			if (NULL != current->prev)
			{
				((xme_hal_doublyLinkedList_genericElement_t*)current->prev)->next = current->next;
			}
			if (NULL != current->next)
			{
				next = ((xme_hal_doublyLinkedList_genericElement_t*)current->next)->prev = current->prev;
			}
			if (linkedList->head == current)
			{
				linkedList->head = current->next;
			}
			if (linkedList->end == current)
			{
				linkedList->end = current->prev;
			}

			// Free the memory allocated for maintaining the item
			xme_hal_mem_free(current);
			linkedList->count--;

			// Stop if we should only remove the first matching item
			if (!all)
			{
				return 1;
			}

			// Go to the next element
			current = (xme_hal_doublyLinkedList_genericElement_t*)next;
		}
		else
		{
			// Go to the next element
			current = (xme_hal_doublyLinkedList_genericElement_t*)current->next;
		}
	};

	return origCount - linkedList->count;
}

void*
xme_hal_linkedList_itemFromIndex
(
	xme_hal_linkedList_descriptor_t* linkedList,
	uint16_t index,
	bool doublyLinked
)
{
	bool reverse = false;
	xme_hal_doublyLinkedList_genericElement_t* current;

	XME_ASSERT_RVAL(NULL != linkedList, NULL);
	XME_CHECK(index < linkedList->count, NULL);

	// If we want the last element, directly return it
	XME_ASSERT_RVAL(0 != linkedList->count, NULL);
	XME_ASSERT_RVAL(NULL != linkedList->head, NULL);
	XME_ASSERT_RVAL(NULL != linkedList->end, NULL);
	XME_CHECK(index != linkedList->count-1, ((xme_hal_doublyLinkedList_genericElement_t*)linkedList->end)->item);

	// If the list is doubly linked and the item is more towards the end
	// of the list, we browse through the list in reverse direction
	if (doublyLinked && index >= (linkedList->count+1) >> 1)
	{
		reverse = true;
		index = linkedList->count - index - 1;
		current = (xme_hal_doublyLinkedList_genericElement_t*)linkedList->end;
	}
	else
	{
		current = (xme_hal_doublyLinkedList_genericElement_t*)linkedList->head;
	}

	while (index--)
	{
		current = (xme_hal_doublyLinkedList_genericElement_t*)(reverse ? current->prev : current->next);
	}

	return current->item;
}

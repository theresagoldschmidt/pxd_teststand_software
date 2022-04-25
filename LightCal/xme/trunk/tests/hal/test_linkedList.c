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
 *         Linked list testsuite.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Simon Barner <barner@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "tests/hal/test_linkedList.h"

#include "tests/tests.h"
#include "xme/hal/linkedList.h"

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
int
xme_tests_hal_singlyLinkedList_simple()
{
	XME_TESTSUITE_BEGIN();

	{
		// Allocate linked list with 6 elements
		XME_HAL_SINGLYLINKEDLIST(double, myList, 6);

		// Initialize linked list
		XME_HAL_LINKEDLIST_INIT(myList);

		// Retrieval of nonexistant items
		XME_TEST(NULL == XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(myList, 0));
		XME_TEST(NULL == XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(myList, 1));
		XME_TEST(NULL == XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(myList, 0x1234));

		// Removal of nonexistant items
		XME_TEST(0 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(0 == XME_HAL_SINGLYLINKEDLIST_REMOVE_ITEM(myList, NULL, false));
		XME_TEST(0 == XME_HAL_SINGLYLINKEDLIST_REMOVE_ITEM(myList, NULL, true));
		XME_TEST(0 == XME_HAL_SINGLYLINKEDLIST_REMOVE_ITEM(myList, (void*)0x1234, false));
		XME_TEST(0 == XME_HAL_SINGLYLINKEDLIST_REMOVE_ITEM(myList, (void*)0x1234, true));
		XME_TEST(0 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));

		// Insertion of test items
		XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_SINGLYLINKEDLIST_ADD_ITEM(myList, NULL));
		XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_SINGLYLINKEDLIST_ADD_ITEM(myList, (void*)0x1234));
		XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_SINGLYLINKEDLIST_ADD_ITEM(myList, NULL));
		XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_SINGLYLINKEDLIST_ADD_ITEM(myList, NULL));
		XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_SINGLYLINKEDLIST_ADD_ITEM(myList, (void*)0x5678));
		XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_SINGLYLINKEDLIST_ADD_ITEM(myList, NULL));

		// Retrieval of items
		XME_TEST(NULL == XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(myList, 0));
		XME_TEST((void*)0x1234 == XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(myList, 1));
		XME_TEST(NULL == XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(myList, 2));
		XME_TEST(NULL == XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(myList, 3));
		XME_TEST((void*)0x5678 == XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(myList, 4));
		XME_TEST(NULL == XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(myList, 5));

		// Batch remove
		XME_TEST(6 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(1 == XME_HAL_SINGLYLINKEDLIST_REMOVE_ITEM(myList, NULL, false));
		XME_TEST(5 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(1 == XME_HAL_SINGLYLINKEDLIST_REMOVE_ITEM(myList, NULL, false));
		XME_TEST(4 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(2 == XME_HAL_SINGLYLINKEDLIST_REMOVE_ITEM(myList, NULL, true));
		XME_TEST(2 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(0 == XME_HAL_SINGLYLINKEDLIST_REMOVE_ITEM(myList, NULL, false));
		XME_TEST(2 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(0 == XME_HAL_SINGLYLINKEDLIST_REMOVE_ITEM(myList, NULL, true));
		XME_TEST(2 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(1 == XME_HAL_SINGLYLINKEDLIST_REMOVE_ITEM(myList, (void*)0x5678, true));
		XME_TEST(1 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(1 == XME_HAL_SINGLYLINKEDLIST_REMOVE_ITEM(myList, (void*)0x1234, true));
		XME_TEST(0 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));

		// Finalize linked list
		XME_HAL_LINKEDLIST_FINI(myList);
		XME_TEST(0 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	}

	{
		// Allocate linked list with 3 elements
		XME_HAL_SINGLYLINKEDLIST(double, myList, 3);

		double
			d1 = 1.0,
			d2 = 2.0,
			d3 = 3.0,
			sum;

		uint16_t num = 0;

		// Initialize linked list
		XME_HAL_LINKEDLIST_INIT(myList);

		// Insertion of meaningful data
		XME_TEST(0 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_SINGLYLINKEDLIST_ADD_ITEM(myList, &d1));
		XME_TEST(1 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_SINGLYLINKEDLIST_ADD_ITEM(myList, &d2));
		XME_TEST(2 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_SINGLYLINKEDLIST_ADD_ITEM(myList, &d3));
		XME_TEST(3 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	
#ifdef XME_HAL_LINKEDLIST_STATIC_ALLOC

		// Exceeding capacity
		XME_TEST(XME_CORE_STATUS_OUT_OF_RESOURCES == XME_HAL_SINGLYLINKEDLIST_ADD_ITEM(myList, &d2));
		XME_TEST(3 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));

#endif // #ifdef XME_HAL_LINKEDLIST_STATIC_ALLOC

		// Sum up linked list items, after updating the sum, add 5 to items
		sum = 0.0;
		num = 0;
		XME_HAL_LINKEDLIST_ITERATE(myList, double, item, sum += *item; *item = *item + 5.0; num++);
		XME_TEST(6.0 == sum); // 1 + 2 + 3
		XME_TEST(3 == num);

		// Check if updating worked
		sum=0.0;
		XME_HAL_LINKEDLIST_ITERATE(myList, double, item, sum += *item);
		XME_TEST(21.0 == sum); // (1+5) + (2+5) + (3+5)

		// Remove 2nd item
		XME_HAL_SINGLYLINKEDLIST_REMOVE_ITEM(myList, &d2, false);
		XME_TEST(2 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));

		// Check sum again
		sum = 0.0;
		num = 0;
		XME_HAL_LINKEDLIST_ITERATE(myList, double, item, sum += *item; num++);
		XME_TEST(14.0 == sum); // (1+5) + (3+5)
		XME_TEST(2 == num);

		// Finalize linked list
		XME_HAL_LINKEDLIST_FINI(myList);
		XME_TEST(0 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	}

	XME_TESTSUITE_END();
}

int
xme_tests_hal_singlyLinkedList_ordered()
{
	typedef struct
	{
		uint16_t priority;
		uint32_t value;
	}
	data_t;

	// Allocate linked list with 5 elements
	XME_HAL_SINGLYLINKEDLIST(data_t, myList, 5);

	data_t d1 = { 1, 10 };
	data_t d2 = { 2, 15 };
	data_t d3 = { 3,  5 };
	data_t d4 = { 0, 25 };
	data_t d5 = { 2, 15 };

	XME_TESTSUITE_BEGIN();

	// Initialize linked list
	XME_HAL_LINKEDLIST_INIT(myList);

	// Insert items sorted by priority
	XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_SINGLYLINKEDLIST_ADD_ITEM_SORTED(myList, &d1, offsetof(data_t, priority), uint16_t));
	XME_TEST(1 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_SINGLYLINKEDLIST_ADD_ITEM_SORTED(myList, &d2, offsetof(data_t, priority), uint16_t));
	XME_TEST(2 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_SINGLYLINKEDLIST_ADD_ITEM_SORTED(myList, &d3, offsetof(data_t, priority), uint16_t));
	XME_TEST(3 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_SINGLYLINKEDLIST_ADD_ITEM_SORTED(myList, &d4, offsetof(data_t, priority), uint16_t));
	XME_TEST(4 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_SINGLYLINKEDLIST_ADD_ITEM_SORTED(myList, &d5, offsetof(data_t, priority), uint16_t));
	XME_TEST(5 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));

	// Check position of list elements
	XME_TEST(&d4 == XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(myList, 0));
	XME_TEST(&d1 == XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(myList, 1));
	XME_TEST(&d2 == XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(myList, 2));
	XME_TEST(&d5 == XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(myList, 3));
	XME_TEST(&d3 == XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(myList, 4));

	// Clear the list
	XME_HAL_LINKEDLIST_FINI(myList);

	// Insert items sorted by value
	XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_SINGLYLINKEDLIST_ADD_ITEM_SORTED(myList, &d1, offsetof(data_t, value), uint32_t));
	XME_TEST(1 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_SINGLYLINKEDLIST_ADD_ITEM_SORTED(myList, &d2, offsetof(data_t, value), uint32_t));
	XME_TEST(2 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_SINGLYLINKEDLIST_ADD_ITEM_SORTED(myList, &d3, offsetof(data_t, value), uint32_t));
	XME_TEST(3 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_SINGLYLINKEDLIST_ADD_ITEM_SORTED(myList, &d4, offsetof(data_t, value), uint32_t));
	XME_TEST(4 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_SINGLYLINKEDLIST_ADD_ITEM_SORTED(myList, &d5, offsetof(data_t, value), uint32_t));
	XME_TEST(5 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));

	// Check position of list elements
	XME_TEST(&d3 == XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(myList, 0));
	XME_TEST(&d1 == XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(myList, 1));
	XME_TEST(&d2 == XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(myList, 2));
	XME_TEST(&d5 == XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(myList, 3));
	XME_TEST(&d4 == XME_HAL_SINGLYLINKEDLIST_ITEM_FROM_INDEX(myList, 4));

	// Clear the list
	XME_HAL_LINKEDLIST_FINI(myList);

	XME_TESTSUITE_END();
}

int
xme_tests_hal_doublyLinkedList_simple()
{
	XME_TESTSUITE_BEGIN();

	{
		// Allocate linked list with 6 elements
		XME_HAL_DOUBLYLINKEDLIST(double, myList, 6);

		// Initialize linked list
		XME_HAL_LINKEDLIST_INIT(myList);

		// Retrieval of nonexistant items
		XME_TEST(NULL == XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX(myList, 0));
		XME_TEST(NULL == XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX(myList, 1));
		XME_TEST(NULL == XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX(myList, 0x1234));

		// Removal of nonexistant items
		XME_TEST(0 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(0 == XME_HAL_DOUBLYLINKEDLIST_REMOVE_ITEM(myList, NULL, false));
		XME_TEST(0 == XME_HAL_DOUBLYLINKEDLIST_REMOVE_ITEM(myList, NULL, true));
		XME_TEST(0 == XME_HAL_DOUBLYLINKEDLIST_REMOVE_ITEM(myList, (void*)0x1234, false));
		XME_TEST(0 == XME_HAL_DOUBLYLINKEDLIST_REMOVE_ITEM(myList, (void*)0x1234, true));
		XME_TEST(0 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));

		// Insertion of test items
		XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM(myList, NULL));
		XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM(myList, (void*)0x1234));
		XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM(myList, NULL));
		XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM(myList, NULL));
		XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM(myList, (void*)0x5678));
		XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM(myList, NULL));

		// Retrieval of items
		XME_TEST(NULL == XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX(myList, 0));
		XME_TEST((void*)0x1234 == XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX(myList, 1));
		XME_TEST(NULL == XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX(myList, 2));
		XME_TEST(NULL == XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX(myList, 3));
		XME_TEST((void*)0x5678 == XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX(myList, 4));
		XME_TEST(NULL == XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX(myList, 5));

		// Batch remove
		XME_TEST(6 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(1 == XME_HAL_DOUBLYLINKEDLIST_REMOVE_ITEM(myList, NULL, false));
		XME_TEST(5 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(1 == XME_HAL_DOUBLYLINKEDLIST_REMOVE_ITEM(myList, NULL, false));
		XME_TEST(4 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(2 == XME_HAL_DOUBLYLINKEDLIST_REMOVE_ITEM(myList, NULL, true));
		XME_TEST(2 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(0 == XME_HAL_DOUBLYLINKEDLIST_REMOVE_ITEM(myList, NULL, false));
		XME_TEST(2 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(0 == XME_HAL_DOUBLYLINKEDLIST_REMOVE_ITEM(myList, NULL, true));
		XME_TEST(2 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(1 == XME_HAL_DOUBLYLINKEDLIST_REMOVE_ITEM(myList, (void*)0x5678, true));
		XME_TEST(1 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(1 == XME_HAL_DOUBLYLINKEDLIST_REMOVE_ITEM(myList, (void*)0x1234, true));
		XME_TEST(0 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));

		// Finalize linked list
		XME_HAL_LINKEDLIST_FINI(myList);
		XME_TEST(0 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	}

	{
		// Allocate linked list with 3 elements
		XME_HAL_DOUBLYLINKEDLIST(double, myList, 3);

		double
			d1 = 1.0,
			d2 = 2.0,
			d3 = 3.0,
			sum;

		uint16_t num = 0;

		// Initialize linked list
		XME_HAL_LINKEDLIST_INIT(myList);

		// Insertion of meaningful data
		XME_TEST(0 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM(myList, &d1));
		XME_TEST(1 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM(myList, &d2));
		XME_TEST(2 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
		XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM(myList, &d3));
		XME_TEST(3 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	
#ifdef XME_HAL_LINKEDLIST_STATIC_ALLOC

		// Exceeding capacity
		XME_TEST(XME_CORE_STATUS_OUT_OF_RESOURCES == XME_HAL_LINKEDLIST_ADD_ITEM(myList, &d2));
		XME_TEST(3 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));

#endif // #ifdef XME_HAL_LINKEDLIST_STATIC_ALLOC

		// Sum up linked list items, after updating the sum, add 3 to items
		sum = 0.0;
		num = 0;
		XME_HAL_LINKEDLIST_ITERATE(myList, double, item, sum += *item; *item = *item + 3.0; num++);
		XME_TEST(6.0 == sum); // 1 + 2 + 3
		XME_TEST(3 == num);

		// Sum up linked list items in reverse order, after updating the sum, add 2 to items
		sum = 0.0;
		num = 0;
		XME_HAL_DOUBLYLINKEDLIST_ITERATE_REVERSE(myList, double, item, sum += *item; *item = *item + 2.0; num++);
		XME_TEST(15.0 == sum); // (1+3) + (2+3) + (3+3)
		XME_TEST(3 == num);

		// Check if updating worked
		sum=0.0;
		XME_HAL_LINKEDLIST_ITERATE(myList, double, item, sum += *item);
		XME_TEST(21.0 == sum); // (1+5) + (2+5) + (3+5)

		sum=0.0;
		XME_HAL_DOUBLYLINKEDLIST_ITERATE_REVERSE(myList, double, item, sum += *item);
		XME_TEST(21.0 == sum); // (1+5) + (2+5) + (3+5)

		// Remove 2nd item
		XME_HAL_DOUBLYLINKEDLIST_REMOVE_ITEM(myList, &d2, false);
		XME_TEST(2 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));

		// Check sum again
		sum = 0.0;
		num = 0;
		XME_HAL_LINKEDLIST_ITERATE(myList, double, item, sum += *item; num++);
		XME_TEST(14.0 == sum); // (1+5) + (3+5)
		XME_TEST(2 == num);

		sum = 0.0;
		num = 0;
		XME_HAL_DOUBLYLINKEDLIST_ITERATE_REVERSE(myList, double, item, sum += *item; num++);
		XME_TEST(14.0 == sum); // (1+5) + (3+5)
		XME_TEST(2 == num);

		// Finalize linked list
		XME_HAL_LINKEDLIST_FINI(myList);
		XME_TEST(0 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	}

	XME_TESTSUITE_END();
}

int
xme_tests_hal_doublyLinkedList_ordered()
{
	typedef struct
	{
		uint16_t priority;
		uint32_t value;
	}
	data_t;

	// Allocate linked list with 5 elements
	XME_HAL_DOUBLYLINKEDLIST(data_t, myList, 5);

	data_t d1 = { 1, 10 };
	data_t d2 = { 2, 15 };
	data_t d3 = { 3,  5 };
	data_t d4 = { 0, 25 };
	data_t d5 = { 2, 25 };

	XME_TESTSUITE_BEGIN();

	// Initialize linked list
	XME_HAL_LINKEDLIST_INIT(myList);

	// Insert items sorted by priority
	XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM_SORTED(myList, &d1, offsetof(data_t, priority), uint16_t));
	XME_TEST(1 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM_SORTED(myList, &d2, offsetof(data_t, priority), uint16_t));
	XME_TEST(2 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM_SORTED(myList, &d3, offsetof(data_t, priority), uint16_t));
	XME_TEST(3 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM_SORTED(myList, &d4, offsetof(data_t, priority), uint16_t));
	XME_TEST(4 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM_SORTED(myList, &d5, offsetof(data_t, priority), uint16_t));
	XME_TEST(5 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));

	// Check position of list elements
	XME_TEST(&d4 == XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX(myList, 0));
	XME_TEST(&d1 == XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX(myList, 1));
	XME_TEST(&d2 == XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX(myList, 2));
	XME_TEST(&d5 == XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX(myList, 3));
	XME_TEST(&d3 == XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX(myList, 4));

	// Clear the list
	XME_HAL_LINKEDLIST_FINI(myList);

	// Insert items sorted by value
	XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM_SORTED(myList, &d1, offsetof(data_t, value), uint32_t));
	XME_TEST(1 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM_SORTED(myList, &d2, offsetof(data_t, value), uint32_t));
	XME_TEST(2 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM_SORTED(myList, &d3, offsetof(data_t, value), uint32_t));
	XME_TEST(3 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM_SORTED(myList, &d4, offsetof(data_t, value), uint32_t));
	XME_TEST(4 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));
	XME_TEST(XME_CORE_STATUS_SUCCESS == XME_HAL_DOUBLYLINKEDLIST_ADD_ITEM_SORTED(myList, &d5, offsetof(data_t, value), uint32_t));
	XME_TEST(5 == XME_HAL_LINKEDLIST_ITEM_COUNT(myList));

	// Check position of list elements
	XME_TEST(&d3 == XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX(myList, 0));
	XME_TEST(&d1 == XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX(myList, 1));
	XME_TEST(&d2 == XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX(myList, 2));
	XME_TEST(&d4 == XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX(myList, 3));
	XME_TEST(&d5 == XME_HAL_DOUBLYLINKEDLIST_ITEM_FROM_INDEX(myList, 4));

	// Clear the list
 	XME_HAL_LINKEDLIST_FINI(myList);

	XME_TESTSUITE_END();
}

int
xme_tests_hal_linkedList()
{
	XME_TESTSUITE_BEGIN();

	XME_TESTSUITE(xme_tests_hal_singlyLinkedList_simple(), "xme_tests_hal_singlyLinkedList_simple testsuite failed!");
	XME_TESTSUITE(xme_tests_hal_singlyLinkedList_ordered(), "xme_tests_hal_singlyLinkedList_ordered testsuite failed!");
	XME_TESTSUITE(xme_tests_hal_doublyLinkedList_simple(), "xme_tests_hal_doublyLinkedList_simple testsuite failed!");
	XME_TESTSUITE(xme_tests_hal_doublyLinkedList_ordered(), "xme_tests_hal_doublyLinkedList_ordered testsuite failed!");

	XME_TESTSUITE_END();
}

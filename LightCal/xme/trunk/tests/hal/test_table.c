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
 *         Table testsuite.
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "tests/hal/test_table.h"

#include "tests/tests.h"
#include "xme/hal/table.h"

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
int
xme_tests_hal_table()
{
	/* Variable allocation */

	// Allocate table with 3 elements
	XME_HAL_TABLE(double, myTable, 3);
	xme_hal_table_rowHandle_t
		h1,
		h2,
		h3,
		h4,
		h5,
		h6,
		h7;
	
	double
		*d1,
		*d2,
		*d3,
		sum;

	uint16_t num=0;

	/* Begin of tests */
	XME_TESTSUITE_BEGIN();

	// Initialization, allocate all 3 rows:
	// Ensure that table has correct size and
	// the returned handles are unique.
	XME_HAL_TABLE_INIT(myTable);
	XME_TEST(0 == XME_HAL_TABLE_ITEM_COUNT(myTable));

	XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == XME_HAL_TABLE_REMOVE_ITEM(myTable, (xme_hal_table_rowHandle_t)0));
	XME_TEST(XME_CORE_STATUS_INVALID_HANDLE == XME_HAL_TABLE_REMOVE_ITEM(myTable, (xme_hal_table_rowHandle_t)1));

	h1 = XME_HAL_TABLE_ADD_ITEM(myTable);
	XME_TEST(h1 != XME_HAL_TABLE_INVALID_ROW_HANDLE);
	XME_TEST(1 == XME_HAL_TABLE_ITEM_COUNT(myTable));

	h2 = XME_HAL_TABLE_ADD_ITEM(myTable);
	XME_TEST(h2 != XME_HAL_TABLE_INVALID_ROW_HANDLE);
	XME_TEST(h1 != h2);
	XME_TEST(2 == XME_HAL_TABLE_ITEM_COUNT(myTable));

	h3 = XME_HAL_TABLE_ADD_ITEM(myTable);
	XME_TEST(h3 != XME_HAL_TABLE_INVALID_ROW_HANDLE);
	XME_TEST(h1 != h3);
	XME_TEST(h2 != h3);
	XME_TEST(3 == XME_HAL_TABLE_ITEM_COUNT(myTable));

	// Add 4th item
	h4 = XME_HAL_TABLE_ADD_ITEM(myTable);

#ifdef XME_HAL_TABLE_STATIC_ALLOC

	// For static tables, this fails
	XME_TEST(h4 == XME_HAL_TABLE_INVALID_ROW_HANDLE);
	XME_TEST(3 == XME_HAL_TABLE_ITEM_COUNT(myTable));

#else // #ifdef XME_HAL_TABLE_STATIC_ALLOC

	// For dynamic tables, this works well, ...
	XME_TEST(h4 != XME_HAL_TABLE_INVALID_ROW_HANDLE);
	XME_TEST(4 == XME_HAL_TABLE_ITEM_COUNT(myTable));
	
	// ... but we revert the change in order to simply further tests.
	XME_HAL_TABLE_REMOVE_ITEM(myTable, h4);
	XME_TEST(3 == XME_HAL_TABLE_ITEM_COUNT(myTable));
	h4 = XME_HAL_TABLE_INVALID_ROW_HANDLE;

#endif // #ifdef XME_HAL_TABLE_STATIC_ALLOC

	// Assign values to table rows.
	d1 = (double*)XME_HAL_TABLE_ITEM_FROM_HANDLE(myTable, h1);
	XME_TEST(NULL != d1);
	*d1 = 1.0;

	d2 = (double*)XME_HAL_TABLE_ITEM_FROM_HANDLE(myTable, h2);
	XME_TEST(NULL != d2);
	*d2 = 2.0;

	d3 = (double*)XME_HAL_TABLE_ITEM_FROM_HANDLE(myTable, h3);
	XME_TEST(NULL != d3);
	*d3 = 3.0;

	// Sum up table, after updating the sum, add 5 to row values.
	sum=0.0;
	num=0;
	XME_HAL_TABLE_ITERATE(myTable, uint16_t, i, double, item, sum += *item; *item = *item + 5.0; num++);
	XME_TEST(6.0 == sum); // 1 + 2 + 3
	XME_TEST(3 == num);

	// Check if updating worked.
	sum=0.0;
	XME_HAL_TABLE_ITERATE(myTable, uint16_t, i, double, item, sum += *item);
	XME_TEST(21.0 == sum); // (1+5) + (2+5) + (3+5)

	// Remove 2nd item
	XME_HAL_TABLE_REMOVE_ITEM(myTable, h2);
	XME_TEST(2 == XME_HAL_TABLE_ITEM_COUNT(myTable));

	// Check sum again
	sum=0.0;
	num=0;
	XME_HAL_TABLE_ITERATE(myTable, uint16_t, i, double, item, sum += *item; num++);
	XME_TEST(14.0 == sum); // (1+5) + (3+5)
	XME_TEST(2 == num);

	// Add new item, will go to 2nd row
	h5 = XME_HAL_TABLE_ADD_ITEM(myTable);
	XME_TEST(h5 != XME_HAL_TABLE_INVALID_ROW_HANDLE);
	XME_TEST(h1 != h5);
	XME_TEST(h3 != h5);
	XME_TEST(3 == XME_HAL_TABLE_ITEM_COUNT(myTable));

	h6 = XME_HAL_TABLE_ADD_ITEM(myTable);

#ifdef XME_HAL_TABLE_STATIC_ALLOC

	// For static tables, this fails
	XME_TEST(h6 == XME_HAL_TABLE_INVALID_ROW_HANDLE);
	XME_TEST(3 == XME_HAL_TABLE_ITEM_COUNT(myTable));

#else // #ifdef XME_HAL_TABLE_STATIC_ALLOC

	// For dynamic tables, this works well, ...
	XME_TEST(h6 != XME_HAL_TABLE_INVALID_ROW_HANDLE);
	XME_TEST(4 == XME_HAL_TABLE_ITEM_COUNT(myTable));

	// ... but we revert the change in order to simply further tests.
	XME_HAL_TABLE_REMOVE_ITEM(myTable, h6);
	XME_TEST(3 == XME_HAL_TABLE_ITEM_COUNT(myTable));
	h6 = XME_HAL_TABLE_INVALID_ROW_HANDLE;

#endif // #ifdef XME_HAL_TABLE_STATIC_ALLOC

	// Delete last row
	XME_HAL_TABLE_REMOVE_ITEM(myTable, h3);
	XME_TEST(2 == XME_HAL_TABLE_ITEM_COUNT(myTable));

	// And add it again
	h7 = XME_HAL_TABLE_ADD_ITEM(myTable);
	XME_TEST(h7 != XME_HAL_TABLE_INVALID_ROW_HANDLE);
	XME_TEST(h1 != h7);
	XME_TEST(h5 != h7);
	XME_TEST(3 == XME_HAL_TABLE_ITEM_COUNT(myTable));

	// Check value of 2nd row: should be still (1+5)
	XME_TEST(6.0 == *XME_HAL_TABLE_ITEM_FROM_HANDLE(myTable, h1));

	XME_HAL_TABLE_FINI(myTable);

	XME_TEST(0 == XME_HAL_TABLE_ITEM_COUNT(myTable));

	XME_TESTSUITE_END();
}

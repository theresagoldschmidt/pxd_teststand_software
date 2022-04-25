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
 *         Meta data abstraction.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/md.h"

#include "xme/core/defines.h"
#include "xme/hal/table.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_core_md_topicMetaDataItemType_t
 *
 * \brief  Item type for topic meta data.
*/
typedef enum
{
	XME_CORE_MD_TOPIC_META_DATA_TYPE_STRING, ///< String property.
	XME_CORE_MD_TOPIC_META_DATA_TYPE_NUMERIC, ///< Numeric property (positive or negative integer).
	XME_CORE_MD_TOPIC_META_DATA_TYPE_UNSIGNED, ///< Unsigned property (positive integer).
	XME_CORE_MD_TOPIC_META_DATA_TYPE_DECIMAL ///< Decimal property (double-precision floating-point number).
}
xme_core_md_topicMetaDataItemType_t;

typedef struct
{
	const char* key; ///< Property key.
	xme_core_md_topicMetaDataOperator_t op; ///< Property operator.
	xme_core_md_topicMetaDataItemType_t type; ///< Property type.
	union
	{
		const char* str; ///< String property value.
		int64_t num; ///< Numeric property value (positive or negative integer).
		uint64_t uns; ///< Unsigned property value (positive integer).
		double dec; ///< Decimal property value (double-precision floating-point number).
	} value;
}
xme_core_md_topicMetaDataItem_t;

typedef struct
{
	XME_HAL_TABLE(xme_core_md_topicMetaDataItem_t, items, XME_HAL_DEFINES_MAX_TOPIC_META_DATA_ITEMS);
}
xme_core_md_topicMetaDataDescriptor_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
xme_core_status_t
xme_core_md_fetchTopicMetaDataItem
(
	xme_core_md_topicMetaDataHandle_t topicMetaDataHandle,
	const char* key,
	xme_core_md_topicMetaDataItem_t** outTopicMetaDataItem,
	xme_hal_table_rowHandle_t* outTopicMetaDataItemHandle,
	bool autoAlloc
);

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
typedef struct
{
	//private
	XME_HAL_TABLE(xme_core_md_topicMetaDataDescriptor_t, topicMetaDataDescriptors, XME_HAL_DEFINES_MAX_TOPIC_META_DATA_DESCRIPTORS); // topicMetaDataHandle is an index into this table
}
xme_core_md_configStruct_t;

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
static xme_core_md_configStruct_t xme_core_md_config;

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_core_md_init()
{
	XME_HAL_TABLE_INIT(xme_core_md_config.topicMetaDataDescriptors);

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_core_md_fini()
{
	XME_HAL_TABLE_FINI(xme_core_md_config.topicMetaDataDescriptors);
}

xme_core_md_topicMetaDataHandle_t
xme_core_md_createTopicMetaData()
{
	xme_core_md_topicMetaDataDescriptor_t* desc;

	// Allocate a unique topic meta data handle
	xme_core_md_topicMetaDataHandle_t newTopicMetaDataHandle = (xme_core_md_topicMetaDataHandle_t)XME_HAL_TABLE_ADD_ITEM(xme_core_md_config.topicMetaDataDescriptors);

	// Store the topic and the topicMetaData, set subscribers to "unknown"
	desc = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_md_config.topicMetaDataDescriptors, newTopicMetaDataHandle);
	XME_HAL_TABLE_INIT(desc->items);

	// Return the unique topic meta data handle
	return newTopicMetaDataHandle;
}

bool
xme_core_md_isTopicMetaDataHandleValid(xme_core_md_topicMetaDataHandle_t topicMetaDataHandle)
{
	return (0 != XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_md_config.topicMetaDataDescriptors, topicMetaDataHandle));
}

xme_core_status_t
xme_core_md_fetchTopicMetaDataItem
(
	xme_core_md_topicMetaDataHandle_t topicMetaDataHandle,
	const char* key,
	xme_core_md_topicMetaDataItem_t** outTopicMetaDataItem,
	xme_hal_table_rowHandle_t* outTopicMetaDataItemHandle,
	bool autoAlloc
)
{
	xme_core_md_topicMetaDataDescriptor_t* desc;

	XME_ASSERT(0 != outTopicMetaDataItem);
	XME_ASSERT(0 != outTopicMetaDataItemHandle);
	XME_ASSERT(0 != key);
	XME_CHECK(XME_CORE_MD_EMPTY_META_DATA != topicMetaDataHandle, XME_CORE_STATUS_INVALID_HANDLE);

	desc = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_md_config.topicMetaDataDescriptors, topicMetaDataHandle);
	XME_CHECK(0 != desc, XME_CORE_STATUS_INVALID_HANDLE);

	*outTopicMetaDataItem = 0;
	*outTopicMetaDataItemHandle = 0;

	XME_HAL_TABLE_ITERATE
	(
		desc->items,
		xme_hal_table_rowHandle_t, index,
		xme_core_md_topicMetaDataItem_t, item,
		{
			if (!strcmp(key, item->key))
			{
				// The keys are identical
				*outTopicMetaDataItem = item;
				*outTopicMetaDataItemHandle = index;
				break;
			}
		}
	);

	if (!*outTopicMetaDataItem && autoAlloc)
	{
		*outTopicMetaDataItemHandle = XME_HAL_TABLE_ADD_ITEM(desc->items);
		XME_CHECK(0 != *outTopicMetaDataItemHandle, XME_CORE_STATUS_OUT_OF_RESOURCES);

		*outTopicMetaDataItem = XME_HAL_TABLE_ITEM_FROM_HANDLE(desc->items, *outTopicMetaDataItemHandle);
		(*outTopicMetaDataItem)->key = key;
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_md_setTopicMetaDataString(xme_core_md_topicMetaDataHandle_t topicMetaDataHandle, const char* key, xme_core_md_topicMetaDataOperator_t op, const char* value)
{
	xme_core_md_topicMetaDataItem_t* item;
	xme_hal_table_rowHandle_t dummy;

	xme_core_status_t status = xme_core_md_fetchTopicMetaDataItem(topicMetaDataHandle, key, &item, &dummy, true);
	XME_CHECK(XME_CORE_STATUS_SUCCESS == status, status);

	item->op = op;
	item->type = XME_CORE_MD_TOPIC_META_DATA_TYPE_STRING;
	item->value.str = value;

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_md_setTopicMetaDataNumeric(xme_core_md_topicMetaDataHandle_t topicMetaDataHandle, const char* key, xme_core_md_topicMetaDataOperator_t op, int64_t value)
{
	xme_core_md_topicMetaDataItem_t* item;
	xme_hal_table_rowHandle_t dummy;

	xme_core_status_t status = xme_core_md_fetchTopicMetaDataItem(topicMetaDataHandle, key, &item, &dummy, true);
	XME_CHECK(XME_CORE_STATUS_SUCCESS == status, status);

	item->op = op;
	item->type = XME_CORE_MD_TOPIC_META_DATA_TYPE_NUMERIC;
	item->value.num = value;

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_md_setTopicMetaDataUnsigned(xme_core_md_topicMetaDataHandle_t topicMetaDataHandle, const char* key, xme_core_md_topicMetaDataOperator_t op, uint64_t value)
{
	xme_core_md_topicMetaDataItem_t* item;
	xme_hal_table_rowHandle_t dummy;

	xme_core_status_t status = xme_core_md_fetchTopicMetaDataItem(topicMetaDataHandle, key, &item, &dummy, true);
	XME_CHECK(XME_CORE_STATUS_SUCCESS == status, status);

	item->op = op;
	item->type = XME_CORE_MD_TOPIC_META_DATA_TYPE_UNSIGNED;
	item->value.uns = value;

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_md_setTopicMetaDataDecimal(xme_core_md_topicMetaDataHandle_t topicMetaDataHandle, const char* key, xme_core_md_topicMetaDataOperator_t op, double value)
{
	xme_core_md_topicMetaDataItem_t* item;
	xme_hal_table_rowHandle_t dummy;

	xme_core_status_t status = xme_core_md_fetchTopicMetaDataItem(topicMetaDataHandle, key, &item, &dummy, true);
	XME_CHECK(XME_CORE_STATUS_SUCCESS == status, status);

	item->op = op;
	item->type = XME_CORE_MD_TOPIC_META_DATA_TYPE_DECIMAL;
	item->value.dec = value;

	return XME_CORE_STATUS_SUCCESS;
}

uint16_t
xme_core_md_topicMetaDataPropertyCount(xme_core_md_topicMetaDataHandle_t topicMetaDataHandle)
{
	xme_core_md_topicMetaDataDescriptor_t* desc;

	XME_CHECK(XME_CORE_MD_EMPTY_META_DATA != topicMetaDataHandle, 0);

	desc = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_md_config.topicMetaDataDescriptors, topicMetaDataHandle);
	XME_CHECK(0 != desc, 0);

	return XME_HAL_TABLE_ITEM_COUNT(desc->items);
}

xme_core_status_t
xme_core_md_removeTopicMetaData(xme_core_md_topicMetaDataHandle_t topicMetaDataHandle, const char* key)
{
	xme_core_md_topicMetaDataItem_t* item;
	xme_hal_table_rowHandle_t itemHandle;
	xme_core_md_topicMetaDataDescriptor_t* desc;
	xme_core_status_t status;

	XME_ASSERT(0 != key);
	XME_CHECK(XME_CORE_MD_EMPTY_META_DATA != topicMetaDataHandle, XME_CORE_STATUS_INVALID_HANDLE);

	desc = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_md_config.topicMetaDataDescriptors, topicMetaDataHandle);
	XME_ASSERT(0 != desc);

	status = xme_core_md_fetchTopicMetaDataItem(topicMetaDataHandle, key, &item, &itemHandle, false);
	XME_CHECK(XME_CORE_STATUS_SUCCESS == status, status);

	if (0 == itemHandle)
	{
		return XME_CORE_STATUS_NO_SUCH_VALUE;
	}

	XME_HAL_TABLE_REMOVE_ITEM(desc->items, itemHandle);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_core_md_destroyTopicMetaData(xme_core_md_topicMetaDataHandle_t topicMetaDataHandle)
{
	xme_core_md_topicMetaDataDescriptor_t* desc;

	XME_CHECK(XME_CORE_MD_EMPTY_META_DATA != topicMetaDataHandle, XME_CORE_STATUS_INVALID_HANDLE);

	// Retrieve the meta data table
	desc = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_core_md_config.topicMetaDataDescriptors, topicMetaDataHandle);
	XME_CHECK(0 != desc, XME_CORE_STATUS_INVALID_HANDLE);

	// Free the meta data table
	XME_HAL_TABLE_FINI(desc->items);

	// Remove the meta data table item
	{
		xme_core_status_t rval = XME_HAL_TABLE_REMOVE_ITEM(xme_core_md_config.topicMetaDataDescriptors, (xme_hal_table_rowHandle_t)topicMetaDataHandle);
		XME_ASSERT(XME_CORE_STATUS_SUCCESS == rval);
	}

	return XME_CORE_STATUS_SUCCESS;
}

bool
xme_core_md_matchesTopicMetData(const char* key, xme_core_md_topicMetaDataOperator_t op, const char* value)
{
	// TODO: See ticket #738
	return true;
}

int8_t
xme_core_md_compareMetData(xme_core_md_topicMetaDataHandle_t metaData1, xme_core_md_topicMetaDataHandle_t metaData2)
{
	//TODO: See ticket #739
	return metaData1 - metaData2;
}

bool
xme_core_md_filterAcceptsMetdata(xme_core_md_topicMetaDataHandle_t filter, xme_core_md_topicMetaDataHandle_t metaData)
{
	// TODO: See ticket #740
	return true;
}

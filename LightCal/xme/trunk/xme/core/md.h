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

#ifndef XME_CORE_MD_H
#define XME_CORE_MD_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/core.h"

#include "xme/hal/table.h"

#include <stdint.h>

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_core_md_topicMetaDataHandle_t
 *
 * \brief  Locally valid handle to a topic meta data specification.
 *         This handle refers to an object that defines key/value pairs that
 *         describe the meta data associated with a topic. The topic meta
 *         data can also be used to filter topics.
 */
typedef enum
{
	XME_CORE_MD_EMPTY_META_DATA = 0, ///< Empty meta data.
	XME_CORE_MD_MAX_TOPIC_META_DATA_HANDLE = XME_HAL_TABLE_MAX_ROW_HANDLE ///< Largest possible topic meta data handle.
}
xme_core_md_topicMetaDataHandle_t;

/**
 * \typedef xme_core_md_topicFilterHandle_t
 *
 * \brief  Locally valid handle to a topic filter specification.
 *         This handle refers to an object that defines key/value pairs.
 */
typedef uint16_t xme_core_md_topicFilterHandle_t;

/**
 * \typedef xme_core_md_topicMetaDataOperator_t
 *
 * \brief  Operators for topic meta data comparisons.
 */
typedef enum
{
	XME_CORE_MD_TOPIC_META_DATA_OPERATOR_EQUALS,
	XME_CORE_MD_TOPIC_META_DATA_OPERATOR_NOT_EQUALS,
}
xme_core_md_topicMetaDataOperator_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Initializes the meta data component.
 *         Exactly one component of this type must be present on every node.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_SUCCESS if the meta data component has been properly
 *             initialized.
 *          - XME_CORE_STATUS_OUT_OF_RESOURCES if initialization failed.
 */
xme_core_status_t
xme_core_md_init();

/**
 * \brief  Frees all resources occupied by the meta data component.
 *         Exactly one component of this type must be present on every node.
 */
void
xme_core_md_fini();

/**
 * \brief  Allocates a new locally valid topic meta data handle.
 *         Topic meta data handles can be used to specify meta data for topics
 *         as well as filter criteria for topic subscriptions.
 *
 * \return Newly allocated topic meta data handle.
 */
xme_core_md_topicMetaDataHandle_t
xme_core_md_createTopicMetaData();

bool
xme_core_md_isTopicMetaDataHandleValid(xme_core_md_topicMetaDataHandle_t topicMetaDataHandle);

// TODO: How should the signature of these functions look like? See ticket #747
xme_core_status_t
xme_core_md_setTopicMetaDataString(xme_core_md_topicMetaDataHandle_t topicMetaDataHandle, const char* key, xme_core_md_topicMetaDataOperator_t op, const char* value);

xme_core_status_t
xme_core_md_setTopicMetaDataNumeric(xme_core_md_topicMetaDataHandle_t topicMetaDataHandle, const char* key, xme_core_md_topicMetaDataOperator_t op, int64_t value);

xme_core_status_t
xme_core_md_setTopicMetaDataUnsigned(xme_core_md_topicMetaDataHandle_t topicMetaDataHandle, const char* key, xme_core_md_topicMetaDataOperator_t op, uint64_t value);

xme_core_status_t
xme_core_md_setTopicMetaDataDecimal(xme_core_md_topicMetaDataHandle_t topicMetaDataHandle, const char* key, xme_core_md_topicMetaDataOperator_t op, double value);

uint16_t
xme_core_md_topicMetaDataPropertyCount(xme_core_md_topicMetaDataHandle_t topicMetaDataHandle);

xme_core_status_t
xme_core_md_removeTopicMetaData(xme_core_md_topicMetaDataHandle_t topicMetaDataHandle, const char* key);

xme_core_status_t
xme_core_md_destroyTopicMetaData(xme_core_md_topicMetaDataHandle_t topicMetaDataHandle);

bool
xme_core_md_matchesTopicMetData(const char* key, xme_core_md_topicMetaDataOperator_t op, const char* value);

int8_t
xme_core_md_compareMetData(xme_core_md_topicMetaDataHandle_t metaData1, xme_core_md_topicMetaDataHandle_t metaData2);

bool
xme_core_md_filterAcceptsMetdata(xme_core_md_topicMetaDataHandle_t filter, xme_core_md_topicMetaDataHandle_t metaData);

#endif // #ifndef XME_CORE_MD_H

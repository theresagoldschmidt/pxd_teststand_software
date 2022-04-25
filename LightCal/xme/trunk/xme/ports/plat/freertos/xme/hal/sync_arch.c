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
 *         Synchronization abstraction (platform specific part: FreeRTOS).
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Hauke Staehle <staehle@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/sync.h"

#include "xme/core/component.h"
#include "xme/core/core.h"
#include "xme/core/defines.h"

#include "xme/hal/table.h"

#include "FreeRTOS.h"
#include "semphr.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/

typedef struct
{
	xSemaphoreHandle mutex;
}
xme_hal_sync_criticalSectionDescriptor_t;

/**
 * \typedef xme_hal_sync_configStruct_t
 *
 * \brief  Synchronization configuration structure.
 */
XME_COMPONENT_CONFIG_STRUCT
(
	xme_hal_sync,
	// private
	xSemaphoreHandle criticalSectionMutex; ///< Mutex for shared access to the list of critical sections.
	XME_HAL_TABLE(xme_hal_sync_criticalSectionDescriptor_t, criticalSections, XME_HAL_DEFINES_MAX_CRITICAL_SECTION_DESCRIPTORS); // criticalSectionHandle is an index into this table ///< List of critical sections.
);


/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
static xme_hal_sync_configStruct_t xme_hal_sync_config;

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_hal_sync_init()
{
	xSemaphoreHandle  lxSemaphore;

	XME_HAL_TABLE_INIT(xme_hal_sync_config.criticalSections);

	vSemaphoreCreateBinary( lxSemaphore );

	XME_CHECK
		(
			NULL != lxSemaphore,
			XME_CORE_STATUS_OUT_OF_RESOURCES
		);

	xme_hal_sync_config.criticalSectionMutex = lxSemaphore;
	return XME_CORE_STATUS_SUCCESS;
}

void
xme_hal_sync_fini()
{
	while( xSemaphoreTake( xme_hal_sync_config.criticalSectionMutex, portMAX_DELAY ) != pdTRUE ) {}
	XME_HAL_TABLE_FINI(xme_hal_sync_config.criticalSections);
	xSemaphoreGive( xme_hal_sync_config.criticalSectionMutex );

	vQueueDelete( xme_hal_sync_config.criticalSectionMutex );
}

xme_hal_sync_criticalSectionHandle_t
xme_hal_sync_createCriticalSection()
{
	xme_hal_sync_criticalSectionHandle_t  newCriticalSectionHandle;
	xme_hal_sync_criticalSectionDescriptor_t* criticalSectionDesc;
	xSemaphoreHandle  lxSemaphore;

	while( xSemaphoreTake( xme_hal_sync_config.criticalSectionMutex, portMAX_DELAY ) != pdTRUE ) {}
	newCriticalSectionHandle = (xme_hal_sync_criticalSectionHandle_t)XME_HAL_TABLE_ADD_ITEM(xme_hal_sync_config.criticalSections);
	xSemaphoreGive( xme_hal_sync_config.criticalSectionMutex );

	XME_CHECK_REC
	(
		XME_HAL_SYNC_INVALID_CRITICAL_SECTION_HANDLE != newCriticalSectionHandle,
		XME_HAL_SYNC_INVALID_CRITICAL_SECTION_HANDLE,
		{
				xSemaphoreGive(xme_hal_sync_config.criticalSectionMutex);
		}
	);

	// Initialize the critical section descriptor
	criticalSectionDesc = (xme_hal_sync_criticalSectionDescriptor_t*)XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_sync_config.criticalSections, newCriticalSectionHandle);

	vSemaphoreCreateBinary( lxSemaphore );

	XME_CHECK_REC
	(
		NULL != lxSemaphore,
		XME_HAL_SYNC_INVALID_CRITICAL_SECTION_HANDLE,
		{
			XME_HAL_TABLE_REMOVE_ITEM(xme_hal_sync_config.criticalSections, (xme_hal_table_rowHandle_t)newCriticalSectionHandle);
			xSemaphoreGive(xme_hal_sync_config.criticalSectionMutex);
		}
	);

	criticalSectionDesc->mutex = lxSemaphore;

	return newCriticalSectionHandle;
}

xme_core_status_t
xme_hal_sync_destroyCriticalSection
(
	xme_hal_sync_criticalSectionHandle_t criticalSectionHandle
)
{
	xme_hal_sync_criticalSectionDescriptor_t* criticalSectionDesc;

	// Synchronize access to the critical sections list mutex
	while( xSemaphoreTake( xme_hal_sync_config.criticalSectionMutex, portMAX_DELAY ) != pdTRUE ) {}

		// Retrieve the critical section descriptor
		criticalSectionDesc = (xme_hal_sync_criticalSectionDescriptor_t*)XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_sync_config.criticalSections, criticalSectionHandle);

		XME_CHECK_REC
		(
			NULL != criticalSectionDesc,
			XME_CORE_STATUS_INVALID_HANDLE,
			{
					xSemaphoreGive(xme_hal_sync_config.criticalSectionMutex);
			}
		);

		vQueueDelete( criticalSectionDesc->mutex );

		XME_HAL_TABLE_REMOVE_ITEM(xme_hal_sync_config.criticalSections, (xme_hal_table_rowHandle_t)criticalSectionHandle);

	xSemaphoreGive(xme_hal_sync_config.criticalSectionMutex);

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_hal_sync_enterCriticalSection
(
	xme_hal_sync_criticalSectionHandle_t criticalSectionHandle
)
{
	xme_hal_sync_criticalSectionDescriptor_t* criticalSectionDesc;

	// Synchronize access to the critical sections list mutex
	while( xSemaphoreTake( xme_hal_sync_config.criticalSectionMutex, portMAX_DELAY ) != pdTRUE ) {}

		// Retrieve the critical section descriptor
		criticalSectionDesc = (xme_hal_sync_criticalSectionDescriptor_t*)XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_sync_config.criticalSections, criticalSectionHandle);

	xSemaphoreGive(xme_hal_sync_config.criticalSectionMutex);

	XME_ASSERT_NORVAL(NULL != criticalSectionDesc);

	while( xSemaphoreTake( criticalSectionDesc->mutex, portMAX_DELAY ) != pdTRUE ) {}
}

xme_core_status_t
xme_hal_sync_tryEnterCriticalSection
(
	xme_hal_sync_criticalSectionHandle_t criticalSectionHandle
)
{
	xme_hal_sync_criticalSectionDescriptor_t* criticalSectionDesc;

	// Synchronize access to the critical sections list mutex
	while( xSemaphoreTake( xme_hal_sync_config.criticalSectionMutex, portMAX_DELAY ) != pdTRUE ) {}

		// Retrieve the critical section descriptor
		criticalSectionDesc = (xme_hal_sync_criticalSectionDescriptor_t*)XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_sync_config.criticalSections, criticalSectionHandle);

	xSemaphoreGive(xme_hal_sync_config.criticalSectionMutex);

	XME_ASSERT_RVAL(NULL != criticalSectionDesc, XME_CORE_STATUS_INVALID_HANDLE);

	return xSemaphoreTake(criticalSectionDesc->mutex, 1) == pdTRUE ? XME_CORE_STATUS_SUCCESS : XME_CORE_STATUS_WOULD_BLOCK;
}

void
xme_hal_sync_leaveCriticalSection
(
	xme_hal_sync_criticalSectionHandle_t criticalSectionHandle
)
{
	xme_hal_sync_criticalSectionDescriptor_t* criticalSectionDesc;

	// Synchronize access to the critical sections list mutex
	while( xSemaphoreTake( xme_hal_sync_config.criticalSectionMutex, portMAX_DELAY ) != pdTRUE ) {}

		// Retrieve the critical section descriptor
		criticalSectionDesc = (xme_hal_sync_criticalSectionDescriptor_t*)XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_sync_config.criticalSections, criticalSectionHandle);

	xSemaphoreGive(xme_hal_sync_config.criticalSectionMutex);

	XME_ASSERT_NORVAL(NULL != criticalSectionDesc);

	xSemaphoreGive(criticalSectionDesc->mutex);
}

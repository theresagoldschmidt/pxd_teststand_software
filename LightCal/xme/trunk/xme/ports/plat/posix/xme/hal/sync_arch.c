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
 *         Synchronization abstraction (platform specific part: Posix).
 *
 * \author
 *	       Deniz Kabakci <deniz.kabakci@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/sync.h"

#include "xme/core/component.h"
#include "xme/core/core.h"
#include "xme/core/defines.h"

#include "xme/hal/table.h"

#include <pthread.h>

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
typedef struct
{
	pthread_mutex_t mutex;
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
	pthread_mutex_t criticalSectionMutex; ///< Mutex for shared access to the list of critical sections.
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
	XME_HAL_TABLE_INIT(xme_hal_sync_config.criticalSections);

	XME_CHECK
	(
		0 == pthread_mutex_init(&xme_hal_sync_config.criticalSectionMutex, NULL),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_hal_sync_fini()
{
	// Synchronize access to the critical sections list mutex
	pthread_mutex_lock(&xme_hal_sync_config.criticalSectionMutex);
	{
		XME_HAL_TABLE_FINI(xme_hal_sync_config.criticalSections);
	}
	pthread_mutex_unlock(&xme_hal_sync_config.criticalSectionMutex);

	// Destroy the critical sections list mutex
	pthread_mutex_destroy(&xme_hal_sync_config.criticalSectionMutex);
}

xme_hal_sync_criticalSectionHandle_t
xme_hal_sync_createCriticalSection()
{
	xme_hal_sync_criticalSectionHandle_t newCriticalSectionHandle;
	xme_hal_sync_criticalSectionDescriptor_t* criticalSectionDesc;

	// Synchronize access to the critical sections list mutex
	pthread_mutex_lock(&xme_hal_sync_config.criticalSectionMutex);
	{
		// Allocate a unique critical section handle
		newCriticalSectionHandle = (xme_hal_sync_criticalSectionHandle_t)XME_HAL_TABLE_ADD_ITEM(xme_hal_sync_config.criticalSections);
	}
	//LeaveCriticalSection(&xme_hal_sync_config.criticalSectionMutex);
	pthread_mutex_unlock(&xme_hal_sync_config.criticalSectionMutex);

	XME_CHECK_REC
	(
		XME_HAL_SYNC_INVALID_CRITICAL_SECTION_HANDLE != newCriticalSectionHandle,
		XME_HAL_SYNC_INVALID_CRITICAL_SECTION_HANDLE,
		{
			pthread_mutex_unlock(&xme_hal_sync_config.criticalSectionMutex);
		}
	);

	// Initialize the critical section descriptor
	criticalSectionDesc = (xme_hal_sync_criticalSectionDescriptor_t*)XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_sync_config.criticalSections, newCriticalSectionHandle);

	XME_ASSERT_RVAL(NULL != criticalSectionDesc, XME_HAL_SYNC_INVALID_CRITICAL_SECTION_HANDLE);

	XME_CHECK_REC
	(
		0 == pthread_mutex_init(&criticalSectionDesc->mutex, NULL),
		XME_HAL_SYNC_INVALID_CRITICAL_SECTION_HANDLE,
		{
			XME_HAL_TABLE_REMOVE_ITEM(xme_hal_sync_config.criticalSections, (xme_hal_table_rowHandle_t)newCriticalSectionHandle);
			pthread_mutex_unlock(&xme_hal_sync_config.criticalSectionMutex);
		}
	);

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
	pthread_mutex_lock(&xme_hal_sync_config.criticalSectionMutex);
	{
		// Retrieve the critical section descriptor
		criticalSectionDesc = (xme_hal_sync_criticalSectionDescriptor_t*)XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_sync_config.criticalSections, criticalSectionHandle);

		XME_CHECK_REC
		(
			NULL != criticalSectionDesc,
			XME_CORE_STATUS_INVALID_HANDLE,
			{
				pthread_mutex_unlock(&xme_hal_sync_config.criticalSectionMutex);
			}
		);

		pthread_mutex_destroy(&criticalSectionDesc->mutex);

		XME_HAL_TABLE_REMOVE_ITEM(xme_hal_sync_config.criticalSections, (xme_hal_table_rowHandle_t)criticalSectionHandle);
	}
	pthread_mutex_unlock(&xme_hal_sync_config.criticalSectionMutex);

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
	pthread_mutex_lock(&xme_hal_sync_config.criticalSectionMutex);
	{
		// Retrieve the critical section descriptor
		criticalSectionDesc = (xme_hal_sync_criticalSectionDescriptor_t*)XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_sync_config.criticalSections, criticalSectionHandle);
	}
	pthread_mutex_unlock(&xme_hal_sync_config.criticalSectionMutex);

	XME_ASSERT_NORVAL(NULL != criticalSectionDesc);

	pthread_mutex_lock(&criticalSectionDesc->mutex);
}

xme_core_status_t
xme_hal_sync_tryEnterCriticalSection
(
	xme_hal_sync_criticalSectionHandle_t criticalSectionHandle
)
{
	xme_hal_sync_criticalSectionDescriptor_t* criticalSectionDesc;

	// Synchronize access to the critical sections list mutex
	pthread_mutex_lock(&xme_hal_sync_config.criticalSectionMutex);
	{
		// Retrieve the critical section descriptor
		criticalSectionDesc = (xme_hal_sync_criticalSectionDescriptor_t*)XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_sync_config.criticalSections, criticalSectionHandle);
	}
	pthread_mutex_unlock(&xme_hal_sync_config.criticalSectionMutex);

	XME_ASSERT_RVAL(NULL != criticalSectionDesc, XME_CORE_STATUS_INVALID_HANDLE);

	return pthread_mutex_trylock(&criticalSectionDesc->mutex) ? XME_CORE_STATUS_WOULD_BLOCK : XME_CORE_STATUS_SUCCESS;	
}

void
xme_hal_sync_leaveCriticalSection
(
	xme_hal_sync_criticalSectionHandle_t criticalSectionHandle
)
{
	xme_hal_sync_criticalSectionDescriptor_t* criticalSectionDesc;

	// Synchronize access to the critical sections list mutex
	pthread_mutex_lock(&xme_hal_sync_config.criticalSectionMutex);
	{
		// Retrieve the critical section descriptor
		criticalSectionDesc = (xme_hal_sync_criticalSectionDescriptor_t*)XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_sync_config.criticalSections, criticalSectionHandle);
	}
	pthread_mutex_unlock(&xme_hal_sync_config.criticalSectionMutex);

	XME_ASSERT_NORVAL(NULL != criticalSectionDesc);

	pthread_mutex_unlock(&criticalSectionDesc->mutex);
}

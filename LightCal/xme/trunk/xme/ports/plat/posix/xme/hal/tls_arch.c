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
 *         Thread-local storage abstraction (platform specific part: Posix).
 *
 * \author
 *	       Deniz Kabakci <deniz.kabakci@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 */

// TODO (See ticket #806): Access to the TLS functions must follow the monitor pattern!
//                         Currently, data structures may get corrupted if multiple threads
//                         access them in parallel! To implement this in a nice way, we need
//                         a semaphore abstraction!

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/tls.h"

#include "xme/core/defines.h"

#include "xme/hal/table.h"

#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
						
/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \ingroup hal_tls
 *
 * \typedef xme_hal_tls_item_t
 *
 * \brief  Thread-local storage descriptor.
 */
typedef struct
{
	pthread_key_t index;///< Platform dependent thread-local storage index.
	uint16_t size; ///< Requested size of the memory at the given thread-local storage index.
}
xme_hal_tls_item_t;

/**
 * \ingroup hal_tls
 *
 * \typedef xme_hal_tls_configStruct_t
 *
 * \brief  Thread-local storage configuration structure.
 */
typedef struct
{
	XME_HAL_TABLE(xme_hal_tls_item_t, items, XME_HAL_DEFINES_MAX_TLS_ITEMS);
}
xme_hal_tls_configStruct_t;

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
xme_hal_tls_configStruct_t xme_hal_tls_config;

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_hal_tls_init()
{
	XME_HAL_TABLE_INIT(xme_hal_tls_config.items);

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_hal_tls_fini()
{
	XME_ASSERT_NORVAL(XME_ASSERT_NO_SIDE_EFFECTS(0 == XME_HAL_TABLE_ITEM_COUNT(xme_hal_tls_config.items)));
	XME_HAL_TABLE_FINI(xme_hal_tls_config.items);
}

xme_hal_tls_handle_t
xme_hal_tls_alloc
(
	uint16_t size
)
{
	xme_hal_tls_handle_t handle;
	xme_hal_tls_item_t* item;

	XME_CHECK
	(
		0 != size,
		XME_HAL_TLS_INVALID_TLS_HANDLE
	);

	XME_CHECK
	(
		0 != (handle = (xme_hal_tls_handle_t)XME_HAL_TABLE_ADD_ITEM(xme_hal_tls_config.items)),
		XME_HAL_TLS_INVALID_TLS_HANDLE
	);

	item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_tls_config.items, handle);
	XME_ASSERT_RVAL(NULL != item, XME_HAL_TLS_INVALID_TLS_HANDLE);

	XME_CHECK_REC
	(
		//TLS_OUT_OF_INDEXES != (item->index = TlsAlloc()),
		//ENOMEM != pthread_key_create(&item->index,NULL),	// which one is correct??
		0 == pthread_key_create(&item->index,NULL),
		XME_HAL_TLS_INVALID_TLS_HANDLE, //XME_CORE_STATUS_OUT_OF_RESOURCES
		{
			XME_HAL_TABLE_REMOVE_ITEM(xme_hal_tls_config.items, (xme_hal_table_rowHandle_t)handle);
		}
	);

	item->size = size;

	return handle;
}

void*
xme_hal_tls_get
(
	xme_hal_tls_handle_t tlsHandle
)
{
	xme_hal_tls_item_t* item;
	void* data;

	XME_CHECK
	(
		NULL != (item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_tls_config.items, tlsHandle)),
		NULL // XME_CORE_STATUS_INVALID_HANDLE
	);

	//data = TlsGetValue(item->index);
	data=pthread_getspecific(item->index);
	if (NULL == data)
	{
#if 0
		XME_CHECK
		(
			ERROR_SUCCESS == GetLastError(),
			NULL // XME_CORE_STATUS_INVALID_PARAMETER
		);
#endif
		// Memory has not yet been allocated
		XME_CHECK
		(
			NULL != (data = malloc(item->size)),
			NULL // XME_CORE_STATUS_OUT_OF_RESOURCES
		);

		XME_CHECK_REC
		(
			(0==pthread_setspecific(item->index, data)),
			NULL, // XME_CORE_STATUS_OUT_OF_RESOURCES
			{

				free(data);
			}
		);
	}
	
	XME_ASSERT_RVAL(NULL != data, NULL);
	
	return data;
}

uint16_t
xme_hal_tls_getSize
(
	xme_hal_tls_handle_t tlsHandle
)
{
	xme_hal_tls_item_t* item;

	XME_CHECK
	(
		NULL != (item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_tls_config.items, tlsHandle)),
		0
	);

	return item->size;
}

xme_core_status_t
xme_hal_tls_free
(
	xme_hal_tls_handle_t tlsHandle
)
{
	// TODO (See ticket #807): TlsFree() does not take care of freeing all the allocated memory occupied
	//                         by other threads! Assert here that the memory has been freed before!

	xme_hal_tls_item_t* item;

	XME_CHECK
	(
		NULL != (item = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_tls_config.items, tlsHandle)),
		XME_CORE_STATUS_INVALID_HANDLE
	);

	XME_CHECK
	(
		0 == pthread_key_delete(item->index),
		XME_CORE_STATUS_INVALID_PARAMETER
	);

	{
		xme_core_status_t rval = XME_HAL_TABLE_REMOVE_ITEM(xme_hal_tls_config.items, (xme_hal_table_rowHandle_t)tlsHandle);
		XME_ASSERT(XME_CORE_STATUS_SUCCESS == rval);
	}

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_hal_tls_registerThread()
{
	// Nothing to do, memory will be allocated on demand as soon as data need to be stored
	return XME_CORE_STATUS_SUCCESS;
}

void
xme_hal_tls_deregisterThread()
{
	// Free the memory occupied by the local storage of this thread
	XME_HAL_TABLE_ITERATE
	(
		xme_hal_tls_config.items,
		xme_hal_tls_handle_t, handle,
		xme_hal_tls_item_t, item,
		{
			void* data=pthread_getspecific(item->index);
			if (NULL != data)
			{
				free(data);
			}
		}
	);
}

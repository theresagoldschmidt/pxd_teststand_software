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
 *         Thread-local storage abstraction (architecture specific part:
 *         generic embedded implementation).
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

// This implementation is for embedded platforms without multithreading.
// In this case, thread local storage is implemented using a single shared
// memory location for each TLS handle.

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/tls.h"

#include "xme/core/defines.h"

#include "xme/hal/sharedPtr.h"

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_hal_tls_init()
{
	// Nothing to do

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_hal_tls_fini()
{
	// Nothing to do
}

xme_hal_tls_handle_t
xme_hal_tls_alloc
(
	uint16_t size
)
{
	// Return a handle to the memory
	return (xme_hal_tls_handle_t)xme_hal_sharedPtr_create(size);
}

void*
xme_hal_tls_get
(
	xme_hal_tls_handle_t tlsHandle
)
{
	// Return the pointer to the memory
	return xme_hal_sharedPtr_getPointer((xme_hal_sharedPtr_t)tlsHandle);
}

xme_core_status_t
xme_hal_tls_free
(
	xme_hal_tls_handle_t tlsHandle
)
{
	xme_hal_sharedPtr_destroy((xme_hal_sharedPtr_t)tlsHandle);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_hal_tls_registerThread()
{
	// Nothing to do

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_hal_tls_deregisterThread()
{
	// Nothing to do
}

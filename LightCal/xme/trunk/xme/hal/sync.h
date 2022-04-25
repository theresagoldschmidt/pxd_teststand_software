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
 *         Public synchronization API.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_CORE_SYNC_H
#define XME_CORE_SYNC_H

/**
 * \defgroup hal_sync Synchronization
 *
 * \brief  Synchronization abstraction.
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/table.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \ingroup hal_sync
 *
 * \typedef xme_hal_sync_criticalSectionHandle_t
 *
 * \brief  Critical section handle.
 */
typedef enum
{
	XME_HAL_SYNC_INVALID_CRITICAL_SECTION_HANDLE = 0, ///< Invalid critical section handle.
	XME_HAL_SYNC_MAX_CRITICAL_SECTION_HANDLE = XME_HAL_TABLE_MAX_ROW_HANDLE ///< Largest possible critical section handle.
}
xme_hal_sync_criticalSectionHandle_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \ingroup hal_sync
 *
 * \brief  Initializes the synchronization abstraction.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the synchronization abstraction has
 *            been properly initialized.
 *          - XME_CORE_STATUS_OUT_OF_RESOURCES if synchronization abstraction
 *            initialization failed.
 */
xme_core_status_t
xme_hal_sync_init();

/**
 * \ingroup hal_sync
 *
 * \brief  Frees resources occupied by the synchronization abstraction.
 */
void
xme_hal_sync_fini();

/**
 * \ingroup hal_sync
 *
 * \brief  Allocates a new critical section handle.
 *
 * \note   Critical section handles allocated by a call to this function need
 *         to be deleted once they are not used any more with a call to
 *         xme_hal_sync_destroyCriticalSection().
 *
 * \return Returns a non-zero critical section handle on success and
 *         XME_HAL_SYNC_INVALID_CRITICAL_SECTION_HANDLE on error.
 */
xme_hal_sync_criticalSectionHandle_t
xme_hal_sync_createCriticalSection();

/**
 * \ingroup hal_sync
 *
 * \brief  Destroys the given critical section object.
 *
 * \note   Do not use the critical section handle in any function
 *         after the critical section has been destroyed.
 *
 * \param  criticalSectionHandle Handle of the critical section to destroy.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the critical section has been
 *            successfully destroyed.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the given critical section
 *            handle was invalid.
 */
xme_core_status_t
xme_hal_sync_destroyCriticalSection
(
	xme_hal_sync_criticalSectionHandle_t criticalSectionHandle
);

/**
 * \ingroup hal_sync
 *
 * \brief  Waits for ownership of the specified critical section.
 *         The function returns when the calling task is granted ownership.
 *
 * \param  criticalSectionHandle Handle of the critical section to enter.
 */
void
xme_hal_sync_enterCriticalSection
(
	xme_hal_sync_criticalSectionHandle_t criticalSectionHandle
);

/**
 * \ingroup hal_sync
 *
 * \brief  Attempts to enter a critical section without blocking. If the call
 *         is successful, the calling task takes ownership of the critical
 *         section.
 *
 * \param  criticalSectionHandle Handle of the critical section to enter.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the calling task has successfully
 *            taken ownership of the specified critical section.
 *          - XME_CORE_STATUS_WOULD_BLOCK if the critical section is currently
 *            in use by another task. Ownership of the calling task has not
 *            been taken.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the given critical section
 *            handle was invalid.
 */
xme_core_status_t
xme_hal_sync_tryEnterCriticalSection
(
	xme_hal_sync_criticalSectionHandle_t criticalSectionHandle
);

/**
 * \ingroup hal_sync
 *
 * \brief  Releases ownership of the specified critical section.
 *
 * \param  criticalSectionHandle Handle of the critical section to leave.
 */
void
xme_hal_sync_leaveCriticalSection
(
	xme_hal_sync_criticalSectionHandle_t criticalSectionHandle
);

#endif // #ifndef XME_CORE_SYNC_H

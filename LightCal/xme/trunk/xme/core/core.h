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
 *         Core runtime system.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_CORE_CORE_H
#define XME_CORE_CORE_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/defines.h"

#include <stdbool.h>

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_core_status_t
 *
 * \brief Core status codes.
 */
typedef enum
{
	XME_CORE_STATUS_SUCCESS = 0, ///< The operation completed successfully. This must be defined as zero.
	XME_CORE_STATUS_UNEXPECTED, ///< Error: Assertion failed. This value can be returned by any API function in debug mode even if an assertion failed even if it is not documented.
	XME_CORE_STATUS_NO_SUCH_VALUE, ///< Error: No such value.
	XME_CORE_STATUS_INTERNAL_ERROR, ///< Error: Unspecified internal error.
	XME_CORE_STATUS_INVALID_PARAMETER, ///< Error: Invalid parameter. At least one of the given parameters is set to an invalid or unsupported value.
	XME_CORE_STATUS_INVALID_CONFIGURATION, ///< Error: Invalid configuration. This means that all parameters were set to proper values, but the given combination of configuration parameters is not allowed at this point in time.
	XME_CORE_STATUS_INVALID_HANDLE, ///< Error: Invalid handle. The specified handle is not valid (any more).
	XME_CORE_STATUS_PERMISSION_DENIED, ///< Error: Permission denied.
	XME_CORE_STATUS_UNSUPPORTED, ///< Error: Requested function is not supported on this platform.
	XME_CORE_STATUS_NOT_FOUND, ///< Error: Object not found.
	XME_CORE_STATUS_OUT_OF_RESOURCES, ///< Error: Out of resources.
	XME_CORE_STATUS_ALREADY_EXIST, ///< Error: Object already exist.
	XME_CORE_STATUS_TEMPORARY_FAILURE, ///< Error: Temporary failure, try again later.
	XME_CORE_STATUS_TIMEOUT, ///< Error: Request timed out.
	XME_CORE_STATUS_CONNECTION_REFUSED, ///< Error: Connection refused.
	XME_CORE_STATUS_WOULD_BLOCK ///< Error: Operation would block.
}
xme_core_status_t;

/**
 * \typedef xme_core_state_t
 *
 * \brief Internal runtime system state.
 */
typedef enum
{
	XME_CORE_STATE_UNDEF = 0, ///< Runtime system is in an undefined state.
	XME_CORE_STATE_INIT,      ///< Runtime system is initialized.
	XME_CORE_STATE_SHUTDOWN   ///< Runtime system is about to be shut down.
}
xme_core_state_t;

/**
 * \typedef xme_core_configStruct_t
 *
 * \brief Internal XME configuration structure.
 */
typedef struct
{
	xme_core_state_t state; ///< Runtime system state.
}
xme_core_configStruct_t;

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
/**
 * \brief 
 */
static xme_core_configStruct_t xme_core_config =
{
	XME_CORE_STATE_UNDEF, // state
};

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Initializes the runtime system.
 *
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the runtime system has been properly
 *            initialized.
 *          - XME_CORE_STATUS_OUT_OF_RESOURCES if runtime system initialization
 *            failed.
 */
xme_core_status_t
xme_core_init();

/**
 * \brief  Runs the runtime system.
 *         If the runtime system is properly initialized, a call to this
 *         function will not return until the core system is stopped using
 *         xme_core_shutdown(). Otherwise, the function may return
 *         immediately.
 */
void
xme_core_run();

/**
 * \brief  Invokes an asynchronous shutdown of the runtime system.
 */
void
xme_core_shutdown();

/**
 * \brief  Returns whether an asynchronous shutdown of the runtime system is in progress.
 *
 * \return Whether an asynchronous shutdown of the runtime system is in
 *         progress.
 */
bool
xme_core_isShutdownInProgress();

/**
 * \brief  Frees resources occupied by the runtime system.
 */
void
xme_core_fini();

#endif // #ifndef XME_CORE_CORE_H

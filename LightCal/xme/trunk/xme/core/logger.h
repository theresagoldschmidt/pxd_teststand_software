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
 *         Logger component.
 *
 *         Publishes and sends log messages.
 *
 * \author
 *         Benjamin Wiesmueller <wiesmueller@fortiss.org>
 */

#ifndef XME_CORE_LOGGER_H
#define XME_CORE_LOGGER_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/core.h"
#include "xme/core/log.h"
#include "xme/core/dcc.h"

#include "xme/hal/tls.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_core_logger_configStruct_t
 *
 * \brief   Configuration structure of the logger component.
 */
typedef struct
{
	// private
	xme_core_dcc_publicationHandle_t publicationHandle;
	xme_hal_tls_handle_t logNestingLvlTlsHandle;
}
xme_core_logger_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Creates a logger component.
 *         Exactly one component of this type must be present on every node.
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the component has been successfully
 *            initialized.
 *          - XME_CORE_STATUS_INVALID_CONFIGURATION if a component of this
 *            type has already been initialized. Exactly one component of this
 *            type must be present on every node.
 *          - XME_CORE_STATUS_OUT_OF_RESOURCES if the needed resources could
 *            not be allocated.
 */
xme_core_status_t
xme_core_logger_create(xme_core_logger_configStruct_t* config);

/**
 * \brief  Activates a directory component.
 */
xme_core_status_t
xme_core_logger_activate(xme_core_logger_configStruct_t* config);

/**
 * \brief  Deactivates a directory component.
 */
void
xme_core_logger_deactivate(xme_core_logger_configStruct_t* config);

/**
 * \brief  Destroys a directory component.
 *         Exactly one component of this type must be present on every node.
 */
void
xme_core_logger_destroy(xme_core_logger_configStruct_t* config);

#endif // #ifndef XME_CORE_LOGGER_H

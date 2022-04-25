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
 *         Logger registry.
 *
 * \author
 *         Benjamin Wiesmueller <wiesmueller@fortiss.org>
 */

#ifndef XME_CORE_LOGREG_H
#define XME_CORE_LOGREG_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/component.h"
#include "xme/core/log.h"

#include "xme/hal/table.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef  xme_core_logReg_loggerHandle_t
 *
 * \brief    Logger handle.
 */
typedef enum
{
	XME_CORE_LOGREG_INVALID_LOGGER_HANDLE = 0, ///< Invalid logger handle.
	XME_CORE_LOGREG_MAX_LOGGER_HANDLE = XME_HAL_TABLE_MAX_ROW_HANDLE ///< Largest possible task handle.
}
xme_core_logReg_loggerHandle_t;

/**
 * \typedef xme_core_logReg_taskCallback_t
 *
 * \brief  Logger callback function.
 *
 * \param  componentId Locally valid id of component which is sending this log 
 *         message.
 * \param  severity Severity of log message.
 * \param  message The log message itself.
 * \param  userData Pointer to data as passed in the register call.
 */
typedef void (*xme_core_logReg_loggerCallback_t) 
(
	xme_core_component_t componentId, 
	xme_log_severity_t severity, 
	const char* message,
	void* userData
);

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Initializes the logger registry.
 */
xme_core_status_t
xme_core_logReg_init();

/**
 * \brief  Frees resources occupied by the logger registry.
 */
void
xme_core_logReg_fini();

/**
 * \brief  Registers a logger.
 *
 *         After registration the logger callback function will be invoked
 *         every time the XME_LOG() macro is called and the severity of the log
 *         message is at least as high as the given minimum severity for this
 *         logger.
 *         Note that log messages created inside the context of a registered
 *         logger component will not be processed by the logger registry.
 *
 * \param  minSeverity Minimal severity for which the logger callback should be
 *         invoked.
 * \param  callback Pointer to callback function doing the actual logging (e.g.,
 *         publishing log messages or writing to console).
 * \param  userData A pointer to user-defined data that will be passed along with
 *         each call of the callback function (e.g., a reference to the
 *         configuration structure of the logger).
 * \return Handler for registered logger or XME_CORE_LOGREG_INVALID_LOGGER_HANDLE
 *         if maximum number of loggers is reached or logger component id is
 *         invalid.
 */
xme_core_logReg_loggerHandle_t
xme_core_logReg_registerLogger
(
	xme_log_severity_t minSeverity,
	xme_core_logReg_loggerCallback_t callback,
	void* userData
);

/**
 * \brief  Deregisters a logger.
 *
 * \param  loggerHandle Handle of the logger to remove.
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the logger has been successfully
 *            deregistered.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the given logger handle was
 *            invalid.
 */
xme_core_status_t
xme_core_logReg_deregisterLogger
(
	xme_core_logReg_loggerHandle_t loggerHandle
);

/**
 * \brief  Pass log message to all registered loggers.
 *
 *         Called by the XME_LOG macro if the logger registry is used for 
 *         logging.
 *
 * \param  componentId Locally valid id of the component which is sending this 
 *         log message.
 * \param  severity Severity of the log message.
 * \param  message The log message itself.
 */
void
xme_core_logReg_log
(
	xme_log_severity_t severity,
	const char* message
);

#endif // #ifndef XME_CORE_LOGREG_H

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
 *         Logging system abstraction.
 *
 * \author
 *         Simon Barner <Barner@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Benjamin Wiesmueller <wiesmueller@fortiss.org>
 */

#ifndef XME_CORE_LOG_H
#define XME_CORE_LOG_H

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_log_severity_t
 *
 * \brief Log message severity.
 */
typedef enum
{
	XME_LOG_DEBUG   = -4, ///< Debug message for developers.
	XME_LOG_VERBOSE = -2, ///< Verbose message.
	XME_LOG_NOTE    =  0, ///< Note.
	XME_LOG_WARNING =  2, ///< Warning.
	XME_LOG_ERROR   =  4, ///< Error message.
	XME_LOG_FATAL   =  6  ///< Fatal error message.
}
xme_log_severity_t;

/**
 * \typedef xme_core_log_logCallback_t
 *
 * \brief  Callback function for sending log messages.
 *
 * \param  severity Log message severity.
 * \param  message Log message string (no further formatting done).
 */
typedef void (*xme_core_log_logCallback_t)
(
	xme_log_severity_t severity,
	const char* message
);

/******************************************************************************/
/***   Global variables                                                     ***/
/******************************************************************************/
/**
 * Callback function used by the platform specific implementations of XME_LOG 
 * (XME_LOG_ARCH definition in log_arch.h) to send log messages.
 * The core logger component will set this to the actual function during its
 * initialization. When not using the core logger you can point the callback
 * to a custom implementation.
 * When NULL no log messages will be sent.
 */
xme_core_log_logCallback_t xme_core_log_logCallback;

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
/**
 * \def    XME_LOG_CONSOLE_MIN_SEVERITY
 *
 * \brief  Minimum severity for console logging in case the xme_core_logger
 *         component is not used.
 */
#define XME_LOG_CONSOLE_MIN_SEVERITY XME_LOG_NOTE

/**
 * \def    XME_LOG
 *
 * \brief  Send a log message.
 *
 *         Depending on the platform specific implementation long log messages 
 *         might be truncated, or message formatting might be unavailable.
 *
 * \param  severity Log message severity.
 * \param  message Log message string literal or const char* pointer. The string
 *         can include special format characters as known from the printf()
 *         function.
 * \param  ... Optional parameters for formatting the log message string.
 */
#define XME_LOG(severity, message, ...) \
	XME_LOG_ARCH(severity, message, ##__VA_ARGS__)

/**
 * \def    XME_LOG_IF
 *
 * \brief  Sends a log message if the given condition holds.
 *
 *         Depending on the platform specific implementation long log messages 
 *         might be truncated, or message formatting might be unavailable.
 *
 * \param  condition Condition to check. Should evaluate to a boolean value.
 * \param  severity Log message severity.
 * \param  message Log message string literal or const char* pointer. The string
 *         can include special format characters as known from the printf()
 *         function.
 * \param  ... Optional parameters for formatting the log message string.
 */
#define XME_LOG_IF(condition, severity, message, ...) \
	do { \
		if (condition) \
		{ \
			XME_LOG(severity, message, ##__VA_ARGS__); \
		} \
	} while (0)

/******************************************************************************/
/***   Platform-specific includes                                           ***/
/******************************************************************************/
#include "xme/core/log_arch.h"

#endif // #ifndef XME_CORE_LOG_H

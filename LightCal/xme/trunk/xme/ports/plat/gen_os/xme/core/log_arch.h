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
 *         Logging system abstraction (architecture specific part: generic OS
 *                                     based implementation).
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Benjamin Wiesmueller <wiesmueller@fortiss.org>
 */

#ifndef XME_CORE_LOG_ARCH_H
#define XME_CORE_LOG_ARCH_H

#ifndef XME_CORE_LOG_H
	#error This architecture-specific header file should not be included directly. Include the generic header file (usually without "_arch" suffix) instead.
#endif // #ifndef XME_CORE_LOG_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdint.h>
#include <stdio.h>

/******************************************************************************/
/***   Global variables                                                     ***/
/******************************************************************************/
extern xme_core_log_logCallback_t xme_core_log_logCallback;

/******************************************************************************/
/***   Forward declarations                                                 ***/
/******************************************************************************/
/**
 * \brief  Forward declaration to prevent circular dependency.
 *
 * \see    Header xme/hal/mem.h
 */
void*
xme_hal_mem_alloc
(
	uint16_t size
);

/**
 * \brief  Forward declaration to prevent circular dependency.
 *
 * \see    Header xme/hal/mem.h
 */
void
xme_hal_mem_free
(
	void* memory
);

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
/**
 * \def    XME_LOG_CONSOLE
 *
 * \brief  Outputs a log message to the console.
 *
 * \param  severity Log message severity.
 * \param  message Log message string literal or const char* pointer. The string
 *         can include special format characters as known from the printf()
 *         function.
 * \param  ... Optional parameters for formatting the log message string.
 */
#define XME_LOG_CONSOLE(severity, message, ...) \
	do { \
		if (severity < XME_LOG_CONSOLE_MIN_SEVERITY) \
		{ \
			break; \
		} \
		printf \
		( \
			"%s", \
			(XME_LOG_WARNING == severity) ? "Warning: " : ( \
			(XME_LOG_ERROR == severity) ? "Error: " : ( \
			(XME_LOG_FATAL == severity) ? "Fatal: " : ( \
			(XME_LOG_VERBOSE == severity) ? "Verbose: " : ( \
			(XME_LOG_DEBUG == severity) ? "Debug: " : "")))) \
		); \
		printf(message, ##__VA_ARGS__); \
	} while (0)

/**
 * \def    XME_LOG_ARCH
 *
 * \brief  Platform specific implementation of XME_LOG.
 *
 *         Formats log message and calls log callback.
 *
 * \param  severity Log message severity.
 * \param  message Log message string literal or const char* pointer. The string
 *         can include special format characters as known from the printf()
 *         function.
 * \param  ... Optional parameters for formatting the log message string.
 */
#ifdef WIN32
	// Use _scprintf() to determine the length of the formatted string and
	// allocate a buffer that is large enough.
	#define XME_LOG_ARCH(severity, message, ...) \
		do { \
			if (NULL != xme_core_log_logCallback) \
			{ \
				char* strBuffer; \
				int formattedMsgLen; \
				\
				formattedMsgLen = _scprintf(message, ##__VA_ARGS__); /* terminating null character not included */ \
				strBuffer = (char*)xme_hal_mem_alloc((uint16_t)(formattedMsgLen + 1)); \
				\
				if (NULL != strBuffer) \
				{ \
					_snprintf_s(strBuffer, (formattedMsgLen + 1), _TRUNCATE, message, ##__VA_ARGS__); \
					xme_core_log_logCallback(severity, strBuffer); \
					xme_hal_mem_free(strBuffer); \
				} \
				else \
				{ \
					xme_core_log_logCallback(severity, "[Not enough memory for log message]"); \
				} \
			} \
			else \
			{ \
				XME_LOG_CONSOLE(severity, message, ##__VA_ARGS__); \
			} \
		} while (0)

#else // #ifdef WIN32
	// Use snprintf() to determine the length of the formatted string and
	// allocate a buffer that is large enough.
	// TODO: Test non-WIN32 version of XME_LOG_LOGREG. See ticket #726
	#define XME_LOG_ARCH(severity, message, ...) \
		do { \
			if (NULL != xme_core_log_logCallback) \
			{ \
				char* strBuffer; \
				int formattedMsgLen; \
				\
				formattedMsgLen = snprintf(NULL, 0, message, ##__VA_ARGS__); /* terminating null character not included */ \
				strBuffer = (char*)xme_hal_mem_alloc((uint16_t)(formattedMsgLen + 1)); \
				\
				if (NULL != strBuffer) \
				{ \
					snprintf(strBuffer, (formattedMsgLen + 1), message, ##__VA_ARGS__); \
					xme_core_log_logCallback(severity, strBuffer); \
					xme_hal_mem_free(strBuffer); \
				} \
				else \
				{ \
					xme_core_log_logCallback(severity, "[Not enough memory for log message]"); \
				} \
			} \
			else \
			{ \
				XME_LOG_CONSOLE(severity, message, ##__VA_ARGS__); \
			} \
		} while (0)

#endif // #ifdef WIN32

#endif // #ifndef XME_CORE_LOG_ARCH_H

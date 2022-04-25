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
 *         Generic defines.
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_CORE_DEFINES_H
#define XME_CORE_DEFINES_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
// Include options controlled by FindXME.cmake which generates this include
// file into a subdirectory of the binary folder.
#include "xme/xme_opt.h"

#include "xme/core/log.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
/**
 * \def    XME_CHECK
 *
 * \brief  Returns from the current function with the given return value if the
 *         given condition does not hold.
 *
 *         This macro helps to enforce a paradigm most functions is XME
 *         follow: they first check the arguments given to them and return
 *         an error code if a parameter does not meet their expectations.
 *
 * \note   If you need to do cleanup tasks in case the condition does not
 *         hold, use the XME_CHECK_REC() macro instead. If you want to output
 *         a log message when the condition does not hold, use the
 *         XME_CHECK_MSG() or XME_CHECK_MSG_REC() macro instead.
 *
 * \param  condition Condition to check. Should evaluate to a boolean value.
 * \param  rval Return value to pass to the caller of the current function
 *         in case the condition does not hold.
 */
#define XME_CHECK(condition, rval) \
	do { \
		if (!(condition)) \
		{ \
			return rval; \
		} \
	} while (0)

/**
 * \def    XME_CHECK_REC
 *
 * \brief  Performs the given recovery (cleanup) operations and returns from
 *         the current function with the given return value if the given
 *         condition does not hold.
 *
 *         This macro helps to enforce a paradigm most functions is XME
 *         follow: they first check the arguments given to them and return
 *         an error code if a parameter does not meet their expectations.
 *
 * \note   If you do not need to do cleanup tasks in case the condition does
 *         not hold, use the XME_CHECK() macro instead. If you want to output
 *         a log message when the condition does not hold, use the
 *         XME_CHECK_MSG() or XME_CHECK_MSG_REC() macro instead.
 *
 * \param  condition Condition to check. Should evaluate to a boolean value.
 * \param  rval Return value to pass to the caller of the current function
 *         in case the condition does not hold.
 * \param  recovery Recovery (cleanup) operations to perform in case the
 *         condition does not hold.
 */
#define XME_CHECK_REC(condition, rval, recovery) \
	do { \
		if (!(condition)) \
		{ \
			do { recovery; } while (0); \
			return rval; \
		} \
	} while (0)

/**
 * \def    XME_CHECK_MSG
 *
 * \brief  Outputs a log message and returns from the current function with
 *         the given return value if the given condition does not hold.
 *
 *         This macro helps to enforce a paradigm most functions is XME
 *         follow: they first check the arguments given to them and return
 *         an error code if a parameter does not meet their expectations.
 *
 * \note   If you need to do cleanup tasks in case the condition does not
 *         hold, use the XME_CHECK_MSG_REC() macro instead. If you do not want
 *         to output a log message when the condition does not hold, use the
 *         XME_CHECK() or XME_CHECK_REC() macro instead.
 *
 * \param  condition Condition to check. Should evaluate to a boolean value.
 * \param  rval Return value to pass to the caller of the current function
 *         in case the condition does not hold.
 * \param  severity Log message severity.
 * \param  message Log message string. The string can include special format
 *         characters as known from the printf() function.
 * \param  ... Optional parameters for formatting the log message string.
 */
#define XME_CHECK_MSG(condition, rval, severity, message, ...) \
	do { \
		if (!(condition)) \
		{ \
			XME_LOG(severity, message, ##__VA_ARGS__); \
			return rval; \
		} \
	} while (0)

/**
 * \def    XME_CHECK_MSG_REC
 *
 * \brief  Outputs a log message, performs the given recovery (cleanup)
 *         operations and returns from the current function with the given
 *         return value if the given condition does not hold.
 *
 *         This macro helps to enforce a paradigm most functions is XME
 *         follow: they first check the arguments given to them and return
 *         an error code if a parameter does not meet their expectations.
 *
 * \note   If you need to do cleanup tasks in case the condition does not
 *         hold, use the XME_CHECK_MSG() macro instead. If you do not want
 *         to output a log message when the condition does not hold, use the
 *         XME_CHECK() or XME_CHECK_REC() macro instead.
 *
 * \param  condition Condition to check. Should evaluate to a boolean value.
 * \param  rval Return value to pass to the caller of the current function
 *         in case the condition does not hold.
 * \param  recovery Recovery (cleanup) operations to perform in case the
 *         condition does not hold.
 * \param  severity Log message severity.
 * \param  message Log message string. The string can include special format
 *         characters as known from the printf() function.
 * \param  ... Optional parameters for formatting the log message string.
 */
#define XME_CHECK_MSG_REC(condition, rval, recovery, severity, message, ...) \
	do { \
		if (!(condition)) \
		{ \
			XME_LOG(severity, message, ##__VA_ARGS__); \
			do { recovery; } while (0); \
			return rval; \
		} \
	} while (0)

/******************************************************************************/
/***   Platform-specific includes                                           ***/
/******************************************************************************/
#include "xme/core/defines_arch.h"

#endif // #ifndef XME_CORE_DEFINES_H

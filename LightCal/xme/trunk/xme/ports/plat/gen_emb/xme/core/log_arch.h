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
 *         Logging system abstraction (architecture specific part: generic embedded
 *                                     implementation).
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_CORE_LOG_ARCH_H
#define XME_CORE_LOG_ARCH_H

#ifndef XME_CORE_LOG_H
	#error This architecture-specific header file should not be included directly. Include the generic header file (usually without "_arch" suffix) instead.
#endif // #ifndef XME_CORE_LOG_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/

/******************************************************************************/
/***   Global variables                                                     ***/
/******************************************************************************/
extern xme_core_log_logCallback_t xme_core_log_logCallback;

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/

/**
 * \def    XME_LOG_ARCH
 *
 * \brief  Platform specific implementation of XME_LOG.
 *
 *         Calls log callback function. No formatting supported here.
 *
 * \param  severity Log message severity.
 * \param  message Log message string literal or const char* pointer.
 * \param  ... Optional parameters will be ignored.
 */
#define XME_LOG_ARCH(severity, message, ...) \
	do { \
		if (NULL != xme_core_log_logCallback) \
		{ \
			xme_core_log_logCallback(severity, message); \
		} \
	} while (0)
	
// TODO: Not fully implemented yet. See ticket #839

#endif // #ifndef XME_CORE_LOG_ARCH_H

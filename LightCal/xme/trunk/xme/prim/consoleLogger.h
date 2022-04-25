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
 *         Console logger.
 *
 *         Receives XME_LOG messages and prints them to the console.
 *
 * \author
 *         Benjamin Wiesmueller <wiesmueller@fortiss.org>
 */

#ifndef XME_PRIM_CONSOLELOGGER_H
#define XME_PRIM_CONSOLELOGGER_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/core.h"
#include "xme/core/dcc.h"
#include "xme/core/log.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_prim_consoleLogger_configStruct
 *
 * \brief  Console logger configuration structure.
 */
typedef struct
{
	// public
	bool localOnly; ///< When true will only log local messages.
	xme_log_severity_t minSeverity; ///< Minimum severity that will be logged.
	bool verbose; ///< When true additional information (component name, component id, device type, device guid) 
	              ///  about the sending component will be printed.
	// private
	xme_core_dcc_subscriptionHandle_t subscriptionHandle;
}
xme_prim_consoleLogger_configStruct_t;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
xme_core_status_t
xme_prim_consoleLogger_create(xme_prim_consoleLogger_configStruct_t* config);

xme_core_status_t
xme_prim_consoleLogger_activate(xme_prim_consoleLogger_configStruct_t* config);

void
xme_prim_consoleLogger_deactivate(xme_prim_consoleLogger_configStruct_t* config);

void
xme_prim_consoleLogger_destroy(xme_prim_consoleLogger_configStruct_t* config);

#endif // #ifndef XME_PRIM_CONSOLELOGGER_H

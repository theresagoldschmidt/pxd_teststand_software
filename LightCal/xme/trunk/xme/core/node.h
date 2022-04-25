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
 *         Node related defines.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_CORE_NODE_H
#define XME_CORE_NODE_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_core_node_nodeId_t
 *
 * \brief  Unique node identifier.
 */
typedef enum
{
	XME_CORE_NODE_INVALID_NODE_ID = 0, ///< Invalid node identifier.
	XME_CORE_NODE_LOCAL_NODE_ID = 1, ///< Node identifier for local-only communication.
	XME_CORE_NODE_MAX_NODE_ID = XME_HAL_TABLE_MAX_ROW_HANDLE ///< Largest possible node identifier.
}
xme_core_node_nodeId_t;

#endif // #ifndef XME_CORE_NODE_H

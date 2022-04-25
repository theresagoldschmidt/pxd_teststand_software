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
 *         Plugin for the interface manager to work with TCP/socket based networks.
 *
 * \author
 *         Hauke Staehle <staehle@fortiss.org>
 */

#ifndef XME_CORE_NET_PLUGIN_TCP_H
#define XME_CORE_NET_PLUGIN_TCP_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/net.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

xme_hal_net_interfaceDescr_t* xme_core_net_plugin_tcp_get_interface(void);

#endif // #ifndef XME_CORE_NET_PLUGIN_TCP_H

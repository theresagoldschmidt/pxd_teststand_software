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
 *         Ethernet driver (architecture specific part: ARMv7-M, STM32)
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 */

 // TODO: Move board specific pin definitions to BSP header! See ticket #846

#ifndef ENET_ARCH_H
#define ENET_ARCH_H

/******************************************************************************/
/***   Includes                                                            ***/
/******************************************************************************/
#include "enet.h"

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Initializes the ethernet peripheral.
 */
void ETH_driver_init(void);

void Get_MAC_address (eth_mac_addr_t macAddress);
void Set_MAC_Address(eth_mac_addr_t macadd);

#endif // #ifndef ENET_ARCH_H

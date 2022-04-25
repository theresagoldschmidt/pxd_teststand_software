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
 *         LWIP tests.
 *
 * \author
 *         Mario Guma <mariosgu@yahoo.com>
 *         Simon Barner <barner@fortiss.org>
 */

#ifndef LWIP_TEST_H
#define LWIP_TEST_H
/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "lwip/udp.h"
 
/******************************************************************************/
/***   Prototyes                                                            ***/
/******************************************************************************/
int received;
/// Raw API Initialize
struct udp_pcb* raw_init_udp();

/// Raw API Send
void raw_send_udp(struct udp_pcb *pcb, char msg[]);

void udp_echo_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct ip_addr *addr, u16_t port);

/// Raw API Receive
void raw_receive_udp(struct udp_pcb *pcb);

#endif // #ifndef LWIP_TEST_H

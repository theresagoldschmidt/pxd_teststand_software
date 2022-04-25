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

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define IPCADDR0 192
#define IPCADDR1 168
#define IPCADDR2 17
#define IPCADDR3 145

#define IPDADDR0 192
#define IPDADDR1 168
#define IPDADDR2 17
#define IPDADDR3 7

#define CLIENT_PORT_NUM 69

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <string.h>
#include "lwip_test.h"
#include "bsp/pins_custom.h"
#include "comp_led.h"
#include "xme/hal/dio.h"


/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
//Raw API Initialize
struct udp_pcb* raw_init_udp()
{
	received = 0;
	struct udp_pcb* ptel_pcb;
	ptel_pcb = udp_new();

	xme_hal_dio_led_init(XME_BSP_LED_CUSTOM_1_CLOCK, XME_BSP_LED_CUSTOM_1_PORT, XME_BSP_LED_CUSTOM_1_PIN);
	xme_hal_dio_led_init(XME_BSP_LED_CUSTOM_2_CLOCK, XME_BSP_LED_CUSTOM_2_PORT, XME_BSP_LED_CUSTOM_2_PIN);

	if (ptel_pcb == NULL) {
		LWIP_DEBUGF(UDP_DEBUG, ("udp_new failed!\n"));
	}
	return ptel_pcb;
}


//Raw API Send (MCU-->PC)
void raw_send_udp(struct udp_pcb *pcb, char msg[])
{
	struct pbuf *p;
	//Allocate packet buffer
	p = pbuf_alloc(PBUF_TRANSPORT, sizeof(msg), PBUF_RAM);

	// Copy the original data buffer over to the packet buffer's payload
	memcpy(p->payload, msg, sizeof(msg));

	struct ip_addr dest;
	IP4_ADDR(&dest, IPDADDR0, IPDADDR1, IPDADDR2, IPDADDR3);
	udp_sendto(pcb, p, &dest, CLIENT_PORT_NUM);
	//udp_sendto(pcb, p, '192.168.17.7', 69);

	//Turn LED 2 ON which indicates that we sent smth
	xme_hal_dio_led_toggle(XME_BSP_LED_1_PORT, XME_BSP_LED_1_PIN);

	pbuf_free(p); //De-allocate packet buffer
	//Delay( 200 ); //some delay!
}


void udp_echo_recv(void *arg, struct udp_pcb *pcb, struct pbuf *p, struct ip_addr *addr, u16_t port)
{

	if (p != NULL)
	{
	   received = 1;
	   struct ip_addr dest;

	   IP4_ADDR(&dest, IPDADDR0, IPDADDR1, IPDADDR2, IPDADDR3);
	   udp_sendto(pcb, p, &dest, 69); //dest port
	   pbuf_free(p);

	   xme_hal_dio_led_toggle(XME_BSP_LED_2_PORT, XME_BSP_LED_2_PIN);
   }
}


//Raw API Receive
void raw_receive_udp(struct udp_pcb *pcb)
{
    if (udp_bind(pcb, IP_ADDR_ANY, 69) != ERR_OK)
		{
		 LWIP_DEBUGF(UDP_DEBUG, ("udp_bind failed!\n"));
		}
    udp_recv(pcb, udp_echo_recv, NULL);
}

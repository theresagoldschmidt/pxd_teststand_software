/*
 * Copyright (c) 2011 fortiss GmbH
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is contributed to the lwIP TCP/IP stack.
 *
 */

 /**
 * \file
 *         Basic timing functionality for bare-metal use of LWIP.
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 */
 
 #ifndef LWIP_PORT_STM32_ARCH_BAREMETAL_TIMER_H
 #define LWIP_PORT_STM32_ARCH_BAREMETAL_TIMER_H
/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/***   Protoypes                                                            ***/
/******************************************************************************/
/// Initializes timer module
void lwip_timer_init (void);

/// Timer update function to be invoked in SysTick interrupt handler
void lwip_timer_update(void);

/// Return current time
/// @return Return current time
uint32_t lwip_timer_getCurrentTime(void);

/// Simple delay function (busy-waiting)
/// @param nCount: Number of 10 msec units to wait
void lwip_timer_delay(uint32_t nCount);

#endif // #ifndef LWIP_PORT_STM32_ARCH_BAREMETAL_TIMER_H

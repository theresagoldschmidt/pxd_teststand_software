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
 
/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define SYSTEMTICK_PERIOD_MS  10

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#include "timer.h"
#include "stm32f10x_rcc.h"

/******************************************************************************/
/***   Variables                                                            ***/
/******************************************************************************/
/* this variable is used to create a time reference incremented by 10ms */
uint32_t currentTime = 0;
uint32_t timingdelay;

void lwip_timer_init (void)
{
	RCC_ClocksTypeDef RCC_Clocks;

	/* SystTick configuration: an interrupt every 10ms */
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
}

// Needs to be invoked in SysTick interrupt handler
void lwip_timer_update(void)
{
	currentTime += SYSTEMTICK_PERIOD_MS;
}

uint32_t lwip_timer_getCurrentTime(void)
{
	return currentTime;
}

void lwip_timer_delay(uint32_t nCount)
{
	/* Capture the current local time */
	timingdelay = currentTime + nCount;

	/* wait until the desired delay finish */
	while(timingdelay > currentTime)
	{
	}
}

//*****************************************************************************
//
// lwiplib.h - Prototypes for the lwIP library wrapper API.
//
// Copyright (c) 2008-2011 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 6852 of the Stellaris Firmware Development Package.
//
//*****************************************************************************

#ifndef __LWIPLIB_H__
#define __LWIPLIB_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
// lwIP Options
//
//*****************************************************************************
// Managed by xme_build_option()
#include "lwip/opt.h"
#include "bsp/bsp_opt.h"
#include "network_settings.h"

//*****************************************************************************
//
// lwIP API Header Files
//
//*****************************************************************************
#include "lwip/api.h"
#include "lwip/netifapi.h"
#include "lwip/tcp.h"
#include "lwip/udp.h"
#include "lwip/tcpip.h"
#include "lwip/sockets.h"
#include "lwip/mem.h"
#include "lwip/stats.h"

//*****************************************************************************
//
// IP Address Acquisition Modes
//
//*****************************************************************************
#define IPADDR_USE_STATIC       0
#define IPADDR_USE_DHCP         1
#define IPADDR_USE_AUTOIP       2

//*****************************************************************************
//
// lwIP Abstraction Layer API
//
//*****************************************************************************
extern void lwIPInit(uint8_t *pucMac, unsigned long ulIPAddr,
                     unsigned long ulNetMask, unsigned long ulGWAddr,
                     unsigned long ulIPMode);
extern void lwIPTimer(unsigned long ulTimeMS);
extern unsigned long lwIPLocalIPAddrGet(void);
extern unsigned long lwIPLocalNetMaskGet(void);
extern unsigned long lwIPLocalGWAddrGet(void);
extern void lwIPLocalMACGet(unsigned char *pucMac);
extern void lwIPNetworkConfigChange(unsigned long ulIPAddr,
                                    unsigned long ulNetMask,
                                    unsigned long ulGWAddr,
                                    unsigned long ulIPMode);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __LWIPLIB_H__

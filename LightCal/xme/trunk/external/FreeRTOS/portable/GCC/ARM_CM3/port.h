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
 *  Contributed to the FreeRTOS distribution.
 *
 *  FreeRTOS is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License (version 2) as published by the
 *  Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
 *  >>>NOTE<<< The modification to the GPL is included to allow you to
 *  distribute a combined work that includes FreeRTOS without being obliged to
 *  provide the source code for proprietary components outside of the FreeRTOS
 *  kernel.  FreeRTOS is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details. You should have received a copy of the GNU General Public
 *  License and the FreeRTOS license exception along with FreeRTOS; if not it
 *  can be viewed here: http://www.freertos.org/a00114.html and also obtained
 *  by writing to Richard Barry, contact details for whom are available on the
 *  FreeRTOS WEB site.
 */
#ifndef PORT_H
#define PORT_H

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/*
 * Exception handlers.
 */
#ifdef XME_PLATFORM_FREERTOS
void PendSV_Handler( void ) __attribute__ (( naked ));
void SysTick_Handler(void);
void SVC_Handler( void ) __attribute__ (( naked ));
#else
void xPortPendSVHandler( void ) __attribute__ (( naked ));
void xPortSysTickHandler( void );
void vPortSVCHandler( void ) __attribute__ (( naked ));
#endif // #ifdef XME_PLATFORM_FREERTOS
#endif // #ifndef PORT_H

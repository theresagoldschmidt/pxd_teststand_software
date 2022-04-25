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
 *         Generic header for display abstraction. Board specific implementation.
 *
 * \author
 *         Hauke Staehle <staehle@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdint.h>
#include "xme/hal/display.h"
#include "drivers/rit128x96x4.h"

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/

void
xme_hal_display_init(void)
{
#ifdef XME_PLATFORM_FREERTOS
	vPortEnterCritical();
#endif
	RIT128x96x4Init(1000000);
	RIT128x96x4DisplayOn();
#ifdef XME_PLATFORM_FREERTOS
	vPortExitCritical();
#endif
}

void
xme_hal_display_on(void)
{
#ifdef XME_PLATFORM_FREERTOS
	vPortEnterCritical();
#endif
	RIT128x96x4Enable(1000000);
#ifdef XME_PLATFORM_FREERTOS
	vPortExitCritical();
#endif
}

void
xme_hal_display_off(void)
{
#ifdef XME_PLATFORM_FREERTOS
	vPortEnterCritical();
#endif
	RIT128x96x4DisplayOff();
#ifdef XME_PLATFORM_FREERTOS
	vPortExitCritical();
#endif
}
void
xme_hal_display_clear(void)
{
#ifdef XME_PLATFORM_FREERTOS
	vPortEnterCritical();
#endif
	RIT128x96x4Clear();
#ifdef XME_PLATFORM_FREERTOS
	vPortExitCritical();
#endif
}

void
xme_hal_display_char(char c, uint16_t pos_x, uint16_t pos_y)
{
	char buf[2];

	buf[0] = c;
	buf[1] = '\0';
#ifdef XME_PLATFORM_FREERTOS
	vPortEnterCritical();
#endif
	RIT128x96x4StringDraw(buf, pos_x, pos_y, 15);
#ifdef XME_PLATFORM_FREERTOS
	vPortExitCritical();
#endif
}

void
xme_hal_display_bitmap(
		char *d,
		uint16_t pos_x,
		uint16_t pos_y,
		uint16_t size_x,
		uint16_t size_y,
		xme_hal_display_bitmap_t t)
{
#ifdef XME_PLATFORM_FREERTOS
	vPortEnterCritical();
#endif
	RIT128x96x4ImageDraw(d, pos_x, pos_y, size_x, size_y);
#ifdef XME_PLATFORM_FREERTOS
	vPortExitCritical();
#endif
}

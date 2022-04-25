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
 *         Shared memory abstraction (architecture specific part: generic OS
 *         based implementation).
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Stephan Sommer <sommer@fortiss.org>
 */

#ifndef XME_HAL_SHM_ARCH_H
#define XME_HAL_SHM_ARCH_H

#ifndef XME_HAL_SHAREDPTR_H
	#error This architecture-specific header file should not be included directly. Include the generic header file (usually without "_arch" suffix) instead.
#endif // #ifndef XME_HAL_SHAREDPTR_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdint.h>

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/

// Number of elements in the data buffer hosted by the broker.
#define XME_CORE_BROKER_PACKET_BUFFER 50

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/

typedef struct
{
	void* slot_data; ///< Pointer to memory area.
	uint16_t size; ///< Size of allocated memory area.
	xme_hal_sharedPtr_referenceCount_t referenceCount; ///< Reference count.
}
xme_hal_sharedPtr_bufferRow_t;

// TODO: change this to a efficient implementation!! See ticket #823
extern xme_hal_sharedPtr_bufferRow_t xme_hal_sharedPtr_buffer[XME_CORE_BROKER_PACKET_BUFFER];

#endif // #ifndef XME_HAL_SHM_ARCH_H

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
 *         Digital output pin.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_PRIM_DIGITALOUTPUT_H
#define XME_PRIM_DIGITALOUTPUT_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/mdl.h"
#include "xme/hal/io.h"

/******************************************************************************/
/***   Model constraints                                                    ***/
/******************************************************************************/
XME_MDL_RESOURCE_DEPENDENCY()

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_prim_digitalOutput_configStruct
 *
 * \brief  Digital output configuration structure.
 */
typedef struct
{
	xme_hal_io_portPin pin;
}
xme_prim_digitalOutput_configStruct;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
void
xme_prim_digitalOutput_create(xme_prim_digitalOutput_configStruct* config);

void
xme_prim_digitalOutput_activate(xme_prim_digitalOutput_configStruct* config);

void
xme_prim_digitalOutput_deactivate(xme_prim_digitalOutput_configStruct* config);

void
xme_prim_digitalOutput_destroy(xme_prim_digitalOutput_configStruct* config);

#endif // #ifndef XME_PRIM_DIGITALOUTPUT_H

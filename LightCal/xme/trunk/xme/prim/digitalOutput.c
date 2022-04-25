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

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/prim/digitalOutput.h"

#include "xme/hal/dio.h"

/******************************************************************************/
/***   Forward declarations                                                 ***/
/******************************************************************************/
XME_SUBSCRIPTION
xme_prim_digitalOutput_onBoolean(void* data);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
void
xme_prim_digitalOutput_create(xme_prim_digitalOutput_configStruct* config)
{
	xme_core_read("Boolean", xme_prim_digitalOutput_onBoolean, config);
}

void
xme_prim_digitalOutput_activate(xme_prim_digitalOutput_configStruct* config)
{
	xme_hal_dio_configurePin(config->pin, XME_HAL_DIO_PIN_OUPUT);
}

void
xme_prim_digitalOutput_deactivate(xme_prim_digitalOutput_configStruct* config)
{
	// Switch back to default configuration.
	// Note: If input is not always the default configuration, introduce an
	// XME_HAL_DIO_PIN_RESET flag or a separate HAL function for this.
	xme_hal_dio_setDir(config->pin, XME_HAL_DIO_PIN_INPUT);
}

void
xme_prim_digitalOutput_destroy(xme_prim_digitalOutput_configStruct* config)
{
}

XME_SUBSCRIPTION
xme_prim_digitalOutput_onBoolean(void* data, void* userData)
{
	bool* value = (bool*)data;
	xme_hal_dio_setPin(config->..., *value);
}

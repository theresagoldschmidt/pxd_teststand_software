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
 *         Component Generator example project.
 *
 *         Generates code templates for components and places them at the
 *         appropriate locations in the build system.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "templateGeneratorComponent.h"
#include "xme/core/componentList.h"

/******************************************************************************/
/***   Component configurations                                             ***/
/******************************************************************************/
//XME_COMPONENT_CONFIG_INSTANCE(xme_core_nodeManager) =
//{
//	0x00000000021041A1, // deviceType
//	XME_CORE_DEVICE_GUID_RANDOM // deviceGuid
//};

XME_COMPONENT_CONFIG_INSTANCE(templateGeneratorComponent, 1);

/******************************************************************************/
/***   Component descriptor                                                 ***/
/******************************************************************************/
XME_COMPONENT_LIST_BEGIN
//	XME_COMPONENT_LIST_ITEM(xme_core_nodeManager, 0)
	XME_COMPONENT_LIST_ITEM(templateGeneratorComponent, 0)
XME_COMPONENT_LIST_END;

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
int
main(int argc, char* argv[])
{
	XME_CHECK_MSG
	(
		XME_CORE_STATUS_SUCCESS == xme_core_init(),
		1,
		XME_LOG_FATAL,
		"Fatal error during XME initialization!\n"
	);

	xme_core_run();

	xme_core_fini();

	return 0;
}

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
 *         Software component abstraction.
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_CORE_COMPONENTLIST_H
#define XME_CORE_COMPONENTLIST_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/component.h"
#include "xme/core/resourceManager.h"
#include "xme/core/directory.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
// TODO (See ticket #745): Implicitely add directory to component list as follows
//       (currently, this produces an inclusion order problem):
#ifdef XME_CORE_DIRECTORY_TYPE_LOCAL
	#define XME_COMPONENT_LIST_BEGIN \
		XME_COMPONENT_CONFIG_INSTANCE(xme_core_directory) = \
		{ \
			/* public */ \
			XME_CORE_DATACHANNEL_MASK, /* baseDataChannel */ \
			XME_CORE_DATACHANNEL_MASK /* dataChannelMask */ \
		}; \
		\
		xme_core_componentDescriptor_t xme_core_resourceManager_componentDescriptors[] = \
		{ \
			XME_COMPONENT_LIST_ITEM(xme_core_directory, 0) \
			XME_COMPONENT_LIST_ITEM_NO_CONFIG(xme_core_resourceManager)
			// TODO: Add resource manager as well. See ticket #745
#else // #ifdef XME_CORE_DIRECTORY_TYPE_LOCAL
	#define XME_COMPONENT_LIST_BEGIN \
		XME_COMPONENT_CONFIG_INSTANCE(xme_core_directory) = \
		{ \
			/* public */ \
			(xme_core_dataChannel_t)0, /* baseDataChannel */ \
			XME_CORE_DATACHANNEL_MASK /* dataChannelMask */ \
		}; \
		\
		xme_core_componentDescriptor_t xme_core_resourceManager_componentDescriptors[] = \
		{ \
			XME_COMPONENT_LIST_ITEM(xme_core_directory, 0) \
			XME_COMPONENT_LIST_ITEM_NO_CONFIG(xme_core_resourceManager)
			// TODO: Add resource manager as well. See ticket #745
#endif // #ifdef XME_CORE_DIRECTORY_TYPE_LOCAL

#define XME_COMPONENT_LIST_ITEM(componentName, configIndex) \
		{ \
			(xme_core_componentCreationCallback_t)&componentName##_create, \
			(xme_core_componentActivationCallback_t)&componentName##_activate, \
			(xme_core_componentDeactivationCallback_t)&componentName##_deactivate, \
			(xme_core_componentDestructionCallback_t)&componentName##_destroy, \
			(xme_core_componentConfigStruct_t*)&componentName##_config[configIndex], \
			XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT, \
			#componentName \
		},

#define XME_COMPONENT_LIST_ITEM_NO_CONFIG(componentName) \
		{ \
			(xme_core_componentCreationCallback_t)&componentName##_create, \
			(xme_core_componentActivationCallback_t)&componentName##_activate, \
			(xme_core_componentDeactivationCallback_t)&componentName##_deactivate, \
			(xme_core_componentDestructionCallback_t)&componentName##_destroy, \
			(xme_core_componentConfigStruct_t*)NULL, \
			XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT, \
			#componentName \
		},

#define XME_COMPONENT_LIST_ITEM_MANUAL(componentName, createCallback, activateCallback, deactivateCallback, destroyCallback, config) \
		{ \
			(xme_core_componentCreationCallback_t)createCallback, \
			(xme_core_componentActivationCallback_t)activateCallback, \
			(xme_core_componentDeactivationCallback_t)deactivateCallback, \
			(xme_core_componentDestructionCallback_t)destroyCallback, \
			(xme_core_componentConfigStruct_t*)config, \
			XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT, \
			#componentName \
		},

#define XME_COMPONENT_LIST_EMPTY /* nothing */

#define XME_COMPONENT_LIST_END \
	}; \
	xme_core_component_t xme_core_resourceManager_componentCount = (xme_core_component_t)(sizeof(xme_core_resourceManager_componentDescriptors)/sizeof(xme_core_resourceManager_componentDescriptors[0]))

#endif // #ifndef XME_CORE_COMPONENTLIST_H

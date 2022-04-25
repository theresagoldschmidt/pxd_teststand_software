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

#ifndef XME_CORE_COMPONENT_H
#define XME_CORE_COMPONENT_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/core.h"

#include "xme/hal/table.h"

#include <stdint.h>

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
// TODO (See ticket #803): Remove these defines and force correct order in component list (i.e., 1. directory, 2. resource manager)
#define XME_CORE_COMPONENT_DIRECTORY_COMPONENT_ID ((xme_core_component_t)0x0001) ///< Component ID of directory.
#define XME_CORE_COMPONENT_RESOURCEMANAGER_COMPONENT_ID ((xme_core_component_t)0x0002) ///< Component ID of resource manager.

/**
 * \def    XME_COMPONENT_CONFIG_STRUCT
 *
 * \brief  Defines a configuration structure type for a component type with
 *         the given name.
 *
 *         The configuration structure type will be named
 *         (componentName)_configStruct_t.
 *         Using this macro ensures that naming conventions are enforced.
 *
 * \param  componentTypeName Full name of the component type the configuration
 *         structure is to be defined for (e.g., xme_adv_myComponent).
 * \param  members List of members (with data type) in the configuration
 *         structure, separated by semicola.
 */
#define XME_COMPONENT_CONFIG_STRUCT(componentTypeName, members) \
	typedef struct \
	{ \
		members \
	} \
	componentTypeName##_configStruct_t

/**
 * \def    XME_COMPONENT_CONFIG
 *
 * \brief  Generates the name of the component configuration array for the
 *         given component type.
 *
 * \param  componentTypeName Full name of the component the configuration structure
 *         is defined for (e.g., xme_adv_myComponent).
 */
#define XME_COMPONENT_CONFIG(componentTypeName) \
	componentTypeName##_config

/**
 * \def    XME_COMPONENT_CONFIG_INSTANCE
 *
 * \brief  Defines an array of component configurations for the given
 *         component type.
 *
 *         The component configuration array type will be named
 *         (componentName)_config and will be of type
 *         (componentName)_configStruct_t.
 *
 *         Usage examples:
 *         \code
 *         // In case you want to initialize at least one member of the
 *         // configuration structure, use the following syntax
 *         // (assuming xme_adv_myComponent has at least two public
 *         // configuration variables, the first one being a string and
 *         // the second one being a number):
 *         XME_COMPONENT_CONFIG_INSTANCE(xme_adv_myComponent) =
 *         {
 *         	// Initial configuration of first instance of xme_adv_myComponent
 *         	"myComponent1",
 *         	42
 *         },
 *         {
 *         	// Initial configuration of second instance of xme_adv_myComponent
 *         	"myComponent2",
 *         	43
 *         };
 *         \endcode
 *
 *         \code
 *         // In case the component does not contain any members that should
 *         // be initialized (assuming two instance of xme_adv_myComponent
 *         // should exist):
 *         XME_COMPONENT_CONFIG_INSTANCE(xme_adv_myComponent, 2);
 *         \endcode
 *
 * \param  componentTypeName Full name of the component type the configuration
 *         array is to be defined for (e.g., xme_adv_myComponent).
 * \param  ... Optional parameter that specifies the number of configuration
 *         instances to create in case the configuration instances do not
 *         contain any fields that should be initialized. In this case, the
 *         compiler can not statically determine the number of elements,
 *         which is why their number has to be explicitly specified here.
 *         This value can also be specified when the configuration instances
 *         are initialized explicitly, in which case it must match the
 *         number of explicit configuration instances initializations that
 *         follow this macro. See examples section.
 */
#define XME_COMPONENT_CONFIG_INSTANCE(componentTypeName, ...) \
	static \
	componentTypeName##_configStruct_t XME_COMPONENT_CONFIG(componentTypeName)[__VA_ARGS__]

/**
 * \def    XME_COMPONENT_CONTEXT
 *
 * \brief  Switches the component context temporarily to the one of the
 *         specified component and executes the code specified in body.
 *
 *         This macro must be called directly or indirectly when a component
 *         is called from the runtime system or from the execution context of
 *         another component. That component may then in turn call the runtime
 *         system. This macro ensures that in this case the runtime system can
 *         reliably detect which component made the specific call.
 *
 * \param  componentId Identifier of the component to use as context for
 *         evaluating the code in body.
 * \param  body Code to execute within the component context componentId.
 *         The code made directly or indirectly call functions from the
 *         runtime system and the runtime system will be aware that these
 *         calls are associated to the component componentId.
 */
#define XME_COMPONENT_CONTEXT(componentId, body) \
	do { \
		xme_core_component_t oldComponentId = xme_core_resourceManager_getCurrentComponentId(); \
		xme_core_resourceManager_setCurrentComponentId(componentId); \
		{ \
			body \
		} \
		xme_core_resourceManager_setCurrentComponentId(oldComponentId); \
	} while (0)

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_core_component_t
 *
 * \brief  Locally valid identifier of a component.
 */
typedef enum
{
	XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT = 0, ///< Invalid component context.
	XME_CORE_COMPONENT_MAX_COMPONENT_CONTEXT = XME_HAL_TABLE_MAX_ROW_HANDLE ///< Largest possible component context.
}
xme_core_component_t;

/**
 * \typedef xme_core_component_port_t
 *
 * \brief  Locally valid identifier of a component port.
 */
typedef enum
{
	XME_CORE_COMPONENT_INVALID_COMPONENT_PORT = 0, ///< Invalid component port.
	XME_CORE_COMPONENT_MAX_COMPONENT_PORT = XME_HAL_TABLE_MAX_ROW_HANDLE ///< Largest possible component port.
}
xme_core_component_port_t;

// TODOC. See ticket #722
typedef enum
{
	XME_CORE_COMPONENT_PORTTYPE_INVALID = 0,
	XME_CORE_COMPONENT_PORTTYPE_DCC_PUBLICATION = 1, // output port
	XME_CORE_COMPONENT_PORTTYPE_DCC_SUBSCRIPTION, // input port
	XME_CORE_COMPONENT_PORTTYPE_RR_REQUEST_SENDER = 8, // output port
	XME_CORE_COMPONENT_PORTTYPE_RR_REQUEST_HANDLER, // input port
	XME_CORE_COMPONENT_PORTTYPE_RR_RESPONSE_SENDER, // output port
	XME_CORE_COMPONENT_PORTTYPE_RR_RESPONSE_HANDLER // input port
}
xme_core_component_portType_t;

/**
 * \typedef xme_core_componentConfigStruct_t
 *
 * \brief  Component configuration structure type (dereferenced).
 */
typedef void xme_core_componentConfigStruct_t;

/**
 * \typedef xme_core_componentCreationCallback_t
 *
 * \brief  Callback function for software component creation.
 */
typedef xme_core_status_t (*xme_core_componentCreationCallback_t) (xme_core_componentConfigStruct_t* config);

/**
 * \typedef xme_core_componentActivationCallback_t
 *
 * \brief  Callback function for software component activation and
 *         deactivation.
 */
typedef xme_core_status_t (*xme_core_componentActivationCallback_t) (xme_core_componentConfigStruct_t* config);

/**
 * \typedef xme_core_componentDeactivationCallback_t
 *
 * \brief  Callback function for software component activation and
 *         deactivation.
 */
typedef void (*xme_core_componentDeactivationCallback_t) (xme_core_componentConfigStruct_t* config);

/**
 * \typedef xme_core_componentDestructionCallback_t
 *
 * \brief  Callback function for software component destruction.
 */
typedef void (*xme_core_componentDestructionCallback_t) (xme_core_componentConfigStruct_t* config);

/**
 * \typedef xme_core_componentDescriptor
 *
 * \brief  Software component descriptor.
 */
typedef struct
{
	xme_core_componentCreationCallback_t create;
	xme_core_componentActivationCallback_t activate;
	xme_core_componentDeactivationCallback_t deactivate;
	xme_core_componentDestructionCallback_t destroy;
	xme_core_componentConfigStruct_t* config;
	xme_core_component_t componentId;
	const char* componentName;
}
xme_core_componentDescriptor_t;

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
extern xme_core_componentDescriptor_t xme_core_resourceManager_componentDescriptors[];

#endif // #ifndef XME_CORE_COMPONENT_H

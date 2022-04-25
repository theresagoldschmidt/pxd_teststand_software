/*
 * Copyright (c) 2011, fortiss GmbH.
 * All rights reserved.
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
 *         Chat example project.
 *
 * \author
 *		   Hauke Stähle <staehle@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/componentList.h"
#include "xme/core/core.h"
#include "xme/core/defines.h"
#include "xme/core/directory.h"
#include "xme/core/nodeManager.h"
#include "xme/core/interfaceManager.h"
#include "xme/core/broker.h"
#include "belle_II_GUI_XME_Component.h"
#include "xme/adv/heartbeat.h"

//#include <QtGui/QApplication>
//#include "mainwindow.h"
//#include "guiHeader.h"


/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define XME_CORE_NODEMANAGER_DEVICE_TYPE ((xme_core_device_type_t)0x0000000000111112)
#define XME_CORE_NODEMANAGER_DEVICE_GUID ((xme_core_device_guid_t)0x0000000000211114)

/******************************************************************************/
/***   Component configurations                                             ***/
/******************************************************************************/

XME_COMPONENT_CONFIG_INSTANCE(xme_core_nodeManager) =
{
	// public
	XME_CORE_NODEMANAGER_DEVICE_TYPE, // deviceType
	XME_CORE_NODEMANAGER_DEVICE_GUID // deviceGuid
};

XME_COMPONENT_CONFIG_INSTANCE(xme_adv_heartbeat) =
{
	(xme_hal_time_handle_t)400
};

/******************************************************************************/
/***   Component descriptor                                                 ***/
/******************************************************************************/
XME_COMPONENT_LIST_BEGIN
	XME_COMPONENT_LIST_ITEM(xme_core_nodeManager, 0)
	XME_COMPONENT_LIST_ITEM_NO_CONFIG(belle_II_GUI_XME_Component)
	XME_COMPONENT_LIST_ITEM(xme_adv_heartbeat, 0)
XME_COMPONENT_LIST_END;
/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
/*! \mainpage Belle II Project Documentation
 *
 *			The Belle II firmware project, GUI project and all documentation (datasheets, presentations,
 *			design schematics etc.) related to the Belle II project can be checked out from the
 *			https://orion.fortiss.org/svn/FB2_BelleII/trunk repository. The https://orion.fortiss.org/svn/FB2_BelleII/trunk/src
 *			folder contains the source code for the Power Supply (PS) firmware and GUI. The Belle II 
 *			project documentation can be found in the https://orion.fortiss.org/svn/FB2_BelleII/trunk/docs folder.
 *
 * \section gui_sec GUI project documentation
 *
 * 			The GUI code can be found in the 'gui' directory (https://orion.fortiss.org/svn/FB2_BelleII/trunk/src/gui/). 
 *			The folder contains source code files that initialize the communication with the PS nodes and all windows (main, calibration, monitoring windows).
 *
 * \section dev_env	 Development environment
 *
 *			In order to be able to build the Qt based GUI (Slow Control) Microsoft Visual C++ Express 
 *			(http://www.microsoft.com/visualstudio/en-us/products/2010-editions/visual-cpp-express) and QT needs to be downloaded. 
 *			QT 4.7.4 is recomended to be used.
 *			After checking out the 'gui' project (also in the Belle repository), Cmake has to be run in order to build the system. 
 *			At the end Microsoft Visual C++ Express has to be opened and the program has to be compiled and started.
 *
 * \section files	Files and their description
 *
 * 			Detailed description of all files (which functions and variables it contains) can be found in this documentation when
 * 			the 'Files' tab is opened.Moreover the code is commented in such a way to be easy understandable for the user.
 *			
 *			Important notes:  
 *			
 *			1. Function that initializes Chromosome and starts the GUI application (the main window) can be found in the belle_II_GUI_XME.c file.
 *			The belle_II_GUI_XME_Component.c file contains the component that enables the GUI to communicate with the PS nodes.
 *
 *			2. The mapping between MAC Address and Node number is done as following. A file 
*			named MAC_Addresses.csv that contains the MAC addresses	of the nodes and their numbers
 *			(example of such a file can be found under ..svn/docs/implementation specification) has to be placed in the
 *			home directory/Belle II directory (e.g. C:/Users/XYZ/BelleII). This file exists on GUI side and is read
 *			when the GU is initialized. Commands (Start, Stop etc.) and data for calibration etc. are sent to a 
 *			certain PS node by providing the node number. Moreover, the node number corresponds to the last byte
 *			of the MAC address that is assigned to the node. On the PS node side the MAC address (compiled in the firmware)
 *			is provided at the very beginning when the bootloader is started and it is stored on the first page 
 *			(byte 2-8) of the EEPROM (on this page in addition the firmware version is stored on the first byte position).
 *
 *			3. The log data (containing the logging information of the PS nodes) are stored in a file named 'log.csv' which is created once in
 *			a folder BelleII (also is created in addtion) in the home directory.
 *
 * \section xme_section Chromosome overview
 *
 *			CHROMOSOME (often abbreviated by 'XME') is a domain-independent, data-centric middle-ware
 *			for cyber-physical systems that has been used in the Belle II project in order to enable communication
 *			between the PS nodes and the GUI (in future also the DAQ). CHROMOSOME is based on the very powerful
 *			concept of data-centric	design. Components specify which data they consume and produce and based on this information,
 *			the middleware calculates the communication routes. It is important to note that CHROMOSOME
 *			does not broadcast the data, instead it calculates specific routes based on the requirements.
 *			The routes are calculated only when components are added or removed.
 *
 *			Data is grouped by so-called topics. A topic is a data type with a certain structure. Examples for
 *			topics are command, logdata etc. (can be found in belle_II_PS_node_XME_Component.c). This enables
 *			the exchange of data between different applications.
 *
 *			Important notes:
 *			
 *			1. The description below contains example code that is part of the Belle II 'firmware' project. 
 *			2. The belle_II_GUI_XME_Component.c file contains the component that enables the GUI to communicate with the PS nodes
 *			via the Chromosome middleware.
 *
 * \subsection comp_def Component Definition
 *
 * 			A CHROMOSOME component is defined by four functions to create, activate, deactivate and destroy it, illustrated
 * 			here for a component called belle_network_connection_Component:
 *
 * 			belle_network_connection_Component_create(belle_network_connection_Component_configStruct_t* config);
 * 			belle_network_connection_Component_create(belle_network_connection_Component_configStruct_t* config);
 * 			belle_network_connection_Component_activate(belle_network_connection_Component_configStruct_t* config);
 * 			belle_network_connection_Component_deactivate(belle_network_connection_Component_configStruct_t* config);
 *
 * 			The distinction between creation/activation and deactivation/destruction is required in case a
 * 			component needs to be migrated: in this case, the component is first deactivated (to obtain a consistent state),
 * 			then moved and activated at the new location. All four functions take a pointer to a configuration structure, which
 * 			represents the state of the respective component instance. Multiple component instances of the same component
 * 			type usually have their own configuration structures. A typical task during creation of a component is
 * 			registration of data demand and data production.
 *
 * \subsection comp_inst Component Instantiation
 *
 * 			Components can be instantiated by adding them to the so-called component list, usually defined
 * 			in the main program file. Mandatory core components are implicitly added to the list. An initial
 * 			configuration may be provided for each component. A sample component list with respective configurations is shown below:
 *
 *			XME_COMPONENT_CONFIG_INSTANCE(xme_core_nodeManager) =
 *			{
 *				// public
 *				XME_CORE_NODEMANAGER_DEVICE_TYPE, // deviceType
 *				XME_CORE_NODEMANAGER_DEVICE_GUID // deviceGuid
 *			};
 *
 *			XME_COMPONENT_CONFIG_INSTANCE(belle_network_connection_Component) =
 *			{
 *				(xme_hal_time_handle_t)10000, // deadline for heartbeats in ms
 *				true // catch heartbeats from all GUIDs
 *			};
 *
 *			XME_COMPONENT_LIST_BEGIN
 *				XME_COMPONENT_LIST_ITEM(xme_core_nodeManager, 0)
 *				XME_COMPONENT_LIST_ITEM_NO_CONFIG(belle_II_PS_node_XME_Component)
 *				XME_COMPONENT_LIST_ITEM(belle_network_connection_Component, 0)
 *			XME_COMPONENT_LIST_END;
 *
 *			This code section can be found in the main.c and it shows that three components (besides internal core components),
 *			namely	xme_core_nodeManager, belle_II_PS_node_XME_Component and belle_network_connection_Component, will be
 *			present on this node.
 *
 * \subsection send_receive_data Sending and Receiving Data
 *
 * 			Before sending data, a component states its intent to the runtime system. This is necessary
 * 			to allow the appropriate communication routes to be set up and is usually performed from within
 * 			a component's create function. For using data centric communication, include the file 'xme/core/dcc.h'.
 * 			To inform CHROMOSOME that a component intends (e.g. belle_II_PS_node_XME_Component) to send data under
 * 			a specific topic, call:
 *
 *			pubHandleMonitor =
 *					xme_core_dcc_publishTopic(
 *					MONITOR_MESSAGE_TOPIC_ID, XME_CORE_MD_EMPTY_META_DATA, NULL
 *					);
 *			The actual sending of data is performed with:
 *
 *			xme_core_dcc_sendTopicData(pubHandleMonitor, &data, sizeof(data));
 *
 *			A component can subscribe to a certain topic with a given receiveTopicCallback function by calling:
 *
 *			subHandleCommand =
 *					xme_core_dcc_subscribeTopic(
 *					COMMAND_TOPIC_ID, XME_CORE_MD_EMPTY_META_DATA, command_subscription, NULL
 *					);
 *
 *			Whenever data matching the given topic is received, the given callback function is invoked.
 *			command_subscription must have a signature matching xme_core_dcc_receiveTopicCallback_t, namely:
 *
 *			void command_subscription(xme_hal_sharedPtr_t dataHandle, void* userData);
 *
 *			dataHandle is a reference to the memory where the received data is located.
 */
/*! \brief This is the main function of the Belle II GUI project.
 *
 *  The main function starts the GUI by calling callGui(argc, argv) function and also activates Chromosome.
 */
int
main (int argc, char* argv[])
{
	callGui(argc, argv);
	
	xme_core_run();

	return 0;
}

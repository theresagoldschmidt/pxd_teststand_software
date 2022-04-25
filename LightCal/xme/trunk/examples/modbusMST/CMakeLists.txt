#
# Copyright (c) 2011-2012, fortiss GmbH.
# Licensed under the Apache License, Version 2.0.
#
# Use, modification and distribution are subject to the terms specified
# in the accompanying license file LICENSE.txt located at the root directory
# of this software distribution. A copy is available at
# http://chromosome.fortiss.org/.
#
# This file is part of CHROMOSOME.
#
# $Id$
#
# File:
#         CMake file for ModbusMST node example.
#
# Author:
#         Michael Geisinger <geisinger@fortiss.org>
#         Marcel Flesch <fleschmarcel@mytum.de>
#
# Architecture:
#         Windows
#

cmake_minimum_required(VERSION 2.6)

# Project name must not be executable name when using the Eclipse CDT generator
project(ModbusMSTNode)

# Find CHROMOSOME
set (XME_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set (CMAKE_MODULE_PATH "${XME_ROOT}/tools/cmake")
# Cannot use xme_set() since package XME has not been loaded (we're actually
# preparing to do so right now...)
set_property (GLOBAL PROPERTY XME_PLATFORM "windows")
set_property (GLOBAL PROPERTY XME_ARCHITECTURE "x86")
set (XME_TESTSUITE YES)
find_package (XME REQUIRED)

# Find other packages
#find_package (SomePackage REQUIRED)

# TODO: This is a workaround for target_link_libraries() only affecting executables
#       specified *after* the call to link_directories(). However, target_link_libraries()
#       will only be called once the respective component that depends on the third-
#       party library is linked against the executable using xme_link_components()
#       (i.e., after invocation of xme_add_executable())!
#       The current workaround is to manually find the required package and adding its
#       library path to the project. See issue #260.
# FIXME!
find_package (MySqlConnectorC REQUIRED)
link_directories (${MYSQLCONNECTORC_LIB_PATH})
find_package (Libmodbus REQUIRED)
link_directories (${LIBMODBUS_LIB_PATH})

# Add your local include and library directories here
#xme_include_directory (somedir)
#xme_link_directory (somedir)

# Setup output directories
set(
	CMAKE_LIBRARY_OUTPUT_DIRECTORY
	${PROJECT_BINARY_DIR}/target
	CACHE PATH
	"Target directory for all libraries"
)
set(
	CMAKE_RUNTIME_OUTPUT_DIRECTORY
	${PROJECT_BINARY_DIR}/target
	CACHE PATH
	"Target directory for all executables"
)
set(
	CMAKE_ARCHIVE_OUTPUT_DIRECTORY
	${PROJECT_BINARY_DIR}/target
	CACHE PATH
	"Target directory for all static libraries"
)

# Add subdirectories
#add_subdirectory (somedir)

# Add definitions
add_definitions (-DXME_CORE_DIRECTORY_TYPE_MASTER)

# Set up cross compiler

# Set up sources and headers
set(
	SOURCES
	modbusMst.c
)

set(
	HEADERS
)

# Define target
xme_add_executable(
	modbusMst
	${SOURCES}
	${HEADERS}
)

# Build XME components
xme_link_components(
	modbusMst
	xme_adv_databaseLogger
	xme_adv_loginServer
	xme_adv_mstFusion
	xme_adv_mstLogger
	xme_adv_mstManager
	xme_prim_ipLoginClientProxy
	xme_prim_modbusMaster
	xme_prim_modbusSimulator
	xme_core_core
	xme_hal_dio
	xme_hal_linkedList
	xme_hal_net
)

# Build documentation
xme_build_documentation(
	"ModbusMST Documentation"
	TARGET modbusMst_doc
	OUTPUT_DIR ""
	OUTPUT_NAME "modbusMST.chm"
	INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	#AUTO
	CLEAN
	FILES ${SOURCES} ${HEADERS}
)

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
#         CMake file for light node example.
#
# Author:
#         Simon Barner <barner@fortiss.org>
#         Michael Geisinger <geisinger@fortiss.org>
#
# Architecture:
#         Windows
#

cmake_minimum_required(VERSION 2.6)

# Project name must not be executable name when using the Eclipse CDT generator
project(LightNode)

# Find CHROMOSOME
set (XME_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")
set (CMAKE_MODULE_PATH "${XME_ROOT}/tools/cmake")
# Cannot use xme_set() since package XME has not been loaded (we're actually
# preparing to do so right now...)
set_property (GLOBAL PROPERTY XME_PLATFORM "windows")
set_property (GLOBAL PROPERTY XME_ARCHITECTURE "x86")
set (XME_TESTSUITE YES)
#set(XME_COMPONENT_DIRS ...)
find_package (XME REQUIRED)

# Find other packages
#find_package (SomePackage REQUIRED)

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
	light.c
)

set(
	HEADERS
)

# Define target
xme_add_executable(
	light
	${SOURCES}
	${HEADERS}
)

# Build XME components
xme_link_components(
	light
	xme_adv_loginServer
	xme_prim_ipLoginClientProxy
	xme_prim_randomNumberGenerator
	xme_core_core
	xme_hal_dio
	xme_hal_net
	xme_adv_heartbeat
	xme_adv_testCpu
	xme_adv_testMemory
	xme_adv_testHeartbeat
	xme_adv_testConsistency
	xme_adv_healthmonitor
	xme_adv_voter
)

# Build documentation
xme_build_documentation(
	"Light Documentation"
	TARGET light_doc
	OUTPUT_DIR ""
	OUTPUT_NAME "light.chm"
	INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	#AUTO
	CLEAN
	FILES ${SOURCES} ${HEADERS}
)

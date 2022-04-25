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

project(LightNode)

# Main project settings
set (XME_PLATFORM "windows")
set (XME_ARCHITECTURE "x86")
set (XME_TESTSUITE YES)
include ("../CMakeCommon.txt")

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
	xme_prim_consoleLogger
	xme_prim_ipLoginClientProxy
	xme_prim_randomNumberGenerator
	xme_core_core
	xme_hal_dio
	xme_hal_net
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

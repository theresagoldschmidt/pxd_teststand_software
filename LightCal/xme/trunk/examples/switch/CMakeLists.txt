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
#         CMake file for switch node example.
#
# Author:
#         Simon Barner <barner@fortiss.org>
#         Michael Geisinger <geisinger@fortiss.org>
#
# Architecture:
#         Windows
#

cmake_minimum_required(VERSION 2.6)

project(SwitchNode)

# Main project settings
set (XME_PLATFORM "windows")
set (XME_ARCHITECTURE "x86")
set (XME_TESTSUITE YES)
include ("../CMakeCommon.txt")

# Add definitions
add_definitions (-DXME_CORE_DIRECTORY_TYPE_LOCAL)

# Set up cross compiler

# Set up sources and headers
set(
	SOURCES
	switch.c
)

set(
	HEADERS
)

# Define target
xme_add_executable(
	switch
	${SOURCES}
	${HEADERS}
)

# Build XME components
xme_link_components(
	switch
	xme_prim_ipLoginServerProxy
	xme_core_core
	xme_prim_consoleLogger
	xme_prim_randomNumberGenerator
	xme_hal_dio
	xme_hal_net
)

# Build documentation
xme_build_documentation(
	"Switch Documentation"
	TARGET switch_doc
	OUTPUT_DIR ""
	OUTPUT_NAME "switch.chm"
	INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	#AUTO
	CLEAN
	FILES ${SOURCES} ${HEADERS}
)

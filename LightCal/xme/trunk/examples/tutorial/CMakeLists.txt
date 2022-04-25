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
#         CMake file for tutorial node example.
#
# Author:
#         Simon Barner <barner@fortiss.org>
#         Michael Geisinger <geisinger@fortiss.org>
#         Dominik Sojer <sojer@fortiss.org>
#
# Architecture:
#         Windows
#

cmake_minimum_required(VERSION 2.6)

project(Tutorial)

# Main project settings
set (XME_PLATFORM "windows")
set (XME_ARCHITECTURE "x86")
set (XME_TESTSUITE NO)
include ("../CMakeCommon.txt")

# Add definitions
add_definitions (-DXME_CORE_DIRECTORY_TYPE_LOCAL)

# Set up cross compiler

# Set up sources and headers
set(
	SOURCES
	tutorial.c
	chatComponent.c
	helloWorldComponent.c
)

set(
	HEADERS
	chatComponent.h
	helloWorldComponent.h
)

# Define target
xme_add_executable(
	"tutorial"
	${SOURCES}
	${HEADERS}
)

# Build XME components
xme_link_components(
	"tutorial"
	xme_core_core
	xme_prim_ipLoginServerProxy
	xme_hal_net
	xme_hal_sleep
)

# Build documentation
xme_build_documentation(
	"Tutorial"
	TARGET "tutorial_doc"
	OUTPUT_DIR ""
	OUTPUT_NAME "tutorial.chm"
	INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}
	#AUTO
	CLEAN
	FILES ${SOURCES} ${HEADERS}
)

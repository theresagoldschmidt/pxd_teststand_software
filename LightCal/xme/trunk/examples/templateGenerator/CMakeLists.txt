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
#         CMake file for template generator example.
#
# Author:
#         Michael Geisinger <geisinger@fortiss.org>
#
# Architecture:
#         Windows
#

cmake_minimum_required(VERSION 2.6)

project(TemplateGenerator)

# Main project settings
set (XME_PLATFORM "windows")
set (XME_ARCHITECTURE "x86")
set (XME_TESTSUITE NO)
include ("../CMakeCommon.txt")

# Add definitions
add_definitions (-DXME_CORE_DIRECTORY_TYPE_LOCAL)

# Set up cross compiler

# Copy templates to target directory
configure_file(
	"componentTemplate.h.in"
	"${CMAKE_BINARY_DIR}/componentTemplate.h.in"
	COPYONLY
)
configure_file(
	"componentTemplate.c.in"
	"${CMAKE_BINARY_DIR}/componentTemplate.c.in"
	COPYONLY
)
configure_file(
	"CMakeListsTemplate.txt.in"
	"${CMAKE_BINARY_DIR}/CMakeListsTemplate.txt.in"
	COPYONLY
)

# Install templates
get_property (__PLATFORM__ GLOBAL PROPERTY "XME_PLATFORM")
get_property (__ARCHITECTURE__ GLOBAL PROPERTY "XME_ARCHITECTURE")
install(
	FILES
	"componentTemplate.h.in"
	"componentTemplate.c.in"
	"CMakeListsTemplate.txt.in"
	DESTINATION
	"bin/${__PLATFORM__}_${__ARCHITECTURE__}"
)

# Set up sources and headers
set(
	SOURCES
	templateGenerator.c
	templateGeneratorComponent.c
)

set(
	HEADERS
	templateGeneratorComponent.h
)

# Define target
xme_add_executable(
	"templateGenerator"
	${SOURCES}
	${HEADERS}
)

# Build XME components
xme_link_components(
	"templateGenerator"
	xme_core_core
)

# Build documentation
#xme_build_documentation(
#	"Template Generator"
#	TARGET "templateGenerator_doc"
#	OUTPUT_DIR ""
#	OUTPUT_NAME "templateGenerator.chm"
#	INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}
#	#AUTO
#	CLEAN
#	FILES ${SOURCES} ${HEADERS}
#)

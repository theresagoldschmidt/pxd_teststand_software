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
#         CMake file for Inverted Pendulum
#
# Author:
#         Kai Huang <khuang@fortiss.org>
#         Gang Chen <gachen@fortiss.org>			
#
# Architecture:
#         NiosII
#
# Platform:
#         freeRTOS
#

cmake_minimum_required(VERSION 2.8.7)

# Project name must not be executable name when using the Eclipse CDT generator
project(inverted_pendulum)

# Find CHROMOSOME
set (XME_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../")
set (
	CMAKE_MODULE_PATH
	"${XME_ROOT}/tools/cmake"
	"${CMAKE_CURRENT_SOURCE_DIR}/../tools/cmake"
)
# Cannot use xme_set() since package XME has not been loaded (we're actually
# preparing to do so right now...)
set_property (GLOBAL PROPERTY XME_PLATFORM "freertos")
set_property (GLOBAL PROPERTY XME_ARCHITECTURE "NiosII")
set_property (GLOBAL PROPERTY XME_FAMILY "FPGA")
set_property (GLOBAL PROPERTY XME_BOARD "CYCLONE3DEVKIT")
set_property (GLOBAL PROPERTY XME_TARGET "EP3C120F780C7N")
# message("SET PROP")
# set_property (GLOBAL PROPERTY XME_BUILDING_APPLICATION_FOR_BOOTLOADER FALSE)
# set_property (GLOBAL PROPERTY XME_TARGET_FLASH_ADDRESS 0x0)
# set_property (GLOBAL PROPERTY XME_MEMORYMANAGEMENT "dynamic")

#set (XME_TESTSUITE YES)
find_package(XME REQUIRED)

xme_get (_XME_TARGET PROPERTY_GLOBAL XME_TARGET)
 
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

# Add definitions
add_definitions (-DXME_CORE_DIRECTORY_TYPE_MASTER)

# Set up sources and headers
#
set(
	SOURCES
	main.c
	comp_led.c
)

set(
	HEADERS
	comp_led.h
	common_ids.h
)


# Define target
xme_add_executable(
	blinky
	${SOURCES}
	${HEADERS}
)

# TODO: Remove this hack: See external/LM3/CMakeLists.txt
#target_link_libraries(
#	blinky
#)

# Build XME components
xme_link_components(
	blinky
	xme_target_bsp
	xme_core_core
	xme_hal_dio
)

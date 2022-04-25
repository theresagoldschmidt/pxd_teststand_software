#
# Copyright (c) 2011-2012, fortiss GmbH.
# Licensed under the Apache License, Version 2.0.
#
# Use, modification and distribution are subject to the terms specified
# in the accompanying license file LICENSE.txt located at the root directory
# of this software distribution. A copy is available at
# htttp://chromosome.fortiss.org/.
#
# This file is part of CHROMOSOME.
#
# $Id$
#
# File:
#         This project shows an example howto connect
#         to a publisher in a reliable way.
#
#         This means that a connection is setup via
#         TCP to the publsiher. The project uses
#         freeRTOS and lwIP.
#
# Author:
#         Hauke Staehle <staehle@fortiss.org>
#
# Architecture:
#         LM3
#
# Platform:
#         freertos
#

cmake_minimum_required(VERSION 2.6)

# Project name must not be executable name when using the Eclipse CDT generator
project(XMEreliableConnectionSubscriber)

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
set_property (GLOBAL PROPERTY XME_ARCHITECTURE "ARMv7-M")
set_property (GLOBAL PROPERTY XME_FAMILY "LM3")
set_property (GLOBAL PROPERTY XME_BOARD "EK-LM3S8962")
set_property (GLOBAL PROPERTY XME_TARGET "LM3S8962")
set_property (GLOBAL PROPERTY XME_BUILDING_APPLICATION_FOR_BOOTLOADER FALSE)
set_property (GLOBAL PROPERTY XME_TARGET_FLASH_ADDRESS 0x0)
set_property (GLOBAL PROPERTY XME_MEMORYMANAGEMENT "dynamic")


#set (XME_TESTSUITE YES)
find_package(XME REQUIRED)
 
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
	reliableSub.c
	reliableSubComponent.c
)

set(
	HEADERS
	reliableSubComponent.h
)

# Define target
xme_add_executable(
	reliableSubscriber
	${SOURCES}
)

# TODO: Remove this hack: See external/LM3/CMakeLists.txt
target_link_libraries(
	reliableSubscriber
	-Wl,--whole-archive
	xme_target_startup
	-Wl,--no-whole-archive
)

# Build XME components
xme_link_components(
	reliableSubscriber
	xme_target_bsp
	xme_core_core
	xme_hal_dio
	xme_hal_net
	xme_lwip_core
	xme_lwip_core_ipv4
	xme_lwip_netif_ethernet
	xme_hal_console	
)

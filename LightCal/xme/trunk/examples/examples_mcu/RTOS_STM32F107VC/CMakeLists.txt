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
#         CMake file for CHROMOSOME-based 
#
# Author:
#         Simon Barner <barner@fortiss.org>
#         Hauke Staehle <staehle@fortiss.org>
#
# Architecture:
#         ARMv7-M
#
# Platform:
#         RTOS
#

cmake_minimum_required(VERSION 2.6)

# Project name must not be executable name when using the Eclipse CDT generator
project(RTOS_STM32F107VC)

# Find CHROMOSOME
set (XME_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../")
set (
	CMAKE_MODULE_PATH
	"${XME_ROOT}/tools/cmake"
#	"${CMAKE_CURRENT_SOURCE_DIR}/../tools/cmake"
	"${CMAKE_CURRENT_SOURCE_DIR}/../../../../../mcu/tools/cmake"

)
# Cannot use xme_set() since package XME has not been loaded (we're actually
# preparing to do so right now...)
set_property (GLOBAL PROPERTY XME_PLATFORM "freertos")
set_property (GLOBAL PROPERTY XME_LWIP_DHCP FALSE)
set_property (GLOBAL PROPERTY XME_ARCHITECTURE "ARMv7-M")
set_property (GLOBAL PROPERTY XME_FAMILY "STM")
set_property (GLOBAL PROPERTY XME_BOARD "STM32-P107")
set_property (GLOBAL PROPERTY XME_TARGET "STM32F107VC")
set_property (GLOBAL PROPERTY XME_BUILDING_APPLICATION_FOR_BOOTLOADER FALSE)
#set_property (GLOBAL PROPERTY XME_TARGET_FLASH_ADDRESS 0x0)
set_property (GLOBAL PROPERTY XME_MEMORYMANAGEMENT "dynamic")
# Avoid out-of-RAM linker error
set_property (GLOBAL PROPERTY EXTERNAL_FREERTOS_configTOTAL_HEAP_SIZE 10240)

#set (XME_TESTSUITE YES)
find_package(XME REQUIRED)

find_package(NetworkSettings REQUIRED)
network_settings("NODE_1")

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
	comp_led_switcher.c
	stm32f10x_it.c
	lwip_test.c
)

set(
	HEADERS
	comp_led.h
	comp_led_switcher.h
	common_ids.h
	stm32f10x_conf.h
	stm32f10x_it.h
)


# Define target
xme_add_executable(
	RTOS_STM32F107VC
	${SOURCES}
	${HEADERS}
)

# TODO: Remove this hack: See external/LM3/CMakeLists.txt
target_link_libraries(
	RTOS_STM32F107VC
	-Wl,--whole-archive
	xme_target_startup
	-Wl,--no-whole-archive
)

# Build XME components
xme_link_components(
	RTOS_STM32F107VC	
	xme_hal_persistence
	xme_lwip_core
	xme_lwip_core_ipv4
	xme_lwip_api
	xme_lwip_netif_ethernet
	
	xme_freertos_kernel
#	xme_freertos_coroutine
#	xme_freertos_timers
	xme_freertos_memmang
	
	xme_target_bsp
	xme_core_core
	xme_hal_dio
	xme_hal_net	
)

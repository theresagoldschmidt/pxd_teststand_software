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
# Author:
#         Benjamin Wiesmueller <wiesmueller@fortiss.org>
#

cmake_minimum_required(VERSION 2.6)

# Project name must not be executable name when using the Eclipse CDT generator
project(xmeInterface)

# Find CHROMOSOME
set (XME_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../..")
set (CMAKE_MODULE_PATH "${XME_ROOT}/tools/cmake")
# Cannot use xme_set() since package XME has not been loaded (we're actually
# preparing to do so right now...)
set_property (GLOBAL PROPERTY XME_PLATFORM "windows")
set_property (GLOBAL PROPERTY XME_ARCHITECTURE "x86")
set (XME_TESTSUITE YES)
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

# Add project specific component directory
xme_add_subdirectory (components)

# Add definitions
add_definitions (-DXME_CORE_DIRECTORY_TYPE_LOCAL)

# Set up cross compiler

# Set up sources and headers
set(
	SOURCES
	mmtInterface.c
)

set(
	HEADERS
)

# Define target
#xme_add_executable(
#	mmtInterface
#	${SOURCES}
#	${HEADERS}
#)

# Build XME components
#xme_link_components(
#	mmtInterface
#	xme_core_core
#	xme_prim_ipLoginServerProxy
#	xme_adv_mmtLogger
#	#xme_adv_mmtUpdater
#	#xme_adv_mmtUpdaterDummy
#	#xme_adv_mmtCommander
#)

# Create a target for the library
add_library(
	xmeInterfaceLib
	SHARED
	${SOURCES}
	${HEADERS}
)

# Build XME components
xme_link_components(
	xmeInterfaceLib
	xme_core_core
	xme_prim_ipLoginServerProxy
	xme_adv_mmtLogger
	#xme_adv_mmtUpdater
	#xme_adv_mmtUpdaterDummy
	#xme_adv_mmtCommander
)

# Copy resulting library to MMT
set (COPY_LIB_TO_MMT true)
set (MMT_XMEINTERFACELIB_PATH "C:\\Users\\wiesmueller\\repositories\\mmt\\Multifunk.tool\\libraries\\xmeInterfaceLib")

if (COPY_LIB_TO_MMT)
	add_custom_command(
		TARGET xmeInterfaceLib
	        POST_BUILD
        	COMMAND echo Copying $(TargetFileName) to MMT library directory \"${MMT_XMEINTERFACELIB_PATH}\"\ncopy $(TargetPath) \"${MMT_XMEINTERFACELIB_PATH}\\$(TargetFileName)\"
	        COMMENT "Copying xmeInterfaceLib to MMT"
	)
endif (COPY_LIB_TO_MMT)

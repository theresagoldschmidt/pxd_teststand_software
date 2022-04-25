#
# Copyright (c) 2011-2012, fortiss GmbH.
# Licensed under the Apache License, Version 2.0.
#
# Use, modification and distribution are subject to the terms specified
# in the accompanying license file LICENSE.txt located at the root directory
# of this software distribution. A copy is available at
# http://chromosome.fortiss.org.
#
# This file is part of CHROMOSOME.
#
# $Id$
#
# Author:
#         Michael Geisinger <geisinger@fortiss.org>
#

xme_get (_XME_BUILD_HOST PROPERTY_GLOBAL XME_BUILD_HOST)

if (_XME_BUILD_HOST STREQUAL "windows")

	# Find libmodbus
	find_path(
		LIBMODBUS_PATH
		"include/modbus.h"
		PATHS
		"${XME_EXTERNAL_DIR}/libmodbus"
		NO_DEFAULT_PATH
	)

	if (NOT LIBMODBUS_PATH)
		message(FATAL_ERROR "libmodbus not found!")
	endif (NOT LIBMODBUS_PATH)

	# Public include path
	set (LIBMODBUS_INCLUDE_PATH "${LIBMODBUS_PATH}/include" CACHE FILEPATH "libmodbus include path")

	# Determine library path
	if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
		set (_LIBPATH "win64")
	else (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
		set (_LIBPATH "win32")
	endif (${CMAKE_SIZEOF_VOID_P} EQUAL 8)

	if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		set (_LIBPATH "${_LIBPATH}/debug")
	else (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		set (_LIBPATH "${_LIBPATH}/release")
	endif (${CMAKE_BUILD_TYPE} STREQUAL "Debug")

	# Publish library path
	set (LIBMODBUS_LIB_PATH "${LIBMODBUS_PATH}/lib/${_LIBPATH}" CACHE FILEPATH "libmodbus library path")
	set (LIBMODBUS_LINK_TARGET "${LIBMODBUS_LIB_PATH}/libmodbus.lib" CACHE FILEPATH "libmodbus link target")

else (_XME_BUILD_HOST STREQUAL "windows")

	message(FATAL_ERROR "libmodbus currently only available on Windows!")

endif (_XME_BUILD_HOST STREQUAL "windows")

message(STATUS "LIBMODBUS_INCLUDE_PATH: ${LIBMODBUS_INCLUDE_PATH}")
message(STATUS "LIBMODBUS_LIB_PATH: ${LIBMODBUS_LIB_PATH}")
message(STATUS "LIBMODBUS_LINK_TARGET: ${LIBMODBUS_LINK_TARGET}")

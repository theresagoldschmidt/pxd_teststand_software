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

	# Find easycom
	find_path(
		EASYCOM_PATH
		"include/easycom/com/Com.h"
		PATHS
		"${XME_EXTERNAL_DIR}/easycom"
		NO_DEFAULT_PATH
	)

	if (NOT EASYCOM_PATH)
		message(FATAL_ERROR "easycom not found!")
	endif (NOT EASYCOM_PATH)

	# Public include path
	set (EASYCOM_INCLUDE_PATH "${EASYCOM_PATH}/include" CACHE FILEPATH "easycom include path")

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
	set (EASYCOM_LIB_PATH "${EASYCOM_PATH}/lib/${_LIBPATH}" CACHE FILEPATH "easycom library path")
	set (EASYCOM_LINK_TARGET "${EASYCOM_LIB_PATH}/easycom_com.lib;${EASYCOM_LIB_PATH}/easycom_exception.lib" CACHE FILEPATH "easycom link target")
	set (EASYCOM_LIBMODBUS_LINK_TARGET "${EASYCOM_LIB_PATH}/easycom_libmodbus.lib" CACHE FILEPATH "easycom_libmodbus link target")

else (_XME_BUILD_HOST STREQUAL "windows")

	message(FATAL_ERROR "easycom currently only available on Windows!")

endif (_XME_BUILD_HOST STREQUAL "windows")

message(STATUS "EASYCOM_INCLUDE_PATH: ${EASYCOM_INCLUDE_PATH}")
message(STATUS "EASYCOM_LIB_PATH: ${EASYCOM_LIB_PATH}")
message(STATUS "EASYCOM_LINK_TARGET: ${EASYCOM_LINK_TARGET}")
message(STATUS "EASYCOM_LIBMODBUS_LINK_TARGET: ${EASYCOM_LIBMODBUS_LINK_TARGET}")

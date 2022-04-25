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

	# Define desired linking style
	set (MYSQLCONNECTORC_LINK_STATIC FALSE) # Link dynamically on Windows to avoid multiply defined symbols

	# Find MySQL Connector C
	find_path(
		MYSQLCONNECTORC_PATH
		"include/mysql.h"
		PATHS
		${XME_EXTERNAL_DIR}/MySQLConnectorC
		NO_DEFAULT_PATH
	)

	if (NOT MYSQLCONNECTORC_PATH)
		message(FATAL_ERROR "MySQL Connector C not found")
	endif (NOT MYSQLCONNECTORC_PATH)

	# Public include path
	set (MYSQLCONNECTORC_INCLUDE_PATH "${MYSQLCONNECTORC_PATH}/include" CACHE FILEPATH "MySQL Connector C include path")

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
	set (MYSQLCONNECTORC_LIB_PATH "${MYSQLCONNECTORC_PATH}/lib/${_LIBPATH}" CACHE FILEPATH "MySQL Connector C library path")
	set (MYSQLCONNECTORC_LIB_SHARED "${MYSQLCONNECTORC_LIB_PATH}/libmysql.dll" CACHE FILEPATH "MySQL Connector C shared library object")
	set (MYSQLCONNECTORC_LIB_STATIC "${MYSQLCONNECTORC_LIB_PATH}/mysqlclient.lib" CACHE FILEPATH "MySQL Connector C static library object")

	if (MYSQLCONNECTORC_LINK_STATIC)
		# Link statically
		set (MYSQLCONNECTORC_LINK_TARGET "mysqlclient")

	else (MYSQLCONNECTORC_LINK_STATIC)
		# Link dynamically
		set (MYSQLCONNECTORC_LINK_TARGET "libmysql")

		# Copy shared library to target directory
		configure_file(
			${MYSQLCONNECTORC_LIB_SHARED}
			${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/libmysql.dll
			COPYONLY
		)
		configure_file(
			${MYSQLCONNECTORC_LIB_SHARED}
			${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/libmysql.dll
			COPYONLY
		)

		# TODO: Install library with target

	endif (MYSQLCONNECTORC_LINK_STATIC)

elseif (_XME_BUILD_HOST STREQUAL "posix")

	# Define desired linking style
	set (MYSQLCONNECTORC_LINK_STATIC FALSE) 

	# Find MySQL Connector C
	find_path(
		MYSQLCONNECTORC_PATH
		"include/mysql.h"
		PATHS
		${XME_EXTERNAL_DIR}/MySQLConnectorC
		NO_DEFAULT_PATH
	)

	if (NOT MYSQLCONNECTORC_PATH)
		message(FATAL_ERROR "MySQL Connector C not found")
	endif (NOT MYSQLCONNECTORC_PATH)

	# Public include path
	set (MYSQLCONNECTORC_INCLUDE_PATH "${MYSQLCONNECTORC_PATH}/include" CACHE FILEPATH "MySQL Connector C include path")

	# Determine library path
	if (${_XME_ARCHITECTURE} STREQUAL "amd64")
		set (_LIBPATH "linux64")	
	else (${_XME_ARCHITECTURE} STREQUAL "amd64")
		set (_LIBPATH "linux")
	endif (${CMAKE_SIZEOF_VOID_P} EQUAL "amd64")

	if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		set (_LIBPATH "${_LIBPATH}/debug")
	else (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
		set (_LIBPATH "${_LIBPATH}/release")
	endif (${CMAKE_BUILD_TYPE} STREQUAL "Debug")

	# Publish library path
	set (MYSQLCONNECTORC_LIB_PATH "${MYSQLCONNECTORC_PATH}/lib/${_LIBPATH}" CACHE FILEPATH "MySQL Connector C library path ")
	set (MYSQLCONNECTORC_LIB_SHARED "${MYSQLCONNECTORC_LIB_PATH}/libmysqlclient.so" CACHE FILEPATH "MySQL Connector C shared library object")
	set (MYSQLCONNECTORC_LIB_STATIC "${MYSQLCONNECTORC_LIB_PATH}/libmysqlclient.a" CACHE FILEPATH "MySQL Connector C static library object")

	if (MYSQLCONNECTORC_LINK_STATIC)
		# Link statically
		set (MYSQLCONNECTORC_LINK_TARGET "mysqlclient.a")

	else (MYSQLCONNECTORC_LINK_STATIC)
		# Link dynamically
		set (MYSQLCONNECTORC_LINK_TARGET "mysqlclient.so")

		# TODO: Install library with target

	endif (MYSQLCONNECTORC_LINK_STATIC)

else(_XME_BUILD_HOST STREQUAL "windows")

	message(FATAL_ERROR "MySQL Connector C currently only available on Windows and Linux!")

endif (_XME_BUILD_HOST STREQUAL "windows")

message(STATUS "MYSQLCONNECTORC_INCLUDE_PATH: ${MYSQLCONNECTORC_INCLUDE_PATH}")
message(STATUS "MYSQLCONNECTORC_LIB_PATH: ${MYSQLCONNECTORC_LIB_PATH}")
message(STATUS "MYSQLCONNECTORC_LIB_SHARED: ${MYSQLCONNECTORC_LIB_SHARED}")
message(STATUS "MYSQLCONNECTORC_LIB_STATIC: ${MYSQLCONNECTORC_LIB_STATIC}")

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
#         Simon Barner <barner@fortiss.org>
#

# This CMake script detects the CHROMOSOME environment and defines some
# general purpose macros for use with CHROMOSOME. For details on the
# defined macros, see the documentation directly above the respective
# definition below.

# 2.8.3: CMAKE_CURRENT_LIST_DIR
# 2.8.5:
#  - string (FIND)
#  - Linked resources for Eclipse CDT projects.
#    See: <http://www.cmake.org/pipermail/cmake/2011-February/042703.html>
#
cmake_minimum_required(VERSION 2.8.5)

# Whether XME should test itself during generation
set (XME_BUILDSYSTEM_SELFTEST TRUE CACHE PATH "Enable CHROMOSOME build system self-tests")

# Set CMAKE_INSTALL_PREFIX (all install rules will install their files relative to this path)
set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install")

# Asserts that the given condition holds.
macro (xme_assert)
	if (${ARGV})
	else (${ARGV})
		message (SEND_ERROR "XME assertion failed: ${ARGV}")
	endif (${ARGV})
endmacro (xme_assert)

# Remove trailing 0 (as workaround for hex2dec(), see below)
macro (shortenhex IN OUT)
	string (LENGTH ${IN} _POS)
	math(EXPR _POS "${_POS} - 1")

	string(SUBSTRING  ${IN} ${_POS} -1 _CHAR)
	if (NOT ${CHAR} EQUAL 0)
		message (FATAL_ERROR "Addresses must end in 0 (due to overflow in CMake's math() command.")
	endif (NOT ${CHAR} EQUAL 0)

	string(SUBSTRING ${IN} 0 ${_POS} ${OUT})
endmacro (shortenhex IN OUT)

# Hexadecimal (as string) to decimal (as number) conversion
macro(hex2dec HEX DEC)
	if (${HEX} STREQUAL "0x80000000")
		# TODO: Work around the fact that CMake's math implementation works on int32_t.
		#       Fortunately, the largest hex value we currently need to manipulate is
		#       0x08000000, so shortenhex() is currently not needed
		set (${DEC} 2147483648)
	else (${HEX} STREQUAL "0x80000000")
		set (_DIGITS "0123456789ABCDEF")
		set (_DEC 0)

		string(TOUPPER ${HEX} _HEX)
		string(SUBSTRING ${_HEX} 0 2 _PREF)
		if (NOT "0X" STREQUAL _PREF)
			message(FATAL_ERROR "hex2dec: Not a hexadecimal number: '${HEX}'")
		endif (NOT "0X" STREQUAL _PREF)

		set (I 2)
		string (LENGTH ${_HEX} N)

		while (${I} LESS ${N})
			string(SUBSTRING ${_HEX} ${I} 1 _CHAR)
			string(FIND ${_DIGITS} ${_CHAR} _POS)

			if (_POS EQUAL -1)
				message(FATAL_ERROR "hex2dec: Not a hexadecimal number: '${HEX}'")
			endif (_POS EQUAL -1)
			math(EXPR _DEC "${_DEC}*16 + ${_POS}")
			math(EXPR I "${I} + 1")

			if (${_DEC} LESS 0)
				message (FATAL_ERROR "hex2dec: An overflow occurred. Maximum argument: 0x7fffffff.")
			endif (${_DEC} LESS 0)
		endwhile (${I} LESS ${N})

		set (${DEC} ${_DEC})
	endif (${HEX} STREQUAL "0x80000000")
endmacro(hex2dec)

if (XME_BUILDSYSTEM_SELFTEST)
	hex2dec("0x" __DEC__)
	xme_assert(${__DEC__} EQUAL 0)
	hex2dec("0x0" __DEC__)
	xme_assert(${__DEC__} EQUAL 0)
	hex2dec("0x1" __DEC__)
	xme_assert(${__DEC__} EQUAL 1)
	hex2dec("0x7" __DEC__)
	xme_assert(${__DEC__} EQUAL 7)
	hex2dec("0xF" __DEC__)
	xme_assert(${__DEC__} EQUAL 15)
	hex2dec("0xf" __DEC__)
	xme_assert(${__DEC__} EQUAL 15)
	hex2dec("0xFF" __DEC__)
	xme_assert(${__DEC__} EQUAL 255)
	hex2dec("0xff" __DEC__)
	xme_assert(${__DEC__} EQUAL 255)
	hex2dec("0x100" __DEC__)
	xme_assert(${__DEC__} EQUAL 256)
	hex2dec("0x7FFFFFFF" __DEC__)
	xme_assert(${__DEC__} EQUAL 2147483647)
	hex2dec("0x80000000" __DEC__)
	xme_assert(${__DEC__} EQUAL 2147483648)
endif (XME_BUILDSYSTEM_SELFTEST)

# Decimal (as number) to hexadecimal (as string) conversion
macro(dec2hex DEC HEX)
	set (_DIGITS "0123456789ABCDEF")

	if (DEC LESS 0)
		message (FATAL_ERROR "dec2hex: Not a non-negative integer: ${DEC}")
	endif (DEC LESS 0)

	if (${DEC} EQUAL 0)
		set (${HEX} "0x0")
	else (${DEC} EQUAL 0)
		set (_HEX "")
		set (_DEC ${DEC})
		while (_DEC GREATER 0)
			math (EXPR _DIGVAL "${_DEC} % 16")
			string(SUBSTRING ${_DIGITS} ${_DIGVAL} 1 _DIG)
			set (_HEX "${_DIG}${_HEX}")
			math(EXPR _DEC "${_DEC} / 16")
		endwhile (_DEC GREATER 0)

		set (${HEX} "0x${_HEX}")
	endif (${DEC} EQUAL 0)
endmacro(dec2hex)

if (XME_BUILDSYSTEM_SELFTEST)
	dec2hex(0 __HEX__)
	xme_assert(${__HEX__} STREQUAL "0x0")
	dec2hex(1 __HEX__)
	xme_assert(${__HEX__} STREQUAL "0x1")
	dec2hex(7 __HEX__)
	xme_assert(${__HEX__} STREQUAL "0x7")
	dec2hex(15 __HEX__)
	xme_assert(${__HEX__} STREQUAL "0xF")
	dec2hex(255 __HEX__)
	xme_assert(${__HEX__} STREQUAL "0xFF")
	dec2hex(256 __HEX__)
	xme_assert(${__HEX__} STREQUAL "0x100")
	dec2hex(2147483647 __HEX__)
	xme_assert(${__HEX__} STREQUAL "0x7FFFFFFF") # This is the maximum value that can be represented by CMake, larger values will be changed to 2147483647!
	#dec2hex(2147483648 __HEX__)
	#xme_assert(${__HEX__} STREQUAL "0x80000000")
endif (XME_BUILDSYSTEM_SELFTEST)

# Simplifies the given input path by replacing each occurrence of "." and/or
# ".." in conjunction with a specified parent directory. Here are some
# examples (see testsuite below for more examples):
#  - "/some/./dir" --> "/some/dir"
#  - "/some/path/.." --> "/some"
#  - "/some/path/name/../../dir" --> "/some/dir"
#  - "/some/../../dir" --> "/../dir"
#  - "some/../../dir" --> "../dir"
macro (xme_simplify_path OUTPUT PATH)
	set (VALUE ${PATH})

	while (1)
		if (${VALUE} MATCHES "^[.]/")
			string (REGEX REPLACE "^[.]/" "" VALUE ${VALUE})
		else (${VALUE} MATCHES "^[.]/")
			break()
		endif (${VALUE} MATCHES "^[.]/")
	endwhile (1)

	while (1)
		if (${VALUE} MATCHES "/[.]$")
			string (REGEX REPLACE "/[.]$" "" VALUE ${VALUE})
		else (${VALUE} MATCHES "/[.]$")
			break()
		endif (${VALUE} MATCHES "/[.]$")
	endwhile (1)

	while (1)
		if (${VALUE} MATCHES "/[.]/")
			string (REGEX REPLACE "/[.]/" "/" VALUE ${VALUE})
		else (${VALUE} MATCHES "/[.]/")
			break()
		endif (${VALUE} MATCHES "/[.]/")
	endwhile (1)

	while (1)
		if (${VALUE} MATCHES "/[^/.]+/[.][.]($|/)")
			string (REGEX REPLACE "/[^/.]+/[.][.]($|/)" "\\1" VALUE ${VALUE})
		else (${VALUE} MATCHES "/[^/.]+/[.][.]($|/)")
			if (${VALUE} MATCHES "^[^/.]+/[.][.]($|/)")
				string (REGEX REPLACE "^[^/.]+/[.][.]($|/)" "" VALUE ${VALUE})
			else (${VALUE} MATCHES "^[^/.]+/[.][.]($|/)")
				break ()
			endif (${VALUE} MATCHES "^[^/.]+/[.][.]($|/)")
		endif (${VALUE} MATCHES "/[^/.]+/[.][.]($|/)")
	endwhile (1)

	if (${VALUE} STREQUAL "")
		set (${OUTPUT} '.')
	else (${VALUE} STREQUAL "")
		set (${OUTPUT} ${VALUE})
	endif (${VALUE} STREQUAL "")

	#message (STATUS "Simplified path from '${PATH}' to '${${OUTPUT}}'")
endmacro (xme_simplify_path)

if (XME_BUILDSYSTEM_SELFTEST)
	# Path simplification with "."
	xme_simplify_path(__PATH__ "/some/./dir")
	xme_assert(${__PATH__} STREQUAL "/some/dir")
	xme_simplify_path(__PATH__ "/some/././dir")
	xme_assert(${__PATH__} STREQUAL "/some/dir")
	xme_simplify_path(__PATH__ "/some/./")
	xme_assert(${__PATH__} STREQUAL "/some/")
	xme_simplify_path(__PATH__ "/some/.")
	xme_assert(${__PATH__} STREQUAL "/some")
	xme_simplify_path(__PATH__ "/./some/dir")
	xme_assert(${__PATH__} STREQUAL "/some/dir")
	xme_simplify_path(__PATH__ "/./some/./dir")
	xme_assert(${__PATH__} STREQUAL "/some/dir")

	# Path simplification with ".."
	xme_simplify_path(__PATH__ "/some/path/..")
	xme_assert(${__PATH__} STREQUAL "/some")
	xme_simplify_path(__PATH__ "/some/path/name/../../dir")
	xme_assert(${__PATH__} STREQUAL "/some/dir")
	xme_simplify_path(__PATH__ "/some/../../dir")
	xme_assert(${__PATH__} STREQUAL "/../dir")
	xme_simplify_path(__PATH__ "some/../../dir")
	xme_assert(${__PATH__} STREQUAL "../dir")

	# Path simplification with "." and ".." combined
	xme_simplify_path(__PATH__ "/./some/./../dir")
	xme_assert(${__PATH__} STREQUAL "/dir")
	xme_simplify_path(__PATH__ "./some/./../dir")
	xme_assert(${__PATH__} STREQUAL "dir")

	# Strange file names
	xme_simplify_path(__PATH__ "dir./some/./../dir")
	xme_assert(${__PATH__} STREQUAL "dir./dir")
	xme_simplify_path(__PATH__ "dir.name/some/./../dir")
	xme_assert(${__PATH__} STREQUAL "dir.name/dir")
	xme_simplify_path(__PATH__ "dir../some/./../dir")
	xme_assert(${__PATH__} STREQUAL "dir../dir")
	#xme_simplify_path(__PATH__ "dir/..some/./../dir")
	#xme_assert(${__PATH__} STREQUAL "dir/dir") # TODO: Currently fails!
endif (XME_BUILDSYSTEM_SELFTEST)

macro (xme_resolve_path OUTPUT PATH)
	if (IS_ABSOLUTE ${PATH})
		set (_PATH_ ${PATH})
	else (IS_ABSOLUTE ${PATH})
		xme_get (_XME_CURRENT_SOURCE_DIR PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR)
		set (_PATH_ "${_XME_CURRENT_SOURCE_DIR}/${PATH}")
	endif (IS_ABSOLUTE ${PATH})

	xme_simplify_path(${OUTPUT} ${_PATH_})

	#message (STATUS "Resolved path from '${PATH}' to '${${OUTPUT}}'")
endmacro (xme_resolve_path)

macro (xme_defined RVAL TYPE NAME)
	if ("VARIABLE" STREQUAL ${TYPE})
		if (DEFINED ${NAME})
			set (${RVAL} 1)
		else (DEFINED ${NAME})
			set (${RVAL} 0)
		endif (DEFINED ${NAME})
	elseif ("PROPERTY_GLOBAL" STREQUAL ${TYPE})
		get_property (${RVAL} GLOBAL PROPERTY ${NAME} SET)
	else ("PROPERTY_GLOBAL" STREQUAL ${TYPE})
		message (FATAL_ERROR "Invalid value for TYPE in function xme_defined: '${TYPE}'")
	endif ("VARIABLE" STREQUAL ${TYPE})
endmacro (xme_defined)

macro (xme_set TYPE NAME VAL)
	if ("VARIABLE" STREQUAL ${TYPE})
		set (${NAME} ${VAL})
	elseif ("PROPERTY_GLOBAL" STREQUAL ${TYPE})
		set_property (GLOBAL PROPERTY ${NAME} ${VAL})
	else ("PROPERTY_GLOBAL" STREQUAL ${TYPE})
		message (FATAL_ERROR "Invalid value for TYPE in function xme_set: '${TYPE}'")
	endif ("VARIABLE" STREQUAL ${TYPE})
endmacro(xme_set)

macro (xme_weak_set TYPE NAME VAL)
	xme_defined(_DEFINED ${TYPE} ${NAME})
	if (NOT ${_DEFINED})
		xme_set(${TYPE} ${NAME} ${VAL})
	endif (NOT ${_DEFINED})
endmacro (xme_weak_set)

macro (xme_get RVAL TYPE NAME)
	if ("VARIABLE" STREQUAL ${TYPE})
		set (${RVAL} ${${NAME}})
	elseif ("PROPERTY_GLOBAL" STREQUAL ${TYPE})
		get_property (${RVAL} GLOBAL PROPERTY ${NAME})
# message("GET PROP ${NAME} --> ${${RVAL}}")
	else ("PROPERTY_GLOBAL" STREQUAL ${TYPE})
		message (FATAL_ERROR "Invalid value for TYPE in function xme_get: '${TYPE}'")
	endif ("VARIABLE" STREQUAL ${TYPE})
endmacro(xme_get)

macro (xme_append TYPE NAME VAL)
	if ("VARIABLE" STREQUAL ${TYPE})
		list (APPEND ${NAME} ${VAL})
	elseif ("PROPERTY_GLOBAL" STREQUAL ${TYPE})
		get_property (TMP GLOBAL PROPERTY ${NAME})
		list (APPEND TMP ${VAL})
		set_property (GLOBAL PROPERTY ${NAME} ${TMP})
	else ("PROPERTY_GLOBAL" STREQUAL ${TYPE})
		message (FATAL_ERROR "Invalid value for TYPE in function xme_append: '${TYPE}'")
	endif ("VARIABLE" STREQUAL ${TYPE})
endmacro(xme_append)

macro (xme_build_option_tag FILE_NAME TAG)
	string (REGEX REPLACE "/|\\.|\\\\" "_" _TMP ${FILE_NAME})
	string (TOUPPER ${_TMP} _TMP)
	set (${TAG} ${_TMP})
endmacro (xme_build_option_tag)
	
macro (xme_build_option OPTION_NAME DEFAULT_VALUE OPTION_FILE)

	# Optional parameters
	if (${ARGC} GREATER 3)
		set(_OPTION_PREFIX ${ARGV3})
		
		# Remove prefix from generated option
		string (REPLACE ${_OPTION_PREFIX} "" OPTION_NAME_DEFINE ${OPTION_NAME})
	else (${ARGC} GREATER 3)
		set (OPTION_NAME_DEFINE ${OPTION_NAME})
	endif (${ARGC} GREATER 3)

	xme_build_option_tag (${OPTION_FILE} "TAG")
	xme_defined (_XME_OPTIONS_INCLUDE_FILE_CLOSED PROPERTY_GLOBAL XME_OPTIONS_INCLUDE_FILE_${TAG}_CLOSED)
	
	if (${_XME_OPTIONS_INCLUDE_FILE_CLOSED})
		message (FATAL_ERROR "Cannot add option ${OPTION_NAME} to ${OPTION_FILE}: include guard has already been closed.")
	endif (${_XME_OPTIONS_INCLUDE_FILE_CLOSED})
	
	xme_get(_XME_OPTIONS_INCLUDE_DIR PROPERTY_GLOBAL XME_OPTIONS_INCLUDE_DIR)	
	set (_XME_OPTIONS_INCLUDE_FILE ${_XME_OPTIONS_INCLUDE_DIR}/${OPTION_FILE})
	
	# Provide default values for XME options
	xme_defined (_DEFINED PROPERTY_GLOBAL ${OPTION_NAME})
	if (NOT ${_DEFINED})
		message (STATUS "Setting build option ${OPTION_NAME} to ${DEFAULT_VALUE} (default value).")
		
		# Write default value to global property so it can be accessed in other CMake scripts
		xme_set (PROPERTY_GLOBAL ${OPTION_NAME} "${DEFAULT_VALUE}")
		xme_get (_VALUE PROPERTY_GLOBAL ${OPTION_NAME})
	else (NOT ${_DEFINED})
		xme_get (_VALUE PROPERTY_GLOBAL ${OPTION_NAME})
		message (STATUS "Setting build option ${OPTION_NAME} to ${_VALUE} (user supplied value).")
	endif (NOT ${_DEFINED})
		
	file(APPEND ${_XME_OPTIONS_INCLUDE_FILE} "#ifndef ${OPTION_NAME_DEFINE}\n")
	file(APPEND ${_XME_OPTIONS_INCLUDE_FILE} "	#define ${OPTION_NAME_DEFINE} ${_VALUE}\n")
	file(APPEND ${_XME_OPTIONS_INCLUDE_FILE} "#endif\n")
endmacro (xme_build_option)

macro (xme_build_option_file_open OPTION_FILE)
	xme_build_option_tag (${OPTION_FILE} "TAG")

	xme_get(_XME_OPTIONS_INCLUDE_DIR PROPERTY_GLOBAL XME_OPTIONS_INCLUDE_DIR)
	set (_XME_OPTIONS_INCLUDE_FILE ${_XME_OPTIONS_INCLUDE_DIR}/${OPTION_FILE})

	file (APPEND "${_XME_OPTIONS_INCLUDE_FILE}" "// DO NOT EDIT! DO NOT EDIT!  DO NOT EDIT!  DO NOT EDIT! DO NOT EDIT!\n")
	file (APPEND "${_XME_OPTIONS_INCLUDE_FILE}" "// \n")
	file (APPEND "${_XME_OPTIONS_INCLUDE_FILE}" "// This file will be re-generated by FindXME.cmake each time the project is reconfigured.\n")
	file (APPEND "${_XME_OPTIONS_INCLUDE_FILE}" "// Instead, set the according XME property, or adjust the default value set by xme_build_option.\n")
	file (APPEND "${_XME_OPTIONS_INCLUDE_FILE}" "// \n")
	file (APPEND "${_XME_OPTIONS_INCLUDE_FILE}" "\n")
	file (APPEND "${_XME_OPTIONS_INCLUDE_FILE}" "#ifndef ${TAG}\n")
	file (APPEND "${_XME_OPTIONS_INCLUDE_FILE}" "#define ${TAG}\n")
	file (APPEND "${_XME_OPTIONS_INCLUDE_FILE}" "\n")
endmacro (xme_build_option_file_open)

macro (xme_build_option_set NAME VAL MSG)
	message (STATUS "Forcing option ${NAME} to ${VAL}: ${MSG}")
	xme_set (PROPERTY_GLOBAL ${NAME} ${VAL})
endmacro (xme_build_option_set)

macro (xme_build_option_weak_set NAME VAL)
	xme_weak_set (PROPERTY_GLOBAL ${NAME} ${VAL})
endmacro (xme_build_option_weak_set)

macro (xme_build_option_file_close OPTION_FILE)
	xme_build_option_tag (${OPTION_FILE} "TAG")
	xme_set (PROPERTY_GLOBAL XME_OPTIONS_INCLUDE_FILE_${TAG}_CLOSED TRUE)
	
	xme_get(_XME_OPTIONS_INCLUDE_DIR PROPERTY_GLOBAL XME_OPTIONS_INCLUDE_DIR)
	set (_XME_OPTIONS_INCLUDE_FILE ${_XME_OPTIONS_INCLUDE_DIR}/${OPTION_FILE})
	
	file (APPEND "${_XME_OPTIONS_INCLUDE_FILE}" "\n")
	file (APPEND "${_XME_OPTIONS_INCLUDE_FILE}" "#endif // #ifndef ${TAG}\n")
endmacro (xme_build_option_file_close)

macro (xme_build_option_assert OPTION_NAME MSG)
	xme_defined(_DEFINED PROPERTY_GLOBAL ${OPTION_NAME})
	if (NOT ${_DEFINED})
		message (FATAL_ERROR "xme_build_option_set(PROPERTY_GLOBAL ${OPTION_NAME} <...>) is required: ${MSG}")
	endif (NOT ${_DEFINED})
endmacro (xme_build_option_assert)

# Set root dir: CACHE Variable XME_ROOT
if (NOT DEFINED XME_ROOT)
	set (XME_ROOT "${CMAKE_CURRENT_LIST_DIR}/../..")
endif (NOT DEFINED XME_ROOT)
xme_simplify_path (XME_ROOT "${XME_ROOT}")
set (XME_ROOT "${XME_ROOT}" CACHE PATH "CHROMOSOME root directory")

# Set board support package dir (pin definitions, etc.): CACHE Variable XME_BSP_DIR (via find_file)
if (NOT DEFINED XME_BSP_DIR)
	find_file (XME_BSP_DIR "bsp" PATHS "${XME_ROOT}" DOC "CHROMOSOME BSP directory" NO_DEFAULT_PATH)
endif (NOT DEFINED XME_BSP_DIR)

# Set examples dir (CHROMOSOME examples): CACHE Variable XME_EXAMPLES_DIR (via find_file)
if (NOT DEFINED XME_EXAMPLES_DIR)
	find_file (XME_EXAMPLES_DIR "examples" PATHS "${XME_ROOT}" DOC "CHROMOSOME examples directory" NO_DEFAULT_PATH)
endif (NOT DEFINED XME_EXAMPLES_DIR)

# Set external dir (adapted vendor sources for CHROMOSOME): CACHE Variable XME_EXTERNAL_DIR (via find_file)
if (NOT DEFINED XME_EXTERNAL_DIR)
	find_file (XME_EXTERNAL_DIR "external" PATHS "${XME_ROOT}" DOC "CHROMOSOME external directory" NO_DEFAULT_PATH)
endif (NOT DEFINED XME_EXTERNAL_DIR)

# Set src dir (CHROMOSOME sources): CACHE Variable XME_SRC_DIR (via find_file)
if (NOT DEFINED XME_SRC_DIR)
	find_file (XME_SRC_DIR "xme" PATHS "${XME_ROOT}" DOC "CHROMOSOME source directory" NO_DEFAULT_PATH)
endif (NOT DEFINED XME_SRC_DIR)

# Set tests dir (CHROMOSOME tests): CACHE Variable XME_TESTS_DIR (via find_file)
if (NOT DEFINED XME_TESTS_DIR)
	find_file (XME_TESTS_DIR "tests" PATHS "${XME_ROOT}" DOC "CHROMOSOME tests directory" NO_DEFAULT_PATH)
endif (NOT DEFINED XME_TESTS_DIR)

# Set tools dir (CHROMOSOME tools): CACHE Variable XME_TOOLS_DIR (via find_file)
if (NOT DEFINED XME_TOOLS_DIR)
	find_file (XME_TOOLS_DIR "tools" PATHS "${XME_ROOT}" DOC "CHROMOSOME tools directory" NO_DEFAULT_PATH)
endif (NOT DEFINED XME_TOOLS_DIR)

# Add install rule for LICENSE.txt
install(
	FILES
	"${XME_ROOT}/LICENSE.txt"
	DESTINATION
	"."
)

# Add install rule for tools
install(
    DIRECTORY
    ${XME_TOOLS_DIR}
    DESTINATION
    "src"
)

# Add install rule for tests directory
install(
    DIRECTORY
    ${XME_TESTS_DIR}
    DESTINATION
    "src"
)

# Add install rule for xme source directory (except ports which will be added in 
# xme_add_subdirectory in order to only install the needed platform specific sources)
install(
    DIRECTORY
    ${XME_SRC_DIR}
    DESTINATION
    "src"
	PATTERN "ports/*" EXCLUDE
)

# Add install rule for default build directory.
# For an example project resisting in ${XME_EXAMPLES_DIR}/some/project,
# the default build directory is ${XME_EXAMPLES_DIR}/build/some/project.
# The default build directory will only be installed if the respective
# project is contained in ${XME_EXAMPLES_DIR}. For packaging to not drop
# the default build directory, a dummy file is copied into it.
string (LENGTH "${XME_EXAMPLES_DIR}/" __LEN__)
string (LENGTH "${CMAKE_PARENT_LIST_FILE}" __LEN2__)
if (__LEN__ LESS __LEN2__)
	set (__MIN_LEN__ ${__LEN__})
else (__LEN__ LESS __LEN2__)
	set (__MIN_LEN__ ${__LEN2__})
endif (__LEN__ LESS __LEN2__)
string (SUBSTRING "${CMAKE_PARENT_LIST_FILE}" 0 ${__MIN_LEN__} __SUBSTR__)
if ("${XME_EXAMPLES_DIR}/" STREQUAL __SUBSTR__)
	# Retrieve relative path of project below the examples directory
	string (SUBSTRING "${CMAKE_PARENT_LIST_FILE}" ${__LEN__} -1 __SUBSTR__)
	string (REPLACE "/CMakeLists.txt" "" __SUBSTR__ "${__SUBSTR__}")

	message(STATUS "Installing default build directory 'examples/build/${__SUBSTR__}' for this project")

	install(
		FILES "${CMAKE_CURRENT_LIST_DIR}/dummy.in"
		DESTINATION "src/examples/build/${__SUBSTR__}"
		RENAME "dummy"
	)
endif ("${XME_EXAMPLES_DIR}/" STREQUAL __SUBSTR__)



# CACHE Variable XME_FOUND
set (XME_FOUND TRUE CACHE INTERNAL "Whether CHROMOSOME has been found")

# Global property: XME_CURRENT_SOURCE_DIR
xme_simplify_path (_XME_CURRENT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
xme_set (PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR "${_XME_CURRENT_SOURCE_DIR}")



# Macro:      xme_install_rec ( <destination> <prefix> [ <absolute-file-paths> ... ] )
#
# Purpose:    For internal use only. Adds install rules for the given files. Files will be installed
#             into <CMAKE_INSTALL_PREFIX>/<destination>/<given file path without <prefix>>.
#             For every given file path this functions walks through the parent directories and install
#             all found files until the <prefix> directory is reached or until a directory with a 
#             CMakeLists.txt is found.
#
# Parameters: <destination> Subdirectory inside installation directory where the given files will be installed.
#             <prefix>      See above.
#             [ <absolute-file-paths> ... ]
#                           Files to install with absolute paths. Must reside somewhere under <prefix>.
#
macro (xme_install_rec)
	if (${ARGC} GREATER 1)   
		set (__FILES__ ${ARGV})
		list(GET __FILES__ 0 __DESTINATION__)
		list(GET __FILES__ 1 __PREFIX__)
		list (REMOVE_AT __FILES__ 0 1)

		# Install every file into the install directory preserving the directory structure 
		foreach (__FILE__ ${__FILES__})
			# Ignore file if it is not located in PREFIX:
			#
			# This is needed to avoid an endless loop when walking up the
			# path in the while loop below.
			string (FIND ${__FILE__} ${__PREFIX__} __POS__)
			if (NOT (__POS__ EQUAL -1))
				set (__FILE_IDEST__ "")
				# Remove ${XME_ROOT} & filename (including the last '/') to get path for install destination
				string (REGEX REPLACE "^${__PREFIX__}" "" __FILE_IDEST__ ${__FILE__})
				string (REGEX REPLACE "/[^/]+$" "" __FILE_IDEST__ ${__FILE_IDEST__})
				
				install(
					FILES
					${__FILE__}
					DESTINATION
					"${__DESTINATION__}${__FILE_IDEST__}"
				)
				
				# Walk through parent directories until __PREFIX__ is reached or a CMakeLists.txt is found
				string (FIND "${__PREFIX__}${__FILE_IDEST__}" "/" __POS__ REVERSE)
				string (SUBSTRING "${__PREFIX__}${__FILE_IDEST__}" 0 ${__POS__} __DIR__)
				set (__CONTINUE__ TRUE)
				while (${__CONTINUE__})
					if (EXISTS "${__DIR__}/CMakeLists.txt")
						set (__CONTINUE__ FALSE)
					else (EXISTS "${__DIR__}/CMakeLists.txt")
						if ("${__DIR__}" EQUAL "${__PREFIX__}")
							set (__CONTINUE__ FALSE)
						else ("${__DIR__}" EQUAL "${__PREFIX__}")
						
							# Add install rule for all files in directory (excluding directories)
							file(GLOB __DIR_CONTENTS__ "${__DIR__}/*")
							
							set (__DIR_FILES__ "")
							foreach (__F__ ${__DIR_CONTENTS__})
								if (NOT IS_DIRECTORY ${__F__})
									set (__DIR_FILES__ ${__DIR_FILES__} ${__F__})
								endif (NOT IS_DIRECTORY ${__F__})
							endforeach (__F__)
							foreach (__DIR_FILE__ ${__DIR_FILES__})
								set (__DIR_FILE_IDEST__ "")
								# Remove __PREFIX__ & filename (including the last '/') to get path for install destination
								string (REGEX REPLACE "^${__PREFIX__}" "" __DIR_FILE_IDEST__ ${__DIR_FILE__})
								string (REGEX REPLACE "/[^/]+$" "" __DIR_FILE_IDEST__ ${__DIR_FILE_IDEST__})

								install(
									FILES
									${__DIR_FILE__}
									DESTINATION
									"${__DESTINATION__}${__DIR_FILE_IDEST__}"
								)
							endforeach (__DIR_FILE__)
						
						endif ("${__DIR__}" EQUAL "${__PREFIX__}")
					endif (EXISTS "${__DIR__}/CMakeLists.txt")
					
					string (FIND "${__DIR__}" "/" __POS__ REVERSE)
					string (SUBSTRING "${__DIR__}" 0 ${__POS__} __DIR__)
				endwhile (${__CONTINUE__})
			else (NOT (__POS__ EQUAL -1))
				message (STATUS "${__FILE__} is not located in ${__PREFIX__}. Skipped generation of install rule.")
			endif (NOT (__POS__ EQUAL -1))
    	endforeach (__FILE__)
    endif (${ARGC} GREATER 1)
endmacro (xme_install_rec)



# Define some useful macros
macro (xme_add_subdirectory DIRNAME)

	# Handle optional parameters
	set (_USE_ADD_SUBDIRECTORY FALSE)
	set (_CMAKE_SCRIPT_NAME "CMakeLists.txt")
	if (${ARGC} GREATER 1)
		set (_USE_ADD_SUBDIRECTORY ${ARGV1})
		if (${ARGC} GREATER 2)
			set (_CMAKE_SCRIPT_NAME ${ARGV2})
			if (${_USE_ADD_SUBDIRECTORY} AND NOT (${_CMAKE_SCRIPT_NAME} STREQUAL "CMakeLists.txt"))
				message (FATAL_ERROR "xme_add_subdirectory: A custom script name is currently only supported in 'include' mode.")
			endif (${_USE_ADD_SUBDIRECTORY} AND NOT (${_CMAKE_SCRIPT_NAME} STREQUAL "CMakeLists.txt"))
		endif (${ARGC} GREATER 2)
	endif (${ARGC} GREATER 1)

	if (IS_ABSOLUTE ${DIRNAME})
		xme_get (__XME_OLD_SOURCE_DIR__ PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR)
		xme_set (PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR "${DIRNAME}")
		
		# Store dir in global property for later use
	    xme_append (PROPERTY_GLOBAL XME_SOURCE_DIR_LIST "${DIRNAME}")
	    
		if (${_USE_ADD_SUBDIRECTORY})
			message (STATUS "--> Entering directory '${DIRNAME}' using add_subdirectory().")

			# Derive valid relative binary dir from absolute source dir specification
			# 1. Abreviate  with xme
			string (REGEX REPLACE "^${XME_SRC_DIR}" "xme" _BIN_SUBDIR ${DIRNAME})
			xme_get (_XME_BUILD_HOST PROPERTY_GLOBAL XME_BUILD_HOST)
			if (_XME_BUILD_HOST STREQUAL "windows")
				# 2. On windows, change c: to c_ etc.
				string (REGEX REPLACE ":" "_" _BIN_SUBDIR ${_BIN_SUBDIR})
			endif (_XME_BUILD_HOST STREQUAL "windows")
			add_subdirectory (${DIRNAME} "${CMAKE_CURRENT_BINARY_DIR}/${_BIN_SUBDIR}")
		else (${_USE_ADD_SUBDIRECTORY})
			message (STATUS "--> Entering directory '${DIRNAME}' using include().")
			include ("${DIRNAME}/${_CMAKE_SCRIPT_NAME}")
		endif (${_USE_ADD_SUBDIRECTORY})
		
		# Store directory for use after if
		xme_get (__DIR_ABSOLUTE__ PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR)
		
		xme_set (PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR "${__XME_OLD_SOURCE_DIR__}")
		message (STATUS "<-- leaving directory '${DIRNAME}'.")
	else (IS_ABSOLUTE ${DIRNAME})
		xme_get (_XME_CURRENT_SOURCE_DIR PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR)
		xme_set (PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR "${_XME_CURRENT_SOURCE_DIR}/${DIRNAME}")
		
		# Store dir in global property for later use
	    xme_append (PROPERTY_GLOBAL XME_SOURCE_DIR_LIST "${_XME_CURRENT_SOURCE_DIR}/${DIRNAME}")
		
		if (${_USE_ADD_SUBDIRECTORY})
			message (STATUS "--> Entering directory '${_XME_CURRENT_SOURCE_DIR}/${DIRNAME}' using add_subdirectory().")
			add_subdirectory ("${_XME_CURRENT_SOURCE_DIR}/${DIRNAME}" "${CMAKE_CURRENT_BINARY_DIR}/${DIRNAME}")
		else (${_USE_ADD_SUBDIRECTORY})
			message (STATUS "--> Entering directory '${_XME_CURRENT_SOURCE_DIR}/${DIRNAME}' using include().")
			include ("${_XME_CURRENT_SOURCE_DIR}/${DIRNAME}/${_CMAKE_SCRIPT_NAME}")
		endif (${_USE_ADD_SUBDIRECTORY})
		xme_get (_XME_CURRENT_SOURCE_DIR PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR)
		xme_simplify_path (_XME_CURRENT_SOURCE_DIR "${_XME_CURRENT_SOURCE_DIR}/..")
		
		# Store directory for use after if
		xme_get (__DIR_ABSOLUTE__ PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR)
		
		message (STATUS "<-- leaving directory '${_XME_CURRENT_SOURCE_DIR}/${DIRNAME}'.")
		xme_set (PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR "${_XME_CURRENT_SOURCE_DIR}")
	endif (IS_ABSOLUTE ${DIRNAME})

	# Add install rule for all files in directory (excluding directories)
	file(GLOB __DIR_CONTENTS__ "${__DIR_ABSOLUTE__}/*")
	set (__FILES__ "")
	foreach (__F__ ${__DIR_CONTENTS__})
		if (NOT IS_DIRECTORY ${__F__})
			set (__FILES__ ${__FILES__} ${__F__})
		endif (NOT IS_DIRECTORY ${__F__})
	endforeach (__F__)
	foreach (__FILE__ ${__FILES__})
		set (__FILE_IDEST__ "")
		# Remove ${XME_ROOT} & filename (including the last '/') to get path for install destination
		string (REGEX REPLACE "^${XME_ROOT}" "" __FILE_IDEST__ ${__FILE__})
		string (REGEX REPLACE "/[^/]+$" "" __FILE_IDEST__ ${__FILE_IDEST__})
		
		install(
			FILES
			${__FILE__}
			DESTINATION
			"src/${__FILE_IDEST__}"
		)
	endforeach (__FILE__)
	
	# Ensure that include paths defined in subdirectories are also
	# available at the current scope.
	#
	# TODO: If the structure looks like this: p/A, p/B, then the directories
	#       defined in p/A will also be applied to p/B. The correct solution
	#       would involve managing a stack of include directories.
	xme_get (_INCLUDE_DIRECTORIES PROPERTY_GLOBAL XME_INCLUDE_DIRECTORIES)

	foreach (_INCLUDE_DIRECTORY ${_INCLUDE_DIRECTORIES})
		# xme_include_directory ensures absolute paths
		#message (STATUS "Pulled include directory '${_INCLUDE_DIRECTORY}' from '${DIRNAME}'.")
		include_directories (${_INCLUDE_DIRECTORY})
	endforeach (_INCLUDE_DIRECTORY)

	# Ensure that library paths defined in subdirectories are also
	# available at the current scope.
	#
	# TODO: If the structure looks like this: p/A, p/B, then the directories
	#       defined in p/A will also be applied to p/B. The correct solution
	#       would involve managing a stack of library directories.
	xme_get (_LIBRARY_DIRECTORIES PROPERTY_GLOBAL XME_LIBRARY_DIRECTORIES)

	foreach (_LIBRARY_DIRECTORY ${_LIBRARY_DIRECTORIES})
		# xme_library_directory ensures absolute paths
		#message (STATUS "Pulled library directory '${_LIBRARY_DIRECTORY}' from '${DIRNAME}'.")
		link_directories (${_LIBRARY_DIRECTORY})
	endforeach (_LIBRARY_DIRECTORY)

	# Ensure that defines specified in subdirectories are also
	# available at the current scope.
	#
	# TODO: If the structure looks like this: p/A, p/B, then the defines
	#       specified in p/A will also be applied to p/B. The correct solution
	#       would involve managing a stack of definitions.
	xme_get (_DEFINITIONS PROPERTY_GLOBAL XME_DEFINITIONS)

	foreach (_DEFINITION ${_DEFINITIONS})
		#message (STATUS "Pulled definition '${_DEFINITION}' from '${DIRNAME}'.")
		add_definitions (${_DEFINITION})
	endforeach (_DEFINITION)
	
endmacro (xme_add_subdirectory)


# Global property XME_COMPONENTS
xme_set (PROPERTY_GLOBAL XME_COMPONENTS "")

# Global property XME_DOCUMENTATION_FILES
xme_set (PROPERTY_GLOBAL XME_DOCUMENTATION_FILES "")



# Macro:      xme_add_component ( <component-name>
#                 [ <header.h> ... | <source.c> ... | <dependency> ... ]
#                 [ ( HEADERS | HDR ) <header1> [ <header2> ...] ]
#                 [ ( SOURCES | SRC ) <source1> [ <source2> ... ] ]
#                 [ ( DEPENDS | DEP ) <dependency1> [ <dependency2> ... ] ]
#                 [ ( PACKAGES | PKG ) <package1> [ <package2> ... ] ]
#                 [ ( INCLUDE_PATH | INC_PATH ) ( [ VALUE ] <includepath1> [ <includepath2> ... ] | VALUEOF <varname1> [ <varname2> ... ] ) ]
#                 [ ( LIBRARY_PATH | LIB_PATH ) ( [ VALUE ] <linkpath1> [ <linkpath2> ... ] | VALUEOF <varname1> [ <varname2> ... ] ) ]
#                 [ ( LINK | LNK ) ( [ VALUE ] <library1> [ <library2> ... ] | VALUEOF <varname1> [ <varname2> ... ] ) ]
#             )
#
# Purpose:    Defines a software component from a number of source and/or header files and dependencies.
#             Component names should follow the naming conventions specified in the description of the <component-name>
#             parameter below.
#             If multiple components with the same name are defined across the build system, their properties
#             (e.g., source files, header files, dependencies) will be merged in order of definition.
#             This is useful for defining a general interface for a component in one directory of the
#             build system, while the (possibly platform dependent) implementation is defined in a
#             different directory. For examples, see the "xme/hal" directory (generic header files) and
#             the subdirectories of "xme/ports" (platform dependent implementations).
#
#             For each software component with at least one source file, a static linker library will be built.
#             Components with only header files or no files at all are considered "virtual" (such components can
#             serve as an abstraction for a number of dependencies, for example). Since such components do not generate
#             any target files (i.e., static linker libraries), they will not be linked against when referenced by
#             other components (however their dependencies in turn will be considered for linking).
#
# Parameters: <component-name>           Name of the software component. You should use the following naming conventions:
#                                         - Name should only contain the following types of characters: "a-z", "0-9", "_".
#                                         - Name should start with "xme_<level>_", where <level> is one of the following:
#                                            * "adv" if the component is a high-level, platform independent component with
#                                              no direct interaction with the hardware abstraction layer (HAL).
#                                            * "prim" if the component directly calls function from the hardware
#                                              abstraction layer (HAL).
#                                            * "hal" if the component implements hardware abstraction functionality.
#                                            * "core" if the component provides system-internal functionality (should only
#                                              be used by XME core components).
#
#             <header.h> ...             Name(s) of C header file(s) to use for building the component (including ".h" suffix).
#                                        All files specified in this way will be automatically interpreted as header files
#                                        if their file extension is ".h".
#
#             <source.c> ...             Name(s) of C source file(s) to use for building the component (including ".c" suffix).
#                                        All files specified in this way will be automatically interpreted as source files
#                                        if their file extension is ".c".
#
#             <dependency> ...           Name(s) of other XME component(s) that is/are required for this component to work
#                                        properly. This means that the new component will be linked against the referenced
#                                        component and that include directories of the referenced component will be available
#                                        in the new component. All items specified in this way that have no file extension
#                                        will be automatically interpreted as dependencies.
#
#             ( HEADERS | HDR ) <header1> [ <header2> ...]
#                                        The "HEADERS" or "HDR" keywords indicate that the following items shall be
#                                        interpreted as header files to use for building the component.
#
#             ( SOURCES | SRC ) <source1> [ <source2> ... ]
#                                        The "SOURCES" or "SRC" keywords indicate that the following items shall be
#                                        interpreted as source files to use for building the component.
#
#             ( DEPENDS | DEP ) <dependency1> [ <dependency2> ... ]
#                                        The "DEPENDS" or "DEP" keywords indicate that the following items shall be
#                                        interpreted as names of other components that are required for this component to work
#                                        properly. This means that the new component will be linked against the referenced
#                                        component and that include directories of the referenced component will be available
#                                        in the new component.
#
#             ( PACKAGES | PKG ) <package1> [ <package2> ... ]
#                                        The "PACKAGES" or "PKG" keywords indicate that the following items shall be
#                                        interpreted as names of CMake packets that are required for this component to work
#                                        properly. This means that a find_packet() will be issued as soon as the new component
#                                        is used in a project. Configuring the build system will fail if the respective packet
#                                        has not been found, i.e., the packet is marked as "REQUIRED". Variables defined by
#                                        the referenced packet can be used as input to the INCLUDE_PATH, LIBRARY_PATH and/or
#                                        LINK parameters (see below).
#
#             ( INCLUDE_PATH | INC_PATH ) ( [ VALUE ] <includepath1> [ <includepath2> ... ] ] [  | VALUEOF <varname1> [ <varname2> ... ] )
#                                        The "INCLUDE_PATH" or "INC_PATH" keywords indicate that the following items shall be
#                                        interpreted as one of the following:
#                                         - If the "VALUEOF" keyword is present in front of an item, the name of a variable
#                                           containing the name of one or multiple include paths to use when building the
#                                           component.
#                                           This is useful if the actual value is not know during initial configuration of the
#                                           build system, for example because it if only valid after a package referenced with
#                                           the "PACKAGES" keyword has been found.
#                                         - If the "VALUE" keyword if present in front of an item, or the "VALUEOF" keyword
#                                           is not present in between the "INCLUDE_PATH"/"INC_PATH" keyword and the respective
#                                           item, the name of an include path to use when building the component.
#
#             ( LIBRARY_PATH | LIB_PATH ) ( [ VALUE ] <linkpath1> [ <linkpath2> ... ] | VALUEOF <varname1> [ <varname2> ... ] )
#                                        The "LIBRARY_PATH" or "LIB_PATH" keywords indicate that the following items shall be
#                                        interpreted as one of the following:
#                                         - If the "VALUEOF" keyword is present in front of an item, the name of a variable
#                                           containing the name of one or multiple library paths to use when building the
#                                           component.
#                                           This is useful if the actual value is not know during initial configuration of the
#                                           build system, for example because it if only valid after a package referenced with
#                                           the "PACKAGES" keyword has been found.
#                                         - If the "VALUE" keyword if present in front of an item, or the "VALUEOF" keyword
#                                           is not present in between the "LIBRARY_PATH"/"LIB_PATH" keyword and the respective
#                                           item, the name of a library path to use when building the component.
#
#             ( LINK | LNK ) ( [ VALUE ] <library1> [ <library2> ... ] | VALUEOF <varname1> [ <varname2> ... ] )
#                                        The "LINK" or "LNK" keywords indicate that the following items shall be interpreted as
#                                        one of the following:
#                                         - If the "VALUEOF" keyword is present in front of an item, the name of a variable
#                                           containing the name of one or multiple linker libraries (static or dynamic) to use
#                                           when building the component.
#                                           This is useful if the actual value is not know during initial configuration of the
#                                           build system, for example because it if only valid after a package referenced with
#                                           the "PACKAGES" keyword has been found.
#                                         - If the "VALUE" keyword if present in front of an item, or the "VALUEOF" keyword
#                                           is not present in between the "LINK"/"LNK" keyword and the respective item, the
#                                           name of a linker library (static or dynamic) to use when building the component.
#
macro (xme_add_component)
	set (__USAGE__ "Usage: xme_add_component ( <component-name> [ <header.h> ... | <source.c> ... | <dependency> ... ] [ ( HEADERS | HDR ) <header1> [ <header2> ...] ] [ ( SOURCES | SRC ) <source1> [ <source2> ... ] ] [ ( DEPENDS | DEP ) <dependency1> [ <dependency2> ... ] ] [ ( PACKAGES | PKG ) <package1> [ <package2> ... ] ] [ ( INCLUDE_PATH | INC_PATH ) ( [ VALUE ] <includepath1> [ <includepath2> ... ] | VALUEOF <varname1> [ <varname2> ... ] ) ] [ ( LIBRARY_PATH | LIB_PATH ) ( [ VALUE ] <linkpath1> [ <linkpath2> ... ] | VALUEOF <varname1> [ <varname2> ... ] ) ] [ ( LINK | LNK ) ( [ VALUE ] <library1> [ <library2> ... ] | VALUEOF <varname1> [ <varname2> ... ] ) ] )")

	if (${ARGC} LESS 1)
		message (FATAL_ERROR ${__USAGE__})
	endif (${ARGC} LESS 1)

	set (__ARGS__ ${ARGV})
	list (GET __ARGS__ 0 __COMP_NAME__)
	list (REMOVE_AT __ARGS__ 0)
	set (__COMP_NAME_LOWER__ ${__COMP_NAME__})
	string (TOUPPER ${__COMP_NAME__} __COMP_NAME_UPPER__)

	list (LENGTH __ARGS__ __ARGC__)
	if (${__ARGC__} LESS 1)
		message (FATAL_ERROR ${__USAGE__})
	endif (${__ARGC__} LESS 1)

	xme_get (_XME_CURRENT_SOURCE_DIR PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR)
	set (__MODE__ 0)
	set (__BYVALUE__ TRUE)
	set (__HDR__)
	set (__SRC__)
	set (__ASM_SRC__)
	set (__DEP__)
	set (__PKG__)
	set (__INP__)
	set (__LIP__)
	set (__LNK__)
	set (__WARN_IF_NEW_MODE__ FALSE)
	foreach (ARG ${__ARGS__})
		if (ARG STREQUAL "HEADERS" OR ARG STREQUAL "HDR")
			set (__MODE__ 1)
			if (__WARN_IF_NEW_MODE__)
				message (WARNING "Found section with no input statements before '${ARG}' in definition of component '${__COMP_NAME_LOWER__}' in file '${CMAKE_CURRENT_LIST_FILE}'! Maybe you are missing a parameter or is it set to an empty value?")
			endif (__WARN_IF_NEW_MODE__)
			set (__WARN_IF_NEW_MODE__ TRUE)
		elseif (ARG STREQUAL "SOURCES" OR ARG STREQUAL "SRC")
			set (__MODE__ 2)
			if (__WARN_IF_NEW_MODE__)
				message (WARNING "Section with no input statements before '${ARG}' in definition of component '${__COMP_NAME_LOWER__}' in file '${CMAKE_CURRENT_LIST_FILE}'! Maybe you are missing a parameter or is it set to an empty value?")
			endif (__WARN_IF_NEW_MODE__)
			set (__WARN_IF_NEW_MODE__ TRUE)
		elseif (ARG STREQUAL "DEPENDS" OR ARG STREQUAL "DEP")
			set (__MODE__ 3)
			if (__WARN_IF_NEW_MODE__)
				message (WARNING "Section with no input statements before '${ARG}' in definition of component '${__COMP_NAME_LOWER__}' in file '${CMAKE_CURRENT_LIST_FILE}'! Maybe you are missing a parameter or is it set to an empty value?")
			endif (__WARN_IF_NEW_MODE__)
			set (__WARN_IF_NEW_MODE__ TRUE)
		elseif (ARG STREQUAL "PACKAGES" OR ARG STREQUAL "PKG")
			set (__MODE__ 4)
			if (__WARN_IF_NEW_MODE__)
				message (WARNING "Section with no input statements before '${ARG}' in definition of component '${__COMP_NAME_LOWER__}' in file '${CMAKE_CURRENT_LIST_FILE}'! Maybe you are missing a parameter or is it set to an empty value?")
			endif (__WARN_IF_NEW_MODE__)
			set (__WARN_IF_NEW_MODE__ TRUE)
		elseif (ARG STREQUAL "INCLUDE_PATH" OR ARG STREQUAL "INC_PATH")
			set (__MODE__ 5)
			if (__WARN_IF_NEW_MODE__)
				message (WARNING "Section with no input statements before '${ARG}' in definition of component '${__COMP_NAME_LOWER__}' in file '${CMAKE_CURRENT_LIST_FILE}'! Maybe you are missing a parameter or is it set to an empty value?")
			endif (__WARN_IF_NEW_MODE__)
			set (__WARN_IF_NEW_MODE__ TRUE)
		elseif (ARG STREQUAL "LIBRARY_PATH" OR ARG STREQUAL "LIB_PATH")
			set (__MODE__ 6)
			if (__WARN_IF_NEW_MODE__)
				message (WARNING "Section with no input statements before '${ARG}' in definition of component '${__COMP_NAME_LOWER__}' in file '${CMAKE_CURRENT_LIST_FILE}'! Maybe you are missing a parameter or is it set to an empty value?")
			endif (__WARN_IF_NEW_MODE__)
			set (__WARN_IF_NEW_MODE__ TRUE)
		elseif (ARG STREQUAL "LINK" OR ARG STREQUAL "LNK")
			set (__MODE__ 7)
			if (__WARN_IF_NEW_MODE__)
				message (WARNING "Section with no input statements before '${ARG}' in definition of component '${__COMP_NAME_LOWER__}' in file '${CMAKE_CURRENT_LIST_FILE}'! Maybe you are missing a parameter or is it set to an empty value?")
			endif (__WARN_IF_NEW_MODE__)
			set (__WARN_IF_NEW_MODE__ TRUE)
		elseif (ARG STREQUAL "VALUE")
			set (__BYVALUE__ TRUE)
		elseif (ARG STREQUAL "VALUEOF")
			set (__BYVALUE__ FALSE)
		else (ARG STREQUAL "HEADERS" OR ARG STREQUAL "HDR")
			set (__WARN_IF_NEW_MODE__ FALSE)

			if (IS_ABSOLUTE ${ARG})
				set (_FILE ${ARG})
			else (IS_ABSOLUTE ${ARG})
				set (_FILE ${_XME_CURRENT_SOURCE_DIR}/${ARG})
			endif (IS_ABSOLUTE ${ARG})

			if (__MODE__ EQUAL 0)
				# Auto-detect item type
				#  - *.c is treated as a source file
				#  - *.h is treated as a header file
				#  - anything else is treated as a component dependency
				string (REGEX MATCH "^.*\\.(.+)$" __AUTO_MATCH__ ${ARG})
				if (__AUTO_MATCH__)
					if (NOT EXISTS ${_FILE})
						if (IS_ABSOLUTE ${ARG})
							message (SEND_ERROR "Unable to find file '${_FILE}' for component '${__COMP_NAME_LOWER__}'!")
						else (IS_ABSOLUTE ${ARG})
							message (SEND_ERROR "Unable to find file '${ARG}' in '${_XME_CURRENT_SOURCE_DIR}' for component '${__COMP_NAME_LOWER__}'!")
						endif (IS_ABSOLUTE ${ARG})
					else (NOT EXISTS ${_FILE})
						if (${CMAKE_MATCH_1} STREQUAL "h")
							# Header file
							list (APPEND __HDR__ ${_FILE})
						elseif (${CMAKE_MATCH_1} STREQUAL "c")
							# Source file
							list (APPEND __SRC__ ${_FILE})
						elseif ((${CMAKE_MATCH_1} STREQUAL "S") OR (${CMAKE_MATCH_1} STREQUAL "s"))
							# Source file
							list (APPEND __ASM_SRC__ ${_FILE})
						else (${CMAKE_MATCH_1} STREQUAL "h")
							message (SEND_ERROR "Unable to determine type of file '${ARG}' for component '${__COMP_NAME_LOWER__}'!")
						endif (${CMAKE_MATCH_1} STREQUAL "h")
					endif (NOT EXISTS ${_FILE})
				else (__AUTO_MATCH__)
					# Dependency
					list (APPEND __DEP__ ${ARG})
				endif (__AUTO_MATCH__)
			elseif (__MODE__ EQUAL 1)
				# Header file
				if (NOT EXISTS ${_FILE})
					if (IS_ABSOLUTE ${ARG})
						message (SEND_ERROR "Unable to find file '${_FILE}' for component '${__COMP_NAME_LOWER__}'!")
					else (IS_ABSOLUTE ${ARG})
						message (SEND_ERROR "Unable to find file '${ARG}' in '${_XME_CURRENT_SOURCE_DIR}' for component '${__COMP_NAME_LOWER__}'!")
					endif (IS_ABSOLUTE ${ARG})
				else (NOT EXISTS ${_FILE})
					list (APPEND __HDR__ ${_FILE})
				endif (NOT EXISTS ${_FILE})
			elseif (__MODE__ EQUAL 2)
				# Source file
				if (NOT EXISTS ${_FILE})
					if (IS_ABSOLUTE ${ARG})
						message (SEND_ERROR "Unable to find file '${_FILE}' for component '${__COMP_NAME_LOWER__}'!")
					else (IS_ABSOLUTE ${ARG})
						message (SEND_ERROR "Unable to find file '${ARG}' in '${_XME_CURRENT_SOURCE_DIR}' for component '${__COMP_NAME_LOWER__}'!")
					endif (IS_ABSOLUTE ${ARG})
				else (NOT EXISTS ${_FILE})
					list (APPEND __SRC__ ${_FILE})
				endif (NOT EXISTS ${_FILE})
			elseif (__MODE__ EQUAL 3)
				# Dependency
				list (APPEND __DEP__ ${ARG})
			elseif (__MODE__ EQUAL 4)
				# Package dependency
				list (APPEND __PKG__ ${ARG})
			elseif (__MODE__ EQUAL 5)
				# Include path variable name
				list (APPEND __INP__ ${__BYVALUE__} ${ARG})
			elseif (__MODE__ EQUAL 6)
				# Library path variable name
				list (APPEND __LIP__ ${__BYVALUE__} ${ARG})
			else (__MODE__ EQUAL 0)
				# Link library
				list (APPEND __LNK__ ${__BYVALUE__} ${ARG})
			endif (__MODE__ EQUAL 0)
		endif (ARG STREQUAL "HEADERS" OR ARG STREQUAL "HDR")
	endforeach (ARG)

	if (__WARN_IF_NEW_MODE__)
		message (WARNING "Definition of component '${__COMP_NAME_LOWER__}' in file '${CMAKE_CURRENT_LIST_FILE}' ended with a keyword! Maybe you are missing a parameter or is it set to an empty value?")
	endif (__WARN_IF_NEW_MODE__)

	xme_append (PROPERTY_GLOBAL "${__COMP_NAME_UPPER__}_HEADERS" "${__HDR__}")
	xme_append (PROPERTY_GLOBAL "${__COMP_NAME_UPPER__}_SOURCES" "${__SRC__}")
	xme_append (PROPERTY_GLOBAL "${__COMP_NAME_UPPER__}_SOURCES" "${__ASM_SRC__}")
	xme_append (PROPERTY_GLOBAL "${__COMP_NAME_UPPER__}_DEPENDS" "${__DEP__}")
	xme_append (PROPERTY_GLOBAL "${__COMP_NAME_UPPER__}_PACKAGES" "${__PKG__}")
	xme_append (PROPERTY_GLOBAL "${__COMP_NAME_UPPER__}_INCLUDE_PATHS" "${__INP__}")
	xme_append (PROPERTY_GLOBAL "${__COMP_NAME_UPPER__}_LIBRARY_PATHS" "${__LIP__}")
	xme_append (PROPERTY_GLOBAL "${__COMP_NAME_UPPER__}_LINKS" "${__LNK__}")

	xme_append (PROPERTY_GLOBAL XME_DOCUMENTATION_FILES "${__HDR__}")
	xme_append (PROPERTY_GLOBAL XME_DOCUMENTATION_FILES "${__SRC__}")
	xme_append (PROPERTY_GLOBAL XME_DOCUMENTATION_FILES "${__ASM_SRC__}")
	
	# Add install rule for included files and recursively add all files (but not directories) in
	# This is done to install sources which are not added via xme_add_subdirectory(), e.g.
	# the generic platform specific code which is included by the corresponding platform.
	#
	
	# TODO: Make install prefix an optional argument to xme_add_component() (defaulting
	#       to XME_ROOT). This could be used to install components that are outside of
	#       XME_ROOT, and which are currently not installed.
	xme_install_rec("src" "${XME_ROOT}" "${__HDR__}" "${__SRC__}" "${__ASM_SRC__}")
	
	#source_group ("Header Files" FILES ${${__COMP_NAME_UPPER__}_HEADERS})
	#source_group ("Source Files" FILES ${${__COMP_NAME_UPPER__}_SOURCES})

	# Add "Build System" source group (currently only CMakeLists.txt file)
	set (__CMAKELISTS__ "${_XME_CURRENT_SOURCE_DIR}/CMakeLists.txt")
	set ("${__COMP_NAME_UPPER__}_BUILDSYSTEM" ${${__COMP_NAME_UPPER__}_BUILDSYSTEM} ${__CMAKELISTS__})
	source_group ("Build System" FILES ${__CMAKELISTS__})

	# TODO: Use __COMP_NAME_LOWER__ instead of __COMP_NAME__?
	xme_get (_TMP_ PROPERTY_GLOBAL "${__COMP_NAME_UPPER__}_HEADERS")
	#message (STATUS "Updated '${__COMP_NAME__}' header files to: '${_TMP_}'")
	# TODO: Use __COMP_NAME_LOWER__ instead of __COMP_NAME__?
	xme_get (_TMP_ PROPERTY_GLOBAL "${__COMP_NAME_UPPER__}_SOURCES")
	#message (STATUS "Updated '${__COMP_NAME__}' source files to: '${_TMP_}'")

	# Accumulating sources across multiple directories also with
	# add_subdirectory() since we are using global properties to propagate the values.
	xme_get (__XME_COMPONENTS__ PROPERTY_GLOBAL XME_COMPONENTS)
	list (FIND __XME_COMPONENTS__ ${__COMP_NAME_LOWER__} __XME_COMPONENT_FOUND__)
	if (__XME_COMPONENT_FOUND__ LESS 0)
		# TODO: Use __COMP_NAME_LOWER__ instead?
		message (STATUS "Found XME component '${__COMP_NAME__}'")
		# TODO: Use __COMP_NAME_LOWER__ instead?
		list (APPEND __XME_COMPONENTS__ ${__COMP_NAME__})
		xme_set (PROPERTY_GLOBAL XME_COMPONENTS "${__XME_COMPONENTS__}")
	endif (__XME_COMPONENT_FOUND__ LESS 0)

endmacro (xme_add_component)


macro (xme_definition)
	# Append definitions to global list
	xme_get (_DEFINITIONS PROPERTY_GLOBAL XME_DEFINITIONS)
	foreach (ARG ${ARGV})
		list (FIND _DEFINITIONS ${ARG} _FOUND)
		if (${_FOUND} EQUAL -1)
			xme_append (PROPERTY_GLOBAL XME_DEFINITIONS "${ARG}")
		endif (${_FOUND} EQUAL -1)
	endforeach (ARG)

	# Ensure that add_definitions() is called with all definitions
	# specified so far. This is important because add_subdirectory() defines
	# a new scope.
	xme_get (_DEFINITIONS PROPERTY_GLOBAL XME_DEFINITIONS)

	foreach (_DEFINITION ${_DEFINITIONS})
		add_definitions(${_DEFINITION})
		message (STATUS "Added definition '${_DEFINITION}'.")
	endforeach (_DEFINITION)
endmacro (xme_definition)

macro (xme_include_directory)
	xme_get (_XME_CURRENT_SOURCE_DIR PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR)
	set (__ARGS__ ${ARGV})
	if (${ARGC} LESS 1)
		set (__ARGS__ ${_XME_CURRENT_SOURCE_DIR})
	endif (${ARGC} LESS 1)

	# Append new include directories to global list of include directories
	# (as absolute path)
	xme_get (_INCLUDE_DIRECTORIES PROPERTY_GLOBAL XME_INCLUDE_DIRECTORIES)
	foreach (ARG ${__ARGS__})
		if (NOT IS_ABSOLUTE ${ARG})
			set (ARG "${_XME_CURRENT_SOURCE_DIR}/${ARG}")
		endif (NOT IS_ABSOLUTE ${ARG})
		xme_simplify_path (ARG "${ARG}")

		list (FIND _INCLUDE_DIRECTORIES ${ARG} _FOUND)
		if (${_FOUND} EQUAL -1)
			xme_append (PROPERTY_GLOBAL XME_INCLUDE_DIRECTORIES "${ARG}")
		endif (${_FOUND} EQUAL -1)
	endforeach (ARG)

	# Ensure that include_directories() is called with all include directories
	# defined so far. This is important because add_subdirectory() defines
	# a new scope.
	xme_get (_INCLUDE_DIRECTORIES PROPERTY_GLOBAL XME_INCLUDE_DIRECTORIES)

	foreach (_INCLUDE_DIRECTORY ${_INCLUDE_DIRECTORIES})
		include_directories(${_INCLUDE_DIRECTORY})
		message (STATUS "Added include directory '${_INCLUDE_DIRECTORY}'.")
	endforeach (_INCLUDE_DIRECTORY)
endmacro (xme_include_directory)


macro (xme_lib_directory)
	xme_get (_XME_CURRENT_SOURCE_DIR PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR)
	set (__ARGS__ ${ARGV})
	if (${ARGC} LESS 1)
		set (__ARGS__ ${_XME_CURRENT_SOURCE_DIR})
	endif (${ARGC} LESS 1)

	# Append new library directories to global list of library directories
	# (as absolute path)
	xme_get (_LIBRARY_DIRECTORIES PROPERTY_GLOBAL XME_LIBRARY_DIRECTORIES)
	foreach (ARG ${__ARGS__})
		if (NOT IS_ABSOLUTE ${ARG})
			set (ARG "${_XME_CURRENT_SOURCE_DIR}/${ARG}")
		endif (NOT IS_ABSOLUTE ${ARG})
		xme_simplify_path (ARG "${ARG}")

		list (FIND _LIBRARY_DIRECTORIES ${ARG} _FOUND)
		if (${_FOUND} EQUAL -1)
			xme_append (PROPERTY_GLOBAL XME_LIBRARY_DIRECTORIES "${ARG}")
		endif (${_FOUND} EQUAL -1)
	endforeach (ARG)

	# Ensure that link_directories() is called with all link directories
	# defined so far. This is important because add_subdirectory() defines
	# a new scope.
	xme_get (_LIBRARY_DIRECTORIES PROPERTY_GLOBAL XME_LIBRARY_DIRECTORIES)

	foreach (_LIBRARY_DIRECTORY ${_LIBRARY_DIRECTORIES})
		link_directories (${_LIBRARY_DIRECTORY})
		message (STATUS "Added library directory '${_LIBRARY_DIRECTORY}' at scope '${CMAKE_CURRENT_LIST_FILE}'")
	endforeach (_LIBRARY_DIRECTORY)
endmacro (xme_lib_directory)



# Macro:      xme_add_executable ( <target-name> [ <header.h> ... | <source.c> ... ] )
#
# Purpose:    Specifies that an executable target is to be built from the given sources and header files.
#             The list of source and header files typically consists only of the main() program with the
#             main loop. All other required software components should be linked to the executable target
#             by using the xme_link_components() macro.
#
# Parameters: <target-name>              Name of the executable target to create. Depending on the
#                                        execution platform, an extension might be appended to the name
#                                        automatically.
#
#             [ <header.h> ... | <source.c> ... ]
#                                        Name(s) of C header and source file(s) to use for building the
#                                        respective target.
#
macro (xme_add_executable TARGET)
	set (__FILES__ ${ARGV})
	list (REMOVE_AT __FILES__ 0)

	xme_get (_XME_CURRENT_SOURCE_DIR PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR)

	if (NOT ${_XME_CURRENT_SOURCE_DIR} EQUAL "")
		set (__ABS_FILES__)
		foreach (FILE ${__FILES__})
			if (NOT EXISTS "${_XME_CURRENT_SOURCE_DIR}/${FILE}")
				message (SEND_ERROR "Unable to find file '${FILE}' in '${_XME_CURRENT_SOURCE_DIR}' for executable '${TARGET}'!")
			else (NOT EXISTS "${_XME_CURRENT_SOURCE_DIR}/${FILE}")
				list (APPEND __ABS_FILES__ "${_XME_CURRENT_SOURCE_DIR}/${FILE}")
			endif (NOT EXISTS "${_XME_CURRENT_SOURCE_DIR}/${FILE}")
		endforeach (FILE)
	else (NOT ${_XME_CURRENT_SOURCE_DIR} EQUAL "")
		foreach (FILE ${__FILES__})
			if (NOT EXISTS "${FILE}")
				message (SEND_ERROR "Unable to find file '${FILE}' for executable '${TARGET}'!")
			else (NOT EXISTS "${FILE}")
				list (APPEND __ABS_FILES__ "${FILE}")
			endif (NOT EXISTS "${FILE}")
		endforeach (FILE ${__FILES__})
	endif (NOT ${_XME_CURRENT_SOURCE_DIR} EQUAL "")

	# Define target
	add_executable (${TARGET} ${__ABS_FILES__})
	message (STATUS "Generated executable target '${TARGET}'")
	
	# Add install rule for executable
    xme_get (__XME_PLATFORM__ PROPERTY_GLOBAL XME_PLATFORM)
    xme_get (__XME_ARCHITECTURE__ PROPERTY_GLOBAL XME_ARCHITECTURE)
    install(
        TARGETS
        ${TARGET}
        DESTINATION
        "bin/${__XME_PLATFORM__}_${__XME_ARCHITECTURE__}"
    )
    
	# We want to install example project directories but not the tests directory which is installed seperately
	string (COMPARE NOTEQUAL ${_XME_CURRENT_SOURCE_DIR} ${XME_TESTS_DIR} __IS_NOT_EQUAL__)
	if (${__IS_NOT_EQUAL__})
		install(
			DIRECTORY
			${_XME_CURRENT_SOURCE_DIR}
			DESTINATION
			"src/examples"
			PATTERN "*/build*" EXCLUDE
		)
	endif (${__IS_NOT_EQUAL__})
	
	# Create map file
	xme_defined (_XME_LINKER_FLAGS_MAP_FILE_DEFINED PROPERTY_GLOBAL XME_LINKER_FLAGS_MAP_FILE)
	if (${_XME_LINKER_FLAGS_MAP_FILE_DEFINED})
		xme_get (_XME_LINKER_FLAGS_MAP_FILE PROPERTY_GLOBAL XME_LINKER_FLAGS_MAP_FILE)
		set_target_properties (${TARGET} PROPERTIES LINK_FLAGS "${_XME_LINKER_FLAGS_MAP_FILE}${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.map")
		set (_CLEAN_UP ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.map)
	endif (${_XME_LINKER_FLAGS_MAP_FILE_DEFINED})

	xme_defined (_XME_IMAGE_FORMAT_DEFINED PROPERTY_GLOBAL XME_IMAGE_FORMAT)

	# Create firmware image and disassembly
	if (${_XME_IMAGE_FORMAT_DEFINED})
		xme_get (_XME_IMAGE_FORMAT PROPERTY_GLOBAL XME_IMAGE_FORMAT)
		xme_get (_XME_IMAGE_EXTENSION PROPERTY_GLOBAL XME_IMAGE_EXTENSION)
		set (_IMAGE_FILE ${TARGET}.${_XME_IMAGE_EXTENSION})
		set (_DUMP_FILE ${TARGET}_dump.${_XME_IMAGE_EXTENSION})

		# Use tempory file ${TARGET}.tmp in order to be able to attach
		# objcopy run to current executable target
		add_custom_command(TARGET ${TARGET} POST_BUILD
			COMMAND ${CMAKE_OBJCOPY} --output-format=${_XME_IMAGE_FORMAT} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_IMAGE_FILE}
			COMMENT "Objcopying ${TARGET} to ${_IMAGE_FILE}")
		list (APPEND _CLEAN_UP ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_IMAGE_FILE})

		# If the user requested an image file, also generate a disassembly
		# since chances are good that we are building for an embedded platform
		add_custom_command(TARGET ${TARGET} POST_BUILD
			COMMAND ${CMAKE_OBJDUMP} --all-headers --source ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET} > ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.txt
			COMMENT "Objdumping ${TARGET} to ${TARGET}.txt (-> headers, disassembly)")
		list (APPEND _CLEAN_UP ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TARGET}.txt)
	endif (${_XME_IMAGE_FORMAT_DEFINED})

	# Add target that allow to upload the image
	xme_defined (XME_DEBUGGER_WRAPPER_DEFINED PROPERTY_GLOBAL XME_DEBUGGER_WRAPPER)
	if (XME_DEBUGGER_WRAPPER_DEFINED)
		xme_get (_XME_DEBUGGER_WRAPPER PROPERTY_GLOBAL XME_DEBUGGER_WRAPPER)
		xme_get (_XME_DEBUGGER_TARGET PROPERTY_GLOBAL XME_DEBUGGER_TARGET)
		xme_get (_XME_TARGET_FLASH_ADDRESS PROPERTY_GLOBAL XME_TARGET_FLASH_ADDRESS)
		xme_get (_XME_IMAGE_FLASH_ADDRESS PROPERTY_GLOBAL XME_IMAGE_FLASH_ADDRESS)
		xme_get (_XME_TARGET_FLASH_SIZE PROPERTY_GLOBAL XME_TARGET_FLASH_SIZE)

		# Add target that allow to upload the image
		# TODO: We currently erase the entire chip although this might not always be desireable.
		add_custom_target (upload ${_XME_DEBUGGER_WRAPPER} -ex \"target ${_XME_DEBUGGER_TARGET}\" -ex \"monitor halt\" -ex \"monitor flash erase_address ${_XME_TARGET_FLASH_ADDRESS} ${_XME_TARGET_FLASH_SIZE}\" -ex \"monitor halt\" -ex \"monitor flash write_image ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_IMAGE_FILE} ${_XME_IMAGE_FLASH_ADDRESS} ${_XME_IMAGE_EXTENSION}\" -ex \"monitor verify_image ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_IMAGE_FILE} ${_XME_IMAGE_FLASH_ADDRESS} ${_XME_IMAGE_EXTENSION}\" -ex \"monitor reset init\" -ex \"monitor halt\" -ex \"disconnect\" -ex \"quit\" DEPENDS ${TARGET}.tmp.img)

		# Add target that allow to dump the image to a file
		add_custom_target (dump ${_XME_DEBUGGER_WRAPPER} -ex \"target ${_XME_DEBUGGER_TARGET}\" -ex \"monitor halt\" -ex \"monitor dump_image ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_DUMP_FILE} ${_XME_TARGET_FLASH_ADDRESS} ${_XME_TARGET_FLASH_SIZE}\"  -ex \"monitor reset init\" -ex \"monitor halt\" -ex \"disconnect\" -ex \"quit\" DEPENDS ${TARGET}.tmp.img)

		list (APPEND _CLEAN_UP ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_DUMP_FILE})

		# For applications with bootloader support, create bootloader target
		xme_defined(XME_BUILDING_APPLICATION_FOR_BOOTLOADER_DEFINED PROPERTY_GLOBAL XME_BUILDING_APPLICATION_FOR_BOOTLOADER)
		xme_defined(XME_NODE_IP_ADDRESS_DEFINED PROPERTY_GLOBAL XME_NODE_IP_ADDRESS)

		if (XME_BUILDING_APPLICATION_FOR_BOOTLOADER_DEFINED AND XME_NODE_IP_ADDRESS_DEFINED)
			xme_get(_XME_NODE_IP_ADDRESS PROPERTY_GLOBAL XME_NODE_IP_ADDRESS)
			add_custom_target (bootloader ${TFTP} -i  ${_XME_NODE_IP_ADDRESS} PUT ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${_IMAGE_FILE} DEPENDS ${TARGET}.tmp.img)
		endif (XME_BUILDING_APPLICATION_FOR_BOOTLOADER_DEFINED AND XME_NODE_IP_ADDRESS_DEFINED)

		xme_defined (XME_ECLIPSE_LAUNCHES_DIR_DEFINED PROPERTY_GLOBAL XME_ECLIPSE_LAUNCHES_DIR)
		if (XME_ECLIPSE_LAUNCHES_DIR_DEFINED)
			xme_get (ECLIPSE_LAUNCHES_DIR PROPERTY_GLOBAL XME_ECLIPSE_LAUNCHES_DIR)
			xme_get (DEBUGGER_CFG PROPERTY_GLOBAL XME_DEBUGGER_CFG)
			set (DEBUG_TARGET ${TARGET})

			string (FIND ${CMAKE_SOURCE_DIR} "/" _POS REVERSE)
			math (EXPR _POS "${_POS} + 1")
			string (SUBSTRING ${CMAKE_SOURCE_DIR} ${_POS} -1 _DIR)

			set (PROJECT_ATTR "${PROJECT_NAME}-${CMAKE_BUILD_TYPE}@${_DIR}")

			# Configure launch configuration that allows to debug the target
			configure_file ("${XME_TOOLS_DIR}/toolchain/eclipse/GDB.launch.in" "${ECLIPSE_LAUNCHES_DIR}/GDB-${TARGET}.launch" @ONLY)

			# Configure launch configuration that allows to upload the target
			configure_file ("${XME_TOOLS_DIR}/toolchain/eclipse/Upload.launch.in" "${ECLIPSE_LAUNCHES_DIR}/Upload-${TARGET}.launch" @ONLY)

			# Configure launch configuration that allows to download the target and dump it to a file.
			configure_file ("${XME_TOOLS_DIR}/toolchain/eclipse/Dump.launch.in" "${ECLIPSE_LAUNCHES_DIR}/Dump-${TARGET}.launch" @ONLY)

			if (XME_BUILDING_APPLICATION_FOR_BOOTLOADER_DEFINED AND XME_NODE_IP_ADDRESS_DEFINED)
				find_package(TFTP)
				if (TFTP_FOUND)
					xme_get(_XME_NODE_IP_ADDRESS PROPERTY_GLOBAL XME_NODE_IP_ADDRESS)
					configure_file ("${XME_TOOLS_DIR}/toolchain/eclipse/Bootloader.launch.in" "${ECLIPSE_LAUNCHES_DIR}/Bootloader-${TARGET}.launch" @ONLY)
				else (TFTP_FOUND)
					message ("TFTP not found: Cannot use bootloader.")
				endif (TFTP_FOUND)
			endif (XME_BUILDING_APPLICATION_FOR_BOOTLOADER_DEFINED AND XME_NODE_IP_ADDRESS_DEFINED)
		endif (XME_ECLIPSE_LAUNCHES_DIR_DEFINED)
	endif (XME_DEBUGGER_WRAPPER_DEFINED)

	# Ensure that additional files are considered during clean-up
	get_directory_property(_OLD_CLEAN_UP ADDITIONAL_MAKE_CLEAN_FILES)
	list (APPEND _OLD_CLEAN_UP ${_CLEAN_UP})
	set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${_OLD_CLEAN_UP}")

endmacro (xme_add_executable)



# Macro:      xme_link_components ( <target-name> [ <component-name> ... ] )
#
# Purpose:    Specifies that the named software component(s) should be linked to the specified
#             executable target after building. This command will also take care of resolving
#             all recursive dependencies between the components (as specified by the respective
#             call(s) to xme_add_component()) and schedule them for building if necessary.
#
# Parameters: <target-name>              Name of the executable target to link the software components
#                                        against.
#
#             [ <component-name> ... ]   Name(s) of software component(s) defined using xme_add_component()
#                                        to link against the executable target.
#
macro (xme_link_components TARGET)
	xme_get (_XME_COMPONENTS PROPERTY_GLOBAL XME_COMPONENTS)
	#message ("LINK '${ARGV}' COMPS '${_XME_COMPONENTS}'")
	xme_link_components_rec (${ARGV})
	#set (XME_COMPONENTS "")
endmacro (xme_link_components TARGET)

macro (xme_link_components_rec TARGET)
	set (__DEPENDS__ ${ARGV})
	list (REMOVE_AT __DEPENDS__ 0)

	xme_get (_XME_COMPONENTS PROPERTY_GLOBAL XME_COMPONENTS)
	#message ("LINK* '${TARGET}' against '${__DEPENDS__}' COMPS '${_XME_COMPONENTS}'")

	# __DEPENDS__ contains all direct dependencies of the TARGET.
	# Link the TARGET to these dependencies and then recursively
	# call xme_link_components() to add the dependencies of the
	# dependent libraries.

	foreach (__COMP__ ${__DEPENDS__})
		set (__COMP_LOWER__ ${__COMP__})
		string (TOUPPER ${__COMP__} __COMP_UPPER__)

		list (FIND _XME_COMPONENTS ${__COMP_LOWER__} __INDEX__)
		if (${__INDEX__} LESS 0)
			message (FATAL_ERROR "Target '${TARGET}' depends on unavailable component '${__COMP_LOWER__}'!")
		endif (${__INDEX__} LESS 0)

		xme_get (_COMPONENT_BUILT PROPERTY_GLOBAL XME_COMPONENT_BUILT_${__COMP_UPPER__})
		if (${_COMPONENT_BUILT})
			#message (STATUS "Skipping build of component '${__COMP_LOWER__}', since it has already been configured for build!")
		else (${_COMPONENT_BUILT})

			# If a component does not include any source files (e.g., only header files),
			# we add it to the project, but obviously we do not link against it, because
			# no real library can be generated and the component is more of a structural
			# sort. We call such a library a 'virtual' component.

			xme_get (_HEADERS_ PROPERTY_GLOBAL ${__COMP_UPPER__}_HEADERS)
			xme_get (_SOURCES_ PROPERTY_GLOBAL ${__COMP_UPPER__}_SOURCES)
			xme_get (_PACKAGES_ PROPERTY_GLOBAL ${__COMP_UPPER__}_PACKAGES)
			xme_get (_INCLUDE_PATHS_ PROPERTY_GLOBAL ${__COMP_UPPER__}_INCLUDE_PATHS)
			xme_get (_LIBRARY_PATHS_ PROPERTY_GLOBAL ${__COMP_UPPER__}_LIBRARY_PATHS)

			# Find required packages
			foreach (__CURPKG__ ${_PACKAGES_})
				message (STATUS "Importing package '${__CURPKG__}', which is required by component '${__COMP_LOWER__}'")
				find_package (${__CURPKG__} REQUIRED)
			endforeach (__CURPKG__)

			# Add include paths
			set (__IS_BY_VALUE__ TRUE)
			set (__BY_VALUE__)
			foreach (__PATH__ ${_INCLUDE_PATHS_})
				if (__IS_BY_VALUE__)
					# ${__PATH__} is YES if the next value is passed "by value" and NO if it is the name of a variable
					set (__BY_VALUE__ ${__PATH__})
					set (__IS_BY_VALUE__ FALSE)
				else (__IS_BY_VALUE__)
					if (__BY_VALUE__)
						# ${__PATH__} is the name of the include path
						xme_include_directory (${__PATH__})
					else (__BY_VALUE__)
						# ${__PATH__} is the name of the variable holding the include path
						if (NOT ${__PATH__})
							message(WARNING "Lazily evaluated variable '${__PATH__}', which is required for component '${__COMP_LOWER__}', is not defined!")
						else (NOT ${__PATH__})
							xme_include_directory (${${__PATH__}})
						endif (NOT ${__PATH__})
					endif (__BY_VALUE__)
					set (__IS_BY_VALUE__ TRUE)
				endif (__IS_BY_VALUE__)
			endforeach (__PATH__)

			# Add library paths
			set (__IS_BY_VALUE__ TRUE)
			set (__BY_VALUE__)
			foreach (__PATH__ ${_LIBRARY_PATHS_})
				if (__IS_BY_VALUE__)
					# ${__PATH__} is YES if the next value is passed "by value" and NO if it is the name of a variable
					set (__BY_VALUE__ ${__PATH__})
					set (__IS_BY_VALUE__ FALSE)
				else (__IS_BY_VALUE__)
					if (__BY_VALUE__)
						# ${__PATH__} is the name of the library path
						xme_lib_directory (${__PATH__})
					else (__BY_VALUE__)
						# ${__PATH__} is the name of the variable holding the library path
						if (NOT ${__PATH__})
							message(WARNING "Lazily evaluated variable '${__PATH__}', which is required for component '${__COMP_LOWER__}', is not defined!")
						else (NOT ${__PATH__})
							xme_lib_directory (${${__PATH__}})
						endif (NOT ${__PATH__})
					endif (__BY_VALUE__)
					set (__IS_BY_VALUE__ TRUE)
				endif (__IS_BY_VALUE__)
			endforeach (__PATH__)

			list (LENGTH _SOURCES_ __NUM_SOURCES__)
			if (${__NUM_SOURCES__} LESS 1)
				# Neither sources nor dependencies, this component is virtual
				message (STATUS "Generated target for virtual XME component '${__COMP_LOWER__}'")
				set ("XME_COMPONENT_VIRTUAL_${__COMP_UPPER__}" TRUE)
			else (${__NUM_SOURCES__} LESS 1)
				# Either sources or dependencies, this component is 'real'
				message (STATUS "Generated target for XME component '${__COMP_LOWER__}'")
				set ("XME_COMPONENT_VIRTUAL_${__COMP_UPPER__}" FALSE)
			endif (${__NUM_SOURCES__} LESS 1)

			add_library(
				${__COMP_LOWER__} STATIC
				${_HEADERS_}
				${_SOURCES_}
				${${__COMP_UPPER__}_BUILDSYSTEM}
			)

			set_target_properties (${__COMP_LOWER__} PROPERTIES "LINKER_LANGUAGE" "C")

			# If the user requested an image file, also dump library headers and disassembly
			# since chances are good that we are building for an embedded platform
			xme_defined (_XME_IMAGE_FORMAT_DEFINED PROPERTY_GLOBAL XME_IMAGE_FORMAT)
			if (${_XME_IMAGE_FORMAT_DEFINED})
				get_target_property(_LIB_FILE ${__COMP_LOWER__} LOCATION)
				add_custom_command(TARGET ${__COMP_LOWER__} POST_BUILD
					COMMAND ${CMAKE_OBJDUMP} --all-headers --source ${_LIB_FILE} > ${_LIB_FILE}.txt
					COMMENT "Objdumping ${_LIB_FILE} to ${_LIB_FILE}.txt (-> headers, disassembly)")
				list (APPEND _CLEAN_UP ${_LIB_FILE}.txt)
			endif (${_XME_IMAGE_FORMAT_DEFINED})

			xme_set (PROPERTY_GLOBAL XME_COMPONENT_BUILT_${__COMP_UPPER__} TRUE)
		endif (${_COMPONENT_BUILT})

		if (${XME_COMPONENT_VIRTUAL_${__COMP_UPPER__}})
			message (STATUS "Linked target '${TARGET}' against virtual XME component '${__COMP_LOWER__}'")
		else (${XME_COMPONENT_VIRTUAL_${__COMP_UPPER__}})
			message (STATUS "Linked target '${TARGET}' against XME component '${__COMP_LOWER__}'")

			# Link the target against all directly specified dependencies
			target_link_libraries (${TARGET} ${__COMP_LOWER__})
		endif (${XME_COMPONENT_VIRTUAL_${__COMP_UPPER__}})

		# Link against third-party libraries
		xme_get (_LINKS_ PROPERTY_GLOBAL "${__COMP_UPPER__}_LINKS")
		set (__IS_BY_VALUE__ TRUE)
		set (__BY_VALUE__)
		foreach (__LNK__ ${_LINKS_})
			if (__IS_BY_VALUE__)
				# ${__LNK__} is YES if the next value is passed "by value" and NO if it is the name of a variable
				set (__BY_VALUE__ ${__LNK__})
				set (__IS_BY_VALUE__ FALSE)
			else (__IS_BY_VALUE__)
				if (__BY_VALUE__)
					# ${__LNK__} is the name of the linker library
					message (STATUS "Linked target '${TARGET}' against third-party library '${__LNK__}'")
					target_link_libraries (${TARGET} ${__LNK__})
				else (__BY_VALUE__)
					# ${__LNK__} is the name of the variable holding the linker library name
					if (NOT ${__LNK__})
						message(WARNING "Lazily evaluated variable '${__LNK__}', which is required for component '${__COMP_LOWER__}', is not defined!")
					else (NOT ${__LNK__})
						message (STATUS "Linked target '${TARGET}' against third-party library '${${__LNK__}}'")
						target_link_libraries (${TARGET} ${${__LNK__}})
					endif (NOT ${__LNK__})
				endif (__BY_VALUE__)
				set (__IS_BY_VALUE__ TRUE)
			endif (__IS_BY_VALUE__)
		endforeach (__LNK__)

		# Recursively add the dependencies of the respective dependency
		xme_get (_DEPENDS_ PROPERTY_GLOBAL "${__COMP_UPPER__}_DEPENDS")
		xme_link_components_rec (${__COMP__} ${_DEPENDS_})
	endforeach (__COMP__)

	# Ensure that additional files are considered during clean-up
	get_directory_property(_OLD_CLEAN_UP ADDITIONAL_MAKE_CLEAN_FILES)
	list (APPEND _OLD_CLEAN_UP ${_CLEAN_UP})
	set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${_OLD_CLEAN_UP}")

endmacro (xme_link_components_rec)



# Macro:      xme_build_documentation ( <project> TARGET <target> [ OUTPUT_DIR <output-dir> ] [ OUTPUT_NAME <output-name> ] [ INSTALL_DIR <install-dir> ] [AUTO] [CLEAN] [ FILES <file1> [ <file2> ... ] ] )
#
# Purpose:    Schedule Doxygen documentation to be built for an amount of source files.
#
# Parameters: <project>                  Human-readable name of the documentation project.
#
#             TARGET <target>            Name of the build target for documentation creation.
#                                        A custom target with that name will be created.
#
#             OUTPUT_DIR <output-dir>    Output directory for generated documentation. In case
#                                        HTML Help Workshop is installed under Windows, the
#                                        resulting *.chm file will be placed in this directory.
#                                        Omit or set to an empty value to place the files in a
#                                        subdirectory of the current build directory.
#
#             OUTPUT_NAME <output-name>  Name (including extension) of the resulting *.chm file
#                                        if HTML Help Workshop is used. Omit or leave empty to
#                                        use the default "index.chm".
#
#             INSTALL_DIR <install-dir>  Installation directory for generated documentation.
#                                        Omit or leave empty to disable installation.
#
#             AUTO                       If specified, the given target will be scheduled to be
#                                        built automatically when the project is built. Otherwise,
#                                        the documentation target build must be invoked manually
#                                        on demand.
#
#             CLEAN                      If specified, the output directory will be completely
#                                        removed before each Doxygen run.
#
#             FILES <file1> [ ... ]      List of additional source files to use as input for
#                                        documentation. All source and header files of used
#                                        or linked XME components will automatically be added
#                                        and do not need to be specified explicitly.
#
macro (xme_build_documentation PROJECT_NAME TARGET_KEYWORD TARGET_NAME) # optional: OUTPUT_DIR_KEYWORD OUTPUT_DIR OUTPUT_NAME_KEYWORD OUTPUT_NAME INSTALL_DIR_KEYWORD INSTALL_DIR FILES_KEYWORD AUTO CLEAN FILES ...

	xme_get (_XME_CURRENT_SOURCE_DIR PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR)
	set (__SYNTAX_OK__ TRUE)

	if ("" STREQUAL PROJECT_NAME)
		message (SEND_ERROR "Error: Empty project name argument!")
		set (__SYNTAX_OK__ FALSE)
	elseif (NOT "xTARGET" STREQUAL "x${TARGET_KEYWORD}") # TARGET is a special keyword to if(), hence needs to be escaped
		message (SEND_ERROR "Error: Missing TARGET keyword!")
		set (__SYNTAX_OK__ FALSE)
	elseif (TARGET_NAME STREQUAL "")
		message (SEND_ERROR "Error: Empty target name argument!")
		set (__SYNTAX_OK__ FALSE)
	endif ("" STREQUAL PROJECT_NAME)

	# Parse remaining options
	set (__OUTPUT_DIR__ "") # OUTPUT_DIR <output-dir>
	set (__OUTPUT_NAME__ "") # OUTPUT_NAME <output-name>
	set (__INSTALL_DIR__ "") # INSTALL_DIR <install-dir>
	set (__AUTO__ FALSE) # AUTO
	set (__CLEAN__ FALSE) # CLEAN
	set (__FILES__) # FILES ...

	set (__MODE__ 0)
	foreach (ARG ${ARGN})
		if ("OUTPUT_DIR" STREQUAL ARG)
			set (__MODE__ 1)
		elseif ("OUTPUT_NAME" STREQUAL ARG)
			set (__MODE__ 2)
		elseif ("INSTALL_DIR" STREQUAL ARG)
			set (__MODE__ 3)
		elseif ("AUTO" STREQUAL ARG)
			set (__AUTO__ TRUE)
			set (__MODE__ 0)
		elseif ("CLEAN" STREQUAL ARG)
			set (__CLEAN__ TRUE)
			set (__MODE__ 0)
		elseif ("FILES" STREQUAL ARG)
			set (__MODE__ 4)
		else ("OUTPUT_NAME" STREQUAL ARG)
			if (__MODE__ EQUAL 0)
				message (SEND_ERROR "Error: Unexpected arguments (probably missing keyword)!")
				set (__SYNTAX_OK__ FALSE)
				break()
			elseif (__MODE__ EQUAL 1)
				set (__OUTPUT_DIR__ ${ARG})
			elseif (__MODE__ EQUAL 2)
				set (__OUTPUT_NAME__ ${ARG})
			elseif (__MODE__ EQUAL 3)
				set (__INSTALL_DIR__ ${ARG})
			elseif (__MODE__ EQUAL 4)
				foreach (__FILE__ ${ARG})
					list (APPEND __FILES__ "${_XME_CURRENT_SOURCE_DIR}/${ARG}")
				endforeach (__FILE__)
			endif (__MODE__ EQUAL 0)
		endif ("OUTPUT_DIR" STREQUAL ARG)
	endforeach (ARG)

	# Check for errors
	if (NOT __SYNTAX_OK__)
		message (FATAL_ERROR "Usage: xme_build_documentation ( <project> TARGET <target> [ OUTPUT_DIR <output-dir> ] [ OUTPUT_NAME <output-name> ] [ INSTALL_DIR <install-dir> ] [AUTO] [CLEAN] [ FILES <file1> [ <file2> ... ] ] )")
	endif (NOT __SYNTAX_OK__)

	xme_get (_XME_DOCUMENTATION_FILES PROPERTY_GLOBAL XME_DOCUMENTATION_FILES)
	list (LENGTH _XME_DOCUMENTATION_FILES __LENGTH_XME__)
	list (LENGTH __FILES__ __LENGTH_ARG__)
	math (EXPR __LENGTH__ "${__LENGTH_XME__} + ${__LENGTH_ARG__}")

	if (__LENGTH__ GREATER 0)
		set (__AUTO_ARG__ "")
		if (__AUTO__)
			set (__AUTO_ARG__ "AUTO")
		endif (__AUTO__)

		set (__CLEAN_ARG__ "")
		if (__CLEAN__)
			set (__CLEAN_ARG__ "CLEAN")
		endif (__CLEAN__)

		xme_simplify_path (XME_ROOT_SIMPLE "${XME_ROOT}")
		doxygen_generate_documentation(
			"${PROJECT_NAME}"
			TARGET "${TARGET_NAME}"
			BASE_DIR "${XME_ROOT_SIMPLE}"
			OUTPUT_DIR "${__OUTPUT_DIR__}"
			OUTPUT_NAME "${__OUTPUT_NAME__}"
			INSTALL_DIR "${__INSTALL_DIR__}"
			${__AUTO_ARG__}
			${__CLEAN_ARG__}
			FILES ${_XME_DOCUMENTATION_FILES} ${__FILES__}
		)

		message (STATUS "Generated target '${TARGET_NAME}' for building '${PROJECT_NAME}' with ${__LENGTH__} input file(s)")
	endif (__LENGTH__ GREATER 0)

	# Generate dependency graph if Graphviz is available
	if (GRAPHVIZ_FOUND)
		add_custom_command(
			TARGET "${TARGET_NAME}"
			POST_BUILD
			COMMAND "${CMAKE_COMMAND}" "--graphviz=${PROJECT_NAME}.dot" ${CMAKE_SOURCE_DIR}
			COMMAND "${GRAPHVIZ_BINARY}" -Tpdf "${PROJECT_NAME}.dot" > "${PROJECT_NAME}.pdf"
			COMMAND "${GRAPHVIZ_BINARY}" -Tpng "${PROJECT_NAME}.dot" > "${PROJECT_NAME}.png"
			WORKING_DIRECTORY .
			COMMENT "Building dependency graph..."
		)
	endif (GRAPHVIZ_FOUND)

endmacro (xme_build_documentation)



# Macro:      xme_generic_port_path ( <output-varname> <port-name> )
#
# Purpose:    Returns a relative path from the current directory (XME_CURRENT_SOURCE_DIR) to
#             the directory of the XME port for architecture/platfrom port-name.
#             This is useful to reference generic implementations (e.g., in C language)
#             from architecture specific ports (e.g., ARMv7-M).
#             As a side effect, the computed directory will be added as an include
#             directory since it might contain header files.
#
# Parameters: <output-varname>           Name of the variable to receive the relative path name.
#
#             <port-name>                Name of the architecture/platform to look up.
#
macro (xme_generic_port_path OUTPUT_PATH PORT)
	if (${ARGC} EQUAL 3)
		set(_NUM_DIRS_TO_REMOVE ${ARGV2})
		if (${_NUM_DIRS_TO_REMOVE} LESS 0)
			set(_NUM_DIRS_TO_REMOVE 1)
		endif (${_NUM_DIRS_TO_REMOVE} LESS 0)
	else (${ARGC} EQUAL 3)
		set(_NUM_DIRS_TO_REMOVE 1)
	endif (${ARGC} EQUAL 3)

	# Calculate current relative path (with respect to port root),
	# i.e. the suffix after ${XME_CURRENT_SOURCE_DIR}/ports/<os|plat>.
	xme_get (_DIR PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR)

	# Remove ${XME_SRC_DIR}/ports/
	string (REGEX REPLACE "^${XME_SRC_DIR}/ports/" "" _DIR ${_DIR})

	# Remove arch (or plat)
	string (FIND ${_DIR} "/" _POS)
	math (EXPR _POS "${_POS} + 1")
	string (SUBSTRING ${_DIR} ${_POS} -1 _DIR)

	# Remove respective architecture / platform directories
	# and compute respective part of the relative path which
	# is contributed by these directories.
	set (_I 0)
	set (_PREFIX_CNT 0)
	while (NOT (${_I} EQUAL ${_NUM_DIRS_TO_REMOVE}))
		string (FIND ${_DIR} "/" _POS)
		math (EXPR _POS "${_POS} + 1")
		string (SUBSTRING ${_DIR} ${_POS} -1 _DIR)
		math (EXPR _I "${_I} + 1")
		math (EXPR _PREFIX_CNT "${_PREFIX_CNT} + 1")
	endwhile (NOT (${_I} EQUAL ${_NUM_DIRS_TO_REMOVE}))
	
	set (_REL_PATH ${_DIR})
	# Finalize relative path from current directory by walking
	# up to ${XME_CURRENT_SOURCE_DIR}/ports/<arch|plat>.
	while (NOT ${_POS} EQUAL -1)
		string (FIND ${_DIR} "/" _POS)
		math (EXPR _PREFIX_CNT "${_PREFIX_CNT} + 1")
		string (SUBSTRING ${_DIR} 0 ${_POS} _TMP)
		if (NOT ${_POS} EQUAL -1)
			math (EXPR _POS "${_POS} + 1")
			string (SUBSTRING ${_DIR} ${_POS} -1 _DIR)
		endif (NOT ${_POS} EQUAL -1)	
	endwhile (NOT ${_POS} EQUAL -1)

	xme_get (_DIR PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR)
	while (${_PREFIX_CNT} GREATER 0)
		string (FIND ${_DIR} "/" _POS REVERSE)
		string (SUBSTRING "${_DIR}" 0 ${_POS} _DIR)
		math (EXPR _PREFIX_CNT "${_PREFIX_CNT} -1")
	endwhile (${_PREFIX_CNT} GREATER 0)

	# Add generic port directory to list of include directories
	xme_include_directory("${_DIR}/${PORT}")

	# Finalize path by appending desired port and subdirectory within
	# current port
	set (${OUTPUT_PATH} "${_DIR}/${PORT}/${_REL_PATH}")
endmacro (xme_generic_port_path)

# Global property XME_BUILD_HOST
xme_defined (XME_BUILD_HOST_DEFINED PROPERTY_GLOBAL XME_BUILD_HOST)
if (NOT ${XME_BUILD_HOST_DEFINED})
	message (STATUS "XME_BUILD_HOST not defined. Assuming: 'windows'.")
	xme_set (PROPERTY_GLOBAL XME_BUILD_HOST "windows")
else (NOT ${XME_BUILD_HOST_DEFINED})
	xme_get (_XME_BUILD_HOST PROPERTY_GLOBAL XME_BUILD_HOST)
	message (STATUS "XME_BUILD_HOST: '${_XME_BUILD_HOST}'.")
endif (NOT ${XME_BUILD_HOST_DEFINED})

# Global property XME_PLATFORM (= software "stack", i.e. OS, middleware, ...)
xme_defined (XME_PLATFORM_DEFINED PROPERTY_GLOBAL XME_PLATFORM)
if (NOT ${XME_PLATFORM_DEFINED})
	set (_XME_PLATFORM "posix")
	xme_set (PROPERTY_GLOBAL XME_PLATFORM "${_XME_PLATFORM}")
	message (WARNING "XME_PLATFORM not defined, assuming platform '${_XME_PLATFORM}'")
else (NOT ${XME_PLATFORM_DEFINED})
	xme_get(_XME_PLATFORM PROPERTY_GLOBAL XME_PLATFORM)

	# Sanity check
	set (_XME_AVAILABLE_PLATFORMS "baremetal freertos posix windows")
	list (FIND _XME_AVAILABLE_PLATFORMS ${_XME_PLATFORM}  _XME_PLATFORM_FOUND_)
	if (${_XME_PLATFORM_FOUND_})
		message (STATUS "Build target platform: ${_XME_PLATFORM}")
	else (${_XME_PLATFORM_FOUND_})
		message (FATAL_ERROR "Unkown build target platform: ${_XME_PLATFORM}. Currently supported: ${_XME_AVAILABLE_PLATFORMS}.")
	endif (${_XME_PLATFORM_FOUND_})
endif (NOT ${XME_PLATFORM_DEFINED})

# Global property XME_ARCHITECTURE (= processor architecture)
xme_defined (XME_ARCHITECTURE_DEFINED PROPERTY_GLOBAL XME_ARCHITECTURE)
if (NOT ${XME_ARCHITECTURE_DEFINED})
	set (_XME_ARCHITECTURE "x86")
	xme_set (PROPERTY_GLOBAL XME_ARCHITECTURE "${_XME_ARCHITECTURE}")
	message (WARNING "XME_ARCHITECTURE not defined, assuming architecture '${_XME_ARCHITECTURE}'")
else (NOT ${XME_ARCHITECTURE_DEFINED})
	xme_get(_XME_ARCHITECTURE PROPERTY_GLOBAL XME_ARCHITECTURE)

	# Sanity check
	set (_XME_AVAILABLE_ARCHITECTURES "amd64 x86 ARMv7-M NiosII")
	list (FIND _XME_AVAILABLE_ARCHITECTURES ${_XME_ARCHITECTURE}  _XME_ARCHITECTURE_FOUND)
	if (${_XME_ARCHITECTURE_FOUND})
		message (STATUS "Build target architecture: ${_XME_ARCHITECTURE}")
	else (${_XME_ARCHITECTURE_FOUND})
		message (FATAL_ERROR "Unkown build target architecture: ${_XME_ARCHITECTURE}. Currently supported: ${_XME_AVAILABLE_ARCHITECTURES}.")
	endif (${_XME_ARCHITECTURE_FOUND})
endif (NOT ${XME_ARCHITECTURE_DEFINED})

# Check whether we have a platform/architecture combination
# which is currently supported

if (NOT ((${_XME_PLATFORM} STREQUAL "windows" AND ${_XME_ARCHITECTURE} STREQUAL "x86") OR
	(${_XME_PLATFORM} STREQUAL "posix" AND ${_XME_ARCHITECTURE} STREQUAL "x86") OR
	(${_XME_PLATFORM} STREQUAL "posix" AND ${_XME_ARCHITECTURE} STREQUAL "amd64") OR
	(${_XME_PLATFORM} STREQUAL "freertos" AND ${_XME_ARCHITECTURE} STREQUAL "ARMv7-M") OR
	(${_XME_PLATFORM} STREQUAL "baremetal" AND ${_XME_ARCHITECTURE} STREQUAL "ARMv7-M") OR
	(${_XME_PLATFORM} STREQUAL "freertos" AND ${_XME_ARCHITECTURE} STREQUAL "NiosII")
	)
)
	message (FATAL_ERROR "Unsupported combination of platform/architecture: ${_XME_PLATFORM}/${_XME_ARCHITECTURE}.\nCurrently supported: windows/x86, posix/x86, posix/amd64, baremetal/ARMv7-M., freertos/ARMv7-M, freertos/NiosII.\n")
else (NOT ((${_XME_PLATFORM} STREQUAL "windows" AND ${_XME_ARCHITECTURE} STREQUAL "x86") OR
	(${_XME_PLATFORM} STREQUAL "posix" AND ${_XME_ARCHITECTURE} STREQUAL "x86") OR
	(${_XME_PLATFORM} STREQUAL "posix" AND ${_XME_ARCHITECTURE} STREQUAL "amd64") OR
	(${_XME_PLATFORM} STREQUAL "freertos" AND ${_XME_ARCHITECTURE} STREQUAL "ARMv7-M") OR
	(${_XME_PLATFORM} STREQUAL "baremetal" AND ${_XME_ARCHITECTURE} STREQUAL "ARMv7-M") OR
	(${_XME_PLATFORM} STREQUAL "freertos" AND ${_XME_ARCHITECTURE} STREQUAL "NiosII")
	)
)
	message (STATUS "Platform/architecture combination ${_XME_PLATFORM}/${_XME_ARCHITECTURE} is supported!")

	xme_get(_XME_TARGET PROPERTY_GLOBAL XME_TARGET)
	# TODO: Ugly
	if (NOT DEFINED _XME_TARGET)
		set(_XME_TARGET "_")
	endif (NOT DEFINED _XME_TARGET)

	if (${_XME_ARCHITECTURE} STREQUAL "ARMv7-M")
		# Target device
		if (${_XME_TARGET} STREQUAL "STM32F107VC")
			message (STATUS "Supported target device: ${_XME_TARGET}.")
		endif (${_XME_TARGET} STREQUAL "STM32F107VC")

		# Target device
		if (${_XME_TARGET} STREQUAL "LM3S8962")
			message (STATUS "Supported target device: ${_XME_TARGET}.")
		endif (${_XME_TARGET} STREQUAL "LM3S8962")

		# Board support package
		xme_defined (_XME_BOARD_DEFINED PROPERTY_GLOBAL XME_BOARD)
		if (NOT ${_XME_BOARD_DEFINED})
			message (FATAL_ERROR "XME_BOARD not defined. Possible values for target architecture ${_XME_ARCHITECTURE}: 'EK-LM3S8962', 'MCBSTM32C', 'STM32-P107', 'LMU_BELL-II_V1'.")
		endif (NOT ${_XME_BOARD_DEFINED})

		xme_get(_XME_BOARD PROPERTY_GLOBAL XME_BOARD)
		if (NOT ((_XME_BOARD STREQUAL "MCBSTM32C") OR (_XME_BOARD STREQUAL "EK-LM3S8962") OR (_XME_BOARD STREQUAL "STM32-P107") OR (_XME_BOARD STREQUAL "LMU_BELL-II_V1")))
			message (FATAL_ERROR "XME_BOARD '${_XME_BOARD}' is not supported for target architecture ${_XME_ARCHITECTURE}. Possible values are: 'EK-LM3S8962', 'MCBSTM32C', 'STM32-P107', LMU_BELL-II_V1.")
		endif (NOT ((_XME_BOARD STREQUAL "MCBSTM32C") OR (_XME_BOARD STREQUAL "EK-LM3S8962") OR (_XME_BOARD STREQUAL "STM32-P107") OR (_XME_BOARD STREQUAL "LMU_BELL-II_V1")))

		# Image format
		xme_defined (_XME_IMAGE_FORMAT_DEFINED PROPERTY_GLOBAL XME_IMAGE_FORMAT)
		if (NOT ${_XME_IMAGE_FORMAT_DEFINED})
			xme_set (PROPERTY_GLOBAL XME_IMAGE_FORMAT "binary")
			message (STATUS "XME_IMAGE_FORMAT not defined. Defaulting to 'binary' for target architecture ${_XME_ARCHITECTURE}")
		endif (NOT ${_XME_IMAGE_FORMAT_DEFINED})

		# Linker flags
		xme_set (PROPERTY_GLOBAL XME_LINKER_FLAGS_MAP_FILE "-Wl,-Map,")

		# Addresses and sizes
		xme_defined (_XME_TARGET_FLASH_ADDRESS_DEFINED PROPERTY_GLOBAL XME_TARGET_FLASH_ADDRESS)
		if (NOT ${_XME_TARGET_FLASH_ADDRESS_DEFINED})
			message (STATUS "XME_TARGET_FLASH_ADDRESS not defined. Defaulting to '0x08000000'.")
			xme_set (PROPERTY_GLOBAL XME_TARGET_FLASH_ADDRESS "0x08000000")
		endif (NOT ${_XME_TARGET_FLASH_ADDRESS_DEFINED})

		xme_defined (_XME_TARGET_FLASH_SIZE_DEFINED PROPERTY_GLOBAL XME_TARGET_FLASH_SIZE)
		if (NOT ${_XME_TARGET_FLASH_SIZE_DEFINED})
			message (STATUS "XME_TARGET_FLASH_SIZE not defined. Defaulting to 256kB.")
			xme_set (PROPERTY_GLOBAL XME_TARGET_FLASH_SIZE "0x40000")
		endif (NOT ${_XME_TARGET_FLASH_SIZE_DEFINED})

		xme_defined (_XME_TARGET_BOOTLOADER_SIZE_DEFINED PROPERTY_GLOBAL XME_TARGET_BOOTLOADER_SIZE)
		if (NOT ${_XME_TARGET_BOOTLOADER_SIZE_DEFINED})
			message (STATUS "XME_TARGET_BOOTLOADER_SIZE not defined. Defaulting to 64kB.")
		#	xme_set (PROPERTY_GLOBAL XME_TARGET_BOOTLOADER_SIZE "0x20000")
			xme_set (PROPERTY_GLOBAL XME_TARGET_BOOTLOADER_SIZE "0x10000")
		endif (NOT ${_XME_TARGET_BOOTLOADER_SIZE_DEFINED})

		# Defaults for bootloader handling
		xme_defined (_XME_BUILDING_BOOTLOADER_DEFINED PROPERTY_GLOBAL XME_BUILDING_BOOTLOADER)
		if (NOT _XME_BUILDING_BOOTLOADER_DEFINED)
			message (STATUS "XME_BUILDING_BOOTLOADER not defined. Defaulting to FALSE.")
			xme_set(PROPERTY_GLOBAL XME_BUILDING_BOOTLOADER FALSE)
		endif (NOT _XME_BUILDING_BOOTLOADER_DEFINED)

		xme_defined (_XME_BUILDING_APPLICATION_FOR_BOOTLOADER_DEFINED PROPERTY_GLOBAL XME_BUILDING_APPLICATION_FOR_BOOTLOADER_DEFINED)
		if (NOT _XME_BUILDING_APPLICATION_FOR_BOOTLOADER_DEFINED)
			message (STATUS "XME_BUILDING_APPLICATION_FOR_BOOTLOADER_DEFINED not defined. Defaulting to FALSE.")
			xme_set(PROPERTY_GLOBAL XME_BUILDING_APPLICATION_FOR_BOOTLOADER_DEFINED FALSE)
		endif (NOT _XME_BUILDING_APPLICATION_FOR_BOOTLOADER_DEFINED)

		# Start addresses and sizes when building bootloader / application with(without) bootloader support
		xme_get(_XME_TARGET_FLASH_ADDRESS PROPERTY_GLOBAL XME_TARGET_FLASH_ADDRESS)
		xme_get(_XME_TARGET_FLASH_SIZE PROPERTY_GLOBAL XME_TARGET_FLASH_SIZE)
		xme_get(_XME_TARGET_BOOTLOADER_SIZE PROPERTY_GLOBAL XME_TARGET_BOOTLOADER_SIZE)
		xme_get(_XME_BUILDING_BOOTLOADER PROPERTY_GLOBAL XME_BUILDING_BOOTLOADER)
		xme_get(_XME_BUILDING_APPLICATION_FOR_BOOTLOADER PROPERTY_GLOBAL XME_BUILDING_APPLICATION_FOR_BOOTLOADER)

		if (_XME_BUILDING_BOOTLOADER AND _XME_BUILDING_APPLICATION_FOR_BOOTLOADER)
			message (FATAL_ERROR "XME_BUILDING_BOOTLOADER and XME_BUILDING_APPLICATION_FOR_BOOTLOADER may not both evaluate to TRUE.")
		endif (_XME_BUILDING_BOOTLOADER AND _XME_BUILDING_APPLICATION_FOR_BOOTLOADER)

		if (_XME_BUILDING_BOOTLOADER)
			# Start address
			xme_set(PROPERTY_GLOBAL XME_IMAGE_FLASH_ADDRESS ${_XME_TARGET_FLASH_ADDRESS})

			# Maximum application size
			hex2dec (${_XME_TARGET_BOOTLOADER_SIZE} _XME_TARGET_BOOTLOADER_SIZE_DEC)
			math(EXPR _SIZE "${_XME_TARGET_BOOTLOADER_SIZE_DEC} / 1024")
			xme_set(PROPERTY_GLOBAL XME_IMAGE_MAX_SIZE "${_SIZE}K")

			message (STATUS "Building bootloader. Image start address: ${_XME_TARGET_FLASH_ADDRESS}. Maximum size: ${_SIZE}kB.")
		else (_XME_BUILDING_BOOTLOADER)
			if (_XME_BUILDING_APPLICATION_FOR_BOOTLOADER)
				# Start address
				hex2dec (${_XME_TARGET_FLASH_ADDRESS} _XME_TARGET_FLASH_ADDRESS_DEC)
				hex2dec (${_XME_TARGET_FLASH_SIZE} _XME_TARGET_FLASH_SIZE_DEC)
				hex2dec (${_XME_TARGET_BOOTLOADER_SIZE} _XME_TARGET_BOOTLOADER_SIZE_DEC)

				math(EXPR _XME_IMAGE_FLASH_ADDRESS_DEC "${_XME_TARGET_FLASH_ADDRESS_DEC} + ${_XME_TARGET_BOOTLOADER_SIZE_DEC}")
				dec2hex(${_XME_IMAGE_FLASH_ADDRESS_DEC} _XME_IMAGE_FLASH_ADDRESS)
				xme_set(PROPERTY_GLOBAL XME_IMAGE_FLASH_ADDRESS "${_XME_IMAGE_FLASH_ADDRESS}")

				# Maximum application size
				math(EXPR _SIZE "(${_XME_TARGET_FLASH_SIZE_DEC} - ${_XME_TARGET_BOOTLOADER_SIZE_DEC}) / 1024")
				xme_set(PROPERTY_GLOBAL XME_IMAGE_MAX_SIZE "${_SIZE}K")

				message (STATUS "Building application for bootloader. Image start address: ${_XME_IMAGE_FLASH_ADDRESS}. Maximum size: ${_SIZE}kB.")
			else (_XME_BUILDING_APPLICATION_FOR_BOOTLOADER)
				# Start address
				xme_set(PROPERTY_GLOBAL XME_IMAGE_FLASH_ADDRESS ${_XME_TARGET_FLASH_ADDRESS})

				# Maximum application size
				hex2dec (${_XME_TARGET_FLASH_SIZE} _XME_TARGET_FLASH_SIZE_DEC)
				math(EXPR _SIZE "${_XME_TARGET_FLASH_SIZE_DEC} / 1024")

				xme_set(PROPERTY_GLOBAL XME_IMAGE_MAX_SIZE ${_SIZE}K)

				message (STATUS "Building standalone application. Image start address: ${_XME_TARGET_FLASH_ADDRESS}. Maximum size: ${_SIZE}kB.")
			endif (_XME_BUILDING_APPLICATION_FOR_BOOTLOADER)
		endif (_XME_BUILDING_BOOTLOADER)
	elseif (${_XME_ARCHITECTURE} STREQUAL "NiosII")
		# Target device
		if (${_XME_TARGET} STREQUAL "EP3C120F780C7N")
			message (STATUS "Supported target device: ${_XME_TARGET}.")
		endif (${_XME_TARGET} STREQUAL "EP3C120F780C7N")
		
		# Linker flags
		xme_set (PROPERTY_GLOBAL XME_LINKER_FLAGS_MAP_FILE "-Wl,-Map,")
	
		else (${_XME_ARCHITECTURE} STREQUAL "NiosII")
		if (NOT ${__XME_TARGET} STREQUAL "_")
			message ("Ignoring unsupported target device: ${_XME_TARGET}.")
		endif (NOT ${__XME_TARGET} STREQUAL "_")
		xme_set (PROPERTY_GLOBAL XME_TARGET "")
	endif (${_XME_ARCHITECTURE} STREQUAL "ARMv7-M")
endif (NOT ((${_XME_PLATFORM} STREQUAL "windows" AND ${_XME_ARCHITECTURE} STREQUAL "x86") OR
(${_XME_PLATFORM} STREQUAL "posix" AND ${_XME_ARCHITECTURE} STREQUAL "x86") OR
(${_XME_PLATFORM} STREQUAL "posix" AND ${_XME_ARCHITECTURE} STREQUAL "amd64") OR
(${_XME_PLATFORM} STREQUAL "freertos" AND ${_XME_ARCHITECTURE} STREQUAL "ARMv7-M") OR
(${_XME_PLATFORM} STREQUAL "baremetal" AND ${_XME_ARCHITECTURE} STREQUAL "ARMv7-M") OR
(${_XME_PLATFORM} STREQUAL "freertos" AND ${_XME_ARCHITECTURE} STREQUAL "NiosII")
)
)

# Check whether we have a valid image file specification
xme_defined (_XME_IMAGE_FORMAT_DEFINED PROPERTY_GLOBAL XME_IMAGE_FORMAT)
if (NOT ${_XME_IMAGE_FORMAT_DEFINED})
	message (STATUS "Image format not specified: will not run objcopy to create target image.")
else (NOT ${_XME_IMAGE_FORMAT_DEFINED})
	# Support more output formats as needed
	xme_get (_XME_IMAGE_FORMAT PROPERTY_GLOBAL XME_IMAGE_FORMAT)
	if (_XME_IMAGE_FORMAT STREQUAL "ihex")
		xme_set (PROPERTY_GLOBAL XME_IMAGE_EXTENSION "hex")
	elseif (_XME_IMAGE_FORMAT STREQUAL "srec")
		xme_set (PROPERTY_GLOBAL XME_IMAGE_EXTENSION "srec")
	elseif (_XME_IMAGE_FORMAT STREQUAL "binary")
		xme_set (PROPERTY_GLOBAL XME_IMAGE_EXTENSION "bin")
	else (_XME_IMAGE_FORMAT STREQUAL "binary")
		message (STATUS "Unkown image format: '${_XME_IMAGE_FORMAT}'")
	endif (_XME_IMAGE_FORMAT STREQUAL "ihex")
endif (NOT ${_XME_IMAGE_FORMAT_DEFINED})

xme_defined (_XME_DEBUGGER_TARGET_DEFINED PROPERTY_GLOBAL XME_DEBUGGER_TARGET)
if (NOT ${_XME_DEBUGGER_TARGET_DEFINED})
	message (STATUS "XME_DEBUGGER_TARGET not defined. Defaulting to 'remote localhost:3333'.")
	xme_set (PROPERTY_GLOBAL XME_DEBUGGER_TARGET "remote localhost:3333")
endif (NOT ${_XME_DEBUGGER_TARGET_DEFINED})

# Generic build settings
xme_include_directory(
	${CMAKE_CURRENT_SOURCE_DIR}
	)

xme_lib_directory(
	${CMAKE_CURRENT_SOURCE_DIR}
	)

# Platform/architecture dependant include and link directories
xme_get (_XME_PLATFORM PROPERTY_GLOBAL XME_PLATFORM)
xme_get (_XME_ARCHITECTURE PROPERTY_GLOBAL XME_ARCHITECTURE)

# Reset some flags since we are accumulating
# serveral settings here but we want to avoid
# duplicates when CMake is run several times.
set (CMAKE_C_FLAGS "" CACHE STRING "" FORCE)
set (CMAKE_CXX_FLAGS "" CACHE STRING "" FORCE)
set (CMAKE_ASM_FLAGS "" CACHE STRING "" FORCE)
set (CMAKE_EXE_LINKER_FLAGS "" CACHE STRING "" FORCE)

# Platform/architecture dependant toolchain flags
if (${_XME_PLATFORM} STREQUAL "posix")
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC" CACHE STRING "" FORCE)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC" CACHE STRING "" FORCE)
	if (${_XME_ARCHITECTURE} STREQUAL "x86")
		set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32" CACHE STRING "" FORCE)
	endif (${_XME_ARCHITECTURE} STREQUAL "x86")
elseif (${_XME_ARCHITECTURE} STREQUAL "ARMv7-M")
	if (DEFINED CMAKE_TOOLCHAIN_FILE)
		string (REGEX MATCH "ToolchainSourceryCodebench.cmake$" _KOWN_TOOLCHAIN ${CMAKE_TOOLCHAIN_FILE})
	endif (DEFINED CMAKE_TOOLCHAIN_FILE)
	if (_KOWN_TOOLCHAIN STREQUAL "")
		message (FATAL_ERROR "You must use a supported toolchain file for this target.\nCurrently supported: ToolchainSourceryCodebench.cmake.")
	else (_KOWN_TOOLCHAIN STREQUAL "")
		message (STATUS "Using toolchain file ${CMAKE_TOOLCHAIN_FILE}.")

		# Enabling assembler support from the Toolchain specification does not
		# work (leads to a hang of CMakeGUI).
		enable_language(ASM)

		# Target processor
		set (PROC_FLAGS "-mcpu=cortex-m3 -mthumb")
		set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${PROC_FLAGS}" CACHE STRING "" FORCE)
		set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PROC_FLAGS}" CACHE STRING "" FORCE)
		set (CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${PROC_FLAGS} -x assembler-with-cpp" CACHE STRING "" FORCE)

		# Linker script
		xme_get (_XME_TARGET PROPERTY_GLOBAL XME_TARGET)
		if (${_XME_TARGET} STREQUAL "STM32F107VC")
			xme_get(_XME_IMAGE_FLASH_ADDRESS PROPERTY_GLOBAL XME_IMAGE_FLASH_ADDRESS)
			xme_get(_XME_IMAGE_MAX_SIZE PROPERTY_GLOBAL XME_IMAGE_MAX_SIZE)
			set (LINKER_SCRIPT_BASE "stm32f107vc.ld")
			set (LINKER_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/${LINKER_SCRIPT_BASE}")
			configure_file (${XME_TOOLS_DIR}/toolchain/ARMv7-M/${LINKER_SCRIPT_BASE}.in ${LINKER_SCRIPT} @ONLY)
			set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostartfiles -T${LINKER_SCRIPT}" CACHE STRING "" FORCE)
		endif (${_XME_TARGET} STREQUAL "STM32F107VC")

		if (${_XME_TARGET} STREQUAL "LM3S8962")
			xme_get(_XME_IMAGE_FLASH_ADDRESS PROPERTY_GLOBAL XME_IMAGE_FLASH_ADDRESS)
			xme_get(_XME_IMAGE_MAX_SIZE PROPERTY_GLOBAL XME_IMAGE_MAX_SIZE)
			set (LINKER_SCRIPT_BASE "lm3s8962.ld")
			set (LINKER_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/${LINKER_SCRIPT_BASE}")
			configure_file (${XME_TOOLS_DIR}/toolchain/ARMv7-M/${LINKER_SCRIPT_BASE}.in ${LINKER_SCRIPT} @ONLY)
			set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostartfiles -T${LINKER_SCRIPT}" CACHE STRING "" FORCE)
		endif (${_XME_TARGET} STREQUAL "LM3S8962")

		xme_set (PROPERTY_GLOBAL XME_DEBUGGER_CFG ${XME_TOOLS_DIR}/toolchain/ARMv7-M/gdb-arm-jtag-usb-stm32)

		find_path (
			ECLIPSE_WORKSPACE_DIR
			.metadata
			${XME_ROOT}/../../../../devenv/workspace
			${XME_ROOT}/../../../devenv/workspace
			${XME_ROOT}/../../devenv/workspace
			$ENV{BELLE_TOOLCHAIN}/../workspace
			)
		if (NOT ECLIPSE_WORKSPACE_DIR STREQUAL "ECLIPSE_WORKSPACE_DIR-NOTFOUND")
			set (ECLIPSE_LAUNCHES_DIR "${ECLIPSE_WORKSPACE_DIR}/.metadata/.plugins/org.eclipse.debug.core/.launches")
			xme_set (PROPERTY_GLOBAL XME_ECLIPSE_LAUNCHES_DIR ${ECLIPSE_LAUNCHES_DIR})
			message (STATUS "Eclipse workspace found. You will have to restart Eclipse in order to apply changes.")
		else (NOT ECLIPSE_WORKSPACE_DIR STREQUAL "ECLIPSE_WORKSPACE_DIR-NOTFOUND")
			message ("Eclipse workspace not found (looking for .metadata). Will not generate launch configurations.")
		endif (NOT ECLIPSE_WORKSPACE_DIR STREQUAL "ECLIPSE_WORKSPACE_DIR-NOTFOUND")

		find_package (OpenOCD)
		if (NOT OPENOCD STREQUAL "OPENOCD-NOTFOUND")
			set (OPENOCD_WRAPPER ${XME_TOOLS_DIR}/toolchain/ARMv7-M/openocd.bat)
			xme_set (PROPERTY_GLOBAL XME_OPENOCD_WRAPPER ${OPENOCD_WRAPPER})
			# Create wrapper batch script for OpenOCD that kills all OpenOCD and arm-none-eabi-gdb processes
			# before starting OpenOCD
			configure_file (${OPENOCD_WRAPPER}.in ${OPENOCD_WRAPPER} @ONLY)

			# Create Eclipse launch-configurations to start and kill OpenOCD

			set (OPENOCD_CFG_PATH "${XME_TOOLS_DIR}/toolchain/ARMv7-M")

			# We assume here that all MCUs of one type can be
			# debugged with the same OpenOCD configuration regardless of the
			# board where they are built into.
			#
			# If this is not sufficient, an additional distinction depending on
			# _XME_BOARD needs to be performed.
			if (${_XME_TARGET} STREQUAL "STM32F107VC")
				set (OPENOCD_CFG_FILE "openocd-arm-jtag-ew-p107")
			endif (${_XME_TARGET} STREQUAL "STM32F107VC")

			if (${_XME_TARGET} STREQUAL "LM3S8962")
				set (OPENOCD_CFG_FILE "openocd-luminary-lm3s8962.cfg")
			endif (${_XME_TARGET} STREQUAL "LM3S8962")

			# Create one launch configuration for each of the different OpenOCD configurations.
			# Currently, there is one configuration per MCU type (see above).
			if (NOT ECLIPSE_WORKSPACE_DIR STREQUAL "ECLIPSE_WORKSPACE_DIR-NOTFOUND")
				configure_file ("${XME_TOOLS_DIR}/toolchain/eclipse/OpenOCD.launch.in" "${ECLIPSE_LAUNCHES_DIR}/OpenOCD-${_XME_TARGET}.launch" @ONLY)
				set (TASKKILL "$ENV{WINDIR}/system32/taskkill.exe")
				configure_file ("${XME_TOOLS_DIR}/toolchain/eclipse/OpenOCD_Kill.launch.in" "${ECLIPSE_LAUNCHES_DIR}/OpenOCD_Kill.launch" @ONLY)
			endif (NOT ECLIPSE_WORKSPACE_DIR STREQUAL "ECLIPSE_WORKSPACE_DIR-NOTFOUND")
		endif (NOT OPENOCD STREQUAL "OPENOCD-NOTFOUND")

		# Make targets to reset/start or reset/halt the target devices (requires an OpenOCD connection)
		# targets that allow to upload the image, as well as to reset the target
		if (NOT CMAKE_DEBUGGER STREQUAL "CMAKE_DEBUGGER-NOTFOUND")
			# Create wrapper batch script for arm-none-eabi-gdb that kills existing all OpenOCD and arm-none-eabi-gdb processes
			# before starting OpenOCD
			set (DEBUGGER_WRAPPER ${XME_TOOLS_DIR}/toolchain/ARMv7-M/arm-none-eabi-gdb.bat)
			xme_set (PROPERTY_GLOBAL XME_DEBUGGER_WRAPPER ${DEBUGGER_WRAPPER})
			configure_file (${DEBUGGER_WRAPPER}.in ${DEBUGGER_WRAPPER} @ONLY)

			# Create Eclipse launch-configurations to reset run/halt the target
			if (NOT ECLIPSE_WORKSPACE_DIR STREQUAL "ECLIPSE_WORKSPACE_DIR-NOTFOUND")
				configure_file ("${XME_TOOLS_DIR}/toolchain/eclipse/Reset halt.launch.in" "${ECLIPSE_LAUNCHES_DIR}/Reset halt.launch" @ONLY)
				configure_file ("${XME_TOOLS_DIR}/toolchain/eclipse/Reset run.launch.in" "${ECLIPSE_LAUNCHES_DIR}/Reset run.launch" @ONLY)
			endif (NOT ECLIPSE_WORKSPACE_DIR STREQUAL "ECLIPSE_WORKSPACE_DIR-NOTFOUND")

			xme_get(_XME_DEBUGGER_TARGET PROPERTY_GLOBAL XME_DEBUGGER_TARGET)
			add_custom_target (reset_halt ${DEBUGGER_WRAPPER} -ex \"target ${_XME_DEBUGGER_TARGET}\" -ex \"monitor reset init\" -ex \"monitor halt\" -ex \"disconnect\" -ex \"quit\")
			add_custom_target (reset_run ${DEBUGGER_WRAPPER} -ex \"target ${_XME_DEBUGGER_TARGET}\" -ex \"monitor reset run\" -ex \"resume\" -ex \"disconnect\" -ex \"quit\")
		endif (NOT CMAKE_DEBUGGER STREQUAL "CMAKE_DEBUGGER-NOTFOUND")

	endif (_KOWN_TOOLCHAIN STREQUAL "")
elseif (${_XME_ARCHITECTURE} STREQUAL "NiosII")
	###################################################################
	###################################################################
    enable_language(ASM)

	# need to merge these two bsp dirs ??
    xme_get (_XME_ARCHITECTURE PROPERTY_GLOBAL XME_ARCHITECTURE)
    xme_get (_XME_BOARD PROPERTY_GLOBAL XME_BOARD)
	# FixME: need to change back to trunk bsp dir. FreeRTOS CMakeList.txt has the same problem
    set (__XME_BSP_DIR "${XME_BSP_DIR}/${_XME_ARCHITECTURE}/${_XME_BOARD}/gen_bsp")
	#set  (__XME_BSP_DIR "/cygdrive/c/fortiss/FPGA/hardware/ddr_economic/ddr/test_nios/software/small_mem_bsp")

    # linker script: -T'../bsp//linker.x'
    # The linker.x is generated per project
#	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T C:/fortiss/FPGA/hardware/nios_simple/software/bsp/linker.x" CACHE STRING "" FORCE)
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${__XME_BSP_DIR}/linker.x" CACHE STRING "" FORCE)

	# bootloader
#	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -msys-crt0=C:/fortiss/FPGA/hardware/nios_simple/software/bsp/obj/HAL/src/crt0.o" CACHE STRING "" FORCE)
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -msys-crt0=${__XME_BSP_DIR}/obj/HAL/src/crt0.o" CACHE STRING "" FORCE)

	# bsp: libhal_bsp.a
#	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -msys-lib=hal_bsp -LC:/fortiss/FPGA/hardware/nios_simple/software/bsp" CACHE STRING "" FORCE)
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -msys-lib=hal_bsp -L${__XME_BSP_DIR}" CACHE STRING "" FORCE)

	# hardware div/mul/mulx
	set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -EL -mno-hw-div -mno-hw-mul -mno-hw-mulx -lm" CACHE STRING "" FORCE)
		
	# FixME: maybe the bus width should not be here !!!! (yes)
	# set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -DSYSTEM_BUS_WIDTH=32" CACHE STRING "" FORCE)

#	message("${CMAKE_EXE_LINKER_FLAGS}")
elseif (${_XME_PLATFORM} STREQUAL "windows")
	if (MSVC)
		# Enable "Edit and Continue" compatible program database
		add_definitions(/ZI)
	endif (MSVC)
else  (${_XME_PLATFORM} STREQUAL "posix")
	message (STATUS "Not adding any platform/architecture dependant toolchain flags.")
endif (${_XME_PLATFORM} STREQUAL "posix")

# GNU Compiler Collection settings
# Adopted from GNU ARM Eclipse Plugin: <http://sourceforge.net/projects/gnuarmeclipse/>
if(CMAKE_COMPILER_IS_GNUCC)
	# Improve CDT error parser performance
	if (CMAKE_EXTRA_GENERATOR STREQUAL "Eclipse CDT4")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0" CACHE STRING "" FORCE)
	endif (CMAKE_EXTRA_GENERATOR STREQUAL "Eclipse CDT4")

	if (NOT ${_XME_ARCHITECTURE} STREQUAL "NiosII")
		set (CMAKE_C_FLAGS_DEBUG "-g3 -gdwarf-2" CACHE STRING "" FORCE)
		set (CMAKE_C_FLAGS_RELWITHDEBINFO "-O2 -g3 -gdwarf-2"  CACHE STRING "" FORCE)
	else (${_XME_ARCHITECTURE} STREQUAL "NiosII")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -xc -O0 -DSYSTEM_BUS_WIDTH=32 -MP -MMD -pipe -D__hal__ -DALT_NO_INSTRUCTION_EMULATION -DALT_SINGLE_THREADED -EL -mno-hw-div -mno-hw-mul -mno-hw-mulx" CACHE STRING "" FORCE)
	endif (NOT ${_XME_ARCHITECTURE} STREQUAL "NiosII")
	
	# If we are using the GNU C compiler, assume that we are also using
	# the GNU assembler.
	if (DEFINED CMAKE_ASM_COMPILER)
		if (CMAKE_EXTRA_GENERATOR STREQUAL "Eclipse CDT4")
			set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -fmessage-length=0" CACHE STRING "" FORCE)
		endif (CMAKE_EXTRA_GENERATOR STREQUAL "Eclipse CDT4")

		if (NOT ${_XME_ARCHITECTURE} STREQUAL "NiosII")
			set (CMAKE_ASM_FLAGS_DEBUG "-g3 -gdwarf-2" CACHE STRING "" FORCE)
			set (CMAKE_ASM_FLAGS_RELWITHDEBINFO "-O2 -g3 -gdwarf-2"  CACHE STRING "" FORCE)
		else (${_XME_ARCHITECTURE} STREQUAL "NiosII")
			set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} -O0 -DSYSTEM_BUS_WIDTH=32 -MP -MMD -pipe -D__hal__ -DALT_NO_INSTRUCTION_EMULATION -DALT_SINGLE_THREADED -EL -mno-hw-div -mno-hw-mul -mno-hw-mulx" CACHE STRING "" FORCE)
		endif (NOT ${_XME_ARCHITECTURE} STREQUAL "NiosII")
		# message(${CMAKE_ASM_FLAGS})
	endif (DEFINED CMAKE_ASM_COMPILER)
endif(CMAKE_COMPILER_IS_GNUCC)

if(CMAKE_COMPILER_IS_GNUCXX)
	# Improve CDT error parser performance
	if (CMAKE_EXTRA_GENERATOR STREQUAL "Eclipse CDT4")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0" CACHE STRING "" FORCE)
	endif (CMAKE_EXTRA_GENERATOR STREQUAL "Eclipse CDT4")

	if (NOT ${_XME_ARCHITECTURE} STREQUAL "NiosII")
		set (CMAKE_CXX_FLAGS_DEBUG "-g3 -gdwarf-2" CACHE STRING "" FORCE)
		set (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g3 -gdwarf-2" CACHE STRING "" FORCE)
	endif (NOT ${_XME_ARCHITECTURE} STREQUAL "NiosII")
		
endif(CMAKE_COMPILER_IS_GNUCXX)

if (NOT CMAKE_BUILD_TYPE)
	set (CMAKE_BUILD_TYPE "Debug" CACHE STRING "" FORCE)
	message (STATUS "CMAKE_BUILD_TYPE not defined, defaulting to debug.")
endif (NOT CMAKE_BUILD_TYPE)

# Generate targets that allow switching the build configuration in eclipse.
# The current CDT generator does not offer native support for that, so
# you have to run dedicated targets.
if (CMAKE_EXTRA_GENERATOR STREQUAL "Eclipse CDT4")
	add_custom_target(buildTypeDebug
		${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=Debug ${PROJECT_SOURCE_DIR}
		COMMAND ${CMAKE_MAKE_PROGRAM} clean
		WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
		)

	add_custom_target(buildTypeRelease
		${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=Release ${PROJECT_SOURCE_DIR}
		COMMAND ${CMAKE_MAKE_PROGRAM} clean
		WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
		)

	add_custom_target(buildTypeReleaseWithDebugInfo
		${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=RelWithDebugInfo ${PROJECT_SOURCE_DIR}
		COMMAND ${CMAKE_MAKE_PROGRAM} clean
		WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
		)

	add_custom_target(buildTypeMinSizeRelease
		${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=MinSizeRel ${PROJECT_SOURCE_DIR}
		COMMAND ${CMAKE_MAKE_PROGRAM} clean
		WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
		)
endif (CMAKE_EXTRA_GENERATOR STREQUAL "Eclipse CDT4")

#
# Handle chromosome build options
#  - Provide default value
#  - Create include files since specifying all options on the command line
#    will exceed the command line limit (8192 on Windows).
#

# Configure options directory and add it to the include path
xme_set(PROPERTY_GLOBAL XME_OPTIONS_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/options")
xme_get(_XME_OPTIONS_INCLUDE_DIR PROPERTY_GLOBAL XME_OPTIONS_INCLUDE_DIR)
xme_include_directory("${_XME_OPTIONS_INCLUDE_DIR}")

# Purge options directory
file(REMOVE_RECURSE "${_XME_OPTIONS_INCLUDE_DIR}")
	
xme_build_option_file_open ("xme/xme_opt.h")
xme_build_option_file_open ("bsp/bsp_opt.h")
xme_build_option_file_open ("FreeRTOSConfig.h")
xme_build_option_file_open ("lwipopts.h")

xme_add_subdirectory(${XME_BSP_DIR} FALSE "Options.cmake")
xme_add_subdirectory(${XME_SRC_DIR} FALSE "Options.cmake")
xme_add_subdirectory(${XME_EXTERNAL_DIR} FALSE "Options.cmake")

xme_build_option_file_close ("xme/xme_opt.h")
xme_build_option_file_close ("bsp/bsp_opt.h")
xme_build_option_file_close ("FreeRTOSConfig.h")
xme_build_option_file_close ("lwipopts.h")

# Find related packages
find_package (Doxygen)

# Add CHROMOSOME BSP directory to current project
xme_add_subdirectory (${XME_BSP_DIR} TRUE)

# Add CHROMOSOME external directory to current project (-> set variables defining paths to specific components)
xme_add_subdirectory (${XME_EXTERNAL_DIR} TRUE)

# Add CHROMOSOME sources to the current project
xme_get (_XME_CURRENT_SOURCE_DIR PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR)
xme_set (PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR ${XME_SRC_DIR})

xme_add_subdirectory (${XME_SRC_DIR} TRUE)

xme_set (PROPERTY_GLOBAL XME_CURRENT_SOURCE_DIR ${_XME_CURRENT_SOURCE_DIR})

# Add custom search paths for CHROMOSOME components (typically used for third-party components)
foreach (_CD_ ${XME_COMPONENT_DIRS})
	xme_resolve_path(_CDS_ ${_CD_})
	message(STATUS "Adding custom component directory '${_CD_}' (which resolves to '${_CDS_}')")
	xme_add_subdirectory (${_CDS_})
endforeach (_CD_)

# Apply specific compiler settings
# Pendantic: treat warnings as errors and report (almost) all warnings
if (CMAKE_COMPILER_IS_GNUCXX)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
elseif (CMAKE_COMPILER_IS_GNUC)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")
elseif (MSVC)
	set (CMAKE_C_FLAGS "/WX /W3")
	set (CMAKE_CXX_FLAGS "/WX /W3")
endif (CMAKE_COMPILER_IS_GNUCXX)

# Check whether the testsuite should be built
if (XME_TESTSUITE)
	xme_add_subdirectory (${XME_TESTS_DIR})
	message (STATUS "Generated target for XME testsuite")
endif (XME_TESTSUITE)

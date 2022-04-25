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
#         Simon Barner <barner@fortiss.org>
#

######################################################
###   Root path of Sourcery Codebench installation ###
######################################################
if(WIN32)
find_path (
    SOURCERY_CODEBENCH_PATH
    bin/arm-none-eabi-gcc.exe
	$ENV{BELLE_TOOLCHAIN}/arm-gcc
)
else(WIN32)
find_path(
    SOURCERY_CODEBENCH_PATH
    bin/arm-none-eabi-gcc
	/usr/bin
	/usr/local/bin
)
endif(WIN32)

######################################################
###   Sourcery Codebench                           ###
######################################################
# $PATH is also searched

find_program(
	SOURCERY_CODEBENCH_C_COMPILER
	arm-none-eabi-gcc
	${SOURCERY_CODEBENCH_PATH}/bin
)

find_program(
	SOURCERY_CODEBENCH_CXX_COMPILER
	arm-none-eabi-g++
	${SOURCERY_CODEBENCH_PATH}/bin
)

find_program(
	SOURCERY_CODEBENCH_DEBUGGER
	arm-none-eabi-gdb
	${SOURCERY_CODEBENCH_PATH}/bin
)

if(${SOURCERY_CODEBENCH_C_COMPILER} STREQUAL "SOURCERY_CODEBENCH_C_COMPILER-NOTFOUND")
	SET (SOURCERY_CODEBENCH_FOUND
		FALSE
	)
else(${SOURCERY_CODEBENCH_C_COMPILER} STREQUAL "SOURCERY_CODEBENCH_C_COMPILER-NOTFOUND")
	SET (CODESOURCERY_FOUND
		TRUE
	)
endif(${SOURCERY_CODEBENCH_C_COMPILER} STREQUAL "SOURCERY_CODEBENCH_C_COMPILER-NOTFOUND")

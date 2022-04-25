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
###   Root path of OpenOCD installation            ###
######################################################
xme_get (_XME_BUILD_HOST PROPERTY_GLOBAL XME_BUILD_HOST)
if (_XME_BUILD_HOST STREQUAL "windows")
find_path (
	OPENOCD_PATH
	bin/openocd.exe
	$ENV{BELLE_TOOLCHAIN}/openocd
)
else(_XME_BUILD_HOST STREQUAL "windows")
find_path(
	OPENOCD_PATH
	bin/openocd
	/usr
	/usr/local
)
endif (_XME_BUILD_HOST STREQUAL "windows")

######################################################
###   OpenOCD                                      ###
######################################################
# $PATH is also searched

find_program(
	OPENOCD
	openocd
	${OPENOCD_PATH}/bin
)

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
###   TFTP client                                  ###
######################################################
if (_XME_BUILD_HOST STREQUAL "windows")
# TODO
#
# find_program does not work on Windows (7) since tftp comes from a deployment package
# and its real path is something like
# C:/Windows/winsxs/amd64_microsoft-windows-t..-deployment-package_31bf3856ad364e35_6.1.7600.16385_none_bac291589d407fde
# It is listed in c:/windows/system32 using some kind of overlay, but cannot be found
# be find_program there.
#
# Since we need a full path in eclipse launch configurations, we use a local copy
# until this is fixed.
#
find_program(
	TFTP
	tftp.bat
	$ENV{BELLE_TOOLCHAIN}/tools/bin
)

else(_XME_BUILD_HOST STREQUAL "windows")
find_program(
	TFTP
	tftp
	/usr/bin
	/usr/local/bin
)
endif (_XME_BUILD_HOST STREQUAL "windows")

if (NOT ${TFTP} STREQUAL "TFTP-NOTFOUND")
	set (TFTP_FOUND TRUE)
else (NOT ${TFTP} STREQUAL "TFTP-NOTFOUND")
	set (TFTP_FOUND FALSE)
endif (NOT ${TFTP} STREQUAL "TFTP-NOTFOUND")

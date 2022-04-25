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
#         Benjamin Wiesmueller <wiesmueller@fortiss.org>
#

# This CPackConfig script can be used to create an XME package.
# All installed projects must be built before starting this script. Note that 
# the INSTALL target depends on ALL_BUILD, so you need to build the whole project 
# (including documentation and tests).
#
# To run this script execute:
# cpack -C <configuration> --config <config-file>
# Where <configuration> is [Release|Debug|...] and <config-file> is the CPackConfig
# file to use.

# Directory where the projects are located
set (__PROJECT_BASE_DIR__ ".")

# __PROJECT_LIST__ is a list of pairs: <dir>;<name>
# Where <dir> is the directory of the project relative to __PROJECT_BASE_DIR__ and
# <name> is the name of the project as defined in its CMakeLists.txt
set (__PROJECT_LIST__ "tutorial;Tutorial;coordinator;Coordinator;templateGenerator;TemplateGenerator")
# TODO!

set (CPACK_PACKAGE_NAME "XMETutorial")
set (CPACK_PACKAGE_VERSION "0.2.0")
set (CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-win32")
set (CPACK_PACKAGE_DESCRIPTION_FILE "PackageDescription.txt")
set (CPACK_RESOURCE_FILE_LICENSE "C:/WorkingCopies/chromosome/trunk/LICENSE.txt")
set (CPACK_GENERATOR "ZIP")
set (CPACK_CMAKE_GENERATOR "Visual Studio 10")

include ("./XmeCPack.cmake")

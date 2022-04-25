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

# Some convenience commands that can be included in CPackConfig files.
#
# Set the following variables before including this:
# __PROJECT_LIST__      A list of pairs: <dir>;<name> (e.g. "light;LightNode" "switch;SwitchNode")
#                       Where <dir> is the directory of the project relative to __PROJECT_BASE_DIR__
#                       and <name> is the name of the project as defined in its CMakeLists.txt
# __PROJECT_BASE_DIR__  Directory where the projects are located
# CPACK_PACKAGE_VERSION Version for package in format <major>.<minor>.<patch>
#
# This script will add the given projects to the packaged projects and set the
# CPACK_PACKAGE_VERSION_MAJOR/_MINOR/_PATCH variables according to CPACK_PACKAGE_VERSION.

string (REPLACE "." ";"  __CPACK_PACKAGE_VERSION_SPLIT__ ${CPACK_PACKAGE_VERSION})
list (GET __CPACK_PACKAGE_VERSION_SPLIT__ 0 CPACK_PACKAGE_VERSION_MAJOR)
list (GET __CPACK_PACKAGE_VERSION_SPLIT__ 1 CPACK_PACKAGE_VERSION_MINOR)
list (GET __CPACK_PACKAGE_VERSION_SPLIT__ 2 CPACK_PACKAGE_VERSION_PATCH)

list (LENGTH __PROJECT_LIST__ __PROJECT_LIST_LENGTH__)
set (__INDEX__ 0)
while (${__INDEX__} LESS ${__PROJECT_LIST_LENGTH__})
    list (GET __PROJECT_LIST__ ${__INDEX__} __PROJECT_DIR__)
    math (EXPR __INDEX__ "${__INDEX__} + 1")
    list (GET __PROJECT_LIST__ ${__INDEX__} __PROJECT_NAME__)
    math (EXPR __INDEX__ "${__INDEX__} + 1")   
    
    set(
        CPACK_INSTALL_CMAKE_PROJECTS
        ${CPACK_INSTALL_CMAKE_PROJECTS}
        "${__PROJECT_BASE_DIR__}/build/${__PROJECT_DIR__};${__PROJECT_NAME__};ALL;/"
    )
endwhile (${__INDEX__} LESS ${__PROJECT_LIST_LENGTH__})

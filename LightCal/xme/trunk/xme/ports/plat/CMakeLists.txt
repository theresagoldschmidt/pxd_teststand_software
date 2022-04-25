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
#         Michael Geisinger <geisinger@fortiss.org>
#

xme_get(_XME_PLATFORM PROPERTY_GLOBAL XME_PLATFORM)

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/${_XME_PLATFORM}")
	xme_add_subdirectory(${_XME_PLATFORM})
endif (EXISTS "${CMAKE_CURRENT_LIST_DIR}/${_XME_PLATFORM}")

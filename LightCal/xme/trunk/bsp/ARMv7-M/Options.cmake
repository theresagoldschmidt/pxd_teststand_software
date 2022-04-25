#
# Copyright (c) 2011-2012, fortiss GmbH.
# Licensed under the Apache License, Version 2.0.
# 
# Use, modification and distribution are subject to the terms specified
# in the accompanying license file LICENSE.txt located at the root directory
# of this software distribution. A copy is available at
# http://chromosome.fortiss.org/.
#
# This file is part of CHROMOSOME.
#
# $Id$
#
# Author:
#         Simon Barner <barner@fortiss.org>
#

xme_get(_XME_BOARD PROPERTY_GLOBAL XME_BOARD)
xme_add_subdirectory(${_XME_BOARD} FALSE "Options.cmake")

# xme_build_option_weak_set: Value can be overridden by user

# lwIP options (common to all Cortex M3 parts)
xme_build_option_weak_set(EXTERNAL_LWIP_MEM_ALIGNMENT 4)

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

# lwIP will influence some FreeRTOS settings

xme_get(_XME_ARCHITECTURE PROPERTY_GLOBAL XME_ARCHITECTURE)

if (_XME_ARCHITECTURE STREQUAL "ARMv7-M")
	xme_add_subdirectory("lwIP" FALSE "Options.cmake")
	xme_add_subdirectory("FreeRTOS" FALSE "Options.cmake")
endif (_XME_ARCHITECTURE STREQUAL "ARMv7-M")

if (_XME_ARCHITECTURE STREQUAL "NiosII")
	xme_add_subdirectory("FreeRTOS" FALSE "Options.cmake")
	#s temporary remove the lwIP for NiosII	
	xme_add_subdirectory("lwIP" FALSE "Options.cmake")
endif (_XME_ARCHITECTURE STREQUAL "NiosII")
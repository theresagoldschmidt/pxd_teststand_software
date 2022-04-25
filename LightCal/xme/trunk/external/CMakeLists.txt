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
#         Hauke Staehle <staehle@fortiss.org>
#         Kai Huang <khuang@fortiss.org>
#

# Make this directory appear as a subproject in the project explorer of eclipse
project (external)

xme_get(_XME_ARCHITECTURE PROPERTY_GLOBAL XME_ARCHITECTURE)
xme_get(_XME_FAMILY PROPERTY_GLOBAL XME_FAMILY)

if (_XME_ARCHITECTURE STREQUAL "NiosII")
	# Set lwIP dir: CACHE Variable XME_EXTERNAL_LWIP_DIR (via find_file)
	if (NOT DEFINED XME_EXTERNAL_LWIP_DIR)
		find_file (XME_EXTERNAL_LWIP_DIR "lwIP" PATHS "${XME_EXTERNAL_DIR}" DOC "CHROMOSOME external directory: lwIP" NO_DEFAULT_PATH)
	endif (NOT DEFINED XME_EXTERNAL_LWIP_DIR)

	xme_add_subdirectory(${XME_EXTERNAL_LWIP_DIR})

	# Set FreeRTOS dir: CACHE Variable XME_EXTERNAL_FREERTOS_DIR (via find_file)
	if (NOT DEFINED XME_EXTERNAL_FREERTOS_DIR)
		find_file (XME_EXTERNAL_FREERTOS_DIR "FreeRTOS" PATHS "${XME_EXTERNAL_DIR}" DOC "CHROMOSOME external directory: FreeRTOS" NO_DEFAULT_PATH)
	endif (NOT DEFINED XME_EXTERNAL_FREERTOS_DIR)

	xme_get(_XME_PLATFORM PROPERTY_GLOBAL XME_PLATFORM)

	if (_XME_PLATFORM STREQUAL "freertos")
		xme_add_subdirectory(${XME_EXTERNAL_FREERTOS_DIR})
	endif (_XME_PLATFORM STREQUAL "freertos")
endif (_XME_ARCHITECTURE STREQUAL "NiosII")


if (_XME_ARCHITECTURE STREQUAL "ARMv7-M")
	if (_XME_FAMILY STREQUAL "STM")
		# Set CMSIS dir: CACHE Variable XME_EXTERNAL_CMSIS_DIR (via find_file)
		if (NOT DEFINED XME_EXTERNAL_CMSIS_DIR)
			find_file (XME_EXTERNAL_CMSIS_DIR "CMSIS" PATHS "${XME_EXTERNAL_DIR}" DOC "CHROMOSOME external directory: CMSIS" NO_DEFAULT_PATH)
		endif (NOT DEFINED XME_EXTERNAL_CMSIS_DIR)

		xme_add_subdirectory(${XME_EXTERNAL_CMSIS_DIR})

		# Set STM32F10xSPL dir: CACHE Variable XME_EXTERNAL_STM32F10XSPL_DIR (via find_file)
		if (NOT DEFINED XME_EXTERNAL_STM32F10XSPL_DIR)
			find_file (XME_EXTERNAL_STM32F10XSPL_DIR "STM32F10xSPL" PATHS "${XME_EXTERNAL_DIR}" DOC "CHROMOSOME external directory: STM32F10xSPL" NO_DEFAULT_PATH)
		endif (NOT DEFINED XME_EXTERNAL_STM32F10XSPL_DIR)

		xme_add_subdirectory(${XME_EXTERNAL_STM32F10XSPL_DIR})
	endif (_XME_FAMILY STREQUAL "STM")

	if (_XME_FAMILY STREQUAL "LM3")
		# Set Stellarisware dir
		if (NOT DEFINED XME_EXTERNAL_STELLARISWARE_DIR)
			find_file (XME_EXTERNAL_STELLARISWARE_DIR "StellarisWare" PATHS "${XME_EXTERNAL_DIR}" DOC "CHROMOSOME external directory: StellarisWare" NO_DEFAULT_PATH)
		endif (NOT DEFINED XME_EXTERNAL_STELLARISWARE_DIR)

		xme_add_subdirectory(${XME_EXTERNAL_STELLARISWARE_DIR})
	endif (_XME_FAMILY STREQUAL "LM3")	

	# Set lwIP dir: CACHE Variable XME_EXTERNAL_LWIP_DIR (via find_file)
	if (NOT DEFINED XME_EXTERNAL_LWIP_DIR)
		find_file (XME_EXTERNAL_LWIP_DIR "lwIP" PATHS "${XME_EXTERNAL_DIR}" DOC "CHROMOSOME external directory: lwIP" NO_DEFAULT_PATH)
	endif (NOT DEFINED XME_EXTERNAL_LWIP_DIR)

	xme_add_subdirectory(${XME_EXTERNAL_LWIP_DIR})

	# Set FreeRTOS dir: CACHE Variable XME_EXTERNAL_FREERTOS_DIR (via find_file)
	if (NOT DEFINED XME_EXTERNAL_FREERTOS_DIR)
		find_file (XME_EXTERNAL_FREERTOS_DIR "FreeRTOS" PATHS "${XME_EXTERNAL_DIR}" DOC "CHROMOSOME external directory: FreeRTOS" NO_DEFAULT_PATH)
	endif (NOT DEFINED XME_EXTERNAL_FREERTOS_DIR)

	xme_get(_XME_PLATFORM PROPERTY_GLOBAL XME_PLATFORM)

	if (_XME_PLATFORM STREQUAL "freertos")
		xme_add_subdirectory(${XME_EXTERNAL_FREERTOS_DIR})
	endif (_XME_PLATFORM STREQUAL "freertos")
endif (_XME_ARCHITECTURE STREQUAL "ARMv7-M")

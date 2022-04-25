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

# Define directories

xme_get (_XME_TARGET PROPERTY_GLOBAL XME_TARGET)
string (REGEX MATCH "^STM32F10" _IS_STM32F10X ${_XME_TARGET})

if (_IS_STM32F10X)

	# Set CMSIS core support dir: CACHE Variable XME_EXTERNAL_CMSIS_CORE_SUPPORT_DIR (via find_file)
	if (NOT DEFINED XME_EXTERNAL_CMSIS_CORE_SUPPORT_DIR)
		find_file (XME_EXTERNAL_CMSIS_CORE_SUPPORT_DIR "CoreSupport" PATHS "${XME_EXTERNAL_CMSIS_DIR}/CM3" DOC "CHROMOSOME external directory: CMSIS core support" NO_DEFAULT_PATH)
	endif (NOT DEFINED XME_EXTERNAL_CMSIS_CORE_SUPPORT_DIR)

	# Set CMSIS device support dir: CACHE Variable XME_EXTERNAL_CMSIS_DEVICE_SUPPORT_DIR (via find_file)
	if (NOT DEFINED XME_EXTERNAL_CMSIS_DEVICE_SUPPORT_DIR)
		find_file (XME_EXTERNAL_CMSIS_DEVICE_SUPPORT_DIR "DeviceSupport/ST/STM32F10x" PATHS "${XME_EXTERNAL_CMSIS_DIR}/CM3" DOC "CHROMOSOME external directory: CMSIS device support" NO_DEFAULT_PATH)
	endif (NOT DEFINED XME_EXTERNAL_CMSIS_DEVICE_SUPPORT_DIR)

	# Set CMSIS startup code dir: CACHE Variable XME_EXTERNAL_CMSIS_STARTUP_CODE_DIR (via find_file)
	if (NOT DEFINED XME_EXTERNAL_CMSIS_STARTUP_CODE_DIR)
		find_file (XME_EXTERNAL_CMSIS_STARTUP_CODE_DIR "startup" PATHS "${XME_EXTERNAL_CMSIS_DEVICE_SUPPORT_DIR}" DOC "CHROMOSOME external directory: CMSIS startup code" NO_DEFAULT_PATH)
	endif (NOT DEFINED XME_EXTERNAL_CMSIS_STARTUP_CODE_DIR)

	# Include directories
	xme_include_directory(
		${XME_EXTERNAL_CMSIS_CORE_SUPPORT_DIR}
		${XME_EXTERNAL_CMSIS_DEVICE_SUPPORT_DIR}
	)

	# Defines
	xme_definition(
		-DSTM32F10X_CL
		-DUSE_STDPERIPH_DRIVER
	)

	# Define target specific XME components, i.e. components below the HAL layer.
	# The latter are defined in ${XME_SRC_DIR}/ports/arch/ARMv7-M/hal
	#
	# TODO: This components needs to be linked with the following command:
	#       -Wl,--whole-archive xme_target_startup --Wl,--no-whole-archive
	#
	#		We need to enhance xme_link_components() to be able to specify
	#		linker options.
	
	# Startup code
	xme_add_component(
		"xme_target_startup"
		${XME_EXTERNAL_CMSIS_STARTUP_CODE_DIR}/gcc_ride7/startup_stm32f10x_cl.S
	)

	# Board support package
	# - system initialization
	# - depends on: startup code
	xme_add_component(
		"xme_target_bsp"
		${XME_EXTERNAL_CMSIS_CORE_SUPPORT_DIR}/core_cm3.c
		${XME_EXTERNAL_CMSIS_CORE_SUPPORT_DIR}/core_cm3.h
		${XME_EXTERNAL_CMSIS_DEVICE_SUPPORT_DIR}/system_stm32f10x.c
		${XME_EXTERNAL_CMSIS_DEVICE_SUPPORT_DIR}/system_stm32f10x.h
		${XME_EXTERNAL_CMSIS_DEVICE_SUPPORT_DIR}/stm32f10x.h
		xme_target_startup
	)

else (_IS_STM32F10X)
	message (FATAL_ERROR "Cannot use CMSIS external library: XME_TARGET not specified or unkown target device: ${_XME_TARGET}")
endif (_IS_STM32F10X)
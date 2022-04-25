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

# Define generic settings

# Set FreeRTOS include dir: CACHE Variable XME_EXTERNAL_FREERTOS_INCLUDE_DIR (via find_file)
if (NOT DEFINED XME_EXTERNAL_FREERTOS_INCLUDE_DIR)
	find_file (XME_EXTERNAL_FREERTOS_INCLUDE_DIR "include" PATHS "${XME_EXTERNAL_FREERTOS_DIR}" DOC "CHROMOSOME external directory: FreeRTOS include" NO_DEFAULT_PATH)
endif (NOT DEFINED XME_EXTERNAL_FREERTOS_INCLUDE_DIR)


# Include directories
xme_include_directory(
	${XME_EXTERNAL_FREERTOS_INCLUDE_DIR}
)

# Components
xme_add_component(
	"xme_freertos_kernel"
	${XME_EXTERNAL_FREERTOS_DIR}/list.c
	${XME_EXTERNAL_FREERTOS_DIR}/queue.c
	${XME_EXTERNAL_FREERTOS_DIR}/tasks.c
	${XME_EXTERNAL_FREERTOS_INCLUDE_DIR}/FreeRTOS.h
	${XME_EXTERNAL_FREERTOS_INCLUDE_DIR}/list.h
	${XME_EXTERNAL_FREERTOS_INCLUDE_DIR}/portable.h
	${XME_EXTERNAL_FREERTOS_INCLUDE_DIR}/projdefs.h
	${XME_EXTERNAL_FREERTOS_INCLUDE_DIR}/queue.h
	${XME_EXTERNAL_FREERTOS_INCLUDE_DIR}/semphr.h
	${XME_EXTERNAL_FREERTOS_INCLUDE_DIR}/StackMacros.h
	${XME_EXTERNAL_FREERTOS_INCLUDE_DIR}/task.h
    ${XME_EXTERNAL_FREERTOS_INCLUDE_DIR}/mpu_wrappers.h
)

xme_add_component(
	"xme_freertos_coroutine"
	${XME_EXTERNAL_FREERTOS_DIR}/croutine.c
	${XME_EXTERNAL_FREERTOS_INCLUDE_DIR}/croutine.h
)

xme_add_component(
	"xme_freertos_timers"
	${XME_EXTERNAL_FREERTOS_DIR}/timers.c
	${XME_EXTERNAL_FREERTOS_INCLUDE_DIR}/timers.h
)

# Define architecture specific settings
xme_defined (_XME_TARGET_DEFINED PROPERTY_GLOBAL XME_TARGET)

if (NOT _XME_TARGET_DEFINED)
	set (_XME_TARGET "_")
else (NOT _XME_TARGET_DEFINED)
	xme_get (_XME_TARGET PROPERTY_GLOBAL XME_TARGET)
endif (NOT _XME_TARGET_DEFINED)
string (REGEX MATCH "^STM32F10" _IS_STM32F10X ${_XME_TARGET})
string (REGEX MATCH "^LM3S8962" _IS_LM3S8962 ${_XME_TARGET})
string (REGEX MATCH "^EP3C120F780C7N" _IS_EP3C120F780C7N ${_XME_TARGET})

# Set FreeRTOS port dir: CACHE Variable XME_EXTERNAL_FREERTOS_PORT_DIR (via find_file)
if (NOT DEFINED XME_EXTERNAL_FREERTOS_PORT_DIR)
	find_file (XME_EXTERNAL_FREERTOS_PORT_DIR "portable" PATHS "${XME_EXTERNAL_FREERTOS_DIR}" DOC "CHROMOSOME external directory: FreeRTOS port" NO_DEFAULT_PATH)
endif (NOT DEFINED XME_EXTERNAL_FREERTOS_PORT_DIR)

xme_definition(
	-DXME_PLATFORM_FREERTOS
)

if (_IS_STM32F10X OR _IS_LM3S8962)
	
	xme_include_directory(
		${XME_EXTERNAL_FREERTOS_PORT_DIR}/GCC/ARM_CM3
	)
	

	# Augment startup code with FreeRTOS interrupt handlers.
	#
	# Since the startup code will be added using the --whole-archive linker
	# flag, it it ensured that the interrupt handlers actually appear in the
	# binary image. 
	xme_add_component(
		"xme_target_startup"
		${XME_EXTERNAL_FREERTOS_PORT_DIR}/GCC/ARM_CM3/port.c
		${XME_EXTERNAL_FREERTOS_PORT_DIR}/GCC/ARM_CM3/portmacro.h
	)

	# Components: Functions defined in port.c are part of the kernel
	xme_add_component(
		"xme_freertos_kernel"
		"xme_freertos_memmang"
		"xme_target_startup"
	)

	xme_defined (_XME_MEMORYMANAGEMENT_DEFINED PROPERTY_GLOBAL XME_MEMORYMANAGEMENT)
	if (NOT ${_XME_MEMORYMANAGEMENT_DEFINED})
		xme_set (PROPERTY_GLOBAL XME_MEMORYMANAGEMENT "static")
		message (STATUS "XME_MEMORYMANAGEMENT not defined. Defaulting to 'static'")
	endif (NOT ${_XME_MEMORYMANAGEMENT_DEFINED})

	xme_get(_XME_MEMORYMANAGEMENT PROPERTY_GLOBAL XME_MEMORYMANAGEMENT)

	if (_XME_MEMORYMANAGEMENT STREQUAL "static")	
		xme_add_component(
			"xme_freertos_memmang"
			${XME_EXTERNAL_FREERTOS_PORT_DIR}/MemMang/heap_1.c
		)
	else (_XME_MEMORYMANAGEMENT STREQUAL "static")	
		xme_add_component(
			"xme_freertos_memmang"
			${XME_EXTERNAL_FREERTOS_PORT_DIR}/MemMang/heap_2.c
		)
	endif (_XME_MEMORYMANAGEMENT STREQUAL "static")		
elseif (_IS_EP3C120F780C7N) 

	# CycloneIII EP3C120F780C7N board
	xme_include_directory(
            ${XME_EXTERNAL_FREERTOS_PORT_DIR}/GCC/NiosII
    )
   
    xme_get (_XME_ARCHITECTURE PROPERTY_GLOBAL XME_ARCHITECTURE)
    xme_get (_XME_BOARD PROPERTY_GLOBAL XME_BOARD)
	# the location of the bsp
    set (__XME_BSP_DIR "${XME_BSP_DIR}/${_XME_ARCHITECTURE}/${_XME_BOARD}/gen_bsp")
	xme_include_directory(
			"${__XME_BSP_DIR}/HAL/inc"
			"${__XME_BSP_DIR}/drivers/inc"
			"${__XME_BSP_DIR}"
    )   

    xme_add_component(
        "xme_target_startup"
        ${XME_EXTERNAL_FREERTOS_PORT_DIR}/GCC/NiosII/port.c
        ${XME_EXTERNAL_FREERTOS_PORT_DIR}/GCC/NiosII/port_asm.S
        ${XME_EXTERNAL_FREERTOS_PORT_DIR}/GCC/NiosII/portmacro.h
        ${XME_EXTERNAL_FREERTOS_PORT_DIR}/MemMang/heap_2.c
    )

    # Components: Functions defined in port.c are part of the kernel
    xme_add_component(
        "xme_freertos_kernel"
        #"xme_freertos_memmang"
        "xme_target_startup"
    )

#else (_IS_EP3C120F780C7N)
else (_IS_STM32F10X OR _IS_LM3S8962)
	if (_XME_TARGET_DEFINED)
		message ("Cannot use FreeRTOS external library: XME_TARGET refers to unknown target device: ${_XME_TARGET}")
	else (_XME_TARGET_DEFINED)
		message ("Cannot use FreeRTOS external library: XME_TARGET not specified.")
	endif (_XME_TARGET_DEFINED)
endif (_IS_STM32F10X OR _IS_LM3S8962)

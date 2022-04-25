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

# Set STM32F10xSPL include dir: CACHE Variable XME_EXTERNAL_STM32F10XSPL_INC_DIR (via find_file)
if (NOT DEFINED XME_EXTERNAL_STM32F10XSPL_INC_DIR)
	find_file (XME_EXTERNAL_STM32F10XSPL_INC_DIR "inc" PATHS "${XME_EXTERNAL_STM32F10XSPL_DIR}" DOC "CHROMOSOME external directory: STM32F10xSPL include dir" NO_DEFAULT_PATH)
endif (NOT DEFINED XME_EXTERNAL_STM32F10XSPL_INC_DIR)

# Set STM32F10xSPL source dir: CACHE Variable XME_EXTERNAL_STM32F10XSPL_SRC_DIR (via find_file)
if (NOT DEFINED XME_EXTERNAL_STM32F10XSPL_SRC_DIR)
	find_file (XME_EXTERNAL_STM32F10XSPL_SRC_DIR "src" PATHS "${XME_EXTERNAL_STM32F10XSPL_DIR}" DOC "CHROMOSOME external directory: STM32F10xSPL source dir" NO_DEFAULT_PATH)
endif (NOT DEFINED XME_EXTERNAL_STM32F10XSPL_SRC_DIR)
	
# Include directories
xme_include_directory(
	${XME_EXTERNAL_STM32F10XSPL_INC_DIR}
)

# Define target specific XME components, i.e. components below the HAL layer.
# The latter are defined in ${XME_SRC_DIR}/ports/arch/ARMv7-M/hal

# - reset and clock control
xme_add_component(
	"xme_stm32f0xspl_rcc"
	${XME_EXTERNAL_STM32F10XSPL_SRC_DIR}/stm32f10x_rcc.c
	${XME_EXTERNAL_STM32F10XSPL_INC_DIR}/stm32f10x_rcc.h
)

# Misc: NVIC support etc.
xme_add_component(
	"xme_stm32f0xspl_misc"
	${XME_EXTERNAL_STM32F10XSPL_SRC_DIR}/misc.c
	${XME_EXTERNAL_STM32F10XSPL_INC_DIR}/misc.h
	${XME_EXTERNAL_STM32F10XSPL_SRC_DIR}/stm32f10x_exti.c
	${XME_EXTERNAL_STM32F10XSPL_INC_DIR}/stm32f10x_exti.h
)

# GPIO 
xme_add_component(
	"xme_stm32f0xspl_gpio"
	${XME_EXTERNAL_STM32F10XSPL_SRC_DIR}/stm32f10x_gpio.c
	${XME_EXTERNAL_STM32F10XSPL_INC_DIR}/stm32f10x_gpio.h
)

# SPI
xme_add_component(
	"xme_stm32f0xspl_spi"
	${XME_EXTERNAL_STM32F10XSPL_SRC_DIR}/stm32f10x_spi.c
	${XME_EXTERNAL_STM32F10XSPL_INC_DIR}/stm32f10x_spi.h
)

# I2C
xme_add_component(
	"xme_stm32f0xspl_i2c"
	${XME_EXTERNAL_STM32F10XSPL_SRC_DIR}/stm32f10x_i2c.c
	${XME_EXTERNAL_STM32F10XSPL_INC_DIR}/stm32f10x_i2c.h
)

# ADC
xme_add_component(
	"xme_stm32f0xspl_adc"
	${XME_EXTERNAL_STM32F10XSPL_SRC_DIR}/stm32f10x_adc.c
	${XME_EXTERNAL_STM32F10XSPL_INC_DIR}/stm32f10x_adc.h
)


# Board support package:
# Add RCC support since this is required in the startup code to
# increase the clock speed of the MCU
xme_add_component(
	"xme_target_bsp"
	xme_stm32f0xspl_rcc
)

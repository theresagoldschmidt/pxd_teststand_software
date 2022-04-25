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
#         Hauke Staehle <staehle@fortiss.org>
#

# Retrieve relative path to generic C implementation

#xme_generic_port_path (_GEN_PATH "gen_c" 2)

#xme_add_component(
#	"xme_hal_math"
#	${_GEN_PATH}/math_arch.c ${_GEN_PATH}/math_arch.h
#)

#xme_add_component(
#	"xme_hal_console"
#	${_GEN_PATH}/console_arch.c
#	xme_hal_display
#)

# Reset-and-clock control provided by BSP
xme_add_component(
	"xme_hal_dio"
	dio_arch.c dio_arch.h
	"xme_target_bsp"
)

#xme_get (_XME_TARGET_FLASH_ADDRESS PROPERTY_GLOBAL XME_TARGET_FLASH_ADDRESS)
#xme_get (_XME_TARGET_FLASH_SIZE PROPERTY_GLOBAL XME_TARGET_FLASH_SIZE)
#xme_get (_XME_TARGET_BOOTLOADER_SIZE PROPERTY_GLOBAL XME_TARGET_BOOTLOADER_SIZE)

#hex2dec (${_XME_TARGET_FLASH_ADDRESS} _XME_TARGET_FLASH_ADDRESS_DEC)
#hex2dec (${_XME_TARGET_FLASH_SIZE} _XME_TARGET_FLASH_SIZE_DEC)
#hex2dec (${_XME_TARGET_BOOTLOADER_SIZE} _XME_TARGET_BOOTLOADER_SIZE_DEC)

#math(EXPR _FLASH_USER_FIRST_PAGE_ADDRESS_DEC "${_XME_TARGET_FLASH_ADDRESS_DEC} + ${_XME_TARGET_BOOTLOADER_SIZE_DEC}")
#dec2hex(${_FLASH_USER_FIRST_PAGE_ADDRESS_DEC} _FLASH_USER_FIRST_PAGE_ADDRESS)

#math(EXPR _FLASH_USER_END_ADDRESS_DEC "${_XME_TARGET_FLASH_ADDRESS_DEC} + ${_XME_TARGET_FLASH_SIZE_DEC} - 1")
#dec2hex(${_FLASH_USER_END_ADDRESS_DEC} _FLASH_USER_END_ADDRESS)

# xme_definition(
#	"-DXME_HAL_INTERNAL_FLASH_USER_FIRST_PAGE_ADDRESS=${_FLASH_USER_FIRST_PAGE_ADDRESS}"
#	"-DXME_HAL_INTERNAL_FLASH_USER_END_ADDRESS=${_FLASH_USER_END_ADDRESS}"
#)

#xme_add_component(
#	"xme_hal_persistence"
#	internal_flash.c
#	internal_flash.h
#)

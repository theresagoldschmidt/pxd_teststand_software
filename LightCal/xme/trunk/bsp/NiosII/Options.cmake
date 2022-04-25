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
#         Kai Huang <khuang@fortiss.org>

xme_get(_XME_BOARD PROPERTY_GLOBAL XME_BOARD)
xme_add_subdirectory(${_XME_BOARD} FALSE "Options.cmake")

# xme_build_option_weak_set: Value can be overridden by user

# lwIP options (common to all Cortex M3 parts)
xme_build_option_weak_set(EXTERNAL_LWIP_MEM_ALIGNMENT 4)


# BSP: Ethernet options
# xme_build_option(XME_BSP_STM32_ETHERNET_AF_DEFAULT 1 "bsp/bsp_opt.h")
# xme_build_option(XME_BSP_STM32_ETHERNET_RMII 1 "bsp/bsp_opt.h")
# xme_build_option(XME_BSP_STM32_ETHERNET_RMII_PLL3 1 "bsp/bsp_opt.h")
# xme_build_option(XME_BSP_STM32_ETHERNET_PHY_STE101P 1 "bsp/bsp_opt.h")

# xme_build_option_weak_set: Value can be overridden by user

# FreeRTOS options
# The frequency depends on the startup code which is currently in external/CMSIS for this MCU

# FixME: Should be consistent with the BSP setting in the NiosII IDA !!!
xme_build_option_weak_set(EXTERNAL_FREERTOS_configCPU_CLOCK_HZ "( ( unsigned long ) 72000000 )")
xme_build_option_weak_set(EXTERNAL_FREERTOS_configTOTAL_HEAP_SIZE 20480)
xme_build_option_weak_set(EXTERNAL_FREERTOS_configMINIMAL_STACK_SIZE 256)

# lwIP options
xme_build_option_weak_set(EXTERNAL_LWIP_MEM_SIZE                     1600)
xme_build_option_weak_set(EXTERNAL_LWIP_CHECKSUM_BY_HARDWARE         1)

# FixME: for this version, no MAC setting
xme_build_option_set(MAC_ADDR_FIRMWARE                               "0" "MAC address in non-volatile registers USER{1,2}")

# xme_build_option_set (EXTERNAL_LWIP_SYS_LIGHTWEIGHT_PROT 1 "Required since the STM32 interface driver uses interrupts.")
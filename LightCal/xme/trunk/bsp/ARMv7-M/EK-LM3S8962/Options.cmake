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

# BSP options:
# TODO: See ticket #904

# xme_build_option_weak_set: Value can be overridden by user

# FreeRTOS options
# The frequency depends on the startup code which is currently in TODO for this MCU
#
# TODO: Verify these settings! See ticket #904
#
xme_build_option_weak_set(EXTERNAL_FREERTOS_configCPU_CLOCK_HZ      "( ( unsigned long ) 50000000 )")
xme_build_option_weak_set(EXTERNAL_FREERTOS_configTOTAL_HEAP_SIZE    20480)
xme_build_option_weak_set(EXTERNAL_FREERTOS_configMINIMAL_STACK_SIZE 256)

# lwIP options
xme_build_option_weak_set(EXTERNAL_LWIP_MEM_SIZE                     1600)
xme_build_option_set (EXTERNAL_LWIP_SYS_LIGHTWEIGHT_PROT             1 "Required since the LM3S8962's interface driver uses interrupts.")

xme_build_option_set(EXTERNAL_LWIP_PBUF_LINK_HLEN                    16 "The LM3S8962 has a 16 byte link layer header.")
xme_build_option_set(EXTERNAL_LWIP_ETH_PAD_SIZE                      2 "The LM3S8962's MAC address is padded by 2 bytes in the LM3S8962 link layer header.")


# TOOD (See ticket #904): Check, if this can be integrated into the Luminary driver
#xme_build_option_weak_set(EXTERNAL_LWIP_CHECKSUM_BY_HARDWARE         1)

# Network settings
# For the LM3S6965 Evaluation Kit, the MAC address will be stored in the
# non-volatile USER0 and USER1 registers, so the user is not required
# to specify a MAC address that is to be compiled into the firmware.
xme_build_option_set(MAC_ADDR_FIRMWARE                               "0" "MAC address in non-volatile registers USER{1,2}")

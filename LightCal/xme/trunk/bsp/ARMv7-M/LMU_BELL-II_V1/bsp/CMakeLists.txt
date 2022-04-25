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

# Board support package
xme_add_component(
	"xme_target_bsp"
	pins_bootloader.h
	pins_buttons.h
	pins_leds.h
	pins_network.h
	pins_custom.h
	../../stm32/bsp/enet_arch.c
	../../stm32/bsp/enet_arch.h
	../../stm32/bsp/pins_eth.h
)

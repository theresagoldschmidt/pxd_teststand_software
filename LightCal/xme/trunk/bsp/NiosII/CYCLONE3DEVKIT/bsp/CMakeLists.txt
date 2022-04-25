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

# FixME !!!!! need the real files

# Board support package
xme_add_component(
	"xme_target_bsp"
	pins_leds.h
	pins_network.h
	pins_buttons.h
	enet_arch.h
)

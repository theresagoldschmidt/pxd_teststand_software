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
#         Michael Geisinger <geisinger@fortiss.org>
#

xme_add_component(
	"xme_prim_consoleLogger"
	consoleLogger.h consoleLogger.c
)

xme_add_component(
	"xme_prim_digitalOutput"
	digitalOutput.h digitalOutput.c
	xme_hal_dio
)

xme_add_component(
	"xme_prim_ipLoginClientProxy"
	ipLoginClientProxy.h ipLoginClientProxy.c
	xme_hal_net
)

xme_add_component(
	"xme_prim_ipLoginServerProxy"
	ipLoginServerProxy.h ipLoginServerProxy.c
	xme_hal_net
	xme_hal_time
)

xme_add_component(
	"xme_prim_logMsgGenerator"
	logMsgGenerator.h logMsgGenerator.c
)

xme_add_component (
	"xme_prim_modbusMaster"
	modbusMaster.h modbusMaster.c
	PACKAGES Libmodbus
	LIBRARY_PATH VALUEOF LIBMODBUS_LIB_PATH
	INCLUDE_PATH VALUEOF LIBMODBUS_INCLUDE_PATH
	LINK VALUEOF LIBMODBUS_LINK_TARGET
)

xme_add_component(
	"xme_prim_modbusSimulator"
	modbusSimulator.h modbusSimulator.c
)

xme_add_component (
	"xme_prim_randomArrayGenerator"
	randomArrayGenerator.h randomArrayGenerator.c
	xme_hal_random
)

xme_add_component (
	"xme_prim_randomNumberGenerator"
	randomNumberGenerator.h randomNumberGenerator.c
	xme_hal_random
)

xme_add_component (
	"xme_prim_randomStringGenerator"
	randomStringGenerator.h randomStringGenerator.c
	xme_hal_random
)

xme_add_component(
	"xme_prim_topicDump"
	topicDump.h topicDump.c
)

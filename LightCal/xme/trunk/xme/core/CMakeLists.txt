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
#         Michael Geisinger <geisinger@fortiss.org>
#

xme_get(_XME_OPTIONS_INCLUDE_FILE PROPERTY_GLOBAL XME_OPTIONS_INCLUDE_FILE) 

xme_add_component (
	"xme_core_core"
	broker.h broker.c
	component.h componentList.h
	core.h core.c xme_hal_energy xme_hal_math xme_hal_sched xme_hal_sync xme_hal_table xme_hal_tls xme_hal_random
	dataChannel.h
	dcc.h dcc.c xme_hal_sharedPtr
	defines.h
	device.h
	directory.h directory.c
	interface.h interface.c
	interfaceManager.h interfaceManager.c
	log.h
	logger.h logger.c
	mdl.h
	md.h md.c
	node.h
	nodeManager.h nodeManager.c xme_hal_random xme_hal_sharedPtr
	net_plugin.h net_plugin.c xme_hal_net
	net_plugin_tcp.h net_plugin_tcp.c xme_hal_net
	packet.c packet.h
	resourceManager.h resourceManager.c
	routingTable.h routingTable.c
	rr.h rr.c
	topic.h
	${_XME_OPTIONS_INCLUDE_FILE}
)

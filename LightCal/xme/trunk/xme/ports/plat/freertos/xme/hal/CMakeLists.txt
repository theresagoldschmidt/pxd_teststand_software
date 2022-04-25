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


# Retrieve relative path to generic "embedded" implementation
xme_generic_port_path (_GEN_PATH "gen_emb")

xme_add_component(
	"xme_hal_energy"
	energy_arch.c
)

xme_add_component(
	"xme_hal_random"
	${_GEN_PATH}/random_arch.h ${_GEN_PATH}/random_arch.c
)

xme_add_component(
	"xme_hal_net"
	net_arch.h
	net_arch.c
	xme_lwip_api
	xme_lwip_netif_ethernet
	xme_lwip_core_ipv4
	xme_lwip_core
)
xme_add_component(
	"xme_hal_time"
	time_arch.h
	time_arch.c
	xme_freertos_timers
)

xme_add_component(
	"xme_hal_sched"
	sched_arch.c
	xme_hal_table
	xme_freertos_kernel
)

xme_add_component(
	"xme_hal_table"
	${_GEN_PATH}/table_arch.h ${_GEN_PATH}/table_arch.c
	xme_hal_sharedPtr
)

xme_add_component(
	"xme_hal_tls"
	${_GEN_PATH}/tls_arch.c
	"xme_hal_sharedPtr"
)

xme_add_component(
	"xme_hal_uart"
	uart_arch.c
)

xme_add_component(
	"xme_hal_mem"
	mem_arch.c
	mem_arch.h
)


xme_add_component(
	"xme_hal_sharedPtr"
	sharedPtr_arch.c
	sharedPtr_arch.h
)

xme_add_component(
	"xme_hal_sync"
	sync_arch.c
	"xme_hal_table"
)

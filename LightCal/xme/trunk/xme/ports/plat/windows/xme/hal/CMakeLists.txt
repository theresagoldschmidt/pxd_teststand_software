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

# Retrieve relative path to generic OS-based implementation
xme_generic_port_path (_GEN_PATH "gen_os")

xme_add_component(
	"xme_hal_dio"
	dio_arch.h dio_arch.c
)

xme_add_component(
	"xme_hal_energy"
	energy_arch.c
)

xme_add_component(
	"xme_hal_linkedList"
	${_GEN_PATH}/linkedList_arch.h ${_GEN_PATH}/linkedList_arch.c
	"xme_hal_sharedPtr"
)

xme_add_component(
	"xme_hal_net"
	${_GEN_PATH}/net_arch.h ${_GEN_PATH}/net_arch.c
	"xme_hal_sched"
	"xme_hal_sync"
	LINK Ws2_32
)

xme_add_component(
	"xme_hal_random"
	${_GEN_PATH}/random_arch.h ${_GEN_PATH}/random_arch.c
)

xme_add_component(
	"xme_hal_sched"
	sched_arch.c
	"xme_hal_random"
	"xme_hal_tls"
)

xme_add_component(
	"xme_hal_mem"
	${_GEN_PATH}/mem_arch.h ${_GEN_PATH}/mem_arch.c
)


xme_add_component(
	"xme_hal_sharedPtr"
	${_GEN_PATH}/sharedPtr_arch.h ${_GEN_PATH}/sharedPtr_arch.c
)

xme_add_component(
	"xme_hal_sleep"
	sleep_arch.c
)

xme_add_component(
	"xme_hal_sync"
	sync_arch.c
	"xme_hal_table"
)

xme_add_component(
	"xme_hal_table"
	${_GEN_PATH}/table_arch.h ${_GEN_PATH}/table_arch.c
	"xme_hal_math"
	"xme_hal_sharedPtr"
)

xme_add_component(
	"xme_hal_time"
	time_arch.h
	time_arch.c
)

xme_add_component(
	"xme_hal_tls"
	tls_arch.c
)

xme_add_component(
	"xme_hal_uart"
	uart_arch.c
)

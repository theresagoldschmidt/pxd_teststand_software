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
	"xme_hal_dio"
	dio.h
	xme_hal_io
)

xme_add_component(
	"xme_hal_energy"
	energy.h
)

xme_add_component(
	"xme_hal_io"
	io.h
)

xme_add_component(
	"xme_hal_linkedList"
	linkedList.h
)

xme_add_component(
	"xme_hal_math"
	math.h
)

xme_add_component(
	"xme_hal_net"
	net.h
)

xme_add_component(
	"xme_hal_random"
	random.h
)

xme_add_component(
	"xme_hal_sched"
	sched.h
)

xme_add_component(
	"xme_hal_mem"
	mem.h
)

xme_add_component(
	"xme_hal_sharedPtr"
	sharedPtr.h
	xme_hal_mem
)

xme_add_component(
	"xme_hal_sleep"
	sleep.h
)

xme_add_component(
	"xme_hal_sync"
	sync.h
	"xme_hal_table"
)

xme_add_component(
	"xme_hal_table"
	table.h
)

xme_add_component(
	"xme_hal_time"
	time.h
)

xme_add_component(
	"xme_hal_tls"
	tls.h
)

xme_add_component(
	"xme_hal_uart"
	uart.h
)
	
xme_add_component(
	"xme_hal_display"
	display.h
)

xme_add_component(
	"xme_hal_console"
	console.h
)

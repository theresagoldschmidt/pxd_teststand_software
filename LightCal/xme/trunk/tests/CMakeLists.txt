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

# Define main testsuite target
xme_add_executable(
	tests
	tests.h tests.c
	core/test_core.h core/test_core.c
	core/test_dcc.h core/test_dcc.c
	core/test_defines.h core/test_defines.c
	core/test_directory.h core/test_directory.c
	core/test_login.h core/test_login.c
	core/test_md.h core/test_md.c
	core/test_broker.h core/test_broker.c
	core/test_routingTable.h core/test_routingTable.c
	hal/test_linkedList.h hal/test_linkedList.c
	hal/test_math.h hal/test_math.c
	hal/test_net.h hal/test_net.c
	hal/test_random.h hal/test_random.c
	hal/test_shm.h hal/test_shm.c
	hal/test_table.h hal/test_table.c
	hal/test_time.h hal/test_time.c
)

xme_link_components(
	tests
	xme_core_core
	xme_hal_linkedList
	xme_hal_net
	xme_hal_sleep
	xme_hal_sync
	xme_hal_time
)

# Define local network communication destination target
xme_add_executable(
	tests_slave_net
	core/test_core.h core/test_core.c
	hal/test_net_slave.c
)

xme_link_components(
	tests_slave_net
	xme_core_core
	xme_hal_math
	xme_hal_net
	xme_hal_sleep
	xme_hal_sync
)

# Define login destination target
xme_add_executable(
	tests_slave_login
	core/test_core.h core/test_core.c
	core/test_login_slave.c
)

xme_link_components(
	tests_slave_login
	xme_core_core
	xme_hal_math
	xme_hal_net
	xme_hal_sleep
	xme_hal_sync
)

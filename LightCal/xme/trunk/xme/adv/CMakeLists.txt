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

xme_add_component (
	"xme_adv_databaseLogger"
	databaseLogger.h databaseLogger.c
	xme_hal_sharedPtr
	PACKAGES MySQLConnectorC
	LIBRARY_PATH VALUEOF MYSQLCONNECTORC_LIB_PATH
	INCLUDE_PATH VALUEOF MYSQLCONNECTORC_INCLUDE_PATH
	LINK VALUEOF MYSQLCONNECTORC_LINK_TARGET
)

xme_add_component (
	"xme_adv_loginServer"
	loginServer.h loginServer.c
)

xme_add_component (
	"xme_adv_heartbeat"
	heartbeat.h heartbeat.c healthmonitor.h
)

xme_add_component (
	"xme_adv_testHeartbeat"
	testHeartbeat.c testHeartbeat.h healthmonitor.h
	xme_hal_time
)

xme_add_component (
	"xme_adv_testConsistency"
	testConsistency.c testConsistency.h healthmonitor.h
)

xme_add_component (
	"xme_adv_mstLogger"
	mstLogger.h mstLogger.c
)

xme_add_component (
	"xme_adv_mstManager"
	mstManager.h mstManager.c
)

xme_add_component (
	"xme_adv_testCpu"
	testCpu.h testCpu.c healthmonitor.h
)

xme_add_component (
	"xme_adv_testMemory"
	testMemory.h testMemory.c healthmonitor.h
)

xme_add_component (
	"xme_adv_testHeartbeat"
	testHeartbeat.c testHeartbeat.h healthmonitor.h
)

xme_add_component (
	"xme_adv_voter"
	voter.c voter.h healthmonitor.h
)

xme_add_component (
	"xme_adv_healthmonitor"
	healthmonitor.h healthmonitor.c
)

xme_add_component(
	"xme_adv_mstFusion"
	mstFusion.h mstFusion.c
)

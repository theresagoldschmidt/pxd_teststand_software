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
#         Benjamin Wiesmueller <wiesmueller@fortiss.org>
#

xme_add_component (
	"xme_adv_mmtUpdater"
	mmtUpdater.h mmtUpdater.c
)

xme_add_component (
	"xme_adv_mmtUpdaterDummy"
	mmtUpdaterDummy.h mmtUpdaterDummy.c
)

xme_add_component (
	"xme_adv_mmtCommander"
	mmtCommander.h mmtCommander.c
)

xme_add_component (
	"xme_adv_mmtLogger"
	mmtLogger.h mmtLogger.c
)

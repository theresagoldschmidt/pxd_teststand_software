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

# Retrieve relative path to generic OS-based implementation
xme_generic_port_path (_GEN_PATH "gen_os")

xme_add_component(
	"xme_core_core"
	${_GEN_PATH}/defines_arch.h ${_GEN_PATH}/defines_arch.c
	${_GEN_PATH}/log_arch.h
)


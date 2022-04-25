#
# Copyright (c) 2011-2012, fortiss GmbH.
# Licensed under the Apache License, Version 2.0.
# 
# Use, modification and distribution are subject to the terms specified
# in the accompanying license file LICENSE.txt located at the root directory
# of this software distribution. A copy is available at
# http://chromosome.fortiss.org.
#
# This file is part of CHROMOSOME.
#
# $Id$
#
# Author:
#         Kai Huang <khaung@fortiss.org>
#

xme_include_directory()
xme_add_subdirectory(xme)

xme_get(_XME_ARCHITECTURE PROPERTY_GLOBAL XME_ARCHITECTURE)
# xme_add_subdirectory(${_XME_ARCHITECTURE})

# xme_generic_port_path (_GEN_PATH "gen_c/xme/hal" 1)

# FixME: dirty hack to add include and src for math lib
# message("------------- ${XME_SRC_DIR}")
set(_GEN_C_PATH "${XME_SRC_DIR}/ports/arch/gen_c")
# message("------------- ${_GEN_C_PATH}")

xme_include_directory(
	"${_GEN_C_PATH}"
)
xme_add_component(
	"xme_hal_math"
	${_GEN_C_PATH}/xme/hal/math_arch.c 
	${_GEN_C_PATH}/xme/hal/math_arch.h
)

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
#         Michael Geisinger <geisinger@fortiss.org>
#

if (WIN32)
	file (GLOB __HTMLHELPWORKSHOP_DIRS__ "$ENV{PROGRAMFILES}/HTML Help Workshop*")
	foreach (__HTMLHELPWORKSHOP_DIR__ ${__HTMLHELPWORKSHOP_DIRS__})
		if (IS_DIRECTORY ${__HTMLHELPWORKSHOP_DIR__})
			find_path(
				HTMLHELPWORKSHOP_BINARY_PATH
				NAMES "hhc.exe"
				PATHS "${__HTMLHELPWORKSHOP_DIR__}"
			)
		endif (IS_DIRECTORY ${__HTMLHELPWORKSHOP_DIR__})
	endforeach (__HTMLHELPWORKSHOP_DIR__)
else (WIN32)
	set (HTMLHELPWORKSHOP_BINARY_PATH "HTMLHELPWORKSHOP_BINARY_PATH-NOTFOUND")
endif (WIN32)

if (HTMLHELPWORKSHOP_BINARY_PATH)
	set (HTMLHELPWORKSHOP_FOUND TRUE)
	if (WIN32)
		set (HTMLHELPWORKSHOP_BINARY "${HTMLHELPWORKSHOP_BINARY_PATH}/hhc.exe" CACHE FILEPATH "HTML Help Workshop binary")
	else (WIN32)
		message(FATAL "Unexpected state!")
	endif (WIN32)
else (HTMLHELPWORKSHOP_BINARY_PATH)
	set (HTMLHELPWORKSHOP_FOUND FALSE)
	set (HTMLHELPWORKSHOP_BINARY "HTMLHELPWORKSHOP_BINARY-NOTFOUND" CACHE FILEPATH "HTML Help Workshop binary")
endif (HTMLHELPWORKSHOP_BINARY_PATH)

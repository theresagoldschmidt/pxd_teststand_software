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
	file (GLOB __GRAPHVIZ_DIRS__ "$ENV{PROGRAMFILES}/Graphviz*")
	foreach (__GRAPHVIZ_DIR__ ${__GRAPHVIZ_DIRS__})
		if (IS_DIRECTORY ${__GRAPHVIZ_DIR__})
			find_path(
				GRAPHVIZ_BINARY_PATH
				NAMES "dot.exe"
				PATHS "${__GRAPHVIZ_DIR__}"
				PATH_SUFFIXES "bin"
				DOC "Graphviz binary path"
			)
		endif (IS_DIRECTORY ${__GRAPHVIZ_DIR__})
	endforeach (__GRAPHVIZ_DIR__)
else (WIN32)
	find_path(
		GRAPHVIZ_BINARY_PATH
		NAMES "dot"
		PATHS "/usr/local/bin" "/usr/bin"
		DOC "Graphviz binary path"
	)
endif (WIN32)

if (GRAPHVIZ_BINARY_PATH)
	set (GRAPHVIZ_FOUND TRUE)
	if (WIN32)
		set (GRAPHVIZ_BINARY "${GRAPHVIZ_BINARY_PATH}/dot.exe" CACHE FILEPATH "Graphviz binary")
	else (WIN32)
		set (GRAPHVIZ_BINARY "${GRAPHVIZ_BINARY_PATH}/dot" CACHE FILEPATH "Graphviz binary")
	endif (WIN32)
else (GRAPHVIZ_BINARY_PATH)
	set (GRAPHVIZ_FOUND FALSE)
	set (GRAPHVIZ_BINARY "GRAPHVIZ_BINARY-NOTFOUND" CACHE FILEPATH "Graphviz binary")
endif (GRAPHVIZ_BINARY_PATH)

/*
 * Copyright (c) 2011-2012, fortiss GmbH.
 * Licensed under the Apache License, Version 2.0.
 *
 * Use, modification and distribution are subject to the terms specified
 * in the accompanying license file LICENSE.txt located at the root directory
 * of this software distribution. A copy is available at
 * http://chromosome.fortiss.org/.
 *
 * This file is part of CHROMOSOME.
 *
 * $Id$
 */

/**
 * \file
 *         Generic defines (architecture specific part: generic embedded
 *                          implementation).
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_CORE_DEFINES_ARCH_H
#define XME_CORE_DEFINES_ARCH_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/core.h"

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/

// Some of the do/while loops in the following macros will lead to the code
// for recovery and rval to be never executed. However, the compiler will
// perform a syntax check, which is what we are after here. The compiler will
// optimize the code by removing the unreachable statements anyway.

// TODO (See ticket #838): Use this for release mode only, supply reasonable implementation
//                         of assert functionality
// In release mode, all assertions will effectively be NOPs, but we
// still want to let the compiler check the syntax of all parameters.
//#ifdef NDEBUG
#	define XME_ASSERT_NORVAL(condition) do { while (0) { return; } } while (0)
#	define XME_ASSERT_NORVAL_REC(condition, recovery) do { while (0) { do { recovery; } while (0); return; } } while (0)
#	define XME_ASSERT(condition) do { while (0) { return XME_CORE_STATUS_UNEXPECTED; } } while (0)
#	define XME_ASSERT_RVAL(condition, rval) do { while (0) { return rval; } } while (0)
#	define XME_ASSERT_REC(condition, recovery) do { while (0) { do { recovery; } while (0); return XME_CORE_STATUS_UNEXPECTED; } } while (0)
#	define XME_ASSERT_RVAL_REC(condition, rval, recovery) do { while (0) { do { recovery; } while (0); return rval; } } while (0)
//#else // #ifdef NDEBUG

#endif // #ifndef XME_CORE_DEFINES_ARCH_H

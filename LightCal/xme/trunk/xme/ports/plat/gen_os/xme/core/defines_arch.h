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
 *         Logging system abstraction (architecture specific part: generic OS
 *                                     based implementation).
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_CORE_DEFINES_ARCH_H
#define XME_CORE_DEFINES_ARCH_H

#ifndef XME_CORE_DEFINES_H
	#error This architecture-specific header file should not be included directly. Include the generic header file (usually without "_arch" suffix) instead.
#endif // #ifndef XME_CORE_DEFINES_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/core.h"

#include "xme/core/log.h"

#include <assert.h>

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/

/**
 * \def    XME_ASSERT(condition)
 *
 * \brief  Asserts that a condition holds in debug mode and aborts if it does
 *         not.
 *
 *         Typical usage scenarios of this macro include:
 *         \code
 *         xme_core_status_t
 *         func()
 *         {
 *         	void* q;
 *         	void* p = someFunc();
 *         	XME_ASSERT(NULL != p); // will return XME_CORE_STATUS_UNEXPECTED on assertion failure
 *         	XME_ASSERT(NULL != someFunc()); // will trigger runtime warning about side effects
 *         	XME_ASSERT(XME_ASSERT_NO_SIDE_EFFECTS(NULL != someFunc())); // disable side effects warning
 *         	XME_ASSERT(NULL != (q = p)); // will trigger runtime warning about side effects
 *         	XME_ASSERT(XME_ASSERT_NO_SIDE_EFFECTS(NULL != (q = p))); // disable side effects warning
 *         	return XME_CORE_STATUS_SUCCESS;
 *         }
 *         \endcode
 *
 * \note   Since this macro will only have an effect in debug mode (i.e., when
 *         NDEBUG is not set), the condition must not yield any side effects
 *         relevant to non-debug code. To avoid side effects being present in
 *         the condition, a heuristic check has been implemented that detects
 *         the (potential) presence of side effects during runtime and issues
 *         a warning using XME_LOG().
 *         See xme_assert_checkForSideEffects() for details and on how to
 *         prevent the check for intended side effects.
 *
 * \param  condition Condition to assert. See note.
 *
 * \return Returns void if the condition holds. Returns from the current
 *         function with the return value XME_CORE_STATUS_UNEXPECTED if the
 *         condition does not hold. This requires the return type of that
 *         function to be xme_core_status_t. Use the XME_ASSERT_RVAL() or
 *         XME_ASSERT_NORVAL() macros instead if that function has a different
 *         return type or returns void, respectively.
 *         This behavior is used to make the effect of XME_ASSERT consistent on
 *         platforms that do not allow a debugger to break program execution if
 *         the condition does not hold.
 *         In this case, an error check in the code calling the function where
 *         the assertion is triggered can handle the XME_CORE_STATUS_UNEXPECTED
 *         error code.
 */

/**
 * \def    XME_ASSERT_RVAL(condition, rval)
 *
 * \brief  Asserts that a condition holds in debug mode and aborts if it does
 *         not.
 *
 *         Typical usage scenarios of this macro include:
 *         \code
 *         int
 *         func()
 *         {
 *         	void* q;
 *         	void* p = someFunc();
 *         	XME_ASSERT_RVAL(NULL != p, 42); // will return 42 on assertion failure
 *         	XME_ASSERT_RVAL(NULL != someFunc(), 42); // will trigger runtime warning about side effects
 *         	XME_ASSERT_RVAL(XME_ASSERT_NO_SIDE_EFFECTS(NULL != someFunc()), 42); // disable side effects warning
 *         	XME_ASSERT_RVAL(NULL != (q = p), 42); // will trigger runtime warning about side effects
 *         	XME_ASSERT_RVAL(XME_ASSERT_NO_SIDE_EFFECTS(NULL != (q = p)), 42); // disable side effects warning
 *         	return XME_CORE_STATUS_SUCCESS;
 *         }
 *         \endcode
 *
 * \note   Since this macro will only have an effect in debug mode (i.e., when
 *         NDEBUG is not set), the condition must not yield any side effects
 *         relevant to non-debug code. To avoid side effects being present in
 *         the condition, a heuristic check has been implemented that detects
 *         the (potential) presence of side effects during runtime and issues
 *         a warning using XME_LOG().
 *         See xme_assert_checkForSideEffects() for details and on how to
 *         prevent the check for intended side effects.
 *
 * \param  condition Condition to assert. See note.
 * \param  rval Value to return from the function calling this macro in case
 *         the condition does not hold. See return value.
 *
 * \return Returns void if the condition holds. Returns from the current
 *         function with the return value rval if the condition does not hold.
 *         Use the XME_ASSERT() or XME_ASSERT_NORVAL() macros instead if that
 *         function has a return type of xme_core_status_t or returns void,
 *         respectively.
 *         This behavior is used to make the effect of XME_ASSERT consistent on
 *         platforms that do not allow a debugger to break program execution
 *         if the condition does not hold.
 *         In this case, an error check in the code calling the function where
 *         the assertion is triggered can handle the return value.
 */

/**
 * \def    XME_ASSERT_NORVAL(condition)
 *
 * \brief  Asserts that a condition holds in debug mode and aborts if it does
 *         not.
 *
 *         Typical usage scenarios of this macro include:
 *         \code
 *         void
 *         func()
 *         {
 *         	void* q;
 *         	void* p = someFunc();
 *         	XME_ASSERT_NORVAL(NULL != p); // will return on assertion failure
 *         	XME_ASSERT_NORVAL(NULL != someFunc()); // will trigger runtime warning about side effects
 *         	XME_ASSERT_NORVAL(XME_ASSERT_NO_SIDE_EFFECTS(NULL != someFunc())); // disable side effects warning
 *         	XME_ASSERT_NORVAL(NULL != (q = p)); // will trigger runtime warning about side effects
 *         	XME_ASSERT_NORVAL(XME_ASSERT_NO_SIDE_EFFECTS(NULL != (q = p))); // disable side effects warning
 *         	return XME_CORE_STATUS_SUCCESS;
 *         }
 *         \endcode
 *
 * \note   Since this macro will only have an effect in debug mode (i.e., when
 *         NDEBUG is not set), the condition must not yield any side effects
 *         relevant to non-debug code. To avoid side effects being present in
 *         the condition, a heuristic check has been implemented that detects
 *         the (potential) presence of side effects during runtime and issues
 *         a warning using XME_LOG().
 *         See xme_assert_checkForSideEffects() for details and on how to
 *         prevent the check for intended side effects.
 *
 * \param  condition Condition to assert. See note.
 *
 * \return Returns void if the condition holds. Returns from the current
 *         function with a void return value if the condition does not hold.
 *         Use the XME_ASSERT() or XME_ASSERT_RVAL() macros instead if that
 *         function has a return type of xme_core_status_t or an arbitrary
 *         other return type, respectively.
 *         This behavior is used to make the effect of assertions consistent on
 *         platforms that do not allow a debugger to break program execution
 *         if the condition does not hold.
 */

// Some of the do/while loops in the following macros will lead to the code
// for recovery and rval to be never executed. However, the compiler will
// perform a syntax check, which is what we are after here. The compiler will
// optimize the code by removing the unreachable statements anyway.
#ifdef NDEBUG
	// In release mode, all assertions will effectively be NOPs, but we
	// still want to let the compiler check the syntax of all parameters.
#	define XME_ASSERT(condition) do { while (0) { return XME_CORE_STATUS_UNEXPECTED; } } while (0)
#	define XME_ASSERT_RVAL(condition, rval) do { while (0) { return rval; } } while (0)
#	define XME_ASSERT_NORVAL(condition) do { while (0) { return; } } while (0)
#else // #ifdef NDEBUG
	// In debug mode, assertions will return from the current function
	// with status XME_CORE_STATUS_UNEXPECTED after cleaning up.
#	define XME_ASSERT(condition) do { xme_assert_checkForSideEffects("XME_ASSERT", #condition, __FILE__, __LINE__, 1); assert(condition); break; return XME_CORE_STATUS_UNEXPECTED; } while (0)
#	define XME_ASSERT_RVAL(condition, rval) do { xme_assert_checkForSideEffects("XME_ASSERT_RVAL", #condition, __FILE__, __LINE__, 1); assert(condition); break; return rval; } while (0)
#	define XME_ASSERT_NORVAL(condition) do { xme_assert_checkForSideEffects("XME_ASSERT_NORVAL", #condition, __FILE__, __LINE__, 1); assert(condition); break; return; } while (0)
#	define XME_ASSERT_NO_SIDE_EFFECTS(condition) (1 && (condition))
#endif // #ifdef NDEBUG

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
#ifndef NDEBUG
/**
 * \brief  Uses a heuristic to check for side effects within conditions of
 *         assertion macros.
 *
 *         It is dangerous for conditions of assertion macros like XME_ASSERT(),
 *         XME_ASSERT_RVAL() and XME_ASSERT_NORVAL() to contain side effects,
 *         because those side effects will not be present in release builds,
 *         where those macros are effectively NOPs.
 *
 *         The heuristic detects the following situations as statements with
 *         possible side effects:
 *          - A C identifier that is followed by an opening bracket (i.e., a
 *            function or macro call).
 *          - An assignment operator ('=', '+=', '-=', '*=', '/=', '%='),
 *            except for '=='.
 *          - An increment or decrement operator ('++' or '--', prefix or
 *            postfix).
 *         In case a side effect is detected, a nonzero value is returned and
 *         a warning may be issued depending on the emitWarning parameter.
 *
 *         Note that this heuristic does not cover all situations where side
 *         effects might occur, and it might yield false positives.
 *         See notes on how to acknowledge false positives.
 *
 * \note   To prevent detection of side effects for a specific condition (for
 *         example in case of known false positives), make sure that the
 *         condition is enclosed in XME_ASSERT_NO_SIDE_EFFECTS().
 *
 * \todo   This is a runtime check for side effects.
 *         Of course it would be better to use static code analysis to detect
 *         such a circumstance. Until we have such mechanisms in place, this
 *         is a quick way to avoid common problems with assertion macros.
 *
 * \param  macro Name of the macro that called this function (for debug message
 *         output).
 * \param  condition String representation of the condition to check for
 *         potential side effects.
 * \param  file Name of the file that contains the assertion macro (for debug
 *         message output).
 * \param  line Line within the file that contains the assertion macro (for
 *         debug message output).
 * \param  emitWarning If set to a nonzero value, the function will emit a
 *         warning if condition contains statements with potential side effects.
 *
 * \return Returns zero if no side effects could be detected in condition (as
 *         defined above) or condition contains the XME_ASSERT_NO_SIDE_EFFECTS()
 *         macro to mask potential side effects and a nonzero value otherwise.
 *
 * \see    XME_ASSERT(), XME_ASSERT_RVAL(), XME_ASSERT_NORVAL(),
 *         XME_ASSERT_NO_SIDE_EFFECTS()
 */
int
xme_assert_checkForSideEffects(const char* macro, const char* condition, const char* file, const unsigned long line, int emitWarning);
#endif // #ifndef NDEBUG

#endif // #ifndef XME_CORE_DEFINES_ARCH_H


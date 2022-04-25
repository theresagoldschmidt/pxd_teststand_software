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
 *         Public scheduler API.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_HAL_SCHED_H
#define XME_HAL_SCHED_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/core/core.h"

#include "xme/hal/table.h"
#include "xme/hal/time.h"

#include <stdint.h>

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
/**
 * \def XME_HAL_SCHED_TASK_INITIALLY_SUSPENDED
 *
 * \brief Used in conjunction with xme_hal_sched_addTask() to indicate that
 *        a task should be created suspended.
 */
#define XME_HAL_SCHED_TASK_INITIALLY_SUSPENDED 0xFFFF

// TODO: Define more priorities. See ticket #804
// TODOC. See ticket #722
#ifdef XME_PLATFORM_FREERTOS
#define XME_HAL_SCHED_PRIORITY_NORMAL 2
#else
#define XME_HAL_SCHED_PRIORITY_NORMAL 127
#endif

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
/**
 * \typedef xme_hal_sched_taskHandle_t
 *
 * \brief Task handle.
 */
typedef enum
{
	XME_HAL_SCHED_INVALID_TASK_HANDLE = 0, ///< Invalid task handle.
	XME_HAL_SCHED_MAX_TASK_HANDLE = XME_HAL_TABLE_MAX_ROW_HANDLE ///< Largest possible task handle.
}
xme_hal_sched_taskHandle_t;

/**
 * \typedef xme_hal_sched_taskState_t
 *
 * \brief Task state.
 */
typedef enum
{
	XME_HAL_SCHED_TASK_STATE_RUNNING = 0, ///< Task is currently running. Note that this does not imply when the task will be executed, it just means that the task is scheduled for execution.
	XME_HAL_SCHED_TASK_STATE_SUSPENDED = 1, ///< Task is suspended.
	XME_HAL_SCHED_TASK_STATE_TERMINATING = 255 ///< Task is about to be removed. Since tasks can only be removed when they are not currently executed, this flag indicates that the task will be removed at the next possible point in time after it returns control to the runtime system.
}
xme_hal_sched_taskState_t;

/**
 * \typedef xme_hal_sched_taskCallback_t
 *
 * \brief  Task callback function.
 */
typedef void (*xme_hal_sched_taskCallback_t) (void* userData);

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief  Initializes the scheduler.
 */
xme_core_status_t
xme_hal_sched_init();

/**
 * \brief  Schedules a task for execution.
 *
 * \note   On some platforms, a task's callback function might be run from
 *         a different execution context (e.g., a different thread), so
 *         synchronization mechanisms might be required if the task shares
 *         data with other tasks or the main program.
 *
 * \param  startMs If non-zero, specifies the time in milliseconds from now
 *         when the task should be executed. A value of zero will cause the
 *         task to be started as soon as possible. A value of
 *         XME_HAL_SCHED_TASK_INITIALLY_SUSPENDED will cause the task to stay
 *         suspended until it is resumed using
 *         xme_hal_sched_setTaskExecutionState().
 * \param  periodMs If non-zero, specifies the period in milliseconds of the
 *         task. Otherwise, the task is a one-shot task and will be
 *         automatically removed after execution.
 * \param  priority Task priority (only meaningful for priority-based
 *         scheduler implementations). Higher values mean a higher priority.
 * \param  callback Callback function implementing the task to be executed.
 *         Note that on some platforms, this callback function might be run
 *         from a different execution context (e.g., a different thread),
 *         so synchronization mechanisms might be required if the task
 *         shares data with other tasks or the main program.
 * \param  userData User-defined data to be passed to the callback function.
 */
xme_hal_sched_taskHandle_t
xme_hal_sched_addTask
(
	xme_hal_time_interval_t startMs,
	xme_hal_time_interval_t periodMs,
	uint8_t priority,
	xme_hal_sched_taskCallback_t callback,
	void* userData
);

/**
 * \brief  Sets task exection state of the task with the given handle.
 *
 *         If the task is currently being executed, its execution state will
 *         be updated at the next possible point in time after finishing
 *         execution. The function will block until the new state has been
 *         enforced.
 *
 * \note   The attempt to suspend or resume a task that is scheduled for
 *         termination (i.e., xme_hal_sched_removeTask() has been called
 *         on the task) will fail with a status code of
 *         XME_CORE_STATUS_PERMISSION_DENIED.
 *
 * \param  taskHandle Handle of the task to set execution state of.
 * \param  running Flag indicating whether the task should currently be
 *         executed. A value of true will resume the task, a value of false
 *         will suspend it.
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the task's execution state has
 *            been successfully updated.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the task given handle was
 *            invalid.
 *          - XME_CORE_STATUS_INVALID_CONFIGURATION if the task is
 *            scheduled for termination and was already running.
 *            Suspending a task scheduled for termination is not
 *            supported.
 *          - XME_CORE_STATUS_PERMISSION_DENIED if the task is scheduled
 *            for termination. It is not allowed to suspend or resume a
 *            thread waiting for termination.
 */
xme_core_status_t
xme_hal_sched_setTaskExecutionState(xme_hal_sched_taskHandle_t taskHandle, bool running);

/**
 * \brief  Removes a task from the schedule.
 *
 *         If the task is currently being executed, it is removed at the next
 *         possible point in time after finishing execution. If this function
 *         is called from the execution context of the respective task, the
 *         task is automatically removed after it returns control to the
 *         runtime system. If this function is called from a different
 *         execution context, the function will only return after the task has
 *         been successfully removed. This means that it is guaranteed that
 *         the task will not use any shared resources after this function
 *         returned. This function will also remove the task if it is
 *         currently suspended, so resuming the task is not required
 *         beforehand if it might be suspended.
 *
 * \note   If the respective task executes an infinite loop, a call to this
 *         function from a different execution context than that of the
 *         respective task will never return!
 *
 * \param  taskHandle Handle of the task to remove.
 * \return Returns one of the following status codes:
 *          - XME_CORE_STATUS_SUCCESS if the task's execution state has
 *            been successfully updated.
 *          - XME_CORE_STATUS_INVALID_HANDLE if the task given handle was
 *            invalid.
 */
xme_core_status_t
xme_hal_sched_removeTask(xme_hal_sched_taskHandle_t taskHandle);

/**
 * \brief  Frees resources occupied by the scheduler.
 */
void
xme_hal_sched_fini();

#endif // #ifndef XME_HAL_SCHED_H

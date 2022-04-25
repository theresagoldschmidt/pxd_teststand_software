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
 *         Scheduler abstraction (architecture specific part: FreeRTOS).
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 *         Hauke Staehle <staehle@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/sched.h"

#include "xme/core/component.h"
#include "xme/core/defines.h"
#include "xme/hal/table.h"

// FreeRTOS includes
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include <stdint.h>

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
typedef struct
{
	xTaskHandle taskRTOSHandle; ///< Handle to the task. This is a FreeRTOS-specific handle.
	uint8_t taskState; ///< Execution state of the task (running, suspended, terminating).
	uint16_t startMs; ///< Start time of the task in milliseconds. If zero, the task is started immediately.
	uint16_t periodMs; ///< Period of task execution in milliseconds. If zero, the task is only executed once.
	uint8_t priority; ///< Priority of the task. Higher values denote a higher priority.
	xme_hal_sched_taskCallback_t callback; ///< Task callback function.
	xme_core_component_t componentContext; ///< Context of the component this task belongs to.
	void* userData; ///< User data to pass to task callback function.
	bool selfCleanup; ///< Whether the thread itself should clean up its task record upon destruction. This is the case if the thread of the task itself called the removeTask() function, in which case the runtime system must not clean up the resources until the thread has actually terminated.
}
xme_hal_sched_taskDescriptor_t;

typedef struct
{
	//private
	xSemaphoreHandle taskDescriptorsMutex;	///< Mutex for shared access to the task list.
	XME_HAL_TABLE(xme_hal_sched_taskDescriptor_t, taskDescriptors, XME_HAL_DEFINES_MAX_TASK_DESCRIPTORS); // taskHandle is an index into this table
}
xme_hal_sched_configStruct_t;

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
static xme_hal_sched_configStruct_t xme_hal_sched_config;

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
void
xme_hal_sched_taskWrapper(void* _taskHandle);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
xme_core_status_t
xme_hal_sched_init()
{
	XME_HAL_TABLE_INIT(xme_hal_sched_config.taskDescriptors);

	xme_hal_sched_config.taskDescriptorsMutex = xSemaphoreCreateMutex();

	XME_CHECK
	(
		NULL != xme_hal_sched_config.taskDescriptorsMutex,
		XME_CORE_STATUS_INTERNAL_ERROR
	);

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_hal_sched_taskWrapper(void* _taskHandle)
{
	xme_hal_sched_taskHandle_t taskHandle = (xme_hal_sched_taskHandle_t)_taskHandle;
	xme_hal_sched_taskDescriptor_t* taskDesc;
	portTickType initialDelayTickCount;
	portTickType periodicDelayTickCount;
	xTaskHandle taskRTOSHandle;

	taskDesc = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_sched_config.taskDescriptors, taskHandle);

	initialDelayTickCount = taskDesc->startMs / portTICK_RATE_MS;
	periodicDelayTickCount = taskDesc->periodMs / portTICK_RATE_MS;

	taskRTOSHandle = taskDesc->taskRTOSHandle;

	vTaskDelay( initialDelayTickCount );

	while (1)
	{
			// Check for task removal
			if (XME_HAL_SCHED_TASK_STATE_TERMINATING == taskDesc->taskState)
			{
				break;
			}

			// Switch to the context of the component and run the task
			XME_COMPONENT_CONTEXT
			(
				taskDesc->componentContext,
				{
						taskDesc->callback(taskDesc->userData);
				}
			);

			// Stop looping if this is a non-periodic task
			if ( periodicDelayTickCount == 0 )
			{
				taskDesc->selfCleanup = true;
				break;
			}

			// Delay the task until next execution. Delays are relative to the current time,
			// that has to be changed.
			vTaskDelay( periodicDelayTickCount );
	}

	if (taskDesc->selfCleanup)
	{
		// Remove from XME
		XME_HAL_TABLE_REMOVE_ITEM(xme_hal_sched_config.taskDescriptors, taskHandle);
	}

	// It is allowed in FreeRTOS to delete the current task
	vTaskDelete( taskRTOSHandle );

	// Should never reach this point
	for (;;);
}

xme_hal_sched_taskHandle_t
xme_hal_sched_addTask(xme_hal_time_interval_t startMs, xme_hal_time_interval_t periodMs, uint8_t priority, xme_hal_sched_taskCallback_t callback, void* userData)
{
	xme_hal_sched_taskDescriptor_t* taskDesc;
	xme_core_component_t currentComponentId;
	xme_hal_sched_taskHandle_t newTaskHandle;
	portBASE_TYPE res;

	XME_CHECK
	(
		NULL != callback,
		XME_CORE_STATUS_INVALID_PARAMETER
	);

	XME_CHECK
	(
			xSemaphoreTake( xme_hal_sched_config.taskDescriptorsMutex, portMAX_DELAY ) == pdTRUE,
			XME_CORE_STATUS_INTERNAL_ERROR
	);

	// Allocate a unique task handle
	newTaskHandle = XME_HAL_TABLE_ADD_ITEM(xme_hal_sched_config.taskDescriptors);

	// Store the task profile
	taskDesc = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_sched_config.taskDescriptors, newTaskHandle);

	taskDesc->taskState = (XME_HAL_SCHED_TASK_INITIALLY_SUSPENDED == startMs) ? XME_HAL_SCHED_TASK_STATE_SUSPENDED : XME_HAL_SCHED_TASK_STATE_RUNNING;
	taskDesc->startMs = (XME_HAL_SCHED_TASK_INITIALLY_SUSPENDED == startMs) ? 0 : startMs;
	taskDesc->periodMs = periodMs;
	taskDesc->priority = priority;
	taskDesc->callback = callback;
	taskDesc->componentContext = XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT; // xme_core_resourceManager_getCurrentComponentId(); // TODO: See ticket #821
	taskDesc->userData = userData;
	taskDesc->selfCleanup = false;

	res = xTaskCreate
	(
			xme_hal_sched_taskWrapper,
			( signed char * ) "Task",
			configMINIMAL_STACK_SIZE,
			( void * )newTaskHandle,
			taskDesc->priority,
			&taskDesc->taskRTOSHandle
	);

	if ( res != pdPASS )
	{
		XME_HAL_TABLE_REMOVE_ITEM(xme_hal_sched_config.taskDescriptors, newTaskHandle);
		xSemaphoreGive( xme_hal_sched_config.taskDescriptorsMutex );

		return XME_CORE_STATUS_INTERNAL_ERROR;
	}

	xSemaphoreGive( xme_hal_sched_config.taskDescriptorsMutex );

	// Return the unique task handle
	return newTaskHandle;
}

xme_core_status_t
xme_hal_sched_setTaskExecutionState(xme_hal_sched_taskHandle_t taskHandle, bool running)
{
	xme_hal_sched_taskDescriptor_t* taskDesc;

	// Synchronize access to the task descriptors mutex
	XME_CHECK
	(
			xSemaphoreTake( xme_hal_sched_config.taskDescriptorsMutex, portMAX_DELAY ) == pdTRUE,
			XME_CORE_STATUS_INTERNAL_ERROR
	);

	// Verify the task handle
	XME_CHECK_REC
	(
		NULL != (taskDesc = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_sched_config.taskDescriptors, taskHandle)),
		XME_CORE_STATUS_INVALID_HANDLE,
		{
				xSemaphoreGive(&xme_hal_sched_config.taskDescriptorsMutex);
		}
	);

	// TODO: Check this. See ticket #820
	// Check whether a state change is necessary
	if (!running ^ !!(taskDesc->taskState & XME_HAL_SCHED_TASK_STATE_SUSPENDED))
	{
		// Update suspended flag
		if (running)
		{
			taskDesc->taskState &= ~XME_HAL_SCHED_TASK_STATE_SUSPENDED;
			vTaskResume ( taskDesc->taskRTOSHandle );
		}
		else
		{
			taskDesc->taskState |= XME_HAL_SCHED_TASK_STATE_SUSPENDED;
			vTaskSuspend( taskDesc->taskRTOSHandle );
		}
	}

	xSemaphoreGive( xme_hal_sched_config.taskDescriptorsMutex );

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_hal_sched_removeTask(xme_hal_sched_taskHandle_t taskHandle)
{
	xme_hal_sched_taskDescriptor_t* taskDesc;
	bool selfCleanup = false;

	XME_CHECK
	(
			xSemaphoreTake( xme_hal_sched_config.taskDescriptorsMutex, portMAX_DELAY ) != pdTRUE,
			XME_CORE_STATUS_INTERNAL_ERROR
	);

	// Verify the task handle
	XME_CHECK_REC
	(
		NULL != (taskDesc = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_sched_config.taskDescriptors, taskHandle)),
		XME_CORE_STATUS_INVALID_HANDLE,
		{
				xSemaphoreGive( xme_hal_sched_config.taskDescriptorsMutex );
		}
	);

	// Schedule the task for termination (this will resume the task)
	taskDesc->taskState = XME_HAL_SCHED_TASK_STATE_TERMINATING;

	// If this function has been called from the context of the thread
	// to be removed, we set the selfCleanup flag and can safely
	// continue, as it is guaranteed that the thread will check the
	// task state before subsequently executing the callback function.
	if (xTaskGetCurrentTaskHandle() == taskDesc->taskRTOSHandle)
	{
		taskDesc->selfCleanup = selfCleanup = true;
	}
	else
	{
		vTaskDelete ( taskDesc->taskRTOSHandle );
		XME_HAL_TABLE_REMOVE_ITEM(xme_hal_sched_config.taskDescriptors, taskHandle);
	}

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_hal_sched_fini()
{
	// Synchronize access to the task descriptors mutex
	// TODO: What on error? See ticket #721
	xSemaphoreTake( xme_hal_sched_config.taskDescriptorsMutex, portMAX_DELAY );

	// Terminate all tasks
	XME_HAL_TABLE_ITERATE
	(
		xme_hal_sched_config.taskDescriptors,
		xme_hal_sched_taskHandle_t, taskHandle,
		xme_hal_sched_taskDescriptor_t, taskDesc,
		{
			xme_hal_sched_removeTask(taskHandle);
		}
	);

	xSemaphoreGive( xme_hal_sched_config.taskDescriptorsMutex );

	// Destroy the task descriptors mutex
	vQueueDelete( xme_hal_sched_config.taskDescriptorsMutex );

	XME_HAL_TABLE_FINI(xme_hal_sched_config.taskDescriptors);
}

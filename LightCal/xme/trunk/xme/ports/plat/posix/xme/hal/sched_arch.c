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
 *         Scheduler abstraction (architecture specific part: Posix).
 *
 * \author
 *         Deniz Kabakci <deniz.kabakci@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/sched.h"

#include "xme/core/component.h"
#include "xme/core/defines.h"
#include "xme/core/resourceManager.h"

#include "xme/hal/table.h"
#include "xme/hal/time.h"
#include "xme/hal/tls.h"
#include "xme/hal/random.h"

#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/syscall.h> 
#include <sys/resource.h> 
#include <sys/time.h>
#include <sched.h>

#define linux_posix  1

#ifndef linux_posix
#define generic_posix 1
#endif

#if linux_posix
#define sigev_notify_thread_id _sigev_un._tid ///< Signal the specific thread (SIGEV_THREAD_ID (Linux-specific) option cannot be called directly)
#define gettid() syscall(__NR_gettid) ///< Signal the specific thread (in order to get running thread`s id)
#endif

/******************************************************************************/
/***   Type definitions                                                     ***/
/******************************************************************************/
typedef struct
{
	pthread_mutex_t threadMutex; ///< Mutex for shared access to the thread that represents this task.
	pthread_cond_t stateChangedEvent; ///< Signal sent to the thread to indicate a task state change.
	timer_t taskPeriodTimer; ///< Waitable timer for task period. //Contains the ID of the created timer
	struct sigevent signal_timer; ///<timer event.
	sigset_t waitset;
	int sig_no;
	pthread_t threadHandle; ///< Handle of the thread that represents the task.
#if generic_posix
	int threadId; ///< Identifier of the thread that represents the task.
#endif
	xme_hal_sched_taskState_t taskState; ///< Execution state of the task (running, suspended, terminating).
	xme_hal_time_interval_t startMs; ///< Start time of the task in milliseconds. If zero, the task is started immediately.
	xme_hal_time_interval_t periodMs; ///< Period of task execution in milliseconds. If zero, the task is only executed once.
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
	pthread_mutex_t taskDescriptorsMutex; ///< Mutex for shared access to the task list.
	XME_HAL_TABLE(xme_hal_sched_taskDescriptor_t, taskDescriptors, XME_HAL_DEFINES_MAX_TASK_DESCRIPTORS); ///<taskHandle is an index into this table
}
xme_hal_sched_configStruct_t;

/******************************************************************************/
/***   Static variables                                                     ***/
/******************************************************************************/
static xme_hal_sched_configStruct_t xme_hal_sched_config;

/******************************************************************************/
/***   Protoypes                                                            ***/
/******************************************************************************/
void* xme_hal_sched_taskProc(void* arg);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/

xme_core_status_t
xme_hal_sched_init()
{
	pthread_mutexattr_t taskDescriptorsMutexAttr; ///<Mutex attribute for recursive mutex implementation.

	XME_HAL_TABLE_INIT(xme_hal_sched_config.taskDescriptors);

	XME_CHECK
	(
		0 == pthread_mutexattr_init(&taskDescriptorsMutexAttr),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	XME_CHECK
	(
		0 == pthread_mutexattr_settype(&taskDescriptorsMutexAttr, PTHREAD_MUTEX_RECURSIVE),
		ENOTSUP
	);

	XME_CHECK
	(
		0 == pthread_mutex_init(&xme_hal_sched_config.taskDescriptorsMutex, &taskDescriptorsMutexAttr),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);

	XME_CHECK
	(
		0 == pthread_mutexattr_destroy(&taskDescriptorsMutexAttr),
		ENOTSUP
	);

	return XME_CORE_STATUS_SUCCESS;
}

xme_hal_sched_taskHandle_t
xme_hal_sched_addTask
(
	xme_hal_time_interval_t startMs,
	xme_hal_time_interval_t periodMs,
	uint8_t priority,
	xme_hal_sched_taskCallback_t callback,
	void* userData
)
{
	xme_hal_sched_taskHandle_t newTaskHandle;
	xme_hal_sched_taskDescriptor_t* taskDesc;

	//struct sched_param sp; ///<Initial priority assignment data structure for thread creation.
	pthread_mutexattr_t threadMutexAttr; ///<Mutex attribute for recursive mutex implementation.
	pthread_attr_t threadAttr; ///< Attributes of the thread to set scheduling.
#if generic_posix
	static int count=1;	// assign threadId in every thread creation based on a counter if necessary.
#endif
	XME_CHECK
	(
		NULL != callback,
		XME_HAL_SCHED_INVALID_TASK_HANDLE
	);
	
	// Synchronize access to the task descriptors mutex
	pthread_mutex_lock(&xme_hal_sched_config.taskDescriptorsMutex);
	{
		// Allocate a unique task handle
		newTaskHandle = (xme_hal_sched_taskHandle_t)XME_HAL_TABLE_ADD_ITEM(xme_hal_sched_config.taskDescriptors);

		XME_CHECK_REC
		(
			XME_HAL_SCHED_INVALID_TASK_HANDLE != newTaskHandle,
			XME_HAL_SCHED_INVALID_TASK_HANDLE,
			{
				pthread_mutex_unlock(&xme_hal_sched_config.taskDescriptorsMutex);
			}
		);

		// Store the task profile
		taskDesc = (xme_hal_sched_taskDescriptor_t*)XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_sched_config.taskDescriptors, newTaskHandle);

		XME_ASSERT_RVAL(NULL != taskDesc, XME_HAL_SCHED_INVALID_TASK_HANDLE);

		XME_CHECK_REC
		(
			0 == pthread_mutexattr_init(&threadMutexAttr),
			XME_CORE_STATUS_OUT_OF_RESOURCES,
			{
				XME_HAL_TABLE_REMOVE_ITEM(xme_hal_sched_config.taskDescriptors, (xme_hal_table_rowHandle_t)newTaskHandle);
				pthread_mutexattr_destroy(&threadMutexAttr);
				pthread_mutex_unlock(&xme_hal_sched_config.taskDescriptorsMutex);
			}
		);

		XME_CHECK_REC
		(
			0 == pthread_mutexattr_settype(&threadMutexAttr, PTHREAD_MUTEX_RECURSIVE),
			ENOTSUP,
			{
				XME_HAL_TABLE_REMOVE_ITEM(xme_hal_sched_config.taskDescriptors, (xme_hal_table_rowHandle_t)newTaskHandle);
				pthread_mutexattr_destroy(&threadMutexAttr);
				pthread_mutex_unlock(&xme_hal_sched_config.taskDescriptorsMutex);
			}
		);		
		
		XME_CHECK_REC
		(
			0 == pthread_mutex_init(&taskDesc->threadMutex,&threadMutexAttr),
			XME_HAL_SCHED_INVALID_TASK_HANDLE,
			{
				XME_HAL_TABLE_REMOVE_ITEM(xme_hal_sched_config.taskDescriptors, (xme_hal_table_rowHandle_t)newTaskHandle);
				pthread_mutexattr_destroy(&threadMutexAttr);
				pthread_mutex_destroy(&taskDesc->threadMutex);
				pthread_mutex_unlock(&xme_hal_sched_config.taskDescriptorsMutex);
			}
		);

		XME_CHECK
		(
			0 == pthread_mutexattr_destroy(&threadMutexAttr),
			ENOTSUP
		);

		XME_CHECK_REC
		(
			0 == (pthread_cond_init(&taskDesc->stateChangedEvent,NULL)),		//TODO:State mechanism (suspend-run-terminate) works properly or not?
			XME_HAL_SCHED_INVALID_TASK_HANDLE,
			{
				pthread_cond_destroy(&taskDesc->stateChangedEvent);
				pthread_mutex_destroy(&taskDesc->threadMutex);
				XME_HAL_TABLE_REMOVE_ITEM(xme_hal_sched_config.taskDescriptors, (xme_hal_table_rowHandle_t)newTaskHandle);
				pthread_mutex_unlock(&xme_hal_sched_config.taskDescriptorsMutex);
			}
		);

		// If the task is to be executed periodically, initialize a timer
		// that will only signal the corresponding thread when each period starts		
#if linux_posix
		taskDesc->signal_timer.sigev_notify=SIGEV_THREAD_ID; ///<notification mechanism
		taskDesc->signal_timer.sigev_signo=SIGALRM; ///<signal number,(SIGALRM=14)
#else
		/*if it is not linux posix implementation, each task must be notified by different signal */
		taskDesc->signal_timer.sigev_notify=SIGEV_SIGNAL; ///<notification mechanism 
		taskDesc->signal_timer.sigev_signo=SIGRTMIN+count; 
		taskDesc->signal_timer.sigev_value.sival_int=count; ///< signal data value // create unique numbers for each thread..
#endif
		taskDesc->threadHandle = 0; 
#if generic_posix
		taskDesc->threadId = count; 
		count++; 
#endif
		taskDesc->taskState = (XME_HAL_SCHED_TASK_INITIALLY_SUSPENDED == startMs) ? XME_HAL_SCHED_TASK_STATE_SUSPENDED : XME_HAL_SCHED_TASK_STATE_RUNNING;
		taskDesc->startMs = (XME_HAL_SCHED_TASK_INITIALLY_SUSPENDED == startMs) ? 0 : startMs;
		taskDesc->periodMs = periodMs;
		taskDesc->priority = priority;
		taskDesc->callback = callback;
		taskDesc->componentContext = xme_core_resourceManager_getCurrentComponentId(); // XME_CORE_COMPONENT_INVALID_COMPONENT_CONTEXT;
		taskDesc->userData = userData;
		taskDesc->selfCleanup = false; 	

#if generic_posix
			XME_CHECK_REC
			( 
				0 == timer_create(CLOCK_REALTIME,&(taskDesc->signal_timer), &(taskDesc->taskPeriodTimer)), // The new timer is initially disarmed.
				XME_HAL_SCHED_INVALID_TASK_HANDLE,
				{	
					pthread_cond_destroy(&taskDesc->stateChangedEvent);
					pthread_mutex_destroy(&taskDesc->threadMutex);
					XME_HAL_TABLE_REMOVE_ITEM(xme_hal_sched_config.taskDescriptors, (xme_hal_table_rowHandle_t)newTaskHandle);
					pthread_mutex_unlock(&xme_hal_sched_config.taskDescriptorsMutex);
				}
			);
#endif
		XME_CHECK
		(
			0 == pthread_attr_init(&threadAttr),
			XME_CORE_STATUS_OUT_OF_RESOURCES
		);

		//SetThreadPriority // TODO: See ticket #813  //requires superuser privileges
#if 0
		sp.sched_priority=taskDesc->priority;

		XME_CHECK
		(
			0 == pthread_attr_setinheritsched(&threadAttr, PTHREAD_EXPLICIT_SCHED),
			ENOTSUP
		);

		XME_CHECK
		(
			0 == pthread_attr_setschedpolicy(&threadAttr, SCHED_RR),  // Rt scheduling : SCHED_RR or SCHED_FIFO
			ENOTSUP
		);

		XME_CHECK
		(
			0 == pthread_attr_setschedparam(&threadAttr, &sp),
			ENOTSUP
		);

#endif

		// The last statement of this function related to the thread descriptor is
		// the assignment of the threadHandle member. However, the thread might
		// already access that member before the assignment is complete.
		// Hence, we have to lock the thread mutex here as well.

		pthread_mutex_lock(&taskDesc->threadMutex);
		{
			// Create a thread to serve the task
			XME_CHECK_REC			
			(	
				0 == (pthread_create(&(taskDesc->threadHandle), &threadAttr, xme_hal_sched_taskProc, (void *)newTaskHandle)),
				XME_HAL_SCHED_INVALID_TASK_HANDLE,
				{
#if generic_posix	
					timer_delete(taskDesc->taskPeriodTimer);
					count--; 
#endif
					pthread_cond_destroy(&taskDesc->stateChangedEvent);
					pthread_mutex_unlock(&taskDesc->threadMutex);
					pthread_mutex_destroy(&taskDesc->threadMutex);	
					pthread_attr_destroy(&threadAttr);				
					XME_HAL_TABLE_REMOVE_ITEM(xme_hal_sched_config.taskDescriptors, (xme_hal_table_rowHandle_t)newTaskHandle);
					pthread_mutex_unlock(&xme_hal_sched_config.taskDescriptorsMutex);
				}
			);

			XME_CHECK
			(
				0 == pthread_attr_destroy(&threadAttr),
				ENOTSUP
			);

		}
		pthread_mutex_unlock(&taskDesc->threadMutex);
	}
	pthread_mutex_unlock(&xme_hal_sched_config.taskDescriptorsMutex);

	// Return the unique task handle
	return newTaskHandle;
}

xme_core_status_t
xme_hal_sched_setTaskExecutionState
(
	xme_hal_sched_taskHandle_t taskHandle,
	bool running
)
{
	xme_hal_sched_taskDescriptor_t* taskDesc;

	// Synchronize access to the task descriptors mutex
	pthread_mutex_lock(&xme_hal_sched_config.taskDescriptorsMutex);
	{	
		// Verify the task handle
		XME_CHECK_REC
		(
			0 != (taskDesc = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_sched_config.taskDescriptors, taskHandle)),
			XME_CORE_STATUS_INVALID_HANDLE,
			{
				pthread_mutex_unlock(&xme_hal_sched_config.taskDescriptorsMutex);
			}
		);

		// Synchronize access to the thread mutex
		pthread_mutex_lock(&taskDesc->threadMutex);
		{
			XME_CHECK_REC
			(
#if linux_posix
				0 != taskDesc->threadHandle, 
#else
				(0 != taskDesc->threadHandle && 0 != taskDesc->threadId),
#endif
				XME_CORE_STATUS_INVALID_CONFIGURATION,
				{
					pthread_mutex_unlock(&taskDesc->threadMutex);
					pthread_mutex_unlock(&xme_hal_sched_config.taskDescriptorsMutex);
				}
			); 

			XME_CHECK_REC
			(
				XME_HAL_SCHED_TASK_STATE_TERMINATING != taskDesc->taskState,
				XME_CORE_STATUS_PERMISSION_DENIED,
				{
					pthread_mutex_unlock(&taskDesc->threadMutex);
					pthread_mutex_unlock(&xme_hal_sched_config.taskDescriptorsMutex);
				}
			);

			// Check whether a state change is necessary
			if (!running ^ !!(taskDesc->taskState & XME_HAL_SCHED_TASK_STATE_SUSPENDED))
			{
				// Update suspended flag
				if (running)
				{
					taskDesc->taskState = XME_HAL_SCHED_TASK_STATE_RUNNING;
				}
				else
				{
					taskDesc->taskState = XME_HAL_SCHED_TASK_STATE_SUSPENDED;
				}

				// Signal the thread
				pthread_cond_signal(&taskDesc->stateChangedEvent);			//CHECK: is it working properly?
			}
		}
		pthread_mutex_unlock(&taskDesc->threadMutex);
	}
	pthread_mutex_unlock(&xme_hal_sched_config.taskDescriptorsMutex);

	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_hal_sched_removeTask
(
	xme_hal_sched_taskHandle_t taskHandle
)
{
	xme_hal_sched_taskDescriptor_t* taskDesc;

	// Synchronize access to the task descriptors mutex
	pthread_mutex_lock(&xme_hal_sched_config.taskDescriptorsMutex);
	{
		bool selfCleanup = false;

		// Verify the task handle
		XME_CHECK_REC
		(
			NULL != (taskDesc = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_sched_config.taskDescriptors, taskHandle)),
			XME_CORE_STATUS_INVALID_HANDLE,
			{
				pthread_mutex_unlock(&xme_hal_sched_config.taskDescriptorsMutex);
			}
		);
		//setpriority(PRIO_PROCESS, 0, -19); //nice value adjustment trial

		// Synchronize access to the thread mutex
		pthread_mutex_lock(&taskDesc->threadMutex);	
		{
#if generic_posix
			XME_ASSERT((0 != taskDesc->threadHandle) && (0 != taskDesc->threadId));	
#else
			XME_ASSERT(0 != taskDesc->threadHandle);
#endif	
			// Schedule the task for termination (this will resume the task)
			taskDesc->taskState = XME_HAL_SCHED_TASK_STATE_TERMINATING;				
			
			// If this function has been called from the context of the thread
			// to be removed, we set the selfCleanup flag and can safely
			// continue, as it is guaranteed that the thread will check the
			// task state before subsequently executing the callback function.
			// However, if this function has not been called from the context
			// of the thread, directly cancel it. 
			//TODO: See ticket #753
			if (pthread_equal(pthread_self(),taskDesc->threadHandle))  //needed because it can be called by component itself.
			{
				taskDesc->selfCleanup = selfCleanup = true;
				//printf("xme_hal_sched_removeTask: %ld %ld=?%ld=?%ld %d %d\n",getpid(),gettid(),pthread_self(),taskDesc->threadHandle,sched_get_priority_max(sched_getscheduler(0)),sched_getscheduler(0));
			}
			else
			{
				// TODO: Is this threadsafe? See ticket #753
				// direct cancel : better performance but xme_hal_tls_deregisterThread() - xme_hal_random_deregisterThread() problem! 
				// otherwise, how to cancel a task like chatcomponent (startMs=0,periodMs=0 and inside the task while(1) loop)? Additional signaling for removal maybe another solution.
				// what about something wrong in the thread which must be deleted? 
				// I think that task cancelation from outside of the task should be completely independent of the task to be deleted. 

				timer_delete(taskDesc->taskPeriodTimer);
				XME_CHECK_REC
				(
					0 == pthread_cancel(taskDesc->threadHandle), // thread-safe function
					ENOTSUP,
					{
					//printf("xme_hal_sched_removeTask: %ld %ld=?%ld=?%ld %d %d\n",getpid(),gettid(),pthread_self(),taskDesc->threadHandle,sched_get_priority_max(sched_getscheduler(0)),sched_getscheduler(0));
					}
				);
			} 
		}
		pthread_mutex_unlock(&taskDesc->threadMutex);

		if (!selfCleanup)
		{
			// Free resources allocated in the task descriptor
			pthread_mutex_destroy(&taskDesc->threadMutex);
			XME_HAL_TABLE_REMOVE_ITEM(xme_hal_sched_config.taskDescriptors, taskHandle);
		}
	}

	pthread_mutex_unlock(&xme_hal_sched_config.taskDescriptorsMutex);

	return XME_CORE_STATUS_SUCCESS;
}

void
xme_hal_sched_fini()
{
	//setpriority(PRIO_PROCESS, 0, -20);
	// Synchronize access to the task descriptors mutex
	pthread_mutex_lock(&xme_hal_sched_config.taskDescriptorsMutex);
	{
		// Terminate all threads and wait for them to "join"
		XME_HAL_TABLE_ITERATE
		(
			xme_hal_sched_config.taskDescriptors,
			xme_hal_sched_taskHandle_t, taskHandle,
			xme_hal_sched_taskDescriptor_t, taskDesc,
			{
				xme_hal_sched_removeTask(taskHandle);
			}
		);
	}
	pthread_mutex_unlock(&xme_hal_sched_config.taskDescriptorsMutex);

	// Destroy the task descriptors mutex
	pthread_mutex_destroy(&xme_hal_sched_config.taskDescriptorsMutex);

	XME_HAL_TABLE_FINI(xme_hal_sched_config);
}

// TODO (See ticket #814): What about calling suspend, then immediately resume?
//                         Should the task be executed at now, now+period or at old+period?
//                         The current implementation is now. This means that calling suspend
//                         and resume eliminates the startup time given if it has not already
//                         elapsed.

// Is it necessary to use xme specific error support? Is returning linux errno as below sufficient? If yes, it must be changed above, as well.     
void*
xme_hal_sched_taskProc
(
	void* arg
)
{
	xme_hal_sched_taskHandle_t taskHandle = (xme_hal_sched_taskHandle_t)arg;
	xme_hal_sched_taskDescriptor_t* taskDesc;
	struct itimerspec setting_armed, setting_disarmed ;
#if generic_posix
	sigset_t waitset2;
	static struct sigaction sa;
#endif
	XME_ASSERT_RVAL(0 != taskHandle, NULL);

	// Retrieve the task context
	pthread_mutex_lock(&xme_hal_sched_config.taskDescriptorsMutex);
	{
		taskDesc = XME_HAL_TABLE_ITEM_FROM_HANDLE(xme_hal_sched_config.taskDescriptors, taskHandle);
	}
	pthread_mutex_unlock(&xme_hal_sched_config.taskDescriptorsMutex);

	XME_ASSERT_RVAL(NULL != taskDesc, NULL);

	XME_ASSERT_RVAL
	(
		XME_HAL_SCHED_TASK_STATE_RUNNING == taskDesc->taskState ||
		XME_HAL_SCHED_TASK_STATE_SUSPENDED == taskDesc->taskState ||
		XME_HAL_SCHED_TASK_STATE_TERMINATING == taskDesc->taskState,
		NULL
	);

#if 0
	XME_CHECK
	(
		0 == pthread_setschedparam(taskDesc->threadHandle, SCHED_OTHER, &sp),
		XME_CORE_STATUS_OUT_OF_RESOURCES
	);
#endif

	// Synchronize access to the thread mutex
	pthread_mutex_lock(&taskDesc->threadMutex);
	{

		XME_ASSERT_RVAL(NULL != taskDesc->callback, NULL);
#if linux_posix
		taskDesc->signal_timer.sigev_notify_thread_id = gettid();
			
			XME_CHECK_REC
			(
				0 == timer_create(CLOCK_REALTIME,&(taskDesc->signal_timer), &(taskDesc->taskPeriodTimer)), // The new timer is initially disarmed.
				XME_HAL_SCHED_INVALID_TASK_HANDLE,
				{	
					pthread_mutex_unlock(&taskDesc->threadMutex);
					pthread_mutex_destroy(&taskDesc->threadMutex);
					pthread_cond_destroy(&taskDesc->stateChangedEvent);
					XME_HAL_TABLE_REMOVE_ITEM(xme_hal_sched_config.taskDescriptors, taskHandle);
					pthread_mutex_unlock(&xme_hal_sched_config.taskDescriptorsMutex);
					pthread_exit(NULL);
				}
			);	
#endif
		// Register the thread							
		{
		xme_core_status_t result;

		result = xme_hal_random_registerThread();
		XME_ASSERT_RVAL(XME_CORE_STATUS_SUCCESS == result, NULL);

		result = xme_hal_tls_registerThread();
		XME_ASSERT_RVAL(XME_CORE_STATUS_SUCCESS == result, NULL);
		}

		//setpriority(PRIO_PROCESS, 0, 19);

		// Setting to arm a timer
		// we specify the start time=1 microsecond if startMs equals to 0
		// (given in 100 nanosecond units = 0.0001 milliseconds) and the 
		// periodMs as regular period (in milliseconds).
		//printf("startMs%d\n",taskDesc->startMs);
		//printf("periodMs%d\n",taskDesc->periodMs);
		//printf("threadid%d\n",taskDesc->threadId);
		//printf("tid:%d\n",(int)gettid());
		//printf("pthread:%d\n",(int)pthread_self());
		if(0==taskDesc->startMs){
			setting_armed.it_value.tv_sec=0;
			setting_armed.it_value.tv_nsec=1000;
			setting_armed.it_interval.tv_sec=(taskDesc->periodMs/1000);
			setting_armed.it_interval.tv_nsec=1000000*(taskDesc->periodMs % 1000);	
		}
		else{
			setting_armed.it_value.tv_sec=taskDesc->startMs/10000000;
			setting_armed.it_value.tv_nsec=100*(taskDesc->startMs % 10000000);
			setting_armed.it_interval.tv_sec=(taskDesc->periodMs/1000);
			setting_armed.it_interval.tv_nsec=1000000*(taskDesc->periodMs % 1000);
		}

		//Setting for disarm a timer
		setting_disarmed.it_value.tv_sec=0;
		setting_disarmed.it_value.tv_nsec=0;
		setting_disarmed.it_interval.tv_sec=1;
		setting_disarmed.it_interval.tv_nsec=0;
		
		sigemptyset( &taskDesc->waitset );
#if linux_posix
		sigaddset( &taskDesc->waitset, SIGALRM);
		pthread_sigmask( SIG_BLOCK, &taskDesc->waitset, NULL );
#else
		sigemptyset( &taskDesc->waitset );
		sigaddset( &taskDesc->waitset, (SIGRTMIN+taskDesc->threadId));
		sigprocmask(SIG_BLOCK, &taskDesc->waitset, &waitset2);
		sa.sa_handler = taskDesc->callback;
		sigaction((SIGRTMIN+taskDesc->threadId),&sa,NULL);
#endif

#if 1		 
		XME_CHECK_REC
		(
			0 == timer_settime(taskDesc->taskPeriodTimer, 0, &setting_armed, NULL),
			NULL,						
			{
				pthread_mutex_unlock(&taskDesc->threadMutex);
				printf("Error No:%d\n",errno);
			}

		);
#endif
		//pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);

		while(1)
		{
			while(taskDesc->taskState == XME_HAL_SCHED_TASK_STATE_RUNNING)
			{
				pthread_mutex_unlock(&taskDesc->threadMutex);

				XME_CHECK_REC
				(
#if linux_posix
					0 == sigwait(&taskDesc->waitset,&taskDesc->sig_no),
#else
					-1 ==sigsuspend(&waitset2),
#endif
					NULL,						
					{
						 //TIMEOUT
						printf("Error No:%d\n",errno);
					}

				); 
#if linux_posix
				sigaddset( &taskDesc->waitset, SIGALRM);
#else
				sigaddset( &taskDesc->waitset, SIGRTMIN+taskDesc->threadId);
#endif	
				//pthread_testcancel();
				sched_yield();
				pthread_mutex_lock(&taskDesc->threadMutex);
				
				if (XME_HAL_SCHED_TASK_STATE_TERMINATING == taskDesc->taskState || XME_HAL_SCHED_TASK_STATE_SUSPENDED == taskDesc->taskState)
				{

					XME_CHECK_REC
					(
						0 == timer_settime(taskDesc->taskPeriodTimer, 0, &setting_disarmed, NULL),		
						NULL,					
						{
							pthread_mutex_unlock(&taskDesc->threadMutex);
							printf("Error No:%d\n",errno);
						}

					);
		
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
	
				if (XME_HAL_SCHED_TASK_STATE_TERMINATING == taskDesc->taskState || XME_HAL_SCHED_TASK_STATE_SUSPENDED == taskDesc->taskState)
				{
					XME_CHECK_REC
					(
						0 == timer_settime(taskDesc->taskPeriodTimer, 0, &setting_disarmed, NULL),		
						NULL,					
						{
							pthread_mutex_unlock(&taskDesc->threadMutex);
							printf("Error No:%d\n",errno);
						}

					);

					break;
				}	
			
				// Remove the task if it was only scheduled for a single execution - one shot-timer.
				if (0 == taskDesc->periodMs)
				{
					//xme_hal_sched_removeTask(taskHandle);
					taskDesc->taskState=XME_HAL_SCHED_TASK_STATE_TERMINATING;
					taskDesc->selfCleanup=true;
					break;
				}
				else
				{
					// The threadPeriodTimer must not be NULL for periodic tasks!
					XME_ASSERT_RVAL(NULL != taskDesc->taskPeriodTimer, NULL);
				}
			}

			if (XME_HAL_SCHED_TASK_STATE_TERMINATING == taskDesc->taskState)
			{
				if (NULL != taskDesc->taskPeriodTimer)
				{
					// Cancel the waitable timer
					timer_delete(taskDesc->taskPeriodTimer);
					taskDesc->taskPeriodTimer = NULL;
				}

				break;
			}

			while(taskDesc->taskState == XME_HAL_SCHED_TASK_STATE_SUSPENDED)
			{		
				
				XME_CHECK_REC
				(
					0 == timer_settime(taskDesc->taskPeriodTimer, 0, &setting_disarmed, NULL),
					NULL,
					{
						pthread_mutex_unlock(&taskDesc->threadMutex);
						printf("Error No:%d\n",errno);
					}

				);	
				
				pthread_cond_wait(&taskDesc->stateChangedEvent, &taskDesc->threadMutex); // lock-unlock the mutex automatically
				
				XME_CHECK_REC
				(	
					0 == (pthread_cond_init(&taskDesc->stateChangedEvent,NULL)),	//unsignal the cond. variable	CHECK!
					XME_HAL_SCHED_INVALID_TASK_HANDLE,
					{
						// fill here
					}
				);

				if (XME_HAL_SCHED_TASK_STATE_TERMINATING == taskDesc->taskState)
				{

					break;
				}
				
				else if(taskDesc->taskState == XME_HAL_SCHED_TASK_STATE_RUNNING)
				{	

					XME_CHECK_REC
					(
						0 == timer_settime(taskDesc->taskPeriodTimer, 0, &setting_armed, NULL),
						NULL,					
						{
							pthread_mutex_unlock(&taskDesc->threadMutex);
							printf("Error No:%d\n",errno);
						}
					);

					break;
				}
				else
				{
					//never enter here!
				}
				
			}
		}


		if (taskDesc->selfCleanup)		
		{
			// Close the thread handle

			// Free resources allocated in the thread descriptor
			pthread_cond_destroy(&taskDesc->stateChangedEvent);

			// Leave the critical section
			pthread_mutex_unlock(&taskDesc->threadMutex);

			// Free resources allocated in the task descriptor
			pthread_mutex_destroy(&taskDesc->threadMutex);
	
			pthread_mutex_lock(&xme_hal_sched_config.taskDescriptorsMutex);
			{
				XME_HAL_TABLE_REMOVE_ITEM(xme_hal_sched_config.taskDescriptors, (xme_hal_table_rowHandle_t)taskHandle);
			}
			pthread_mutex_unlock(&xme_hal_sched_config.taskDescriptorsMutex);
	
			// Deregister the thread
			xme_hal_tls_deregisterThread();
			xme_hal_random_deregisterThread();

		}
		
		pthread_exit(NULL);

	}
}








#
# Copyright (c) 2011, fortiss GmbH.
# All rights reserved.
# 
# Use, modification and distribution are subject to the terms specified
# in the accompanying license file LICENSE.txt located at the root directory
# of this software distribution. A copy is available at
# http://chromosome.fortiss.org/.
#
# This file is part of CHROMOSOME.
#

# Ticks
# No default value given here since we xme_build_option_assert() that the user supplies
# matching value for his architecture
xme_build_option_assert("EXTERNAL_FREERTOS_configCPU_CLOCK_HZ" "Probably in bsp/<...>/Options.cmake")
xme_build_option(EXTERNAL_FREERTOS_configCPU_CLOCK_HZ "" "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")

xme_build_option(EXTERNAL_FREERTOS_configTICK_RATE_HZ "( ( portTickType ) 1000 )" "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_configUSE_16_BIT_TICKS 0 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")

# Memory limits
xme_build_option_assert("EXTERNAL_FREERTOS_configTOTAL_HEAP_SIZE" "Probably in bsp/<...>/Options.cmake")
xme_build_option(EXTERNAL_FREERTOS_configTOTAL_HEAP_SIZE 20480 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")

xme_build_option_assert("EXTERNAL_FREERTOS_configMINIMAL_STACK_SIZE" "Probably in bsp/<...>/Options.cmake")
xme_build_option(EXTERNAL_FREERTOS_configMINIMAL_STACK_SIZE 256 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")

# Priorities
xme_build_option(EXTERNAL_FREERTOS_configMAX_PRIORITIES "( ( unsigned portBASE_TYPE ) 6 )" "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_configMAX_CO_ROUTINE_PRIORITIES 2 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")

# KERNEL_INTERRUPT_PRIORITY should be the lowest available priority
# MAX_SYCALL_INTERRUPT_PRIORITY defines a threshold:
# - Interrupts with a priority higher than MAX_SYCALL_INTERRUPT_PRIORITY are not allowed to call any FreeRTOS-API-Functions
# - Interrupts with a priority lower than MAX_SYCALL_INTERRUPT_PRIORITY are allowed to call FreeRTOS-API-Functions
xme_build_option(EXTERNAL_FREERTOS_configKERNEL_INTERRUPT_PRIORITY "255" "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_configMAX_SYSCALL_INTERRUPT_PRIORITY "64" "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")

# Kernel configuration / features (by default, disable all configurable kernel features)
xme_build_option(EXTERNAL_FREERTOS_configUSE_PREEMPTION 0 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_configUSE_CO_ROUTINES 0 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_configUSE_IDLE_HOOK 0 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_configIDLE_SHOULD_YIELD 0 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_configUSE_TICK_HOOK 0 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")

xme_build_option(EXTERNAL_FREERTOS_configUSE_MUTEXES 1 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_configUSE_RECURSIVE_MUTEXES 0 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")

# Debug and trace support
xme_build_option(EXTERNAL_FREERTOS_configUSE_TRACE_FACILITY 0 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_configCHECK_FOR_STACK_OVERFLOW 0 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_configQUEUE_REGISTRY_SIZE 0 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_configMAX_TASK_NAME_LEN 12 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")

# APIs (by default, enable all APIs)
xme_build_option(EXTERNAL_FREERTOS_INCLUDE_vTaskPrioritySet 1 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_INCLUDE_uxTaskPriorityGet 1 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_INCLUDE_vTaskDelete 1 "FreeRTOSConfig.h"  "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_INCLUDE_vTaskCleanUpResources 0 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_INCLUDE_vTaskSuspend 1 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_INCLUDE_vTaskDelayUntil 1 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_INCLUDE_vTaskDelay 1 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_INCLUDE_uxTaskGetStackHighWaterMark 0 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")

# Timers
xme_build_option(EXTERNAL_FREERTOS_configUSE_TIMERS 0 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_configTIMER_TASK_PRIORITY "( configMAX_PRIORITIES - 2)" "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_configTIMER_QUEUE_LENGTH 5 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")
xme_build_option(EXTERNAL_FREERTOS_configTIMER_TASK_STACK_DEPTH "(configMINIMAL_STACK_SIZE)" "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")

# Run-time stats
xme_build_option(EXTERNAL_FREERTOS_configGENERATE_RUN_TIME_STATS 0 "FreeRTOSConfig.h" "EXTERNAL_FREERTOS_")

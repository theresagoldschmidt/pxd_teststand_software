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
 *         TDMA execution manager.
 *
 * \author
 *         Kai Huang <khuang@fortiss.org>
 */

#ifndef EXE_TDMA_H_
#define EXE_TDMA_H_
    #include "FreeRTOS.h"
    #include "task.h"
    //hardware dependence macro
    #define configCPU_TDMA_CLOCK_HZ ( ( unsigned long ) SYS_CLK_FREQ )             //open to user
    //usr dependence macro
    #define xme_task_num 6                                                         //open to user                                                         //open to user
    #define configCPU_TDMA_CLOCK_US ( ( unsigned long ) 1000000)               //
    #define configCPU_TDMA_CLOCK_US_devided ( ( unsigned long ) 85)
    #define TDMA_policy 1
    #define Debug 1                                                          //open to user
    // Creat TDMA Task  Wapper
    typedef void (*exe_callback_func) (void* userData);
    typedef struct
    {
        xTaskHandle taskRTOSHandle; ///< Handle to the task. This is a FreeRTOS-specific handle.
        unsigned char taskState; ///< Execution state of the task (running, suspended, terminating).
        #ifdef  TDMA_policy
        unsigned int slot_len;
        volatile unsigned char task_depth;
        #endif
        #ifdef  EDF_policy
        unsigned int arrive_time;
        unsigned int dead_line;
        #endif
        unsigned char priority; ///< Priority of the task. Higher values denote a higher priority.
        exe_callback_func callback; ///< Task callback function.
        //xme_core_component_t componentContext; ///< Context of the component this task belongs to.
        void* userData; ///< User data to pass to task callback function.
    } exe_task_descriptor_t;
    
    volatile exe_task_descriptor_t task_Desc[xme_task_num];                             //opne to user
    void init_tdma_timer(unsigned int);
    void exe_sched_init();
    void exe_sched_taskWrapper(void*);
#endif /*EXE_TDMA_H_*/

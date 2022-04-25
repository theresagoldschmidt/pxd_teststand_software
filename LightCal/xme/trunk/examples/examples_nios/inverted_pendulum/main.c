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
 *         Inverted pendulum example.
 *
 * \author
 *         Kai Huang <khuang@fortiss.org>
 */

/*
 * main.c
 *
 *  Created on: 2012-2-13
 *      Author: khuang
 */

/* Standard includes. */
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "FreeRTOS.h"

/* Scheduler includes. */
#include "exe_tdma.h"
#include "inverted_pendulum.h"

#if 1
void call_back6(void* param)
{
    static unsigned char led=0x20;
    unsigned int i;
    led=IORD(LED_PIO_BASE,0); ;
    led=(led&(~0x20))+((~(led&0x20))&0x20);
    IOWR(LED_PIO_BASE,0,led);
    for(i=0;i<100;i++);
    //printf("run task6\n");
}
#endif

//add task
void exe_sched_add_task() {
    unsigned int i;
    portBASE_TYPE res;

    printf("in exe_sched_add_task\n");

//    task_Desc[0].callback=call_back6;
    task_Desc[0].callback=xme_inverted_pendulum_task;
    task_Desc[1].callback=call_back6;
    task_Desc[0].slot_len=400;
    task_Desc[1].slot_len=50;

    for(i=0;i<xme_task_num;i++) {
    	task_Desc[i].priority=1;
    	res = xTaskCreate(
    			exe_sched_taskWrapper,
    			( signed char * ) "Task",
    			configMINIMAL_STACK_SIZE,
    			&task_Desc[i],
    			task_Desc[i].priority,
    			&(task_Desc[i].taskRTOSHandle)
    	);
    	if ( res != pdPASS )
    		printf("erro,fail to create task\n");
    	else
    		printf("success to create task\n");
    }
}


int main()
{
    printf("enter main\n");
	xme_inverted_pendulum_create(NULL);
    exe_sched_add_task();
    exe_sched_init();

    printf("init finished\n");
    //IOWR(LED_PIO_BASE,0,0xff);

//    sleep(200);
    vTaskStartScheduler();
    for(;;); // never reach here
    return  0;
}

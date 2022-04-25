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
 *         Nios II FreeRTOS example.
 *
 * \author
 *         Kai Huang <khuang@fortiss.org>
 */
 
/* Standard includes. */
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "system.h"
#include "altera_avalon_pio_regs.h"
#include <stdio.h>
#include "exe_tdma.h"

#if 1


void delay_usr(unsigned int num)
{
    unsigned int i,j;
    for(i=0;i<num;i++)
    {
        for(j=0;j<1000;j++);
    }
}

void call_back1(void* param)
{
    static unsigned char led=0x01;
    unsigned int i;
    led=IORD(LED_PIO_BASE,0); ;
    led=(led&(~0x01))+((~(led&0x01))&0x01);
    IOWR(LED_PIO_BASE,0,led);
    for(i=0;i<100;i++);
    printf("run task0\n");
}
void call_back2(void* param)
{
    static unsigned char led=0x02;
    unsigned int i;
    led=IORD(LED_PIO_BASE,0); ;
    led=(led&(~0x02))+((~(led&0x02))&0x02);
    IOWR(LED_PIO_BASE,0,led);
    for(i=0;i<100;i++);
    printf("run task1\n");
}

void call_back3(void* param)
{
    static unsigned char led=0x04;
    unsigned int i;
    led=IORD(LED_PIO_BASE,0); ;
    led=(led&(~0x04))+((~(led&0x04))&0x04);
    IOWR(LED_PIO_BASE,0,led);
    for(i=0;i<100;i++);
    printf("run task2\n");
}

void call_back4(void* param)
{
    static unsigned char led=0x08;
    unsigned int i;
    led=IORD(LED_PIO_BASE,0); ;
    led=(led&(~0x08))+((~(led&0x08))&0x08);
    IOWR(LED_PIO_BASE,0,led);
    for(i=0;i<100;i++);
    printf("run task3\n");
}

void call_back5(void* param)
{
    static unsigned char led=0x10;
    unsigned int i;
    led=IORD(LED_PIO_BASE,0); ;
    led=(led&(~0x10))+((~(led&0x10))&0x10);
    IOWR(LED_PIO_BASE,0,led);
    for(i=0;i<100;i++);
    printf("run task4\n");
}

void call_back6(void* param)
{
    static unsigned char led=0x20;
    unsigned int i;
    led=IORD(LED_PIO_BASE,0); ;
    led=(led&(~0x20))+((~(led&0x20))&0x20);
    IOWR(LED_PIO_BASE,0,led);
    for(i=0;i<100;i++);
    printf("run task5\n");
}

 //add task
 void exe_sched_add_task()
 {
        unsigned int i;
        portBASE_TYPE res;

        printf("in exe_sched_add_task\n");

        task_Desc[0].callback=call_back1;
        task_Desc[1].callback=call_back2;
        task_Desc[2].callback=call_back3;
        task_Desc[3].callback=call_back4;
        task_Desc[4].callback=call_back5;
        task_Desc[5].callback=call_back6;
        task_Desc[0].slot_len=50000;
        task_Desc[1].slot_len=50000;
        task_Desc[2].slot_len=50000;
        task_Desc[3].slot_len=50000;
        task_Desc[4].slot_len=50000;
        task_Desc[5].slot_len=50000;
#if 1
        for(i=0;i<xme_task_num;i++)
        {
            task_Desc[i].priority=i+40;
            res=xTaskCreate(
                                exe_sched_taskWrapper,
                                ( signed char * ) "Task",
                                configMINIMAL_STACK_SIZE,
                                &task_Desc[i],
                                task_Desc[i].priority,
                                &(task_Desc[i].taskRTOSHandle)
                                );
            if ( res != pdPASS )
            {
               printf("erro,fail to create task\n");
            }
            else
            {
               printf("success to create task\n");
            }
        }
#endif
 }

int main()
{
    exe_sched_add_task();
    exe_sched_init();
    printf("haha\n");
    IOWR(LED_PIO_BASE,0,0xff);
    vTaskStartScheduler();
    for(;;);
    return  0;
}

#endif

///*-----------------------------------------------------------*/
//
void vApplicationStackOverflowHook( void )
{
	/* Look at pxCurrentTCB to see which task overflowed its stack. */
	for( ;; )
    {
		asm( "break" );
    }
}

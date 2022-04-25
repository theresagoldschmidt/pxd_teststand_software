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

/*
 * main.bk.c
 *
 *  Created on: 2012-1-27
 *      Author: khuang
 */

/* Standard includes. */
#include <stddef.h>
#include <stdio.h>
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* NiosII includes. */
#include "system.h"
#include "altera_avalon_pio_regs.h"

#define __FREERTOS 1

#if __FREERTOS
void call_back1(void* param);
void call_back2(void* param);
#endif



int main()
{
	unsigned int i = 200;
	unsigned int j = 0;
    unsigned char led=0x01;
    portBASE_TYPE res;

    printf("hello world!\n");

    led =0xff;
   	IOWR(LED_PIO_BASE,0,led);
  	i=50000; while(i--);

//    	led =0x55;
 //   	IOWR(LED_PIO_BASE,0,led);


#if ! __FREERTOS
	while(1) {
        if(led==0x80) led=0x01;
        else          led=led<<1;
        IOWR(LED_PIO_BASE,0,led);

        i = 50000;
        while(i--);

        printf("haha: %x\n", led);
		//printf("j:%d\n",j++);
	}
#else

    res = xTaskCreate(
                call_back1,
                ( signed char * ) "Task1",
                configMINIMAL_STACK_SIZE,
                NULL,
                1,
                NULL
    );

    if ( res != pdPASS )
         printf("erro,fail to create task:%d\n", res);
    else
          printf("success to create task\n");

    //i=50000; while(i--);
    //led =0x03;
     //   	IOWR(LED_PIO_BASE,0,led);

    xTaskCreate(
                call_back2,
                ( signed char * ) "Task2",
                configMINIMAL_STACK_SIZE,
                NULL,
                2,
                NULL
    );

   // i=50000; while(i--);
	led =0xaa;
	IOWR(LED_PIO_BASE,0,led);
    printf("start schedule\n");
    vTaskStartScheduler();

    while(1);
#endif

}


#if  __FREERTOS

////test FreeRTOS
void call_back1(void* param)
{
    for(;;)
    {
    IOWR(LED_PIO_BASE,0,0x0f);
    printf("run task1\n");
    vTaskDelay(2000);
    }
}
void call_back2(void* param)
{
	for(;;)
    {
    IOWR(LED_PIO_BASE,0,0xf0);
    printf("run task2\n");
    vTaskDelay(1000);
    }
}
#endif

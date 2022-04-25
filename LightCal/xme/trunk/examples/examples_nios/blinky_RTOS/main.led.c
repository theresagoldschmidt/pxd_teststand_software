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
 *         Nios II Free RTOS example.
 *
 * \author
 *         Gang Chen <gachen@fortiss.org>
 */

/*--------------------------------------------------------------------
 * this code is for freeRTOS transplat test
 * function test DDR, Jtag uart, LED
 * Gang Chen
----------------------------------------------------------------------*/
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include <stdio.h>
void delay_usr(unsigned int num)
{
    unsigned int i,j;
    for(i=0;i<num;i++)
    {
        for(j=0;j<100;j++);
    }
}
int main()
{
    unsigned char led=0x01;
    unsigned int i,j;
    unsigned int img[1024][1024];
    printf("hello, this is demo for RTOS^_^\n");
 /*   for(i=0;i<1024;i++)
    {
        for(j=0;j<1024;j++)
            {
                img[i][j]=(i+j);
            }
    } */
    i=0;
    j=0;
    printf("hello, this is demo for RTOS^_^\n");
    while(1)
    {
        IOWR(LED_PIO_BASE,0,led);
        if(led==0x80)
            led=0x01;
        else
            led=led<<1;
        delay_usr(100);
        printf("[%d,%d] output pixel value:%d\n",i,j,img[i][j]);
        j++;
        if(j>1023)
        {
            i++;
            j=0;
            if(i>1023)
            {
                i=0;
                j=0;
            }
        }
    }
    return 0;
}

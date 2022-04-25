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
 *         Component for the inverted pendulum.
 *
 * \author
 *         Kai Huang <khuang@fortiss.org>
 *         Gang Chen <gachen@fortiss.org>
 */

 /*
 * inverted_pendulum.c
 *
 *  Created on: 2012-2-13
 *      Author: khuang
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdio.h>

#include "system.h"
#include "io.h"
#include "sys/alt_timestamp.h"      //timestamp


#include "inverted_pendulum.h"
#include "control.h"
#include "encoder_sensor.h"
#include "dac.h"
#include "upm.h"

//#include "xme/hal/dio.h"
//#include "xme/core/resourceManager.h"

//#define DEBUG
/******************************************************************************/
/***   Forward declarations                                                 ***/
/******************************************************************************/
void xme_inverted_pendulum_task(void* data);
static int ip_count;

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
//static char state = 0;
//static xme_core_resourceManager_taskHandle_t task;

xme_core_status_t
xme_inverted_pendulum_create(xme_inverted_pendulum_configStruct_t* config)
{
	/*
	 * Bit 0 of the HSMC_PIO is used for the \Load Pin of the SPI
	 * The \Load Pin is active low and has to be high in the inactive state
	 * it is set to 0 because the signal is inverted in the optocoupler
	 */
	IOWR_ALTERA_AVALON_PIO_DATA(HSMC_PIO_BASE, 8);

#ifdef DEBUG
	ip_count = 0;
    alt_timestamp_start();
#endif

#if 1
	dac_init();
//	enc_init();
	upm_init();
	dac_set_voltage(0);
	sensor_init();
	upm_enable();
	control_init();
#endif

#if 1
	printf("waiting for a button press\n");

	// Push button 0 of the FPGA has to be pressed to continue with the control
	// The button should be pressed when the initialization was successful and the
	// Enable LED of the UPM is on
	while(IORD_ALTERA_AVALON_PIO_DATA(BUTTON_PIO_BASE) != 14){
		// If the initialization was not succesful press push button 1 to
		// re-initialize
		if(IORD_ALTERA_AVALON_PIO_DATA(BUTTON_PIO_BASE) == 13){
			printf("initializing\n");
			dac_init();
//			enc_init();
			sensor_init();
			upm_init();
			dac_set_voltage(0);
//			enc_set_position(0, 0);
//			enc_set_position(1, 0);
			upm_enable();
			control_init();
		}
	}
	printf("Button was pressed\n");

	// Start the cycle timer
//	init_timer_main();
#endif


	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_inverted_pendulum_activate(xme_inverted_pendulum_configStruct_t* config)
{
//	task = xme_core_resourceManager_scheduleTask (1000,1000,0,	xme_inverted_pendulum_task,	(void *)config);
//	return XME_CORE_STATUS_SUCCESS;
	return 0;
}

void
xme_inverted_pendulum_deactivate(xme_inverted_pendulum_configStruct_t* config)
{
	//xme_core_resourceManager_killTask( task );
}

void
xme_inverted_pendulum_destroy(xme_inverted_pendulum_configStruct_t* config)
{
}


void
xme_inverted_pendulum_task(void* data)
{
	alt_32 voltage = 0;
	alt_32 x_d = 0;
	alt_32 I_m = 0;

#ifdef DEBUG
	int count=0;
	unsigned int time_test_end,time_test_start;
    time_test_start=alt_timestamp();
#endif

	// Safety function to stop the motor before it reaches the end of the rail
	if(get_position() > MAX_AMPLITUDE || get_position() < -MAX_AMPLITUDE){
		printf("Exceeded position\n");
		dac_set_voltage(0);
		return;
	}


	I_m = control_cart(&x_d); //8us


	// I_m -> voltage : the UPM Amplifier Current Gain = 5 A/V
	// I_m is in uA and voltage should be mV
	// the minus is because of the "right-hand-rule", i.e. when the
	// current is positive, the cart has to move to the right! Thus,
	// the voltage has to be negative!!
	voltage = -I_m / 5000;

	// set the new value
	// security!! :) set range to -MAX_MV ... MAX_MV
	if(voltage > MAX_MV)
		voltage = MAX_MV;
	else if(voltage < -MAX_MV)
		voltage = -MAX_MV;

	//while(count < 4000) count++;//1.4+ ms for count=10000; .642ms for 4500
	//while(count < 4000) count++;//1.4+ ms for count=10000; .642ms for 4500

	dac_set_voltage((alt_16)voltage);

#ifdef DEBUG
	time_test_end=alt_timestamp();
	printf("IP (time:%d)\n", (time_test_end- time_test_start)/85);
#endif
//	ip_count++;
	//printf("%d", ip_count);


	//if (ip_count >= 50)
	//	exit(0);
	return;
}



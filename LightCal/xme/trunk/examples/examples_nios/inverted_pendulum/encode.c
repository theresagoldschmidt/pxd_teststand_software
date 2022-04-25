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
 *         Encoder Sensor for the inverted pendulum.
 *
 * \author
 *         Kai Huang <khuang@fortiss.org>
 */

/*
 * encode.c
 *
 *  Created on: 2012-2-13
 *      Author: khuang
 */

#include "encoder_sensor.h"

void sensor_init()
{
	int tmp;
	set_sensor_ctrl(LOCATION_SENSOR_BASE, 1);      // reset
	set_sensor_time(LOCATION_SENSOR_BASE, (0x80000000-10000*85)); // sensing period
	set_sensor_ctrl(LOCATION_SENSOR_BASE, 0); // initialization
	set_sensor_val(LOCATION_SENSOR_BASE, 0); // set the starting value

	tmp = get_sensor_val(LOCATION_SENSOR_BASE);
//	printf("init_loc:%d",tmp);
	tmp = get_sensor_time(LOCATION_SENSOR_BASE);
//	printf("init_loc_time:%x",tmp);
	set_sensor_ctrl(ANGLE_SENSOR_BASE, 1); // reset
	set_sensor_time(ANGLE_SENSOR_BASE, (0x80000000-10000*85)); // sensing period
	set_sensor_ctrl(ANGLE_SENSOR_BASE, 0); // initialization
	set_sensor_val(ANGLE_SENSOR_BASE, 0); // set the starting value
}

inline alt_32 get_position()
{	int tmp1, tmp2;
	tmp1 = get_sensor_val(LOCATION_SENSOR_BASE);
	tmp2 = get_sensor_val(ANGLE_SENSOR_BASE);

	return get_sensor_val(LOCATION_SENSOR_BASE);
}


inline alt_32 get_velocity()
{
	// for updating the velo counter every 10msec
	//	return 100*(alt_u32)velo; Do we need to mutiple 100 ???
	return 100* get_sensor_speed(LOCATION_SENSOR_BASE);
}

inline alt_32 get_angle()
{
	alt_32 tmp_count = 2*get_sensor_val(ANGLE_SENSOR_BASE);

	if(tmp_count < -1024){
		tmp_count += 2048;
		set_sensor_val(ANGLE_SENSOR_BASE, get_sensor_val(ANGLE_SENSOR_BASE)+1024);
	} else if(tmp_count > 1024){
		tmp_count -= 2048;
		set_sensor_val(ANGLE_SENSOR_BASE, get_sensor_val(ANGLE_SENSOR_BASE)-1024);
	}

	return (2 * 3142 * tmp_count) / 2048; // counter1 is global !!??

}

inline alt_32 get_angular_velocity()
{
	// 2 * PI/s = 2048 ticks/s
	return (2 * 3142 * 2* get_sensor_speed(ANGLE_SENSOR_BASE)) / 2048;
}

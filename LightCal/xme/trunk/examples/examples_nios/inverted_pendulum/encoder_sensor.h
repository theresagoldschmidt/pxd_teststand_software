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
 *         Gang Chen <gachen@fortiss.org>
 */

/*
 * encoder_sensor.h
 *
 *  Created on: Feb 6, 2012
 *      Author: Gang
 */

#ifndef ENCODER_SENSOR_H_
#define ENCODER_SENSOR_H_

#include "system.h"
#include "io.h"

//write_operation
#define set_sensor_speed(base_addr,data) IOWR(base_addr+(0<<2), 0, data) //TODO
#define set_sensor_val(base_addr,data)   IOWR(base_addr+(1<<2), 0, data) //TODO
#define set_sensor_ctrl(base_addr,data)  IOWR(base_addr+(2<<2), 0, data)
#define set_sensor_time(base_addr,data)	 IOWR(base_addr+(3<<2), 0, data) //TODO,set the time for compute speed

//read operation
#define get_sensor_speed(base_addr)	     IORD(base_addr+(0<<2), 0)
#define get_sensor_val(base_addr)        IORD(base_addr+(1<<2), 0)
#define get_sensor_ctrl(base_addr)       IORD(base_addr+(2<<2), 0)
#define get_sensor_time(base_addr)	     IORD(base_addr+(3<<2), 0)

void sensor_init();
alt_32 get_position();
alt_32 get_velocity();
alt_32 get_angle();
alt_32 get_angular_velocity();

#endif /* ENCODER_SENSOR_H_ */

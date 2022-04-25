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
 *         Controller for the inverted pendulum.
 *
 * \author
 *         Nadine Keddis <keddis@fortiss.org>
 */

/*
 * control_sesip.c
 * This file contains the implementation of the swing-up controller
 * and the balance controller.
 *
 *  Created on: 08.06.2010
 *      Author: keddis
 */

#include <math.h>
#include "control.h"
#include "encoder_sensor.h"

#define MAX_MOVEMENT_SWINGUP 2000
#define MAX_CURRENT_SWING_UP 1700000
#define RATE_LIMIT_TICKS 7

#define MID_PEND 1
#define LONG_PEND 2
#define PEND_LENGTH MID_PEND
#define Debug_counter
typedef float Accum;
typedef enum state {SWING_UP, BALANCE} state_t;


void control_init(void){

}

alt_32 control_cart(alt_32 *x_d){

	static state_t state = SWING_UP;
//	static alt_32 alpha_offset = 0;

#if PEND_LENGTH == MID_PEND
	// controller parameters for swinging up
	alt_32 Kp_cart = -4264;
	alt_32 Kv_cart = -203;
	alt_32 Kp_pend = -1000;
	alt_32 Kd_pend = -130;
//	alt_32 alpha_down = 0;

	// controller parameters for balancing
	alt_32 K1 = -606;
	alt_32 K2 = 23550;
	alt_32 K3 = -339;
	alt_32 K4 = 2520;
	alt_32 K5 = -60;

#elif PEND_LENGTH == LONG_PEND
	// controller parameters for swinging up
	alt_32 Kp_cart = -3500;
	alt_32 Kv_cart = -480;
	alt_32 Kp_pend = -1000;
	alt_32 Kd_pend = -100;
	alt_32 alpha_down = 0;

	// controller parameters for balancing
	alt_32 K1 = -905;
	alt_32 K2 = 38300;
	alt_32 K3 = -532;
	alt_32 K4 = 5950;
	alt_32 K5 = -85;
#endif

	alt_32 tmp_x_d = 0;
	static alt_32 old_tmp_x_d = 0;
	alt_32 x_c = 0;
	alt_32 alpha = 0;
	alt_32 Dt_x_c = 0;
	static alt_32 Int_x_c = 0;
	alt_32 Dt_alpha = 0;
	int io = 0;

	alt_32 I_m = 0;

//	Accum sin_alpha, cos_alpha;

	if(state == SWING_UP){
		alpha = get_angle();
		Dt_alpha = get_angular_velocity();
		#ifdef Debug_counter
		//printf("alpha:%d Dt_alpha:%d\n", alpha, Dt_alpha);
		#endif
		if((alpha > 2970 || alpha < -2970) && (Dt_alpha < 2600 || Dt_alpha > -2600)){
			state = BALANCE;
			printf("balance\n");
			/*if(alpha > 0){
				alpha_offset = -3142;
			}
			else{
				alpha_offset = 3142;
			}*/
		} else{
			//alpha += 3142;

			//sin_alpha = (Accum) sinf(alpha/1000.0);
			//cos_alpha = (Accum) cosf(alpha/1000.0);
			//alpha_down = (alt_32) atan2f(sin_alpha, cos_alpha);
			//float alpha_down_f = atan2f(cos_alpha, sin_alpha);
			//alpha_down = (alt_32) alpha_down_f;

			x_c = -get_position();
			Dt_x_c = -get_velocity();
			#ifdef Debug_counter
			//printf("x_c:%d Dt_x_c:%d\n", x_c, Dt_x_c);
			#endif
			//tmp_x_d = Kp_pend * alpha_down + Kd_pend * Dt_alpha;
			tmp_x_d = Kp_pend * alpha + Kd_pend * Dt_alpha;

			if(tmp_x_d > MAX_MOVEMENT_SWINGUP){
				//IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, io & 0x01);
				tmp_x_d = MAX_MOVEMENT_SWINGUP;
				io++;
			} else if (tmp_x_d < -MAX_MOVEMENT_SWINGUP){
				//IOWR_ALTERA_AVALON_PIO_DATA(LED_PIO_BASE, io & 0x01);
				tmp_x_d = -MAX_MOVEMENT_SWINGUP;
				io++;
			}

			// rate limiter
			if(tmp_x_d - old_tmp_x_d > RATE_LIMIT_TICKS){
				tmp_x_d = old_tmp_x_d + RATE_LIMIT_TICKS;
			} else if(tmp_x_d - old_tmp_x_d < -RATE_LIMIT_TICKS){
				tmp_x_d = old_tmp_x_d - RATE_LIMIT_TICKS;
			}
			old_tmp_x_d = tmp_x_d;


			// end rate limiter
			I_m = Kp_cart * (x_c - tmp_x_d) + Kv_cart * Dt_x_c;
		}
	} else{
		// get current data
		tmp_x_d = *x_d;
		/*
		 * see NOTICE in control_position()!!!
		 */
		x_c = - get_position();
		Dt_x_c = -get_velocity();
		Int_x_c += x_c;
		// pendulum
		//alpha = enc_get_angle(1) + alpha_offset;
		alpha = get_angle();
		/*
		 * Add an offset to alpha to have a value near to zero
		 * Alpha has a value larger than 2970 or smaller than -2970
		 */
		if(alpha < 0){
			alpha += 3142;
		} else{
			alpha -= 3142;
		}
		Dt_alpha = get_angular_velocity();
	 	I_m = K1 * (tmp_x_d - x_c) + K2 * -alpha + K3 * -Dt_x_c + K4 * -Dt_alpha + K5 * (-Int_x_c / 1000);
	}

	return I_m;
}


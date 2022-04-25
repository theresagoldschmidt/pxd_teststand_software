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
 *         DAC for the inverted pendulum.
 *
 * \author
 *         Nadine Keddis <keddis@fortiss.org>
 */

/*
 * dac.c
 *
 *  Created on: 09.06.2010
 *      Author: keddis
 */

#include "dac.h"
#include "ltc1257.h"

#define DAC_MIN_LIMIT_MV (-10000)
#define DAC_MAX_LIMIT_MV 10000
#define DAC_DELTA_U_MV (DAC_MAX_LIMIT_MV << 1)

void dac_init(void){
	ltc1257_init();

	// set voltage to 0V
	dac_set_voltage(0.0);

}

void dac_done(void){
	// set voltage to 0V
	dac_set_voltage(0.0);

	ltc1257_done();
}

void dac_set_voltage(alt_16 voltage){

	alt_16 dac_voltage;
	alt_u32 tmp_voltage;
	alt_u16 dac_data;


	if(voltage > DAC_MAX_LIMIT_MV){
		dac_voltage = DAC_MAX_LIMIT_MV;
	}
	else if (voltage < DAC_MIN_LIMIT_MV){
		dac_voltage = DAC_MIN_LIMIT_MV;
	}
	else{
		/* Subtract 3 to compensate for the inaccuracy of the DAC */
		dac_voltage = voltage - 3;
	}

	tmp_voltage = (alt_u32)(-dac_voltage + DAC_MAX_LIMIT_MV);
	dac_data = (alt_u16)((tmp_voltage << 12) / DAC_DELTA_U_MV);

	ltc1257_transmit_data(dac_data);
}


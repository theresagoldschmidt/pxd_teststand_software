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
 *         UPM for the inverted pendulum.
 *
 * \author
 *         Nadine Keddis <keddis@fortiss.org>
 */

/*
 * upm.c
 * This file contains the initialization and enable sequence for the UPM.
 *
 *  Created on: 09.06.2010
 *      Author: keddis
 */

#include "upm.h"

void upm_init(){
	// Busy waiting is ok here, because the
	// controlling part is not active yet

	// set AMP_EN to low
	// AMP_EN is bit 1 of UPM
	// set AMP_CAL to high
	// AMP_CAL is bit 0 of UPM
	int input = 1;
	IOWR_ALTERA_AVALON_PIO_DATA(UPM_PIO_BASE, input);

	usleep(200000);

	// AMP_CAL for 200ms to low
	input = 0;
	IOWR_ALTERA_AVALON_PIO_DATA(UPM_PIO_BASE, input);

	usleep(200000);

	// and to high again :)
	// then the upm should stop flashing
	input = 1;
	IOWR_ALTERA_AVALON_PIO_DATA(UPM_PIO_BASE, input);

	usleep(200000);

	// for keeping the state of being enabled cal has to be low??
	input = 0;
	IOWR_ALTERA_AVALON_PIO_DATA(UPM_PIO_BASE, input);
}


void upm_enable(){
	// AMP_Calibrate has to be low the whole procedure
	// AMP_ENABLE has to be high for 0.5 sec
	int input = 2;
	IOWR_ALTERA_AVALON_PIO_DATA(UPM_PIO_BASE, input);
	usleep(500000);

	// AMP_Enable has to be low for 0.2 sec
	input = 0;
	IOWR_ALTERA_AVALON_PIO_DATA(UPM_PIO_BASE, input);
	usleep(200000);

	// and high again
	input = 2;
	IOWR_ALTERA_AVALON_PIO_DATA(UPM_PIO_BASE, input);
}


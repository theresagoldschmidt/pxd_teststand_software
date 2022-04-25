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
 *         LTC1257 for the inverted pendulum.
 *
 * \author
 *         Nadine Keddis <keddis@fortiss.org>
 */

/*
 * ltc1257.c
 * This file contains the functions to transmit data via SPI.
 *
 *  Created on: 09.06.2010
 *      Author: keddis
 */

#include "ltc1257.h"
#include "altera_avalon_spi.h"
#include "system.h"
#include "altera_avalon_pio_regs.h"


alt_u8 ltc1257_initialized = 0;

void ltc1257_init(void){
	ltc1257_initialized = 1;
}

void ltc1257_done(void){
	// TODO
}

void ltc1257_transmit_data(alt_u16 data){
	if(!ltc1257_initialized){
		return;
	}

	/*
	 * The SPI sends 8 bit at a time, so the data has to
	 * be split into two packets of 8 bit each.
	 */
	const alt_u8 write_data = data >> 8;
	const alt_u8 write_data1 = data & 0xff;
	alt_u32 flag =  ALT_AVALON_SPI_COMMAND_MERGE | ALT_AVALON_SPI_COMMAND_TOGGLE_SS_N;
	alt_avalon_spi_command(SPI_CORE_BASE, 0, sizeof(write_data), &write_data, 0, (alt_u8*)0, flag);
	alt_avalon_spi_command(SPI_CORE_BASE, 0, sizeof(write_data1), &write_data1, 0, (alt_u8*)0, flag);
	/*
	 * The \Load pin has to be pulled low for some time and then pulled high again
	 * The signals are inverted because the optocoupler inverts them again
	 */
	int count = 0;

	while(count < 1000) //1.4+ ms for 10000
		count++;
	IOWR_ALTERA_AVALON_PIO_DATA(HSMC_PIO_BASE, 9);
	count = 0;
	while(count < 1000)
		count++;
	IOWR_ALTERA_AVALON_PIO_DATA(HSMC_PIO_BASE, 8);
}


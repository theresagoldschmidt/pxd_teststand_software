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
 * dac.h
 *
 *  Created on: 09.06.2010
 *      Author: keddis
 */

#ifndef DAC_H_
#define DAC_H_

#include "alt_types.h"

/*
 * initializes the ltc1257 and sets voltage to 0V
 */
void dac_init(void);


/*
 * Shuts the ltc1257 down and sets voltage to 0V
 */
void dac_done(void);

/*
 * voltage is the desired output of the dac in mV!!
 * => range: -10000 ... 10000 mV
 */
void dac_set_voltage(alt_16 voltage);

#endif /* DAC_H_ */

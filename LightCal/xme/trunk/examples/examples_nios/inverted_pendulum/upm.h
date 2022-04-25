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
 * upm.h
 *
 *  Created on: 09.06.2010
 *      Author: keddis
 */

#ifndef UPM_H_
#define UPM_H_

#include "system.h"
#include "altera_avalon_pio_regs.h"
#include <unistd.h>

// Init the Universal Power Module
// this function starts the bootupsequence
void upm_init();

// Enable the Universal Power Module
// this is necessary if the cart reached one of
// the limiting photo sensors
void upm_enable();

#endif /* UPM_H_ */

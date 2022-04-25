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
 * ltc1257.h
 *
 *  Created on: 09.06.2010
 *      Author: keddis
 */

#ifndef LTC1257_H_
#define LTC1257_H_

#include "alt_types.h"

// Initialize da_converter ltc1257
// the only thing to be done is setting up spi
void ltc1257_init(void);

// Shuts down the ltc1257
// This currently is a NOP
void ltc1257_done(void);

// Send Data to the dac and enable it
// only bit 0 - bit 11 are used
void ltc1257_transmit_data(alt_u16 data);

#endif /* LTC1257_H_ */

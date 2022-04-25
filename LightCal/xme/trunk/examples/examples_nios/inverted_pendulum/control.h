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
 * control.h
 *
 *  Created on: 08.06.2010
 *      Author: keddis
 */

#ifndef CONTROL_H_
#define CONTROL_H_

#include "alt_types.h"

void control_init(void);

alt_32 control_cart(alt_32 *x_d);

#endif /* CONTROL_H_ */

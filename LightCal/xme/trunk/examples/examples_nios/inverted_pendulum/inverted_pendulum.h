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
 *         Component for the inverted pendulum.
 *
 * \author
 *         Kai Huang <khuang@fortiss.org>
 */

/*
 * inverted_pendulum.h
 *
 *  Created on: 2012-2-13
 *      Author: khuang
 */

#ifndef INVERTED_PENDULUM_H_
#define INVERTED_PENDULUM_H_

#define AMPLITUDE 2000
#define MAX_AMPLITUDE 55000000
#define MAX_MV 500

#define XME_CORE_STATUS_SUCCESS 0

#define xme_core_status_t int

typedef struct    {

} xme_inverted_pendulum_configStruct_t;

xme_core_status_t xme_inverted_pendulum_create(xme_inverted_pendulum_configStruct_t* config);
xme_core_status_t xme_inverted_pendulum_activate(xme_inverted_pendulum_configStruct_t* config);
void xme_inverted_pendulum_deactivate(xme_inverted_pendulum_configStruct_t* config);
void xme_inverted_pendulum_destroy(xme_inverted_pendulum_configStruct_t* config);
void xme_inverted_pendulum_task(void* data);

#endif /* INVERTED_PENDULUM_H_ */

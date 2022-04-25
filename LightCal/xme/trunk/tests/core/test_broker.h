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
 *         Broker testsuite.
 *
 * \author
 *         Stephan Sommer <sommer@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/


/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#define TOPIC1_DATA_CHANNEL	4711
#define TOPIC2_DATA_CHANNEL 4811
#define TOPIC3_DATA_CHANNEL 4911
#define TOPIC1_TR_DATA_CHANNEL 4799

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
int
xme_tests_core_broker();

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
 *         Linked list abstraction.
 *
 * \author
 *         Michael Geisinger <geisinger@fortiss.org>
 */

#ifndef XME_HAL_LINKEDLIST_H
#define XME_HAL_LINKEDLIST_H

/**
 * \defgroup hal_linkedList Linked list
 *
 * \brief  Linked list abstraction.
 *
 *         A linked list is a data structure in which each item consists of a
 *         pointer to the next and/or pevious item as well as a payload.
 *         Items can be added to and removed from the list by adjusting the
 *         next/previous pointers.
 *
 * \note   Linked lists are implementated differently depending on the
 *         architecture. This is why linked lists are covered by the hardware
 *         abstraction library. On some target platforms, linked list items
 *         are dynamically allocated and the number of items is only limited
 *         by the available system memory.
 *         On resource constrained platforms without efficient memory
 *         management, a static maximum size (in terms of number of items) is
 *         given for each linked list.
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/

/******************************************************************************/
/***   Platform-specific includes                                           ***/
/******************************************************************************/
#include "xme/hal/linkedList_arch.h"

#endif // #ifndef XME_HAL_LINKEDLIST_H

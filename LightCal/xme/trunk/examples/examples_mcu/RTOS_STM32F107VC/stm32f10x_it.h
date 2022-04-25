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
 *         Interrupt handlers.
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 */


/* Retained copyright */
/*
  ******************************************************************************
  * file    stm32f10x_it.h
  * author  MCD Application Team
  * version V1.0.0
  * date    07/16/2010
  * brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  * copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

#ifndef STM32F10X_IT_H
#define STM32F10X_IT_H

 /******************************************************************************/
 /***   Includes                                                             ***/
 /******************************************************************************/
#include "stm32f10x.h"

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void DebugMon_Handler(void);

#endif // #ifndef STM32F10x_IT_H

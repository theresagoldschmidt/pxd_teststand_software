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
 *         Board support package: Ethernet pins
 *         (architecture specific part: ARMv7-M, STM32).
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 */
 
#ifndef BSP_PINS_ETHERNET_H
#define BSP_PINS_ETHERNET_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "bsp/bsp_opt.h"

/******************************************************************************/
/***   Definition of Ethernet pins                                          ***/
/******************************************************************************/
#define XME_BSP_STM32_ETHERNET_MDC_PORT             GPIOC
#define XME_BSP_STM32_ETHERNET_MDC_PIN              GPIO_Pin_1

#define XME_BSP_STM32_ETHERNET_RMII_REFCLK_PORT     GPIOA
#define XME_BSP_STM32_ETHERNET_RMII_REFCLK_PIN      GPIO_Pin_1

#define XME_BSP_STM32_ETHERNET_MDIO_PORT            GPIOA
#define XME_BSP_STM32_ETHERNET_MDIO_PIN             GPIO_Pin_2

#if XME_BSP_STM32_ETHERNET_AF_REMAPPED
#define XME_BSP_STM32_ETHERNET_RMII_CRSDV_PORT      GPIOD
#define XME_BSP_STM32_ETHERNET_RMII_CRSDV_PIN       GPIO_Pin_8

#define XME_BSP_STM32_ETHERNET_RMII_RXD0_PORT       GPIOD
#define XME_BSP_STM32_ETHERNET_RMII_RXD0_PIN        GPIO_Pin_9

#define XME_BSP_STM32_ETHERNET_RMII_RXD1_PORT       GPIOD
#define XME_BSP_STM32_ETHERNET_RMII_RXD1_PIN        GPIO_Pin_10
#endif // #if XME_BSP_STM32_ETHERNET_AF_REMAPPED

#if XME_BSP_STM32_ETHERNET_AF_DEFAULT
#define XME_BSP_STM32_ETHERNET_RMII_CRSDV_PORT      GPIOA
#define XME_BSP_STM32_ETHERNET_RMII_CRSDV_PIN       GPIO_Pin_7

#define XME_BSP_STM32_ETHERNET_RMII_RXD0_PORT       GPIOC
#define XME_BSP_STM32_ETHERNET_RMII_RXD0_PIN        GPIO_Pin_4

#define XME_BSP_STM32_ETHERNET_RMII_RXD1_PORT       GPIOC
#define XME_BSP_STM32_ETHERNET_RMII_RXD1_PIN        GPIO_Pin_5
#endif // #if XME_BSP_STM32_ETHERNET_AF_DEFAULT

#define XME_BSP_STM32_ETHERNET_RMII_TXEN_PORT       GPIOB
#define XME_BSP_STM32_ETHERNET_RMII_TXEN_PIN        GPIO_Pin_11

#define XME_BSP_STM32_ETHERNET_RMII_TXD0_PORT       GPIOB
#define XME_BSP_STM32_ETHERNET_RMII_TXD0_PIN        GPIO_Pin_12

#define XME_BSP_STM32_ETHERNET_RMII_TXD1_PORT       GPIOB
#define XME_BSP_STM32_ETHERNET_RMII_TXD1_PIN        GPIO_Pin_13

#define XME_BSP_STM32_ETHERNET_PPSOUT_PORT          GPIOB
#define XME_BSP_STM32_ETHERNET_PPSOUT_PIN           GPIO_Pin_5

#endif // #ifndef BSP_PINS_ETHERNET_H

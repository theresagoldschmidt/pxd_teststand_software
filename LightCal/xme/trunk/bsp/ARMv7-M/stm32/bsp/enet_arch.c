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
 *         Ethernet driver (architecture specific part: ARMv7-M, STM32)
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 */
 
 // TODO: Move board specific pin definitions to BSP header! See ticket #846
// TODO: Join with stm32_eth.c? See ticket #847

 /* retained copyright */
/**
  ******************************************************************************
  * @file    stm32f107.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    07/16/2010 
  * @brief   STM32F107 hardware configuration
  ******************************************************************************
  * @copy
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


/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "stm32_eth.h"
#include "enet_arch.h"
#include "pins_eth.h"

#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "misc.h"

// Options managed by xme_build_option()
#include "bsp/bsp_opt.h"
#include "lwip/opt.h"
#include "network_settings.h"
/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#if (XME_BSP_STM32_ETHERNET_AF_DEFAULT && XME_BSP_STM32_ETHERNET_AF_REMAPPED) || (!XME_BSP_STM32_ETHERNET_AF_DEFAULT && !XME_BSP_STM32_ETHERNET_AF_REMAPPED)
#error "You must choose exactly one AF pin mapping: XME_BSP_STM32_ETHERNET_AF_DEFAULT, or XME_BSP_STM32_ETHERNET_AF_REMAPPED."
#endif

#if (XME_BSP_STM32_ETHERNET_RMII && XME_BSP_STM32_ETHERNET_MII) || (!XME_BSP_STM32_ETHERNET_RMII && !XME_BSP_STM32_ETHERNET_MII)
#error "You must choose exactly one PHY interface: XME_BSP_STM32_ETHERNET_RMII, or XME_BSP_STM32_ETHERNET_MII."
#endif

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
void bsp_eth_gpio_init(void);
void NVIC_Configuration(void);
void Ethernet_Configuration(void);

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
#if XME_BSP_STM32_ETHERNET_RMII
void bsp_eth_gpio_init (void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

#if XME_BSP_STM32_ETHERNET_AF_REMAPPED
	GPIO_PinRemapConfig(GPIO_Remap_ETH, ENABLE);
#endif // #if XME_BSP_STM32_ETHERNET_AF_REMAPPED

#if XME_BSP_STM32_ETHERNET_AF_DEFAULT
	GPIO_PinRemapConfig(GPIO_Remap_ETH, DISABLE);
#endif // #if XME_BSP_STM32_ETHERNET_AF_DEFAULT

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
	                       RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);

	// ETH_MDC
	GPIO_InitStructure.GPIO_Pin = XME_BSP_STM32_ETHERNET_MDC_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(XME_BSP_STM32_ETHERNET_MDC_PORT, &GPIO_InitStructure);

	// ETH_RMII_REF_CLK
	GPIO_InitStructure.GPIO_Pin = XME_BSP_STM32_ETHERNET_RMII_REFCLK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(XME_BSP_STM32_ETHERNET_RMII_REFCLK_PORT, &GPIO_InitStructure);

	// ETH_MDIO
	GPIO_InitStructure.GPIO_Pin = XME_BSP_STM32_ETHERNET_MDIO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(XME_BSP_STM32_ETHERNET_MDIO_PORT, &GPIO_InitStructure);

	// ETH_RMII_CRS_DV
	GPIO_InitStructure.GPIO_Pin = XME_BSP_STM32_ETHERNET_RMII_CRSDV_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(XME_BSP_STM32_ETHERNET_RMII_CRSDV_PORT, &GPIO_InitStructure);

	// ETH_RMII_RXDO
	GPIO_InitStructure.GPIO_Pin = XME_BSP_STM32_ETHERNET_RMII_RXD0_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(XME_BSP_STM32_ETHERNET_RMII_RXD0_PORT, &GPIO_InitStructure);

	// ETH_RMII_RXD1
	GPIO_InitStructure.GPIO_Pin = XME_BSP_STM32_ETHERNET_RMII_RXD1_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(XME_BSP_STM32_ETHERNET_RMII_RXD1_PORT, &GPIO_InitStructure);

	// ETH_RMII_TXEN
	GPIO_InitStructure.GPIO_Pin = XME_BSP_STM32_ETHERNET_RMII_TXEN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(XME_BSP_STM32_ETHERNET_RMII_TXEN_PORT, &GPIO_InitStructure);

	// ETH_RMII_TXD0
	GPIO_InitStructure.GPIO_Pin = XME_BSP_STM32_ETHERNET_RMII_TXD0_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(XME_BSP_STM32_ETHERNET_RMII_TXD0_PORT, &GPIO_InitStructure);

	// ETH_RMII_TXD1
	GPIO_InitStructure.GPIO_Pin = XME_BSP_STM32_ETHERNET_RMII_TXD1_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(XME_BSP_STM32_ETHERNET_RMII_TXD1_PORT, &GPIO_InitStructure);

	// ETH_PPSOUT
	GPIO_InitStructure.GPIO_Pin = XME_BSP_STM32_ETHERNET_PPSOUT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(XME_BSP_STM32_ETHERNET_PPSOUT_PORT, &GPIO_InitStructure);

	// Selected alternate pin function mapping for Ethernet peripheral
	// Needs to be performed after the actual pin configuration!
#if XME_BSP_STM32_ETHERNET_AF_REMAPPED
	GPIO_PinRemapConfig(GPIO_Remap_ETH, ENABLE);
#endif // #if XME_BSP_STM32_ETHERNET_AF_REMAPPED

#if XME_BSP_STM32_ETHERNET_AF_DEFAULT
	GPIO_PinRemapConfig(GPIO_Remap_ETH, DISABLE);
#endif // #if XME_BSP_STM32_ETHERNET_AF_DEFAULT
}

#endif // #if XME_BSP_STM32_ETHERNET_RMII

/**
 * Setting the MAC address.
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
void Set_MAC_Address(eth_mac_addr_t macadd)
{
	ETH_MACAddressConfig(ETH_MAC_Address0, macadd);
}

/**
  * @brief  Setup ethernet peripheral
  * @param  None
  * @retval None
  */
void ETH_driver_init(void)
{
	/* At this stage the microcontroller clock setting is supposed to be
	*  already configured, e.g. through SystemInit() function which is called
	*  from startup file (startup_stm32f10x_xx.s) before to branch to
	*  application main.
	*
	*  To reconfigure the default setting of SystemInit() function, refer to
	*  system_stm32f10x.c file
	*/

  	/* Enable ETHERNET clock  */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ETH_MAC | RCC_AHBPeriph_ETH_MAC_Tx | RCC_AHBPeriph_ETH_MAC_Rx, ENABLE);

	/* Configure the GPIO ports */
	bsp_eth_gpio_init();

	/* NVIC configuration */
	NVIC_Configuration();

	/* Configure the Ethernet peripheral */
	Ethernet_Configuration();
}

#if XME_BSP_STM32_ETHERNET_PHY_STE101P
int bsp_eth_phy_addr (void)
{
	int phyAddr;

	for(phyAddr = 1; 32 >= phyAddr; phyAddr++)
	{
		if((0x0006 == ETH_ReadPHYRegister(phyAddr, 2)) &&
		   (0x1c50 == (ETH_ReadPHYRegister(phyAddr, 3) & 0xFFF0)))
		{
			break;
		}
	}

	return phyAddr;
}
#endif // #if XME_BSP_STM32_ETHERNET_PHY_STE101P

#if XME_BSP_STM32_ETHERNET_PHY_DP83848J
int bsp_eth_phy_addr (void)
{
	return 1;
}
#endif // #if XME_BSP_STM32_ETHERNET_PHY_DP83848J

/**
  * @brief  Configures the Ethernet Interface
  * @param  None
  * @retval None
  */
void Ethernet_Configuration(void)
{
	ETH_InitTypeDef ETH_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

#if XME_BSP_STM32_ETHERNET_MII
	GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_MII);

	/* Get HSE clock = 25MHz on PA8 pin (MCO) */
	RCC_MCOConfig(RCC_MCO_HSE);

#endif // #if XME_BSP_STM32_ETHERNET_MII

#if XME_BSP_STM32_ETHERNET_RMII
	GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_RMII);

#if XME_BSP_STM32_ETHERNET_RMII_PLL3
	/* Configure MCO (PA8) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* Set PLL3 clock output to 50MHz (25MHz /5 *10 =50MHz) */
	RCC_PLL3Config(RCC_PLL3Mul_10);
	/* Enable PLL3 */
	RCC_PLL3Cmd(ENABLE);
	/* Wait till PLL3 is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_PLL3RDY) == RESET)
	{}

	/* Get PLL3 clock on PA8 pin (MCO) */
	RCC_MCOConfig(RCC_MCO_PLL3CLK);
#endif // #if XME_BSP_STM32_ETHERNET_RMII_PLL3
#endif // #if XME_BSP_STM32_ETHERNET_RMII

	/* Reset ETHERNET on AHB Bus */
	ETH_DeInit();

	/* Software reset */
	ETH_SoftwareReset();

	/* Wait for software reset */
	while (ETH_GetSoftwareResetStatus() == SET);

	/* ETHERNET Configuration ------------------------------------------------------*/
	/* Call ETH_StructInit if you don't like to configure all ETH_InitStructure parameter */
	ETH_StructInit(&ETH_InitStructure);

	/* Fill ETH_InitStructure parametrs */
	/*------------------------   MAC   -----------------------------------*/
	ETH_InitStructure.ETH_AutoNegotiation = ETH_AutoNegotiation_Enable  ;
	ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;
	ETH_InitStructure.ETH_RetryTransmission = ETH_RetryTransmission_Disable;
	ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
	ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Enable;
	ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
	ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Disable;
	ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_None;
	ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;

	// TODO: Manage with xme_build_option. See ticket #848
	#ifdef CHECKSUM_BY_HARDWARE
	ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Enable;
	#endif

	/*------------------------   DMA   -----------------------------------*/

	/* When we use the Checksum offload feature, we need to enable the Store and Forward mode:
	the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum,
	if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
	ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
	ETH_InitStructure.ETH_ReceiveStoreForward = ETH_ReceiveStoreForward_Enable;
	ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;

	ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Disable;
	ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Disable;
	ETH_InitStructure.ETH_SecondFrameOperate = ETH_SecondFrameOperate_Enable;
	ETH_InitStructure.ETH_AddressAlignedBeats = ETH_AddressAlignedBeats_Enable;
	ETH_InitStructure.ETH_FixedBurst = ETH_FixedBurst_Enable;
	ETH_InitStructure.ETH_RxDMABurstLength = ETH_RxDMABurstLength_32Beat;
	ETH_InitStructure.ETH_TxDMABurstLength = ETH_TxDMABurstLength_32Beat;
	ETH_InitStructure.ETH_DMAArbitration = ETH_DMAArbitration_RoundRobin_RxTx_2_1;

	/* Configure Ethernet */
	ETH_Init(&ETH_InitStructure, bsp_eth_phy_addr());

	/* Enable the Ethernet Rx Interrupt */
	ETH_DMAITConfig(ETH_DMA_IT_NIS | ETH_DMA_IT_R, ENABLE);
}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;

	/* Set the Vector Table base location at 0x08000000 */
	/*	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0); */
	NVIC_SetVectorTable(XME_HAL_INTERNAL_FLASH_IMAGE_FIRST_PAGE_ADDRESS, 0x0);

	/* 2 bit for pre-emption priority, 2 bits for subpriority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	/* Enable the Ethernet global Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = ETH_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void NVIC_Configuration_Emergency_Bits(void)
{
	NVIC_InitTypeDef NVIC_InitStructure_EXTI;
	EXTI_InitTypeDef EXTI_InitStructure;

	//Connect EXTI0 Line to PC0 OVP Fast pin
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource0);

	// Configure EXTI0 line
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	// Enable and set EXTI0 Interrupt to the lowest priority
	NVIC_InitStructure_EXTI.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStructure_EXTI.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure_EXTI.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure_EXTI.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure_EXTI);

	// Connect EXTI0 Line to PC2 UPS pin
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOC, GPIO_PinSource2);

	// Configure EXTI0 line
	EXTI_InitStructure.EXTI_Line = EXTI_Line2;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	// Enable and set EXTI0 Interrupt to the lowest priority
	NVIC_InitStructure_EXTI.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_InitStructure_EXTI.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure_EXTI.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure_EXTI.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure_EXTI);

	// Connect EXTI13 Line to PC13 pin Therma status
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOD, GPIO_PinSource13);

	// Configure EXT13 line
	EXTI_InitStructure.EXTI_Line = EXTI_Line13;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	// Enable and set EXT13 Interrupt to the lowest priority
	NVIC_InitStructure_EXTI.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_InitStructure_EXTI.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure_EXTI.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure_EXTI.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure_EXTI);
}
/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/

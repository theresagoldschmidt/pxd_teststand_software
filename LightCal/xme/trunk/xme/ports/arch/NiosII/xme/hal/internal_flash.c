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
 *         Data persistence module: Internal flash.
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 */
 
 /* Retained copyright information: */
 /*
  ******************************************************************************
  * file    flash_if.c 
  * author  MCD Application Team
  * version V1.0.0
  * date    07/16/2010 
  * brief   This file provides high level routines to manage internal Flash 
  *          programming (erase and write). 
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

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/internal_flash.h"

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/

xme_core_status_t
xme_hal_internal_flash_init(void)
{ 
	// TODO Error handling. See ticket #721
	//FLASH_Unlock();
	
	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_hal_internal_flash_fini(void)
{ 
	// TODO Error handling. See ticket #721
	//FLASH_Lock();
	
	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_hal_internal_flash_erase(
	xme_hal_persistance_flash_address_t startAddress,
	xme_hal_persistance_flash_address_t endAddress
)
{
	/*
	xme_hal_persistance_flash_address_t flashAddress;

	flashAddress = startAddress;

	while (flashAddress <= (xme_hal_persistance_flash_address_t) endAddress)
	{
		if (FLASH_ErasePage(flashAddress) == FLASH_COMPLETE)
		{
			flashAddress += XME_HAL_INTERNAL_FLASH_PAGE_SIZE;
		}
		else
		{
			return XME_CORE_STATUS_INTERNAL_ERROR;
		}
	}
	*/
	return XME_CORE_STATUS_SUCCESS;
}

xme_core_status_t
xme_hal_internal_flash_write(xme_hal_persistance_flash_address_t* flashAddress, uint32_t* data, uint32_t len)
{
	/*
	uint32_t i = 0;

	for (i=0; i < len; i++)
	{
		if (*flashAddress <= (XME_HAL_INTERNAL_FLASH_USER_END_ADDRESS-4))
		{
			if (FLASH_ProgramWord(*flashAddress, *(uint32_t*)(data + i)) == FLASH_COMPLETE)
			{
				*flashAddress += 4;
			}
			else
			{
				return XME_CORE_STATUS_INTERNAL_ERROR;
			}
		}
		else
		{
			return XME_CORE_STATUS_INVALID_PARAMETER;
		}
	}
	*/
	return XME_CORE_STATUS_SUCCESS;
}

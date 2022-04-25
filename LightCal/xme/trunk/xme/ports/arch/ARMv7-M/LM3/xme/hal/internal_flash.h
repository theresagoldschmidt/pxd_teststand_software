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
  * file    flash_if.h 
  * author  MCD Application Team
  * version V1.0.0
  * date    07/16/2010 
  * brief   Header for flash_if.c module
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

#ifndef XME_HAL_INTERNAL_FLASH_H
#define XME_HAL_INTERNAL_FLASH_H

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include <stdint.h>
#include "xme/core/core.h"

/******************************************************************************/
/***   Typedefs                                                             ***/
/******************************************************************************/
typedef uint32_t xme_hal_persistance_flash_address_t;

/******************************************************************************/
/***   Defines                                                              ***/
/******************************************************************************/
#ifndef XME_HAL_INTERNAL_FLASH_USER_FIRST_PAGE_ADDRESS
#error "You need to specify the first valid address of the user flash area."
#endif

#ifndef XME_HAL_INTERNAL_FLASH_USER_END_ADDRESS
#error "You need to specify the last valid address of the user flash area."
#endif

#define XME_HAL_INTERNAL_FLASH_SIZE (XME_HAL_INTERNAL_FLASH_USER_END_ADDRESS - XME_HAL_INTERNAL_FLASH_USER_FIRST_PAGE_ADDRESS)

#ifndef XME_HAL_INTERNAL_FLASH_PAGE_SIZE
#define XME_HAL_INTERNAL_FLASH_PAGE_SIZE	0x800
#endif

/******************************************************************************/
/***   Prototypes                                                           ***/
/******************************************************************************/
/**
 * \brief Initializes internal flash, i.e. unlocks it for write access.
 *
 * \return Returns one of the following values:
 *         - XME_CORE_STATUS_SUCCESS If flash was successfully unlocked.
 *         - XME_CORE_STATUS_INTERNAL_ERROR On error.
 */
xme_core_status_t xme_hal_internal_flash_init(void);

/**
 * \brief Finalized internal flash, i.e. locks write access.
 *
 * \return Returns one of the following values:
 *         - XME_CORE_STATUS_SUCCESS If flash was successfully locked.
 *         - XME_CORE_STATUS_INTERNAL_ERROR On error.
 */
xme_core_status_t xme_hal_internal_flash_fini(void);

/**
 * \brief Writes a data buffer in flash (data are 32-bit aligned).
 *
 * \param  flashAddress Pointer to start address for writing data buffer.
 *         Side effect: Update \a flashAddress to contain next valid write address.
 *         In case of error, \a flashAddress contains the flash address where the
 *         error occurred.
 * \param  data pointer on data buffer
 * \param  len length of data buffer (unit is 32-bit word)   
 *
 * \return Returns one of the following values:
 *         - XME_CORE_STATUS_SUCCESS If data was successfully written to flash.
 *         - XME_CORE_STATUS_INVALID_PARAMETER If address to be written exceeded
 *           valid flash address space.
 *         - XME_CORE_STATUS_INTERNAL_ERROR On error.
 */
xme_core_status_t xme_hal_internal_flash_write(xme_hal_persistance_flash_address_t* flashAddress, uint32_t* Data, uint32_t len);

/**
 * \brief Erase all user flash.
 *
 * \param startAddress Start address of erase operation (first address to be erased).
 * \param startAddress End address of erase operation (last address to be erased).
 *
 * \return Returns one of the following values:
 *         - XME_CORE_STATUS_SUCCESS If flash was successfully erased.
 *         - XME_CORE_STATUS_INTERNAL_ERROR On error.
 */
xme_core_status_t xme_hal_internal_flash_erase(xme_hal_persistance_flash_address_t startAddress,
                                               xme_hal_persistance_flash_address_t endAddress);

#endif /* XME_HAL_INTERNAL_FLASH_H */

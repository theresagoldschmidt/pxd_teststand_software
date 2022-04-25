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
 *         Math functions (architecture specific part: x86).
 *
 * \author
 *         Simon Barner <barner@fortiss.org>
 *         Michael Geisinger <geisinger@fortiss.org>
 */

/******************************************************************************/
/***   Includes                                                             ***/
/******************************************************************************/
#include "xme/hal/math_arch.h"

/******************************************************************************/
/***   Implementation                                                       ***/
/******************************************************************************/
#ifdef _MSC_VER
__declspec(naked) uint32_t __fastcall
xme_hal_math_ceilPowerOfTwo(uint32_t x)
{
    __asm
	{
        xor eax,eax
        dec ecx
        bsr ecx,ecx
        cmovz ecx,eax
        setnz al
        inc eax
        shl eax,cl
        ret
    }
}

__declspec(naked) uint32_t __fastcall
xme_hal_math_floorPowerOfTwo(uint32_t x) {
    __asm
	{
        xor eax,eax
        bsr ecx,ecx
        setnz al
        shl eax,cl
        ret
    }
}

#elif defined (__GNUC__) /* #ifdef _MSC_VER */
uint32_t
xme_hal_math_ceilPowerOfTwo(uint32_t x)
{
    unsigned eax;
    __asm__
	(
        "xor eax,eax\n\t"
        "dec ecx\n\t"
        "bsr ecx,ecx\n\t"
        "cmovz ecx,eax\n\t"
        "setnz al\n\t"
        "inc eax\n\t"
        "shl eax,cl\n\t"
        : "=a" (eax)
        : "c" (x)
    );
    return eax;
}

uint32_t
xme_hal_math_floorPowerOfTwo(uint32_t x)
{
    unsigned eax;
    __asm__
	(
        "xor eax,eax\n\t"
        "bsr ecx,ecx\n\t"
        "setnz al\n\t"
        "shl eax,cl\n\t"
        : "=a" (eax)
        : "c" (x)
    );
    return eax;
}
#else /* #elif defined (__GNUC__) */
#error "Architecture specific implementation of math functions could not be find. Create a port, or use the generic implementation in arch/gen_c."
#endif /* #elif defined (__GNUC__) */
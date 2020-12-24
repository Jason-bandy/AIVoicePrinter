/******************************************************************
 *                                                                *
 *        Copyright Mentor Graphics Corporation 2004              *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

#ifndef __MUSB_Os_AFS_MEMORY_H__
#define __MUSB_Os_AFS_MEMORY_H__

#include "include.h"
#include "mem_pub.h"

#define MUSB_MemAlloc(a)          os_malloc(a)
#define MUSB_MemRealloc(a,b)      os_realloc(a,b)
#define MUSB_MemFree(a)           os_free(a)

#define MUSB_MemCopy(_pDest, _pSrc, _iSize) \
    os_memcpy((void*)_pDest, (void*)_pSrc, _iSize)

#define MUSB_MemSet(_pDest, _iData, _iSize) \
    os_memset((void*)_pDest, _iData, _iSize)
#endif	/* multiple inclusion protection */

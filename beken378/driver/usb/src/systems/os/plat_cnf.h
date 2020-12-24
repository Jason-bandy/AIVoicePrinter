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

#ifndef __MUSB_Os_PLATFORM_CONFIG_H__
#define __MUSB_Os_PLATFORM_CONFIG_H__


#include <stdlib.h>

/* include board-specific configuration */
#include "brd_cnf.h"

/* First 4 and last 4 task priority are reserved for future use,
 * Highest priority is 0 and lowest priority is 63 */
#define MGC_Os_HIGHEST_PRIORITY       5

#define MGC_Os_LOWEST_PRIORITY        10

#define MGC_Os_SUSPEND            0

/* Maximum number of interrupts, the system provids */
#define MGC_Os_MAX_INTERRUPTS 	4

#endif	/* multiple inclusion protection */

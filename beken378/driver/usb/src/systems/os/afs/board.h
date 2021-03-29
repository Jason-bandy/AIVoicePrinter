/******************************************************************
 *                                                                *
 *        Copyright Mentor Graphics Corporation 2005              *
 *                                                                *
 *                All Rights Reserved.                            *
 *                                                                *
 *    THIS WORK CONTAINS TRADE SECRET AND PROPRIETARY INFORMATION *
 *  WHICH IS THE PROPERTY OF MENTOR GRAPHICS CORPORATION OR ITS   *
 *  LICENSORS AND IS SUBJECT TO LICENSE TERMS.                    *
 *                                                                *
 ******************************************************************/

#ifndef __MUSB_OS_BOARD_H__
#define __MUSB_OS_BOARD_H__

#include "mu_dsi.h"
#include "mu_sys.h"

#include "include.h"
#include "rtos_pub.h"

/* 
 * Define this to log diagnostics to a RAM buffer and upload later with your debugger, etc.
#define MUSB_MSG_BUF
 */

/***************************** TYPES ******************************/

/**
 * @field iVector uHAL's vector for reverse-lookup
 * @field iIndex uHAL's timer index
 * @field pfExpired expiration callback
 * @field pParam expiration callback parameter
 * @field dwTime remaining time, due to uHAL's MAX_PERIOD limitation
 * @field bPeriodic whether currently set for periodic
 */
typedef struct
{
    /* timer implementation, and it depends on operating system*/
    beken2_timer_t timer;

    unsigned int iVector;
    unsigned int iIndex;
    MUSB_pfTimerExpired pfExpired;
    void* pParam;
    uint32_t dwTime;
    uint8_t bPeriodic;

} MGC_AfsTimerWrapper;

/**
 * MGC_AfsUds.
 * Board-specific UDS instance data.
 * @field pfIsr ISR
 * @field pIsrParam parameter to pass controller ISR
 * @field aTimerWrapper timer wrappers
 * @field dwIrq interrupt number
 * @field wTimerCount how many wrappers
 * @field bIndex our index into the global array
 * @field bIsPci TRUE if PCI-based; FALSE otherwise
 */
typedef struct
{
    char aIsrName[8];
    MUSB_pfOsIsr pfIsr;
    void* pIsrParam;
    MGC_AfsTimerWrapper* aTimerWrapper;
    unsigned int dwIrq;
    uint16_t wTimerCount;
    uint8_t bIndex;
    uint8_t bIsPci;
} MGC_AfsUds;

#endif	/* multiple inclusion protection */

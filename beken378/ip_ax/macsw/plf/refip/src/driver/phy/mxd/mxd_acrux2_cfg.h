/*!
* \file    <mxd_acrux2_cfg.h>
* \brief
* \par    Include files
* \par    (C) 2014 Maxscend Technologies Inc.
* \version
* Revision of last commit: $Rev: 19 $  
* Author of last commit  : $Author: maxscend\zhenlin.liu $
* Date of last commit    : $Date: 2014-08-05 15:37:07 +0800 (星期二, 05 八月 2014) $
*
*/
#include "mxd_acrux2_reg.h"
#include "mxd_acrux2_sdk.h"

#ifdef WIN32
#include "..\usb2spi\mxd_porting_silicon.h"
#endif

//#ifndef _BCRUX_CFG_H_
#define _ACRUX2_CFG_H_


#ifdef __GNUC__
#define _MXD_ANDROID_
#elif  WIN32
//#define _MXD_DEV_MATLIB_DLL_
#define _MXD_DEV_
#else
#define _SPRD_DEV_
#endif


#ifndef _MXD_DEV_
#if 0
typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned long int uint32;
typedef unsigned long int  uint64;
#define VOID void
#define MXD_API
#define NULL 0
#define IN
#define OUT
#define CONST const
#endif
#endif

#define TROUT2_RF_BASE_ADDR 0x1000
#define RF_Clbr_Data_LENGTH 258

#ifdef _MXD_DEV_
#include <windows.h>
#include <stdlib.h>
#include "..\usb2spi\mxd_porting_silicon.h"
//#include "..\mxd_porting_silicon.h"
void * ghspi=NULL;

VOID MXD_API OAL_DebugPrint(IN CONST MXD_CHAR* dbgOutput, ...);
#define mxdDbgInfo OAL_DebugPrint
#define mxdDbgInfo1       //
#define mxdDbgInfo2       OAL_DebugPrint

#define hal_rcb_read(regAddr,pRegVal)  MxdSpiReadRcbReg(ghspi,regAddr,(MXD_U32 *)pRegVal)
#define hal_rcb_write(regAddr,regVal)  MxdSpiWriteRcbReg(ghspi,regAddr,regVal)


#define MxdSleep(n) do{\
    MXD_U32 iiii=0;\
    if (n<500)    {\
    /*for(iiii=0; iiii++;iiii<(n*750));;*/ for(iiii=0;iiii<(n*0x003fffff); iiii++);}\
    else    {\
    Sleep(n);    }\
}while (0)

#endif

#ifdef _MXD_DEV_MATLIB_DLL_
//#include <stdafx.h>
#include "..\f320api.h"
#include "..\usb2spilib.h"
void OAL_DebugPrint(IN CONST MXD_CHAR* dbgOutput, ...);
#define mxdDbgInfo(a) OAL_DebugPrint(a)
#define mxdDbgInfo1       //
#define mxdDbgInfo2       //
#define hal_rcb_read(regAddr,pregVal)     Mxd_SpiReadCastorReg(regAddr,(MXD_U32 *)pregVal)
#define hal_rcb_write(regAddr,regVal)    Mxd_SpiWriteCastorReg(regAddr,regVal)
#define trout_reg_read(regAddr,regVal)      Mxd_SpiReadTroutReg(regAddr,regVal)
#define trout_reg_write(regAddr,regVal)     Mxd_SpiWriteTroutReg(regAddr,regVal)
#endif
#if 0
#ifdef _MXD_ANDROID_
#include <linux/delay.h>
extern unsigned int host_read_trout_reg(unsigned int addr);
extern unsigned int host_write_trout_reg(unsigned int val,unsigned int addr);
static void mxd_reg_read(unsigned int reg_addr, unsigned int *preg_data)
{
    *preg_data = host_read_trout_reg(reg_addr<<2);
}
static void  mxd_reg_write(unsigned int reg_addr, unsigned int reg_data)
{
    host_write_trout_reg(reg_data, (reg_addr<<2));
}

#define hal_rcb_read(regAddr,pRegVal)   do{\
    MXD_U32 nAddr=(MXD_U32)regAddr;\
    MXD_U32 val=0;\
    mxd_reg_read(nAddr+TROUT2_RF_BASE_ADDR,&val);\
    *pRegVal = val; \
}while (0)

#define hal_rcb_write(regAddr,regVal)    do{\
    MXD_U32 nAddr=(MXD_U32)regAddr;\
    MXD_U32 nVal=(MXD_U32)regVal;\
    mxd_reg_write(nAddr+TROUT2_RF_BASE_ADDR,nVal);\
}while (0)



#define mxdDbgInfo                              printk
#define mxdDbgInfo1       //
#define mxdDbgInfo2       //
//#define loftMxdDbgInfo    //
//delay us
//#define MxdSleep     udelay
#define MxdSleep(n)     udelay(n*100)
#endif

// for minicode
#ifdef _SPRD_DEV_
extern void             SCI_Sleep(MXD_U32 ms);
extern void             rf_reg_read(MXD_U32 reg_addr, MXD_U32 *reg_data);
extern void             rf_reg_write(MXD_U32 reg_addr, MXD_U32 reg_data);
extern MXD_U32     trout_read_reg(MXD_U32 reg_addr, MXD_U32 *reg_data);
extern MXD_U32     trout_write_reg(MXD_U32 reg_addr, MXD_U32 reg_data);
extern MXD_U32     SCI_TraceLow(const char *x_format, ...);
#define trout_reg_write trout_write_reg


//debug control
#define mxdDbgInfo      SCI_TraceLow
#define mxdDbgInfo1       SCI_TraceLow
#define mxdDbgInfo2       //
#define _btLoftMxdDbgInfo    //
#define _btTxImbMxdDbgInfo    //

#define MxdSleep           SCI_Sleep


#endif
#endif
/*dcoc auto or manual switch*/
#define _DCOC_WIFI_MANUAL_
//#define _THREE_SECTION_TBL_
//#define _WIFI_TX_5dB_STEP_

/**********debug info switch**********/
#define _DEBUG_INFO_     0     // 1:print info ,0 close info
#if 0
#define _WfImbInfo             mxdDbgInfo
#define _WfLoftInfo             mxdDbgInfo
#define _WfDcocInfo            mxdDbgInfo
#else
#define _WfImbInfo(fmt, ...)
#define _WfLoftInfo(fmt, ...)
#define _WfDcocInfo(fmt, ...)
#endif
/* config Macro */

/*
bttx gain table: 0x800 ~ 0x817
btrx RF gain table: 0x818 ~ 0x8e7
btrx IF gain table: 0x8e8 ~ 0x8ff
*/

//#define DCOC_Arithmetic
#define DCOC_TIA 1
#define DCOC_PGA 0


#ifndef _btDcocMxdDbgInfo
#define _btDcocMxdDbgInfo    mxdDbgInfo//
#endif

#ifndef _btLoftMxdDbgInfo
#define _btLoftMxdDbgInfo    mxdDbgInfo//
#endif

#ifndef _btTxImbMxdDbgInfo
#define _btTxImbMxdDbgInfo    mxdDbgInfo//
#endif

#ifndef _btTxPoutMxdDbgInfo
#define _btTxPoutMxdDbgInfo    mxdDbgInfo//
#endif
//1  *Mode Control Logic */
#define __POR_RESET_VALUE__ rf_reg_write(0x4BE0,0x0);
#define __PRESET_MODE__ rf_reg_write(0x4BE4,0x0);
#define __FUNCTION_MODE__ rf_reg_write(0x4FE4,0x0);
#define __SLEEP_MODE__ rf_reg_write(0x47C0,0x0);
#define __POWER_DOWN__ rf_reg_write(0x4784,0x0);
#define __DFT_MODE__ rf_reg_write(0x5FE0,0x0);
#define __DFT_COMPRESSION_MODE__ rf_reg_write(0x7FE0,0x0);
#define __TEST0_MODE__ rf_reg_write(0x57D4,0x0);
#define __TEST1_MODE__ rf_reg_write(0x57D4,0x0);
#define __TEST2_MODE__ rf_reg_write(0x57D4,0x0);
#define __TEST3_MODE__ rf_reg_write(0x5554,0x0);

//#endif /*end  <mxd_bcrux_cfg.h>*/

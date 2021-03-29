/*
* \file    <mxd_acrux2_sdk.h>
* \brief
* \par    Include files
* \par    (C) 2014 Maxscend Technologies Inc.
* \version
* Revision of last commit: $Rev: 36 $
* Author of last commit  : $Author: maxscend\yanping.wang $
* Date of last commit    : $Date: 2014-08-14 18:59:01 +0800 (星期? 14 八月 2014) $
*
*/

#ifndef _ACRUX2_SDK_H_
#define _ACRUX2_SDK_H_

#ifdef __cplusplus
extern "C"{
#endif

#ifndef __USE_32BIT_PROCESSOR__
#define __USE_32BIT_PROCESSOR__
typedef unsigned char MXD_U8, *PMXD_U8;
typedef unsigned short MXD_U16, *PMXD_U16;
typedef unsigned int MXD_U32, *PMXD_U32;
typedef signed char MXD_S8, *PMXD_S8;
typedef signed short MXD_S16, *PMXD_S16;
typedef signed int MXD_S32, *PMXD_S32;
typedef signed char MXD_BOOL;
typedef char MXD_CHAR;
#endif

//#define __MATLAB_DLL__

#ifdef ACRUXSDKDLL_EXPORTS    
/* This macro defined in Project>> Preprocessor */
#define MXD_API_C  __declspec(dllexport)
#else

#ifndef __MATLAB_DLL__
#define MXD_API_C
#else
// Matlab Please Enable This Macro Here!!!
#define MXD_API_C __declspec(dllimport)
#endif

#endif

#ifdef __MATLAB_DLL__
#define mxdDbgInfo 
#endif



typedef struct adjust_reg_t
{
  unsigned int flag;     //
//    unsigned int len;
//    unsigned int ver;               //  version
//    unsigned int dacScaleCh[14];
//    unsigned int btLoft[4];         //sel clbr
//    unsigned int poutSelfChk[18];     
//    unsigned int btImb[2];            //sel clbr
//    unsigned int btPaTbl[6];         //sel clbr
//    unsigned int btPadTbl[6];         //sel clbr
//    unsigned int btDcoc[32];        //sel clbr
//    unsigned int reserved[8]; //user costom    
} ADJUST_REG_T, *P_ADJUST_REG_T, CLBR_PARA_S, *P_CLBR_PARA_S;
// function
MXD_API_C int MxdRfInit(void);
MXD_API_C int MxdRfSetBtMode(unsigned short flagWorkMode) ;
MXD_API_C int MxdRfSetWifiMode(unsigned short flagEn1Dis0);    // flag =1 work  0:idle
MXD_API_C MXD_U32 MxdRfModeCfg(MXD_U32 modeFlags);
MXD_API_C int MxdRfSetFreqMHzBt(unsigned int freqMzh);
MXD_API_C int MxdRfSetFreqChWifi(unsigned int freqCh);
MXD_API_C void acrux2TxEnableNco(int flagModeFreq);
MXD_API_C void acrux2TxDisableNco(void);
MXD_API_C int MxdRfSetTxGainBt(unsigned int gainId);
MXD_API_C int MxdRfSetRxGainBt(unsigned int gainId);
MXD_API_C int MxdRfSetTxGainWifi(unsigned int gainId);
MXD_API_C int MxdRfSetRxGainWifi(unsigned int gainId);
MXD_API_C MXD_U32 acrux2ClbrDcocWifiManual(void);
MXD_API_C MXD_U32 acrux2ClbrDcocBt(void);
MXD_API_C MXD_U32 acrux2ClbrLoftWifi(void);
MXD_API_C MXD_U32 acrux2ClbrLoftBt(void);
MXD_API_C MXD_U32 acrux2ClbrTxImbWifi(void);
MXD_API_C MXD_U32 acrux2ClbrTxImbBt(void);
MXD_API_C MXD_U32 acrux2ClbrPoutWifi(void);
MXD_API_C MXD_U32 acrux2ClbrPoutBt(void);
MXD_API_C MXD_U32 acrux2ClbrRximbBt(void);
MXD_API_C MXD_U32 acrux2ClbrRximbWifi(void);
MXD_API_C void MxdRfWorkModeSel(unsigned short flagBB_MXD);
MXD_API_C void MxdRfGainModeSel(unsigned short flagBB_MXD);
MXD_API_C void  acrux2WorkInit(void);
MXD_API_C int MxdTestDll(int numberA, int numberB );
MXD_API_C int MxdOpenDevice(void);
MXD_API_C int MxdCloseDevice(void);
MXD_API_C MXD_U32 MxdRbusRead(MXD_U32 Addr);
MXD_API_C void MxdRbusWrite(MXD_U32 Addr,MXD_U32 Value);


MXD_U32 acrux2LoftTwoDimensionalSrarch(MXD_U32 defaultVal, MXD_U32 CalId);
void rf_reg_read(MXD_U32 regAddr, MXD_U32 * pRegVal);
void rf_reg_write(MXD_U32 regAddr, MXD_U32 regVal);
void _rf_reg_write(MXD_U32 regAddr, MXD_U32 regVal);
int MxdRfGetClbrPara(P_CLBR_PARA_S pRegVal) ;
int MxdRfSetClbrPara(P_CLBR_PARA_S pRegVal);
void MxdRfPulse(MXD_S32  sIntRssiDb, MXD_U32 rate1M ,MXD_U32 rate2_0_5M);

void acrux2HostBtRxInit(void);
void acrux2BtRxGainTbl();
void acrux2HostBtTxInit(void);
void acrux2HostWifiTxInit(void);
void acrux2HostWifiRxInit(void);
void acrux2BtTxGainTbl(void);

int acrux2SetFreqMhzBt(MXD_U32 freqMHz);
int acrux2SetFreqChWifi(MXD_U32 );
int acrux2SetFreqKhzFm(MXD_U32 );
void acrux2ClbrDcocFm();
void MxdPowerDown(MXD_U16 flags);

void acrux2ClbrTxImbSearch(MXD_U16*,MXD_U16*);
void acrux2DbgGetRfInfo();
MXD_U16 regBitsGet(MXD_U16 regAddr,MXD_U8 bitH, MXD_U8 bitL);
void acrux2LoInit(void);
void acrux2EnableStdMode(MXD_U32);
void  acrux2TxEnableStd(MXD_U32 flagModeFreq);
void acrux2ClbrDcocBtSetMem(MXD_U16 flags, MXD_U16 gianId, MXD_U16 Vdcoc);
int acrux2SetBtFreqInit(void);
void acrux2DbgGetRegInfo(void);
void acrux2ClbrAutoSetPdetGain(MXD_U32 calId);
MXD_U32 acrux2WifiImbGetStd(MXD_U32 STDAvgTime);
MXD_U32 getRegVal(MXD_U32 regAddr);
MXD_U32 acrux2ReadTmpr(void);
int acrux2WifiImbGetDc(MXD_U32 STDAvgTime);
MXD_U32 acrux2ImbCalIdRecovery(MXD_U32 index);
MXD_U32 acrux2ChangeIndex();
void *gpDevice;

#ifndef _BIT0
#define _BIT0  0
#define _BIT1  1
#define _BIT2  2
#define _BIT3  3
#define _BIT4  4
#define _BIT5  5
#define _BIT6  6
#define _BIT7  7
#define _BIT8  8
#define _BIT9  9
#define _BIT10 10
#define _BIT11 11
#define _BIT12 12
#define _BIT13 13
#define _BIT14 14
#define _BIT15 15
#define _BIT16 16
#define _BIT17 17
#define _BIT18 18
#define _BIT19 19
#define _BIT20 20
#define _BIT21 21
#define _BIT22 22
#define _BIT23 23
#define _BIT24 24
#define _BIT25 25
#define _BIT26 26
#define _BIT27 27
#define _BIT28 28
#define _BIT29 29
#define _BIT30 30
#define _BIT31 31
#endif

// MAX macro
#define _MAX(a,b) ((a)>(b)?(a):(b))
// MIN macro
#define _MIN(a,b) ((a)<(b)?(a):(b))
// ABS macro
#define _ABS(a) ( (a)<0?((a)*(-1)):(a))
// Bit Set
#define _BS(a,b)  (a) = (a) | (1<<(b))
// Bit Clear
#define _BC(a,b)  (a) = (a) & ( ~(1<<(b)) )
// BitS Clear
#define _BSC(x,hb,lb) (x) =  (x) &  (  ~( (2<<(hb))-(1<<(lb)) )  )

// Bits Get
#define _BSG(x,hb,lb) ( (   (x)&( ((2<<(hb)) -(1<<(lb) )    )  ))           >>(lb))
// BitS Set
#define _BSS(x,hb,lb,val)  _BSC(x,hb,lb);\
    (x) = (x) | (  ( (2<<(hb))-(1<<(lb)) ) & ((val&((1<<(hb-lb+1))-1))<<(lb))  )
#define _REGBS(reg,bn)      \
    do{ \
    MXD_U32 regVal = 0; \
    rf_reg_read( reg,  &regVal ); \
    _BS(regVal,bn); \
    rf_reg_write(reg, regVal ); \
    /* mxdDbgInfo("[mxd] ::  _REGBS [%x,%x] \r\n",reg,regVal); */\
    }while(0)

// RF_REG Bit clean
#define _REGBC(reg,bn)      \
    do{ \
    MXD_U32 regVal = 0; \
    rf_reg_read( reg,  &regVal ); \
    _BC(regVal,bn); \
    rf_reg_write(reg, regVal ); \
    /*mxdDbgInfo("[mxd] ::  _REGBC [%x,%x] \r\n",reg,regVal); */ \
    }while(0)

// RF_REG Bits set
#define _REGBSS(reg_x,hb,lb,val) \
    do{ \
    MXD_U32 regVal = 0; \
    rf_reg_read( reg_x,  &regVal ); \
    _BSS(regVal,hb,lb,val); \
    rf_reg_write(reg_x, regVal ); \
    }while(0)

//wait until (reg.bitn==1)
#define _REG_CHK(reg,bitn) \
{\
    MXD_U32 regVal = 0; \
    do{ \
    rf_reg_read( reg,  &regVal ); \
    }while(0==(regVal&(1<<bitn)));\
}
#define _REG_CHK2(reg,bitn) \
{\
    MXD_U32 regVal = 0; \
    MXD_U32 maxCheck = 0;\
    do{ \
    rf_reg_read( reg,  &regVal ); \
    }while((0==(regVal&(1<<bitn)))&&((maxCheck++)<500));\
    if(maxCheck==500) mxdDbgInfo("[mxd] [%s]_REG_CHK2#######[%x].bit%d check=%d \r\n",__FUNCTION__,regVal,bitn, maxCheck);\
}
#define _REG_CHANGE(reg,val) \
{\
    MXD_U32 regVal = 0, tmp=val;\
    rf_reg_read( reg,  &regVal ); \
    rf_reg_write( reg,  val ); \
    mxdDbgInfo("[mxd] _REG_CHK2[%x] =0x%04x -> %04x\r\n",reg,regVal,tmp );\
}
#define _DBG_TRACE_REGS_(addr,val) \
        {\
        mxdDbgInfo("[mxd] [0x%04x,0x%04x] %s %d %s %s\r\n",addr,val,__FILE__,__LINE__,__TIME__, __DATE__);\
        }
#define _FLAG_MODE_BT 0
#define _FLAG_MODE_WIFI 1
#define _FLAG_FREQ_0K  2
#define _FLAG_FREQ_500K 0
#define _FLAG_FREQ_1M   1
#define _FLAG_FREQ_2M   3  

        //#define _TROUT_ANT_SEL_REG_   0x58

        //#define   _rfStdStart        _REGBS(bgn, _BIT0);\
        //do{ \
        //  MXD_U32  regVal=0; \
        //  rf_reg_read(1, &regVal);\
        // }while(0); //0x0015


#define   _rfLoftStdStart        _REGBS(txcal_search_bgn, _BIT1);\
do{ \
  MXD_U32  regVal=0; \
  rf_reg_read(1, &regVal);\
 }while(0); //0x0161

/*
  frxp_fm_adc_ctrl_reg.
  bit11=0     0: convert true code to complementary code    1: use fm adc output data directly
*/
        //#define   _rfFmAdcOutCmpl     _REGBC(brxp_aip_ctrl_reg,_BIT3)   //0x0032
        //#define   _rfFmAdcOutDrct      _REGBS(brxp_aip_ctrl_reg,_BIT3)

/*bit[11:9]] antiSel_wifi_TX*/
//for SWITCH_PA_LNA_8025

        //#define     _BIT_BT_LOFT_POSI  (6)

#ifdef __cplusplus
   }
#endif

#endif /* _MXD_BCRUX_SDK_H_ */

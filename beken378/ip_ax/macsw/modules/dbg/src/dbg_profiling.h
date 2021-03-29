/**
 ****************************************************************************************
 * @file dbg_profiling.h
 *
 * @brief Declaration of the profiling signals.
 *
 * Copyright (C) RivieraWaves 2011-2020
 *
 ****************************************************************************************
 */
#ifndef _DBG_PROFILING_H_
#define _DBG_PROFILING_H_

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "rwnx_config.h"

/**
 *****************************************************************************************
 * @defgroup DBG_PROF DBG_PROF
 * @ingroup DEBUG
 * @brief SW profiling debug feature.
 * This feature is used to allow tracing the real-time operation of the SW using GPIO like
 * signals. The signals can then be recorded using an internal (if implemented) or
 * external logic analyzer.
 *
 * @{
 *****************************************************************************************
 */
/**
 * @name Profiling Switches definitions
 * @{
 *****************************************************************************************
 */
/// Profiling switch definition
#if NX_PROFILING_ON
#define TX_BASIC_PROF_DEF
#define RX_BASIC_PROF_DEF
#define MM_BASIC_PROF_DEF
#define TX_AC_PROF_DEF
//#define MSG_BASIC_PROF_DEF
//#define SLEEP_PROF_DEF
//#define TDLS_PROF_DEF
//#define ANT_DIV_PROF_DEF
#if NX_FULLY_HOSTED
#define RTOS_TASK_DEF
#define SLEEP_PROF_DEF
#endif
#if (NX_TX_FRAME)
#define TX_FRAME_PROF_DEF
#endif

#if (NX_RADAR_DETECT)
//#define RADAR_PROF_DEF
#endif

#if (NX_BEACONING || NX_POWERSAVE || NX_CONNECTION_MONITOR || NX_MULTI_ROLE)
#define TBTT_PROF_DEF
#endif

#if (NX_AMPDU_TX)
#define AGG_BASIC_PROF_DEF
#endif

#if (RW_MUMIMO_TX_EN)
//#define MUMIMO_PROF_DEF
#endif

#if (NX_CHNL_CTXT)
//#define CHNL_CTXT_PROF_DEF
#endif //(NX_CHNL_CTXT)

#if NX_HE
#define TX_HE_PROF_DEF
#endif // NX_HE

#if (NX_P2P)
//#define P2P_PROF_DEF
#endif //(NX_P2P)

#if (NX_DPSM)
//#define DPSM_PROF_DEF
#endif

#if (NX_TD)
//#define TD_PROF_DEF
#endif //(NX_TD)

#if (NX_BW_LEN_ADAPT)
//#define BW_DROP_PROF_DEF
#endif

#if (RW_BFMER_EN)
//#define BFR_PROF_DEF
#endif //(RW_BFMER_EN)

#if (NX_UMAC_PRESENT)
//#define PS_PEER_PROF_DEF
//#define DUPLI_PROF_DEF
//#define IPCDESC_PROF_DEF
//#define DEFRAG_PROF_DEF
//#define RC_ALGO_PROF_DEF
#if (NX_AMPDU_RX)
//#define REORD_PROF_DEF
#endif

#if (RW_UMESH_EN)
//#define MESH_PS_PROF_DEF
#endif //(RW_UMESH_EN)

#endif

#endif

#ifdef SLEEP_PROF_DEF
#define CPU_SLEEP_DEF
#define DEEP_SLEEP_DEF
#endif

// Macros to enable/disable SW basic TX profiling bits individually
#ifdef TX_BASIC_PROF_DEF
#if !NX_FULLY_HOSTED
#define TX_IPC_IRQ_DEF
//#define TX_CFM_DMA_IRQ_DEF
#define TX_BUF_ALLOC_DEF
#define TX_DMA_IRQ_DEF
#define TX_BUF_FREE_DEF
#endif
#define TX_MACIF_EVT_DEF
#define TX_NEW_TAIL_DEF
#define TX_MAC_IRQ_DEF
#define TX_PAYL_HDL_DEF
#define TX_CFM_EVT_DEF
#endif

// Macros to enable/disable access category profiling bits individually
#ifdef TX_AC_PROF_DEF
#define TX_AC_BG_DEF
#define TX_AC_IRQ_DEF
#endif

// Macros to enable/disable TX HE profiling bits individually
#ifdef TX_HE_PROF_DEF
#define TX_HE_TRIG_IRQ_DEF
#endif

// Macros to enable/disable TX frame profiling bits individually
#ifdef TX_FRAME_PROF_DEF
#define TX_FRAME_PUSH_DEF
#define TX_FRAME_CFM_DEF
#endif

// Macros to enable/disable SW basic RX profiling bits individually
#ifdef RX_BASIC_PROF_DEF
#define RX_MAC_IRQ_DEF
#define RX_DMA_IRQ_DEF
#define RX_DMA_EVT_DEF
#define RX_MPDU_XFER_DEF
#define RX_MPDU_FREE_DEF
#define RX_CNTRL_EVT_DEF
#if !NX_FULLY_HOSTED
#define RX_IPC_IND_DEF
#endif
#endif

#ifdef TBTT_PROF_DEF
//#define HW_TBTT_EVT_DEF
#if NX_POWERSAVE || NX_CONNECTION_MONITOR || NX_MULTI_ROLE
#define STA_TBTT_DEF
//#define AP_TBTT_DEF
//#define TBTT_IDX_DEF
#endif
#endif

#ifdef MM_BASIC_PROF_DEF
#define MM_HW_IDLE_DEF
#define MM_SET_CHANNEL_DEF
#endif

#ifdef DPSM_PROF_DEF
#define PS_SLEEP_DEF
#define PS_PAUSE_DEF
#define PS_DPSM_UPDATE_DEF
#define PS_CHECK_RX_DEF
#define PS_CHECK_TX_DEF
#define PS_CHECK_BCN_DEF
#endif //(DPSM_PROF_DEF)

#ifdef TD_PROF_DEF
#define TD_CHECK_RX_DEF
#define TD_CHECK_TX_DEF
//#define TD_CHECK_RX_PS_DEF
//#define TD_CHECK_TX_PS_DEF
#define TD_TIMER_END_DEF
#endif //(TD_PROF_DEF)

#ifdef BFR_PROF_DEF
#define BFR_SU_CALIB_DEF
#define BFR_MU_CALIB_DEF
#define BFR_RX_BFR_DEF
#define BFR_UPLOAD_DEF
#define BFR_DOWNLOAD_DEF
#define BFR_TX_IND_DEF
#define BFR_TX_CFM_DEF
#define BFR_SMM_IDX_DEF
#endif //(BFR_PROF_DEF)

#ifdef DUPLI_PROF_DEF
#define RX_DUPLI_CHECK_DEF
#define RX_DUPLI_NSTA_CHECK_DEF
#endif

#ifdef IPCDESC_PROF_DEF
#define IPCDESC_PREPARE_DEF
#define IPCDESC_TRANSFER_DEF
#endif

#ifdef DEFRAG_PROF_DEF
#define DEFRAG_CHECK_DEF
#define DEFRAG_TRANSFER_DEF
#define DEFRAG_UPD_LENGTH_DEF
#endif

#ifdef REORD_PROF_DEF
#define REORD_CHECK_DEF
#define REORD_BAR_CHECK_DEF
#define REORD_FLUSH_DEF
#define REORD_FWD_DEF
#endif

// Macros to enable/disable SW basic AGG profiling bits individually
#ifdef AGG_BASIC_PROF_DEF
#if !NX_FULLY_HOSTED
#define AGG_FIRST_MPDU_DWNLD_DEF
#endif
#define AGG_START_AMPDU_DEF
#define AGG_ADD_MPDU_DEF
//#define AGG_SMPDU_DONETX_DEF
#define AGG_FINISH_AMPDU_DEF
#define AGG_BAR_DONETX_DEF
#define AGG_BA_RXED_DEF
#endif

// Macros to enable/disable MUMIMO profiling bits individually
#ifdef MUMIMO_PROF_DEF
#define MU_SEC_USER_IRQ_DEF
#define MU_PPDU_START_DEF
#define MU_MPDU_ADD_DEF
#define MU_PPDU_CLOSE_DEF
#define MU_USER_POS_DEF
#define MU_USER_POS_IRQ_DEF
#endif

#ifdef CHNL_CTXT_PROF_DEF
#define CHAN_CTXT_CDE_EVT_PROF_DEF
//#define CHAN_CTXT_TBTT_SWITCH_PROF_DEF
#define CHAN_CTXT_IDX_PROF_DEF
//#define CHAN_CTXT_TX_DISCARD_PROF_DEF
#define CHAN_CTXT_WAIT_END_PROF_DEF
//#define CHAN_CTXT_SWITCH_PROF_DEF
#define CHAN_CTXT_TBTT_PRES_PROF_DEF
#endif

#ifdef P2P_PROF_DEF
#define P2P_NOA_ABS_DEF
#define P2P_CTW_DEF
#define P2P_WAIT_BCN_DEF
#define P2P_ABSENCE_DEF
#define P2P_PS_PAUSED_DEF
#endif

#ifdef RADAR_PROF_DEF
#define RADAR_IRQ_DEF
#endif

#ifdef PS_PEER_PROF_DEF
#define PS_PEER_STATE_DEF
#define PS_BCMC_STATE_DEF
#define PS_STATE_VAL_DEF
#define PS_PSPOLL_RX_DEF
#define PS_TRAFFIC_REQ_DEF
#endif //(PS_PEER_PROF_DEF)

// Macros to enable/disable SW basic AGG profiling bits individually
#ifdef BW_DROP_PROF_DEF
#define BW_DROP_IRQ_DEF
#define BW_DROP_STEP_DEF
#endif

// Macros to enable/disable rate control algorithm profiling bits individually
#ifdef RC_ALGO_PROF_DEF
#define RC_UPD_COUNTERS_DEF
#define RC_UPD_COUNTERS_TRIAL_DEF
#define RC_STATS_CALC_DEF
#define RC_UPD_RETRY_CHAIN_DEF
#define RC_UPD_RETRY_CHAIN_TRIAL_DEF
#define RC_SET_TRIAL_BUFFER_DEF
#define RC_LOOKAROUND_TX_DEF
#define RC_SW_RETRY_DEF
#endif

#ifdef FT_PROF_DEF
#define FT_OVER_DS_REQ_DEF
#define FT_REASSOC_DEF
#endif

#ifdef MESH_PS_PROF_DEF
#define MESH_PS_ENABLE_DEF
#define MESH_PS_SP_OWNER_DEF
#define MESH_PS_SP_RECIP_DEF
#define MESH_PS_WAIT_BCMC_DEF
#define MESH_PS_WAIT_BCN_DEF
#define MESH_PS_LOCAL_MAW_DEF
#define MESH_PS_PEER_MAW_DEF
#endif //(MESH_PS_PROF_DEF)

#ifdef TDLS_PROF_DEF
//#define TDLS_CHSW_REQ_RX_DEF
//#define TDLS_CHSW_RESP_TX_DEF
//#define TDLS_CHSW_REQ_TX_DEF
//#define TDLS_CHSW_RESP_RX_DEF
#define TDLS_SWITCH_TO_OFFCH_DEF
#define TDLS_SWITCH_TO_BASECH_DEF
//#define TDLS_CHSW_TIME_TIMER_DEF
//#define TDLS_CHSW_TIMEOUT_TIMER_DEF
//#define TDLS_CHSW_REQ_TX_TIMER_DEF
//#define TDLS_CHSW_END_TIMER_DEF
#define TDLS_DELAY_CHSW_DEF
#endif

#ifdef ANT_DIV_PROF_DEF
#define ANT_DIV_SWITCH_DEF
#endif

/// @}

/**
 * @name Profiling Identifier definitions
 * @{
 *****************************************************************************************
 */
/// Profiling signal identifiers
enum
{
    #ifdef TX_IPC_IRQ_DEF
    TX_IPC_IRQ,
    #endif
    #ifdef TX_MACIF_EVT_DEF
    TX_MACIF_EVT,
    #endif
    #ifdef TX_BUF_ALLOC_DEF
    TX_BUF_ALLOC,
    #endif
    #ifdef TX_DMA_IRQ_DEF
    TX_DMA_IRQ,
    #endif
    #ifdef TX_PAYL_HDL_DEF
    TX_PAYL_HDL,
    #endif
    #ifdef TX_NEW_TAIL_DEF
    TX_NEW_TAIL,
    #endif
    #ifdef TX_MAC_IRQ_DEF
    TX_MAC_IRQ,
    #endif
    #ifdef TX_BUF_FREE_DEF
    TX_BUF_FREE,
    #endif
    #ifdef TX_CFM_EVT_DEF
    TX_CFM_EVT,
    #endif
    #ifdef TX_CFM_DMA_IRQ_DEF
    TX_CFM_DMA_IRQ,
    #endif
    #ifdef TX_HE_TRIG_IRQ_DEF
    TX_HE_TRIG_IRQ,
    #endif
    #ifdef MSG_BASIC_PROF_DEF
    MSG_IRQ,
    MSG_FWD,
    MSG_IPC_IND,
    #endif
    #ifdef RX_MAC_IRQ_DEF
    RX_MAC_IRQ,
    #endif
    #ifdef RX_CNTRL_EVT_DEF
    RX_CNTRL_EVT,
    #endif
    #ifdef RX_MPDU_XFER_DEF
    RX_MPDU_XFER,
    #endif
    #ifdef RX_MPDU_FREE_DEF
    RX_MPDU_FREE,
    #endif
    #ifdef RX_DMA_IRQ_DEF
    RX_DMA_IRQ,
    #endif
    #ifdef RX_DMA_EVT_DEF
    RX_DMA_EVT,
    #endif
    #ifdef RX_IPC_IND_DEF
    RX_IPC_IND,
    #endif
    #ifdef RX_HOSTBUF_IDX_DEF
    RX_HOSTBUF_IDX,
    #endif
    #ifdef AGG_FIRST_MPDU_DWNLD_DEF
    AGG_FIRST_MPDU_DWNLD,
    #endif
    #ifdef AGG_START_AMPDU_DEF
    AGG_START_AMPDU,
    #endif
    #ifdef AGG_ADD_MPDU_DEF
    AGG_ADD_MPDU,
    #endif
    #ifdef AGG_FINISH_AMPDU_DEF
    AGG_FINISH_AMPDU,
    #endif
    #ifdef AGG_SMPDU_DONETX_DEF
    AGG_SMPDU_DONETX,
    #endif
    #ifdef AGG_BAR_DONETX_DEF
    AGG_BAR_DONETX,
    #endif
    #ifdef AGG_BA_RXED_DEF
    AGG_BA_RXED,
    #endif

    #ifdef MU_SEC_USER_IRQ_DEF
    MU_SEC_USER_IRQ,
    #endif
    #ifdef MU_PPDU_START_DEF
    MU_PPDU_START,
    #endif
    #ifdef MU_MPDU_ADD_DEF
    MU_MPDU_ADD,
    #endif
    #ifdef MU_PPDU_CLOSE_DEF
    MU_PPDU_CLOSE,
    #endif
    #ifdef MU_USER_POS_DEF
    MU_USER_POS,
    MU_USER_POS_LAST,
    #endif
    #ifdef MU_USER_POS_IRQ_DEF
    MU_USER_POS_IRQ,
    MU_USER_POS_IRQ_LAST,
    #endif

    #ifdef CHAN_CTXT_CDE_EVT_PROF_DEF
    CHAN_CTXT_CDE_EVT,
    #endif
    #ifdef CHAN_CTXT_TBTT_SWITCH_PROF_DEF
    CHAN_CTXT_TBTT_SWITCH,
    #endif
    #ifdef CHAN_CTXT_IDX_PROF_DEF
    CHAN_CTXT_IDX_PROF,
    CHAN_CTXT_IDX_1    = CHAN_CTXT_IDX_PROF + 1,
    CHAN_CTXT_IDX_LAST = CHAN_CTXT_IDX_PROF + 2,
    #endif
    #ifdef CHAN_CTXT_TX_DISCARD_PROF_DEF
    CHAN_CTXT_TX_DISCARD,
    #endif
    #ifdef CHAN_CTXT_WAIT_END_PROF_DEF
    CHAN_CTXT_WAIT_END,
    #endif
    #ifdef CHAN_CTXT_SWITCH_PROF_DEF
    CHAN_CTXT_SWITCH,
    #endif
    #ifdef CHAN_CTXT_TBTT_PRES_PROF_DEF
    CHAN_CTXT_TBTT_PRES,
    #endif

    #ifdef P2P_NOA_ABS_DEF
    P2P_NOA_0_ABS,
    P2P_NOA_1_ABS,
    #endif
    #ifdef P2P_CTW_DEF
    P2P_CTW,
    #endif
    #ifdef P2P_WAIT_BCN_DEF
    P2P_WAIT_BCN,
    #endif
    #ifdef P2P_ABSENCE_DEF
    P2P_ABSENCE,
    #endif
    #ifdef P2P_PS_PAUSED_DEF
    P2P_PS_PAUSED,
    #endif

    #ifdef MM_HW_IDLE_DEF
    MM_HW_IDLE,
    #endif
    #ifdef MM_SET_CHANNEL_DEF
    MM_SET_CHANNEL,
    #endif
    #ifdef RADAR_IRQ_DEF
    RADAR_IRQ,
    #endif
    #ifdef TX_FRAME_PUSH_DEF
    TX_FRAME_PUSH,
    #endif
    #ifdef TX_FRAME_CFM_DEF
    TX_FRAME_CFM,
    #endif
    #ifdef TX_AC_BG_DEF
    TX_AC_BG,
    TX_AC_BG_LAST = TX_AC_BG + 1,
    #endif
    #ifdef TX_AC_IRQ_DEF
    TX_AC_IRQ,
    TX_AC_IRQ_LAST = TX_AC_IRQ + 1,
    #endif
    #ifdef BCN_PRIM_TBTT_IRQ_DEF
    BCN_PRIM_TBTT_IRQ,
    #endif
    #ifdef BCN_SEC_TBTT_IRQ_DEF
    BCN_SEC_TBTT_IRQ,
    #endif
    #ifdef HW_TBTT_EVT_DEF
    HW_TBTT_EVT,
    #endif
    #ifdef AP_TBTT_DEF
    AP_TBTT,
    #endif
    #ifdef STA_TBTT_DEF
    STA_TBTT,
    #endif
    #ifdef TBTT_IDX_DEF
    TBTT_IDX_PROF,
    TBTT_IDX_1    = TBTT_IDX_PROF + 1,
    TBTT_IDX_LAST = TBTT_IDX_PROF + 2,
    #endif

    #ifdef PS_SLEEP_DEF
    PS_SLEEP,
    #endif
    #ifdef PS_PAUSE_DEF
    PS_PAUSE,
    #endif
    #ifdef PS_DPSM_UPDATE_DEF
    PS_DPSM_UPDATE,
    #endif
    #ifdef PS_CHECK_RX_DEF
    PS_CHECK_RX,
    #endif
    #ifdef PS_CHECK_TX_DEF
    PS_CHECK_TX,
    #endif
    #ifdef PS_CHECK_BCN_DEF
    PS_CHECK_BCN,
    #endif

    #ifdef TD_CHECK_RX_DEF
    TD_CHECK_RX,
    #endif
    #ifdef TD_CHECK_TX_DEF
    TD_CHECK_TX,
    #endif
    #ifdef TD_CHECK_RX_PS_DEF
    TD_CHECK_RX_PS,
    #endif
    #ifdef TD_CHECK_TX_PS_DEF
    TD_CHECK_TX_PS,
    #endif
    #ifdef TD_TIMER_END_DEF
    TD_TIMER_END,
    #endif

    #ifdef BFR_MU_CALIB_DEF
    BFR_MU_CALIB,
    #endif //(BFR_MU_CALIB_DEF)
    #ifdef BFR_SU_CALIB_DEF
    BFR_SU_CALIB,
    #endif //(BFR_SU_CALIB_DEF)
    #ifdef BFR_RX_BFR_DEF
    BFR_RX_BFR,
    #endif //(BFR_RX_BFR_DEF)
    #ifdef BFR_UPLOAD_DEF
    BFR_UPLOAD,
    #endif //(BFR_UPLOAD_DEF)
    #ifdef BFR_DOWNLOAD_DEF
    BFR_DOWNLOAD,
    #endif //(BFR_DOWNLOAD_DEF)
    #ifdef BFR_TX_IND_DEF
    BFR_TX_IND,
    #endif //(BFR_TX_IND_DEF)
    #ifdef BFR_TX_CFM_DEF
    BFR_TX_CFM,
    #endif //(BFR_TX_CFM_DEF)
    #ifdef BFR_SMM_IDX_DEF
    BFR_SMM_IDX_0,
    BFR_SMM_IDX_1,
    BFR_SMM_IDX_2,
    BFR_SMM_IDX_LAST,
    #endif //(BFR_SMM_IDX_DEF)

    #ifdef RX_DUPLI_CHECK_DEF
    RX_DUPLI_CHECK,
    #endif
    #ifdef RX_DUPLI_NSTA_CHECK_DEF
    RX_DUPLI_NSTA_CHECK,
    #endif

    #ifdef IPCDESC_PREPARE_DEF
    IPCDESC_PREPARE,
    #endif
    #ifdef IPCDESC_TRANSFER_DEF
    IPCDESC_TRANSFER,
    #endif

    #ifdef DEFRAG_CHECK_DEF
    DEFRAG_CHECK,
    #endif
    #ifdef DEFRAG_TRANSFER_DEF
    DEFRAG_TRANSFER,
    #endif
    #ifdef DEFRAG_UPD_LENGTH_DEF
    DEFRAG_UPD_LENGTH,
    #endif

    #ifdef REORD_CHECK_DEF
    REORD_CHECK,
    #endif
    #ifdef REORD_BAR_CHECK_DEF
    REORD_BAR_CHECK,
    #endif
    #ifdef REORD_FLUSH_DEF
    REORD_FLUSH,
    #endif
    #ifdef REORD_FWD_DEF
    REORD_FWD,
    #endif

    #ifdef MAC2ETH_UPDATE_DEF
    MAC2ETH_UPDATE,
    #endif

    #ifdef PS_PEER_STATE_DEF
    PS_PEER_STATE,
    #endif
    #ifdef PS_BCMC_STATE_DEF
    PS_BCMC_STATE,
    #endif
    #ifdef PS_STATE_VAL_DEF
    PS_STATE_VAL,
    PS_STATE_VAL_LAST = PS_STATE_VAL + 1,
    #endif
    #ifdef PS_PSPOLL_RX_DEF
    PS_PSPOLL_RX,
    #endif
    #ifdef PS_TRAFFIC_REQ_DEF
    PS_TRAFFIC_REQ,
    #endif
    #ifdef BW_DROP_IRQ_DEF
    BW_DROP_IRQ,
    #endif
    #ifdef BW_DROP_STEP_DEF
    BW_DROP_STEP,
    #endif

    #ifdef RC_UPD_COUNTERS_DEF
    RC_UPD_COUNTERS,
    #endif
    #ifdef RC_UPD_COUNTERS_TRIAL_DEF
    RC_UPD_COUNTERS_TRIAL,
    #endif
    #ifdef RC_STATS_CALC_DEF
    RC_STATS_CALC,
    #endif
    #ifdef RC_UPD_RETRY_CHAIN_DEF
    RC_UPD_RETRY_CHAIN,
    #endif
    #ifdef RC_UPD_RETRY_CHAIN_TRIAL_DEF
    RC_UPD_RETRY_CHAIN_TRIAL,
    #endif
    #ifdef RC_LOOKAROUND_TX_DEF
    RC_LOOKAROUND_TX,
    #endif
    #ifdef RC_SET_TRIAL_BUFFER_DEF
    RC_SET_TRIAL_BUFFER,
    #endif
    #ifdef RC_SW_RETRY_DEF
    RC_SW_RETRY,
    #endif

    #ifdef FT_OVER_DS_REQ_DEF
    FT_OVER_DS_REQ,
    #endif
    #ifdef FT_REASSOC_DEF
    FT_REASSOC,
    #endif

    #ifdef MESH_PS_ENABLE_DEF
    MESH_PS_ENABLE,
    #endif //(MESH_PS_ENABLE_DEF)
    #ifdef MESH_PS_SP_OWNER_DEF
    MESH_PS_SP_OWNER,
    #endif //(MESH_PS_SP_OWNER_DEF)
    #ifdef MESH_PS_SP_RECIP_DEF
    MESH_PS_SP_RECIP,
    #endif //(MESH_PS_SP_RECIP_DEF)
    #ifdef MESH_PS_WAIT_BCMC_DEF
    MESH_PS_WAIT_BCMC,
    #endif //(MESH_PS_WAIT_BCMC_DEF)
    #ifdef MESH_PS_WAIT_BCN_DEF
    MESH_PS_WAIT_BCN,
    #endif //(MESH_PS_WAIT_BCN_DEF)
    #ifdef MESH_PS_LOCAL_MAW_DEF
    MESH_PS_LOCAL_MAW,
    #endif //(MESH_PS_LOCAL_MAW_DEF)
    #ifdef MESH_PS_PEER_MAW_DEF
    MESH_PS_PEER_MAW,
    #endif //(MESH_PS_PEER_MAW_DEF)

    #ifdef CPU_SLEEP_DEF
    CPU_SLEEP,
    #endif

    #ifdef DEEP_SLEEP_DEF
    DEEP_SLEEP,
    #endif

    #ifdef TDLS_CHSW_REQ_RX_DEF
    TDLS_RX_CHSW_REQ,
    #endif
    #ifdef TDLS_CHSW_RESP_RX_DEF
    TDLS_RX_CHSW_RESP,
    #endif
    #ifdef TDLS_CHSW_REQ_TX_DEF
    TDLS_TX_CHSW_REQ,
    #endif
    #ifdef TDLS_CHSW_RESP_TX_DEF
    TDLS_TX_CHSW_RESP,
    #endif
    #ifdef TDLS_SWITCH_TO_OFFCH_DEF
    TDLS_SWITCH_TO_OFFCH,
    #endif
    #ifdef TDLS_SWITCH_TO_BASECH_DEF
    TDLS_SWITCH_TO_BASECH,
    #endif
    #ifdef TDLS_CHSW_TIME_TIMER_DEF
    TDLS_CHSW_TIME_TIMER,
    #endif
    #ifdef TDLS_CHSW_TIMEOUT_TIMER_DEF
    TDLS_CHSW_TIMEOUT_TIMER,
    #endif
    #ifdef TDLS_CHSW_END_TIMER_DEF
    TDLS_CHSW_END_TIMER,
    #endif
    #ifdef TDLS_CHSW_REQ_TX_TIMER_DEF
    TDLS_CHSW_REQ_TX_TIMER,
    #endif
    #ifdef TDLS_DELAY_CHSW_DEF
    TDLS_DELAY_CHSW,
    #endif
    #ifdef TDLS_CHSW_NULL_FRAME_TX_DEF
    TDLS_TX_CHSW_NULL_FRAME,
    #endif
    #ifdef RTOS_TASK_DEF
    RTOS_TASK,
    RTOS_TASK_LAST = RTOS_TASK + 3,
    #endif
    #ifdef ANT_DIV_SWITCH_DEF
    ANT_DIV_SWITCH,
    #endif

    DBG_PROF_MAX
};
/// @}


#if NX_PROFILING_ON
/// Variable linking a profiling identifier to its name string
extern const char * const dbg_prof_conf[DBG_PROF_MAX];
#endif


/// @}  // end of group DBG_PROF

#endif // _DBG_PROFILING_H_

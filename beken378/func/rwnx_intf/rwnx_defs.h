/**
 ******************************************************************************
 *
 * @file rwnx_defs.h
 *
 * @brief Main driver structure declarations for fullmac driver
 *
 * Copyright (C) RivieraWaves 2012-2019
 *
 ******************************************************************************
 */

#ifndef _RWNX_DEFS_H_
#define _RWNX_DEFS_H_

#include "mm_task.h"

#define __MDM_MAJOR_VERSION(v)     (((v) & 0xFF000000) >> 24)
#define __MDM_MINOR_VERSION(v)     (((v) & 0x00FF0000) >> 16)
#define __MDM_PHYCFG_FROM_VERS(v)  (((v) & MDM_RFMODE_MASK) >> MDM_RFMODE_LSB)

#define MDM_PHY_CONFIG_TRIDENT     0
#define MDM_PHY_CONFIG_ELMA        1
#define MDM_PHY_CONFIG_KARST       2

#define RWNX_TX_LIFETIME_MS             100

struct rwnx_sec_phy_chan {
    u16 prim20_freq;
    u16 center_freq1;
    u16 center_freq2;
    //enum nl80211_band band;
    u8 type;
};

#define RWNX_CH_NOT_SET 0xFF
#define RWNX_INVALID_VIF 0xFF
#define RWNX_INVALID_STA 0xFF

struct rwnx_hw;

/**
 * rwnx_phy_info - Phy information
 *
 * @phy_cnt: Number of phy interface
 * @cfg: Configuration send to firmware
 * @sec_chan: Channel configuration of the second phy interface (if phy_cnt > 1)
 * @limit_bw: Set to true to limit BW on requested channel. Only set to use
 * VHT with old radio that don't support 80MHz (deprecated)
 */
struct rwnx_phy_info {
    u8 cnt;
    struct phy_cfg_tag cfg;
    struct rwnx_sec_phy_chan sec_chan;
    bool limit_bw;
};

struct rwnx_hw {
    struct rwnx_mod_params *mod_params;
    struct wiphy *wiphy;
    unsigned long drv_flags;
    u8 monitor_vif; 		/* FW id of the monitor interface, RWNX_INVALID_VIF if no monitor vif at fw level */

    struct mm_version_cfm version_cfm;          /* Lower layers versions - obtained via MM_VERSION_REQ */

    u8 avail_idx_map;
    u8 vif_started;
    bool adding_sta;

    //struct rwnx_phy_info phy;

    //struct rwnx_radar radar;

	/* extended capabilities supported */
	//u8 ext_capa[8];

#ifdef CONFIG_RWNX_MUMIMO_TX
	struct rwnx_mu_info mu;
#endif
};

#define RWNX_TAG "rwnx"
#define RWNX_LOGI(...) BK_LOGI(RWNX_TAG, ##__VA_ARGS__)
#define RWNX_LOGW(...) BK_LOGW(RWNX_TAG, ##__VA_ARGS__)
#define RWNX_LOGE(...) BK_LOGE(RWNX_TAG, ##__VA_ARGS__)
#define RWNX_LOGD(...) BK_LOGD(RWNX_TAG, ##__VA_ARGS__)

extern struct rwnx_hw g_rwnx_hw;
#endif /* _RWNX_DEFS_H_ */


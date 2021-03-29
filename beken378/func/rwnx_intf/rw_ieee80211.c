#include "include.h"
#include "rw_ieee80211.h"
#include "rw_pub.h"
#include "rwnx.h"
#include "rw_msdu.h"
#include "mem_pub.h"
#if CFG_IEEE80211AX
#include "rwnx_defs.h"
#include "rwnx_rx.h"
#include "rwnx_params.h"
#endif
#include "rwnx_defs.h"

#if CFG_POWER_TABLE
#include "bk_pwr_tbl_pub.h"
#endif

typedef struct _wifi_cn_code_st_ {
	UINT32 init;
	wifi_country_t cfg;
} WIFI_CN_ST, *WIFI_CN_PTR;

WIFI_CN_ST g_country_code = {0};

#define COUNTRY_CODE_CN   {.cc= "CN", .schan=1, .nchan=13, .max_tx_power=0, .policy=WIFI_COUNTRY_POLICY_MANUAL};
#define COUNTRY_CODE_US   {.cc= "US", .schan=1, .nchan=11, .max_tx_power=0, .policy=WIFI_COUNTRY_POLICY_MANUAL};
#define COUNTRY_CODE_EP   {.cc= "EP", .schan=1, .nchan=13, .max_tx_power=0, .policy=WIFI_COUNTRY_POLICY_MANUAL};
#define COUNTRY_CODE_JP   {.cc= "JP", .schan=1, .nchan=14, .max_tx_power=0, .policy=WIFI_COUNTRY_POLICY_MANUAL};
#define COUNTRY_CODE_AU   {.cc= "AU", .schan=1, .nchan=13, .max_tx_power=0, .policy=WIFI_COUNTRY_POLICY_MANUAL};

static struct ieee80211_channel rw_2ghz_channels[] = {
	CHAN2G(1, 2412, 0),
	CHAN2G(2, 2417, 0),
	CHAN2G(3, 2422, 0),
	CHAN2G(4, 2427, 0),
	CHAN2G(5, 2432, 0),
	CHAN2G(6, 2437, 0),
	CHAN2G(7, 2442, 0),
	CHAN2G(8, 2447, 0),
	CHAN2G(9, 2452, 0),
	CHAN2G(10, 2457, 0),
	CHAN2G(11, 2462, 0),
	CHAN2G(12, 2467, 0),
	CHAN2G(13, 2472, 0),
	CHAN2G(14, 2484, 0),
};

struct ieee80211_channel rw_5ghz_a_channels[] = {
	CHAN5G(34, 0),
	CHAN5G(36, 0),
	CHAN5G(38, 0),
	CHAN5G(40, 0),
	CHAN5G(42, 0),
	CHAN5G(44, 0),
	CHAN5G(46, 0),
	CHAN5G(48, 0),
	CHAN5G(52, 0),
	CHAN5G(56, 0),
	CHAN5G(60, 0),
	CHAN5G(64, 0),
	CHAN5G(100, 0),
	CHAN5G(104, 0),
	CHAN5G(108, 0),
	CHAN5G(112, 0),
	CHAN5G(116, 0),
	CHAN5G(120, 0),
	CHAN5G(124, 0),
	CHAN5G(128, 0),
	CHAN5G(132, 0),
	CHAN5G(136, 0),
	CHAN5G(140, 0),
	CHAN5G(149, 0),
	CHAN5G(153, 0),
	CHAN5G(157, 0),
	CHAN5G(161, 0),
	CHAN5G(165, 0),
	CHAN5G(184, 0),
	CHAN5G(188, 0),
	CHAN5G(192, 0),
	CHAN5G(196, 0),
	CHAN5G(200, 0),
	CHAN5G(204, 0),
	CHAN5G(208, 0),
	CHAN5G(212, 0),
	CHAN5G(216, 0),
};

struct wiphy g_wiphy;
#if CFG_IEEE80211AX
struct rwnx_hw g_rwnx_hw;
#endif

unsigned char beacon[149] = {
	0x80, 0x00, // Frame Control
	0x00, 0x00, // Duration
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Addr1
	0x12, 0x71, 0x11, 0x71, 0x0B, 0x71, // Addr2
	0x12, 0x71, 0x11, 0x71, 0x0B, 0x71, // Addr3
	0x00, 0x00, // SN
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Timestamp
	0x64, 0x00, // Beacon Interval
	0x31, 0x04, // Capabilities
	0x00, 0x05, 0x4A, 0x49, 0x45, 0x57, 0x55, // SSID
	0x01, 0x08, 0x82, 0x84, 0x8B, 0x96, 0x0C, 0x12, 0x18, 0x24, // Supported rates
	0x03, 0x01, 0x06, // DS
	0x05, 0x04, 0x00, 0x02, 0x00, 0x00, // TIM
	0x2A, 0x01, 0x00, // ERP
	0x32, 0x04, 0x30, 0x48, 0x60, 0x6C, // Extended Supported Rates
	0x2D, 0x1A, 0x0C, 0x00, 0x1B, 0xFF, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x96,
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // HT Capabilities
	0x3D, 0x16, 0x08, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // HT Information
	0xDD, 0x18, 0x00, 0x50, 0xF2, 0x02, 0x01, 0x01, 0x00, 0x00, 0x03, 0xA4, 0x00, 0x00, 0x27, 0xA4,
	0x00, 0x00, 0x42, 0x43, 0x5E, 0x00, 0x62, 0x32, 0x2F, 0x00 // WMM
};

#define RWNX_HT_CAPABILITIES                                    \
{                                                               \
    .ht_supported   = true,                                     \
    .cap            = 0,                                        \
    .ampdu_factor   = IEEE80211_HT_MAX_AMPDU_8K,                \
    .ampdu_density  = IEEE80211_HT_MPDU_DENSITY_16,             \
    .mcs        = {                                             \
        .rx_mask = { 0xff, 0, 0, 0, 0, 0, 0, 0, 0, 0, },        \
        .rx_highest = cpu_to_le16(65),                          \
        .tx_params = IEEE80211_HT_MCS_TX_DEFINED,               \
    },                                                          \
}

#define RWNX_VHT_CAPABILITIES                                   \
{                                                               \
    .vht_supported = false,                                     \
    .cap       =                                                \
      (3 << IEEE80211_VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_SHIFT),\
    .vht_mcs       = {                                          \
        .rx_mcs_map = cpu_to_le16(                              \
                      IEEE80211_VHT_MCS_SUPPORT_0_9    << 0  |  \
                      IEEE80211_VHT_MCS_NOT_SUPPORTED  << 2  |  \
                      IEEE80211_VHT_MCS_NOT_SUPPORTED  << 4  |  \
                      IEEE80211_VHT_MCS_NOT_SUPPORTED  << 6  |  \
                      IEEE80211_VHT_MCS_NOT_SUPPORTED  << 8  |  \
                      IEEE80211_VHT_MCS_NOT_SUPPORTED  << 10 |  \
                      IEEE80211_VHT_MCS_NOT_SUPPORTED  << 12 |  \
                      IEEE80211_VHT_MCS_NOT_SUPPORTED  << 14),  \
        .tx_mcs_map = cpu_to_le16(                              \
                      IEEE80211_VHT_MCS_SUPPORT_0_9    << 0  |  \
                      IEEE80211_VHT_MCS_NOT_SUPPORTED  << 2  |  \
                      IEEE80211_VHT_MCS_NOT_SUPPORTED  << 4  |  \
                      IEEE80211_VHT_MCS_NOT_SUPPORTED  << 6  |  \
                      IEEE80211_VHT_MCS_NOT_SUPPORTED  << 8  |  \
                      IEEE80211_VHT_MCS_NOT_SUPPORTED  << 10 |  \
                      IEEE80211_VHT_MCS_NOT_SUPPORTED  << 12 |  \
                      IEEE80211_VHT_MCS_NOT_SUPPORTED  << 14),  \
    }                                                           \
}

#define RWNX_HE_CAPABILITIES                                    \
{                                                               \
    .has_he = false,                                            \
    .he_cap_elem = {                                            \
        .mac_cap_info[0] = 0,                                   \
        .mac_cap_info[1] = 0,                                   \
        .mac_cap_info[2] = 0,                                   \
        .mac_cap_info[3] = 0,                                   \
        .mac_cap_info[4] = 0,                                   \
        .mac_cap_info[5] = 0,                                   \
        .phy_cap_info[0] = 0,                                   \
        .phy_cap_info[1] = 0,                                   \
        .phy_cap_info[2] = 0,                                   \
        .phy_cap_info[3] = 0,                                   \
        .phy_cap_info[4] = 0,                                   \
        .phy_cap_info[5] = 0,                                   \
        .phy_cap_info[6] = 0,                                   \
        .phy_cap_info[7] = 0,                                   \
        .phy_cap_info[8] = 0,                                   \
        .phy_cap_info[9] = 0,                                   \
        .phy_cap_info[10] = 0,                                  \
    },                                                          \
    .he_mcs_nss_supp = {                                        \
        .rx_mcs_80 = cpu_to_le16(0xfffa),                       \
        .tx_mcs_80 = cpu_to_le16(0xfffa),                       \
        .rx_mcs_160 = cpu_to_le16(0xffff),                      \
        .tx_mcs_160 = cpu_to_le16(0xffff),                      \
        .rx_mcs_80p80 = cpu_to_le16(0xffff),                    \
        .tx_mcs_80p80 = cpu_to_le16(0xffff),                    \
    },                                                          \
    .ppe_thres = {0x08, 0x1c, 0x07},                            \
}

#if CFG_IEEE80211AX
static struct ieee80211_sband_iftype_data rwnx_he_capa = {
//	.types_mask = BIT(NL80211_IFTYPE_STATION),
	.he_cap = RWNX_HE_CAPABILITIES,
};
#endif

static struct ieee80211_supported_band rwnx_band_2GHz = {
	.channels   = rw_2ghz_channels,
	.n_channels = ARRAY_SIZE(rw_2ghz_channels),
//	.bitrates   = rwnx_ratetable,
//	.n_bitrates = ARRAY_SIZE(rwnx_ratetable),
	.ht_cap     = RWNX_HT_CAPABILITIES,
#if CFG_IEEE80211AX
	.iftype_data = &rwnx_he_capa,
	.n_iftype_data = 1,
#else
	.iftype_data = NULL,
	.n_iftype_data = 0,
#endif
};

static struct ieee80211_supported_band rwnx_band_5GHz = {
#ifdef ENABLE_5GHZ_IEEE80211
	.channels   = rw_5ghz_a_channels,
	.n_channels = ARRAY_SIZE(rw_5ghz_a_channels),
#else
	.channels	= NULL,
	.n_channels = 0,
#endif
//	.bitrates   = &rwnx_ratetable[4],
//	.n_bitrates = ARRAY_SIZE(rwnx_ratetable) - 4,
	.ht_cap     = RWNX_HT_CAPABILITIES,
	.vht_cap    = RWNX_VHT_CAPABILITIES,
#if CFG_IEEE80211AX
	.iftype_data = &rwnx_he_capa,
	.n_iftype_data = 1,
#else
	.iftype_data = NULL,
	.n_iftype_data = 0,
#endif
};

UINT32 rw_ieee80211_init(void)
{
#if CFG_IEEE80211AX
	struct rwnx_hw *rwnx_hw;
	int i;
#endif
	RW_CONNECTOR_T intf = {0, };
	struct wiphy *wiphy;

	wiphy = &g_wiphy;
	memset(wiphy, 0, sizeof(*wiphy));

#if CFG_IEEE80211AX
	rwnx_hw = &g_rwnx_hw;
	memset(rwnx_hw, 0, sizeof(*rwnx_hw));

	rwnx_hw->wiphy = wiphy;
	rwnx_hw->mod_params = &rwnx_mod_params;

	rwnx_hw->vif_started = 0;
	rwnx_hw->monitor_vif = RWNX_INVALID_VIF;
	rwnx_hw->adding_sta = false;

	for (i = 0; i < NX_VIRT_DEV_MAX + NX_REMOTE_STA_MAX; i++)
		rwnx_hw->avail_idx_map |= BIT(i);
#endif

	wiphy->bands[IEEE80211_BAND_2GHZ] = &rwnx_band_2GHz;
	wiphy->bands[IEEE80211_BAND_5GHZ] = &rwnx_band_5GHz;

#if !CFG_IEEE80211AX
	//SMPS disabled, 20M SGI, TxSTBC ?!(only one NSS), Rx STBC,
	wiphy->bands[IEEE80211_BAND_2GHZ]->ht_cap.cap = 0x1ac;  // FIXME: hard coded here, use `rwnx_set_ht_capa()` function.
#endif

	intf.msg_outbound_func = mr_kmsg_fwd;
	intf.data_outbound_func = rwm_upload_data;
	intf.rx_alloc_func = rwm_get_rx_free_node;
#if CFG_IEEE80211AX
	intf.monitor_outbound_func = rwm_rx_monitor;
#else
	intf.get_rx_valid_status_func = rwm_get_rx_valid;
	intf.tx_confirm_func = rwm_tx_confirm;
#endif

	rwnxl_register_connector(&intf);

	/* init country code */
	g_country_code.cfg.cc[0] = 'C';
	g_country_code.cfg.cc[1] = 'N';
	g_country_code.cfg.cc[2] = 0;
	g_country_code.cfg.schan = 1;
	g_country_code.cfg.nchan = 13;
	g_country_code.cfg.max_tx_power = 0;
	g_country_code.cfg.policy = WIFI_COUNTRY_POLICY_MANUAL;

	g_country_code.init = 1;

	return 0;
}

#if CFG_POWER_TABLE
static UINT32 rw_ieee80211_set_chan_power(void)
{
    uint8_t chan_idx;
    uint16_t txpwr;

    if(g_country_code.init == 0)
        return kNotInitializedErr;

    if(g_country_code.cfg.policy == WIFI_COUNTRY_POLICY_AUTO)
        return kNoErr;

    if(g_country_code.cfg.max_tx_power < 0)
    {
        // disable this channel
        txpwr = 0;
    }
    else if(g_country_code.cfg.max_tx_power == 0)
    {
        // use default
        txpwr = 300;
    }
    else
    {
        txpwr = g_country_code.cfg.max_tx_power * 10;
    }

    for (chan_idx = 1; chan_idx <= 14; chan_idx++)
    {
        if(rw_ieee80211_is_scan_rst_in_countrycode(chan_idx) == 1)
        {
            pwr_tbl_set_chan_pwr(chan_idx, txpwr, txpwr, txpwr, txpwr);
        }
        else
        {
            // disable this channel
            pwr_tbl_set_chan_pwr(chan_idx, 0, 0, 0, 0);
        }
    }
    os_printf("set chan maxpower:%d-%d\r\n", txpwr, g_country_code.cfg.max_tx_power);

    return kNoErr;
}
#endif

UINT32 rw_ieee80211_set_country(const wifi_country_t *country)
{
	if (country) {
		UINT32 prev_policy;

		if (g_country_code.init == 0)
			return kNotInitializedErr;

		prev_policy = g_country_code.cfg.policy;

		os_memcpy(&g_country_code.cfg, country, sizeof(wifi_country_t));
		RWNX_LOGI("rw_ieee80211_set_country code:\r\n");
		RWNX_LOGI("code: %s\r\n", g_country_code.cfg.cc);
		RWNX_LOGI("channel: %d - %d\r\n", g_country_code.cfg.schan,
				  g_country_code.cfg.schan + g_country_code.cfg.nchan - 1);

		if (g_country_code.cfg.policy == WIFI_COUNTRY_POLICY_MANUAL) {
			RWNX_LOGI("mode: MANUAL\r\n", g_country_code.cfg.policy);
		} else {
			RWNX_LOGI("mode: AUTO\r\n", g_country_code.cfg.policy);
			if (prev_policy != g_country_code.cfg.policy) {
				RWNX_LOGI("in auto mode, need change softap beacon\r\n");
				// to do
			}
		}

#if CFG_POWER_TABLE
		rw_ieee80211_set_chan_power();
#endif
		return kNoErr;
	} else {
		return kParamErr;
	}
}

UINT32 rw_ieee80211_get_country(wifi_country_t *country)
{
	if (country) {
		if (g_country_code.init == 0)
			return kNotInitializedErr;

		os_memcpy(country, &g_country_code.cfg, sizeof(wifi_country_t));

		return kNoErr;
	} else {
		return kParamErr;
	}
}

UINT32 rw_ieee80211_get_centre_frequency(UINT32 chan_id)
{
	UINT32 freq = 0;
	struct ieee80211_channel *channels = NULL;

	if ((chan_id >= 1) && (chan_id <= 14))
		channels = (struct ieee80211_channel *)rw_2ghz_channels;

#ifdef ENABLE_5GHZ_IEEE80211
	if (chan_id > 14) {
	}
#endif

	if (channels)
		freq = channels[chan_id - 1].center_freq;

	if (freq != 0) {
		return freq;
	} else {
		RWNX_LOGI("centre freq is 0 \r\n");
		return 0;
	}
}

UINT8 rw_ieee80211_get_chan_id(UINT32 freq)
{
	int i;
	struct ieee80211_channel *channels = NULL;

	channels = (struct ieee80211_channel *)rw_2ghz_channels;
	for (i = 0; i < 14; i++) {
		if (channels[i].center_freq == freq)
			break;
	}

	if (i == 14)
		return 0;

	return i + 1;
}

UINT8 rw_ieee80211_init_scan_chan(struct scanu_start_req *req)
{
	UINT32 i, start_chan, num_chan;

	ASSERT(g_country_code.init);
	ASSERT(req);

	start_chan = g_country_code.cfg.schan;

	if (g_country_code.cfg.policy == WIFI_COUNTRY_POLICY_MANUAL) {
		num_chan = g_country_code.cfg.nchan;
	} else {
		// auto mode, sta need scan all channel
		num_chan = g_wiphy.bands[IEEE80211_BAND_2GHZ]->n_channels;
	}

	for (i = 0; i < num_chan; i ++) {
		req->chan[i].band = IEEE80211_BAND_2GHZ;
		req->chan[i].flags = 0;
		req->chan[i].freq = rw_ieee80211_get_centre_frequency(i + start_chan);
	}

#ifdef ENABLE_5GHZ_IEEE80211
	for (i = 0; i < g_wiphy.bands[IEEE80211_BAND_5GHZ]->n_channels; i ++) {
		req->chan[i].band = IEEE80211_BAND_5GHZ;
		req->chan[i].flags = 0;
		req->chan[i].freq = rw_ieee80211_get_centre_frequency(i
							+ SCAN_CHANNEL_2G4
							+ start_chan);

		ASSERT(req->chan[i].freq);
	}
#endif

	req->chan_cnt = num_chan;

	return 0;
}

UINT8 rw_ieee80211_is_scan_rst_in_countrycode(UINT8 freq)
{
	UINT32 start_chan, end_chan;

	ASSERT(g_country_code.init);

	if (g_country_code.cfg.policy != WIFI_COUNTRY_POLICY_MANUAL) {
		// auto mode, sta need scan all channel, and get all rst
		return 1;
	}

	start_chan = g_country_code.cfg.schan;
	end_chan = (start_chan + g_country_code.cfg.nchan - 1);

	if ((freq < start_chan) || (freq > end_chan))
		return 0;

	return 1;
}

#if CFG_IEEE80211N
void rw_ieee80211_set_ht_cap(UINT8 ht_supp)
{
	g_wiphy.bands[IEEE80211_BAND_2GHZ]->ht_cap.ht_supported = ht_supp;
	rw_msg_send_me_config_req();
}
#endif

// eof


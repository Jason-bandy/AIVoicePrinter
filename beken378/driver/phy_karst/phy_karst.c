/**
 ****************************************************************************************
 *
 * @file phy_karst.c
 *
 * Copyright (C) RivieraWaves 2014-2020
 *
 * When setting a channel, several procedures can be advantageously left-out depending on
 * whether the band, bw, frequency remain unchanged (calibrations, old modem clk mmc
 * toggles ..), e.g. for multi-channel. Although the changes are trivial they might get
 * in the way of properly measuring the RF behavior.
 ****************************************************************************************
 */
#include "rwnx_config.h"
#include "phy.h"
#include "_dbg.h"

#include "hal_machw.h"
#include "reg_mac_core.h"

#if NX_MDM_VER < 30
#include "reg_mdm_stat.h"
#include "reg_mdmdsss_cfg.h"
#endif
#include "reg_mdm_cfg.h"

#include "reg_riu.h"
#include "reg_rc.h"
#include "reg_agc.h"
#include "reg_macbypass.h"

#include "reg_phy_crm.h"
#include "crm.h"

#include "rd.h"
#if NX_UF_EN
#include "uf.h"
#endif
#include "phy_mem.h"
#include "intc_pub.h"

#if NX_MDM_VER < 20
#error "Needs to be compiled with modem version 20 minimum"
#endif

#if NX_UF_EN
#define RXV_REG_MIN_VERS    2
#endif

/// Structure containing the parameters of the Karst PHY configuration
struct phy_karst_cfg_tag
{
    /// TX IQ mismatch compensation in 2.4GHz
    uint32_t tx_iq_comp_2_4G[2];
    /// RX IQ mismatch compensation in 2.4GHz
    uint32_t rx_iq_comp_2_4G[2];
    /// TX IQ mismatch compensation in 5GHz
    uint32_t tx_iq_comp_5G[2];
    /// RX IQ mismatch compensation in 5GHz
    uint32_t rx_iq_comp_5G[2];
    /// RF path used by default (0 or 1)
    uint8_t path_used;
};

/// PHY driver context.
struct phy_env_tag
{
    /// Karst configuration parameters
    struct phy_karst_cfg_tag cfg;
    /// Currently configured channel
    struct mac_chan_op chan;
};

struct phy_env_tag phy_env;

#if NX_MDM_VER < 30
#define REG_DFLT_SW_CTRL    ((1 << RC_PRESCALER_LSB) & (~RC_START_DONE_BIT))
#else
#define REG_DFLT_SW_CTRL    ((1 << RC_SW_PRESCALER_LSB) & (~RC_START_DONE_BIT))
#endif

#define __RIU_RADARFIFO_ADDR 0x60C04000
#define __LDPC_RAM_ADDR      0x60C09000

// To keep correct TX quality limit pwr to 10 dBm
#define PHY_KARST_MAX_PWR    ((int8_t)10) // dBm
// And stay above 1 dBm
#define PHY_KARST_MIN_PWR    ((int8_t)-10)  // dBm

static inline void udelay(uint32_t us)
{
    uint32_t e = nxmac_monotonic_counter_2_lo_get() + us;

    do {
        volatile int n = 1 << 5; // relax
        while (n--)
            ;
    } while ((int32_t)(nxmac_monotonic_counter_2_lo_get() - e) < 0);
}

const uint32_t trx_reg_val[] = {
    0x0528846A,     //reg0
    0xAE09C181,     //reg1
    0xCFA28800,     //reg2
    0x00002164,     //reg3
    0x65C89655,     //reg4
    0x54C030AA,     //reg5
    0x727AFFF4,     //reg6
    0x57E6826B,     //reg7
    0x28AE091F,     //reg8
    0x801E077F,     //reg9
    0x8063DDFF,     //rega
    0xA9758ACE,     //regb
    0x86782EAA,     //regc
    0x47390195,     //reg12
    0x7B305ECC      //reg13
};

static void trx_reg_set(uint8_t id, uint32_t val)
{
    if(id <= 12)
    {
        REG_PL_WR(TRX_BASE+id*4, val);
    }
    else
    {
        REG_PL_WR(TRX_BASE+(id+5)*4, val);
    }
}

static uint32_t trx_reg_get(uint8_t id)
{
    uint32_t        rdata   ;
    if(id <= 12)
    {
        rdata = REG_PL_RD(TRX_BASE+id*4);
    }
    else
    {
        rdata = REG_PL_RD(TRX_BASE+(id+5)*4);
    }
    return rdata;
}

static void karst_set_channel(uint8_t band, uint16_t freq1, uint8_t chantype)
{
    extern void rwnx_cal_set_40M_setting(void);
    extern void rwnx_cal_set_20M_setting(void);
    uint32_t    val     ;
    uint32_t    chan    ;

    chan = freq1 - 2400;
    val = trx_reg_get(5);
    val &= ~(0x7f << 25);
    val |= (chan << 25);
    trx_reg_set(5, val);

    if(band == PHY_CHNL_BW_40) {
        rwnx_cal_set_40M_setting();
    }
    else
    {
        rwnx_cal_set_20M_setting();
    }
}

static void riu_pssel_set(uint16_t freq, uint16_t freq1, uint8_t chantype)
{
    unsigned int pssel;

    /**
     * PSSEL - use pssel of RW-WSDM-RIU-HW-REG.xls
     */
    if (chantype == PHY_CHNL_BW_40)
    {
        // pssel = 1 or 2
        pssel = freq < freq1 ? 1 : 2;
    }
    else if (chantype == PHY_CHNL_BW_80)
    {
        // pssel = 0, 1, 2 or 3
        int _offs = freq1 - freq;
        if (_offs > 0)
            pssel = _offs > 10 ? 0 : 1;
        else
            pssel = -_offs > 10 ? 3 : 2;
    }
    else
    {
        pssel = 0;
    }

    riu_psselect_setf(pssel);
}

static void mdm_primary_set(uint16_t freq, uint16_t freq1, uint8_t chantype)
{
    #if NX_MDM_VER >= 30
    unsigned int primary;

    if (chantype == PHY_CHNL_BW_40)
    {
        primary = freq < freq1 ? 0 : 1;
    }
    else if (chantype == PHY_CHNL_BW_80)
    {
        primary = 0; // TBD
    }
    else
    {
        primary = 0;
    }

    mdm_primaryind_set(primary);
    #endif
}

static void mdm_set_channel(uint8_t band, uint16_t freq, uint16_t freq1, uint8_t chantype,
                            uint8_t flags)
{
    // Reset the MDM/RIU before configuration
    crm_mdm_reset();

    /*
     *************************************************************************************
     * Band-dependent (2.4G, 5G) settings
     *************************************************************************************
     */
    // By default we consider we won't be on a DFS channel
    #if NX_RADAR_DETECT
    riu_radardeten_setf(0); // Disable radar detection
    crm_radartimclkforce_setf(0); // Disable radar timer clock
    riu_irqmacradardeten_setf(0); // disable radar detection interrupt
    #endif
    #if NX_MAC_HE
    nxmac_disable_tbru_26_resp_setf(0); // Allow RU26
    #endif
    if(band == PHY_BAND_5G)
    {
        nxmac_abgn_mode_setf(MODE_802_11AC_5);
        mdm_rxdsssen_setf(0);
        riu_ofdmonly_setf(1);   // AGC detection OFDM only
        #if NX_RADAR_DETECT || NX_MAC_HE
        if (flags & CHAN_RADAR)
        {
            #if NX_RADAR_DETECT
            riu_radardeten_setf(1); // Enable radar detection
            crm_radartimclkforce_setf(1); // Enable radar timer clock
            riu_irqmacradardeten_setf(1); // Enable radar detection interrupt
            #endif
            #if NX_MAC_HE
            nxmac_disable_tbru_26_resp_setf(1); // No RU26 when tuned to a DFS channel
            #endif
        }
        #endif
        riu_rwnxagcevt3_set(0);     /* write 0 to disable DSSS detection correlator */
    }
    else
    {
        nxmac_abgn_mode_setf(MODE_802_11N_2_4);
        mdm_rxdsssen_setf(1);
        riu_ofdmonly_setf(0);   // AGC detection OFDM and DSSS
        riu_rwnxagcevt3_set(RIU_RWNXAGCEVT3_RESET);/* write default to enable DSSS detection correlator */
    }

    /*
     *************************************************************************************
     * Frequency-dependent settings
     *************************************************************************************
     */
    #if NX_MDM_VER >= 30
    // For symbol clock error compensation in HE TB
    mdm_invcarrierfreq_setf((1 << 26) / freq1);
    #endif

    /*
     *************************************************************************************
     * Bandwidth-dependent settings
     *************************************************************************************
     */
    // Set PSSEL
    riu_pssel_set(freq, freq1, chantype);
    // Set Primary Channel in MDM
    mdm_primary_set(freq, freq1, chantype);
    // Configure maximum BW
    mdm_txcbwmax_setf(chantype);
    nxmac_max_supported_bw_setf(chantype);
    // Put default values
    mdm_fdoctrl0_set(MDM_FDOCTRL0_RESET);
    #if NX_MDM_VER < 30
    mdm_tbectrl2_set(0x00007F05);
    #else
    mdm_conf_bw_setf(chantype);
    mdm_rxcbwmax_setf(chantype);
    // To be uncommented when NX_MDM_VER=30 supports 80 MHz
    //mdm_tbectrl2_set(MDM_TBECTRL2_RESET);
    #endif
    #if NX_MDM_VER >= 22
    mdm_smoothforcectrl_set(0);
    #else
    mdm_smoothctrl_set(0x01880C06);
    #endif

    //  3us for TX RAMPUP <=> TXRFRAMPUP = 360
    if (chantype == PHY_CHNL_BW_20)
    {
        #if NX_MDM_VER < 30
        mdm_txstartdelay_setf(180);
        mdm_txctrl1_pack(0, 0, 28, 48);
        // TBE for 60MHz
        mdm_tbe_count_adjust_20_setf(0);
        mdm_txctrl3_pack(720, 1080);
        mdm_dcestimctrl_pack(0, 0, 0, 15, 15);
        // For FPGA, divide value by 2 due to timing constraints
        mdm_waithtstf_setf(7);
        #else
        mdm_txstartdelay_setf(384);
        mdm_txctrl1_pack(64, 96);
        // TBE for 120MHz
        mdm_tbe_count_adjust_20_setf(4);
        mdm_dcestimctrl_pack(0, 2, 10, 15);
        // For FPGA, divide value by 2 due to timing constraints
        mdm_waithtstf_setf(7);
        mdm_tdsyncoff20_setf(25);
        #endif

        #if NX_MDM_VER < 30
        mdm_tddchtstfmargin_setf(6);
        #else
        mdm_tddchtstfmargin_setf(1);
        #endif

        // No ACI margin in BW=20MHz due to latency on HTSIG decoding
        riu_rwnxagcaci20marg0_set(0);
        riu_rwnxagcaci20marg1_set(0);
        riu_rwnxagcaci20marg2_set(0);

        #if NX_MDM_VER >= 30
        // Increase DC convergence due to Karst RF performance
        riu_dccenteredholdtime50ns_setf(15);
        #endif
    }
    else
    {

        #if NX_MDM_VER < 30
        // TBE for 120MHz
        mdm_tbe_count_adjust_20_setf(2);
        mdm_txstartdelay_setf(360);
        mdm_txctrl3_pack(1440, 2160);
        #endif

        if (chantype == PHY_CHNL_BW_40)
        {
            #if NX_MDM_VER < 30
            mdm_txctrl1_pack(0, 39, 82, 96);
            mdm_rxtdctrl0_pack(18, 64, 252, 13);
            mdm_dcestimctrl_pack(0, 0, 8, 30, 31);
            // For FPGA, divide value by 2 due to timing constraints
            mdm_waithtstf_setf(15);
            #else
            mdm_txstartdelay_setf(384);
            mdm_txctrl1_pack(64, 96);
            mdm_dcestimctrl_pack(0, 3, 16, 31);
            // For FPGA, divide value by 2 due to timing constraints
            mdm_waithtstf_setf(15);
            mdm_tbe_count_adjust_20_setf(4);
            mdm_tdsyncoff20_setf(24);
            #endif

            #if NX_MDM_VER < 30
            mdm_tddchtstfmargin_setf(6);
            #else
            mdm_tddchtstfmargin_setf(1);
            #endif
        }
        else // chantype == PHY_CHNL_BW_80
        {
            #if NX_MDM_VER < 30
            mdm_txctrl1_pack(22, 60, 105, 120);
            mdm_rxtdctrl0_pack(18, 64, 247, 23);
            mdm_dcestimctrl_pack(0, 0, 38, 43, 63);
            // For FPGA, divide value by 2 due to timing constraints
            mdm_waithtstf_setf(31);
            #endif

            mdm_tddchtstfmargin_setf(14);

            #if NX_MDM_VER >= 22 && NX_MDM_VER < 30
            mdm_cfgvhtsts2smoothforce_setf(1);
            mdm_cfgvhtsts2smooth_setf(2);
            #else
            mdm_smoothctrl_set(0x018E0C06);
            #endif
            mdm_tbectrl2_set(0x00007F03);
        }

        // Set back default ACI margin
        riu_rwnxagcaci20marg0_set(RIU_RWNXAGCACI20MARG0_RESET);
        riu_rwnxagcaci20marg1_set(RIU_RWNXAGCACI20MARG1_RESET);
        riu_rwnxagcaci20marg2_set(RIU_RWNXAGCACI20MARG2_RESET);

        #if NX_MDM_VER >= 30
        // Set back default DC parameters for BW > 20MHz
        riu_dccenteredholdtime50ns_setf(RIU_DCCENTEREDHOLDTIME50NS_RST);
        #endif

    }

    #if NX_MDM_VER >= 21
    /* Reset RX IQ compensation if available */
    if (riu_iqcomp_getf())
    {
        riu_iqestiterclr_setf(1);
    }
    #endif /* NX_MDM_VER >= 21 */
}

static void phy_hw_set_channel(uint8_t band, uint16_t freq, uint16_t freq1,
                               uint8_t chantype, uint8_t flags, uint8_t index)
{
    dbg(D_INF D_PHY "%s: band=%d freq=%d freq1=%d chantype=%d sx=%d\n",__func__,band,freq,freq1,chantype,index);

    /*
     *************************************************************************************
     * Clock configuration
     *************************************************************************************
     */
#if NX_CRM
    crm_clk_set(chantype);
#endif
    /*
     *************************************************************************************
     * MODEM/RIU configuration
     *************************************************************************************
     */
    mdm_set_channel(band, freq, freq1, chantype, flags);

    /*
     *************************************************************************************
     * RF/RF board configuration
     *************************************************************************************
     */
    karst_set_channel(chantype, freq1, chantype);

}
#if 0
RC_TypeDef *const rc_inst = (RC_TypeDef *)RC_BASE;

static void rcb_init()
{
    rc_inst->REG0X0  = 0x00000709;
    rc_inst->REG0X1  = 0x00002000;

    //Tx/Rx Path on/off delay, switch delay
    rc_inst->REG0X2  = 0x00080008;
    rc_inst->REG0X3  = 0x00080028;
    rc_inst->REG0X4  = 0x00080050;
    rc_inst->REG0X5  = 0x00080008;
    rc_inst->REG0X6  = 0x00000010;

    //Tx&Rx Control
    rc_inst->REG0X8  = 0x00100014;
    rc_inst->REG0X9  = 0x08000800;
    rc_inst->REG0XA  = 0x0000b320;

    #if 0       //Board #27
    //Tx Mismatch Compensation
    rc_inst->REG0XB  = 0x08040844;
    rc_inst->REG0XC  = 0x0FE00FFF;
    rc_inst->REG0XD  = 0x07900800;
    #else       //Board #24
    //Tx Mismatch Compensation
    rc_inst->REG0XB  = 0x07D807FF;
    rc_inst->REG0XC  = 0x0FFF0FFF;
    rc_inst->REG0XD  = 0x08600800;
    #endif

    //Rx Mismatch Compensation
    rc_inst->REG0X12 = 0x00000000;
    rc_inst->REG0X13 = 0x02000000;
    rc_inst->REG0X15 = 0x00000000;
    rc_inst->REG0X16 = 0x00000000;
    rc_inst->REG0X17 = 0x00020005;
    rc_inst->REG0X1A = 0x00000006;

    //RF Gain Set
    rc_inst->REG0X1B = 0x00000008;

    //Block Enable
    rc_inst->REG0X1F = 0xDDF90339;
    rc_inst->REG0X20 = 0xD8C00130;
    rc_inst->REG0X21 = 0xDA01BCF0;
    rc_inst->REG0X22 = 0xDDF90339;

    //Rx Path DC Compensation
    rc_inst->REG0X23 = 0x00009080;
    rc_inst->REG0X24 = 0x00009181;
    rc_inst->REG0X25 = 0x00009282;
    rc_inst->REG0X26 = 0x00009383;
    rc_inst->REG0X27 = 0x00009484;
    rc_inst->REG0X28 = 0x00009585;
    rc_inst->REG0X29 = 0x00009686;
    rc_inst->REG0X2A = 0x00009787;
    rc_inst->REG0X2B = 0x00009888;
    rc_inst->REG0X2C = 0x00009989;
    rc_inst->REG0X2D = 0x00009a8a;
    rc_inst->REG0X2E = 0x00009b8b;
    rc_inst->REG0X2F = 0x00009c8c;
    rc_inst->REG0X30 = 0x00009d8d;
    rc_inst->REG0X31 = 0x00009e8e;
    rc_inst->REG0X32 = 0x00009f8f;
}
#endif

static void agcmem_init()
{
    unsigned int *p;
    int i;
    volatile int j;

    p = (unsigned int *)(AGCMEM_BASE);

    //crm_ahbclkforce_setf(0x1);
    riu_agcfsmreset_setf(0x1);
    crm_agcmemclkforce_setf(0x1);

    for (j = 0; j < 10; j ++);

    for (i = 0; i < 512; i ++)
        *(p++) = agcmem_init_value[i];

    for (j = 0; j < 10; j ++);

    crm_agcmemclkforce_setf(0x0);
    riu_agcfsmreset_setf(0x0);
    //crm_ahbclkforce_setf(0x0);
}

#if (RW_NX_LDPC_DEC || TEST_AHB_ACCESS_PHY_MEM_EN)
static void ldpcrx_cfgmem_init()
{
	unsigned int		*p;
	int					i;

	p = (unsigned int *)(LDPCRXCFGMEM_BASE);

	for(i = 0; i < 335; i ++)
		*(p++) = ldpcrxcfgmem_init_value[i];

}
#endif

#if (BK_NX_PEAK_CANCEL || TEST_AHB_ACCESS_PHY_MEM_EN)
static void peakcw_cfgmem_init()
{
	unsigned int		*p;
	int					i;

	p = (unsigned int *)(PEAKCWMEM_BASE);

	for(i = 0; i < 256; i ++)
		*(p++) = peakcw_init_value[i];

}
#endif

#if (BK_NX_PWRTBL_EN || TEST_AHB_ACCESS_PHY_MEM_EN)
static void pow_table_init()
{
    int                 i;
    volatile int        j;
    uint32_t            *p = (uint32_t *)POWTBL_BASE;

    rc_inst->REG0X8 &= ~ BIT_RC_TX_POWTBL_EN;
    for(j = 0; j < 10; j ++);
    for(i = 0; i < 128; i ++)
        *(p++) = powtable_init_value[i];
    for(j = 0; j < 10; j ++);
    rc_inst->REG0X8 |=  BIT_RC_TX_POWTBL_EN;
}
#endif

#if (BK_NX_DPD_EN || TEST_AHB_ACCESS_PHY_MEM_EN)
static void dpd_table_init()
{
    int                 i;
    volatile int        j;
    uint32_t            *p = (uint32_t *)DPDTBL_BASE;

    rc_inst->REG0X8 &= ~ BIT_RC_TX_DPD_EN;
    for(j = 0; j < 10; j ++);
    for(i = 0; i < 256; i ++)
        *(p++) = dpdtable_init_value[i];
    for(j = 0; j < 10; j ++);
    rc_inst->REG0X8 |=  BIT_RC_TX_DPD_EN;
}
#endif

extern void rwnx_cal_recover_rcbeken_reg_val(void);
extern void rwnx_cal_recover_trx_reg_val(void);
static void karst_init(const struct phy_karst_cfg_tag *cfg)
{
#if 0
    //trx
    for(i = 0; i < sizeof(trx_reg_val)/4; i ++)
    {
        trx_reg_set(i, trx_reg_val[i]);
    }
#else
    #if CFG_SUPPORT_CALIBRATION
    rwnx_cal_recover_trx_reg_val();
    #endif
#endif

    #if BK_NX_PWRTBL_EN
    //powtbl
    pow_table_init();
    #endif

    #if BK_NX_DPD_EN
    //dpd
    dpd_table_init();
    #endif

    //rf enable
    //rc_inst->REG0X0 |= BIT_RC_RF_EN;
    rc_cntl_stat_set(1);
}

static void mdm_init(const struct phy_karst_cfg_tag *cfg)
{
    intc_service_register(FIQ_MODEM, PRI_FIQ_MODEM, phy_mdm_isr);
    intc_service_register(FIQ_RC, PRI_FIQ_RC, phy_rc_isr);

    // Check if we are compiled for this version of the PHY
    ASSERT_ERR((mdm_major_version_getf() + 2) * 10 + mdm_minor_version_getf()
                                                                 == NX_MDM_VER);

    // Reset the MDM/RIU before configuration
#if NX_CRM
    crm_mdm_reset();
#endif
    /*
     *************************************************************************************
     * MODEM configuration
     *************************************************************************************
     */
    // Supported features
    mdm_rxmode_set(MDM_RXMMEN_BIT | MDM_RXDSSSEN_BIT);
    mdm_rxnssmax_setf(mdm_nss_getf() - 1);
    mdm_rxndpnstsmax_setf(mdm_nsts_getf() - 1);
    mdm_rxldpcen_setf(mdm_ldpcdec_getf());
    mdm_rxvhten_setf(phy_vht_supported());
    mdm_rxstbcen_setf(1);
    #if NX_MDM_VER < 30
    mdm_rxgfen_setf(1);
    mdm_rxmumimoen_setf(mdm_mumimorx_getf());
    mdm_rxmumimoapeplenen_setf(mdm_mumimorx_getf());
    #else
    mdm_rxdcmen_setf(mdm_he_getf());
    mdm_rxheen_setf(mdm_he_getf());
    mdm_rxvhtmumimoen_setf(mdm_vht_getf() & mdm_mumimorx_getf());
    mdm_rxhemumimoen_setf(mdm_he_getf() & mdm_mumimorx_getf());
    #endif

    // Set DSSS precomp
    mdm_precomp_setf(45);

    #if NX_MDM_VER == 20
    #if RW_NX_LDPC_DEC
    // Set LDPC table selection for FPGA limitation
    mdm_ldpcdectablesel_setf(2);
    #endif
    #endif

    // Allow GF/SGI/STBC (bit14 reset) - TEMPORARY!!!!
    mdm_rxframeviolationmask_setf(0xFFFFBFFF);

    mdm_txmode_set(MDM_TXSTBCEN_BIT | MDM_TXGFEN_BIT  |
                   MDM_TXMMEN_BIT   | MDM_TXDSSSEN_BIT);
    mdm_txnssmax_setf(mdm_nss_getf() - 1);
    mdm_ntxmax_setf(mdm_ntx_getf() - 1);
    mdm_txcbwmax_setf(mdm_chbw_getf());
    mdm_txldpcen_setf(mdm_ldpcenc_getf());
    mdm_txvhten_setf(phy_vht_supported());
    #if NX_MDM_VER >= 30
    mdm_txheen_setf(phy_he_supported());
    mdm_tdfocpeslopeen_setf(1);
    #endif
    mdm_txmumimoen_setf(mdm_mumimotx_getf());

    #if NX_MDM_VER < 30
    /* AGC reset mode
     don't turn off RF if rxreq de-asserted for few cycles after a RXERR */
    mdm_rxtdctrl1_set(mdm_rxtdctrl1_get()|1);
    #endif

    /* Enable automatic smoothing filter selection from SNR, then disable force */
    #if NX_MDM_VER < 22
    mdm_cfgsmoothforce_setf(0);
    #endif

    #if NX_MDM_VER < 30
    if (mdm_nss_getf() == 1)
    {
        /* limit NDBPSMAX to 1x1 80 MCS7 LGI(292.5Mb/s) / SGI (325.0Mb/s) */
        mdm_rxctrl1_set(0x04920492);
    }
    else
    {
        #if defined(CFG_VIRTEX6)
        /* limit NDBPSMAX to 2x2 80 MCS4 LGI(351Mb/s) / SGI (390.0Mb/s) */
        mdm_rxctrl1_set(0x057C057C);
        #elif defined(CFG_VIRTEX7)
        /* No limitation on VIRTEX7 platform */
        mdm_rxctrl1_set(0x0C300C30);
        #endif
    }
    #else
    mdm_txtdsfoctrl_set(0x10000000);
    mdm_txtdcfoctrl_set(0x10000000);
    mdm_rxctrl5_set(0x00670780);
    mdm_tdfoctrl0_set(0x00340100);
    #endif

    /* LDPC Dec Config Memory */
    #if RW_NX_LDPC_DEC
    ldpcrx_cfgmem_init();
    #endif

    /* Peak Cancel waves Config memory */
    #if BK_NX_PEAK_CANCEL
    peakcw_cfgmem_init();
    mdm_peakcancelctrl_set(0x2300);
    #endif

    /*
     *************************************************************************************
     * RIU configuration
     *************************************************************************************
     */
    /* Enable RC clock */
    crm_rcclkforce_setf(1);
    crm_ahbclkforce_setf(1);

    /* RCB Init */
    //rcb_init();
    #if CFG_SUPPORT_CALIBRATION
    rwnx_cal_recover_rcbeken_reg_val();
    #endif

    #if NX_MDM_VER >= 21
    /* Enable RX IQ compensation if available */
    if (riu_iqcomp_getf())
    {
        riu_rxiqphaseesten_setf(1);
        riu_rxiqgainesten_setf(1);
        riu_rxiqphasecompen_setf(1);
        riu_rxiqgaincompen_setf(1);
        riu_iqestiterclr_setf(1);
    }
    #endif /* NX_MDM_VER >= 21 */

    /* limit RIU to 1 or 2 antenna active depending on modem capabilities */
    if (mdm_nss_getf() == 2)
    {
        riu_activeant_setf(3);
        riu_combpathsel_setf(3);
    }
    else
    {
        riu_activeant_setf(1);
        /* limit AGC with a single antenna (path0) */
        riu_combpathsel_setf(1);
    }

    // Tx Digital gain
    #if NX_MDM_VER < 30
    riu_rwnxfectrl0_set(0x001A1A1A);
    if (mdm_ntx_getf() == 2)
        riu_rwnxfectrl1_set(0x001A1A1A);
    #else
    riu_rifsdeten_setf(0);
    riu_rwnxfectrl0_set(0x00404040);
    if (mdm_ntx_getf() == 2)
        riu_rwnxfectrl1_set(0x00404040);

    /* disable the riu dccentered compensation */
    riu_dccenteredtype_setf(0);
    #endif

    riu_crossupthrqdbm_setf(0x200); /* write 0x200 to disable AGC cross-up */
    riu_crossdnthrqdbm_setf(0x200); /* write 0x200 to disable AGC cross-down */

    riu_rwnxagcccatimeout_set(8000000); // 100ms
    riu_irqmacccatimeouten_setf(1);

    riu_rwnxagcgainrange_set(0x4D794D79);   //max_gain=77, min_gain=-10
    riu_adcpowsupthrdbm_setf(0xB3);         //thrd=-77

    /* Enable HW antenna selection */
    riu_rxpathselfromreg_setf(0);

    #if BK_MAC
    /* to improve 11b rx sensitivity*/
    mdm_rho_setf(3);
    #endif

    /* AGC Memory Initial */
    agcmem_init();
#if 0
    /*
     *************************************************************************************
     * MACBYPASS configuration
     *************************************************************************************
     */
    #if NX_MDM_VER >= 30
    macbyp_clken_set(1); // enable clock
    #endif
    macbyp_trigger_set(0x00000012);
    macbyp_ctrl_set(0x00000100);

    #if NX_UF_EN
    if (macbyp_version_get() >= RXV_REG_MIN_VERS)
    {
        macbyp_clken_set(1); // enable clock
        macbyp_int3_gen_setf(0x07);// configure interrupt generation
        macbyp_mode_setf(1); //set mode RX
    }
    #endif
#endif
}

static void phy_hw_init(const struct phy_karst_cfg_tag *cfg)
{
    /*
     *************************************************************************************
     * MODEM/RIU configuration
     *************************************************************************************
     */
    mdm_init(cfg);

    /*
     *************************************************************************************
     * RF/RF board configuration
     *************************************************************************************
     */
    karst_init(cfg);

    /*
     *************************************************************************************
     * Set a default channel
     *************************************************************************************
     */
    phy_hw_set_channel(PHY_BAND_2G4, 2437, 2437, PHY_CHNL_BW_20, 0, 0);
}

void phy_init(const struct phy_cfg_tag *config)
{
    const struct phy_karst_cfg_tag *cfg = (const struct phy_karst_cfg_tag *)&config->parameters;

    phy_hw_init(cfg);

    phy_env.cfg               = *cfg;
    phy_env.chan.band         = PHY_BAND_2G4;
    phy_env.chan.type         = PHY_CHNL_BW_OTHER;
    phy_env.chan.prim20_freq  =
    phy_env.chan.center1_freq =
    phy_env.chan.center2_freq = PHY_UNUSED;
}

void phy_mdm_isr(void)
{
    #if NX_UF_EN
    if (macbyp_int3_state_getf())
    {
        //Indicate that an unsupported HT frame has been capture
        uf_event_ind();

        //interrupt 3 ack
        macbyp_int3_ack_clearf(1);
    }
    #endif
}

/**
 * FIXME: This is for debug purpose only and does not call rd_event_ind
 */
void phy_rc_isr(void)
{
    uint32_t irq_status = riu_rwnxmacintstatmasked_get();

    riu_rwnxmacintack_clear(irq_status);

    #if NX_RADAR_DETECT
    if (irq_status & RIU_IRQMACRADARDETMASKED_BIT)
    {
        PROF_RADAR_IRQ_SET();
        rd_event_ind(PHY_PRIM);
        PROF_RADAR_IRQ_CLR();
    }
    #endif

    if (irq_status & RIU_IRQMACCCATIMEOUTMASKED_BIT)
        crm_mdm_reset();

    ASSERT_REC(!(irq_status & RIU_IRQMACCCATIMEOUTMASKED_BIT));
}

void phy_get_version(uint32_t *version_1, uint32_t *version_2)
{
    *version_1 = mdm_hdmconfig_get();
    // TODO Add version reading for other PHY elements than modem.
    *version_2 = mdm_hdmversion_get();
}

void phy_init_channel_param(struct mac_chan_op *chan, uint8_t band, uint8_t type, uint16_t prim20_freq,
                     uint16_t center1_freq, uint16_t center2_freq)
{
    if(chan)
    {
        uint32_t        chan_freq = prim20_freq;
        switch(type)
        {
            //40MHz, PrimaryLower. channel=1~9
            case 1:
                chan->type = PHY_CHNL_BW_40;
                chan->prim20_freq  = chan_freq;
                chan->center1_freq = chan_freq; //chan_freq+10;
                break;
            //40MHz, PrimaryAbove. channel=5~13
            case 2:
                chan->type = PHY_CHNL_BW_40;
                chan->prim20_freq    = chan_freq;
                chan->center1_freq   = chan_freq-10;
                break;
            //20MHz. channel=1~14
            default :
                chan->type = PHY_CHNL_BW_20;
                chan->prim20_freq    = chan_freq;
                chan->center1_freq   = chan_freq;
                break;
        }

        chan->center2_freq = 0;
        chan->band = band;
        chan->flags = 0;
        chan->tx_power = 0;
    }
}

void phy_set_channel(const struct mac_chan_op *chan, uint8_t index)
{
    if (index > 0)
    {
        dbg(D_ERR D_PHY "%s: radio %d does not exist\n", __func__, index);
        return;
    }

    if (phy_env.chan.band == chan->band &&
        phy_env.chan.type == chan->type &&
        phy_env.chan.flags == chan->flags &&
        phy_env.chan.prim20_freq == chan->prim20_freq &&
        phy_env.chan.center1_freq == chan->center1_freq &&
        phy_env.chan.center2_freq == chan->center2_freq)
    {
        dbg(D_INF D_PHY "%s: Setting same channel, do nothing\n", __func__);
        return;
    }

    phy_hw_set_channel(chan->band, chan->prim20_freq, chan->center1_freq, chan->type,
                       chan->flags, index);
    phy_env.chan = *chan;
}

void phy_get_channel(struct phy_channel_info *info, uint8_t index)
{
    if (index > 0)
    {
        dbg(D_ERR D_PHY "%s: radio %d does not exist\n", __func__, index);
    }
    // Map the band, channel type, primary channel index on info1
    info->info1 = phy_env.chan.band | (phy_env.chan.type << 8) | (phy_env.chan.prim20_freq << 16);
    // Map center freq on info2
    info->info2 = phy_env.chan.center1_freq | (phy_env.chan.center2_freq << 16);
}

void phy_stop(void)
{
    if (nxmac_current_state_getf() != HW_IDLE)
        dbg(D_ERR "%s MAC state != IDLE\n", __func__);
}

uint32_t phy_get_channel_switch_dur(void)
{
    return 1000;
}

#if NX_RADAR_DETECT
bool phy_has_radar_pulse(int rd_idx)
{

    ASSERT_ERR(rd_idx == PHY_PRIM);

    return (riu_radfifoempty_getf() == 0);
}

bool phy_get_radar_pulse(int rd_idx, struct phy_radar_pulse *pulse)
{
    ASSERT_ERR(rd_idx == PHY_PRIM);

    // Check if FIFO is empty
    if (riu_radfifoempty_getf())
        return (false);

    pulse->pulse = REG_PL_RD(__RIU_RADARFIFO_ADDR);

    return (true);
}
#endif

bool phy_vht_supported(void)
{
    #if NX_MDM_VER > 20
    return ((mdm_vht_getf() != 0) || (mdm_chbw_getf() > PHY_CHNL_BW_40));
    #else
    return true;
    #endif
}

bool phy_he_supported(void)
{
    #if NX_MDM_VER < 30
    return false;
    #else
    return (mdm_he_getf() != 0);
    #endif
}

bool phy_uf_supported(void)
{
    #if NX_UF_EN
    return true ? macbyp_version_get() >= RXV_REG_MIN_VERS : false;
    #else
    return false;
    #endif
}

void phy_uf_enable(bool enable)
{
    #if NX_UF_EN
    // enable/disable macbypass interrupt line 3
    macbyp_interrupt3_en_setf(enable);
    #endif
}

bool phy_ldpc_tx_supported(void)
{
    return (mdm_ldpcenc_getf() != 0);
}

bool phy_ldpc_rx_supported(void)
{
    return (mdm_ldpcdec_getf() != 0);
}

bool phy_bfmee_supported(void)
{
    return (mdm_bfmee_getf() != 0);
}

bool phy_bfmer_supported(void)
{
    return (mdm_bfmer_getf() != 0);
}

bool phy_mu_mimo_rx_supported(void)
{
    return (mdm_mumimorx_getf() != 0);
}

bool phy_mu_mimo_tx_supported(void)
{
    return (mdm_mumimotx_getf() != 0);
}

#if RW_MUMIMO_RX_EN
void phy_set_group_id_info(uint32_t membership_addr, uint32_t userpos_addr)
{
    int i;

    // Set membership status
    for(i=0; i<MDM_MUMIMO_GROUPID_TAB_COUNT; i++)
    {
        mdm_mumimo_groupid_tab_set(i, co_read32p(membership_addr + 4 * i));
    }

    // Set user position
    for(i=0; i<MDM_MUMIMO_USERPOSITION_TAB_COUNT; i++)
    {
        mdm_mumimo_userposition_tab_set(i, co_read32p(userpos_addr + 4 * i));
    }
}
#endif

void phy_set_aid(uint16_t aid)
{
    #if NX_MDM_VER >= 30
    mdm_hestaid_setf(0, aid);
    mdm_hestaid_setf(1, 0);
    mdm_hestaid_setf(2, 2047);
    #endif
}


uint8_t phy_get_bw(void)
{
    return (mdm_chbw_getf());
}

uint8_t phy_get_nss(void)
{
    return (mdm_nss_getf() - 1);
}

uint8_t phy_get_ntx(void)
{
    return (mdm_ntx_getf() - 1);
}

uint8_t phy_get_nrx(void)
{
    return (mdm_nrx_getf() - 1);
}

#if RW_BFMER_EN
uint8_t phy_get_bfr_mem_size(void)
{
    return (mdm_bfmer_mem_size_getf());
}
#endif

void phy_get_rf_gain_idx(int8_t *power, uint8_t *idx)
{
    if (*power > PHY_KARST_MAX_PWR)
    {
        *power = PHY_KARST_MAX_PWR;
    }
    else if (*power < PHY_KARST_MIN_PWR)
    {
        *power = PHY_KARST_MIN_PWR;
    }

    /* idx is simply the power level in dBm */
    *idx = (uint8_t)*power;
}

void phy_get_rf_gain_capab(int8_t *max, int8_t *min)
{
    *max = PHY_KARST_MAX_PWR; // dBm
    *min = PHY_KARST_MIN_PWR; // dBm
}

uint8_t phy_get_antenna_set(void)
{
    /// Default TX antenna mask
    return (CO_BIT((phy_get_ntx()) + 1) - 1);
}

#if NX_DEBUG_DUMP
void phy_get_diag_state(struct dbg_debug_info_tag *dbg_info)
{
    int     i;
    for (i = 0; i < DBG_DIAGS_PHY_MAX; i++)
    {
        // Go through the different banks and copies the diag values
        crm_phy_diagsel_l_setf(i);
        dbg_info->diags_phy[i] = crm_phy_diagval_l_getf();
    }
}
#endif

uint8_t phy_switch_antenna_paths(void)
{
    #if 0
    PROF_ANT_DIV_SWITCH_SET();
    uint8_t value = karst_fb_pathmux_sel_getf();
    if (value == 0)
        value = 1;
    else
        value = 0;
    karst_fb_pathmux_sel_setf(value);
    PROF_ANT_DIV_SWITCH_CLR();

    return value;
    #else
    return 0;
    #endif
}

void phy_wakeup_reinit()
{
    struct phy_env_tag      phy_env_sleep  ;
    struct phy_cfg_tag      *config;

    //memcpy(&phy_env_sleep, &phy_env, sizeof(struct phy_env_tag));
    phy_env_sleep = phy_env;

    config = (struct phy_cfg_tag *)&(phy_env_sleep.cfg);

    phy_init(config);

    phy_set_channel(&phy_env_sleep.chan, PHY_PRIM);

}

void phy_update_chan_freq(struct mac_chan_op *chan, uint8_t bw_mode, uint8_t chan_idx)
{
    uint32_t        chan_freq;
    chan_freq = phy_channel_to_freq(PHY_BAND_2G4, chan_idx);
    switch(bw_mode)
    {
        //40MHz, PrimaryLower. channel=1~9
        case 1  :   chan->type = PHY_CHNL_BW_40;
                    chan->prim20_freq  = chan_freq;
                    chan->center1_freq = chan_freq+10;
                    break;
        //40MHz, PrimaryAbove. channel=5~13
        case 2  :   chan->type = PHY_CHNL_BW_40;
                    chan->prim20_freq    = chan_freq;
                    chan->center1_freq   = chan_freq-10;
                    break;
        //20MHz. channel=1~14
        default :   chan->type = PHY_CHNL_BW_20;
                    chan->prim20_freq    = chan_freq;
                    chan->center1_freq   = chan_freq;
                    break;
    }
}

void macbyp_tx_ppdu(struct phy_ppdu_format *ppdu)
{
    uint32_t            cfg ;
    struct mac_chan_op  chan;

    chan.band = PHY_BAND_2G4;
    phy_update_chan_freq(&chan, ppdu->bw_mode, ppdu->chan_idx);
    chan.center2_freq = 0;
    chan.flags = 0;
    chan.tx_power = ppdu->tx_pwr;

    phy_set_channel(&chan, PHY_PRIM);

    //config macbypass
    macbyp_clken_set(0x1);
    macbyp_payload_set(ppdu->pld_type << 16);
    macbyp_frameperburst_set(ppdu->num);
    macbyp_interframe_delay_set(30*100);        //25~100us

    switch(ppdu->modf)
    {
        //non-ht
        case 0  :   macbyp_txv_set(0, ppdu->pre_gi << 7);
                    macbyp_txv_set(4, ppdu->len & 0xff);
                    cfg = (ppdu->len >> 8) & 0xff;
                    switch(ppdu->mcs)
                    {
                        case 0  :
                        case 1  :
                        case 2  :
                        case 3  :   cfg |= (ppdu->mcs << 4);  break;
                        case 4  :   cfg |= 0xb0;    break;
                        case 5  :   cfg |= 0xf0;    break;
                        case 6  :   cfg |= 0xa0;    break;
                        case 7  :   cfg |= 0xe0;    break;
                        case 8  :   cfg |= 0x90;    break;
                        case 9  :   cfg |= 0xd0;    break;
                        case 10 :   cfg |= 0x80;    break;
                        case 11 :   cfg |= 0xc0;    break;
                        default :   cfg |= 0xb0;    break;
                    }
                    macbyp_txv_set(5, cfg);
                    macbyp_txv_set(8, 0x0);
                    macbyp_txv_set(9, 0x0);
                    break;
        //ht-20
        case 1  :
        //ht-40
        case 2  :   macbyp_txv_set(0, 0x2 | ((ppdu->modf == 2) << 4));
                    macbyp_txv_set(4, 0x00);
                    macbyp_txv_set(5, 0xb0);
                    macbyp_txv_set(8, ppdu->pre_gi << 2);
                    macbyp_txv_set(9, 0x0);
                    macbyp_txv_set(10, ppdu->mcs);
                    macbyp_txv_set(11, ppdu->len & 0xff);
                    macbyp_txv_set(12, (ppdu->len >> 8) & 0xff);
                    break;
        //he20-su
        case 3  :   macbyp_txv_set(0, 0x5);
                    macbyp_txv_set(4, 0x00);
                    macbyp_txv_set(5, 0x00);
                    macbyp_txv_set(8, ppdu->pre_gi << 2);
                    macbyp_txv_set(9, 0x0);
                    macbyp_txv_set(10, 0x0);
                    macbyp_txv_set(11, 0x0);
                    macbyp_txv_set(12, 0x0);
                    macbyp_txv_set(13, 0x0);
                    macbyp_txv_set(14, ppdu->mcs);
                    macbyp_txv_set(15, ppdu->len & 0xff);
                    macbyp_txv_set(16, (ppdu->len >> 8) & 0xff);
        default :   break;
    }
    macbyp_txv_set(1, 0x1);
    macbyp_txv_set(2, ppdu->tx_pwr);
    cfg = 0x1 | ((!ppdu->len) << 7);
    macbyp_txv_set(3, cfg);
    macbyp_txv_set(6, 0x0);
    macbyp_txv_set(7, 0x0);

    macbyp_ctrl_set(0x301);      //startup transmit

}

void macbyp_rx_ppdu(uint8_t bw_mode, uint8_t chan_idx)
{
    struct mac_chan_op  chan;

    //set channel
    chan.band = PHY_BAND_2G4;
    phy_update_chan_freq(&chan, bw_mode, chan_idx);
    chan.center2_freq = 0;
    chan.flags = 0;
    chan.tx_power = 10;

    phy_set_channel(&chan, PHY_PRIM);

    //start receive
    macbyp_clken_set(0x1);
    macbyp_ctrl_set(0x103);      //startup receive
}

void print_rx_ppdu_info(void *p)
{
#if NX_PRINT != NX_PRINT_NONE
    uint32_t            fcs_ok  ;
    uint32_t            fcs_bad ;
    uint32_t            rx_end  ;
    uint32_t            rx_err  ;

    fcs_ok = macbyp_stat_frame_ok_get();
    fcs_bad = macbyp_stat_frame_bad_get();
    rx_end = macbyp_stat_rxend_get();
    rx_err = macbyp_stat_rxerror_get() + macbyp_stat_phyerr_get();
#endif

    REG_PL_WR(MACBYP_CTRL_ADDR, REG_PL_RD(MACBYP_CTRL_ADDR) | MACBYP_CLEAR_STAT_BIT);
    udelay(2);
    REG_PL_WR(MACBYP_CTRL_ADDR, REG_PL_RD(MACBYP_CTRL_ADDR) & ~ MACBYP_CLEAR_STAT_BIT);

    dbg(D_CRT D_PHY "Rx PPDU Infomation:\r\n");
    dbg(D_CRT D_PHY "    FCS OK Number  : %0d (%0d %%)\r\n", fcs_ok , (100*fcs_ok)/rx_end);
    dbg(D_CRT D_PHY "    FCS Err Number : %0d (%0d %%)\r\n", fcs_bad, (100*fcs_bad)/rx_end);
    dbg(D_CRT D_PHY "    Rx Err Number  : %0d (%0d %%)\r\n", rx_err , (100*rx_err)/rx_end);
	dbg(D_CRT D_PHY "\r\n");
}

void macbyp_disable()
{
    macbyp_ctrl_set(0x0);
}

void rcb_tx_sinwave(uint8_t enable, uint8_t freq)
{
#if 0
    uint32_t        sin_f   ;
    uint32_t        cfg     ;

    if(enable)
    {
        //set sinwave freq
        sin_f = (2048*freq)/40;
        SET_REG_FIELD(rc_inst->REG0XA, RC_TX_SIN_FREQ, sin_f);

        //set xmit pattern
        SET_REG_FIELD(rc_inst->REG0X8, RC_TX_PATTERN, 0x2U);

        //set force xmit enable
        rc_inst->REG0X1 = 0x00002009;
    }
    else
    {
        SET_REG_FIELD(rc_inst->REG0X8, RC_TX_PATTERN, 0x0U);
        rc_inst->REG0X1 = 0x00002000;
    }
#endif
}

void rcb_tx_triangle(uint8_t enable, uint8_t amp)
{
#if 0
    if(enable)
    {
        //set wave amplitude
        SET_REG_FIELD(rc_inst->REG0XA, RC_TX_TRIANGLE_AMP, amp);
        SET_REG_FIELD(rc_inst->REG0X8, RC_TX_DPD_ADDR_SCALE, amp);
        SET_REG_FIELD(rc_inst->REG0X8, RC_TX_DPD_EN, 0x1U);

        //set xmit pattern
        SET_REG_FIELD(rc_inst->REG0X8, RC_TX_PATTERN, 0x3U);

        //set force xmit enable
        rc_inst->REG0X1 = 0x00002009;
    }
    else
    {
        SET_REG_FIELD(rc_inst->REG0X8, RC_TX_PATTERN, 0x0U);
        rc_inst->REG0X1 = 0x00002000;
    }
#endif
}

#define     PHY_LA_SMP_LEN      0x2000

void phy_la_sample(uint8_t source, uint32_t *p)
{
    uint32_t        diag_sel;
    switch(source)
    {
        //RF ADC Data
        case 0  :    diag_sel = 79 + (80 << 8) + (2 << 16);
                    p[0] = 0x02000200;
                    p[1] = 0x02000200;
                    p[2] = 0;
                    break;
        //RF DAC Data
        case 1  :    diag_sel = 83 + (84 << 8) + (2 << 16);
                    p[0] = 0x10001000;
                    p[1]  = 0x10001000;
                    p[2] = 0;
                    break;
        //FE ADC Data
        case 2  :    diag_sel = 81 + (82 << 8) + (2 << 16);
                    p[0] = 0x10001000;
                    p[1] = 0x10001000;
                    p[2] = 0;
                    break;
        //FE DAC Data
        case 3  :    diag_sel = 85 + (86 << 8) + (2 << 16);
                    p[0] = 0x10001000;
                    p[1]  = 0x10001000;
                    p[2] = 0;
                    break;
        //Default
        default :    diag_sel = 79 + (80 << 8) + (2 << 16);
                    p[0] = 0x02000200;
                    p[1]  = 0x02000200;
                    p[2] = 0;
                    break;
    }
    crm_phy_diagsel_set(diag_sel);
}


#if TEST_AHB_ACCESS_PHY_MEM_EN
int phy_check_mem_rw()
{
    int                 i;
    volatile int        j;
    uint32_t            *p_mem;
    int                 err_cnt = 0;

    #if (BK_NX_DPD_EN == 0)
    dpd_table_init();
    #endif

    #if (BK_NX_PWRTBL_EN == 0)
    pow_table_init();
    #endif

    #if (BK_NX_PEAK_CANCEL == 0)
    peakcw_cfgmem_init();
    #endif

    #if (RW_NX_LDPC_DEC == 0)
    ldpcrx_cfgmem_init();
    #endif

    //check agc mem
    p_mem = (uint32_t *)AGCMEM_BASE;

    riu_agcfsmreset_setf(0x1);
    crm_agcmemclkforce_setf(0x1);

    for(j = 0; j < 10; j ++);

	for(i = 0; i < 512; i ++)
		if(p_mem[i] != agcmem_init_value[i])
          err_cnt ++;

    for(j = 0; j < 10; j ++);

    crm_agcmemclkforce_setf(0x0);
    riu_agcfsmreset_setf(0x0);

    if(err_cnt > 0)
        dbg(D_ERR D_PHY "ERR: AGC\r\n");
#if 0
    //check power table memory
    for(j = 0; j < 1000; j ++);
    err_cnt = 0;
    p_mem = (uint32_t *)POWTBL_BASE;
    rc_inst->REG0X8 &= ~ BIT_RC_TX_POWTBL_EN;

    for(j = 0; j < 10; j ++);

    for(i = 0; i < 128; i ++)
        if(p_mem[i] != powtable_init_value[i])
            err_cnt ++;

    for(j = 0; j < 10; j ++);

    rc_inst->REG0X8 |=  BIT_RC_TX_POWTBL_EN;

    if(err_cnt > 0)
        dbg(D_ERR D_PHY "ERR: POWTBL\r\n");

    //check dpd table memory
    for(j = 0; j < 1000; j ++);

    err_cnt = 0;
    p_mem = (uint32_t *)DPDTBL_BASE;
    rc_inst->REG0X8 &= ~ BIT_RC_TX_DPD_EN;

    for(j = 0; j < 10; j ++);

    for(i = 0; i < 128; i ++)
        if(p_mem[i] != dpdtable_init_value[i])
            err_cnt ++;

    for(j = 0; j < 10; j ++);

    rc_inst->REG0X8 |=  BIT_RC_TX_DPD_EN;
#endif
    if(err_cnt > 0)
        dbg(D_ERR D_PHY "ERR: DPDTBL\r\n");

    //check ldpcrxcfg memory
    err_cnt = 0;
    for(j = 0; j < 1000; j ++);
    p_mem = (uint32_t *)(LDPCRXCFGMEM_BASE);

	for(i = 0; i < 335; i ++)
        if(p_mem[i] != ldpcrxcfgmem_init_value[i])
            err_cnt ++;

    if(err_cnt > 0)
        dbg(D_ERR D_PHY "ERR: LDPCRXCFG\r\n");


    //check peak cancel wave memory
    err_cnt = 0;
    for(j = 0; j < 1000; j ++);

    p_mem = (uint32_t *)(PEAKCWMEM_BASE);
    mdm_peakcancelbypass_setf(1);

    for(j = 0; j < 10; j ++);

	for(i = 0; i < 256; i ++)
        if(p_mem[i] != (peakcw_init_value[i]*65793))
            err_cnt ++;

    for(j = 0; j < 10; j ++);
    mdm_peakcancelbypass_setf(0);

    if(err_cnt > 0)
        dbg(D_ERR D_PHY "ERR: PEAK CANCEL\r\n");

    return err_cnt;
}

#endif

//cca ctrl
uint8_t phy_open_cca(void)
{
    //agc_rwnxagcccactrl_set((agc_rwnxagcccactrl_get() & ~0x00000fff) | 0x00000377);

	return 0;
}

uint8_t phy_close_cca(void)
{
    //agc_rwnxagcccactrl_set((agc_rwnxagcccactrl_get() & ~0x00000fff) & ~(0x00000377));

	return 0;
}

uint8_t phy_show_cca(void)
{
	return 0;
}


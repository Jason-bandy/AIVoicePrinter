#include "include.h"
#include "arm_arch.h"
#include "mac_phy_bypass.h"
#include "mac_phy_bypass_pub.h"
#include "drv_model_pub.h"
#include "uart_pub.h"

#if CFG_MAC_PHY_BAPASS
static SDD_OPERATIONS mpb_op = {
            mpb_ctrl
};

void mpb_init(void)
{
	sddev_register_dev(MPB_DEV_NAME, &mpb_op);
}

void mpb_exit(void)
{
	sddev_unregister_dev(MPB_DEV_NAME);
}

static UINT32 mpb_select_tx_rate(UINT32 rate)
{
    UINT32 param = rate;
    
    switch(rate)
    {
        case 1 :	param = 0x0;	break;  // 1Mbps
        case 2 :	param = 0x1;	break;  // 2Mbps
        case 5 :	param = 0x2;	break;	// 5.5Mbps
        case 11:	param = 0x3;	break;	// 11Mbps
        case 6 :	param = 0xb;	break;	// 6Mbps
        case 9 :	param = 0xf;	break;	// 9Mbps
        case 12:	param = 0xa;	break;	// 12Mbps
        case 18:	param = 0xe;	break;	// 18Mbps
        case 24:	param = 0x9;	break;	// 24Mbps
        case 36:	param = 0xd;	break;	// 36Mbps
        case 48:	param = 0x8;	break;	// 48Mbps
        case 54:	param = 0xc;	break;	// 54Mbps
        default: {
            #if CFG_IEEE80211AX
            if(rate >= 128 && rate <=137)  //mcs0- mcs9
            #else
            if(rate >= 128 && rate <=135) 
            #endif
            {
                param -= 128;
            }
            else {
                os_printf("mpb_select_tx_rate wrong rate:%d\r\n", rate);
            }
        }
    }  

    os_printf("mpb_select_tx_rate rate:%d\r\n", param);
    return param;
}

#if CFG_IEEE80211AX
#include "reg_macbypass.h"
//0: all zeros;  1:incr; 2:rand
#define TX_PLD_ALL_0    0
#define TX_PLD_INCR     1
#define TX_PLD_RAND     2
UINT32 g_mod_format = 0;

static void mpb_set_tx_packet_num(UINT32 *num_ptr)
{
    if(num_ptr)
    {
        UINT32 reg;
        UINT32 num = (*(UINT32*)num_ptr);
        
        reg = macbyp_frameperburst_get();
        reg &= ~MACBYP_NFRAME_MASK;
        reg |= ((num << MACBYP_NFRAME_LSB) & MACBYP_NFRAME_MASK);
        macbyp_frameperburst_set(reg);
    }
}

static void mpb_set_tx_payload_type(UINT32 *type_ptr)
{
    if(type_ptr)
    {
        //0: all zeros;  1:incr;  2:rand
        UINT32 reg;
        UINT32 type = (*(UINT32*)type_ptr);

        reg = macbyp_payload_get();
        reg &= ~MACBYP_PAYLOAD_TX_MASK;
        reg |= ((type << MACBYP_PAYLOAD_TX_LSB) & MACBYP_PAYLOAD_TX_MASK);
        macbyp_payload_set(reg);
    }
}

static void mpb_en_init(void)
{
    UINT32 i, reg;
    macbyp_clken_set(0x1);

    for(i=0; i<MACBYP_TXV_COUNT; i++)
        macbyp_txv_set(i, 0);

    reg = 0;
    reg |= ((TX_PLD_INCR << MACBYP_PAYLOAD_TX_LSB) & MACBYP_PAYLOAD_TX_MASK);
    macbyp_payload_set(reg); // def payload incr

    reg = 0;
    reg |= ((0 << MACBYP_NFRAME_LSB) & MACBYP_NFRAME_MASK);
    macbyp_frameperburst_set(reg);  // def packet num, no limited

    //set Number of Transmit Chains.
    reg = macbyp_txv_get(3);
    reg |= (1 << 0);
    macbyp_txv_set(3, reg);

    //set Antenna Set.
    reg = macbyp_txv_get(1);
    reg |= 0x1;
    macbyp_txv_set(1, reg);
}

static void mpb_start_trx(void)
{
    UINT32 reg;
    reg = macbyp_ctrl_get();
    reg &= ~(MACBYP_MODE_MASK);
    reg &= ~(MACBYP_BYPASS_BIT);
    reg |= (((3) << MACBYP_MODE_LSB) & MACBYP_MODE_MASK);
    reg |= MACBYP_BYPASS_BIT;
	macbyp_ctrl_set(reg);
}

static void mpb_stop_trx(void)
{
    UINT32 reg;
    reg = macbyp_ctrl_get();
    reg &= ~(MACBYP_BYPASS_BIT);
	macbyp_ctrl_set(reg); 
}

void mpb_set_txdelay(UINT32 delay_us)
{
    UINT32 delay_us_value;
#if 0   
    // only has 120M clock
    delay_us_value = (UINT32)(delay_us * 120);

    if(delay_us_value > MACBYP_INTERFRAME_DELAY_MASK)
        delay_us_value = MACBYP_INTERFRAME_DELAY_MASK;

    macbyp_interframe_delay_set(delay_us_value);
#else
    macbyp_interframe_delay_set(30 * 100);
#endif
}

void mpb_set_txdelay_precision(float delay_us)
{
#if 0
    UINT32 delay_us_value;
    
    // only has 120M clock
    delay_us_value = (UINT32)(delay_us * 120 + 0.5);

    if(delay_us_value > MACBYP_INTERFRAME_DELAY_MASK)
        delay_us_value = MACBYP_INTERFRAME_DELAY_MASK;

    macbyp_interframe_delay_set(delay_us_value);
#else
    macbyp_interframe_delay_set(30 * 100);
#endif
}

UINT32 mpb_ctrl(UINT32 cmd, void *param)
{
	UINT32 len, reg;
	
	switch(cmd)
	{
		case MCMD_TX_LEGACY_SET_LEN:
			len = (*(UINT32*)param) & TX_LEGACY_DATA_LEN_MASK;

            if(len)
            {
                reg = macbyp_txv_get(4);
                reg &= ~(0xff);
                reg |= len & 0xff;
                macbyp_txv_set(4, reg);

                reg = macbyp_txv_get(5);
                reg &= ~(0xf);
                reg |= (len >> 8) & 0xf;
                macbyp_txv_set(5, reg);
            }
            else
            {
                // continuous transmit mode, with infinite frame length
                reg = macbyp_txv_get(3);
                reg |= (1<<7);
                macbyp_txv_set(3, reg);
            }
			break;

		case MCMD_TX_HT_VHT_SET_LEN:
			len = (*(UINT32*)param) & TX_HT_VHT_DATA_LEN_MASK;

            if(len)
            {
                reg = macbyp_txv_get(11);
                reg &= ~(0xff);
                reg |= len & 0xff;
                macbyp_txv_set(11, reg);

                reg = macbyp_txv_get(12);
                reg &= ~(0xff);
                reg |= (len >> 8) & 0xff;
                macbyp_txv_set(12, reg);
            }
            else
            {
                // continuous transmit mode, with infinite frame length
                reg = macbyp_txv_get(3);
                reg |= (1<<7);
                macbyp_txv_set(3, reg);
            }
			break;
        case MCMD_TX_HE_SET_LEN:
			len = (*(UINT32*)param) & TX_HE_DATA_LEN_MASK;

            if(len)
            {
                reg = macbyp_txv_get(15);
                reg &= ~(0xff);
                reg |= len & 0xff;
                macbyp_txv_set(15, reg);

                reg = macbyp_txv_get(16);
                reg &= ~(0xff);
                reg |= (len >> 8) & 0xff;
                macbyp_txv_set(16, reg);

                reg = macbyp_txv_get(17);
                reg &= ~(0xf);
                reg |= (len >> 16) & 0xf;
                macbyp_txv_set(17, reg);
            }
            else
            {
                // continuous transmit mode, with infinite frame length
                reg = macbyp_txv_get(3);
                reg |= (1<<7);
                macbyp_txv_set(3, reg);
            }
			break;
			
		case MCMD_TX_MODE_BYPASS_MAC:
			//mpb_tx_mode();
			break;
			
		case MCMD_RX_MODE_BYPASS_MAC:
			//mpb_rx_mode();
			break;

        case MCMD_STOP_BYPASS_MAC:
            mpb_stop_trx();
            break;

        case MCMD_START_BYPASS_MAC:
            mpb_start_trx();
            break;

        case MCMD_SET_BANDWIDTH:
            len = (*(UINT32*)param);
            reg = macbyp_txv_get(0);
            reg &= ~(0x7 << 4);
            reg |= ((len & 0x7) << 4);
            macbyp_txv_set(0, reg);
            break; 

        case MCMD_SET_GI:  //0x0: 800ns;  0x1: 400ns
            len = (*(UINT32*)param);
            if(g_mod_format == 0)
            {
                // For formatMod = NON-HT and the transmission modulation is DSSS/CCK,
                // 0: Short Preamble, 1: Long Preamble
                //reg = macbyp_txv_get(0);
                //reg &= ~(0x1 << 7);
                //reg |= ((len & 0x1) << 7);
                //macbyp_txv_set(0, reg);
            }
            else
            {
                //Guard Interval Type
                // 0: LONG_GI (800 ns), 1: SHORT_GI (400 ns)
                reg = macbyp_txv_get(8);
                reg &= ~(0x1 << 2);
                reg |= ((len & 0x1) << 2);
                macbyp_txv_set(8, reg);
            }
            break;
/*
b0000: NON-HT,  b0001: NON-HT-DUP-OFDM,  b0010: HT-MM
b0011: HT-GF,   b0100: VHT,              b0101: HE-SU
b0110: HE-MU,   b0111: HE-EXT-SU,        b1000: HE-TB
*/
        // for modulate format: 0x0: Non-HT; 0x1:Non-HT-DUP; 0x2: HT-MM;  0x3: HT-GF    
        // for rate:  0-11: b to g,  mcs 0-7:  MCS0 =128, MCS1=129 to CS7=135.
 		case MCMD_BYPASS_TX_SET_RATE_MFORMAT: {
            if(param)
            {
                MBPS_TXS_MFR_ST st =(*(MBPS_TXS_MFR_PTR)param);
                g_mod_format = st.mod_format;
                
                UINT32 is_dsss_cck = 0;
                if((st.rate == 1) && (g_mod_format == 0))
                    is_dsss_cck = 1;
                
                st.rate =  mpb_select_tx_rate(st.rate);
                
                // for Format and Modulation
                reg = macbyp_txv_get(0);
                reg &= ~(0xf << 0);
                reg |= ((g_mod_format & 0xf) << 0);
                reg &= ~(1 << 7);
                if(is_dsss_cck == 1)
                {
                    // For formatMod = NON-HT and the transmission modulation is DSSS/CCK,
                    // should Long Preamble
                    reg |= (1 << 7);
                }
                macbyp_txv_set(0, reg);
                                    
                // for rate
                if(g_mod_format < 2)
                {
                    reg = macbyp_txv_get(5);
                    reg &= ~(0xf << 4);
                    reg |= ((st.rate & 0xf) << 4);
                    macbyp_txv_set(5, reg);
                }
                else if(g_mod_format < 5)
                {
                    // HT, VHT or HE PPDU, always indicates 6 Mbps-0xb
                    reg = macbyp_txv_get(5);
                    reg &= ~(0xf << 4);
                    reg |= ((0xb & 0xf) << 4);
                    macbyp_txv_set(5, reg);

                    // Modulation Coding Scheme
                    reg = macbyp_txv_get(10);
                    reg &= ~(0x7f << 0);
                    reg |= ((st.rate & 0x7f) << 0);
                    macbyp_txv_set(10, reg);
                }
                else 
                {
                    reg = macbyp_txv_get(5);
                    reg &= ~(0xf << 4);
                    reg |= ((0xb & 0xf) << 4);
                    macbyp_txv_set(5, reg);

                    // Modulation Coding Scheme
                    reg = macbyp_txv_get(14);
                    reg &= ~(0xf << 0);
                    reg |= ((st.rate & 0xf) << 0);
                    macbyp_txv_set(14, reg);
                }
            }
            break; 
            }

        case MCMD_SET_TXDELAY:
            mpb_set_txdelay(*(UINT32*)param);
            break;

        case MCMD_INIT_BYPASS_MAC:
            mpb_en_init();
            break;

        case MCMD_BYPASS_MAC_SET_TX_PLD_TYPE:
            mpb_set_tx_payload_type(param);
            break;

        case MCMD_BYPASS_MAC_SET_TX_PKT_NUM:
			mpb_set_tx_packet_num(param);
            break;
		default:
			break;
	}
	
	return 0;
}
#else
UINT32 reg_134 = 0x00;
UINT32 reg_135 = 0xc4;
UINT32 reg_138 = 0x00;
UINT32 reg_139 = 0x10;
UINT32 reg_140 = 0x00;
UINT32 reg_129 = 0x00;
UINT32 reg_132 = 0x80;
UINT32 reg_133 = 0x00;
UINT32 g_band = 0;

#include "mac_phy_bypass.h"
struct MPB_TypeDef mpb_regs =
{
    (volatile MPB_REG0x0_TypeDef  *)(MPB_ADDR_BASE + 0 * 4),
    (volatile MPB_REG0x1_TypeDef  *)(MPB_ADDR_BASE + 1 * 4),
    (volatile MPB_REG0x2_TypeDef  *)(MPB_ADDR_BASE + 2 * 4),
    (volatile MPB_REG0x3_TypeDef  *)(MPB_ADDR_BASE + 3 * 4),
    (volatile MPB_REG0x4_TypeDef  *)(MPB_ADDR_BASE + 4 * 4),
    (volatile MPB_REG0x8_TypeDef  *)(MPB_ADDR_BASE + 8 * 4),
    (volatile MPB_REG0x9_TypeDef  *)(MPB_ADDR_BASE + 9 * 4),
    (volatile MPB_REG0xA_TypeDef  *)(MPB_ADDR_BASE + 10 * 4),    
    (volatile MPB_REG0xB_TypeDef  *)(MPB_ADDR_BASE + 11 * 4),
    
    (volatile MPB_REG0x80_TypeDef  *)(MPB_ADDR_BASE + 128 * 4),
    (volatile MPB_REG0x81_TypeDef  *)(MPB_ADDR_BASE + 129 * 4),
    (volatile MPB_REG0x82_TypeDef  *)(MPB_ADDR_BASE + 130 * 4),
    (volatile MPB_REG0x83_TypeDef  *)(MPB_ADDR_BASE + 131 * 4),
    (volatile MPB_REG0x84_TypeDef  *)(MPB_ADDR_BASE + 132 * 4),
    (volatile MPB_REG0x85_TypeDef  *)(MPB_ADDR_BASE + 133 * 4),
    (volatile MPB_REG0x86_TypeDef  *)(MPB_ADDR_BASE + 134 * 4),
    (volatile MPB_REG0x87_TypeDef  *)(MPB_ADDR_BASE + 135 * 4),
    (volatile MPB_REG0x88_TypeDef  *)(MPB_ADDR_BASE + 136 * 4),
    (volatile MPB_REG0x89_TypeDef  *)(MPB_ADDR_BASE + 137 * 4),
    (volatile MPB_REG0x8A_TypeDef  *)(MPB_ADDR_BASE + 138 * 4),
    (volatile MPB_REG0x8B_TypeDef  *)(MPB_ADDR_BASE + 139 * 4),
    (volatile MPB_REG0x8C_TypeDef  *)(MPB_ADDR_BASE + 140 * 4),
    (volatile MPB_REG0x8D_TypeDef  *)(MPB_ADDR_BASE + 141 * 4),
    (volatile MPB_REG0x8E_TypeDef  *)(MPB_ADDR_BASE + 142 * 4),
    (volatile MPB_REG0x8F_TypeDef  *)(MPB_ADDR_BASE + 143 * 4),
};

void mpb_tx_mode(void)
{
    mpb_regs.r0->value   = 0x00;
    mpb_regs.r128->value = 0x34;
    mpb_regs.r129->value = reg_129;
    mpb_regs.r130->value = 0x00;
    mpb_regs.r131->value = 0x00;
    mpb_regs.r132->value = reg_132;
    mpb_regs.r133->value = reg_133;
    mpb_regs.r134->value = reg_134;
    mpb_regs.r135->value = reg_135;
    mpb_regs.r136->value = 0x00;
    mpb_regs.r137->value = 0x00;
    mpb_regs.r138->value = reg_138;
    mpb_regs.r139->value = reg_139;
    mpb_regs.r140->value = reg_140;
    mpb_regs.r141->value = 0x00;
    mpb_regs.r142->value = 0x00;
    mpb_regs.r143->value = 0xff;
    mpb_regs.r3->value   = 0x177; ///0xEA6;//0xEA6,177
    mpb_regs.r2->value   = 0x10;
}

void mpb_stop_trx(void)
{
	mpb_regs.r0->value &= (~0x01);
}

void mpb_start_trx(void)
{
	mpb_regs.r0->value |= 0x01;
}

void mpb_set_txdelay(UINT32 delay_us)
{
    UINT32 delay_us_value;

    if(g_band == 1)
        delay_us_value = delay_us * 60;
    else
        delay_us_value = delay_us * 30;

    if(delay_us_value > 0xfffff)
    delay_us_value = 0xfffff;

    mpb_regs.r3->value = delay_us_value;
}

void mpb_set_txdelay_precision(float delay_us)
{
    UINT32 delay_us_value;

    if (g_band == 1)
    {
        delay_us_value = (UINT32)(delay_us * 60 + 0.5);
    }
    else
    {
        delay_us_value = (UINT32)(delay_us * 30 + 0.5);
    }

    if (delay_us_value > 0xfffff)
    {
        delay_us_value = 0xfffff;
    }

    mpb_regs.r3->value = delay_us_value;
}

UINT32 mpb_ctrl(UINT32 cmd, void *param)
{
	UINT32 len;
	
	switch(cmd)
	{
		case MCMD_TX_LEGACY_SET_LEN:
			len = (*(UINT32*)param);
            reg_134 &= ~(0xff);
            reg_135 &= ~(0xf);
			reg_134 |= len & 0xff;
			reg_135 |= (len >> 8) & 0xf;
			break;

		case MCMD_TX_HT_VHT_SET_LEN:
			len = (*(UINT32*)param);
            reg_138 &= ~(0xff);
            reg_139 &= ~(0xff);
            reg_140 &= ~(0xf);
			reg_138 |= len & 0xff;
			reg_139 |= (len >> 8) & 0xff;
			reg_140 |= (len >> 16) & 0xf;
			break;

		case MCMD_TX_MODE_BYPASS_MAC:
			mpb_tx_mode();
			break;

		case MCMD_RX_MODE_BYPASS_MAC:
			//mpb_rx_mode();
			break;

        case MCMD_STOP_BYPASS_MAC:
            mpb_stop_trx();
            break;

        case MCMD_START_BYPASS_MAC:
            mpb_start_trx();
            break;

        case MCMD_SET_BANDWIDTH:
            reg_129 &= (~(PPDU_BANDWIDTH_MASK << PPDU_BANDWIDTH_POSI));
            reg_129 |= (((*(UINT32*)param)&&PPDU_BANDWIDTH_MASK)<< PPDU_BANDWIDTH_POSI); 
            g_band = (*(UINT32*)param);
            break; 

        case MCMD_SET_GI:  //0x0: 800ns;  0x1: 400ns
            reg_140 &= (~(0x1 << 6));
            reg_140 |= (((*(UINT32*)param)&&0x1)<< 6);
            break;

        // for modulate format: 0x0: Non-HT; 0x1:Non-HT-DUP; 0x2: HT-MM;  0x3: HT-GF    
        // for rate:  0-11: b to g,  mcs 0-7:  MCS0 =128, MCS1=129 to CS7=135.
 		case MCMD_BYPASS_TX_SET_RATE_MFORMAT: {
            MBPS_TXS_MFR_ST st =(*(MBPS_TXS_MFR_PTR)param);

            st.rate =  mpb_select_tx_rate(st.rate);

            reg_132 &= ~(0xff);
            reg_132 |= (0x80 | st.rate);

            reg_135 &= ~(0xf0);
            if(st.mod_format >= 0x2) {
                reg_135 |= 0xb0;
            }
            else {
			    reg_135 |= (st.rate & PPDU_RATE_MASK) << PPDU_RATE_POSI;
            }

            reg_133 = st.mod_format;
			break; 
 		    }
        
        case MCMD_SET_TXDELAY:
            mpb_set_txdelay(*(UINT32*)param);
            break;
			
		default:
			break;
	}
	
	return 0;
}

#endif // CFG_IEEE80211AX
#endif // CFG_MAC_PHY_BAPASS
// eof

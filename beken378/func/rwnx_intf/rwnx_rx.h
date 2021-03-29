#ifndef __FHOST_MSDU_H__
#define __FHOST_MSDU_H__

#include "txl_buffer.h"
#include "rw_msdu.h"

#define CFG_TXQ_POOL_NUM                    32
#define CFG_RX_BUF_POOL_NUM                 32
#define CFG_RX_UF_BUF_POOL_NUM              2
#define CFG_MSDU_MAX_LEN                    1600    /* TBD: need to be an A-MSDU size */

struct fhost_txdesc {
	struct txdesc txdesc;    /* make it first element */
	struct txl_buffer_tag txl_buf;
	struct tx_hw_desc hwdesc;
	struct pbuf *p;
	void *args;
	uint8_t vif_idx;
	uint8_t sta_idx;
	bool sync;
};

struct fhost_rx_vect {
    /** Total length for the MPDU transfer */
    uint32_t len                   :16;

    uint32_t reserved              : 8;

    /** AMPDU Status Information */
    uint32_t mpdu_cnt              : 6;
    uint32_t ampdu_cnt             : 2;

    /** TSF Low */
    uint32_t tsf_lo;
    /** TSF High */
    uint32_t tsf_hi;

    /// Contains the bytes 4 - 1 of Receive Vector 1
    uint32_t            recvec1a;
    /// Contains the bytes 8 - 5 of Receive Vector 1
    uint32_t            recvec1b;
    /// Contains the bytes 12 - 9 of Receive Vector 1
    uint32_t            recvec1c;
    /// Contains the bytes 16 - 13 of Receive Vector 1
    uint32_t            recvec1d;
    /// Contains the bytes 4 - 1 of Receive Vector 2
    uint32_t            recvec2a;
    ///  Contains the bytes 8 - 5 of Receive Vector 2
    uint32_t            recvec2b;

    /** Status **/
    uint32_t    rx_vect2_valid     : 1;
    uint32_t    resp_frame         : 1;
    /** Decryption Status */
    uint32_t    decr_status        : 3;
    uint32_t    rx_fifo_oflow      : 1;

    /** Frame Unsuccessful */
    uint32_t    undef_err          : 1;
    uint32_t    phy_err            : 1;
    uint32_t    fcs_err            : 1;
    uint32_t    addr_mismatch      : 1;
    uint32_t    ga_frame           : 1;
    uint32_t    current_ac         : 2;

    uint32_t    frm_successful_rx  : 1;
    /** Descriptor Done  */
    uint32_t    desc_done_rx       : 1;
    /** Key Storage RAM Index */
    uint32_t    key_sram_index     : 10;
    /** Key Storage RAM Index Valid */
    uint32_t    key_sram_v         : 1;
    uint32_t    type               : 2;
    uint32_t    subtype            : 4;
};

//// sizeof(fhost_rx_header) == RXL_HEADER_INFO_LEN,
//// refer struct rx_dmadesc{}.hd.frmlen.
struct fhost_rx_header {
    /** RX vector */
    struct fhost_rx_vect hwvect;

    /** PHY channel information */
    struct phy_channel_info phy_info;

    /** RX flags */
    uint32_t    flags_is_amsdu     : 1;
    uint32_t    flags_is_80211_mpdu: 1;
    uint32_t    flags_is_4addr     : 1;
    uint32_t    flags_new_peer     : 1;
    uint32_t    flags_user_prio    : 3;
    uint32_t    flags_rsvd0        : 1;
    uint32_t    flags_vif_idx      : 8;    // 0xFF if invalid VIF index
    uint32_t    flags_sta_idx      : 8;    // 0xFF if invalid STA index
    uint32_t    flags_dst_idx      : 8;    // 0xFF if unknown destination STA
    #if NX_AMSDU_DEAGG
    /// Array of host buffer identifiers for the other A-MSDU subframes
    uint32_t amsdu_hostids[NX_MAX_MSDU_PER_RX_AMSDU - 1];
    #endif
    #if NX_MON_DATA
    /// MAC header backup descriptor (used only for MSDU when there is a monitor and a data interface)
    struct rxu_machdrdesc mac_hdr_backup;
    #endif
    /** Pattern indicating if the buffer is available for the driver */
    uint32_t    pattern;
};

int fhost_rxbuf_push(void);
void fhost_rx_desc_handler(struct rxu_stat_val *rxstat);
void fhost_tx_cfm_push(uint8_t queue_idx, struct txdesc *txdesc);
int fhost_txbuf_push(void *desc, uint8_t queue_idx);
int fhost_rxbuf_repush(uint32_t host_id);
int fhost_rxbuf_push();

/* align to 4 bytes */
static inline struct fhost_txdesc *pbuf_fhost_txdesc(struct pbuf *p)
{
	uint32_t payload;
	payload = CO_ALIGN4_HI((uint32_t)p->payload);

	return (struct fhost_txdesc *)payload;
}

#endif //__FHOST_MSDU_H__


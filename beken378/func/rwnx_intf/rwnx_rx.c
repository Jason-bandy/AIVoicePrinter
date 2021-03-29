#include "stdio.h"
#include "string.h"
#include "rwnx_config.h"
#include "mac.h"
#include "macif.h"
#include "hal_desc.h"
#include "mac_frame.h"
#include "rxu_cntrl.h"
#include "txl_buffer.h"
#include "vif_mgmt.h"
#include "sta_mgmt.h"
#include "rwnx_rx.h"
#include "ke_mem.h"
#include "rw_msdu.h"
#include "rxl_hwdesc.h"
#include "prot/ethernet.h"
#include "sk_intf.h"

#if CFG_IEEE80211AX
void fhost_msdu_init()
{
}

void hapd_intf_mgmt_tx_cb(void *arg)
{
    MSDU_NODE_T *node = arg;
    uint8_t *mpdu = node->p->payload;
    struct fhost_txdesc *fhost_txdesc = node->ftxdesc;
    struct txdesc *txdesc = &fhost_txdesc->txdesc;
    struct tx_cfm_tag *cfm = &txdesc->host.cfm;
    bool acked = !!(cfm->status & TX_STATUS_ACKNOWLEDGED);
    struct mac_hdr *hdr = (struct mac_hdr *)mpdu;

    ASSERT(node->p);

    hdr->fctl &= ~MAC_FCTRL_PROTOCOLVERSION_MASK;

    /* protocol version 2 is reserved for indicating ACKed frame (TX
     * callbacks), and version 1 for indicating failed frame (no ACK, TX
     * callbacks) */
    if (acked)
        hdr->fctl |= 2;
    else
        hdr->fctl |= 1;

    // send to hostapd / wpa supplicant
    ke_mgmt_packet_tx(mpdu, node->len, txdesc->host.vif_idx);
}

/*
 * Push the confirmation to the FHOST, refer rwnx_drv: rwnx_txdatacfm
 *
 */
void fhost_tx_cfm_push(uint8_t queue_idx, struct txdesc *txdesc)
{
    struct tx_cfm_tag *cfm = &txdesc->host.cfm;

    if (cfm->status & TX_STATUS_DONE)
    {
        if (txdesc->host.flags & TXU_CNTRL_MGMT)
        {
#if 0
            /* Confirm transmission to CFG80211 */
            cfg80211_mgmt_tx_status(&sw_txhdr->rwnx_vif->wdev,
                                    (unsigned long)skb,
                                    (skb->data + sw_txhdr->headroom),
                                    sw_txhdr->frame_len,
                                    rwnx_txst.acknowledged,
                                    GFP_ATOMIC);
#endif
        }

        //retry xmit: packet has been transmitted but not acknowledged, driver must repush it.
        if (cfm->status & TX_STATUS_RETRY_REQUIRED)
        {
            /* retransmit the frame: BA bit lost */
            cfm->status = 0;
            txdesc->host.flags |= TXU_CNTRL_RETRY;
            txdesc->host.flags &= ~TXU_CNTRL_MORE_DATA;
            if (fhost_txbuf_prepend(txdesc, queue_idx) != kNoErr)
                goto err_tx;
        }
        else if (cfm->status & TX_STATUS_SW_RETRY_REQUIRED)
        {
            // software retry: need to set pn, rebuild frame header, mic, etc.
            cfm->status = 0;

            if (fhost_txbuf_prepend(txdesc, queue_idx) != kNoErr)
                goto err_tx;
        }
        else if (cfm->status & TX_STATUS_ACKNOWLEDGED)
        {
            //xmit successfully, run callback
            rwm_tx_confirm(txdesc);
            dbg(D_INF D_FHOST "FHOST: xmit one packet success by queue(%0d)\r\n", queue_idx);
        }
        else
        {
            //os_printf("XXX: %s %d, xmit failed, status 0x%x, cb %p, arg %p\n", __func__, __LINE__, cfm->status,
            //    txdesc->host.cfm_cb, txdesc->host.cfm_cb_arg);
            //xmit failed
err_tx:
            rwm_tx_confirm(txdesc);
            dbg(D_INF D_FHOST "FHOST: xmit one packet failed by queue(%0d)\r\n", queue_idx);
        }
    }
    else
    {
        dbg(D_ERR D_FHOST "FHOST: Rx one error tx_cfm\r\n");
    }
}

int fhost_rxbuf_push()
{
#if !BK_MIN_RX_BUFSZ
#if CFG_USE_LWIP_NETSTACK
    /*
     * +------+----------+-------------------+
     * | pbuf |  rxvect  |   IEEE 802.3 Data |
     * +------+----------+-------------------+
     */
    struct pbuf *pbuf;

    pbuf = pbuf_alloc(PBUF_RAW, CFG_MSDU_MAX_LEN, PBUF_RAM);
    ASSERT_ERR(pbuf != NULL);

    if (pbuf && fhost_rxbuf_repush((uint32_t)pbuf))
    {
        // queue failed, free pbuf
        pbuf_free(pbuf);
    }

    BUILD_BUG_ON(sizeof(struct fhost_rx_header) != RXL_HEADER_INFO_LEN);

#else
    uint8_t *payload;

    payload = ke_malloc(CFG_MSDU_MAX_LEN);
    ASSERT_ERR(payload != NULL);

    if (fhost_rxbuf_repush((uint32_t)payload))
        ke_free(payload);
#endif
#endif //!BK_MIN_RX_BUFSZ

    return 0;
}

void fhost_free_rx_buffer(uint32 host_id)
{
#if CFG_USE_LWIP_NETSTACK
    struct pbuf *p = (struct pbuf *)host_id;

    if (p)
        pbuf_free(p);
#else
    ke_free((void *)host_id);
#endif
}

/*
 * FIXME: BK7236, don't free buff_addr when lwip is used or monitor.
 */
void fhost_rx_desc_handler(struct rxu_stat_val *rxstat)
{
    uint16_t status = rxstat->status;
    struct fhost_rx_header *rxhdr;
    struct pbuf *p;
    uint32_t host_id;

    host_id = rxstat->host_id;
    p = (struct pbuf *)rxstat->host_id;

    rxhdr = (struct fhost_rx_header *)(p->payload);

    /* Check if we need to delete the buffer */
    if (status & RX_STAT_DELETE)
    {
        fhost_free_rx_buffer(host_id);
        goto end;
    }

    /* Check if we need to forward the buffer coming from a monitor interface */
    if (status & RX_STAT_MONITOR)
    {
        if (g_rwnx_connector.monitor_outbound_func /* && rxhdr->hwvect.len >= 24*/)  // e.g. rwm_rx_monitor
            g_rwnx_connector.monitor_outbound_func((void *)host_id, rxhdr->hwvect.len);

        /* Only monitor interface exist */
        if (status == RX_STAT_MONITOR)
        {
            status |= RX_STAT_ALLOC;
            fhost_free_rx_buffer(host_id);
        }
    }

    /* Check if we need to update the length */
    if (status & RX_STAT_LEN_UPDATE)
    {
        rxhdr->hwvect.len = rxstat->frame_len;

        if (status & RX_STAT_ETH_LEN_UPDATE)
        {
            /* Update Length Field inside the Ethernet Header */
            struct eth_hdr *hdr = (struct eth_hdr *)((uint8_t *)rxhdr + sizeof(struct fhost_rx_header));

            hdr->type = htons(rxstat->frame_len - sizeof(struct eth_hdr));
        }
        goto end;
    }

    /* Check if it must be discarded after informing upper layer */
    if (status & RX_STAT_SPURIOUS)
    {
        if (g_rwnx_connector.spurious_outbound_func)
            g_rwnx_connector.spurious_outbound_func((void *)host_id, rxhdr->hwvect.len);

        if (fhost_rxbuf_repush(host_id))
            pbuf_free((struct pbuf *)host_id);

        goto end;
    }

    /* Check if we need to forward the buffer */
    if (status & RX_STAT_FORWARD)
    {
        if (g_rwnx_connector.data_outbound_func)  //rwm_upload_data
            g_rwnx_connector.data_outbound_func((void *)host_id, rxhdr->hwvect.len);
    }

    /* Check if we need to allocate a new buffer */
    if (status & RX_STAT_ALLOC)
        fhost_rxbuf_push();

end:
    return;
}

#endif


#include "include.h"
#include "doubly_list.h"
#include "rw_msdu.h"
#include "rw_pub.h"
#include "str_pub.h"
#include "mem_pub.h"
#include "txu_cntrl.h"

#include "lwip/pbuf.h"
#ifdef CFG_WFA_CERTIFICATION
#include "prot/ip4.h"
#include "prot/ip6.h"
#include "prot/ethernet.h"
#endif

#include "arm_arch.h"
#if CFG_GENERAL_DMA
#include "general_dma_pub.h"
#endif
#include "param_config.h"
#include "fake_clock_pub.h"
#include "power_save_pub.h"
#if CFG_IEEE80211AX
#include "rwnx_rx.h"
#include "rxl_hwdesc.h"
#include "ieee802_11_defs.h"
#include "wpa_ctrl.h"
#include "macif.h"
#include "ip_port.h"
#endif
#include "rwnx_defs.h"

void ethernetif_input(int iface, struct pbuf *p);
UINT32 rwm_transfer_node(MSDU_NODE_T *node, u8 flag);
extern int bmsg_ps_handler_rf_ps_mode_real_wakeup(void);
extern int ke_mgmt_packet_tx(unsigned char *buf, int len, int flag);

LIST_HEAD_DEFINE(msdu_rx_list);

#if CFG_USE_AP_PS
#include "ps.h"
#include "app.h"
#define MAX_PS_STA_NUM          BROADCAST_STA_IDX_MIN   // CFG_STA_MAX
#define MAX_BUFFER_TIME         10000       // 10S

typedef struct sta_ps_st {
    beken_timer_t timer;
    struct list_head txing;
} STA_PS_ST, *STA_PS_PTR;

typedef struct rwm_ap_ps_st {
    BOOL active;
    STA_PS_ST sta_ps[MAX_PS_STA_NUM];
} AP_PS_ST, *AP_PS_PTR;

AP_PS_ST g_ap_ps = {0};
#endif
UINT8 g_tid = 0xFF;

void rwm_push_rx_list(MSDU_NODE_T *node)
{
    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    list_add_tail(&node->hdr, &msdu_rx_list);
    GLOBAL_INT_RESTORE();
}

MSDU_NODE_T *rwm_pop_rx_list(void)
{
    LIST_HEADER_T *tmp;
    LIST_HEADER_T *pos;
    MSDU_NODE_PTR node;

    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();

    node = NULLPTR;
    list_for_each_safe(pos, tmp, &msdu_rx_list)
    {
        list_del(pos);
        node = list_entry(pos, MSDU_NODE_T, hdr);

        break;
    }

    GLOBAL_INT_RESTORE();

    return node;
}

void rwm_flush_rx_list(void)
{
    MSDU_NODE_T *node_ptr;

    while(1)
    {
        node_ptr = rwm_pop_rx_list();
        if(node_ptr)
        {
            os_free(node_ptr);
        }
        else
        {
            break;
        }
    }
}

void rwm_tx_confirm(void *param)
{
#if !CFG_IEEE80211AX
	struct txdesc *txdesc = (struct txdesc *)param;

	if (txdesc && txdesc->host.msdu_node) {
		if (txdesc->host.callback)
			(*txdesc->host.callback)(txdesc->host.param);
		os_null_printf("flush_desc:0x%x\r\n", txdesc->host.msdu_node);

		os_free(txdesc->host.msdu_node);
		txdesc->host.msdu_node = NULL;
	}
#else
	struct txdesc *txdesc = (struct txdesc *)param;
	MSDU_NODE_T *node = txdesc->host.buf;

	if (txdesc && node) {
		struct fhost_txdesc *fhost_txdesc = container_of(txdesc, struct fhost_txdesc, txdesc);
		void (*cfm_cb)(void *) = txdesc->host.cfm_cb;

		os_null_printf("%s %d: txdesc %p, fhost_txdesc %p, cfm_cb %p, arg %p\n",
				__func__, __LINE__, txdesc, fhost_txdesc, cfm_cb, txdesc->host.cfm_cb_arg);

		// callback for mgmt frame tx
		if (cfm_cb)
			cfm_cb(txdesc->host.cfm_cb_arg);

		os_null_printf("flush_desc: node %p, pbuf %p\n", node, node->p);

		if (node->p) {
			pbuf_free(node->p);
			node->p = NULL;
		}
		// free msdu
		os_free(node);
		txdesc->host.buf = NULL;

		// free fhost tx descriptor
		os_free(fhost_txdesc);
	}
#endif
}

void rwm_tx_msdu_renew(UINT8 *buf, UINT32 len, UINT8 *orig_addr)
{
#if CFG_GENERAL_DMA
    gdma_memcpy((void *)((UINT32)orig_addr + CFG_MSDU_RESV_HEAD_LEN), buf, len);
#else
    os_memmove((void *)((UINT32)orig_addr + CFG_MSDU_RESV_HEAD_LEN), buf, len);
#endif
}

void rwm_tx_mpdu_renew(UINT8 *buf, UINT32 len, UINT8 *orig_addr)
{
#if CFG_GENERAL_DMA
    gdma_memcpy((void *)((UINT32)orig_addr), buf, len);
#else
    os_memmove((void *)((UINT32)orig_addr), buf, len);
#endif
}

UINT8 *rwm_get_msdu_content_ptr(MSDU_NODE_T *node)
{
#if !CFG_IEEE80211AX
    return (UINT8 *)((UINT32)node->msdu_ptr + CFG_MSDU_RESV_HEAD_LEN);
#else
    return node->msdu_ptr;
#endif
}

UINT8 *rwm_get_mpdu_content_ptr(MSDU_NODE_T *node)
{
    return (UINT8 *)((UINT32)node->msdu_ptr);
}

void rwm_txdesc_copy(struct txdesc *dst_local, ETH_HDR_PTR eth_hdr_ptr)
{
    struct hostdesc *host_ptr;

    host_ptr = &dst_local->host;

    os_memcpy(&host_ptr->eth_dest_addr, &eth_hdr_ptr->e_dest, sizeof(host_ptr->eth_dest_addr));
    os_memcpy(&host_ptr->eth_src_addr, &eth_hdr_ptr->e_src, sizeof(host_ptr->eth_src_addr));
}

int rwm_raw_frame_with_cb(uint8_t *buffer, int len, void *cb, void *param)
{
#if !CFG_IEEE80211AX
	int ret = 0;
	uint8_t *pkt = buffer;
	MSDU_NODE_T *node;
	UINT8 *content_ptr;
	UINT32 queue_idx = AC_VI;
	struct txdesc *txdesc_new;
	struct umacdesc *umac;

	node = rwm_tx_node_alloc(len);
	if (node == NULL) {
		goto exit;
	}

	rwm_tx_msdu_renew(pkt, len, node->msdu_ptr);
	content_ptr = rwm_get_msdu_content_ptr(node);

	txdesc_new = tx_txdesc_prepare(queue_idx);
	if(txdesc_new == NULL || TXDESC_STA_USED == txdesc_new->status) {
		rwm_node_free(node);
		goto exit;
	}

	txdesc_new->status = TXDESC_STA_USED;
	txdesc_new->host.flags = TXU_CNTRL_MGMT;
	txdesc_new->host.orig_addr = (UINT32)node->msdu_ptr;
	txdesc_new->host.packet_addr = (UINT32)content_ptr;
	txdesc_new->host.packet_len = len;
	txdesc_new->host.status_desc_addr = (UINT32)content_ptr;
	txdesc_new->host.tid = 0xff;
	txdesc_new->host.callback = (mgmt_tx_cb_t)cb;
	txdesc_new->host.param = param;
	txdesc_new->host.msdu_node = (void *)node;

	umac = &txdesc_new->umac;
	umac->payl_len = len;
	umac->head_len = 0;
	umac->tail_len = 0;
	umac->hdr_len_802_2 = 0;

	umac->buf_control = &txl_buffer_control_24G;

	txdesc_new->lmac.agg_desc = NULL;
	txdesc_new->lmac.hw_desc->cfm.status = 0;

	ps_set_data_prevent();
	nxmac_pwr_mgt_setf(0);

	bmsg_ps_handler_rf_ps_mode_real_wakeup();

	txl_cntrl_push(txdesc_new, queue_idx);

	ret = len;

exit:
	return ret;
#else /* CFG_IEEE80211AX */
	/* TODO: BK7236 monitor tx */
	return len;
#endif
}

#if !CFG_IEEE80211AX
MSDU_NODE_T *rwm_tx_node_alloc(UINT32 len)
{
    UINT8 *buff_ptr;
    MSDU_NODE_T *node_ptr = 0;

#if (CFG_SUPPORT_RTT) && (CFG_SOC_NAME == SOC_BK7221U || CFG_SOC_NAME == SOC_BK7271)
    node_ptr = (MSDU_NODE_T *)dtcm_malloc(sizeof(MSDU_NODE_T)
                                        + CFG_MSDU_RESV_HEAD_LEN
                                        + len
                                        + CFG_MSDU_RESV_TAIL_LEN);
#elif (CFG_OS_FREERTOS) && (CFG_SOC_NAME == SOC_BK7221U)
    node_ptr = (MSDU_NODE_T *)pvPortMalloc(sizeof(MSDU_NODE_T)
                                        + CFG_MSDU_RESV_HEAD_LEN
                                        + len
                                        + CFG_MSDU_RESV_TAIL_LEN);
#else
    node_ptr = (MSDU_NODE_T *)os_malloc(sizeof(MSDU_NODE_T)
                                        + CFG_MSDU_RESV_HEAD_LEN
                                        + len
                                        + CFG_MSDU_RESV_TAIL_LEN);
#endif

    if(NULL == node_ptr)
    {
        goto alloc_exit;
    }

    buff_ptr = (UINT8 *)((UINT32)node_ptr + sizeof(MSDU_NODE_T));

    node_ptr->msdu_ptr = buff_ptr;
    node_ptr->len = len;

alloc_exit:
    return node_ptr;
}

#else

MSDU_NODE_T *rwm_tx_node_alloc(UINT32 len, pbuf_layer layer)
{
	MSDU_NODE_T *node_ptr = 0;
	struct pbuf *p;

	/* no payload here */
	node_ptr = (MSDU_NODE_T *)os_zalloc(sizeof(MSDU_NODE_T));

	if (!node_ptr)
		goto alloc_exit;

	p = pbuf_alloc(layer, len, PBUF_RAM);
	if (!p) {
		os_free(node_ptr);
		node_ptr = 0;
		goto alloc_exit;
	}

	node_ptr->p = p;
	node_ptr->msdu_ptr = p->payload;
	node_ptr->len = p->tot_len;

alloc_exit:
	return node_ptr;
}

MSDU_NODE_T *rwm_tx_node_alloc_with_pbuf(struct pbuf *p)
{
	MSDU_NODE_T *node_ptr = 0;

	node_ptr = (MSDU_NODE_T *)os_zalloc(sizeof(MSDU_NODE_T));

	if (!node_ptr)
		goto alloc_exit;

	node_ptr->p = p;
	node_ptr->msdu_ptr = p->payload;
	node_ptr->len = p->tot_len;

alloc_exit:
	return node_ptr;
}

#endif

void rwm_node_free(MSDU_NODE_T *node)
{
    ASSERT(node);

#if CFG_IEEE80211AX
	if (node->ftxdesc) {
		os_free(node->ftxdesc);
		node->ftxdesc = NULL;
	}
	if (node->p) {
		pbuf_free(node->p);
		node->p = NULL;
	}
#endif
    os_free(node);
}

UINT8 *rwm_rx_buf_alloc(UINT32 len)
{
    return (UINT8 *)os_malloc(len);
}

UINT32 rwm_get_rx_valid(void)
{
    UINT32 count = 0;
    LIST_HEADER_T *tmp;
    LIST_HEADER_T *pos;
    LIST_HEADER_T *head = &msdu_rx_list;

    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();
    list_for_each_safe(pos, tmp, head)
    {
        count ++;
    }
    GLOBAL_INT_RESTORE();

    return ((count >= MSDU_RX_MAX_CNT) ? 0 : 1);
}

UINT32 rwm_get_rx_valid_node_len(void)
{
    UINT32 len = 0;
    LIST_HEADER_T *tmp;
    LIST_HEADER_T *pos;
    MSDU_NODE_PTR node;

    GLOBAL_INT_DECLARATION();

    GLOBAL_INT_DISABLE();

    node = NULLPTR;
    list_for_each_safe(pos, tmp, &msdu_rx_list)
    {
        node = list_entry(pos, MSDU_NODE_T, hdr);
        len = node->len;
        break;
    }

    GLOBAL_INT_RESTORE();

    return len;
}

UINT8 rwm_get_tid()
{
    return g_tid;
}

void rwm_set_tid(UINT8 tid)
{
    if (0xFF == tid)
    {
        g_tid = tid;
    }
    else
    {
        g_tid = tid & MAC_QOSCTRL_UP_MSK;
    }
}

#if CFG_USE_AP_PS
extern void bmsg_txing_sender(uint8_t sta_idx);
void rwm_push_txing_list(MSDU_NODE_T *node, UINT8 sta_idx)
{
    GLOBAL_INT_DECLARATION();

    if(sta_idx >= MAX_PS_STA_NUM)
        return;

    GLOBAL_INT_DISABLE();
    list_add_tail(&node->hdr, &g_ap_ps.sta_ps[sta_idx].txing);
    GLOBAL_INT_RESTORE();
}

MSDU_NODE_T *rwm_pop_txing_list(UINT8 sta_idx)
{
    LIST_HEADER_T *tmp;
    LIST_HEADER_T *pos;
    MSDU_NODE_PTR node;

    GLOBAL_INT_DECLARATION();

    if(sta_idx >= MAX_PS_STA_NUM)
        return NULL;

    GLOBAL_INT_DISABLE();

    node = NULLPTR;
    list_for_each_safe(pos, tmp, &g_ap_ps.sta_ps[sta_idx].txing)
    {
        list_del(pos);
        node = list_entry(pos, MSDU_NODE_T, hdr);
        break;
    }

    GLOBAL_INT_RESTORE();

    return node;
}

UINT32 rwm_txling_list_node_count(UINT8 sta_idx)
{
    if(sta_idx >= MAX_PS_STA_NUM)
        return 0;

    return list_size(&g_ap_ps.sta_ps[sta_idx].txing);
}

void rwm_flush_txing_list(UINT8 sta_idx)
{
    MSDU_NODE_T *node_ptr;
    int ret;

    if(sta_idx >= MAX_PS_STA_NUM)
        return;

    if(rwm_txling_list_node_count(sta_idx))
    {
        RWNX_LOGI("flush buffered node, staid:%d\r\n", sta_idx);
        while(1) {
            node_ptr = rwm_pop_txing_list(sta_idx);
            if(node_ptr)
                os_free(node_ptr);
            else
                break;
        }
    }

    if(rtos_is_timer_running(&g_ap_ps.sta_ps[sta_idx].timer))
    {
        RWNX_LOGI("stop ap ps timer, staid:%d\r\n", sta_idx);
        ret = rtos_stop_timer(&g_ap_ps.sta_ps[sta_idx].timer);
        ASSERT(0 == ret);
    }
}

void rwm_ps_tranfer_node(MSDU_NODE_T *node)
{
    UINT8 vif_idx, sta_idx;
    BOOL need_buffer = false;

    if(!node)
        return;

    vif_idx = node->vif_idx;
    sta_idx = node->sta_idx;

    if(!rwm_mgmt_is_ap_inface(vif_idx))
    {
        #if NX_POWERSAVE
        txl_cntrl_inc_pck_cnt();
        #endif
        // normal transfer
        rwm_transfer_node(node, 0);
    }
    else
    {
        // only ap mode need check peer stations in ps mode
        if(!g_ap_ps.active)
        {
            #if NX_POWERSAVE
            txl_cntrl_inc_pck_cnt();
            #endif
            // normal transfer
            rwm_transfer_node(node, 0);
        }
        else
        {
             if((sta_mgmt_is_in_ps(sta_idx) || rwm_txling_list_node_count(sta_idx))
                && (sta_idx < MAX_PS_STA_NUM))
             {
                if(!rwm_txling_list_node_count(sta_idx))
                {
                    u8 vif_idx = sta_mgmt_get_vif_idx(sta_idx);
                    u16 aid = sta_mgmt_get_aid(sta_idx);
                    int ret;

                    //RWNX_LOGI("on bcn tim: vif:%d, aid:%d, sta:%d\r\n", vif_idx, aid, sta_idx);

                    //rwnx_send_me_uapsd_traffic_ind(sta_idx, 1);
                    rw_msg_send_tim_update(vif_idx, aid, 1);
                    if(!rtos_is_timer_running(&g_ap_ps.sta_ps[sta_idx].timer)) {
                        ret = rtos_start_timer(&g_ap_ps.sta_ps[sta_idx].timer);
	                    ASSERT(0 == ret);
                    }
                }

                // add this node to txing list
                //RWNX_LOGI("addto txing list node:%p, sta:%d\r\n", node, sta_idx);
                rwm_push_txing_list(node, sta_idx);
             }
             else
             {
                #if NX_POWERSAVE
                txl_cntrl_inc_pck_cnt();
                #endif
                // normal transfer
                rwm_transfer_node(node, 0);
             }
        }
    }
}

void rwm_msdu_send_txing_node(UINT8 sta_idx)
{
    MSDU_NODE_T *node = NULL;
    struct txdesc *txdesc_new = NULL;
    UINT32 node_left;

    node = rwm_pop_txing_list(sta_idx);
    if(!node)
        return;

    node_left = rwm_txling_list_node_count(sta_idx);

    #if NX_POWERSAVE
    txl_cntrl_inc_pck_cnt();
    #endif

    //RWNX_LOGI("pop txing list node:%p, sta:%d\r\n",node, sta_idx);
    rwm_transfer_node(node, TXU_CNTRL_MORE_DATA);

    if(node_left && !sta_mgmt_is_in_ps(sta_idx)) {
        // trigger sending txing again
        bmsg_txing_sender(sta_idx);
    }
    else if(!node_left)
    {
        u8 vif_idx = sta_mgmt_get_vif_idx(sta_idx);
        u16 aid = sta_mgmt_get_aid(sta_idx);

        //RWNX_LOGI("off bcn tim: vif:%d, aid:%d, sta:%d\r\n", vif_idx, aid, sta_idx);

        //rwnx_send_me_uapsd_traffic_ind(sta_idx, 0);
        rw_msg_send_tim_update(vif_idx, aid, 0);
    }
}

void rwm_msdu_ps_change_ind_handler(void *msg)
{
    struct ke_msg *msg_ptr = (struct ke_msg *)msg;
    struct mm_ps_change_ind *ind;
    UINT32 node_left;
    int ret;

    if(!msg_ptr || !msg_ptr->param)
        return;

    ind = (struct mm_ps_change_ind *)msg_ptr->param;
    node_left = rwm_txling_list_node_count(ind->sta_idx);

    if((ind->ps_state == PS_MODE_OFF) && node_left) {
        // trigger txing sending
       // RWNX_LOGI("ps off, trigger txing sending %d\r\n", node_left);
        bmsg_txing_sender(ind->sta_idx);

        if(rtos_is_timer_running(&g_ap_ps.sta_ps[ind->sta_idx].timer)) {
            ret = rtos_stop_timer(&g_ap_ps.sta_ps[ind->sta_idx].timer);
            ASSERT(0 == ret);
        }
    } else if((ind->ps_state == PS_MODE_ON) && node_left)  {

        //RWNX_LOGI("ps_change on:%d\r\n", node_left);
        // do something
        if(!rtos_is_timer_running(&g_ap_ps.sta_ps[ind->sta_idx].timer)) {
            ret = rtos_start_timer(&g_ap_ps.sta_ps[ind->sta_idx].timer);
	        ASSERT(0 == ret);
        }
    }
}

void rwm_msdu_ap_ps_timeout(void *data)
{
    u8 sta_idx = (u32)data;

    rwm_flush_txing_list(sta_idx);
}

#endif

void rwm_msdu_init(void)
{
    #if CFG_USE_AP_PS
    g_ap_ps.active = true;

    for(int i=0; i<MAX_PS_STA_NUM; i++)
    {
        int ret;
        INIT_LIST_HEAD(&g_ap_ps.sta_ps[i].txing);

    	ret = rtos_init_timer(&g_ap_ps.sta_ps[i].timer,
			                   MAX_BUFFER_TIME,
			                   rwm_msdu_ap_ps_timeout,
			                   (void *)i);
        ASSERT(0 == ret);
    }
    #endif

    g_tid = 0xFF;
}

#ifdef CFG_WFA_CERTIFICATION
/*
 * IEEE802.11-2016: Table 10-1—UP-to-AC mappings
 */
uint8_t ipv4_ieee8023_dscp(UINT8 *buf)
{
	uint8_t tos;
	struct ip_hdr *hdr = (struct ip_hdr *)buf;

	tos = IPH_TOS(hdr);

	return (tos & 0xfc) >> 5;
}

/* extract flow control field */
uint8_t ipv6_ieee8023_dscp(UINT8 *buf)
{
	uint8_t tos;
	struct ip6_hdr *hdr = (struct ip6_hdr *)buf;

	tos = IP6H_FL(hdr);

	return (tos & 0xfc) >> 5;
}
#endif

void ieee80211_data_tx_cb(void *param)
{
	struct txdesc *txdesc_new = (struct txdesc *)param;
#if CFG_IEEE80211AX
	MSDU_NODE_T *node = txdesc_new->host.buf;
#else
	MSDU_NODE_T *node = (MSDU_NODE_T *)txdesc_new->host.msdu_node;
#endif
	struct tx_hd *txhd = &txdesc_new->lmac.hw_desc->thd;
	struct ieee80211_tx_cb *cb = (struct ieee80211_tx_cb *)node->args;
	uint32_t status = txhd->statinfo;

	if (0 == node) {
		RWNX_LOGI("zero_node\r\n");
		return;
	}

	if (status & FRAME_SUCCESSFUL_TX_BIT /*DESC_DONE_SW_TX_BIT*/)
		cb->result = RW_SUCCESS;
	else
		cb->result = RW_FAILURE;

	rtos_set_semaphore(&cb->sema);
}

int qos_need_enabled(struct sta_info_tag *sta)
{
	if (!sta)
		return 0;
#if CFG_IEEE80211AX
	if (!STA_CAPA(sta, QOS))
		return 0;
#else
	if (!(sta->info.capa_flags & STA_QOS_CAPA))
		return 0;
#endif

	return 1;
}

/*
 * get user priority from @buf.
 * ipv4 dscp/tos, ipv6 flow control. for eapol packets, disable qos.
 */
uint8_t classify8021d(UINT8 *buf)
{
#ifdef CFG_WFA_CERTIFICATION
	struct eth_hdr *ethhdr = (struct eth_hdr *)buf;

	switch (PP_HTONS(ethhdr->type)) {
	case ETHTYPE_IP:
		return ipv4_ieee8023_dscp(ethhdr + 1);
	case ETHTYPE_IPV6:
		return ipv6_ieee8023_dscp(ethhdr + 1);
	case ETH_P_PAE:
		return 7;	/* TID7 highest user priority */
	default:
		return 0;
	}
#else
	return 4;		// TID4: mapped to AC_VI
#endif
}

#if !CFG_IEEE80211AX
UINT32 rwm_transfer(UINT8 vif_idx, UINT8 *buf, UINT32 len, int sync, void *args)
{
    UINT32 ret = 0;
    MSDU_NODE_T *node;
    ETH_HDR_PTR eth_hdr_ptr;

    ret = RW_FAILURE;
    node = rwm_tx_node_alloc(len);
    if(NULL == node)
    {
        #if NX_POWERSAVE
        txl_cntrl_dec_pck_cnt();
        #endif

        RWNX_LOGI("rwm_transfer no node\r\n");
        goto tx_exit;
    }
    rwm_tx_msdu_renew(buf, len, node->msdu_ptr);

    eth_hdr_ptr = (ETH_HDR_PTR)buf;
    node->vif_idx = vif_idx;
	node->sync = sync;
	node->args = args;
    node->sta_idx = rwm_mgmt_tx_get_staidx(vif_idx,
                             &eth_hdr_ptr->e_dest);

#if CFG_USE_AP_PS
    rwm_ps_tranfer_node(node);
#else
    rwm_transfer_node(node, 0);
#endif

tx_exit:
    return ret;
}

UINT32 rwm_transfer_node(MSDU_NODE_T *node, u8 flag)
{
    UINT8 tid;
    UINT32 ret = 0;
    UINT8 *content_ptr;
    UINT32 queue_idx;
    ETH_HDR_PTR eth_hdr_ptr;
    struct txdesc *txdesc_new;

#if CFG_RWNX_QOS_MSDU
	struct sta_info_tag *sta;
	struct vif_info_tag *vif;
#endif

    if(!node) {
        goto tx_exit;
    }

    content_ptr = rwm_get_msdu_content_ptr(node);
    eth_hdr_ptr = (ETH_HDR_PTR)content_ptr;

#if CFG_RWNX_QOS_MSDU
	vif = rwm_mgmt_vif_idx2ptr(node->vif_idx);
	if (NULL == vif)
	{
		RWNX_LOGI("%s: vif is NULL!\r\n", __func__);
		goto tx_exit;
	}
	if (likely(vif->active)) {
		sta = &sta_info_tab[vif->u.sta.ap_id];
		if (qos_need_enabled(sta)) {
			int i;
			tid = classify8021d((UINT8 *)eth_hdr_ptr);
			/* check admission ctrl */
			for (i = mac_tid2ac[tid]; i >= 0; i--)
				if (!(vif->bss_info.edca_param.acm & BIT(i)))
					break;
			if (i < 0)
				goto tx_exit;
			queue_idx = i;	/* AC_* */
		} else {
			/*
			 * non-WMM STA
			 *
			 * CWmin 15, CWmax 1023, AIFSN 2, TXOP 0. set these values when joining with this BSS.
			 */
			tid = 0xFF;
			queue_idx = AC_VI;
		}
	} else {
		tid = 0xFF;
	    queue_idx = AC_VI;
	}
#else /* !CFG_RWNX_QOS_MSDU */
    tid = rwm_get_tid();

    queue_idx = AC_VI;
#endif /* CFG_RWNX_QOS_MSDU */

    txdesc_new = tx_txdesc_prepare(queue_idx);
    if(TXDESC_STA_USED == txdesc_new->status)
    {
        RWNX_LOGI("rwm_transfer no txdesc \r\n");
        goto tx_exit;
    }

    txdesc_new->status = TXDESC_STA_USED;
    rwm_txdesc_copy(txdesc_new, eth_hdr_ptr);

    txdesc_new->host.flags            = flag;
#if NX_AMSDU_TX
    txdesc_new->host.orig_addr[0]     = (UINT32)node->msdu_ptr;
    txdesc_new->host.packet_addr[0]   = (UINT32)content_ptr + 14;
    txdesc_new->host.packet_len[0]    = node->len - 14;
    txdesc_new->host.packet_cnt       = 1;
#else
    txdesc_new->host.orig_addr        = (UINT32)node->msdu_ptr;
    txdesc_new->host.packet_addr      = (UINT32)content_ptr + 14;
    txdesc_new->host.packet_len       = node->len - 14;
#endif
    txdesc_new->host.status_desc_addr = (UINT32)content_ptr + 14;
    txdesc_new->host.ethertype        = eth_hdr_ptr->e_proto;
    txdesc_new->host.tid              = tid;

    txdesc_new->host.vif_idx          = node->vif_idx;
    txdesc_new->host.staid            = node->sta_idx;
	txdesc_new->host.msdu_node        = (void *)node;

	if (node->sync)
	{
		txdesc_new->host.callback		  = (mgmt_tx_cb_t)ieee80211_data_tx_cb;
		txdesc_new->host.param			  = (void *)txdesc_new;
	}
	else
	{
		txdesc_new->host.callback = 0;
	}

    txdesc_new->lmac.agg_desc = NULL;
    txdesc_new->lmac.hw_desc->cfm.status = 0;

    txu_cntrl_push(txdesc_new, queue_idx);
    return ret;

tx_exit:
    if (NULL != node)
        rwm_node_free(node);
#if NX_POWERSAVE
    txl_cntrl_dec_pck_cnt();
#endif
    return ret;
}

UINT32 rwm_get_rx_free_node(struct pbuf **p_ret, UINT32 len)
{
    struct pbuf *p;

    p = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);
    *p_ret = p;

    return RW_SUCCESS;
}

UINT32 rwm_upload_data(RW_RXIFO_PTR rx_info)
{
    struct pbuf *p = (struct pbuf *)rx_info->data;

    os_null_printf("s:%d, v:%d, d:%d, r:%d, c:%d, l:%d, %p\r\n",
                   rx_info->sta_idx,
                   rx_info->vif_idx,
                   rx_info->dst_idx,
                   rx_info->rssi,
                   rx_info->center_freq,
                   rx_info->length,
                   rx_info->data);

    ethernetif_input(rx_info->vif_idx, p);

    return RW_SUCCESS;
}

#else /* CFG_IEEE80211AX */

//pbuf_coalesce, pbuf_copy
static int rwm_add_payload(struct pbuf *p, struct fhost_txdesc *ftxdesc)
{
	int i;
	struct tx_pbd *pbd;
	struct txl_buffer_tag *txl_buf;
	txl_buf = &ftxdesc->txl_buf;

	/* iterate through pbuf chain */
	pbd = txl_buf->pbd;
	for (i = 0; p && i < TX_PBD_CNT; i++) {
		pbd->upatterntx = TX_PAYLOAD_DESC_PATTERN;
		if (i == 0) {
			/* pbd[0] trim ethernet hdr */
			pbd->datastartptr = (uint32_t)p->payload + sizeof(ETH_HDR_T);
		} else {
			pbd->datastartptr = (uint32_t)p->payload;
		}
		pbd->dataendptr = (uint32_t)p->payload + p->len - 1;
		pbd->bufctrlinfo = 0;
		pbd->next = CPU2HW(pbd + 1);

		pbd++;
		p = p->next;
	}

	if (i > 0)
		txl_buf->pbd[i - 1].next = 0;

	if (p || i == TX_PBD_CNT) {
		RWNX_LOGE("not all data are pushed\n");
		return -1;
	}

	return 0;
}

UINT32 rwm_transfer_node(MSDU_NODE_T *node, u8 flag)
{
	uint8_t                 queue_idx;
	struct fhost_txdesc     *fhost_txdesc = 0;
	struct txdesc           *txdesc;
	struct hostdesc         *host;
	struct txl_buffer_tag   *txl_buf;
	struct tx_pbd           *pbd;
	struct tx_hw_desc       *hwdesc;
	UINT8 tid;
	UINT32 ret = 0;
	UINT8 *content_ptr;
	ETH_HDR_PTR eth_hdr_ptr;
	struct pbuf *p;

#if CFG_RWNX_QOS_MSDU
	struct sta_info_tag *sta;
	struct vif_info_tag *vif;
#endif

	if (!node)
		goto tx_exit;

	p = node->p;

	content_ptr = p->payload;
	eth_hdr_ptr = (ETH_HDR_PTR)content_ptr;

#if CFG_RWNX_QOS_MSDU
	vif = rwm_mgmt_vif_idx2ptr(node->vif_idx);
	if (NULL == vif) {
		RWNX_LOGI("%s: vif is NULL!\r\n", __func__);
		goto tx_exit;
	}

	if (likely(vif->active)) {
		sta = &sta_info_tab[vif->u.sta.ap_id];
		if (qos_need_enabled(sta)) {
			int i;
			tid = classify8021d((UINT8 *)eth_hdr_ptr);
			/* check admission ctrl */
			for (i = mac_tid2ac[tid]; i >= 0; i--)
				if (!(vif->bss_info.edca_param.acm & BIT(i)))
					break;
			if (i < 0)
				goto tx_exit;
			queue_idx = i;	/* AC_* */
		} else {
			/*
			 * non-WMM STA
			 *
			 * CWmin 15, CWmax 1023, AIFSN 2, TXOP 0. set these values when joining with this BSS.
			 */
			tid = 0xFF;
			queue_idx = AC_VI;
		}
	} else {
		tid = 0xFF;
		queue_idx = AC_VI;
	}
#else /* !CFG_RWNX_QOS_MSDU */
	tid = rwm_get_tid();

	queue_idx = AC_VI;
#endif /* CFG_RWNX_QOS_MSDU */

	//alloc tx desc
	fhost_txdesc = (struct fhost_txdesc *)os_zalloc(sizeof(struct fhost_txdesc));
	if (!fhost_txdesc)
		goto tx_exit;

	node->ftxdesc = fhost_txdesc;

	txdesc = &fhost_txdesc->txdesc;
	host = &txdesc->host;
	txl_buf = &fhost_txdesc->txl_buf;
	pbd = &txl_buf->pbd[0];
	hwdesc = &fhost_txdesc->hwdesc;

	//fill host desc
	rwm_txdesc_copy(txdesc, eth_hdr_ptr);
	host->buf = node;
	host->flags = flag;
#if NX_AMSDU_TX
	host->orig_addr[0]     = (UINT32)node->msdu_ptr;
	host->packet_addr[0]   = (UINT32)content_ptr + sizeof(ETH_HDR_T);
	host->packet_len[0]    = node->len - sizeof(ETH_HDR_T);
	host->packet_cnt       = 1;
#else
	host->packet_addr      = (UINT32)content_ptr + sizeof(ETH_HDR_T);
	host->packet_len       = node->len - sizeof(ETH_HDR_T);
#endif
#if !NX_FULLY_HOSTED
	host.status_desc_addr = (UINT32)content_ptr + sizeof(ETH_HDR_T);
#endif
	host->ethertype       = eth_hdr_ptr->e_proto;
	host->tid = tid;

	host->vif_idx = node->vif_idx;
	host->staid = node->sta_idx;

	if (node->sync) {
		host->cfm_cb      = ieee80211_data_tx_cb;
		host->cfm_cb_arg  = txdesc;
	}
	//fill lmac desc
	txdesc->lmac.hw_desc = hwdesc;
	txdesc->lmac.buffer  = txl_buf;
	txdesc->lmac.agg_desc = NULL;
#if !NX_FULLY_HOSTED
	txdesc->lmac.hw_desc->cfm.status = 0;
#endif

	hwdesc->thd.first_pbd_ptr = (uint32_t)pbd;
	hwdesc->thd.dataendptr = 0;
	hwdesc->thd.datastartptr = 0;

	rwm_add_payload(p, fhost_txdesc);

	//notify fw
	ret = fhost_txbuf_push(txdesc, queue_idx);
	if (ret)
		goto tx_exit;

	return 0;

tx_exit:
	if (node)
		rwm_node_free(node);
#if NX_POWERSAVE
	txl_cntrl_dec_pck_cnt();
#endif
	return ret;
}

UINT32 rwm_transfer(UINT8 vif_idx, struct pbuf *p, UINT8 *buf, UINT32 len, int sync, void *args)
{
	UINT32 ret = 0;
	MSDU_NODE_T *node;
	ETH_HDR_PTR eth_hdr_ptr;

	ret = RW_FAILURE;
	node = rwm_tx_node_alloc_with_pbuf(p);
	if (NULL == node) {
#if NX_POWERSAVE
		txl_cntrl_dec_pck_cnt();
#endif

		RWNX_LOGI("rwm_transfer no node\r\n");
		goto tx_exit;
	}

	eth_hdr_ptr = (ETH_HDR_PTR)buf;
	node->vif_idx = vif_idx;
	node->sync = sync;
	node->args = args;
	node->sta_idx = rwm_mgmt_tx_get_staidx(vif_idx,
										   &eth_hdr_ptr->e_dest);
#if CFG_USE_AP_PS
	rwm_ps_tranfer_node(node);
#else
	rwm_transfer_node(node, 0);
#endif

	return ret;

tx_exit:
	if (node)
		rwm_node_free(node);

	return ret;
}

/**
 * transmit mgmt frame
 */
void rwm_transfer_mgmt_node(MSDU_NODE_T *node)
{
	struct hostdesc *host;
	UINT8 *content_ptr;
	UINT32 queue_idx = AC_VI;
	struct txdesc *txdesc;
	struct fhost_txdesc *fhost_txdesc;
	struct txl_buffer_tag *txl_buf;
	struct tx_pbd *pbd;
	struct tx_hw_desc *hwdesc;
	int ret = 0;
	//uint8_t vif_index = 0xFF;
	//struct vif_info_tag *vif;
    bool robust;

	content_ptr = rwm_get_mpdu_content_ptr(node);

	// alloc fhost tx desc
	fhost_txdesc = (struct fhost_txdesc *)os_zalloc(sizeof(struct fhost_txdesc));

	if (!fhost_txdesc)
		goto tx_exit;

	node->ftxdesc = fhost_txdesc;

	// initialize variable
	txdesc = &fhost_txdesc->txdesc;
	host = &txdesc->host;
	txl_buf = &fhost_txdesc->txl_buf;
	pbd = &txl_buf->pbd[0];
	hwdesc = &fhost_txdesc->hwdesc;

	robust = ieee80211_is_robust_mgmt_frame(content_ptr, node->len);

	// setup hostdesc
	host->flags = TXU_CNTRL_MGMT;

	if (robust)
		host->flags |= TXU_CNTRL_MGMT_ROBUST;

#if 0 //keepme
    bool robust;
    robust = ieee80211_is_robust_mgmt_frame(content_ptr, len); // TBD: BK7236

    if (robust)
        host.flags |= TXU_CNTRL_MGMT_ROBUST;

    if (params->no_cck)
        desc->host.flags |= TXU_CNTRL_MGMT_NO_CCK;
#endif
	host->buf = node;
	host->packet_addr = (UINT32)content_ptr;	//mark not-internal frame
	host->packet_len = node->len;
	host->tid = 0xff;
	host->vif_idx = node->vif_idx;
	host->staid = node->sta_idx;

	host->cfm_cb = node->cb;
	host->cfm_cb_arg = node->args;

	// fill lmac desc
	txdesc->lmac.hw_desc = hwdesc;
	txdesc->lmac.buffer  = txl_buf;
	txdesc->lmac.agg_desc = NULL;

	hwdesc->thd.first_pbd_ptr = (uint32_t)pbd;
	hwdesc->thd.dataendptr = 0;
	hwdesc->thd.datastartptr = 0;

	// fill tx_pbd
	pbd->upatterntx = TX_PAYLOAD_DESC_PATTERN;
	pbd->datastartptr = (uint32_t)content_ptr;
	pbd->dataendptr = (uint32_t)content_ptr + node->len - 1;
	pbd->bufctrlinfo = 0;
	pbd->next = 0;

	// notify fw
	ret = fhost_txbuf_push((void *)txdesc, queue_idx);
	if (ret)
		goto tx_exit;

    ps_set_data_prevent();
#if CFG_USE_STA_PS
    bmsg_ps_handler_rf_ps_mode_real_wakeup();
    bk_wlan_dtim_rf_ps_mode_do_wakeup();
#endif

	return;

tx_exit:
	if (fhost_txdesc)
		os_free(fhost_txdesc);
	if (node)
		rwm_node_free(node);
}

/* Get RX Buffer */
UINT32 rwm_get_rx_free_node(uint32_t *host_id, int len)
{
    uint32_t buf_addr;
    struct pbuf *p;

    p = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);
    if (p) {
        *host_id = (uint32_t)p;
        buf_addr = (uint32_t)(p->payload);
    } else {
        *host_id = 0;
        buf_addr = 0;
    }

    //if (!buf_addr)
    //    os_printf("%s: xxxxxxxxxx oom\n", __func__);
    return buf_addr;
}

/**
 * rwnx_rx_mgmt - Process one 802.11 management frame
 *
 * @pbuf: pbuf received
 * @rxhdr: HW rx descriptor
 *
 * return: dont_free: 0: need free pbuf, 1: dont free pbuf
 *
 * Process the management frame and free the corresponding skb.
 * If vif is not specified in the rx descriptor, the the frame is uploaded
 * on all active vifs.
 */
static int rwnx_rx_mgmt(struct pbuf *p, struct fhost_rx_header *rxhdr, int vif_idx)
{
	struct ieee80211_mgmt *mgmt = (struct ieee80211_mgmt *)p->payload;

	if (ieee80211_is_beacon(mgmt->frame_control)) {
		//cfg80211_report_obss_beacon
		ke_mgmt_packet_tx(p->payload, p->len, vif_idx);

		return 0;
	} else if ((ieee80211_is_deauth(mgmt->frame_control) ||
				ieee80211_is_disassoc(mgmt->frame_control)) &&
			   (mgmt->u.deauth.reason_code == WLAN_REASON_CLASS2_FRAME_FROM_NONAUTH_STA ||
				mgmt->u.deauth.reason_code == WLAN_REASON_CLASS3_FRAME_FROM_NONASSOC_STA)) {
		if (mgmt->u.deauth.reason_code == WLAN_REASON_CLASS2_FRAME_FROM_NONAUTH_STA)
			wpa_ctrl_request_async(WPA_CTRL_EVENT_UNPROT_DEAUTHENTICATE, p);
		else
			wpa_ctrl_request_async(WPA_CTRL_EVENT_UNPROT_DISASSOCIATE, p);
		return 1;
	} else {
		ke_mgmt_packet_tx(p->payload, p->len, vif_idx);

		return 0;
	}
}

static void rwnx_rx_mgmt_any(struct pbuf *p, struct fhost_rx_header *rxhdr)
{
	int vif_idx = rxhdr->flags_vif_idx;
	int dont_free = 0;
	int i;
	struct vif_info_tag *vif;

	if (vif_idx == INVALID_VIF_IDX) {
		/* FIXME: BK7236 */
		for (i = 0; i < NX_VIRT_DEV_MAX; i++)  {
			vif = &vif_info_tab[i];
			if (vif->active && vif->type != VIF_MONITOR)
				dont_free = rwnx_rx_mgmt(p, rxhdr, vif->index);
		}
	} else {
		rwnx_rx_mgmt(p, rxhdr, vif_idx);
	}

	if (!dont_free)
		pbuf_free(p);
}


void ethernetif_input_amsdu(struct fhost_rx_header *rxhdr, struct pbuf *p)
{
    struct mac_addr temp_mac;
    struct llc_snap *llc_snap;
    struct mac_eth_hdr *eth_hdr;
    struct amsdu_hdr *amsdu_subfrm_hdr = (struct amsdu_hdr *)p->payload;
#if (RW_MESH_EN)
    VIF_INF_PTR p_vif_entry = rwm_mgmt_vif_idx2ptr(rxhdr->flags_vif_idx);
#endif //(RW_MESH_EN)

    /*
     * format of p->payload
     * 1 MESH type
     ****************************************************
     *  |  DA  |  SA  |  LENGTH  |  MESH_CONTROL  |  DATA  |  PADDING  |
     ****************************************************
     * 2 SNAP type
     ***********************************************************************
     *  |  DA  |  SA  |  LENGTH  |  LLC/SNAP  |  DATA  |  PADDING  |
     ***********************************************************************
     * 3 RAW type (should not happen actually)
     ****************************************************
     *  |  DA  |  SA  |  LENGTH  |  DATA  |  PADDING  |
     ****************************************************
     */

    //os_printf("%s amsdu_len=%d\n", __FUNCTION__, p->len);

    //ieee802.11 amsdu_hdr to ieee802.3 mac_eth_hdr
#if (RW_MESH_EN)
    if ((p_vif_entry->type == VIF_MESH_POINT) && (rxhdr->flags_dst_idx != INVALID_STA_IDX))
    {
        /*
            ****************************************************
            *  |  DA  |  SA  |  LENGTH  |  MESH_CONTROL  |  LLC/SNAP    |  DATA  |  PADDING  |
            ****************************************************
            * ==>
            ****************************************************
            *  |  DA  |  SA  |  ETHERTYPE  |  MESH_CONTROL  |  LLC/SNAP    |  DATA  |  PADDING  |
            ****************************************************
            *  Keep subframe as mesh frame, since amsdu_hdr=mac_eth_hdr
            *  set eth_hdr->len as ethertype like rxu_cntrl_mac2eth_update
            */
        eth_hdr = (struct mac_eth_hdr *)amsdu_subfrm_hdr;
        llc_snap = (struct llc_snap *)((uint8_t *)amsdu_subfrm_hdr + sizeof(struct mac_eth_hdr) + rxhdr->mesh_ctrl_len);
        eth_hdr->len = llc_snap->proto_id;
    }
    else
#endif //(RW_MESH_EN)
    {
        llc_snap = (struct llc_snap *)(amsdu_subfrm_hdr + 1);

        if ((!memcmp(llc_snap, &llc_rfc1042_hdr, sizeof(llc_rfc1042_hdr))
             //&& (llc_snap->ether_type != RX_ETH_PROT_ID_AARP) - Appletalk depracated ?
             && (llc_snap->proto_id != LLC_ETHERTYPE_IPX))
            || (!memcmp(llc_snap, &llc_bridge_tunnel_hdr, sizeof(llc_bridge_tunnel_hdr))))
        {
            /*
                ****************************************************
                *  |  DA  |  SA  |  LENGTH  |  LLC/SNAP  |  DATA  |  PADDING  |
                ****************************************************
                * ==>
                ****************************************************
                *  |  DA  |  SA  |  DATA  |  PADDING  |
                ****************************************************
                */
            eth_hdr = (struct mac_eth_hdr *)((uint8_t *)amsdu_subfrm_hdr + sizeof(struct llc_snap_short) + sizeof(amsdu_subfrm_hdr->len));
            MAC_ADDR_CPY(&eth_hdr->sa, &amsdu_subfrm_hdr->sa);
            MAC_ADDR_CPY(&eth_hdr->da, &amsdu_subfrm_hdr->da);

            pbuf_header(p, -(s16)(sizeof(struct llc_snap_short) + sizeof(amsdu_subfrm_hdr->len)));	// skip off
        }
        else
        {
            /*
                ****************************************************
                *  |  DA  |  SA  |  LENGTH  |  DATA  |  PADDING  |
                ****************************************************
                * ==>
                ****************************************************
                *  |  DA  |  SA  |  DATA  |  PADDING  |
                ****************************************************
                */
            eth_hdr = (struct mac_eth_hdr *)((uint8_t *)amsdu_subfrm_hdr + sizeof(amsdu_subfrm_hdr->len));
            MAC_ADDR_CPY(&temp_mac, &amsdu_subfrm_hdr->sa);
            MAC_ADDR_CPY(&eth_hdr->sa, &temp_mac);
            MAC_ADDR_CPY(&temp_mac, &amsdu_subfrm_hdr->da);
            MAC_ADDR_CPY(&eth_hdr->da, &temp_mac);

            pbuf_header(p, -(s16)sizeof(amsdu_subfrm_hdr->len));	// skip off
        }
    }

    ethernetif_input(rxhdr->flags_vif_idx, p);
}

/**
 * rwm_upload_data - upload 802.3 frame to lwip, and 802.11 frame to wpa_s/hostapd
 *
 * @host_id: pbuf address
 * @frame_len: 802.3/802.11 frame len
 */
UINT32 rwm_upload_data(void *host_id, uint32_t frame_len)
{
	/*
	 * +-----  host_id (struct pbuf{} *)
	 * |
	 * V
	 * +----------+-------------------+
	 * |  rxvect  |   IEEE 802.3 Data |
	 * +----------+-------------------+
	 */
	struct fhost_rx_header *rxhdr;
	struct pbuf *p;
	struct pbuf *q;

	if (!host_id)
		return RW_FAILURE;

	p = (struct pbuf *)host_id;

	/* TBD: Do we need to linearize pbuf here ? Will lwip do the work */
	if (p->next) {
		q = pbuf_coalesce(p, PBUF_RAW);
		if (q == p) // OOM
			warning_prf("nonlinear pbuf\n");
	} else {
		q = p;
	}

	q->len = frame_len + RXL_PAYLOAD_OFFSET;
	rxhdr = (struct fhost_rx_header *)q->payload;

	pbuf_header(q, -(s16)RXL_PAYLOAD_OFFSET);

	if (rxhdr->flags_is_80211_mpdu) {
		rwnx_rx_mgmt_any(q, rxhdr);
	} else if (rxhdr->flags_is_amsdu) {
		/* A-MSDU subframe, convert like `rxu_cntrl_mac2eth_update()' and then pass it to lwip */
		ethernetif_input_amsdu(rxhdr, q);
	} else {
		/* for 802.3 frame, pass it to lwip */
		ethernetif_input(rxhdr->flags_vif_idx, q);
	}

	return RW_SUCCESS;
}

/**
 * NOTE: monitor frame is converted to 802.3 frame if the original
 * 802.11 frameis Data format, but the 802.11 mac header is backed
 * up to `fhost_rx_header.mac_hdr_backup'.
 *
 * host_id: struct pbuf *
 * frame_len: ieee80211 len(rxvect not included)
 */
UINT32 rwm_rx_monitor(void *host_id, uint32_t frame_len)
{
	monitor_cb_t cb;
	cb = bk_wlan_get_monitor_cb();

	if (cb) {
		wifi_link_info_t wli;
		struct fhost_rx_header *rxhdr;
		struct pbuf *p;

		p = (struct pbuf *)host_id;
		p->len = frame_len + RXL_PAYLOAD_OFFSET;
		rxhdr = (struct fhost_rx_header *)p->payload;

		pbuf_header(p, -(s16)RXL_PAYLOAD_OFFSET);	// strip rxvect

		wli.rssi = (rxhdr->hwvect.recvec1c >> 24) & 0xff;

		/*
		 * payload is 802.3 format if original 802.11 is Data format, see
		 * function `rxu_cntrl_mac2eth_update()'.
		 */
		cb(p->payload, frame_len, &wli);

		// recovery pbuf for rwm_upload_data if needed
		pbuf_header(p, (s16)RXL_PAYLOAD_OFFSET);
	}

	return 0;
}
#endif /* CFG_IEEE80211AX */

#if !CFG_IEEE80211AX
UINT32 rwm_uploaded_data_handle(UINT8 *upper_buf, UINT32 len)
{
    UINT32 count;
    UINT32 ret = RW_FAILURE;
    MSDU_NODE_T *node_ptr;

    node_ptr = rwm_pop_rx_list();
    if(node_ptr)
    {
        count = MIN(len, node_ptr->len);
#if CFG_GENERAL_DMA
        gdma_memcpy(upper_buf, node_ptr->msdu_ptr, count);
#else
        os_memcpy(upper_buf, node_ptr->msdu_ptr, count);
#endif
        ret = count;

        os_free(node_ptr);
        node_ptr = NULL;
    }

    return ret;
}
#endif

///////////////////////////////////////////////////////////////////////////////
VIF_INF_PTR rwm_mgmt_vif_idx2ptr(UINT8 vif_idx)
{
    VIF_INF_PTR vif_entry = NULL;

    if(vif_idx < NX_VIRT_DEV_MAX)
        vif_entry = &vif_info_tab[vif_idx];

    return vif_entry;
}

VIF_INF_PTR rwm_mgmt_vif_type2ptr(UINT8 vif_type)
{
    VIF_INF_PTR vif_entry = NULL;
    UINT32 i;

    for(i = 0; i < NX_VIRT_DEV_MAX; i++)
    {
        vif_entry = &vif_info_tab[i];
        if(vif_entry->type == vif_type)
            break;
    }

    if(i == NX_VIRT_DEV_MAX)
        vif_entry = NULL;

    return vif_entry;
}

STA_INF_PTR rwm_mgmt_sta_idx2ptr(UINT8 staid)
{
    STA_INF_PTR sta_entry = NULL;

    if(staid < NX_REMOTE_STA_MAX)
        sta_entry = &sta_info_tab[staid];

    return sta_entry;
}

STA_INF_PTR rwm_mgmt_sta_mac2ptr(void *mac)
{
    UINT32 i;
    STA_INF_PTR sta_entry = NULL;

    for(i = 0; i < NX_REMOTE_STA_MAX; i++)
    {
        sta_entry = &sta_info_tab[i];
        if(MAC_ADDR_CMP((void *)&sta_entry->mac_addr, mac))
            break;
    }

    return sta_entry;
}

UINT8 rwm_mgmt_sta_mac2idx(void *mac)
{
    UINT32 i;
    UINT8 staid = 0xff;
    STA_INF_PTR sta_entry = NULL;

    for(i = 0; i < NX_REMOTE_STA_MAX; i++)
    {
        sta_entry = &sta_info_tab[i];
        if(MAC_ADDR_CMP((void *)&sta_entry->mac_addr, mac))
            break;
    }
    if(i < NX_REMOTE_STA_MAX)
        staid = i;

    return staid;
}

UINT8 rwm_mgmt_sta_mac2port(void *mac)
{
    UINT32 i;
    STA_INF_PTR sta_entry = NULL;

    for(i = 0; i < NX_REMOTE_STA_MAX; i++)
    {
        sta_entry = &sta_info_tab[i];
        if(MAC_ADDR_CMP((void *)&sta_entry->mac_addr, mac))
            break;
    }

	if (sta_entry)
	{
		if (sta_entry->ctrl_port_state == PORT_OPEN)
            return 1;
	}

	return 0;
}

UINT8 rwm_mgmt_vif_mac2idx(void *mac)
{
    VIF_INF_PTR vif_entry = NULL;
    UINT8 vif_idx = INVALID_VIF_IDX;
#if CFG_IEEE80211AX
    UINT8 vif_monitor = INVALID_VIF_IDX;
#endif
    UINT32 i;

    for(i = 0; i < NX_VIRT_DEV_MAX; i++)
    {
        vif_entry = &vif_info_tab[i];
        if(MAC_ADDR_CMP((void *)&vif_entry->mac_addr, mac))
        {
#if CFG_IEEE80211AX
            if (VIF_MONITOR == vif_entry->type)
            {
                vif_monitor = i;
                continue;
            }
#endif
            break;
        }
    }

    if(i < NX_VIRT_DEV_MAX)
        vif_idx = i;
#if CFG_IEEE80211AX
    else if(vif_monitor < NX_VIRT_DEV_MAX)
        vif_idx = vif_monitor;
#endif

    return vif_idx;
}

UINT8 rwm_mgmt_vif_name2idx(char *name)
{
    VIF_INF_PTR vif_entry = NULL;
    struct netif *lwip_if;
    UINT8 vif_idx = 0xff;
    UINT32 i;

    for(i = 0; i < NX_VIRT_DEV_MAX; i++)
    {
        vif_entry = &vif_info_tab[i];
        if(vif_entry->priv)
        {
            lwip_if = (struct netif *)vif_entry->priv;
            if (!os_strncmp(lwip_if->hostname, name, os_strlen(lwip_if->hostname)))
            {
                break;
            }
        }
    }

    if(i < NX_VIRT_DEV_MAX)
        vif_idx = i;

    return vif_idx;
}

UINT8 rwm_mgmt_get_hwkeyidx(UINT8 vif_idx, UINT8 staid)
{
    UINT8 hw_key_idx = MM_SEC_MAX_KEY_NBR + 1;
    struct key_info_tag *key = NULL;

    VIF_INF_PTR vif_entry = NULL;
    STA_INF_PTR sta_entry = NULL;

    if(staid == 0xff)   // group key
    {
        vif_entry = rwm_mgmt_vif_idx2ptr(vif_idx);
        if(vif_entry)
            key = vif_entry->default_key;
    }
    else
    {
        sta_entry = rwm_mgmt_sta_idx2ptr(staid);
        if(sta_entry)
            key = *(sta_entry->sta_sec_info.cur_key);
    }

    if(key)
    {
        hw_key_idx = key->hw_key_idx;
    }

    return hw_key_idx;
}

void rwm_mgmt_set_vif_netif(struct netif *net_if)
{
    VIF_INF_PTR vif_entry = NULL;
    UINT8 vif_idx;

    if(!net_if)
        return;

    vif_idx = rwm_mgmt_vif_mac2idx(net_if->hwaddr);
    vif_entry = rwm_mgmt_vif_idx2ptr(vif_idx);

    if(vif_entry)
    {
        vif_entry->priv = net_if;
        net_if->state = (void *)vif_entry;
    }
    else
    {
        RWNX_LOGI("warnning: set_vif_netif failed\r\n");
    }
}

struct netif *rwm_mgmt_get_vif2netif(UINT8 vif_idx)
{
    VIF_INF_PTR vif_entry = NULL;
    struct netif *netif = NULL;

    vif_entry = rwm_mgmt_vif_idx2ptr(vif_idx);

    if(vif_entry)
        netif = (struct netif *)vif_entry->priv;

    return netif;
}

UINT8 rwm_mgmt_get_netif2vif(struct netif *netif)
{
    UINT8 vif_idx = 0xff;
    VIF_INF_PTR vif_entry = NULL;

    if(netif && netif->state)
    {
        vif_entry = (VIF_INF_PTR)netif->state;
        vif_idx = vif_entry->index;
    }

    return vif_idx;
}

UINT8 rwm_mgmt_tx_get_staidx(UINT8 vif_idx, void *dstmac)
{
    UINT8 staid = 0xff;
    VIF_INF_PTR vif_entry = NULL;

    vif_entry = rwm_mgmt_vif_idx2ptr(vif_idx);

    if(vif_entry)
    {
        if(vif_entry->type == VIF_STA)
        {
            staid = vif_entry->u.sta.ap_id;
        }
        else if(vif_entry->type == VIF_AP)
        {
            staid = rwm_mgmt_sta_mac2idx(dstmac);
        }
    }

    if(staid == 0xff)
    {
        staid = VIF_TO_BCMC_IDX(vif_idx);
    }

    return staid;
}

UINT8 rwm_first_vif_idx()
{
	VIF_INF_PTR vif = rwm_mgmt_is_vif_first_used();
	if (vif)
		return vif->index;

	return INVALID_VIF_IDX;
}

u8 rwn_mgmt_is_only_sta_role_add(void)
{
    VIF_INF_PTR vif_entry = (VIF_INF_PTR)rwm_mgmt_is_vif_first_used();

    if(!vif_entry)
        return 0;

    if(vif_entry->type == VIF_STA)
        return 1;

    return 0;
}

#include "lwip/sockets.h"
extern uint8_t* dhcp_lookup_mac(uint8_t *chaddr);

void rwn_mgmt_show_vif_peer_sta_list(UINT8 role)
{
    struct vif_info_tag *vif = (VIF_INF_PTR)rwm_mgmt_is_vif_first_used();
    struct sta_info_tag *sta;
    UINT8 num = 0;

    while(vif) {
        if ( vif->type == role) {
            sta = (struct sta_info_tag *)co_list_pick(&vif->sta_list);
            while (sta != NULL)
            {
                UINT8 *macptr = (UINT8*)sta->mac_addr.array;
                UINT8 *ipptr = NULL;

                if(role == VIF_AP) {
                    ipptr = dhcp_lookup_mac(macptr);
                } else if (role == VIF_STA){
                    struct netif *netif = (struct netif *)vif->priv;
                    ipptr = (UINT8 *)inet_ntoa(netif->gw);
                }

                RWNX_LOGI("%d: mac:%02x-%02x-%02x-%02x-%02x-%02x, ip:%s\r\n", num++,
                    macptr[0], macptr[1], macptr[2],
                    macptr[3], macptr[4], macptr[5],ipptr);

                sta = (struct sta_info_tag *)co_list_next(&sta->list_hdr);
            }
        }
        vif = (VIF_INF_PTR) rwm_mgmt_next(vif);
    }
}

UINT8 rwn_mgmt_if_ap_stas_empty()
{
    struct vif_info_tag *vif = (VIF_INF_PTR)rwm_mgmt_is_vif_first_used();
    UINT8 role = VIF_AP;

    while(vif) {
        if ( vif->type == role) {
            if(co_list_is_empty(&vif->sta_list))
                {
                return 1;
            }
        }
        vif = (VIF_INF_PTR) rwm_mgmt_next(vif);
    }
    return 0;
}


// eof


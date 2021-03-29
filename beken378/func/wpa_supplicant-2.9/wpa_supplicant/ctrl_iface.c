#include <stdint.h>
#include "utils/includes.h"

#if CFG_WPA_CTRL_IFACE
#include "utils/common.h"
#include "utils/eloop.h"
#include "wlan_defs_pub.h"
#include "wpa_supplicant_i.h"
#include "wpa_ctrl.h"
#include "mem_pub.h"
#include "config.h"
#include "scan.h"
#include "main_none.h"
#include "uart_pub.h"
#include "param_config.h"
#include "common/wpa_common.h"
#include "rsn_supp/wpa.h"
#include "bss.h"
#include "main_none.h"
#include "uart_pub.h"
#include "wlan_ui_pub.h"
#include "mm_bcn.h"
#include "ap/hostapd.h"
#include "ctrl_iface.h"
#include "sa_station.h"
#include "rxu_task.h"
#include "wpa_psk_cache.h"
#include "notifier_pub.h"
#include "ap.h"
#include "net.h"
#include "sm_task.h"
#include "wpa_err.h"

extern beken_thread_t wpas_thread_handle;
static int wpa_ctrl_debug_info_dump(struct wpa_supplicant *wpas, uint32_t type);

int __wpa_ctrl_request(wpa_ctrl_cmd_t cmd, void *data, int wait, uint16_t flags)
{
	wpah_msg_t msg = {0};
	int result = WPA_FAIL;

	msg.cmd = cmd;
	msg.argu = (uint32_t)data;
	msg.sema = 0;
	msg.flags = flags;

	if(wait)
		msg.result = &result;
	else
		result = 0;

	if (rtos_is_current_thread(&wpas_thread_handle)) {
		wpa_supplicant_ctrl_iface_receive(&msg);
	} else {
		int ret = 0;

		if (wait && rtos_init_semaphore(&msg.sema, 1) != kNoErr)
			return WPA_ERR_CTRL_SEM_INIT;

		ret = wpa_hostapd_queue_command(&msg);
		if (ret != kNoErr) {
			result = ret;
			goto out;
		}

		if (wait)
			rtos_get_semaphore(&msg.sema, BEKEN_WAIT_FOREVER);	// XXX: forever?

out:
		if (wait)
			rtos_deinit_semaphore(&msg.sema);
	}

	return result;
}

/* sync */
int wpa_ctrl_request(wpa_ctrl_cmd_t cmd, void *data)
{
	return __wpa_ctrl_request(cmd, data, 1, 0);
}

int wpa_ctrl_request_async(wpa_ctrl_cmd_t cmd, void *data)
{
	return __wpa_ctrl_request(cmd, data, 0, 0);
}

/*
 * event >= WPA_CTRL_CMD_RW_EVT_START
 * TODO: free
 */
int wpa_ctrl_event(int event, void *data)
{
	return __wpa_ctrl_request(event, data, 0, 0);
}

int wpa_ctrl_event_sync(int event, void *data)
{
	return __wpa_ctrl_request(event, data, 1, 0);
}

int wpa_ctrl_event_copy(int event, void *data, int len)
{
	void *p = os_malloc(len);
	if (!p)
		return -1;
	os_memcpy(p, data, len);

	return __wpa_ctrl_request(event, p, 0, WPAH_FLAG_FREE);
}

static int wpa_supplicant_ctrl_iface_select_network(
	struct wpa_supplicant *wpa_s, int id, int chan)
{
	struct wpa_ssid *ssid;

	/* cmd: "<network id>" or "any" */
	if (id == -1) {
		wpa_printf(MSG_DEBUG, "CTRL_IFACE: SELECT_NETWORK any");
		ssid = NULL;
	} else {
		wpa_printf(MSG_DEBUG, "CTRL_IFACE: SELECT_NETWORK id=%d", id);

		ssid = wpa_config_get_network(wpa_s->conf, id);
		if (ssid == NULL) {
			wpa_printf(MSG_DEBUG, "CTRL_IFACE: Could not find "
				   "network id=%d", id);
			return -1;
		}
		if (ssid->disabled == 2) {
			wpa_printf(MSG_DEBUG, "CTRL_IFACE: Cannot use "
				   "SELECT_NETWORK with persistent P2P group");
			return -1;
		}
	}

	if (chan) {
		int *freqs = (int *)os_zalloc(sizeof(int) * 2);	// zero terminated
		if (freqs) {
			os_free(wpa_s->select_network_scan_freqs);

			freqs[0] = rw_ieee80211_get_centre_frequency(chan);
			wpa_s->select_network_scan_freqs = freqs;
		}
	}

#if 0
	pos = os_strstr(cmd, " freq=");
	if (pos) {
		int *freqs = freq_range_to_channel_list(wpa_s, pos + 6);
		if (freqs) {
			wpa_s->scan_req = MANUAL_SCAN_REQ;
			os_free(wpa_s->manual_scan_freqs);
			wpa_s->manual_scan_freqs = freqs;
		}
	}
#endif

#if 0
	if(oob_ssid)
	{
		int len;
		int oob_ssid_len;

		ASSERT(0 == wpa_s->ssids_from_scan_req);
		oob_ssid_len = os_strlen(oob_ssid);

		if(0 == wpas_connect_ssid)
		{
			wpas_connect_ssid = (struct wpa_ssid_value *)os_malloc(sizeof(struct wpa_ssid_value));
			ASSERT(wpas_connect_ssid);
		}

		len = MIN(SSID_MAX_LEN, oob_ssid_len);

		wpas_connect_ssid->ssid_len = len;
		os_memcpy(wpas_connect_ssid->ssid, oob_ssid, len);

		wpa_s->num_ssids_from_scan_req = 1;
		wpa_s->ssids_from_scan_req = wpas_connect_ssid;
		wpa_s->scan_req = MANUAL_SCAN_REQ;
		WPA_LOGI("MANUAL_SCAN_REQ\r\n");
	}
#endif

	wpa_s->scan_min_time.sec = 0;
	wpa_s->scan_min_time.usec = 0;
	wpa_supplicant_select_network(wpa_s, ssid);

	return 0;
}

int wpa_supplicant_ctrl_iface_scan_results(
	struct wpa_supplicant *wpa_s, ScanResult_adv *results)
{
	struct wpa_bss *bss;
	int bss_num = 0;
	int index = 0;

	results->ApNum = 0;
	results->ApList = 0;
	dl_list_for_each(bss, &wpa_s->bss_id, struct wpa_bss, list_id) {
		bss_num++;
	}
	if (!bss_num)
		return 0;

	results->ApList = os_malloc(sizeof(*results->ApList) * bss_num);
	if (!results->ApList)
		return -1;
	dl_list_for_each(bss, &wpa_s->bss_id, struct wpa_bss, list_id) {
		os_memcpy(results->ApList[index].ssid, bss->ssid, bss->ssid_len);
		results->ApList[index].ssid[bss->ssid_len] = '\0';
		results->ApList[index].ApPower = bss->level;
		ieee80211_freq_to_chan(bss->freq, (u8 *)&results->ApList[index].channel);
		os_memcpy(results->ApList[index].bssid, bss->bssid, ETH_ALEN);
		results->ApList[index].security = get_security_type_from_ie((u8 *) (bss + 1), bss->ie_len, bss->caps);

		index++;
	}
	results->ApNum = bss_num;

	return 0;
}

static int wpas_ctrl_scan(struct wpa_supplicant *wpa_s, wlan_sta_scan_param_t *params)
{
	unsigned int manual_scan_passive = 0;
	unsigned int manual_scan_use_id = 1;
	unsigned int manual_scan_only_new = 0;
	unsigned int scan_only = 1;
	unsigned int scan_id_count = 0;
	int scan_id[MAX_SCAN_ID];
	void (*scan_res_handler)(struct wpa_supplicant *wpa_s,
				 struct wpa_scan_results *scan_res);
	int *manual_scan_freqs = NULL;
	struct wpa_ssid_value *ssid = NULL;
	unsigned int ssid_count = 0;
	int ret = 0;
#ifdef CONFIG_FULL_SUPPLICANT
	struct wpa_ssid_value *ns;
	char *pos;
#endif

	if (wpa_s->wpa_state == WPA_INTERFACE_DISABLED) {
		return WPA_ERR_STATE;
	}

	if (radio_work_pending(wpa_s, "scan")) {
		wpa_printf(MSG_DEBUG,
			   "Pending scan scheduled - reject new request");
		return WPA_ERR_SCAN_PENDING;
	}

#ifdef CONFIG_INTERWORKING
	if (wpa_s->fetch_anqp_in_progress || wpa_s->network_select) {
		wpa_printf(MSG_DEBUG,
			   "Interworking select in progress - reject new scan");
		return WPA_ERR_SCAN_SELECT_IN_PROGRESS;
	}
#endif /* CONFIG_INTERWORKING */

#ifdef CONFIG_FULL_SUPPLICANT
	if (params) {
		if (os_strncasecmp(params, "TYPE=ONLY", 9) == 0)
			scan_only = 1;

		pos = os_strstr(params, "freq=");
		if (pos) {
			manual_scan_freqs = freq_range_to_channel_list(wpa_s,
								       pos + 5);
			if (manual_scan_freqs == NULL) {
				ret = WPA_ERR_SCAN_FREQ;
				goto done;
			}
		}

		pos = os_strstr(params, "passive=");
		if (pos)
			manual_scan_passive = !!atoi(pos + 8);

		pos = os_strstr(params, "use_id=");
		if (pos)
			manual_scan_use_id = atoi(pos + 7);

		pos = os_strstr(params, "only_new=1");
		if (pos)
			manual_scan_only_new = 1;

		pos = os_strstr(params, "scan_id=");
		if (pos && scan_id_list_parse(wpa_s, pos + 8, &scan_id_count,
					      scan_id) < 0) {
			ret = WPA_ERR_SCAN_ID;
			goto done;
		}

		pos = os_strstr(params, "bssid=");
		if (pos) {
			u8 bssid[ETH_ALEN];

			pos += 6;
			if (hwaddr_aton(pos, bssid)) {
				wpa_printf(MSG_ERROR, "Invalid BSSID %s", pos);
				ret = WPA_ERR_SCAN_BSSID;
				goto done;
			}
			os_memcpy(wpa_s->next_scan_bssid, bssid, ETH_ALEN);
		}

		pos = params;
		while (pos && *pos != '\0') {
			if (os_strncmp(pos, "ssid ", 5) == 0) {
				char *end;

				pos += 5;
				end = pos;
				while (*end) {
					if (*end == '\0' || *end == ' ')
						break;
					end++;
				}

				ns = os_realloc_array(
					ssid, ssid_count + 1,
					sizeof(struct wpa_ssid_value));
				if (ns == NULL) {
					*reply_len = -1;
					goto done;
				}
				ssid = ns;

				if ((end - pos) & 0x01 ||
				    end - pos > 2 * SSID_MAX_LEN ||
				    hexstr2bin(pos, ssid[ssid_count].ssid,
					       (end - pos) / 2) < 0) {
					wpa_printf(MSG_DEBUG,
						   "Invalid SSID value '%s'",
						   pos);
					ret = WPA_ERR_SCAN_SSID;
					goto done;
				}
				ssid[ssid_count].ssid_len = (end - pos) / 2;
				wpa_hexdump_ascii(MSG_DEBUG, "scan SSID",
						  ssid[ssid_count].ssid,
						  ssid[ssid_count].ssid_len);
				ssid_count++;
				pos = end;
			}

			pos = os_strchr(pos, ' ');
			if (pos)
				pos++;
		}
	}
#else
	if (params) {
		ssid_count = params->num_ssids;
		ssid = os_realloc_array(ssid, ssid_count, sizeof(struct wpa_ssid_value));
		if (ssid == NULL) {
			ret = WPA_ERR_NO_MEM;
			goto done;
		}

		/* set scan ssid */
		for (int i = 0; i < ssid_count; i++) {
			os_memcpy(ssid[i].ssid, params->ssids[i].ssid, params->ssids[i].ssid_len);
			ssid[i].ssid_len = params->ssids[i].ssid_len;
#if CFG_SUPPORT_BSSID_CONNECT
			os_memset(ssid[i].bssid, 0xFF, ETH_ALEN);
#endif
			wpa_hexdump_ascii(MSG_DEBUG, "scan SSID",
					  ssid[i].ssid,
					  ssid[i].ssid_len);
		}
	} else {
		/* do wildcard scan */
		ssid_count = 1;
		ssid = os_calloc(1, sizeof(*ssid));
		if (!ssid) {
			ret = WPA_ERR_NO_MEM;
			goto done;
		}
	}
#endif

	wpa_s->num_ssids_from_scan_req = ssid_count;
	os_free(wpa_s->ssids_from_scan_req);
	if (ssid_count) {
		wpa_s->ssids_from_scan_req = ssid;
		ssid = NULL;
	} else {
		wpa_s->ssids_from_scan_req = NULL;
	}

	if (scan_only)
		scan_res_handler = scan_only_handler;
	else if (wpa_s->scan_res_handler == scan_only_handler)
		scan_res_handler = NULL;
	else
		scan_res_handler = wpa_s->scan_res_handler;

	if (!wpa_s->sched_scanning && !wpa_s->scanning &&
	    ((wpa_s->wpa_state <= WPA_SCANNING) ||
	     (wpa_s->wpa_state == WPA_COMPLETED))) {
		wpa_s->manual_scan_passive = manual_scan_passive;
		wpa_s->manual_scan_use_id = manual_scan_use_id;
		wpa_s->manual_scan_only_new = manual_scan_only_new;
		wpa_s->scan_id_count = scan_id_count;
		os_memcpy(wpa_s->scan_id, scan_id, scan_id_count * sizeof(int));
		wpa_s->scan_res_handler = scan_res_handler;
		os_free(wpa_s->manual_scan_freqs);
		wpa_s->manual_scan_freqs = manual_scan_freqs;
		manual_scan_freqs = NULL;

		wpa_s->normal_scans = 0;
		wpa_s->scan_req = MANUAL_SCAN_REQ;
#ifdef CONFIG_WPS
		wpa_s->after_wps = 0;
		wpa_s->known_wps_freq = 0;
#endif
		ret = wpa_supplicant_req_scan(wpa_s, 0, 0);
		if (wpa_s->manual_scan_use_id) {
			wpa_s->manual_scan_id++;
			if (!wpa_s->manual_scan_id)
				wpa_s->manual_scan_id = 1;
			wpa_dbg(wpa_s, MSG_DEBUG, "Assigned scan id %u",
				wpa_s->manual_scan_id);
			//*reply_len = os_snprintf(reply, reply_size, "%u\n",
			//			 wpa_s->manual_scan_id);
		}
#if 0
	} else if (wpa_s->sched_scanning) {
		wpa_s->manual_scan_passive = manual_scan_passive;
		wpa_s->manual_scan_use_id = manual_scan_use_id;
		wpa_s->manual_scan_only_new = manual_scan_only_new;
		wpa_s->scan_id_count = scan_id_count;
		os_memcpy(wpa_s->scan_id, scan_id, scan_id_count * sizeof(int));
		wpa_s->scan_res_handler = scan_res_handler;
		os_free(wpa_s->manual_scan_freqs);
		wpa_s->manual_scan_freqs = manual_scan_freqs;
		manual_scan_freqs = NULL;

		wpa_printf(MSG_DEBUG, "Stop ongoing sched_scan to allow requested full scan to proceed");
		wpa_supplicant_cancel_sched_scan(wpa_s);
		wpa_s->scan_req = MANUAL_SCAN_REQ;
		wpa_supplicant_req_scan(wpa_s, 0, 0);
		if (wpa_s->manual_scan_use_id) {
			wpa_s->manual_scan_id++;
			//*reply_len = os_snprintf(reply, reply_size, "%u\n",
			//			 wpa_s->manual_scan_id);
			wpa_dbg(wpa_s, MSG_DEBUG, "Assigned scan id %u",
				wpa_s->manual_scan_id);
		}
#endif
	} else {
		wpa_printf(MSG_DEBUG, "Ongoing scan action - reject new request");

		if (wpa_s->sched_scanning)
			ret = WPA_ERR_SCAN_SCHED_IN_PROGRESS;
		else if (wpa_s->scanning)
			ret = WPA_ERR_SCAN_IN_PROGRESS;
		else
			ret = WPA_ERR_TRY_AGAIN;
	}

done:
	os_free(manual_scan_freqs);
	os_free(ssid);

	return ret;
}


#ifdef CONFIG_WPA_SUPPLICANT_MULTI_NETWORK
static int wpa_supplicant_ctrl_iface_add_network(struct wpa_supplicant *wpa_s, int id)
{
	struct wpa_ssid *ssid;
	int ret;

	wpa_printf(MSG_DEBUG, "CTRL_IFACE: ADD_NETWORK");

	ssid = wpa_config_add_network(wpa_s->conf);
	if (ssid == NULL)
		return -1;

	wpas_notify_network_added(wpa_s, ssid);

	ssid->disabled = 1;
	wpa_config_set_network_defaults(ssid);

	return 0;
}

static int wpa_supplicant_ctrl_iface_remove_network(struct wpa_supplicant *wpa_s, int id)
{
	int id;
	struct wpa_ssid *ssid;
	int was_disabled;

	/* cmd: "<network id>" or "all" */
	if (id == -1) {
		wpa_printf(MSG_DEBUG, "CTRL_IFACE: REMOVE_NETWORK all");
		if (wpa_s->sched_scanning)
			wpa_supplicant_cancel_sched_scan(wpa_s);

		eapol_sm_invalidate_cached_session(wpa_s->eapol);
		if (wpa_s->current_ssid) {
#ifdef CONFIG_SME
			wpa_s->sme.prev_bssid_set = 0;
#endif /* CONFIG_SME */
			wpa_sm_set_config(wpa_s->wpa, NULL);
			eapol_sm_notify_config(wpa_s->eapol, NULL, NULL);
			if (wpa_s->wpa_state >= WPA_AUTHENTICATING)
				wpa_s->own_disconnect_req = 1;
			wpa_supplicant_deauthenticate(
				wpa_s, WLAN_REASON_DEAUTH_LEAVING);
		}
		ssid = wpa_s->conf->ssid;
		while (ssid) {
			struct wpa_ssid *remove_ssid = ssid;
			id = ssid->id;
			ssid = ssid->next;
			if (wpa_s->last_ssid == remove_ssid)
				wpa_s->last_ssid = NULL;
			wpas_notify_network_removed(wpa_s, remove_ssid);
			wpa_config_remove_network(wpa_s->conf, id);
		}
		return 0;
	}

	wpa_printf(MSG_DEBUG, "CTRL_IFACE: REMOVE_NETWORK id=%d", id);

	ssid = wpa_config_get_network(wpa_s->conf, id);
	if (ssid)
		wpas_notify_network_removed(wpa_s, ssid);
	if (ssid == NULL) {
		wpa_printf(MSG_DEBUG, "CTRL_IFACE: Could not find network "
			   "id=%d", id);
		return -1;
	}

	if (wpa_s->last_ssid == ssid)
		wpa_s->last_ssid = NULL;

	if (ssid == wpa_s->current_ssid || wpa_s->current_ssid == NULL) {
#ifdef CONFIG_SME
		wpa_s->sme.prev_bssid_set = 0;
#endif /* CONFIG_SME */
		/*
		 * Invalidate the EAP session cache if the current or
		 * previously used network is removed.
		 */
		eapol_sm_invalidate_cached_session(wpa_s->eapol);
	}

	if (ssid == wpa_s->current_ssid) {
		wpa_sm_set_config(wpa_s->wpa, NULL);
		eapol_sm_notify_config(wpa_s->eapol, NULL, NULL);

		if (wpa_s->wpa_state >= WPA_AUTHENTICATING)
			wpa_s->own_disconnect_req = 1;
		wpa_supplicant_deauthenticate(wpa_s,
					      WLAN_REASON_DEAUTH_LEAVING);
	}

	was_disabled = ssid->disabled;

	if (wpa_config_remove_network(wpa_s->conf, id) < 0) {
		wpa_printf(MSG_DEBUG, "CTRL_IFACE: Not able to remove the "
			   "network id=%d", id);
		return -1;
	}

	if (!was_disabled && wpa_s->sched_scanning) {
		wpa_printf(MSG_DEBUG, "Stop ongoing sched_scan to remove "
			   "network from filters");
		wpa_supplicant_cancel_sched_scan(wpa_s);
		wpa_supplicant_req_scan(wpa_s, 0, 0);
	}

	return 0;
}
#endif

int wpa_supplicant_ctrl_iface_set_network(struct wpa_supplicant *wpa_s, wlan_sta_config_t *config)
{
	struct wpa_ssid *ssid;
	int id = 0;
#ifdef CONFIG_WPA_SUPPLICANT_MULTI_NETWORK
	id = config->id;
#endif

	ssid = wpa_config_get_network(wpa_s->conf, id);
	if (ssid == NULL) {
		wpa_printf(MSG_DEBUG, "CTRL_IFACE: Could not find network "
			   "id=%d", id);
		return -1;
	}

	switch (config->field) {
	case WLAN_STA_FIELD_SSID:
		//WPA_LOGI("ssid: |%s|\n", config->u.ssid.ssid);
		if ((ssid->ssid_len != config->u.ssid.ssid_len) ||
			os_memcmp(ssid->ssid, config->u.ssid.ssid, ssid->ssid_len)) {
			ssid->psk_set = 0;	// recalc psk
			ssid->mem_only_psk = 1;
			if (wpa_s->wpa)
				wpa_sm_pmksa_cache_flush(wpa_s->wpa, ssid);
		}
		if (ssid->ssid)
			os_free(ssid->ssid);
		ssid->ssid = (u8 *)dup_binstr(config->u.ssid.ssid, config->u.ssid.ssid_len);
		ssid->ssid_len = config->u.ssid.ssid_len;
		break;

	case WLAN_STA_FIELD_BSSID:
		//WPA_LOGI("bssid set\n");
		os_memcpy(ssid->bssid, config->u.bssid, ETH_ALEN);
		ssid->bssid_set = 1;
		break;

	case WLAN_STA_FIELD_FREQ:
		//ssid->frequency = ieee80211_chan_to_freq("CN", 7, config->u.channel);
		//WPA_LOGI("ssid->freq = %d\n", ssid->frequency);
		break;

	case WLAN_STA_FIELD_PSK:
		//WPA_LOGI("psk: |%s|\n", config->u.psk);
		if (config->u.psk[0] != '\0') {
			if (os_strcmp(ssid->passphrase, (char *)config->u.psk)) {
				ssid->psk_set = 0;
				ssid->mem_only_psk = 1;
				if (wpa_s->wpa)
					wpa_sm_pmksa_cache_flush(wpa_s->wpa, ssid);
			}
			str_clear_free(ssid->passphrase);
			ssid->passphrase = os_strdup(config->u.psk);	//dup_binstr(g_sta_param_ptr->key, g_sta_param_ptr->key_len);
			//ssid->mem_only_psk = 1;

			//if (ssid->changes) {
				//wpa_config_update_psk(ssid);
				//ssid->changes = 0;
			//}
		}
		break;
	case WLAN_STA_FIELD_WEP_KEY0:
		//wpa_config_parse_wep_key
		break;
	case WLAN_STA_FIELD_WEP_KEY1:
		break;
	case WLAN_STA_FIELD_WEP_KEY2:
		break;
	case WLAN_STA_FIELD_WEP_KEY3:
		break;
	case WLAN_STA_FIELD_WEP_KEY_INDEX:
		ssid->wep_tx_keyidx = config->u.wep_tx_keyidx;
		break;
	case WLAN_STA_FIELD_KEY_MGMT:
		//WPA_LOGI("key_mgmt: %d\n", __func__, config->u.key_mgmt);
		ssid->key_mgmt = config->u.key_mgmt;
		break;
	case WLAN_STA_FIELD_PAIRWISE_CIPHER:
		//WPA_LOGI("pairwise_cipher: %d\n", __func__, config->u.pairwise_cipher);
		ssid->pairwise_cipher = config->u.pairwise_cipher;
		break;
	case WLAN_STA_FIELD_GROUP_CIPHER:
		//WPA_LOGI("group_cipher: %d\n", __func__, config->u.group_cipher);
		ssid->group_cipher = config->u.group_cipher;
		break;
	case WLAN_STA_FIELD_PROTO:
		//WPA_LOGI("proto: %d\n", __func__, config->u.proto);
		ssid->proto = config->u.proto;
		break;
	case WLAN_STA_FIELD_AUTH_ALG:
		//WPA_LOGI("auth_alg: %d\n", __func__, config->u.auth_alg);
		ssid->auth_alg = config->u.auth_alg;
		break;
	case WLAN_STA_FIELD_WPA_PTK_REKEY:
		//WPA_LOGI("wpa_ptk_rekey: %d\n", __func__, config->u.wpa_ptk_rekey);
		ssid->wpa_ptk_rekey = config->u.wpa_ptk_rekey;
		break;
	case WLAN_STA_FIELD_SCAN_SSID:
		//WPA_LOGI("scan_ssid: %d\n", __func__, config->u.scan_ssid);
		ssid->scan_ssid = config->u.scan_ssid;
		break;

	case WLAN_STA_FIELD_SAE_GROUPS: {
		int *groups = os_malloc(sizeof(config->u.sae_groups));
		if (groups) {
			os_free(wpa_s->conf->sae_groups);
			os_memcpy(groups, config->u.sae_groups, sizeof(config->u.sae_groups));
			wpa_s->conf->sae_groups = groups;
			//while (*groups) {
			//	WPA_LOGI("\t sae_group: %d\n", *groups);
			//	groups++;
			//}
		}
	}	break;

	case WLAN_STA_FIELD_MFP:
#ifdef CONFIG_IEEE80211W
		ssid->ieee80211w = config->u.ieee80211w;
#endif
		break;

	case WLAN_STA_FIELD_DEBUG_LEVEL:
		wpa_debug_level = config->u.debug_level;
		break;

	case WLAN_STA_FIELD_DEBUG_SHOW_KEYS:
		wpa_debug_show_keys = config->u.debug_show_keys;
		break;

	default:
		return -1;
	}

	return 0;
}

static int supplicant_started = 0;
static int hostapd_started = 0;

int wpa_supplicant_ctrl_iface_receive(wpah_msg_t *msg)
{
	struct wpa_supplicant *wpa_s = wpa_suppliant_ctrl_get_wpas();
	struct hapd_interfaces *interfaces = hostapd_ctrl_get_interfaces();
	int res = 0;

#define CHECK_WPA_S()				\
	do {							\
		if (!wpa_s) {\
			res = WPA_ERR_WPAS_INIT;\
			goto exit;		\
		}\
	} while (0)

#define CHECK_HAPD()\
	do {							\
		if (!interfaces || !interfaces->count)\
			goto exit;				\
	} while (0)


	//WPA_LOGI("%s: cmd %d\r\n", __func__, msg->cmd);
	switch (msg->cmd) {
#ifdef CONFIG_WPA_SUPPLICANT_MULTI_NETWORK
	case WPA_CTRL_CMD_ADD_NETWORK:
		CHECK_WPA_S();
		wpa_supplicant_ctrl_iface_add_network(wpa_s, (int)msg->argu);
		break;
	case WPA_CTRL_CMD_REMOVE_NETWORK:
		CHECK_WPA_S();
		wpa_supplicant_ctrl_iface_remove_network(wpa_s, (int)msg->argu);
		break;
#endif
	case WPA_CTRL_CMD_SELECT_NETWORK: {
		res = -1;
		CHECK_WPA_S();

		res = wpa_supplicant_ctrl_iface_select_network(wpa_s, 0/*(int)msg->param*/, (int)msg->argu);
	}	break;

	case WPA_CTRL_CMD_STA_SCAN:
		CHECK_WPA_S();
		res = wpas_ctrl_scan(wpa_s, (wlan_sta_scan_param_t *)msg->argu);
		break;
	case WPA_CTRL_CMD_STA_SCAN_INTERVAL:
		CHECK_WPA_S();
		break;
//	case WPA_CTRL_CMD_STA_CONNECT:
//		CHECK_WPA_S();
//		wpa_supplicant_ctrl_iface_select_network(wpa_s, (int)msg->argu);
//		break;
	case WPA_CTRL_CMD_STA_REASSOCIATE:
		CHECK_WPA_S();
		if (wpa_s->disconnected)
			wpas_request_connection(wpa_s);
		break;
	case WPA_CTRL_CMD_STA_DISCONNECT:
		CHECK_WPA_S();
#if CFG_USE_STA_PS
		bk_wlan_dtim_rf_ps_disable_send_msg();
#endif
#ifdef CONFIG_SME
		wpa_s->sme.prev_bssid_set = 0;
#endif /* CONFIG_SME */
		wpa_s->reassociate = 0;
		wpa_s->disconnected = 1;
		wpa_supplicant_cancel_sched_scan(wpa_s);
		wpa_supplicant_cancel_scan(wpa_s);
		wpa_supplicant_deauthenticate(wpa_s,
						  WLAN_REASON_DEAUTH_LEAVING);
		eloop_cancel_timeout(wpas_network_reenabled, wpa_s, NULL);
		break;
	case WPA_CTRL_CMD_STA_ENABLE:
		/* Enable the station */
		if (!supplicant_started) {
			uint8_t mac[ETH_ALEN];

			//sa_station_init();

			supplicant_main_entry(NULL);
			wifi_get_mac_address((char *)mac, CONFIG_ROLE_STA);
			net_wlan_add_netif(mac);
			supplicant_started = 1;
		}
		break;
	case WPA_CTRL_CMD_STA_DISABLE:
		/* disable the station */
		if (supplicant_started) {
			net_wlan_remove_netif(&g_sta_param_ptr->own_mac);
			supplicant_main_exit();
			wpa_hostapd_release_scan_rst();
			supplicant_started = 0;
		}
		break;
	case WPA_CTRL_CMD_STA_SET: {
		CHECK_WPA_S();
		wlan_sta_config_t *config = (wlan_sta_config_t *)msg->argu;
		*msg->result = wpa_supplicant_ctrl_iface_set_network(wpa_s, config);
	}
		break;
	case WPA_CTRL_CMD_STA_GET:
		CHECK_WPA_S();
		break;
	case WPA_CTRL_CMD_STA_AUTOCONNECT:
		CHECK_WPA_S();
		break;
	case WPA_CTRL_CMD_STA_STATE:
		CHECK_WPA_S();
		break;
	case WPA_CTRL_CMD_STA_AP:
		/* Get the information of connected AP */
		CHECK_WPA_S();
		break;
	case WPA_CTRL_CMD_STA_BSS_MAX_COUNT:
		CHECK_WPA_S();
		break;
	case WPA_CTRL_CMD_STA_BSS_FLUSH:
		CHECK_WPA_S();
		wpa_bss_flush(wpa_s);
		break;
	case WPA_CTRL_CMD_STA_BSS_SIZE_GET:
		CHECK_WPA_S();
		break;
	case WPA_CTRL_CMD_STA_BSS_GET:
		CHECK_WPA_S();
		break;
	case WPA_CTRL_CMD_STA_BSS_SET:
		CHECK_WPA_S();
		break;
	case WPA_CTRL_CMD_STA_GEN_PSK:
		CHECK_WPA_S();
		break;
	case WPA_CTRL_CMD_STA_SCAN_RESULTS: {
		CHECK_WPA_S();
		res = wpa_supplicant_ctrl_iface_scan_results(wpa_s, (ScanResult_adv *)msg->argu);
	}
		break;
	case WPA_CTRL_CMD_STA_WPS_PBC:
		CHECK_WPA_S();
		break;
	case WPA_CTRL_CMD_STA_WPS_GET_PIN:
		CHECK_WPA_S();
		break;
	case WPA_CTRL_CMD_STA_WPS_SET_PIN:
		CHECK_WPA_S();
		break;


	/* ============== softap ================= */
	case WPA_CTRL_CMD_AP_ENABLE:
		/* Enable the AP */
		//hostapd_main_entry(2, 0);
		if (!hostapd_started && !hostapd_main_entry(1, 0)) {
			// don't add any interface, just a dummy ctrl iface
			hostapd_started = 1;
		}
		break;

	/* Reload AP configuration */
	case WPA_CTRL_CMD_AP_RELOAD: {
		int i;

		if (!interfaces) {
			res = -1;
			break;
		}

		// try to add a new hostapd_iface if not exist
		if (!interfaces->count) {
			char bss_config[] = "bss_config=phy0:dummy.conf";
			res = hostapd_add_iface(interfaces, bss_config);
			if (res)
				break;
			net_wlan_add_netif(&g_ap_param_ptr->bssid);

			/* new added iface, just return*/
			break;
		}

		/* reload configuration */
		if (interfaces->reload_config) {
			for (i = 0; i < interfaces->count; i++) {
				res = interfaces->reload_config(interfaces->iface[i]);
				if (res)
					break;
			}
		}
	}	break;

	case WPA_CTRL_CMD_AP_DISABLE: {
		int flag, empty;

		uap_ip_down();
		net_wlan_remove_netif(&g_ap_param_ptr->bssid);

		if (hostapd_started) {
			hostapd_main_exit();
			hostapd_started = 0;
		}

		/* wait for bcn all transmitted */
		GLOBAL_INT_DECLARATION();
		while (1) {
			GLOBAL_INT_DISABLE();
			flag = mm_bcn_get_tx_cfm();
			empty = is_apm_bss_config_empty();
			if (flag == 0 && empty == 1) {
				GLOBAL_INT_RESTORE();
				break;
			} else {
				GLOBAL_INT_RESTORE();
				rtos_delay_milliseconds(50);
			}
		}
	}	break;

	case WPA_CTRL_CMD_AP_SET:
		//hostapd_set_iface -> hostapd_config_fill
		break;
	case WPA_CTRL_CMD_AP_GET:
		break;
	case WPA_CTRL_CMD_AP_STA_NUM:
		break;
	case WPA_CTRL_CMD_AP_STA_INFO:
		break;

	/* channel switch request */
	case WPA_CTRL_CMD_AP_CHAN_SWITCH: {
		int new_freq = (int)msg->argu;

		res = -1;
		CHECK_HAPD();
		res = ap_channel_switch(interfaces->iface[0], new_freq);
	}	break;


	/* ============== MISC ======== */
	case WPA_CTLR_CMD_ADD_NOTIFIER: {
		struct notifier_req *req = (struct notifier_req *)msg->argu;
		register_wlan_notifier(req->func, req->arg);
	}	break;

	case WPA_CTLR_CMD_REMOVE_NOTIFIER: {
		struct notifier_req *req = (struct notifier_req *)msg->argu;
		remove_wlan_notifier(req->func, req->arg);
	}	break;

	case WPA_CTRL_CMD_DEBUG_INFO_DUMP: {
		CHECK_WPA_S();
		wpa_ctrl_debug_info_dump(wpa_s, (uint32_t)msg->argu);
		break;
	}
	}

exit:
	if (msg->result)
		*msg->result = res;	//FIXME
	if (msg->sema)
		rtos_set_semaphore(&msg->sema);
	if (msg->flags & WPAH_FLAG_FREE)
		os_free((void *)msg->argu);

#undef CHECK_WPA_S

	return 0;
}

static void mlme_event_unprot_disconnect(struct wpa_supplicant *wpa_s,
					 enum wpa_event_type type,
					 const u8 *frame, size_t len)
{
	const struct ieee80211_mgmt *mgmt;
	union wpa_event_data event;
	u16 reason_code = 0;

	if (type == EVENT_UNPROT_DEAUTH)
		wpa_printf(MSG_DEBUG, "Unprot Deauthenticate event");
	else
		wpa_printf(MSG_DEBUG, "Unprot Disassociate event");

	if (len < 24)
		return;

	mgmt = (const struct ieee80211_mgmt *) frame;

	os_memset(&event, 0, sizeof(event));
	/* Note: Same offset for Reason Code in both frame subtypes */
	if (len >= 24 + sizeof(mgmt->u.deauth))
		reason_code = le_to_host16(mgmt->u.deauth.reason_code);

	if (type == EVENT_UNPROT_DISASSOC) {
		event.unprot_disassoc.sa = mgmt->sa;
		event.unprot_disassoc.da = mgmt->da;
		event.unprot_disassoc.reason_code = reason_code;
	} else {
		event.unprot_deauth.sa = mgmt->sa;
		event.unprot_deauth.da = mgmt->da;
		event.unprot_deauth.reason_code = reason_code;
	}

	wpa_supplicant_event_sta(wpa_s, type, &event);
}

int wpa_supplicant_handle_events(wpah_msg_t *msg)
{
	struct wpa_supplicant *wpa_s = wpa_suppliant_ctrl_get_wpas();
	int res = 0;

#define CHECK_WPA_S()				\
	do {							\
		if (!wpa_s) goto exit;		\
	} while (0)

	//WPA_LOGI("%s: evt %d\r\n", __func__, msg->cmd);

	CHECK_WPA_S();

	switch (msg->cmd) {
	case WPA_CTRL_EVENT_AUTH_IND: {
#ifdef CONFIG_SME
	    struct sm_auth_indication *ind = (struct sm_auth_indication *)msg->argu;

		if (ind) {
			union wpa_event_data data;

			os_memset(&data, 0, sizeof(data));

			os_memcpy(data.auth.bssid, &ind->bssid, ETH_ALEN);
			os_memcpy(data.auth.peer, &ind->bssid, ETH_ALEN);
			data.auth.auth_type = ind->auth_type;
			data.auth.auth_transaction = ind->auth_transaction;
			data.auth.status_code = ind->status_code;
			data.auth.ies = ind->ie_buf;
			data.auth.ies_len = ind->ie_len;

			//WPA_LOGI("%s: WPA_CTRL_EVENT_AUTH_IND\n", __func__);
			wpa_supplicant_event_sta(wpa_s, EVENT_AUTH, &data);
		} else {
			WPA_LOGE("%s %d: null\n", __func__, __LINE__);
		}
#endif
	}	break;

#ifdef CONFIG_SAE_EXTERNAL
	case WPA_CTRL_EVENT_EXTERNAL_AUTH_IND: {
		struct sm_external_auth_required_ind *ind = (struct sm_external_auth_required_ind *)msg->argu;

		if (ind) {
			union wpa_event_data data;

			os_memset(&data, 0, sizeof(data));

			data.external_auth.ssid = ind->ssid.array;
			data.external_auth.ssid_len = ind->ssid.length;
			data.external_auth.bssid = (u8 *)&ind->bssid;
			data.external_auth.key_mgmt_suite = ind->akm;

			wpa_supplicant_event_sta(wpa_s, EVENT_EXTERNAL_AUTH, &data);
		}
	}	break;
#endif


	case WPA_CTRL_EVENT_ASSOC_IND: {
#ifdef CONFIG_SME
    	struct sm_assoc_indication *ind = (struct sm_assoc_indication *)msg->argu;

		if (ind) {
			if (ind->status_code == 0) {
				/* success */
				wpa_supplicant_event_sta(wpa_s, EVENT_ASSOC, NULL);
			} else {
				union wpa_event_data data;

				os_memset(&data, 0, sizeof(data));
				data.assoc_reject.status_code = ind->status_code;

				wpa_supplicant_event_sta(wpa_s, EVENT_ASSOC_REJECT, &data);
			}
		} else {
			WPA_LOGE("%s %d: null\n", __func__, __LINE__);
		}
#endif
	}	break;

	case WPA_CTRL_EVENT_CONNECT_IND: {
	    struct sm_connect_ind *ind = (struct sm_connect_ind *)msg->argu;

		if (ind) {
			if (ind->status_code == 0) {
				/* success, Association completed */
				wpa_supplicant_event_sta(wpa_s, EVENT_ASSOC, NULL);
			} else {
				union wpa_event_data data;

				os_memset(&data, 0, sizeof(data));
				data.assoc_reject.status_code = ind->status_code;
				data.assoc_reject.timed_out = 0; /* 0: Unspecified, Drop PMKSA Cache */

				wpa_supplicant_event_sta(wpa_s, EVENT_ASSOC_REJECT, &data);
			}
		}
	}	break;

	case WPA_CTRL_EVENT_MGMT_IND: {
		struct rxu_mgt_ind *ind = (struct rxu_mgt_ind *)msg->argu;

		if (ind) {
			union wpa_event_data data;

			//WPA_LOGI("%s: WPA_CTRL_EVENT_MGMT_IND\n", __func__);

			os_memset(&data, 0, sizeof(data));
			data.rx_mgmt.ssi_signal = ind->rssi;
			data.rx_mgmt.frame = (u8 *)ind->payload;
			data.rx_mgmt.frame_len = ind->length;
			data.rx_mgmt.freq = ind->center_freq;

			//print_hex_dump("MGMT: ", ind->payload, ind->length);
			wpa_supplicant_event_sta(wpa_s, EVENT_RX_MGMT, &data);
		} else {
			WPA_LOGI("%s %d: null\n", __func__, __LINE__);
		}
	}	break;

#if CFG_IEEE80211AX
	case WPA_CTRL_EVENT_RX_MGMT_FRAME:
		break;
#endif

	case WPA_CTRL_EVENT_UNPROT_DEAUTHENTICATE:
		/* fall through */
	case WPA_CTRL_EVENT_UNPROT_DISASSOCIATE: {
		struct pbuf *p = (struct pbuf *)msg->argu;

		mlme_event_unprot_disconnect(wpa_s,
			msg->cmd == WPA_CTRL_EVENT_UNPROT_DEAUTHENTICATE ? EVENT_UNPROT_DEAUTH : EVENT_UNPROT_DISASSOC,
			p->payload, p->len);
		pbuf_free(p);
	}	break;

	default:
		break;
	}

exit:
	if (msg->result)
		*msg->result = res; //FIXME
	if (msg->sema)
		rtos_set_semaphore(&msg->sema);
	if (msg->flags & WPAH_FLAG_FREE)
		os_free((void *)msg->argu);

#undef CHECK_WPA_S

	return 0;
}

static int wpa_ctrl_debug_info_dump_scan(struct wpa_supplicant *wpas)
{
	struct os_reltime now;

	if (!wpas)
		return WPA_ERR_PARAM;

	WPA_LOGI("wpa_state=%d\n", wpas->wpa_state);
	WPA_LOGI("scan_prev_wpa_state=%d\n", wpas->scan_prev_wpa_state);
	struct wpa_radio_work *scan_work = wpas->scan_work;
	if (scan_work) {
		os_get_reltime(&now);
		WPA_LOGI("scan_work: freq=%d type=%s start=%d bands=%d time(%d.%d) now(%d.%d)\n",
			scan_work->freq, scan_work->type, scan_work->started, scan_work->bands,
			scan_work->time.sec, scan_work->time.usec);
	}
	WPA_LOGI("scanning=%d\n", wpas->scanning);
	WPA_LOGI("sched_scanning=%d\n", wpas->sched_scanning);
	WPA_LOGI("sched_scan_stop_req=%d\n", wpas->sched_scan_stop_req);
	WPA_LOGI("disconnected=%d\n", wpas->disconnected);
	WPA_LOGI("scan_req=%d\n", wpas->scan_req);
	WPA_LOGI("last_scan_req=%d\n", wpas->last_scan_req);
	WPA_LOGI("enabled_networks=%d\n", wpa_supplicant_enabled_networks(wpas));
	WPA_LOGI("scan_for_connection=%d\n", wpas->scan_for_connection);

	return WPA_OK;
}

static int wpa_ctrl_debug_info_dump(struct wpa_supplicant *wpas, uint32_t type)
{
	if (type == WPA_CTRL_DEBUG_INFO_DUMP_SCAN)
		wpa_ctrl_debug_info_dump_scan(wpas);

	return WPA_OK;
}

#endif //CFG_WPA_CTRL_IFACE


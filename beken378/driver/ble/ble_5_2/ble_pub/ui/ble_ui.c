#include "string.h"
#include "ble_ui.h"
#include "app_ble.h"
#include "app_sdp.h"

ble_notice_cb_t ble_event_notice = NULL;
extern struct app_env_tag app_ble_env;

void ble_set_notice_cb(ble_notice_cb_t func)
{
	ble_event_notice = func;
}

uint8_t ble_appm_get_dev_name(uint8_t* name, uint32_t buf_len)
{
	if (buf_len >= app_ble_env.dev_name_len) {
		// copy name to provided pointer
		memcpy(name, app_ble_env.dev_name, app_ble_env.dev_name_len);
		// return name length
		return app_ble_env.dev_name_len;
	}

	return 0;
}

uint8_t ble_appm_set_dev_name(uint8_t len, uint8_t* name)
{
	// copy name to provided pointer
	if (len < APP_DEVICE_NAME_MAX_LEN) {
		app_ble_env.dev_name_len = len;
		memcpy(app_ble_env.dev_name, name, len);
		// return name length
		return app_ble_env.dev_name_len;
	}

	return 0;
}

ble_err_t bk_ble_adv_start(uint8_t actv_idx, struct adv_param *adv, ble_cmd_cb_t callback)
{
	uint32_t op_mask;
	ble_err_t ret = ERR_SUCCESS;

	if (app_ble_env_state_get() == APP_BLE_READY) {
		if (actv_idx < BLE_ACTIVITY_MAX) {
			op_mask = (1 << BLE_OP_CREATE_ADV_POS) | (1 << BLE_OP_SET_ADV_DATA_POS)
				| (1 << BLE_OP_SET_RSP_DATA_POS) | (1 << BLE_OP_START_ADV_POS);
			app_ble_run(actv_idx, BLE_INIT_ADV, op_mask, callback);
			memcpy(&(app_ble_env.actvs[actv_idx].param.adv), adv, sizeof(struct adv_param));
			ret = app_ble_create_advertising(actv_idx, adv->channel_map, adv->interval_min, adv->interval_max);
			if (ret != ERR_SUCCESS) {
				app_ble_reset();
			}
		} else {
			bk_printf("Unknow actv idx[%d]\r\n", actv_idx);
			return ERR_UNKNOW_IDX;
		}
	} else {
		bk_printf("ble is not ready\r\n");
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t bk_ble_adv_stop(uint8_t actv_idx, ble_cmd_cb_t callback)
{
	uint32_t op_mask;
	ble_err_t ret = ERR_SUCCESS;

	if (app_ble_env_state_get() == APP_BLE_READY) {
		if (actv_idx < BLE_ACTIVITY_MAX) {
			op_mask = (1 << BLE_OP_STOP_ADV_POS) | (1 << BLE_OP_DEL_ADV_POS);
			app_ble_run(actv_idx, BLE_DEINIT_ADV, op_mask, callback);
			ret = app_ble_stop_advertising(actv_idx);
			if (ret != ERR_SUCCESS) {
				app_ble_reset();
			}
		} else {
			bk_printf("Unknow actv idx[%d]\r\n", actv_idx);
			return ERR_UNKNOW_IDX;
		}
	} else {
		bk_printf("ble is not ready\r\n");
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t bk_ble_scan_start(uint8_t actv_idx, struct scan_param *scan, ble_cmd_cb_t callback)
{
	uint32_t op_mask;
	ble_err_t ret = ERR_SUCCESS;

	if (app_ble_env_state_get() == APP_BLE_READY) {
		if (actv_idx < BLE_ACTIVITY_MAX) {
			op_mask = (1 << BLE_OP_CREATE_SCAN_POS) | (1 << BLE_OP_START_SCAN_POS);
			app_ble_run(actv_idx, BLE_INIT_SCAN, op_mask, callback);
			memcpy(&(app_ble_env.actvs[actv_idx].param.scan), scan, sizeof(struct scan_param));
			ret = app_ble_create_scaning(actv_idx);
			if (ret != ERR_SUCCESS) {
				app_ble_reset();
			}
		} else {
			bk_printf("Unknow actv idx[%d]\r\n", actv_idx);
			return ERR_UNKNOW_IDX;
		}
	} else {
		bk_printf("ble is not ready\r\n");
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t bk_ble_scan_stop(uint8_t actv_idx, ble_cmd_cb_t callback)
{
	uint32_t op_mask;
	ble_err_t ret = ERR_SUCCESS;

	if (app_ble_env_state_get() == APP_BLE_READY) {
		if (actv_idx < BLE_ACTIVITY_MAX) {
			op_mask = (1 << BLE_OP_STOP_SCAN_POS) | (1 << BLE_OP_DEL_SCAN_POS);
			app_ble_run(actv_idx, BLE_DEINIT_SCAN, op_mask, callback);
			ret = app_ble_stop_scaning(actv_idx);
			if (ret != ERR_SUCCESS) {
				app_ble_reset();
			}
		} else {
			bk_printf("Unknow actv idx[%d]\r\n", actv_idx);
			return ERR_UNKNOW_IDX;
		}
	} else {
		bk_printf("ble is not ready\r\n");
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t bk_ble_create_advertising(uint8_t actv_idx,
						unsigned char chnl_map,
						uint32_t intv_min,
						uint32_t intv_max,
						ble_cmd_cb_t callback)
{
	uint32_t op_mask;
	ble_err_t ret = ERR_SUCCESS;

	if (app_ble_env_state_get() == APP_BLE_READY) {
		op_mask = 1 << BLE_OP_CREATE_ADV_POS;
		app_ble_run(actv_idx, BLE_CREATE_ADV, op_mask, callback);
		ret = app_ble_create_advertising(actv_idx, chnl_map, intv_min, intv_max);
		if (ret != ERR_SUCCESS) {
			app_ble_reset();
		}
	} else {
		bk_printf("ble is not ready\r\n");
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t bk_ble_start_advertising(uint8_t actv_idx, uint16 duration, ble_cmd_cb_t callback)
{
	uint32_t op_mask;
	ble_err_t ret = ERR_SUCCESS;

	if (app_ble_env_state_get() == APP_BLE_READY) {
		op_mask = 1 << BLE_OP_START_ADV_POS;
		app_ble_run(actv_idx, BLE_START_ADV, op_mask, callback);
		ret = app_ble_start_advertising(actv_idx, duration);
		if (ret != ERR_SUCCESS) {
			app_ble_reset();
		}
	} else {
		bk_printf("ble is not ready\r\n");
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t bk_ble_stop_advertising(uint8_t actv_idx, ble_cmd_cb_t callback)
{
	uint32_t op_mask;
	ble_err_t ret = ERR_SUCCESS;

	if (app_ble_env_state_get() == APP_BLE_READY) {
		op_mask = 1 << BLE_OP_STOP_ADV_POS;
		app_ble_run(actv_idx, BLE_STOP_ADV, op_mask, callback);
		ret = app_ble_stop_advertising(actv_idx);
		if (ret != ERR_SUCCESS) {
			app_ble_reset();
		}
	} else {
		bk_printf("ble is not ready\r\n");
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t bk_ble_delete_advertising(uint8_t actv_idx, ble_cmd_cb_t callback)
{
	uint32_t op_mask;
	ble_err_t ret = ERR_SUCCESS;

	if (app_ble_env_state_get() == APP_BLE_READY) {
		op_mask = 1 << BLE_OP_DEL_ADV_POS;
		app_ble_run(actv_idx, BLE_DELETE_ADV, op_mask, callback);
		ret = app_ble_delete_advertising(actv_idx);
		if (ret != ERR_SUCCESS) {
			app_ble_reset();
		}
	} else {
		bk_printf("ble is not ready\r\n");
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t bk_ble_set_adv_data(uint8_t actv_idx, unsigned char* adv_buff, unsigned char adv_len, ble_cmd_cb_t callback)
{
	uint32_t op_mask;
	ble_err_t ret = ERR_SUCCESS;

	if(adv_len > ADV_DATA_LEN)
	{
		bk_printf("adv_data_len error:%d\r\n", adv_len);
		return ERR_ADV_DATA;
	}

	if (app_ble_env_state_get() == APP_BLE_READY) {
		op_mask = 1 << BLE_OP_SET_ADV_DATA_POS;
		app_ble_run(actv_idx, BLE_SET_ADV_DATA, op_mask, callback);
		ret = app_ble_set_adv_data(actv_idx, adv_buff, adv_len);
		if (ret != ERR_SUCCESS) {
			app_ble_reset();
		}
	} else {
		bk_printf("ble is not ready\r\n");
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t bk_ble_set_scan_rsp_data(uint8_t actv_idx, unsigned char* scan_buff, unsigned char scan_len, ble_cmd_cb_t callback)
{
	uint32_t op_mask;
	ble_err_t ret = ERR_SUCCESS;

	if (scan_len > ADV_DATA_LEN)
	{
		bk_printf("scan_rsp_len error:%d\r\n", scan_len);
		return ERR_ADV_DATA;
	}

	if (app_ble_env_state_get() == APP_BLE_READY) {
		op_mask = 1 << BLE_OP_SET_RSP_DATA_POS;
		app_ble_run(actv_idx, BLE_SET_RSP_DATA, op_mask, callback);
		ret = app_ble_set_scan_rsp_data(actv_idx, scan_buff, scan_len);
		if (ret != ERR_SUCCESS) {
			app_ble_reset();
		}
	} else {
		bk_printf("ble is not ready\r\n");
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

/////////////////////////////// ble connected API /////////////////////////////////////////////
ble_err_t bk_ble_update_param(uint8_t conn_idx, uint16_t intv_min, uint16_t intv_max,
					uint16_t latency, uint16_t sup_to, ble_cmd_cb_t callback)
{
	uint32_t op_mask;
	ble_err_t ret = ERR_SUCCESS;
	struct gapc_conn_param conn_param;

	if (app_ble_env_state_get() == APP_BLE_READY) {
		op_mask = 1 << BLE_OP_UPDATE_CONN_POS;
		app_ble_run(conn_idx, BLE_CONN_UPDATE_PARAM,op_mask, callback);
		conn_param.intv_min = intv_min;
		conn_param.intv_max = intv_max;
		conn_param.latency = latency;
		conn_param.time_out = sup_to;
		ret = app_ble_update_param(conn_idx, &conn_param);
		if (ret != ERR_SUCCESS) {
			app_ble_reset();
		}
	} else {
		bk_printf("ble is not ready\r\n");
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t bk_ble_disconnect(uint8_t conn_idx, ble_cmd_cb_t callback)
{
	uint32_t op_mask;
	ble_err_t ret = ERR_SUCCESS;

	if (app_ble_env_state_get() == APP_BLE_READY) {
		op_mask = 1 << BLE_OP_DIS_CONN_POS;
		app_ble_run(conn_idx, BLE_CONN_DIS_CONN, op_mask, callback);
		ret = app_ble_disconnect(conn_idx, COMMON_ERROR_REMOTE_USER_TERM_CON);
		if (ret != ERR_SUCCESS) {
			app_ble_reset();
		}
	} else {
		bk_printf("ble is not ready\r\n");
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

/////////////////////////////// ble scan API /////////////////////////////////////////////
ble_err_t bk_ble_create_scaning(uint8_t actv_idx, ble_cmd_cb_t callback)
{
	uint32_t op_mask;
	ble_err_t ret = ERR_SUCCESS;

	if (app_ble_env_state_get() == APP_BLE_READY) {
		op_mask = 1 << BLE_OP_CREATE_SCAN_POS;
		app_ble_run(actv_idx, BLE_CREATE_SCAN, op_mask, callback);
		ret = app_ble_create_scaning(actv_idx);
		if (ret != ERR_SUCCESS) {
			app_ble_reset();
		}
	} else {
		bk_printf("ble is not ready\r\n");
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t bk_ble_start_scaning(uint8_t actv_idx, uint16_t scan_intv, uint16_t scan_wd, ble_cmd_cb_t callback)
{
	uint32_t op_mask;
	ble_err_t ret = ERR_SUCCESS;

	if (app_ble_env_state_get() == APP_BLE_READY) {
		op_mask = 1 << BLE_OP_START_SCAN_POS;
		app_ble_run(actv_idx, BLE_START_SCAN, op_mask, callback);
		ret = app_ble_start_scaning(actv_idx, scan_intv, scan_wd);
		if (ret != ERR_SUCCESS) {
			app_ble_reset();
		}
	} else {
		bk_printf("ble is not ready\r\n");
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t bk_ble_stop_scaning(uint8_t actv_idx, ble_cmd_cb_t callback)
{
	uint32_t op_mask;
	ble_err_t ret = ERR_SUCCESS;

	if (app_ble_env_state_get() == APP_BLE_READY) {
		op_mask = 1 << BLE_OP_STOP_SCAN_POS;
		app_ble_run(actv_idx, BLE_STOP_SCAN, op_mask, callback);
		ret = app_ble_stop_scaning(actv_idx);
		if (ret != ERR_SUCCESS) {
			app_ble_reset();
		}
	} else {
		bk_printf("ble is not ready\r\n");
		ret = ERR_BLE_STATUS;
	}

	return ret;
}

ble_err_t bk_ble_delete_scaning(uint8_t actv_idx, ble_cmd_cb_t callback)
{
	uint32_t op_mask;
	ble_err_t ret = ERR_SUCCESS;

	if (app_ble_env_state_get() == APP_BLE_READY) {
		op_mask = 1 << BLE_OP_DEL_SCAN_POS;
		app_ble_run(actv_idx, BLE_DELETE_SCAN, op_mask, callback);
		ret = app_ble_delete_scaning(actv_idx);
		if (ret != ERR_SUCCESS) {
			app_ble_reset();
		}
	} else {
		bk_printf("ble is not ready\r\n");
		ret = ERR_BLE_STATUS;
	}

	return ret;
}


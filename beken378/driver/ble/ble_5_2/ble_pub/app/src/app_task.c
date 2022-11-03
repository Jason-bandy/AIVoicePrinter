/**
 ****************************************************************************************
 *
 * @file appm_task.c
 *
 * @brief RW APP Task implementation
 *
 * Copyright (C) RivieraWaves 2009-2015
 *
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @addtogroup APPTASK
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"          // SW configuration

#if (BLE_APP_PRESENT)

#include "rwapp_config.h"
#include "app_task.h"             // Application Manager Task API
#include "app_ble.h"                  // Application Manager Definition
#include "gapc_msg.h"            // GAP Controller Task API
#include "gapm_msg.h"            // GAP Manager Task API
#include "architect.h"                 // Platform Definitions
#include <string.h>
#include "common_utils.h"
#include "kernel_timer.h"             // Kernel timer
#include "app_ble.h"
#include "gatt_msg.h"

#ifdef __func__
#undef __func__
#endif
#define __func__   __FUNCTION__

/*
 * LOCAL FUNCTION DEFINITIONS
 ****************************************************************************************
 */

extern struct app_env_tag app_ble_env;

#if 0
static uint8_t app_get_handler(const struct app_subtask_handlers *handler_list_desc,
                               kernel_msg_id_t msgid,
                               void *p_param,
                               kernel_task_id_t src_id)
{
	// Counter
	uint8_t counter;

	// Get the message handler function by parsing the message table
	for (counter = handler_list_desc->msg_cnt; 0 < counter; counter--) {
		struct kernel_msg_handler handler
			= (struct kernel_msg_handler)(*(handler_list_desc->p_msg_handler_tab + counter - 1));

		if ((handler.id == msgid) ||
			(handler.id == KERNEL_MSG_DEFAULT_HANDLER)) {
			// If handler is NULL, message should not have been received in this state
			BLE_ASSERT_ERR(handler.func);

			return (uint8_t)(handler.func(msgid, p_param, TASK_BLE_APP, src_id));
		}
	}

	// If we are here no handler has been found, drop the message
	return (KERNEL_MSG_CONSUMED);
}
#endif

/*
 * MESSAGE HANDLERS
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @brief Handles GAPM_ACTIVITY_CREATED_IND event
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_activity_created_ind_handler(kernel_msg_id_t const msgid,
                                             struct gapm_activity_created_ind const *p_param,
                                             kernel_task_id_t const dest_id,
                                             kernel_task_id_t const src_id)
{
	uint8_t actv_idx = (app_ble_env.app_status >> BLE_APP_IDX_POS);

	if (actv_idx >= BLE_ACTIVITY_MAX) {
		bk_printf("unknow actv idx:%d\r\n", actv_idx);
	} else {
		app_ble_env.actvs[actv_idx].gap_advt_idx = p_param->actv_idx;

		if (p_param->actv_type == GAPM_ACTV_TYPE_ADV) {
			BLE_APP_SET_ACTVS_IDX_STATE(actv_idx, ACTV_ADV_CREATED);
		} else if (p_param->actv_type == GAPM_ACTV_TYPE_SCAN) {
			BLE_APP_SET_ACTVS_IDX_STATE(actv_idx, ACTV_SCAN_CREATED);
		}  else if (p_param->actv_type == GAPM_ACTV_TYPE_PER_SYNC) {
			BLE_APP_SET_ACTVS_IDX_STATE(actv_idx, ACTV_PER_SYNC_CREATED);
		} else {
			bk_printf("unknow actv type:%d\r\n", p_param->actv_type);
		}
	}
	return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles GAPM_ACTIVITY_STOPPED_IND event.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_activity_stopped_ind_handler(kernel_msg_id_t const msgid,
                                             struct gapm_activity_stopped_ind const *p_param,
                                             kernel_task_id_t const dest_id,
                                             kernel_task_id_t const src_id)
{
	uint8_t conidx = KERNEL_IDX_GET(dest_id);
	bk_printf("[%s]conidx:%d\r\n",__func__,conidx);

	uint8_t actv_idx = app_ble_find_actv_idx_handle(p_param->actv_idx);

	if (actv_idx >= BLE_ACTIVITY_MAX) {
		bk_printf("unknow actv idx:%d\r\n", actv_idx);
	} else {
		if (p_param->actv_type == GAPM_ACTV_TYPE_ADV) {
			BLE_APP_SET_ACTVS_IDX_STATE(actv_idx, ACTV_ADV_CREATED);
		} else if (p_param->actv_type == GAPM_ACTV_TYPE_SCAN) {
			BLE_APP_SET_ACTVS_IDX_STATE(actv_idx, ACTV_SCAN_CREATED);
		} else if (p_param->actv_type == GAPM_ACTV_TYPE_PER_SYNC) {
			BLE_APP_SET_ACTVS_IDX_STATE(actv_idx, ACTV_PER_SYNC_CREATED);
		} else {
			bk_printf("unknow actv type:%d\r\n", p_param->actv_type);
		}
	}

	return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles GAPM_PROFILE_ADDED_IND event
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance.
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_profile_added_ind_handler(kernel_msg_id_t const msgid,
                                          struct gapm_profile_added_ind *param,
                                          kernel_task_id_t const dest_id,
                                          kernel_task_id_t const src_id)
{
	// Current State
	kernel_state_t state = kernel_state_get(dest_id);
	uint8_t conidx = KERNEL_IDX_GET(dest_id);
	uint16_t id = param->prf_task_id;
	create_db_t ind;

	bk_printf("[%s] prf_task_id:0x%x,prf_task_nb:%d,start_hdl:%d,state:0x%x\r\n",__func__,param->prf_task_id, param->prf_task_nb,param->start_hdl,state);
	bk_printf("conidx:0x%x,dest_id:0x%x,src_id:0x%x\r\n",conidx,dest_id,src_id);

	#if (BLE_COMM_SERVER)
	if((id >= TASK_BLE_ID_COMMON) && (id <= TASK_BLE_ID_COMMON + BLE_NB_PROFILES))
	{
		id = TASK_BLE_ID_COMMON;
	}
	#endif

	switch (id) {
#if (BLE_COMM_SERVER)
	case TASK_BLE_ID_COMMON:
	{
		kernel_state_set(TASK_BLE_APP, APPM_READY);

		ind.prf_id = param->prf_task_id - TASK_BLE_ID_COMMON;
		ind.status = GAP_ERR_NO_ERROR;

		if (ble_event_notice)
			ble_event_notice(BLE_5_CREATE_DB, &ind);
		break;
	}
#endif

	default:
	{
		break;
	}
	}
	return KERNEL_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles GAP manager command complete events.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_BLE_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_cmp_evt_handler(kernel_msg_id_t const msgid,
                                struct gapm_cmp_evt const *param,
                                kernel_task_id_t const dest_id,
                                kernel_task_id_t const src_id)
{
#if (NVDS_SUPPORT)
	uint8_t key_len = KEY_LEN;
#endif //(NVDS_SUPPORT)
	uint8_t conidx = KERNEL_IDX_GET(dest_id);
	uint8_t actv_idx = app_ble_env.app_status >> BLE_APP_IDX_POS;
	uint8_t status = (param->status == GAP_ERR_NO_ERROR) ? ERR_SUCCESS : ERR_CMD_RUN;

	bk_printf("[%s] conidx:%d,operation:0x%x,status:0x%x\r\n",__func__,conidx,param->operation,param->status);
	switch (param->operation) {
	// Reset completed
	case (GAPM_RESET):
		if (param->status == GAP_ERR_NO_ERROR) {
#if (NVDS_SUPPORT)
			nvds_tag_len_t len = 6;
#endif //(NVDS_SUPPORT)

			// Set Device configuration
			struct gapm_set_dev_config_cmd* cmd = KERNEL_MSG_ALLOC(GAPM_SET_DEV_CONFIG_CMD,
									           TASK_BLE_GAPM, TASK_BLE_APP,
									           gapm_set_dev_config_cmd);
			// Set the operation
			cmd->operation = GAPM_SET_DEV_CONFIG;
			// Set the device role - Peripheral / central
			cmd->role = GAP_ROLE_NONE;
			#if (BLE_PERIPHERAL)
			cmd->role      |= GAP_ROLE_PERIPHERAL;
			#endif
			#if (BLE_CENTRAL)
			cmd->role      |= GAP_ROLE_CENTRAL;
			#endif
			#if (BLE_OBSERVER)
			cmd->role      |= GAP_ROLE_OBSERVER;
			#endif
			#if (BLE_BROADCASTER)
			cmd->role      |= GAP_ROLE_BROADCASTER;
			#endif

			#if (BLE_APP_SEC_CON)
			// The Max MTU is increased to support the Public Key exchange
			// HOWEVER, with secure connections enabled you cannot sniff the
			// LEAP and LEAS protocols
			cmd->pairing_mode = GAPM_PAIRING_SEC_CON | GAPM_PAIRING_LEGACY;
			#else // !(BLE_APP_SEC_CON)
			// Do not support secure connections
			cmd->pairing_mode = GAPM_PAIRING_LEGACY;
			#endif //(BLE_APP_SEC_CON)

			// Set Data length parameters
			cmd->sugg_max_tx_octets = LE_MAX_OCTETS;
			cmd->sugg_max_tx_time   = LE_MAX_TIME;

			//Disable eatt
			SETF(cmd->att_cfg,GAPM_ATT_CLI_DIS_AUTO_EATT,1);
			//Disable auto mtu exchange
			SETF(cmd->att_cfg,GAPM_ATT_CLI_DIS_AUTO_MTU_EXCH,1);

#if (BLE_APP_HID)
			cmd->att_cfg = 0;
			SETF(cmd->att_cfg, GAPM_ATT_SLV_PREF_CON_PAR_EN, 1);
#endif //(BLE_APP_HID)

			// Host privacy enabled by default
			cmd->privacy_cfg = 0;


#if (NVDS_SUPPORT)
			if (rwip_param.get(PARAM_ID_BD_ADDRESS, &len, &cmd->addr.addr[0]) == PARAM_OK) {
				// Check if address is a static random address
				if (cmd->addr.addr[5] & 0xC0) {
					// Host privacy enabled by default
					cmd->privacy_cfg |= GAPM_PRIV_CFG_PRIV_ADDR_BIT;
				}
			} else {
				memcpy(&cmd->addr.addr[0],&co_default_bdaddr.addr[0],BD_ADDR_LEN);
				if (cmd->addr.addr[5] & 0xC0) {
					// Host privacy enabled by default
					cmd->privacy_cfg |= GAPM_PRIV_CFG_PRIV_ADDR_BIT;
				}
			}
#endif //(NVDS_SUPPORT)
			bk_printf("cmd->addr.addr[5] :%x\r\n",cmd->addr.addr[5]);

#if (NVDS_SUPPORT)
			if ((app_sec_get_bond_status()==true) &&
			(nvds_get(NVDS_TAG_LOC_IRK, &key_len, app_ble_env.loc_irk) == NVDS_OK)) {
				memcpy(cmd->irk.key, app_ble_env.loc_irk, 16);
			} else
#endif //(NVDS_SUPPORT)
			{
				memset((void *)&cmd->irk.key[0], 0x00, KEY_LEN);
			}
			// Send message
			kernel_msg_send(cmd);
		} else {
			BLE_ASSERT_ERR(0);
		}
		break;

	case (GAPM_GEN_RAND_NB) :
		bk_printf("gapm_cmp_evt:GAPM_GEN_RAND_NB\r\n");
		if (app_ble_env.rand_cnt == 1) {
			// Generate a second random number
			app_ble_env.rand_cnt++;
			struct gapm_gen_rand_nb_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_GEN_RAND_NB_CMD,
										TASK_BLE_GAPM, TASK_BLE_APP,
										gapm_gen_rand_nb_cmd);
			cmd->operation = GAPM_GEN_RAND_NB;
			kernel_msg_send(cmd);
		} else {
			struct gapm_set_irk_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_SET_IRK_CMD,
									TASK_BLE_GAPM, TASK_BLE_APP,
									gapm_set_irk_cmd);
			app_ble_env.rand_cnt = 0;
			///  - GAPM_SET_IRK
			cmd->operation = GAPM_SET_IRK;
			memcpy(&cmd->irk.key[0], &app_ble_env.loc_irk[0], KEY_LEN);
			kernel_msg_send(cmd);
		}
		break;

	case (GAPM_SET_IRK):
		// BLE_ASSERT_INFO(param->status == GAP_ERR_NO_ERROR, param->operation, param->status);

#if (BLE_APP_SEC)
		// If not Bonded already store the generated value in NVDS
		if (app_sec_get_bond_status()==false) {
#if (NVDS_SUPPORT)
			if (nvds_put(NVDS_TAG_LOC_IRK, KEY_LEN, (uint8_t *)&app_ble_env.loc_irk) != NVDS_OK)
#endif //(NVDS_SUPPORT)
			{
				BLE_ASSERT_INFO(0, 0, 0);
			}
		}
#endif //(BLE_APP_SEC)
		app_ble_env.rand_cnt = 0;

		// Go to the create db state
		kernel_state_set(TASK_BLE_APP, APPM_READY);
		bk_printf("gapm_cmp_evt:BLE_STACK_OK\r\n");
		app_ble_env.app_status = APP_BLE_READY;

		if (ble_event_notice) {
			ble_event_notice(BLE_5_STACK_OK, NULL);
		}
		break;

	// Device Configuration updated
	case (GAPM_SET_DEV_CONFIG):
		BLE_ASSERT_INFO(param->status == GAP_ERR_NO_ERROR, param->operation, param->status);
		bk_printf("gapm_cmp_evt:GAPM_SET_DEV_CONFIG\r\n");

#if (BLE_APP_SEC)
		if (app_sec_get_bond_status()==true) {
#if (NVDS_SUPPORT)
			// If Bonded retrieve the local IRK from NVDS
			if (nvds_get(NVDS_TAG_LOC_IRK, &key_len, app_ble_env.loc_irk) == NVDS_OK) {
				// Set the IRK in the GAP
				struct gapm_set_irk_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_SET_IRK_CMD,
										TASK_BLE_GAPM, TASK_BLE_APP,
										gapm_set_irk_cmd);
				///  - GAPM_SET_IRK:
				cmd->operation = GAPM_SET_IRK;
				memcpy(&cmd->irk.key[0], &app_ble_env.loc_irk[0], KEY_LEN);
				kernel_msg_send(cmd);
				bk_printf("gapm_cmp_evt:wait GAPM_SET_IRK\r\n");
			} else
#endif //(NVDS_SUPPORT)
			{
				BLE_ASSERT_ERR(0);
			}
		} else // Need to start the generation of new IRK
#endif //(BLE_APP_SEC)
		{
			struct gapm_gen_rand_nb_cmd *cmd = KERNEL_MSG_ALLOC(GAPM_GEN_RAND_NB_CMD,
										TASK_BLE_GAPM, TASK_BLE_APP,
										gapm_gen_rand_nb_cmd);
			cmd->operation   = GAPM_GEN_RAND_NB;
			app_ble_env.rand_cnt = 1;
			kernel_msg_send(cmd);
			bk_printf("gapm_cmp_evt:wait GAPM_GEN_RAND_NB\r\n");
		}
		break;

#if (BLE_OBSERVER || BLE_CENTRAL)
	case (GAPM_CREATE_SCAN_ACTIVITY):
		if (actv_idx >= BLE_ACTIVITY_MAX) {
			bk_printf("unknow actv idx:%d\r\n", actv_idx);
		} else {
			app_ble_env.op_mask &= ~(1 << BLE_OP_CREATE_SCAN_POS);
			app_ble_next_operation(actv_idx, status);
		}
		break;
#endif
	case (GAPM_CREATE_ADV_ACTIVITY):
		if (actv_idx >= BLE_ACTIVITY_MAX) {
			bk_printf("unknow actv idx:%d\r\n", actv_idx);
		} else {
			app_ble_env.op_mask &= ~(1 << BLE_OP_CREATE_ADV_POS);
			app_ble_next_operation(actv_idx, status);
		}
		break;
	case (GAPM_SET_ADV_DATA):
		if (actv_idx >= BLE_ACTIVITY_MAX) {
			bk_printf("unknow actv idx:%d\r\n", actv_idx);
		} else {
			app_ble_env.op_mask &= ~(1 << BLE_OP_SET_ADV_DATA_POS);
			app_ble_next_operation(actv_idx, status);
		}
		break;
	case (GAPM_SET_SCAN_RSP_DATA):
		if (actv_idx >= BLE_ACTIVITY_MAX) {
			bk_printf("unknow actv idx:%d\r\n", actv_idx);
		} else {
			app_ble_env.op_mask &= ~(1 << BLE_OP_SET_RSP_DATA_POS);
			app_ble_next_operation(actv_idx, status);
		}
		break;
	case (GAPM_START_ACTIVITY):
		if (actv_idx >= BLE_ACTIVITY_MAX) {
			bk_printf("unknow actv idx:%d\r\n", actv_idx);
		} else {
			if (status == ERR_SUCCESS) {
				if (app_ble_env.op_mask & (1 << BLE_OP_START_SCAN_POS)) {
					app_ble_env.actvs[actv_idx].actv_status = ACTV_SCAN_STARTED;
					app_ble_env.op_mask &= ~(1 << BLE_OP_START_SCAN_POS);
				} else if (app_ble_env.op_mask & (1 << BLE_OP_START_ADV_POS)) {
					app_ble_env.actvs[actv_idx].actv_status = ACTV_ADV_STARTED;
					app_ble_env.op_mask &= ~ (1 << BLE_OP_START_ADV_POS);
				}
			}
			app_ble_next_operation(actv_idx, status);
		}
		break;
	case (GAPM_STOP_ACTIVITY):
		if (actv_idx >= BLE_ACTIVITY_MAX) {
			bk_printf("unknow actv idx:%d\r\n", actv_idx);
		} else {
			app_ble_env.op_mask &= ~((1 << BLE_OP_STOP_SCAN_POS) | (1 << BLE_OP_STOP_ADV_POS));
			app_ble_next_operation(actv_idx, status);
		}
		break;
	case (GAPM_DELETE_ACTIVITY):
		if (actv_idx >= BLE_ACTIVITY_MAX) {
			bk_printf("unknow actv idx:%d\r\n", actv_idx);
		} else {
			app_ble_env.actvs[actv_idx].actv_status = ACTV_IDLE;
			app_ble_env.op_mask &= ~((1 << BLE_OP_DEL_SCAN_POS) | (1 << BLE_OP_DEL_ADV_POS));
			app_ble_next_operation(actv_idx, status);
		}
		break;
	default:
		break;
    }

    return (KERNEL_MSG_CONSUMED);
}

static int gapc_get_dev_info_req_ind_handler(kernel_msg_id_t const msgid,
        struct gapc_get_dev_info_req_ind const *param,
        kernel_task_id_t const dest_id,
        kernel_task_id_t const src_id)
{
    bk_printf("%s,req:0x%x,name_offset:%d,max_name_length:%d,token:%d\r\n",__func__,param->req,param->name_offset,param->max_name_length,param->token);
    switch(param->req)
    {
        case GAPC_DEV_NAME:
        {
            struct gapc_get_dev_info_cfm * cfm = KERNEL_MSG_ALLOC_DYN(GAPC_GET_DEV_INFO_CFM,
                                                    src_id, dest_id,
                                                    gapc_get_dev_info_cfm, APP_DEVICE_NAME_MAX_LEN);
            cfm->req = param->req;
            cfm->info.name.value_length = ble_appm_get_dev_name(cfm->info.name.value, APP_DEVICE_NAME_MAX_LEN);
			cfm->token = param->token;
			cfm->complete_length = cfm->info.name.value_length;
			cfm->status = GAP_ERR_NO_ERROR;
            bk_printf("length:%d,name:%s\r\n",cfm->info.name.value_length,cfm->info.name.value);
            // Send message
            kernel_msg_send(cfm);
        } break;

        case GAPC_DEV_APPEARANCE:
        {
            // Allocate message
            struct gapc_get_dev_info_cfm *cfm = KERNEL_MSG_ALLOC(GAPC_GET_DEV_INFO_CFM,
                                                             src_id, dest_id,
                                                             gapc_get_dev_info_cfm);
            cfm->req = param->req;
			cfm->token = param->token;
			cfm->complete_length = sizeof(cfm->info.appearance);
			cfm->status = GAP_ERR_NO_ERROR;
            // Set the device appearance
            #if (BLE_APP_HT)
            // Generic Thermometer - TODO: Use a flag
            cfm->info.appearance = 728;
            #elif (BLE_APP_HID)
            // HID Mouse
            cfm->info.appearance = 962;
            #else
            // No appearance
            cfm->info.appearance = 0;
            #endif

            // Send message
            kernel_msg_send(cfm);
        } break;

        case GAPC_DEV_SLV_PREF_PARAMS:
        {
            // Allocate message
            struct gapc_get_dev_info_cfm *cfm = KERNEL_MSG_ALLOC(GAPC_GET_DEV_INFO_CFM,
																src_id, dest_id,
																gapc_get_dev_info_cfm);
			cfm->req = param->req;
			cfm->token = param->token;
			cfm->complete_length = sizeof(gap_slv_pref_t);
			cfm->status = GAP_ERR_NO_ERROR;
            // Slave preferred Connection interval Min
            cfm->info.slv_pref_params.con_intv_min = 8;
            // Slave preferred Connection interval Max
            cfm->info.slv_pref_params.con_intv_max = 10;
            // Slave preferred Connection latency
            cfm->info.slv_pref_params.slave_latency  = 0;
            // Slave preferred Link supervision timeout
            cfm->info.slv_pref_params.conn_timeout    = 200;  // 2s (500*10ms)

            // Send message
            kernel_msg_send(cfm);
        } break;

        default: /* Do Nothing */ break;
    }


    return (KERNEL_MSG_CONSUMED);
}
/**
 ****************************************************************************************
 * @brief Handles GAPC_SET_DEV_INFO_REQ_IND message.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_BLE_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_set_dev_info_req_ind_handler(kernel_msg_id_t const msgid,
        struct gapc_set_dev_info_req_ind const *param,
        kernel_task_id_t const dest_id,
        kernel_task_id_t const src_id)
{
    // Set Device configuration
    struct gapc_set_dev_info_cfm* cfm = KERNEL_MSG_ALLOC(GAPC_SET_DEV_INFO_CFM, src_id, dest_id,
                                                     gapc_set_dev_info_cfm);
    // Reject to change parameters
    cfm->status = GAP_ERR_REJECTED;
    cfm->req = param->req;
    // Send message
    kernel_msg_send(cfm);

    return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles connection complete event from the GAP. Enable all required profiles
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_BLE_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_connection_req_ind_handler(kernel_msg_id_t const msgid,
                                           struct gapc_connection_req_ind const *param,
                                           kernel_task_id_t const dest_id,
                                           kernel_task_id_t const src_id)
{
	uint8_t conidx = KERNEL_IDX_GET(src_id);
	uint8_t conn_idx;
	conn_ind_t conn_info;
	bk_printf("[%s]conidx:%d,dest_id:0x%x\r\n",__func__,conidx,dest_id);

	// Check if the received Connection Handle was valid
	if (conidx != GAP_INVALID_CONIDX) {
		if(param->role != APP_BLE_MASTER_ROLE){
			conn_idx = app_ble_get_idle_conn_idx_handle();
			bk_printf("[%s]ble_slave conn_idx:%d\r\n", __FUNCTION__,conn_idx);
		}else{
			conn_idx = KERNEL_IDX_GET(dest_id);
			if(BLE_APP_INITING_CHECK_INDEX(conn_idx)){
				conn_idx = BLE_APP_INITING_GET_INDEX(conn_idx);
			}else{
				conn_idx = BLE_CONNECTION_MAX;
			}
			bk_printf("[%s]ble_master conn_idx:%d\r\n", __FUNCTION__,conn_idx);
		}
		if (BLE_CONNECTION_MAX == conn_idx) {
			bk_printf("%s:Can't get conn idx\r\n", __FUNCTION__);
		} else {
			// Retrieve the connection info from the parameters
			app_ble_env.connections[conn_idx].conhdl = conidx;
			app_ble_env.connections[conn_idx].con_interval = param->con_interval;
			app_ble_env.connections[conn_idx].con_latency = param->con_latency;
			app_ble_env.connections[conn_idx].sup_to = param->sup_to;
			app_ble_env.connections[conn_idx].clk_accuracy = param->clk_accuracy;
			app_ble_env.connections[conn_idx].peer_addr_type = param->peer_addr_type;
			memcpy(app_ble_env.connections[conn_idx].peer_addr.addr,param->peer_addr.addr,BD_ADDR_LEN);
			app_ble_env.connections[conn_idx].role = param->role;
		}

		// Send connection confirmation
		uint8_t index = (param->role == APP_BLE_MASTER_ROLE) ? BLE_APP_INITING_INDEX(conn_idx) : conn_idx;
		struct gapc_connection_cfm *cfm = KERNEL_MSG_ALLOC(GAPC_CONNECTION_CFM,
									KERNEL_BUILD_ID(TASK_BLE_GAPC, conidx),
									 KERNEL_BUILD_ID(TASK_BLE_APP,index),
									gapc_connection_cfm);

#if(BLE_APP_SEC)
		cfm->pairing_lvl      = app_sec_get_bond_status(); // TODO [FBE] restore valid data
#else // !(BLE_APP_SEC)
		cfm->pairing_lvl      = GAP_PAIRING_UNAUTH;
#endif // (BLE_APP_SEC)
		// Send the message
		kernel_msg_send(cfm);

		conn_info.conn_idx = conidx;
		conn_info.peer_addr_type = param->peer_addr_type;
		memcpy(conn_info.peer_addr, param->peer_addr.addr, GAP_BD_ADDR_LEN);

		if (ble_event_notice) {
			ble_event_notice(BLE_5_CONNECT_EVENT, &conn_info);
		}
	}
	return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles connection complete event from the GAP. Enable all required profiles
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_BLE_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_param_update_req_ind_handler(kernel_msg_id_t const msgid,
                                           struct gapc_param_update_req_ind const *param,
                                           kernel_task_id_t const dest_id,
                                           kernel_task_id_t const src_id)
{
	uint8_t conidx = KERNEL_IDX_GET(src_id);

	bk_printf("%s\r\n",__func__);
	// Send connection confirmation
	struct gapc_param_update_cfm *cfm = KERNEL_MSG_ALLOC(GAPC_PARAM_UPDATE_CFM,
								KERNEL_BUILD_ID(TASK_BLE_GAPC, conidx), TASK_BLE_APP,
								gapc_param_update_cfm);

	cfm->accept = true;
	cfm->ce_len_min = 10;
	cfm->ce_len_max = 20;

	// Send message
	kernel_msg_send(cfm);

	return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief  GAPC_PARAM_UPDATED_IND
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_param_updated_ind_handler (kernel_msg_id_t const msgid, 
							const struct gapc_param_updated_ind  *param,
							kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
	uint8_t conidx = KERNEL_IDX_GET(src_id);
	uint8_t conn_idx = app_ble_find_conn_idx_handle(conidx);

	bk_printf("[%s]conn_idx:%d,interval:%d,latency:%d,sup_to:%d\r\n",__func__,conn_idx,param->con_interval,param->con_latency,param->sup_to);
	if (BLE_CONNECTION_MAX == conn_idx) {
		bk_printf("%s:Unknow conntions\r\n", __func__);
	}
	return KERNEL_MSG_CONSUMED;
}

/*******************************************************************************
 * Function: gapc_le_pkt_size_ind_handler
 * Description: GAPC_LE_PKT_SIZE_IND
 * Input: msgid   -Id of the message received.
 *		  param   -Pointer to the parameters of the message.
 *		  dest_id -ID of the receiving task instance
 *		  src_id  -ID of the sending task instance.
 * Return: If the message was consumed or not.
 * Others: void
*******************************************************************************/
static int gapc_le_pkt_size_ind_handler (kernel_msg_id_t const msgid, 
							const struct gapc_le_pkt_size_ind  *param,
							kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
	uint8_t conidx = KERNEL_IDX_GET(src_id);
	bk_printf("%s msgid:0x%x,dest_id:0x%x,src_id:0x%x\r\n",__func__,msgid,dest_id,src_id);
	bk_printf("conidx:%x,",conidx);
	bk_printf("1max_rx_octets = %d\r\n",param->max_rx_octets);
	bk_printf("1max_rx_time = %d\r\n",param->max_rx_time);
	bk_printf("1max_tx_octets = %d\r\n",param->max_tx_octets);
	bk_printf("1max_tx_time = %d\r\n",param->max_tx_time);

	return KERNEL_MSG_CONSUMED;
}

/**
 ****************************************************************************************
 * @brief Handles GAP controller command complete events.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_BLE_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_cmp_evt_handler(kernel_msg_id_t const msgid,
                                struct gapc_cmp_evt const *param,
                                kernel_task_id_t const dest_id,
                                kernel_task_id_t const src_id)
{
	uint8_t conidx = KERNEL_IDX_GET(dest_id);
	uint8_t conn_idx = (app_ble_env.app_status >> BLE_APP_IDX_POS);
	uint8_t status = (param->status == GAP_ERR_NO_ERROR) ? ERR_SUCCESS : ERR_CMD_RUN;

	bk_printf("%s conidx:%d,operation:0x%x,status:%x\r\n", __func__, conidx, param->operation, param->status);

	switch (param->operation) {
	case (GAPC_UPDATE_PARAMS):
		if (conn_idx >= BLE_CONNECTION_MAX) {
			bk_printf("unknow conn idx:%d\r\n", conn_idx);
			if ((app_ble_env_state_get() == APP_BLE_CMD_RUNNING)
				&& (BLE_CONN_UPDATE_PARAM == app_ble_env.cmd)) {
					app_ble_next_operation(conn_idx, status);
				}
		} else {
			if((app_ble_env.app_status &  APP_BLE_CMD_RUNNING )
				&& ((app_ble_env.connections[conn_idx].conn_op_mask & (1 << BLE_OP_UPDATE_CONN_POS))
					|| (app_ble_env.op_mask & (1 << BLE_OP_UPDATE_CONN_POS)))){
				if(app_ble_env.connections[conn_idx].conn_op_mask & (1 << BLE_OP_UPDATE_CONN_POS)) {
					app_ble_env.connections[conn_idx].conn_op_mask &= ~(1 << BLE_OP_UPDATE_CONN_POS);
				}
				if(app_ble_env.op_mask & (1 << BLE_OP_UPDATE_CONN_POS)) {
					app_ble_env.op_mask &= ~(1 << BLE_OP_UPDATE_CONN_POS);
				}
				app_ble_next_operation(conn_idx, status);
			}
		}
		break;

	case (GAPC_DISCONNECT):
		if (conn_idx >= BLE_CONNECTION_MAX) {
			bk_printf("unknow conn idx:%d\r\n", conn_idx);
		} else {
			app_ble_env.connections[conn_idx].conn_op_mask &= ~(1 << BLE_OP_UPDATE_CONN_POS);
			app_ble_env.op_mask &= ~(1 << BLE_OP_UPDATE_CONN_POS);
			app_ble_next_operation(conn_idx, status);
		}
		break;

	default:
		break;
	}

	return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles disconnection complete event from the GAP.
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance (TASK_BLE_GAP).
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapc_disconnect_ind_handler(kernel_msg_id_t const msgid,
                                      struct gapc_disconnect_ind const *param,
                                      kernel_task_id_t const dest_id,
                                      kernel_task_id_t const src_id)
{

	uint8_t conidx = KERNEL_IDX_GET(src_id);
	discon_ind_t dis_info;
	uint8_t conn_idx = app_ble_find_conn_idx_handle(conidx);

	bk_printf("[%s]conn_idx:%d,conhdl:%d,reason:0x%x\r\n",__func__, conn_idx, param->conhdl, param->reason);

	if (BLE_CONNECTION_MAX == conn_idx) {
		bk_printf("%s:Unknow conntions\r\n", __FUNCTION__);
		return (KERNEL_MSG_CONSUMED);
	} else {
		app_ble_env.connections[conn_idx].conhdl = UNKNOW_CONN_HDL;
	}
	dis_info.reason = param->reason;
	dis_info.conn_idx = conn_idx;
	 if (app_ble_env.connections[conn_idx].role == APP_BLE_MASTER_ROLE) {
		if (ble_event_notice) {
			ble_event_notice(BLE_5_INIT_DISCONNECT_EVENT, &dis_info);
		}
	}else{
		if (ble_event_notice) {
			ble_event_notice(BLE_5_DISCONNECT_EVENT, &dis_info);
		}
	}
	return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of all messages sent from the lower layers to the application
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int app_msg_handler(kernel_msg_id_t const msgid,
                            void *param,
                            kernel_task_id_t const dest_id,
                            kernel_task_id_t const src_id)
{
	bk_printf("[%s]msgid:0x%x,dest_id:%d,src_id:%d\r\n",__func__,msgid,dest_id,src_id);
	return (KERNEL_MSG_CONSUMED);
}

/**
 ****************************************************************************************
 * @brief Handles reception of random number generated message
 *
 * @param[in] msgid     Id of the message received.
 * @param[in] param     Pointer to the parameters of the message.
 * @param[in] dest_id   ID of the receiving task instance
 * @param[in] src_id    ID of the sending task instance.
 *
 * @return If the message was consumed or not.
 ****************************************************************************************
 */
static int gapm_gen_rand_nb_ind_handler(kernel_msg_id_t const msgid, struct gapm_gen_rand_nb_ind *param,
                                        kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
    if (app_ble_env.rand_cnt==1)      // First part of IRK
    {
        memcpy(&app_ble_env.loc_irk[0], &param->randnb.nb[0], 8);
    }
    else if (app_ble_env.rand_cnt==2) // Second part of IRK
    {
        memcpy(&app_ble_env.loc_irk[8], &param->randnb.nb[0], 8);
    }

    return KERNEL_MSG_CONSUMED;
}

#if (BLE_OBSERVER || BLE_CENTRAL )
static int gapm_ext_adv_report_ind_handler(kernel_msg_id_t const msgid, struct gapm_ext_adv_report_ind *param,
							kernel_task_id_t const dest_id, kernel_task_id_t const src_id)
{
	recv_adv_t adv_param;

	adv_param.actv_idx = app_ble_find_actv_idx_handle(param->actv_idx);
	adv_param.evt_type = param->info;
	adv_param.data = &(param->data[0]);
	adv_param.data_len = param->length;
	adv_param.rssi = param->rssi;
	adv_param.adv_addr_type = param->trans_addr.addr_type;
	memcpy(adv_param.adv_addr, param->trans_addr.addr, GAP_BD_ADDR_LEN);

	if (ble_event_notice)
		ble_event_notice(BLE_5_REPORT_ADV, &adv_param);

	return KERNEL_MSG_CONSUMED;
}
#endif

/*
 * GLOBAL VARIABLES DEFINITION
 ****************************************************************************************
 */

/* Default State handlers definition. */
KERNEL_MSG_HANDLER_TAB(appm)
{
	// GAPM messages
	{GAPM_CMP_EVT,              (kernel_msg_func_t)gapm_cmp_evt_handler},
	{GAPM_GEN_RAND_NB_IND,      (kernel_msg_func_t)gapm_gen_rand_nb_ind_handler},
	{GAPM_ACTIVITY_CREATED_IND, (kernel_msg_func_t)gapm_activity_created_ind_handler},
	{GAPM_ACTIVITY_STOPPED_IND, (kernel_msg_func_t)gapm_activity_stopped_ind_handler},
#if (BLE_OBSERVER || BLE_CENTRAL )
	{GAPM_EXT_ADV_REPORT_IND,   (kernel_msg_func_t)gapm_ext_adv_report_ind_handler},
#endif

	{GAPM_PROFILE_ADDED_IND,    (kernel_msg_func_t)gapm_profile_added_ind_handler},

	// GAPC messages
	{GAPC_CMP_EVT,              (kernel_msg_func_t)gapc_cmp_evt_handler},
	{GAPC_CONNECTION_REQ_IND,   (kernel_msg_func_t)gapc_connection_req_ind_handler},
	{GAPC_DISCONNECT_IND,       (kernel_msg_func_t)gapc_disconnect_ind_handler},
	{GAPC_GET_DEV_INFO_REQ_IND, (kernel_msg_func_t)gapc_get_dev_info_req_ind_handler},
	{GAPC_SET_DEV_INFO_REQ_IND, (kernel_msg_func_t)gapc_set_dev_info_req_ind_handler},
	{GAPC_PARAM_UPDATE_REQ_IND, (kernel_msg_func_t)gapc_param_update_req_ind_handler},
	{GAPC_PARAM_UPDATED_IND,    (kernel_msg_func_t)gapc_param_updated_ind_handler},
	{GAPC_LE_PKT_SIZE_IND,      (kernel_msg_func_t)gapc_le_pkt_size_ind_handler},

	{KERNEL_MSG_DEFAULT_HANDLER,    (kernel_msg_func_t)app_msg_handler},
};

/* Defines the place holder for the states of all the task instances. */
kernel_state_t appm_state[APP_IDX_MAX];

// Application task descriptor
const struct kernel_task_desc TASK_BLE_DESC_APP = {appm_msg_handler_tab, appm_state, APP_IDX_MAX, ARRAY_LEN(appm_msg_handler_tab)};

#endif //(BLE_APP_PRESENT)

/// @} APPTASK

#include "include.h"
#include "drv_model_pub.h"
#include "intc_pub.h"
#include "str_pub.h"
#include "uart_pub.h"
#include "common.h"

#include "sys_ctrl_pub.h"
#include "sys_ctrl.h"
#include "icu_pub.h"
#include "intc_pub.h"
#include "arm_arch.h"
#include "mailbox_pub.h"
#include "rtos_pub.h"
#include "gpio_pub.h"
#include "bt_pub.h"
#include "bt.h"
#include "wlan_cli_pub.h"
#include "param_config.h"

void mailbox_ble2cpu_process(MAILBOX_TYPE_T type, mailbox_t *mailbox)
{
	if (type != MAILBOX_FROM_BT)
		return;

	switch (mailbox->cmd) {
		case MAILBOX_CMD_BLE_START_ADV_CMP:
			bk_printf("[BLE]MAILBOX_CMD_BLE_START_ADV_CMP\r\n");
			break;

		case MAILBOX_CMD_BLE_STOP_ADV_CMP:
			bk_printf("[BLE]MAILBOX_CMD_BLE_STOP_ADV_CMP\r\n");
			break;

		case MAILBOX_CMD_BLE_READ_CMP:
			bk_printf("[BLE]MAILBOX_CMD_BLE_READ_CMP\r\n");
			break;

		case MAILBOX_CMD_BLE_WRITE_CMP:
			{
				int i;
				uint8_t *tmp = (uint8_t *)mailbox->param1;
				uint32_t len = mailbox->param2;

				bk_printf("[BLE]MAILBOX_CMD_BLE_WRITE_CMP len=%d.\r\n", len);
				for (i = 0; i < len; i++) {
					bk_printf("%02x ", tmp[i]);
				}
				bk_printf("\r\n");
			}
			break;

		case MAILBOX_CMD_BLE_SEND_NTF_CMP:
			bk_printf("[BLE]MAILBOX_CMD_BLE_SEND_NTF_CMP\r\n");
			break;

		case MAILBOX_CMD_BLE_SEND_IND_CMP:
			bk_printf("[BLE]MAILBOX_CMD_BLE_SEND_IND_CMP\r\n");
			break;

		case MAILBOX_CMD_BLE_CONN_CMP:
			bk_printf("[BLE]MAILBOX_CMD_BLE_CONN_CMP\r\n");
			break;

		case MAILBOX_CMD_BLE_DISC_CMP:
			bk_printf("[BLE]MAILBOX_CMD_BLE_DISC_CMP\r\n");
			break;

		case MAILBOX_CMD_BLE_GET_STATUS_CMP:
			bk_printf("[BLE]MAILBOX_CMD_BLE_DISC_CMP %d, %d, %d.\r\n", mailbox->param1, mailbox->param2, mailbox->param3);
			break;

		case MAILBOX_CMD_BLE_GET_RSSI_CMP:
			bk_printf("[BLE]MAILBOX_CMD_BLE_GET_RSSI_CMP\r\n");
			break;

		case MAILBOX_CMD_BLE_SCAN_DECODER:
			bk_printf("[BLE]MAILBOX_CMD_BLE_GET_RSSI_CMP\r\n");
			break;

		default:
			break;
	}
}

static void ble_advertise(void)
{
	uint8_t mac[6];
	char ble_name[20];
	uint8_t adv_idx, adv_name_len;
	uint8_t adv_rsp_data[64];
	uint32_t adv_len, scan_rsp_len;

	wifi_get_mac_address((char *)mac, CONFIG_ROLE_STA);
	adv_name_len = snprintf(ble_name, sizeof(ble_name), "bk72xx-%02x%02x", mac[4], mac[5]);

	memset(adv_rsp_data, 0, sizeof(adv_rsp_data));

	adv_idx = 0;
	adv_rsp_data[adv_idx] = 0x02;
	adv_idx++;
	adv_rsp_data[adv_idx] = 0x01;
	adv_idx++;
	adv_rsp_data[adv_idx] = 0x06;
	adv_idx++;

	adv_rsp_data[adv_idx] = adv_name_len + 1;
	adv_idx++;
	adv_rsp_data[adv_idx] = 0x09;
	adv_idx++; //name
	memcpy(&adv_rsp_data[adv_idx], ble_name, adv_name_len);
	adv_idx += adv_name_len;

	adv_len = adv_idx;

	adv_rsp_data[adv_idx] = adv_name_len + 1;
	adv_idx++;
	adv_rsp_data[adv_idx] = 0x08;
	adv_idx++; //name
	memcpy(&adv_rsp_data[adv_idx], ble_name, adv_name_len);
	adv_idx += adv_name_len;
	scan_rsp_len = adv_idx - adv_len;

	ble_host_to_controller(MAILBOX_CMD_BLE_START_ADV, adv_rsp_data, adv_len+scan_rsp_len, adv_len);
}

void ble_test_command(char *pcWriteBuffer, int xWriteBufferLen, int argc, char **argv)
{
	mailbox_t mbx;

	if (os_strcmp(argv[1], "start_adv") == 0) {
		ble_advertise();
	} else if (os_strcmp(argv[1], "stop_adv") == 0) {
		mailbox_set_param(&mbx, MAILBOX_CMD_BLE_STOP_ADV, 0, 0, 0);
		mailbox_ctrl(CMD_MAILBOX_CPU2BT_SEND, &mbx);
	} else if (os_strcmp(argv[1], "notify") == 0) {
		uint8_t len;
		uint16_t prf_id;
		uint16_t att_id;
		uint8_t write_buffer[20];
		uint32_t param;

		if(argc != 5) {
			bk_printf("ble_test notify error.\r\n");
			return;
		}

		len = os_strlen(argv[4]);
		if(len % 2 != 0) {
			os_printf("ble_test notify len error.\r\n");
			return;
		}
		hexstr2bin(argv[4], write_buffer, len / 2);

		prf_id = atoi(argv[2]);
		att_id = atoi(argv[3]);
		param = ((prf_id&0xFFFF) << 16) | (att_id&0xFFFF);

		ble_host_to_controller(MAILBOX_CMD_BLE_SEND_NTF, write_buffer, len / 2, param);
	} else if (os_strcmp(argv[1], "indicate") == 0) {
		uint8_t len;
		uint16_t prf_id;
		uint16_t att_id;
		uint8_t write_buffer[20];
		uint32_t param;

		if(argc != 5) {
			bk_printf("ble_test indicate error.\r\n");
			return;
		}

		len = os_strlen(argv[4]);
		if(len % 2 != 0) {
			os_printf("ble_test indicate len error.\r\n");
			return;
		}
		hexstr2bin(argv[4], write_buffer, len / 2);

		prf_id = atoi(argv[2]);
		att_id = atoi(argv[3]);
		param = ((prf_id&0xFFFF) << 16) | (att_id&0xFFFF);

		ble_host_to_controller(MAILBOX_CMD_BLE_SEND_IND, write_buffer, len / 2, param);
	} else if (os_strcmp(argv[1], "disc") == 0) {
		mailbox_set_param(&mbx, MAILBOX_CMD_BLE_DISC, 0, 0, 0);
		mailbox_ctrl(CMD_MAILBOX_CPU2BT_SEND, &mbx);
	} else if (os_strcmp(argv[1], "status") == 0) {
		mailbox_set_param(&mbx, MAILBOX_CMD_BLE_GET_STATUS, 0, 0, 0);
		mailbox_ctrl(CMD_MAILBOX_CPU2BT_SEND, &mbx);
	} else if (os_strcmp(argv[1], "rssi") == 0) {
		os_printf("NOT SUPPORT!\r\n");
	} else if (os_strcmp(argv[1], "scan_start") == 0) {
		os_printf("NOT SUPPORT!\r\n");
	} else if (os_strcmp(argv[1], "scan_stop") == 0) {
		os_printf("NOT SUPPORT!\r\n");
	}
}

const struct cli_command ble_clis[] = {
	{"ble_test", "ble test", ble_test_command},
};

void bk7271_ble_cli_init(void)
{
	int ret;

	mailbox_ctrl(CMD_MAILBOX_SET_CALLBACK, (void *)mailbox_ble2cpu_process);
	ret = cli_register_commands(ble_clis, sizeof(ble_clis) / sizeof(struct cli_command));
	if (ret)
		os_printf("register ble commands fail.\r\n");
}

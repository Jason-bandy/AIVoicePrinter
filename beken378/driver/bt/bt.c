#include "include.h"
#include "drv_model_pub.h"
#include "intc_pub.h"
#include "uart_pub.h"

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
#include "port.h"
#include "beken_external.h"
#include "param_config.h"

beken_thread_t bt_thread_handle = NULL;
uint32_t bt_stack_size = 51200; //50k
extern btaddr_t btaddr_def;
extern char bt_unit_name[32];

static void bt_start(void)
{
	uint32_t i = 0;
	uint32_t length;
	uint32_t dst_code_addr = BT_IMEM_BASE_ADDR;
	uint32_t reg;

	length = *((volatile unsigned long *)(CFG_BT_SRC_ADD + 52));

	reg = REG_READ(SCTRL_CONTROL);
	reg &= ~(MTB_PRIVILEGE_MASK << MTB_PRIVILEGE_POSI);
	reg |= (MTB_PRIVILEGE_MASK << MTB_PRIVILEGE_POSI);
	REG_WRITE(SCTRL_CONTROL, reg);

	*((volatile unsigned long *) PMU_CO_MCU_CONFIG) |= 0x2A;
	BT_PRT("BT code start from 0x%08X with length %d\r\n", CFG_BT_SRC_ADD, length);

	while (i < length) {
		*((volatile unsigned long *)(dst_code_addr + i)) =
			*((volatile unsigned long *)(CFG_BT_SRC_ADD + i));
		i += 4;
	}

	BT_PRT("BT code ready and start boot\r\n");
	*((volatile unsigned long *)PMU_CO_MCU_CONFIG) &= ~(0x1 << 1);
}

static void bt_main(void *arg)
{
	ceva_main(NULL);

	bt_thread_handle = NULL;
	rtos_delete_thread(NULL);
}

static void bt_thread_start(void)
{
	OSStatus ret;

	ret = rtos_create_thread(&bt_thread_handle,
							8,
							"bt",
							(beken_thread_function_t)bt_main,
							(unsigned short)bt_stack_size,
							(beken_thread_arg_t)0);
	ASSERT(0 == ret);
}

void bt_activate(char *bt_name)
{
	uint8_t *bt_mac = &btaddr_def.b[0];
	uint8_t ble_mac[6];

	BT_PRT("BT Activate\r\n");

	/*set bt address*/
	wifi_get_mac_address((char *)bt_mac, CONFIG_ROLE_STA);
	bt_mac[5] += 1; // add 1, diff from wifi's mac

	wifi_get_mac_address((char *)ble_mac, CONFIG_ROLE_STA);
	ble_mac[5] += 2;
	ble_host_to_controller(MAILBOX_CMD_BLE_SET_ADDR, ble_mac, 6, 0);

	/*set bt name*/
	if (bt_name) {
		os_strncpy(bt_unit_name, bt_name, sizeof(bt_unit_name));
	} else {
		/*use default bt name BK7271_BT-XXXXXXXXXXXX*/
		os_memset(bt_unit_name, 0, sizeof(bt_unit_name));
	}

	bt_thread_start();
}

void mailbox_bt2cpu_process(MAILBOX_TYPE_T type, mailbox_t *mailbox)
{
	if (type != MAILBOX_FROM_BT)
		return;

	switch (mailbox->cmd) {
		case MAILBOX_CMD_HCI_SEND:
			{
				uint8_t *buf = (uint8_t *)mailbox->param1;
				uint32_t len = mailbox->param2;
				juart_receive(buf, len);
			}
			break;

		case MAILBOX_CMD_REG_READ:
			*((uint32_t*)(HOST2CTRL_BUFFER_ADDR + HOST2CTRL_BUFFER_SIZE - 4)) =
				*(uint32_t*)mailbox->param1;
			break;

		case MAILBOX_CMD_REG_WRITE:
			*(uint32_t*)mailbox->param1 = mailbox->param2;
			break;

		default:
			break;
	}
}

void bt_host_to_controller(uint8_t *buff,uint32_t len)
{
	int i;
	mailbox_t mbx;
	uint8_t *src = (uint8_t *)buff;
	uint8_t *dst = (uint8_t *)HOST2CTRL_BUFFER_ADDR;

	for (i = 0; i < len; i++) {
		*dst++ = *src++;
	}

	mailbox_set_param(&mbx, MAILBOX_CMD_HCI_SEND, HOST2CTRL_BUFFER_ADDR, len, 0);
	mailbox_ctrl(CMD_MAILBOX_CPU2BT_SEND, &mbx);
}

void bt_init(void)
{
	uint32_t ret;
	uint32_t param;

	if (get_ate_mode_state()) {
		return;
	}

	param = (BT_ENABLE_1V | BT_ENaBLE_LDO_1V);
	ret = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_ANALOG_CTRL4_SET, &param);
	if (ret) {
		os_printf("set analog4 bt fail.\r\n");
		return;
	}

	param = (ANALOG_LDO_ENABLE | (0x3 & VBG_SEL_HIGH_MASK) << VBG_SEL_HIGH_POSI);
	ret = sddev_control(SCTRL_DEV_NAME, CMD_SCTRL_ANALOG_CTRL9_REAL_SET, &param);
	if (ret) {
		os_printf("set analog9 bt fail.\r\n");
		return;
	}

	mailbox_ctrl(CMD_MAILBOX_SET_CALLBACK, (void *)mailbox_bt2cpu_process);
	REG_WRITE(0x00A10004, 0x00011);
	bt_start();
}

void bt_exit(void)
{
}

void ble_host_to_controller(uint32_t cmd, uint8_t *buff,uint32_t len, uint32_t param)
{
	int i;
	mailbox_t mbx;
	uint8_t *src = (uint8_t *)buff;
	uint8_t *dst = (uint8_t *)HOST2CTRL_BUFFER_ADDR;

	for (i = 0; i < len; i++) {
		*dst++ = *src++;
	}

	mailbox_set_param(&mbx, cmd, HOST2CTRL_BUFFER_ADDR, len, param);
	mailbox_ctrl(CMD_MAILBOX_CPU2BT_SEND, &mbx);
}


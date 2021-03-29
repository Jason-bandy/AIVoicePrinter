#include "include.h"
#include "arm_arch.h"
#include "sys_config.h"
#include "mailbox.h"
#include "mailbox_pub.h"
#include <string.h>
#include "uart_pub.h"
#include "drv_model_pub.h"
#include "icu_pub.h"
#include "app.h"
#include "rtos_pub.h"
#include "uart.h"
#include "dsp_pub.h"

#if (CFG_SOC_NAME == SOC_BK7271)

static const DD_OPERATIONS mailbox_op = {
	mailbox_open,
	mailbox_close,
	NULL,
	NULL,
	mailbox_ctrl
};

static mailbox_isr_callback mailbox_cb[MAILBOX_CALLBACK_COUNT] = {NULL};

void mailbox_init(void)
{
	UINT32 param;

	intc_service_register(IRQ_MAILBOX_DSP, PRI_IRQ_MAILBOX, mailbox_dsp2cpu_isr);
	param = IRQ_MAILBOX_DSP_BIT;
	sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);

	intc_service_register(IRQ_MAILBOX_BT, PRI_IRQ_MAILBOX1, mailbox_bt2cpu_isr);
	param = IRQ_MAILBOX_BT_BIT;
	sddev_control(ICU_DEV_NAME, CMD_ICU_INT_ENABLE, &param);

	ddev_register_dev(MAILBOX_DEV_NAME, (DD_OPERATIONS*)&mailbox_op);
}

void mailbox_exit(void)
{
	ddev_unregister_dev(MAILBOX_DEV_NAME);
}

UINT32 mailbox_open(UINT32 op_flag)
{
	return MAILBOX_SUCCESS;
}

UINT32 mailbox_close(void)
{
	return MAILBOX_SUCCESS;
}

static UINT32 mailbox_cpu2dsp_send(mailbox_t *param)
{
	UINT32 ret = MAILBOX_SUCCESS;
	UINT32 reg;
	UINT32 ready;

	if (!dsp_is_inited()) {
		os_printf("dsp not initialized\r\n");
		return kInProgressErr;
	}

	ready = REG_READ(MAILBOX_CPU2DSP_READY);
	if (!(ready & MAILBOX_READY_BOX0)) {
		reg = param->cmd;
		REG_WRITE(MAILBOX_CPU2DSP_BOX0_CMD, reg);
		reg = param->param1;
		REG_WRITE(MAILBOX_CPU2DSP_BOX0_PARAM1, reg);
		reg = param->param2;
		REG_WRITE(MAILBOX_CPU2DSP_BOX0_PARAM2, reg);
		reg = param->param3;
		REG_WRITE(MAILBOX_CPU2DSP_BOX0_PARAM3, reg);

		ready &= MAILBOX_READY_MASK;
		ready |= MAILBOX_READY_BOX0;
		REG_WRITE(MAILBOX_CPU2DSP_READY, ready);

		if (param->cmd & MAILBOX_CMD_FAST_RSP_FLAG) {
			while (REG_READ(MAILBOX_CPU2DSP_READY) & MAILBOX_READY_BOX0);
		}
	} else if (!(ready & MAILBOX_READY_BOX1)) {
		reg = param->cmd;
		REG_WRITE(MAILBOX_CPU2DSP_BOX1_CMD, reg);
		reg = param->param1;
		REG_WRITE(MAILBOX_CPU2DSP_BOX1_PARAM1, reg);
		reg = param->param2;
		REG_WRITE(MAILBOX_CPU2DSP_BOX1_PARAM2, reg);
		reg = param->param3;
		REG_WRITE(MAILBOX_CPU2DSP_BOX1_PARAM3, reg);
		ready &= MAILBOX_READY_MASK;
		ready |= MAILBOX_READY_BOX1;
		REG_WRITE(MAILBOX_CPU2DSP_READY, ready);

		if (param->cmd & MAILBOX_CMD_FAST_RSP_FLAG) {
			while (REG_READ(MAILBOX_CPU2DSP_READY) & MAILBOX_READY_BOX1);
		}
	} else {
		bk_printf("cpu2dsp_send(0x%x) failed\r\n", param->cmd);
		ret = MAILBOX_NOT_READY;
	}

	return ret;
}

static UINT32 mailbox_cpu2bt_send(mailbox_t *param)
{
	UINT32 ret = MAILBOX_SUCCESS;
	UINT32 reg;
	UINT32 ready;

	ready = REG_READ(MAILBOX_CPU2BT_READY);

	if (!(ready & MAILBOX_READY_BOX0)) {
		reg = param->cmd;
		REG_WRITE(MAILBOX_CPU2BT_BOX0_CMD, reg);
		reg = param->param1;
		REG_WRITE(MAILBOX_CPU2BT_BOX0_PARAM1, reg);
		reg = param->param2;
		REG_WRITE(MAILBOX_CPU2BT_BOX0_PARAM2, reg);
		reg = param->param3;
		REG_WRITE(MAILBOX_CPU2BT_BOX0_PARAM3, reg);
		ready &= MAILBOX_READY_MASK;
		ready |= MAILBOX_READY_BOX0;
		REG_WRITE(MAILBOX_CPU2BT_READY, ready);
		while (REG_READ(MAILBOX_CPU2BT_READY) & ready);
	} else if (!(ready & MAILBOX_READY_BOX1)) {
		reg = param->cmd;
		REG_WRITE(MAILBOX_CPU2BT_BOX1_CMD, reg);
		reg = param->param1;
		REG_WRITE(MAILBOX_CPU2BT_BOX1_PARAM1, reg);
		reg = param->param2;
		REG_WRITE(MAILBOX_CPU2BT_BOX1_PARAM2, reg);
		reg = param->param3;
		REG_WRITE(MAILBOX_CPU2BT_BOX1_PARAM3, reg);
		ready &= MAILBOX_READY_MASK;
		ready |= MAILBOX_READY_BOX1;
		REG_WRITE(MAILBOX_CPU2BT_READY, ready);
		while (REG_READ(MAILBOX_CPU2BT_READY) & ready);
	} else {
		bk_printf("cpu2bt_send(0x%x) failed\r\n", param->cmd);
		ret = MAILBOX_NOT_READY;
	}

	return ret;
}

static UINT32 mailbox_clear_callback(mailbox_isr_callback callback)
{
	UINT32 index;

	for (index = 0; index < MAILBOX_CALLBACK_COUNT; index++) {
		if (callback == mailbox_cb[index]) {
			mailbox_cb[index] = NULL;
			return MAILBOX_SUCCESS;
		}
	}

	os_printf("%s 0x%x not found\n", __FUNCTION__, callback);

	return MAILBOX_NOT_READY;
}

static UINT32 mailbox_set_callback(mailbox_isr_callback callback)
{
	UINT32 index;

	for (index = 0; index < MAILBOX_CALLBACK_COUNT; index++) {
		if ((NULL == mailbox_cb[index]) || (callback == mailbox_cb[index])) {
			mailbox_cb[index] = callback;
			return MAILBOX_SUCCESS;
		}
	}

	os_printf("%s mailbox_cb is full\n", __FUNCTION__);

	return MAILBOX_NOT_READY;
}

UINT32 mailbox_ctrl(UINT32 cmd, void *param)
{
	UINT32 ret;

	switch (cmd) {
	case CMD_MAILBOX_CPU2DSP_SEND:
		ret = mailbox_cpu2dsp_send((mailbox_t *)param);
		break;

	case CMD_MAILBOX_CPU2BT_SEND:
		ret = mailbox_cpu2bt_send((mailbox_t *)param);
		break;

	case CMD_MAILBOX_SET_CALLBACK:
		ret = mailbox_set_callback((mailbox_isr_callback)param);
		break;

	case CMD_MAILBOX_CLEAR_CALLBACK:
		ret = mailbox_clear_callback((mailbox_isr_callback)param);
		break;

	default:
		ret = MAILBOX_UNKNOW_CMD;
		break;
	}

	return ret;


}

static void mailbox_dsp2cpu_isr(void)
{
	UINT32 ready;
	UINT32 clear;
	UINT32 index;
	mailbox_t mailbox;

	ready = REG_READ(MAILBOX_DSP2CPU_READY);
	clear = REG_READ(MAILBOX_DSP2CPU_CLEAR);

	os_null_printf("ready:%x, clear:%x\r\n", ready, clear);

	if (ready & MAILBOX_READY_BOX0) {
		mailbox.cmd = REG_READ(MAILBOX_DSP2CPU_BOX0_CMD);
		mailbox.param1 = REG_READ(MAILBOX_DSP2CPU_BOX0_PARAM1);
		mailbox.param2 = REG_READ(MAILBOX_DSP2CPU_BOX0_PARAM2);
		mailbox.param3 = REG_READ(MAILBOX_DSP2CPU_BOX0_PARAM3);

		clear &= MAILBOX_CLEAR_MASK;
		clear |= MAILBOX_CLEAR_BOX0;
		REG_WRITE(MAILBOX_DSP2CPU_CLEAR, clear);
	} else if (ready & MAILBOX_READY_BOX1) {
		mailbox.cmd = REG_READ(MAILBOX_DSP2CPU_BOX1_CMD);
		mailbox.param1 = REG_READ(MAILBOX_DSP2CPU_BOX1_PARAM1);
		mailbox.param2 = REG_READ(MAILBOX_DSP2CPU_BOX1_PARAM2);
		mailbox.param3 = REG_READ(MAILBOX_DSP2CPU_BOX1_PARAM3);

		clear &= MAILBOX_CLEAR_MASK;
		clear |= MAILBOX_CLEAR_BOX1;
		REG_WRITE(MAILBOX_DSP2CPU_CLEAR, clear);
	}

	os_null_printf("cmd:%x, param1:%x, param2:%x, param3:%x\r\n", mailbox.cmd, mailbox.param1, mailbox.param2, mailbox.param3);
	os_null_printf("[WIFI_R]: (%08x, 0x%08x, %d, %d)\r\n", mailbox.cmd, mailbox.param1, mailbox.param2, mailbox.param3);

	for (index = 0; index < MAILBOX_CALLBACK_COUNT; index++) {
		if (NULL == mailbox_cb[index])
			continue;
		(mailbox_cb[index])(MAILBOX_FROM_DSP, &mailbox);
	}
}

static void mailbox_bt2cpu_isr(void)
{
	UINT32 ready;
	UINT32 clear;
	UINT32 index;
	mailbox_t mailbox;

	ready = REG_READ(MAILBOX_BT2CPU_READY);
	clear = REG_READ(MAILBOX_BT2CPU_CLEAR);

	if (ready & MAILBOX_READY_BOX0) {
		mailbox.cmd = REG_READ(MAILBOX_BT2CPU_BOX0_CMD);
		mailbox.param1 = REG_READ(MAILBOX_BT2CPU_BOX0_PARAM1);
		mailbox.param2 = REG_READ(MAILBOX_BT2CPU_BOX0_PARAM2);
		mailbox.param3 = REG_READ(MAILBOX_BT2CPU_BOX0_PARAM3);
	} else if (ready & MAILBOX_READY_BOX1) {
		mailbox.cmd = REG_READ(MAILBOX_BT2CPU_BOX1_CMD);
		mailbox.param1 = REG_READ(MAILBOX_BT2CPU_BOX1_PARAM1);
		mailbox.param2 = REG_READ(MAILBOX_BT2CPU_BOX1_PARAM2);
		mailbox.param3 = REG_READ(MAILBOX_BT2CPU_BOX1_PARAM3);
	}

	for (index = 0; index < MAILBOX_CALLBACK_COUNT; index++) {
		if (NULL == mailbox_cb[index])
			continue;
		(mailbox_cb[index])(MAILBOX_FROM_BT, &mailbox);
	}

	if (ready & MAILBOX_READY_BOX0) {
		clear &= MAILBOX_CLEAR_MASK;
		clear |= MAILBOX_CLEAR_BOX0;
		REG_WRITE(MAILBOX_BT2CPU_CLEAR, clear);
	} else if (ready & MAILBOX_READY_BOX1) {
		clear &= MAILBOX_CLEAR_MASK;
		clear |= MAILBOX_CLEAR_BOX1;
		REG_WRITE(MAILBOX_BT2CPU_CLEAR, clear);
	}
}
#endif

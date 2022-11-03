#include "ble_rf_port.h"
#include "rwip_config.h"   // RW SW configuration
#include "rwip.h"
#include "rwip_int.h"
#include "architect.h"      // architectural platform definitions
#include <stdlib.h>    // standard lib functions
#include <stddef.h>    // standard definitions
#include <stdint.h>    // standard integer definition
#include <stdbool.h>   // boolean definition
#include <string.h>    // boolean definition
#include "rwip.h"      // RW SW initialization
#include "prf.h"       // RW SW initialization
#include "rwble.h"
#include "uart_pub.h"
#include "rtos_pub.h"
#include "ble.h"
#include "ble_pub.h"
#include "ble_api_5_x.h"
#include "sys_ctrl_pub.h"
#include "icu_pub.h"
#include "intc_pub.h"
#include "drv_model_pub.h"
#include "include.h"
#include "drv_model_pub.h"
#include "intc_pub.h"
#include "uart_pub.h"
#include "app_task.h"
#include "udebug.h"
#include "typedef.h"
#include "common_bt.h"
#include "arm_arch.h"
#include "power_save_pub.h"
#include "gapc_msg.h" 
#include "bk7011_cal_pub.h"

uint8_t ble_switch_mac_sleeped;
UINT32 rf_wifi_used = 0;

void rf_wifi_used_set(void)
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	if (0 == rf_wifi_used) {
		rf_wifi_used = 1;
	}
	GLOBAL_INT_RESTORE();
}

void rf_wifi_used_clr(void)
{
	GLOBAL_INT_DECLARATION();
	GLOBAL_INT_DISABLE();
	if (1 == rf_wifi_used) {
		rf_wifi_used = 0;
	}
	GLOBAL_INT_RESTORE();
}

uint8_t if_ble_sleep(void)
{
	uint8_t ret;
	extern struct rwip_env_tag rwip_env;
	GLOBAL_INT_DIS();
	if (ble_ps_enabled()) {
		ret = (rwip_env.prevent_sleep & RW_DEEP_SLEEP);
	} else {
		ret = 0;
	}
	GLOBAL_INT_RES();

	return ret;
}

void ble_switch_rf_to_wifi(void)
{
}

void ble_request_rf_by_isr(void)
{
}

void ble_release_rf_by_isr(void)
{
}

void ble_delegate_ps_restore_mac_flag_clear(void)
{
}

void ble_stop_delegate_restore_mac_state(int flag)
{
}

void ps_recover_ble_switch_mac_status(void)
{
}


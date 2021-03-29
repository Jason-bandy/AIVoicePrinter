#include "include.h"
#include "func_pub.h"
#include "intc.h"
#include "rwnx.h"
#include "uart_pub.h"
#include "lwip_intf.h"
#include "param_config.h"
#include "saradc_pub.h"
#include "sys_ctrl_pub.h"
#include "drv_model_pub.h"
#include "ate_app.h"
#include "BkDriverWdg.h"
#include "sys_config.h"

#if CFG_ROLE_LAUNCH
#include "role_launch.h"
#endif

#if CFG_SUPPORT_CALIBRATION
#include "bk7011_cal_pub.h"
#endif

#if CFG_UART_DEBUG
#include "uart_debug_pub.h"
#endif

#if CFG_SDIO
#include "sdio_intf_pub.h"
#endif

#if CFG_USB
#include "fusb_pub.h"
#endif

#include "start_type_pub.h"

#if CFG_ENABLE_BUTTON
#include "key/key_main.h"
#endif

#if CFG_EASY_FLASH && (!CFG_SUPPORT_RTT)
#include "easyflash.h"
#include "bk_ef.h"
#endif

#if (!CFG_SUPPORT_RTT)
#include "BkDriverFlash.h"
#endif

#if ((CFG_SOC_NAME == SOC_BK7271) && CFG_USE_BT)
#include "bt_pub.h"
#endif
#include "irda_pub.h"
#include "bk_log.h"

#define TAG "func"

extern void rwnx_cal_initial_calibration(void);

UINT32 func_init_extended(void)
{
	char temp_mac[6];
#if (CFG_SOC_NAME != SOC_BK7236)
	UINT32 reg;
#endif

	cfg_param_init();
	// load mac, init mac first
	wifi_get_mac_address(temp_mac, CONFIG_ROLE_NULL);

#if (CFG_SOC_NAME == SOC_BK7231N) || (CFG_SOC_NAME == SOC_BK7236)
	manual_cal_load_bandgap_calm();
#endif

	BK_LOGI(TAG, "rwnxl init\r\n");
	rwnxl_init();

#if CFG_UART_DEBUG
	#ifndef KEIL_SIMULATOR
	BK_LOGI(TAG, "uart debug init\r\n");
	uart_debug_init();
	#endif
#endif

#if (!CFG_SUPPORT_RTT)
	BK_LOGI(TAG, "intc init\r\n");
	intc_init();
#endif

#if (CFG_SOC_NAME == SOC_BK7271)
	BK_LOGI(TAG, "trng enable\r\n");
	reg = 1;
	sddev_control(IRDA_DEV_NAME, TRNG_CMD_ENABLE, &reg);
#endif

#if CFG_SUPPORT_CALIBRATION
	UINT32 is_tab_inflash = 0;
	BK_LOGI(TAG, "calibration init\r\n");
	calibration_main();
	#if CFG_SUPPORT_MANUAL_CALI
	is_tab_inflash = manual_cal_load_txpwr_tab_flash();
	manual_cal_load_default_txpwr_tab(is_tab_inflash);
	#endif
	#if CFG_SARADC_CALIBRATE
	manual_cal_load_adc_cali_flash();
	#endif
	#if CFG_USE_TEMPERATURE_DETECT
	manual_cal_load_temp_tag_flash();
	#endif

	#if (CFG_SOC_NAME != SOC_BK7231)
	manual_cal_load_lpf_iq_tag_flash();
	manual_cal_load_xtal_tag_flash();
	#endif // (CFG_SOC_NAME != SOC_BK7231)

	rwnx_cal_initial_calibration();

	#if CFG_SUPPORT_MANUAL_CALI
	if (0) //(is_tab_inflash == 0)
	{
		manual_cal_fitting_txpwr_tab();
		manual_cal_save_chipinfo_tab_to_flash();
		manual_cal_save_txpwr_tab_to_flash();
	}
	#endif // CFG_SUPPORT_MANUAL_CALI
#endif

#if CFG_SDIO
	BK_LOGI(TAG, "sdio intf init\r\n");
	sdio_intf_init();
#endif

#if CFG_SDIO_TRANS
	BK_LOGI(TAG, "sdio trans init\r\n");
	sdio_trans_init();
#endif


#if CFG_USB
	BK_LOGI(TAG, "fusb init\r\n");
	if (!get_ate_mode_state()) {
		fusb_init();
	}
#endif

#if CFG_USE_STA_PS
	BK_LOGI(TAG, "ps init\r\n");
#endif

#if CFG_ROLE_LAUNCH
	rl_init();
#endif

	#if CFG_ENABLE_BUTTON
	key_initialization();
	#endif

#if (CFG_SOC_NAME == SOC_BK7221U)
	#if CFG_USE_USB_CHARGE
	extern void usb_plug_func_open(void);
	usb_plug_func_open();
	#endif
#endif

#if (CFG_OS_FREERTOS)
#if CFG_INT_WDG_ENABLED
	BK_LOGI(TAG, "int watchdog enabled, period=%u\r\n", CFG_INT_WDG_PERIOD_MS);
	bk_wdg_initialize(CFG_INT_WDG_PERIOD_MS);
#else
#if (CFG_SOC_NAME == SOC_BK7271)
	BK_LOGI(TAG, "watchdog disabled\r\n");
	bk_wdg_initialize(CFG_INT_WDG_PERIOD_MS);
	bk_wdg_reload();
	bk_wdg_finalize();
#endif
#endif //CFG_INT_WDG_ENABLED

#if CFG_TASK_WDG_ENABLED
	BK_LOGI(TAG, "task watchdog enabled, period=%u\r\n", CFG_TASK_WDG_PERIOD_MS);
#endif
#endif //CFG_OS_FREERTOS

#if (CFG_SOC_NAME == SOC_BK7271)
#if (CFG_USE_BT)
	BK_LOGI(TAG, "BT active\r\n");
	if (!get_ate_mode_state()) {
		bt_activate(NULL);
	}
#endif
#endif

	BK_LOGI(TAG, "func init completed!\r\n\r\n");
	bk_misc_printf_start_type();

	#if (CFG_SOC_NAME != SOC_BK7236)
	// for debug purpuse on bk7236, comment temporarily 
	reg = 0;
	sddev_control(SCTRL_DEV_NAME, CMD_RF_HOLD_BIT_CLR, &reg);
	#else
	// for debug purpuse on bk7236, add it for not in ate mode
	sctrl_rf_ps_enable_clear();
	#endif
	return 0;
}

UINT32 func_init_basic(void)
{
#if (!CFG_SUPPORT_RTT)
    intc_init();
    hal_flash_init();
#endif
    BK_LOGI(TAG, "\r\nSDK Rev: %s\r\n", BEKEN_SDK_REV);

    return 0;
}

// eof

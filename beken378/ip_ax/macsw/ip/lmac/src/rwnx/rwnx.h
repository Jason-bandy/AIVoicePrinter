/**
 ****************************************************************************************
 *
 * @file rwnx.h
 *
 * @brief Main nX MAC definitions.
 *
 * Copyright (C) RivieraWaves 2011-2020
 *
 ****************************************************************************************
 */

/**
 ****************************************************************************************
 * @mainpage RW-WLAN-nX MAC SW project index page
 *
 * @section intro_sec Introduction
 *
 * RW-WLAN-nX is the RivieraWaves 802.11a/b/g/n/ac IP.\n
 * This document presents the Low Level Design of the RW-WLAN-nX MAC SW.\n
 * A good entry point to this document is the @ref MACSW "MACSW" module page.
 ****************************************************************************************
 */

#ifndef _RWNXL_H_
#define _RWNXL_H_

#include "co_bool.h"

/**
 ****************************************************************************************
 * @defgroup MACSW MACSW
 * @brief RW-WLAN-nX root module.
 * @{
 ****************************************************************************************
 */


#if BK_MAC
#define RW_BAK_REG_LEN               (103)

struct ke_msg;
typedef UINT32 (*pf_msg_outbound)(struct ke_msg *msg);
typedef UINT32 (*pf_data_outbound)(void *host_id, uint32_t frame_len);
typedef UINT32 (*pf_rx_alloc)(uint32_t *host_id, int len);
typedef UINT32 (*pf_get_rx_valid_status)(void);

typedef struct _rw_connector_
{
    pf_msg_outbound msg_outbound_func;
    pf_data_outbound data_outbound_func;
    pf_data_outbound monitor_outbound_func;
    pf_data_outbound spurious_outbound_func;
    pf_rx_alloc rx_alloc_func;
    pf_get_rx_valid_status get_rx_valid_status_func;
} RW_CONNECTOR_T;

/*
 * GLOBAL VARIABLES DECLARATIONS
 ****************************************************************************************
 */
#if BK_MAC
extern RW_CONNECTOR_T g_rwnx_connector;
#endif
#endif //BK_MAC

/**
 ****************************************************************************************
 * @brief This function performs all the initializations of the MAC SW.
 *
 ****************************************************************************************
 */
void rwnxl_init(void);

/**
 ****************************************************************************************
 * @brief Start the MAC SW.
 *
 * Start processing wifi event in an endless loop (or the RTOS scheduler).
 * It doesn't returns.
 ****************************************************************************************
 */
void rwnxl_start(void);

/**
 ****************************************************************************************
 * @brief NX reset event handler.
 * This function is part of the recovery mechanism invoked upon an error detection in the
 * LMAC. It performs the full LMAC reset, and restarts the operation.
 *
 * @param[in] dummy Parameter not used but required to follow the kernel event callback
 * format
 ****************************************************************************************
 */
void rwnxl_reset_evt(int dummy);

/**
 ****************************************************************************************
 * @brief This function checks if any kernel event is pending in the MAC SW.
 * If no event is pending, the CPU clock could be gated.
 *
 * @return true if the CPU can be put in sleep, false otherwise.
 ****************************************************************************************
 */
bool rwnxl_cpu_can_sleep(void);

#if BK_MAC
bool rwnxl_get_status_in_doze(void);
typedef void (*IDLE_FUNC)(void);
extern bool rwnxl_sleep(IDLE_FUNC wait_func,IDLE_FUNC do_func);
extern void rwnxl_wakeup(IDLE_FUNC wait_func);
extern void rwnxl_set_nxmac_timer_value(void);

extern int wifi_mac_state_set_idle(void);
extern void wifi_mac_state_set_active(void);
extern void wifi_mac_state_set_prev(void);
extern void wifi_general_mac_state_set_idle(void);
extern void wifi_general_mac_state_set_active(void);
extern void rwnxl_register_connector(RW_CONNECTOR_T *intf);

// FIXME: move function declaration to other place
extern int rwnx_get_noht_rssi_thresold(void) __attribute__ ((weak));
extern uint32_t rwnx_setting_for_single_rate(UINT32 att_value) __attribute__ ((weak));
extern int rwnx_printf_fun(const char *fmt, ...) __attribute__ ((weak));
uint32_t rwnxl_get_reseting_flag(void);

#else
/**
 ****************************************************************************************
 * @brief This function performs the required checks prior to go to DOZE mode.
 * If all these checks pass, then the MAC HW is put in DOZE mode.
 *
 ****************************************************************************************
 */
int rwnxl_sleep(void);

/**
 ****************************************************************************************
 * @brief This function performs the wake up from DOZE mode.
 *
 ****************************************************************************************
 */
void rwnxl_wakeup(void);
#endif


/// @}

#endif // _RWNXL_H_

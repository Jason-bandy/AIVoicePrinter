#ifndef __APP_APCFG_H__
#define __APP_APCFG_H__

#define DEMO_APCFG_QITEM_COUNT             (4)

typedef enum
{
    DEMO_APCFG_IDLE_STATE         = 0,
    DEMO_APCFG_AP_STATE,
    DEMO_APCFG_STA_STATE,
    DEMO_APCFG_STAOK_STATE,

} DEMO_APCFG_STATE;

typedef enum
{
    DEMO_APCFG_STA_NONE         = 0,
    DEMO_APCFG_START,
    DEMO_APCFG_STOP,
    DEMO_APCFG_RESET,
    DEMO_APCFG_TIMER_POLL,
    DEMO_APCFG_REC_SSIDOK,
    DEMO_APCFG_WLAN_LINK,
    DEMO_APCFG_STA_OK,
    DEMO_APCFG_STA_DISCON,
    DEMO_APCFG_HTTP_OTA,
}DEMO_APCFG_MSG;


UINT32 app_demo_apcfg_init(void);
void app_demo_apcfg_send_msg(DEMO_APCFG_MSG new_msg);

#endif


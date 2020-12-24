#ifndef __APP_SOFTAP_DEMO_H__
#define __APP_SOFTAP_DEMO_H__

#include "sys_config.h"

enum
{
    DAP_TIMER_POLL          = 0, 
    DAP_WIFI_DISCONECTED,    
    DAP_WIFI_CONECTED,      
    DAP_APP_CONECTED,
    DAP_APP_DISCONECTED,      
    DAP_EXIT,
    DAP_START_OTA,
    DAP_CHGTO_STA,	//chen
};

void app_demo_softap_send_msg(u32 new_msg, u32 new_data);

typedef  struct  				//CHEN
{
    UINT8 UserCamRev;       /*camera rev*/
    char ssid[32];
    char pwd[64];
} UserTypedef;

typedef struct tvideo_hdr_st
{
    UINT8 id;
    UINT8 is_eof;
    UINT8 pkt_cnt; 
    UINT8 size;
    
#if(CFG_SUPPORT_TIANZHIHENG_DRONE)
    char hd_dt0;
    char hd_dt1;
    char hd_dt2;
    UINT8 hd_dt3; 	
#endif
}HDR_ST, *HDR_PTR;

typedef struct tvideo_ota_st
{
    const char *http_url;
    int    http_port;
    UINT32 http_timeout;
}TV_OTA_ST, *TV_OTA_PTR;

extern UserTypedef UserPara;	//CHEN

extern char st_ssid[32];
extern char st_pwd[32];
#include "video_transfer.h"
void app_demo_add_pkt_header(TV_HDR_PARAM_PTR param);
void app_drone_get_user_ssidkey(void);
void  app_drone_save_ssidkey(void);
void app_ap_main_deinit( void );
int app_demo_softap_is_ota_doing(void);
int app_drone_get_did(void *did);
int app_drone_save_did(void *did);

#endif  // __APP_SOFTAP_DEMO_H__
#ifndef __APP_DRONE_H__
#define __APP_DRONE_H__

enum
{
    DMSG_TIMER_POLL          = 0, 
    DMSG_WIFI_DISCONECTED,    
    DMSG_WIFI_CONECTED,      
    DMSG_APP_CONECTED,
    DMSG_APP_DISCONECTED,      
    DMSG_EXIT,
};

void app_drone_send_msg(u32 new_msg);

#define APP_DEMO_PKT_HDR_8BYTE            1
typedef struct tvideo_hdr_st
{
    UINT8 id;
    UINT8 is_eof;
    UINT8 pkt_cnt; 
    UINT8 size;
#if(APP_DEMO_PKT_HDR_8BYTE)
    char hd_dt0;
    char hd_dt1;
    char hd_dt2;
    UINT8 hd_dt3; 	
#endif
}HDR_ST, *HDR_PTR;

#include "video_transfer.h"
void app_drone_add_pkt_header(TV_HDR_PARAM_PTR param);
#endif

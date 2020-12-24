#ifndef __APP_DEMO_UDP_H__
#define __APP_DEMO_UDP_H__

typedef struct udp_apcfg_st		//chen
{
    char ssid[32];	
    char pwd[32];	
    u16 ssid_len;
    u32 pwd_len;
}UDP_APCFG_ST, *UDP_APCFG_PTR;

UINT32 app_demo_udp_init(void);
void app_demo_udp_deinit(void);
int app_demo_udp_send_packet (UINT8 *data, UINT32 len);
void app_demo_disconnect_cmd_udp(void);

#endif
// eof


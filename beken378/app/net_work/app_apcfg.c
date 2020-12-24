#include "include.h"


#if 1//CFG_USE_APP_DEMO_VIDEO_TRANSFER
#include "common.h"
#include "uart_pub.h"
#include "mem_pub.h"

#include "task.h"
#include "rtos_pub.h"
#include "error.h"

#include "wlan_ui_pub.h"
#include "ieee802_11_demo.h"
#include "rw_pub.h"


#include <lwip/inet.h>
#include "lwip/sockets.h"
#include "ieee802_11_defs.h"


#define APP_APCFG_DEBUG        1
#if APP_APCFG_DEBUG
#define APP_APCFG_PRT      os_printf
#define APP_APCFG_WARN     warning_prf
#define APP_APCFG_FATAL    fatal_prf
#else
#define APP_APCFG_PRT      null_prf
#define APP_APCFG_WARN     null_prf
#define APP_APCFG_FATAL    null_prf
#endif

#define APP_APCFG_QITEM_COUNT           4
#define APP_APCFG_TIMER_INTVAL          500  // ms

#define APP_APCFG_TCP_RCV_BUF_LEN       1460
#define APP_APCFG_TCP_LISTEN_MAX        1
#define APP_APCFG_TCP_SERVER_PORT       5203 

#define APP_APCFG_OFFAP_BEFORE_STA      0

enum
{
    APCFG_WIFI_DISCONECTED = 0,    
    APCFG_WIFI_CONECTED,      
    APCFG_APP_CONECTED,
    APCFG_APP_DISCONECTED,   
    APCFG_GET_NETINFO,
    APCFG_STA_OK,
    APCFG_STA_KEYERR,
    APCFG_EXIT,
}APCFG_MSG;

typedef enum
{
    APCFG_STATUS_WIFI_DISCONECTED          = 0, 
    APCFG_STATUS_WIFI_CONECTED,
    APCFG_STATUS_APP_DISCONECTED,
    APCFG_STATUS_APP_CONECTED, 
    APCFG_STATUS_GET_NETINFO,
    APCFG_STATUS_STA_OK,
    APCFG_STATUS_STA_KEYERR,
} APCFG_STATUS;


typedef struct apcfg_message 
{
    u32 dmsg;
    u32 data;
}APCFG_MSG_T;

typedef struct app_apcfg_st
{
    xTaskHandle thread_hdl;
    beken_queue_t msg_que; 
    u32 status;
    u32 ap_up;

    xTaskHandle tcp_hdl;
    int tcp_fd;
    int fd_list[APP_APCFG_TCP_LISTEN_MAX];
    volatile int tcp_run;
    
	char ssid[SSID_MAX_LEN];	
	char pwd[SSID_MAX_LEN];	
    u16 ssid_len;
    u32 pwd_len;
}APP_APCFG_ST, *APP_APCFG_PTR;


APP_APCFG_PTR g_apcfg = NULL;
void app_apcfg_send_msg(u32 new_msg, u32 new_data)
{
    OSStatus ret;
    APCFG_MSG_T msg;

    if(g_apcfg && g_apcfg->msg_que) 
    {
        msg.dmsg = new_msg;
        
        ret = rtos_push_to_queue(&g_apcfg->msg_que, &msg, BEKEN_NO_WAIT);
        if(kNoErr != ret)
        {
            APP_APCFG_FATAL("app_apcfg_send_msg failed\r\n");
        }
    }
}

static void app_apcfg_tcp_set_keepalive(int fd)
{
    int opt = 1, ret;
    // open tcp keepalive
    ret = lwip_setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof(int));

    opt = 30;  // 30 second
    ret = lwip_setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &opt, sizeof(int)); 

    opt = 1;  // 1s second for intval
    ret = lwip_setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &opt, sizeof(int)); 

    opt = 3;  // 3 times
    ret = lwip_setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &opt, sizeof(int));  
    ret = ret;
}

static void app_apcfg_tcp_rev_handler(int tcpcleint_fd, u8* rev_buf, u32 rev_lan)
{
    #define APCFG_SSID_HEADER       "ssid"
    #define APCFG_KEY_HEADER        "key"
    
    u32 string_len, need_sendmsg = 0;
    u8* string_ptr = rev_buf;

    os_printf("%s\r\n", string_ptr, strstr(string_ptr, APCFG_SSID_HEADER));
    os_printf("%s\r\n", string_ptr, strstr(string_ptr, APCFG_KEY_HEADER));
    
    if(g_apcfg->ssid_len == 0)
    {   
        string_ptr = strstr(rev_buf, APCFG_SSID_HEADER);
        if(string_ptr)
        {
            u8 *start, *end, len;
            start = strchr(string_ptr, (int)(':')) + 1;
            end = strchr(string_ptr, (int)(','));
            os_printf("%p, %p\r\n", start, end);
            
            len = (u32)end - (u32)(start);

            rev_lan -= (strlen(APCFG_SSID_HEADER) + 1);

            if((len <= rev_lan) && len)
            {
                os_memcpy(g_apcfg->ssid, start, len);
                g_apcfg->ssid_len = len;

                lwip_send(tcpcleint_fd, "ssid ok\r\n", strlen("ssid ok\r\n"), MSG_DONTWAIT|MSG_MORE);
                need_sendmsg = 1;

                string_ptr = end + 1;
            }
        }
    }

    if(g_apcfg->pwd_len == 0)
    {
        string_ptr = strstr(rev_buf, APCFG_KEY_HEADER);
        if(string_ptr)
        {
            u8 *start, *end, len;
            start = strchr(string_ptr, (int)(':')) + 1;
            end = strchr(string_ptr, (int)(','));
            
            os_printf("%p, %p\r\n", start, end);
            len = (u32)end - (u32)(start);

            rev_lan -= (strlen(APCFG_KEY_HEADER) + 1);

            if((len <= rev_lan) && len)
            {
                os_memcpy(g_apcfg->pwd, start, len);
                g_apcfg->pwd_len = len;

                lwip_send(tcpcleint_fd, "pwd ok\r\n", strlen("pwd ok\r\n"), MSG_DONTWAIT|MSG_MORE);
                need_sendmsg = 1;

                string_ptr = end + 1;
            }
        }
    }

    if(need_sendmsg)
    {
        if((g_apcfg->ssid_len) && (g_apcfg->pwd_len))
        {
            app_apcfg_send_msg(APCFG_GET_NETINFO, 0);
        }
    }
    
}

static void app_apcfg_tcp_app_connected(void)
{
    app_apcfg_send_msg(APCFG_APP_CONECTED, 0);
}

static void app_apcfg_tcp_app_disconnected(void)
{
    app_apcfg_send_msg(APCFG_APP_DISCONECTED, 0);
}

static void app_apcfg_tcp_main( beken_thread_arg_t data )
{
    GLOBAL_INT_DECLARATION();
    OSStatus err = kNoErr;
    int maxfd = -1;
    int ret = 0, i = 0;
    int snd_len = 0, rcv_len = 0;
    struct sockaddr_in server_addr;
    socklen_t srvaddr_len = 0;
    fd_set watchfd, watchwd;
    u8 *rcv_buf = NULL;

    (void)(data);  

    APP_APCFG_PRT("app_apcfg_tcp_main entry\r\n");

    rcv_buf = (u8*)os_malloc((APP_APCFG_TCP_RCV_BUF_LEN + 1) * sizeof(u8));
    if(!rcv_buf) {
        APP_APCFG_WARN("tcp os_malloc failed\r\n");
        goto app_apcfg_tcp_exit;
    }

    g_apcfg->tcp_fd = lwip_socket(AF_INET, SOCK_STREAM, 0);
    if (g_apcfg->tcp_fd == -1) {
        APP_APCFG_WARN("socket failed\r\n");
        goto app_apcfg_tcp_exit;
    }
 
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(APP_APCFG_TCP_SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    srvaddr_len = (socklen_t)sizeof(server_addr);
    if (lwip_bind(g_apcfg->tcp_fd, (struct sockaddr *)&server_addr, srvaddr_len) == -1) {
        APP_APCFG_WARN("bind failed\r\n");
        goto app_apcfg_tcp_exit;
    }
 
    if (lwip_listen(g_apcfg->tcp_fd, APP_APCFG_TCP_LISTEN_MAX) == -1) {
        APP_APCFG_WARN("listen failed\r\n");
        goto app_apcfg_tcp_exit;
    }

    maxfd = g_apcfg->tcp_fd;
    for (i=0; i<APP_APCFG_TCP_LISTEN_MAX; i++) {
        g_apcfg->fd_list[i] = -1;
    }

    GLOBAL_INT_DISABLE();
    g_apcfg->tcp_run = 1;
    GLOBAL_INT_RESTORE();
    
    os_memset(g_apcfg->ssid, 0, sizeof(g_apcfg->ssid));
    g_apcfg->ssid_len = 0;
    os_memset(g_apcfg->pwd, 0, sizeof(g_apcfg->pwd));
    g_apcfg->pwd_len = 0;

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = (100 % 1000) * 1000;  // 100ms
    
    while ( g_apcfg->tcp_run )
    {
        FD_ZERO(&watchfd);
        FD_SET(g_apcfg->tcp_fd, &watchfd);

        for (i=0; i<APP_APCFG_TCP_LISTEN_MAX; i++) {
            if(g_apcfg->fd_list[i] != -1) {
                FD_SET(g_apcfg->fd_list[i], &watchfd);
                if (maxfd < g_apcfg->fd_list[i]) 
                {
                    maxfd = g_apcfg->fd_list[i];
                }
            }
        }        

        ret = lwip_select(maxfd+1, &watchfd, NULL, NULL, &timeout);
        if (ret < 0) 
        {
            APP_APCFG_WARN("select ret:%d\r\n", ret);
            break;
        }
        else if(ret == 0)
        {
            // timeout
            continue;
        }
        else 
        {
            // is new connection 
            if (FD_ISSET(g_apcfg->tcp_fd, &watchfd)) 
            { 
                int new_cli_sockfd = -1;
                struct sockaddr_in client_addr;
                socklen_t cliaddr_len = 0;
            
                cliaddr_len = sizeof(client_addr);
                new_cli_sockfd = lwip_accept(g_apcfg->tcp_fd, (struct sockaddr *)&client_addr, &cliaddr_len);
                if (new_cli_sockfd < 0) { 
                    APP_APCFG_WARN("accept return fd:%d\r\n", new_cli_sockfd);
                    break;
                }

                APP_APCFG_PRT("new accept fd:%d\r\n", new_cli_sockfd);

                for (i=0; i<APP_APCFG_TCP_LISTEN_MAX; i++) {
                    if (g_apcfg->fd_list[i] == -1) 
                    {
                        g_apcfg->fd_list[i] = new_cli_sockfd;

                        app_apcfg_tcp_set_keepalive(new_cli_sockfd);
                        break;
                    }
                }

                if(i == APP_APCFG_TCP_LISTEN_MAX) {
                    APP_APCFG_WARN("only accept %d clients\r\n", APP_APCFG_TCP_LISTEN_MAX);
                    lwip_close(new_cli_sockfd);
                } 
            }

            // search those added fd  
            for (i=0; i<APP_APCFG_TCP_LISTEN_MAX; i++) 
            { 
                if (g_apcfg->fd_list[i] == -1) {
                    continue;
                }
                if (!FD_ISSET(g_apcfg->fd_list[i], &watchfd)) {
                    continue;
                }

                rcv_len = lwip_recv(g_apcfg->fd_list[i], rcv_buf, APP_APCFG_TCP_RCV_BUF_LEN, 0);
                if(rcv_len <= 0) 
                {
                    int j;
                    // close this socket
                    APP_APCFG_PRT("recv close fd:%d\r\n", g_apcfg->fd_list[i]);
                    lwip_close(g_apcfg->fd_list[i]);
                    g_apcfg->fd_list[i] = -1;

                    for (j=0; j<APP_APCFG_TCP_LISTEN_MAX; j++) {
                        if (g_apcfg->fd_list[j] != -1) 
                        {
                            break;
                        }
                    }
                    if(j == APP_APCFG_TCP_LISTEN_MAX) {
                        APP_APCFG_PRT("not client left, close spidma\r\n");
                        
                    } 
                } 
                else 
                {
                    rcv_len = (rcv_len > APP_APCFG_TCP_RCV_BUF_LEN)? APP_APCFG_TCP_RCV_BUF_LEN: rcv_len;
                    rcv_buf[rcv_len] = 0;

                    app_apcfg_tcp_rev_handler(g_apcfg->fd_list[i], rcv_buf, rcv_len);
                    
                    //snd_len = lwip_send(g_apcfg->fd_list[i], rcv_buf, rcv_len, 0);
                    //if (snd_len < 0) {
                    //    /* err */
                    //    APP_APCFG_PRT("send return fd:%d\r\n", snd_len);
                    //}
                }
                FD_CLR(g_apcfg->fd_list[i], &watchfd);
            }
        }// ret = select
    }
    
app_apcfg_tcp_exit:
    
    APP_APCFG_PRT("app_apcfg_tcp_main exit\r\n");

    if(rcv_buf) {
        os_free(rcv_buf);
        rcv_buf = NULL;
    }

    for (i=0; i<APP_APCFG_TCP_LISTEN_MAX; i++) {
        if(g_apcfg->fd_list[i] != -1) 
        {
            lwip_close(g_apcfg->fd_list[i]);
            g_apcfg->fd_list[i] = -1;
        }
    }

    if(g_apcfg->tcp_fd != -1) {
        lwip_close(g_apcfg->tcp_fd);
        g_apcfg->tcp_fd = -1;
    }

    GLOBAL_INT_DISABLE();
    g_apcfg->tcp_run = 0;
    GLOBAL_INT_RESTORE();

    g_apcfg->tcp_hdl = NULL;
    rtos_delete_thread(NULL);
}

int app_apcfg_tcp_send_packet(UINT8 *data, UINT32 len)
{
    int i = 0, snd_len = 0;

    if((!g_apcfg->tcp_hdl) || (g_apcfg->tcp_fd == -1))
        return 0;

    for (i=0; i<APP_APCFG_TCP_LISTEN_MAX; i++) 
    { 
        if (g_apcfg->fd_list[i] == -1) {
            continue;
        }

        snd_len = lwip_send(g_apcfg->fd_list[i], data, len, MSG_DONTWAIT|MSG_MORE);
        if (snd_len < 0) {
            snd_len = 0;
            APP_APCFG_WARN("app_apcfg_tcp_send_packet failed\r\n");
        }
    }

    return snd_len;
}

static UINT32 app_apcfg_tcp_init(void)
{
    int ret;

    APP_APCFG_PRT("app_apcfg_tcp_init\r\n");
    if(!g_apcfg->tcp_hdl)
    {
        ret = rtos_create_thread(&g_apcfg->tcp_hdl,
                                      BEKEN_DEFAULT_WORKER_PRIORITY,
                                      "apcfg_tcp",
                                      (beken_thread_function_t)app_apcfg_tcp_main,
                                      1024,
                                      (beken_thread_arg_t)NULL);
        if (ret != kNoErr)
        {
            APP_APCFG_FATAL("Error: Failed g_apcfg->tcp_hdl: %d\r\n", ret);
            return kGeneralErr;
        }
    }

    return kNoErr;
}

static void  app_apcfg_tcp_deinit(void)
{
    GLOBAL_INT_DECLARATION();

    if(g_apcfg->tcp_run == 0)
        return;

    APP_APCFG_PRT("app_demo_tcp_deinit\r\n");

    GLOBAL_INT_DISABLE();
    g_apcfg->tcp_run = 0;
    GLOBAL_INT_RESTORE();

    while(g_apcfg->tcp_hdl)
        rtos_delay_milliseconds(10);
}

static int app_apcfg_rw_event_func(rw_evt_type evt_type, void *data)
{
    struct rw_evt_payload *evt_payload = (struct rw_evt_payload *)data;

    if(evt_type == RW_EVT_AP_CONNECTED) {
        APP_APCFG_PRT("RW_EVT_AP_CONNECTED-(mac="MACSTR")\r\n",  MAC2STR(evt_payload->mac));
            app_apcfg_send_msg(APCFG_WIFI_CONECTED, 0);          
    } else if(evt_type == RW_EVT_AP_DISCONNECTED) {
        APP_APCFG_PRT("RW_EVT_AP_DISCONNECTED-(mac="MACSTR")\r\n",  MAC2STR(evt_payload->mac));
            app_apcfg_send_msg(APCFG_WIFI_DISCONECTED, 0);  
    } else if(evt_type == RW_EVT_STA_CONNECT_FAILED) {
        APP_APCFG_PRT("RW_EVT_STA_CONNECT_FAILED-(mac="MACSTR")\r\n",  MAC2STR(evt_payload->mac));
            app_apcfg_send_msg(APCFG_STA_KEYERR, 0);  
    }
    

    return 0;
}

static int app_apcfg_ap_setup(void)
{   
// for softap configuration
#define APP_APCFG_AP_SSID          "beken_ap_netcfg_000"
#define APP_APCFG_AP_KEY           "1234567890"
#define APP_APCFG_AP_NET_IP        "192.168.1.1"
#define APP_APCFG_AP_NET_MASK      "255.255.255.0"
#define APP_APCFG_AP_NET_GW        "192.168.1.1"
#define APP_APCFG_AP_CHANNEL       1   

    network_InitTypeDef_st wNetConfig;
    int len;

    if(g_apcfg->ap_up)
        return 0;

    len = os_strlen(APP_APCFG_AP_SSID);
    if(SSID_MAX_LEN < len)
    {
        APP_APCFG_FATAL("ssid name more than 32 Bytes\r\n");
        return -1;
    }

    os_memset(&wNetConfig, 0x0, sizeof(network_InitTypeDef_st));  

    os_strcpy((char *)wNetConfig.local_ip_addr, APP_APCFG_AP_NET_IP);
    os_strcpy((char *)wNetConfig.net_mask, APP_APCFG_AP_NET_MASK);
    os_strcpy((char *)wNetConfig.gateway_ip_addr, APP_APCFG_AP_NET_GW);
    os_strcpy((char *)wNetConfig.dns_server_ip_addr, APP_APCFG_AP_NET_GW);
    os_strcpy((char *)wNetConfig.wifi_ssid, APP_APCFG_AP_SSID);
    os_strcpy((char *)wNetConfig.wifi_key, APP_APCFG_AP_KEY);
    
    wNetConfig.wifi_mode = BK_SOFT_AP;
    wNetConfig.dhcp_mode = DHCP_SERVER;
    wNetConfig.wifi_retry_interval = 100;

    bk_wlan_ap_set_default_channel(APP_APCFG_AP_CHANNEL);
    
    APP_APCFG_PRT("set ip info: %s,%s,%s\r\n",
            wNetConfig.local_ip_addr,
            wNetConfig.net_mask,
            wNetConfig.dns_server_ip_addr);
    
    APP_APCFG_PRT("ssid:%s  key:%s\r\n", wNetConfig.wifi_ssid, wNetConfig.wifi_key);

    rw_evt_set_callback(RW_EVT_AP_CONNECTED, app_apcfg_rw_event_func);
    rw_evt_set_callback(RW_EVT_AP_DISCONNECTED, app_apcfg_rw_event_func);
    
    bk_wlan_start(&wNetConfig);

    g_apcfg->ap_up = 1;

    return 0;    
}

static int app_apcfg_ap_shutdown(void)
{
    if(g_apcfg->ap_up)
    {
        rw_evt_set_callback(RW_EVT_AP_CONNECTED, NULL);
        rw_evt_set_callback(RW_EVT_AP_DISCONNECTED, NULL);
        
        bk_wlan_stop(BK_SOFT_AP);
        g_apcfg->ap_up = 0;
    }
}

extern void user_connected_callback(FUNCPTR fn);
static void app_apcfg_sta_ok_callback(void)
{
    app_apcfg_send_msg(APCFG_STA_OK, 0);
    user_connected_callback(NULL);
    rw_evt_set_callback(RW_EVT_STA_CONNECT_FAILED, NULL);
}

static void app_apcfg_sta_connect(char *oob_ssid, char *connect_key)
{   
    network_InitTypeDef_st type;

    int len;
	os_memset(&type, 0x0, sizeof(network_InitTypeDef_st));

    len = os_strlen(oob_ssid);
    if(SSID_MAX_LEN < len)
    {
        APP_APCFG_FATAL("ssid name more than 32 Bytes\r\n");
        return;
    }
    
	os_strcpy((char *)type.wifi_ssid, oob_ssid);
	os_strcpy((char *)type.wifi_key, connect_key);
    
	type.wifi_mode = BK_STATION;
	type.dhcp_mode = DHCP_CLIENT;
	type.wifi_retry_interval = 100;

    user_connected_callback(app_apcfg_sta_ok_callback);
    rw_evt_set_callback(RW_EVT_STA_CONNECT_FAILED, app_apcfg_rw_event_func);
    
	APP_APCFG_PRT("ssid:%s key:%s\r\n", type.wifi_ssid, type.wifi_key);
	bk_wlan_start(&type);
}

static void app_apcfg_sta_connect_stop(void)
{
    user_connected_callback(NULL);
    rw_evt_set_callback(RW_EVT_STA_CONNECT_FAILED, NULL);
    
    bk_wlan_stop(BK_STATION);
}

static void app_apcfg_main( beken_thread_arg_t data )
{
    OSStatus err;
    int ret = 0;
    u32 status;

    app_apcfg_ap_setup();
    
    g_apcfg->status = APCFG_STATUS_WIFI_DISCONECTED;

    while(1) 
    {
        APCFG_MSG_T msg;
        status = g_apcfg->status;
        
        err = rtos_pop_from_queue(&g_apcfg->msg_que, &msg, BEKEN_WAIT_FOREVER);
        if(kNoErr == err)
        {
            switch(msg.dmsg) 
            {
                case APCFG_WIFI_DISCONECTED:
                    if(g_apcfg->status == APCFG_STATUS_WIFI_CONECTED) 
                    {
                        g_apcfg->status = APCFG_STATUS_WIFI_DISCONECTED;
                        APP_APCFG_PRT("wifi disconnected!\r\n");
                    }
                    break;                    

                case APCFG_WIFI_CONECTED:
                    APP_APCFG_PRT("g_apcfg->status:%d\r\n", g_apcfg->status);
                    
                    if(g_apcfg->status == APCFG_STATUS_WIFI_DISCONECTED) 
                    {
                        g_apcfg->status = APCFG_STATUS_WIFI_CONECTED;
                        
                        //app_led_send_msg(LED_CONNECT);
                        app_apcfg_tcp_init();

                        APP_APCFG_PRT("wifi connected!\r\n");
                    }
                    #if (APP_APCFG_OFFAP_BEFORE_STA == 0)
                    else if(g_apcfg->status == APCFG_STATUS_STA_OK)
                    {
                        APP_APCFG_PRT("ap wifi connected, after sta ok!\r\n");

                        app_apcfg_tcp_send_packet("sta connected\r\n", strlen("sta connected\r\n"));

                        rtos_delay_milliseconds(1000);
                        app_apcfg_tcp_deinit();
                        app_apcfg_ap_shutdown();

                        app_apcfg_send_msg(APCFG_EXIT, 0);
                    }
                    else if(g_apcfg->status == APCFG_STATUS_STA_KEYERR)
                    {
                        app_apcfg_tcp_send_packet("sta key err\r\n", strlen("sta key err\r\n"));

                        os_memset(g_apcfg->ssid, 0, sizeof(g_apcfg->ssid));
                        g_apcfg->ssid_len = 0;
                        os_memset(g_apcfg->pwd, 0, sizeof(g_apcfg->pwd));
                        g_apcfg->pwd_len = 0;
                        APP_APCFG_PRT("clear ssid key\r\n");

                        g_apcfg->status == APCFG_STATUS_WIFI_CONECTED;
                    }
                    #endif
                    break;

                case APCFG_GET_NETINFO:
                    APP_APCFG_PRT("got netinfo!\r\n");
                    g_apcfg->status = APCFG_STATUS_GET_NETINFO;
                    APP_APCFG_WARN("ssid:%s, len:%d\r\n", g_apcfg->ssid, g_apcfg->ssid_len);
                    APP_APCFG_WARN("key:%s, len:%d\r\n", g_apcfg->pwd, g_apcfg->pwd_len);

                    #if APP_APCFG_OFFAP_BEFORE_STA
                    app_apcfg_tcp_deinit();
                    app_apcfg_ap_shutdown();
                    app_apcfg_sta_connect(g_apcfg->ssid, g_apcfg->pwd);

                    app_apcfg_send_msg(APCFG_EXIT, 0);
                    #else
                    app_apcfg_sta_connect(g_apcfg->ssid, g_apcfg->pwd);
                    #endif
                    break;

                case APCFG_STA_OK:
                    if(g_apcfg->status == APCFG_STATUS_GET_NETINFO)
                    {
                        APP_APCFG_PRT("sta connected!\r\n");
                        
                        g_apcfg->status = APCFG_STATUS_STA_OK;
                    }
                    break;  
                    
                case APCFG_STA_KEYERR:
                    if(g_apcfg->status == APCFG_STATUS_GET_NETINFO)
                    {
                        APP_APCFG_PRT("sta key err:%s!\r\n", g_apcfg->pwd);

                        g_apcfg->status = APCFG_STATUS_STA_KEYERR;
                        //app_apcfg_sta_connect_stop();
                    }
                    break;  

                case APCFG_EXIT:
                    goto app_apcfg_main_exit;
                    break;
                                        
                default:
                    break;
            }
        } 
    }

app_apcfg_main_exit:

    APP_APCFG_PRT("app app_apcfg_main_exit!\r\n");

    app_apcfg_tcp_deinit();
    
    app_apcfg_ap_shutdown();
    
    rtos_deinit_queue(&g_apcfg->msg_que);

    os_free(g_apcfg);
    g_apcfg = NULL;
    
    rtos_delete_thread(NULL);
}

void app_apcfg_init(void)
{
    int ret;

    APP_APCFG_PRT("app_apcfg_init\r\n");

    if(!g_apcfg) 
    {
        g_apcfg = os_malloc(sizeof(APP_APCFG_ST));
        if(!g_apcfg) {
            APP_APCFG_FATAL("app_apcfg_init malloc failed\r\n");
            return;
        }

        os_memset(g_apcfg, 0, sizeof(APP_APCFG_ST));

        ret = rtos_init_queue(&g_apcfg->msg_que, 
                                "apcfg",
                                sizeof(APCFG_MSG_T),
                                APP_APCFG_QITEM_COUNT);
        if (kNoErr != ret) 
        {
            APP_APCFG_FATAL("app_apcfg_init ceate queue failed\r\n");
            os_free(g_apcfg);
            g_apcfg = NULL;
            return;
        }   
        
        ret = rtos_create_thread(&g_apcfg->thread_hdl,
                                      BEKEN_DEFAULT_WORKER_PRIORITY,
                                      "apcfg",
                                      (beken_thread_function_t)app_apcfg_main,
                                      2048,
                                      (beken_thread_arg_t)NULL);
        if (ret != kNoErr)
        {
            APP_APCFG_FATAL("Error: Failed to create apcfg: %d\r\n", ret);

            rtos_deinit_queue(&g_apcfg->msg_que);
            os_free(g_apcfg);
            g_apcfg = NULL;
            return;
        }

    }

}

void app_apcfg_deinit( void )
{
    if(g_apcfg) 
    {
        app_apcfg_send_msg(APCFG_EXIT, 0);

        while(g_apcfg)
            rtos_delay_milliseconds(10);
    }
}

void start_apcfg(int argc, char** argv)
{
    app_apcfg_init();
}
MSH_CMD_EXPORT(start_apcfg, start_apcfg test);

void stop_apcfg(int argc, char** argv)
{
    app_apcfg_deinit();
}
MSH_CMD_EXPORT(stop_apcfg, stop_apcfg test);
#endif


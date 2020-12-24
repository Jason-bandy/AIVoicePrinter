#include "include.h"

// led show spi data state for tianzhiheng
#if CFG_SUPPORT_TIANZHIHENG_DRONE
#include "FreeRTOS.h"
#include "task.h"
#include "rtos_pub.h"
#include "error.h"
#include "fake_clock_pub.h"
#include "mem_pub.h"
#include "gpio_pub.h"
#include "app_led.h"
#include "app_demo_apcfg.h"
#include "app_demo_softap.h"
#include "net_param_pub.h"			//chen
#include "wlan_ui_pub.h"			//chen
#include "rw_pub.h"			        //chen
#include "common.h"			        //chen
//#include "http_client_ota.h"
#include "tzh_ppcs_api.h"
#include "app_video_intf.h"
#include "rw_msg_pub.h"
#include "uart_pub.h"
#include "ntp.h"
#include "motor_main.h"
#include "app_jpeg2avi.h"

#define DEMO_APCFG_DEBUG
#ifdef DEMO_APCFG_DEBUG
#define DEMO_APCFG_PRT      os_printf
#define DEMO_APCFG_WARN     warning_prf
#define DEMO_APCFG_FATAL    fatal_prf
#else
#define DEMO_APCFG_PRT      null_prf
#define DEMO_APCFG_WARN     null_prf
#define DEMO_APCFG_FATAL    null_prf
#endif
#define	SPK_OFF				0
#define	SPK_ON					1

#if(TZH_CUS_MODULE==TZH_CUS1_MOTOR)

#define SPK_GPIO_INDEX              GPIO3
#endif
extern void app_video_intf_open(void);

extern void user_main( beken_thread_arg_t args );
extern void app_sta_connect(char *oob_ssid, char *connect_key);
extern int app_video_intf_open_p2p(void);
extern void app_audio_intf_open (void);
extern void http_ota_alarm_start(void);
extern int dapcm_play_init(int sample_rate);
extern int dapcm_play_write_data(unsigned char * buffer, int buffer_bytes);
extern void init_udphandle_cmd(void);


typedef struct demo_apcfg_st
{
    beken_timer_t demo_apcfg_timer;
    DEMO_APCFG_STATE state;
    DEMO_APCFG_MSG msg;
} DEMO_APCFG_ST, DEMO_APCFG_PTR;

typedef struct demo_apcfg_message
{
    DEMO_APCFG_MSG demo_apcfg_msg;
}DEMO_APCFG_MSG_T;

static DEMO_APCFG_ST demo_apcfgctr;
beken_queue_t demo_apcfg_msg_que = NULL;
xTaskHandle demo_apcfg_thread_handle = NULL;
UINT8 g_sendAck=0;

extern void user_connected_callback(FUNCPTR fn);
extern void app_video_intf_close(void);
extern  int http_ota_fw_download(const char *uri, int need_cmp_vision);
extern void rt_hw_cpu_reset(void);

void Spk_Ctl(UINT8 on)
{
#if(TZH_CUS_MODULE==TZH_CUS1_MOTOR)
    	bk_gpio_config_output(SPK_GPIO_INDEX);		

	if(on)
		{
    		bk_gpio_output(SPK_GPIO_INDEX, 1);
		}
	else
		{
    		bk_gpio_output(SPK_GPIO_INDEX, 0);
		}
#endif	
}

static void app_demo_apcfg_sta_ok_callback(void)
{
    app_demo_apcfg_send_msg(DEMO_APCFG_STA_OK);
    os_printf("=========================\r\n");
    os_printf("demo_apcfg_sta_ok!!!!\r\n");

    user_connected_callback(NULL);
    rw_evt_set_callback(RW_EVT_STA_CONNECT_FAILED, NULL);
}

static int app_demo_apcfg_rw_event_func(rw_evt_type evt_type, void *data)
{
    struct rw_evt_payload *evt_payload = (struct rw_evt_payload *)data;

    if(evt_type == RW_EVT_AP_CONNECTED) 
    {
        os_printf("RW_EVT_AP_CONNECTED-(mac="MACSTR")\r\n",  MAC2STR(evt_payload->mac));
        //  app_apcfg_send_msg(APCFG_WIFI_CONECTED, 0);          
    }
    else if(evt_type == RW_EVT_AP_DISCONNECTED)
    {
        os_printf("RW_EVT_AP_DISCONNECTED-(mac="MACSTR")\r\n",  MAC2STR(evt_payload->mac));
        //    app_apcfg_send_msg(APCFG_WIFI_DISCONECTED, 0);  
    }
    else if(evt_type == RW_EVT_STA_CONNECT_FAILED) 
    {
        os_printf("RW_EVT_STA_CONNECT_FAILED-(mac="MACSTR")\r\n",  MAC2STR(evt_payload->mac));
        //  app_apcfg_send_msg(APCFG_STA_KEYERR, 0);  
    }
    else if(evt_type == RW_EVT_STA_DISCONNECTED)
    {
        os_printf("RW_EVT_STA_DISCONNECTED-(mac="MACSTR")\r\n",  MAC2STR(evt_payload->mac));
        app_demo_apcfg_send_msg(DEMO_APCFG_STA_DISCON);
        user_connected_callback(app_demo_apcfg_sta_ok_callback);
    }

    return 0;
}

void app_sta_connect(char *oob_ssid, char *connect_key)
{      
    network_InitTypeDef_st type;
    int len;

    os_memset(&type, 0x0, sizeof(network_InitTypeDef_st));
    len = os_strlen(oob_ssid);

    os_strcpy((char *)type.wifi_ssid, oob_ssid);
    os_strcpy((char *)type.wifi_key, connect_key);

    type.wifi_mode = BK_STATION;
    type.dhcp_mode = DHCP_CLIENT;
    type.wifi_retry_interval = 100;

    user_connected_callback(app_demo_apcfg_sta_ok_callback);
    rw_evt_set_callback(RW_EVT_STA_CONNECT_FAILED, app_demo_apcfg_rw_event_func);
    rw_evt_set_callback(RW_EVT_STA_DISCONNECTED, app_demo_apcfg_rw_event_func);

    os_printf("ssid:%s key:%s\r\n", type.wifi_ssid, type.wifi_key);
    bk_wlan_start(&type);
}

static void app_sta_connect_stop(void)
{
    user_connected_callback(NULL);
    rw_evt_set_callback(RW_EVT_STA_CONNECT_FAILED, NULL);

    bk_wlan_stop(BK_STATION);
}
void app_demo_apcfg_send_msg(DEMO_APCFG_MSG new_msg)
{
    OSStatus ret;
    DEMO_APCFG_MSG_T msg;

    if(demo_apcfg_msg_que)
    {
        msg.demo_apcfg_msg = new_msg;

        ret = rtos_push_to_queue(&demo_apcfg_msg_que, &msg, BEKEN_NO_WAIT);
        if(kNoErr != ret)
        {
            os_printf("app_demo_apcfg_send_msg failed\r\n");
        }
    }
}

static void app_demo_apcfg_timer_poll_handler(void)
{
    OSStatus err;

    // stop doing timer
    err = rtos_stop_timer(&demo_apcfgctr.demo_apcfg_timer);
    ASSERT(kNoErr == err);
    if(g_sendAck)
    {
        g_sendAck=0;
        app_demo_apcfg_send_msg(DEMO_APCFG_WLAN_LINK);
    }

    //app_video_intf_open();
}

static void app_demo_apcfg_timer_handler(void *data)
{
    app_demo_apcfg_send_msg(DEMO_APCFG_TIMER_POLL);
}

static void app_demo_apcfg_poll_handler(DEMO_APCFG_MSG msg)
{
    uint32_t intval = 0;
    OSStatus err;

    //if(key_apcfgctr.state == next_sta)
    //    return;

    err = rtos_stop_timer(&demo_apcfgctr.demo_apcfg_timer);
    ASSERT(kNoErr == err);

    switch(msg)
    {
    case DEMO_APCFG_STA_NONE:
        break;

 	case DEMO_APCFG_START:
            DEMO_APCFG_PRT("demo_apcfgctr.state=%d!!!!\r\n",demo_apcfgctr.state);
        if( demo_apcfgctr.state != DEMO_APCFG_AP_STATE)
        {
            DEMO_APCFG_PRT("DEMO_APCFG_START!!!!\r\n");

            app_ap_main_deinit();
            app_video_intf_close();
            app_sta_connect_stop();

            TZH_StopPPCS();		
	   // app_video_intf_close_p2p();
	   
	     	init_udphandle_cmd();
            demo_apcfgctr.state = DEMO_APCFG_AP_STATE;
            app_led_send_msg(MONITOR_MODE);		//chen
            user_main(NULL);
        }
        else
        {
            if(st_ssid[0]!=0)
            {
                DEMO_APCFG_PRT("Stop udp cfg!!!!\r\n");
		#if 0		
                app_ap_main_deinit();	
                app_demo_apcfg_send_msg(DEMO_APCFG_WLAN_LINK);
		#else
		rt_hw_cpu_reset();
		#endif
            }
        }
	    break;	
	
	case DEMO_APCFG_STOP:
        DEMO_APCFG_PRT("DEMO_APCFG_STOP!!!!\r\n");
        //app_video_intf_close();
        //app_sta_connect_stop();
	    break;
	
 	case DEMO_APCFG_RESET:
        #if(1)
        DEMO_APCFG_PRT("DEMO_APCFG_RESET!!!!\r\n");
/*		

        if(demo_apcfgctr.state ==DEMO_APCFG_AP_STATE)
        {
            app_demo_softap_send_msg(DAP_EXIT, 0);  
        }
 */       
        demo_apcfgctr.state = DEMO_APCFG_IDLE_STATE;

        os_memset(st_ssid, 0, sizeof(st_ssid));
        os_memset(st_pwd, 0, sizeof(st_pwd));

        os_memcpy(UserPara.ssid, st_ssid, 32);
        os_memcpy(UserPara.pwd, st_pwd, 32);
        app_drone_save_ssidkey();
		
	Init_userpsw();
	
        app_demo_apcfg_send_msg(DEMO_APCFG_START);
        rt_hw_cpu_reset();
	
        #else
        os_printf("using uri: " HTTP_OTA_URL "\n");
        app_led_send_msg(LED_OFF);		

        http_ota_fw_download(HTTP_OTA_URL, 0);
        app_ap_main_deinit();	
        app_demo_apcfg_send_msg(DEMO_APCFG_WLAN_LINK);
        #endif
        break;
        
    case DEMO_APCFG_REC_SSIDOK:
        err = rtos_change_period(&demo_apcfgctr.demo_apcfg_timer, 2);	// 1s
        ASSERT(kNoErr == err);
        DEMO_APCFG_PRT("REC ssid ok ,sending ACK to APP!!!\r\n");
        g_sendAck=1;
        break;	
    
	case DEMO_APCFG_WLAN_LINK:
        DEMO_APCFG_PRT("st_ssid=%s\r\n",st_ssid);
        DEMO_APCFG_PRT("st_pwd=%s\r\n",st_pwd);

        demo_apcfgctr.state = DEMO_APCFG_STA_STATE;
        app_led_send_msg(LED_DISCONNECT);		

        app_ap_main_deinit();
        app_sta_connect(st_ssid,st_pwd);
        //app_video_intf_open();
        break;
        
	case DEMO_APCFG_STA_OK:
        app_led_send_msg(LED_CONNECT);		

        // update system time, when connect to network   
        ntp_sync_to_rtc();

        if(demo_apcfgctr.state == DEMO_APCFG_STA_STATE)
        {
            demo_apcfgctr.state = DEMO_APCFG_STAOK_STATE;

            // start http ota timer
            //http_ota_alarm_start();
		    app_video_intf_open();	//chen
	
            app_video_intf_open_p2p();
            //app_audio_intf_open();
            if(dapcm_play_init(8000) == RT_EOK)
            {
		        Spk_Ctl(SPK_ON);//chen
                TZH_PPCS_RegisterPlayTalkCB(dapcm_play_write_data, 256);	//chen 2048 
            }

            // err = rtos_change_period(&demo_apcfgctr.demo_apcfg_timer, 4);	// 2s
            //ASSERT(kNoErr == err);
        }
        break;
        
    case DEMO_APCFG_STA_DISCON:
		        DEMO_APCFG_PRT("DEMO_APCFG_STA_DISCON!!!\r\n");
			if(demo_apcfgctr.state != DEMO_APCFG_AP_STATE)
				{
				app_led_send_msg(LED_DISCONNECT);	
				}
        		
        
        break;

    case DEMO_APCFG_HTTP_OTA:
        {
            time_t now;
            
            /* output current time */
            now = time(RT_NULL);
            os_printf("msg HTTP_OTA %s\r\n", ctime(&now));

            // do http ota
            //http_ota_fw_download(HTTP_OTA_URL, 1);
        }
        break;

    default:
        break;
    }
}

static void app_key_apcfg_main( beken_thread_arg_t data )
{
    OSStatus err;

    os_memset(&demo_apcfgctr, 0, sizeof(DEMO_APCFG_ST));
    demo_apcfgctr.state = DEMO_APCFG_IDLE_STATE;

    err = rtos_init_timer(&demo_apcfgctr.demo_apcfg_timer,
                          1 * 1000,
                          app_demo_apcfg_timer_handler,
                          (void *)0);
    ASSERT(kNoErr == err);

    err = rtos_start_timer(&demo_apcfgctr.demo_apcfg_timer);
    ASSERT(kNoErr == err);

    while(1)
    {
        DEMO_APCFG_MSG_T msg;
        err = rtos_pop_from_queue(&demo_apcfg_msg_que, &msg, BEKEN_WAIT_FOREVER);
        if(kNoErr == err)
        {
            switch(msg.demo_apcfg_msg)
            {
            case DEMO_APCFG_TIMER_POLL:
                app_demo_apcfg_timer_poll_handler();
                break;
            default:
                app_demo_apcfg_poll_handler(msg.demo_apcfg_msg);
                break;
            }
        }
    }

app_key_apcfg_exit:
    DEMO_APCFG_PRT("app_demo_apcfg_main exit\r\n");

    rtos_deinit_queue(&demo_apcfg_msg_que);
    demo_apcfg_msg_que = NULL;

    demo_apcfg_thread_handle = NULL;
    rtos_delete_thread(NULL);

}

UINT32 app_demo_apcfg_init(void)
{
    int ret;    

    DEMO_APCFG_PRT("app_demo_apcfg_init %d\r\n");
    if((!demo_apcfg_thread_handle) && (!demo_apcfg_msg_que))
    {
        ret = rtos_init_queue(&demo_apcfg_msg_que,
                              "key_apcfg_queue",
                              sizeof(DEMO_APCFG_MSG_T),
                              DEMO_APCFG_QITEM_COUNT);
        if(kNoErr != ret)
        {
            DEMO_APCFG_PRT("temp detect ceate queue failed\r\n");
            return kGeneralErr;
        }

        ret = rtos_create_thread(&demo_apcfg_thread_handle,
                                 BEKEN_DEFAULT_WORKER_PRIORITY,
                                 "app key_apcfg",
                                 (beken_thread_function_t)app_key_apcfg_main,
                                 4096, // 2048
                                 NULL);
        if (ret != kNoErr)
        {
            rtos_deinit_queue(&demo_apcfg_msg_que);
            demo_apcfg_msg_que = NULL;
            DEMO_APCFG_PRT("Error: Failed to create app_demo_apcfg_init: %d\r\n", ret);
            return kGeneralErr;
        }
    }

    return kNoErr;
}
#endif



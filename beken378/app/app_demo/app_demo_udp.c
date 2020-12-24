#include "include.h"
#include "app_demo_config.h"

#if (CFG_USE_APP_DEMO_VIDEO_TRANSFER && APP_DEMO_CFG_USE_UDP)
#include "FreeRTOS.h"
#include "task.h"
#include "rtos_pub.h"
#include "error.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"

#include "app_demo_udp.h"
#include "BkDriverUart.h"

#include "uart_pub.h"
#include "mem_pub.h"

#include "app_video_intf.h"
#include "app_demo_config.h"
#include "app_demo_softap.h"
#include "video_transfer.h"
#include "spidma_intf_pub.h"
#include "camera_intf_pub.h"
#include "app_demo_apcfg.h"


#define APP_DEMO_UDP_DEBUG              1
#if APP_DEMO_UDP_DEBUG
#define APP_DEMO_UDP_PRT                warning_prf
#define APP_DEMO_UDP_WARN               warning_prf
#define APP_DEMO_UDP_FATAL              fatal_prf
#else
#define APP_DEMO_UDP_PRT                null_prf
#define APP_DEMO_UDP_WARN               null_prf
#define APP_DEMO_UDP_FATAL              null_prf
#endif

void put_drone_cmd(UINT8 head,UINT8 index,UINT8 cmd1,UINT8 cmd2);
int app_lwip_udp_cmd_send_packet (UINT8 *data, UINT32 len);		//CHEN-UARTIO
extern UserTypedef UserPara;	//CHEN

#define APP_DEMO_UDP_RCV_BUF_LEN        1472
#define APP_DEMO_UDP_SOCKET_TIMEOUT     100  // ms

int app_demo_udp_img_fd = -1;
volatile int app_demo_udp_romote_connected = 0;
volatile int app_demo_udp_run = 0;
xTaskHandle app_demo_udp_hdl = NULL;
struct sockaddr_in *app_demo_remote = NULL;

struct sockaddr_in cmd_remote;	//chen
int udp_cmd_fd=-1;				//chen
UINT8 cmdbuf[8];

UDP_APCFG_PTR udp_apcfg = NULL;			//chen
char st_ssid[32];
char st_pwd[32];

#if APP_DEMO_EN_VOICE_TRANSFER
#include "app_jpeg2avi.h"
#include "voice_transfer.h"
int app_demo_udp_voice_fd = -1;
struct sockaddr_in *app_demo_udp_voice_remote = NULL;
volatile int app_demo_udp_voice_romote_connected = 0;
static char * g_pUdpAudioAdpcmBuf = NULL;

void app_demo_udp_audio_intf_close (void);
void app_demo_udp_audio_intf_open (void);
int app_demo_udp_voice_send_packet (UINT8 *data, UINT32 len);
#endif  // APP_DEMO_EN_VOICE_TRANSFER

extern void udp_handle_cmd_data(UINT8 *data, UINT16 len);

static void app_demo_udp_handle_cmd_data(UINT8 *data, UINT16 len)
{
    uint8_t crc_cal;
    uint32_t wrlen = 0;
    int i,  j,n,ssid_len,pwd_len;
	
if(data[0] == 0x55) 	
{
	udp_handle_cmd_data(data,len);
	return;
}
    
    if((len<8)||(len>70))
        return;
	
    if((data[0] == 0x66) && (data[len-1]== 0x99))
    {
        crc_cal=0;	
        for ( i=1; i<len-3; i++)
        {
            crc_cal = crc_cal^data[i];
        }
        // APP_DEMO_UDP_PRT("udp_handle_cmd_crc_cal=%d\r\n",crc_cal);

        os_memset(st_ssid, 0, sizeof(st_ssid));
        os_memset(st_pwd, 0, sizeof(st_pwd));

        j=0;
        for(i=1;i<len-3;i++)
        {
            if ((data[i]==0x23))//&&(data[i+1]=="#")&&(data[i+2]=="#"))
            {
                break;
            }
            else 
            {
                st_ssid[j]=data[i];
                j++;
            }
        }
        
        i=i+3;
        wrlen=len-j-6;
        j=0;
        
        for(j=0;j<wrlen;j++)
        {
            st_pwd[j]=data[i];
            i++;
        }

        APP_DEMO_UDP_PRT("st_ssid=%s\r\n",st_ssid);
        APP_DEMO_UDP_PRT("st_pwd=%s\r\n",st_pwd);

        os_memcpy(UserPara.ssid, st_ssid, 32);
        os_memcpy(UserPara.pwd, st_pwd, 32);
        app_drone_save_ssidkey();
        APP_DEMO_UDP_PRT("UserPara.ssid=%s\r\n",UserPara.ssid);
        APP_DEMO_UDP_PRT("UserPara.pwd=%s\r\n",UserPara.pwd);
        rtos_delay_milliseconds(10);								//delay 10ms
		
        put_drone_cmd(0x66,0,1,0);
        for (int i=0; i<5; i++)
        {
       	 app_lwip_udp_cmd_send_packet(cmdbuf,8);		
        }	
		
        app_demo_apcfg_send_msg(DEMO_APCFG_REC_SSIDOK);
		
	
#if 0
        extern void bk_send_byte(UINT8 uport, UINT8 data);
        
        for (int i=0; i<len; i++)
        {
            bk_send_byte(UART1_PORT, data[i]);
        }

#endif		
    }
		

/*    
    if((data[0] != CMD_HEADER_CODE) && (len != CMD_LEN) && (data[len-1] != CMD_TAIL_CODE))
        return;

    crc_cal = (data[1]^data[2]^data[3]^data[4]^data[5]);
    
    if(crc_cal != data[6]) {
        if(((crc_cal == CMD_HEADER_CODE) || (crc_cal == CMD_TAIL_CODE)) 
            && (crc_cal+1 == data[6]))
            // drop this paket for crc is the same with Header or Tailer
            return;
        else // change to right crc
            data[6] = crc_cal;
    }

    {
    extern void bk_send_byte(UINT8 uport, UINT8 data);
    for (int i=0; i<len; i++)
    {
        bk_send_byte(UART1_PORT, data[i]);
    }
    }
   */ 
}

static void app_demo_udp_app_connected(void)
{
    app_demo_softap_send_msg(DAP_APP_CONECTED, 0);
}

static void app_demo_udp_app_disconnected(void)
{
    app_demo_softap_send_msg(DAP_APP_DISCONECTED, 0);
}

#if CFG_SUPPORT_HTTP_OTA
TV_OTA_ST ota_param = 
{
    NULL,
    0,
    0
};
static void app_demo_udp_http_ota_handle(char *rev_data)
{

    if(app_demo_softap_is_ota_doing() == 0)
    {
        // to do
        //
        
        app_demo_softap_send_msg(DAP_START_OTA, &ota_param);

        os_memset(&ota_param, 0, sizeof(TV_OTA_ST));
    }
}
#endif

static void app_demo_udp_receiver(UINT8 *data, UINT32 len, struct sockaddr_in *app_demo_remote)
{
    GLOBAL_INT_DECLARATION();
    
    if(len < 2)
        return;
    
    if(data[0] == CMD_IMG_HEADER) {
        if(data[1] == CMD_START_IMG) {
 
            UINT8 *src_ipaddr = (char *)&app_demo_remote->sin_addr.s_addr;
            APP_DEMO_UDP_PRT("src_ipaddr: %d.%d.%d.%d\r\n", src_ipaddr[0], src_ipaddr[1],
                                                   src_ipaddr[2], src_ipaddr[3]);
            APP_DEMO_UDP_PRT("udp connect to new port:%d\r\n", app_demo_remote->sin_port);

            #if (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF)  
            // deinit camera first, for it may open by other application
            app_video_intf_close();               
            #endif

            GLOBAL_INT_DISABLE();
            app_demo_udp_romote_connected = 1;
            GLOBAL_INT_RESTORE();    

            #if (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF)
            TVIDEO_SETUP_DESC_ST setup;
            
            setup.send_type = TVIDEO_SND_UDP;
            setup.send_func = app_demo_udp_send_packet;
            setup.start_cb = app_demo_udp_app_connected;
            setup.end_cb = app_demo_udp_app_disconnected;

            setup.pkt_header_size = sizeof(HDR_ST);
            setup.add_pkt_header = app_demo_add_pkt_header;
            
            video_transfer_init(&setup);
            #endif
        }
        else if(data[1] == CMD_STOP_IMG) {
            APP_DEMO_UDP_PRT("udp close\r\n");

            #if (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF)  
            video_transfer_deinit();               
            #endif

            GLOBAL_INT_DISABLE();
            app_demo_udp_romote_connected = 0;
            GLOBAL_INT_RESTORE();

            #if (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF)  
            // init camera first, for it may opened by other application
            app_video_intf_open();               
            #endif
        }
        #if CFG_SUPPORT_HTTP_OTA
        else if(data[1] == CMD_START_OTA)
        {
            app_demo_udp_http_ota_handle(&data[2]);
        }
        #endif		
    }

}

#if APP_DEMO_EN_VOICE_TRANSFER
static void app_demo_udp_voice_receiver(UINT8 *data, UINT32 len, struct sockaddr_in *udp_remote)
{
    GLOBAL_INT_DECLARATION();
    
    if(len < 2)
        return;
    
    if(data[0] == CMD_VOICE_HEADER) {
        if(data[1] == CMD_VOICE_START) {
 
            UINT8 *src_ipaddr = (char *)&udp_remote->sin_addr.s_addr;

            APP_DEMO_UDP_PRT("voice transfer start\r\n");
            APP_DEMO_UDP_PRT("src_ipaddr: %d.%d.%d.%d\r\n", src_ipaddr[0], src_ipaddr[1],
                                                   src_ipaddr[2], src_ipaddr[3]);
            APP_DEMO_UDP_PRT("udp connect to new port:%d\r\n", udp_remote->sin_port);

            GLOBAL_INT_DISABLE();
            app_demo_udp_voice_romote_connected = 1;
            GLOBAL_INT_RESTORE(); 
		 tvoice_transfer_deinit();
		 
		app_demo_udp_audio_intf_open();
           // tvoice_transfer_init(app_demo_udp_voice_send_packet);
        }
        else if(data[1] == CMD_VOICE_STOP) {
            APP_DEMO_UDP_PRT("voice transfer stop\r\n");

            GLOBAL_INT_DISABLE();
            app_demo_udp_voice_romote_connected = 0;
            GLOBAL_INT_RESTORE(); 
			
		app_demo_udp_audio_intf_close();
		
           // tvoice_transfer_deinit();
        }
    }
}
#endif // APP_DEMO_EN_VOICE_TRANSFER
static void app_demo_udp_main( beken_thread_arg_t data )
{
    OSStatus err = kNoErr;
    GLOBAL_INT_DECLARATION();
    //chen   int maxfd, udp_cmd_fd, ret = 0;
    int maxfd, ret = 0;
    int snd_len = 0, rcv_len = 0;
    //chen   struct sockaddr_in cmd_remote;
    socklen_t srvaddr_len = 0;
    fd_set watchfd;
    struct timeval timeout;
    u8 *rcv_buf = NULL;

    APP_DEMO_UDP_FATAL("app_demo_udp_main entry\r\n");
    (void)(data);

    rcv_buf = (u8*) os_malloc((APP_DEMO_UDP_RCV_BUF_LEN + 1) * sizeof(u8));
    if(!rcv_buf) {
        APP_DEMO_UDP_PRT("udp os_malloc failed\r\n");
        goto app_udp_exit;
    }

    app_demo_remote = (struct sockaddr_in *)os_malloc(sizeof(struct sockaddr_in));
    if(!app_demo_remote) {
        APP_DEMO_UDP_PRT("udp os_malloc failed\r\n");
        goto app_udp_exit;
    }    

    // for data transfer
    app_demo_udp_img_fd = lwip_socket(AF_INET, SOCK_DGRAM, 0);
    if (app_demo_udp_img_fd == -1) {
        APP_DEMO_UDP_PRT("socket failed\r\n");
        goto app_udp_exit;
    }
 
    app_demo_remote->sin_family = AF_INET;
    app_demo_remote->sin_port = htons(APP_DEMO_UDP_IMG_PORT);
    app_demo_remote->sin_addr.s_addr = htonl(INADDR_ANY);

    srvaddr_len = (socklen_t)sizeof(struct sockaddr_in);
    if (lwip_bind(app_demo_udp_img_fd, (struct sockaddr *)app_demo_remote, srvaddr_len) == -1) {
        APP_DEMO_UDP_PRT("bind failed\r\n");
        goto app_udp_exit;
    }

    //  for recv uart cmd 
    udp_cmd_fd = lwip_socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_cmd_fd == -1) {
        APP_DEMO_UDP_PRT("socket failed\r\n");
        goto app_udp_exit;
    }
 
    cmd_remote.sin_family = AF_INET;
    cmd_remote.sin_port = htons(APP_DEMO_UDP_CMD_PORT);
    cmd_remote.sin_addr.s_addr = htonl(INADDR_ANY);

    if (lwip_bind(udp_cmd_fd, (struct sockaddr *)&cmd_remote, srvaddr_len) == -1) {
        APP_DEMO_UDP_PRT("bind failed\r\n");
        goto app_udp_exit;
    }

    maxfd = (udp_cmd_fd > app_demo_udp_img_fd)? udp_cmd_fd : app_demo_udp_img_fd;

    // for voice transfer
    #if APP_DEMO_EN_VOICE_TRANSFER
    app_demo_udp_voice_remote = (struct sockaddr_in *)os_malloc(sizeof(struct sockaddr_in));
    if(!app_demo_udp_voice_remote) {
        APP_DEMO_UDP_PRT("udp os_malloc failed\r\n");
        goto app_udp_exit;
    } 
    
    app_demo_udp_voice_fd = lwip_socket(AF_INET, SOCK_DGRAM, 0);
    if (app_demo_udp_voice_fd == -1) {
        APP_DEMO_UDP_PRT("vo socket failed\r\n");
        goto app_udp_exit;
    }
 
    app_demo_udp_voice_remote->sin_family = AF_INET;
    app_demo_udp_voice_remote->sin_port = htons(APP_DEMO_UDP_VOICE_PORT);
    app_demo_udp_voice_remote->sin_addr.s_addr = htonl(INADDR_ANY);

    srvaddr_len = (socklen_t)sizeof(struct sockaddr_in);
    if (lwip_bind(app_demo_udp_voice_fd, (struct sockaddr *)app_demo_udp_voice_remote, srvaddr_len) == -1) {
        APP_DEMO_UDP_PRT("bind failed\r\n");
        goto app_udp_exit;
    }
    maxfd = (maxfd > app_demo_udp_voice_fd)? maxfd : app_demo_udp_voice_fd;
    #endif // APP_DEMO_EN_VOICE_TRANSFER
    timeout.tv_sec = APP_DEMO_UDP_SOCKET_TIMEOUT / 1000;
    timeout.tv_usec = (APP_DEMO_UDP_SOCKET_TIMEOUT % 1000) * 1000;

    GLOBAL_INT_DISABLE();
    app_demo_udp_romote_connected = 0;
    app_demo_udp_run = 1;
    #if APP_DEMO_EN_VOICE_TRANSFER
    app_demo_udp_voice_romote_connected = 0;
    #endif
    GLOBAL_INT_RESTORE(); 

    while (app_demo_udp_run)
    {    
        FD_ZERO(&watchfd);
        FD_SET(app_demo_udp_img_fd, &watchfd);
        FD_SET(udp_cmd_fd, &watchfd);
        #if APP_DEMO_EN_VOICE_TRANSFER
        FD_SET(app_demo_udp_voice_fd, &watchfd);
        #endif
        
        ret = lwip_select(maxfd+1, &watchfd, NULL, NULL, &timeout);
        if (ret < 0) {
            APP_DEMO_UDP_PRT("select ret:%d\r\n", ret);
            break;
        } 
        else 
        {
            // is img fd, data transfer
            if(FD_ISSET(app_demo_udp_img_fd, &watchfd)) 
            { 
                rcv_len = lwip_recvfrom(app_demo_udp_img_fd, rcv_buf, APP_DEMO_UDP_RCV_BUF_LEN, 0,
                    (struct sockaddr *)app_demo_remote, &srvaddr_len);
                
                if(rcv_len <= 0) 
                {
                    // close this socket
                    APP_DEMO_UDP_PRT("recv close fd:%d\r\n", app_demo_udp_img_fd);
                    break;
                } 
                else 
                {
                    rcv_len = (rcv_len > APP_DEMO_UDP_RCV_BUF_LEN)? APP_DEMO_UDP_RCV_BUF_LEN: rcv_len;
                    rcv_buf[rcv_len] = 0;

                    app_demo_udp_receiver(rcv_buf, rcv_len, app_demo_remote);
                }
            }
            else if(FD_ISSET(udp_cmd_fd, &watchfd)) 
            {
                rcv_len = lwip_recvfrom(udp_cmd_fd, rcv_buf, APP_DEMO_UDP_RCV_BUF_LEN, 0,
                    (struct sockaddr *)&cmd_remote, &srvaddr_len);
                
                if(rcv_len <= 0) 
                {
                    // close this socket
                    APP_DEMO_UDP_PRT("recv close fd:%d\r\n", udp_cmd_fd);
                    break;
                } 
                else 
                {
                    rcv_len = (rcv_len > APP_DEMO_UDP_RCV_BUF_LEN)? APP_DEMO_UDP_RCV_BUF_LEN: rcv_len;
                    rcv_buf[rcv_len] = 0;
                    
                    app_demo_udp_handle_cmd_data(rcv_buf, rcv_len);
                }

            }
            #if APP_DEMO_EN_VOICE_TRANSFER
            if(FD_ISSET(app_demo_udp_voice_fd, &watchfd)) 
            { 
                rcv_len = lwip_recvfrom(app_demo_udp_voice_fd, rcv_buf, APP_DEMO_UDP_RCV_BUF_LEN, 0,
                    (struct sockaddr *)app_demo_udp_voice_remote, &srvaddr_len);
                
                if(rcv_len <= 0) 
                {
                    // close this socket
                    APP_DEMO_UDP_PRT("recv close fd:%d\r\n", app_demo_udp_voice_fd);
                    break;
                } 
                else 
                {
                    rcv_len = (rcv_len > APP_DEMO_UDP_RCV_BUF_LEN)? APP_DEMO_UDP_RCV_BUF_LEN: rcv_len;
                    rcv_buf[rcv_len] = 0;

                    app_demo_udp_voice_receiver(rcv_buf, rcv_len, app_demo_udp_voice_remote);
                }
            }
            #endif // APP_DEMO_EN_VOICE_TRANSFER
        }
    }
	
app_udp_exit:
    
    APP_DEMO_UDP_FATAL("app_demo_udp_main exit %d\r\n", app_demo_udp_run);

#if (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF)
	// delete for when softap use to cfg net for first time
	// camera intf close by finish net configuration
    //video_transfer_deinit();
#endif

    if(rcv_buf) {
        os_free(rcv_buf);
        rcv_buf = NULL;
    }

    #if APP_DEMO_EN_VOICE_TRANSFER
    if(app_demo_udp_voice_fd != -1) {
        lwip_close(app_demo_udp_voice_fd);
        app_demo_udp_voice_fd = -1;
    }
    
    if(app_demo_udp_voice_remote) {
        os_free(app_demo_udp_voice_remote);
        app_demo_udp_voice_remote = NULL;
    }
    #endif // APP_DEMO_EN_VOICE_TRANSFER
    if(app_demo_remote) {
        os_free(app_demo_remote);
        app_demo_remote = NULL;
    }
    
    if(app_demo_udp_img_fd != -1) {
        lwip_close(app_demo_udp_img_fd);
        app_demo_udp_img_fd = -1;
    }

    if(udp_cmd_fd != -1) {
        lwip_close(udp_cmd_fd);
        udp_cmd_fd = -1;
    }    

    GLOBAL_INT_DISABLE();
    app_demo_udp_romote_connected = 0;
    app_demo_udp_run = 0;
    #if APP_DEMO_EN_VOICE_TRANSFER
    app_demo_udp_voice_romote_connected = 0;
    #endif
    GLOBAL_INT_RESTORE(); 

    app_demo_udp_hdl = NULL;
    rtos_delete_thread(NULL);
}

UINT32 app_demo_udp_init(void)
{
    int ret;

    APP_DEMO_UDP_PRT("app_demo_udp_init\r\n");
    if(!app_demo_udp_hdl)
    {
        ret = rtos_create_thread(&app_demo_udp_hdl,
                                      4,
                                      "app_udp",
                                      (beken_thread_function_t)app_demo_udp_main,
                                      1024,
                                      (beken_thread_arg_t)NULL);
        if (ret != kNoErr)
        {
            APP_DEMO_UDP_PRT("Error: Failed to create spidma_intfer: %d\r\n", ret);
            return kGeneralErr;
        }
    }

    return kNoErr;
}

int app_demo_udp_send_packet (UINT8 *data, UINT32 len)
{
    int send_byte = 0;

    if(!app_demo_udp_romote_connected)
        return 0;

    send_byte = lwip_sendto(app_demo_udp_img_fd, data, len, MSG_DONTWAIT|MSG_MORE,
        (struct sockaddr *)app_demo_remote, sizeof(struct sockaddr_in));
    
    if (send_byte < 0) {
        /* err */
        //APP_DEMO_UDP_PRT("send return fd:%d\r\n", send_byte);
        send_byte = 0;
    }

	return send_byte;
}

int app_lwip_udp_cmd_send_packet (UINT8 *data, UINT32 len)		//CHEN-UARTIO
{
    int send_byte = 0;

    //if(!app_demo_udp_romote_connected)
    //    return 0;

    //cmd_remote.sin_family=AF_INET;
    //cmd_remote.sin_addr.s_addr=udp_remote->sin_addr.s_addr;

    //cmd_remote.sin_addr.s_addr = IPADDR_BROADCAST;
    //cmd_remote.sin_addr.s_addr =  inet_addr("192.168.4.100");
    //cmd_remote.sin_port= htons(40000);


    send_byte = lwip_sendto(udp_cmd_fd, data, len, MSG_DONTWAIT|MSG_MORE,
    (struct sockaddr *)&cmd_remote, sizeof(struct sockaddr_in));

    APP_DEMO_UDP_PRT("UDP CMD send return fd:%d\r\n", send_byte);	//CHEN

    if (send_byte < 0) {
        /* err */
        //LWIP_UDP_PRT("UDP CMD send return fd:%d\r\n", send_byte);	//CHEN
        send_byte = 0;
    }

    return send_byte;
}

void put_drone_cmd(UINT8 head,UINT8 index,UINT8 cmd1,UINT8 cmd2)
{
    cmdbuf[0]=head;	// 0x66, 0x55
    cmdbuf[1]=index;
    cmdbuf[2]=cmd1;
    cmdbuf[3]=cmd2;
    cmdbuf[4]=0;
    cmdbuf[5]=0;
    cmdbuf[6]=cmdbuf[2]^cmdbuf[3]^cmdbuf[4]^cmdbuf[5];
    cmdbuf[7]=0x99;
}

void get_cmd_remote(void)
{
    //UINT8 *src_addr;
    // UINT8 *src_ipaddr = (char *)&udp_remote->sin_addr.s_addr;
    //LWIP_UDP_PRT("src_ipaddr: %d.%d.%d.%d\r\n", src_ipaddr[0], src_ipaddr[1],
    // src_ipaddr[2], src_ipaddr[3]);

    UINT8 *src_ipaddr1 = (char *)&cmd_remote.sin_addr.s_addr;
    APP_DEMO_UDP_PRT("src_ipaddr: %d.%d.%d.%d\r\n", src_ipaddr1[0], src_ipaddr1[1],
    src_ipaddr1[2], src_ipaddr1[3]);

    APP_DEMO_UDP_PRT("CMD udp connect to new port:%d\r\n", cmd_remote.sin_port);  
}

#if APP_DEMO_EN_VOICE_TRANSFER
#if 0
int app_demo_udp_voice_send_packet (UINT8 *data, UINT32 len)
{
    int send_byte = 0;

    if(!app_demo_udp_voice_romote_connected)
        return 0;

    send_byte = lwip_sendto(app_demo_udp_voice_fd, data, len, MSG_DONTWAIT|MSG_MORE,
        (struct sockaddr *)app_demo_udp_voice_remote, sizeof(struct sockaddr_in));
    
    if (send_byte < 0) {
        /* err */
        //LWIP_UDP_PRT("send return fd:%d\r\n", send_byte);
        send_byte = 0;
    }

	return send_byte;
}
#endif

int app_demo_udp_voice_send_packet (UINT8 *data, UINT32 len)
{    int send_byte = 0;

#define TEST_PCM
	if(app_demo_udp_voice_romote_connected)//online    
	{
		UINT32 tx_len = 0;              
		
	#ifdef TEST_PCM         
		if(len > 1472)        //2048
		{            
			len = 1472;//avoid g_pAudioAdpcmBuf overflow        2048
		}        
		memcpy(g_pUdpAudioAdpcmBuf,data,len);        
		tx_len = len ;        
	#else        
      	int adpcmlen = 0;                
		adpcmlen = len/4;        
		ADPCM_EncodeData(0, data, len, g_pUdpAudioAdpcmBuf); 
		tx_len = adpcmlen;        
		//os_printf("tx_len=%d\r\n",tx_len);
	#endif
	
    		send_byte = lwip_sendto(app_demo_udp_voice_fd, g_pUdpAudioAdpcmBuf, tx_len, MSG_DONTWAIT|MSG_MORE,
        				(struct sockaddr *)app_demo_udp_voice_remote, sizeof(struct sockaddr_in));
	
	    if (send_byte < 0) {
        				send_byte = 0;
    					}

	}    
	else //offline    
	{    
		#if 0
		if(jpeg2avi_audio_format_get() == WAVE_FORMAT_PCM)         
		{            
			jpeg2avi_input_data(data,len,eTypeAudio);            
		}        
		else        
		{            
			os_printf("audio format error! 0x%04x@@@\r\n",jpeg2avi_audio_format_get());            
			return -1;        
		}    
		#endif
	}        
	return send_byte;
}

void app_demo_udp_audio_intf_open (void)
{
    os_printf("udp voice open\r\n"); 
    if(g_pUdpAudioAdpcmBuf != NULL)
    {
        os_printf("udp voice aready opened\r\n"); 
        return;
    }

    g_pUdpAudioAdpcmBuf = sdram_realloc(g_pUdpAudioAdpcmBuf, 2112);
    if(g_pUdpAudioAdpcmBuf == NULL)
    {
        os_printf("udp Audio malloc error\r\n");
        return;
    }
    
    tvoice_transfer_init(app_demo_udp_voice_send_packet);
}

void app_demo_udp_audio_intf_close (void)
{
    os_printf("udp voice close\r\n");
    if(g_pUdpAudioAdpcmBuf == NULL)
    {
        os_printf("voice aready closed\r\n"); 
        return;
    }

    tvoice_transfer_deinit();  
    
    sdram_free(g_pUdpAudioAdpcmBuf);
    g_pUdpAudioAdpcmBuf = NULL;
}



#endif //APP_DEMO_EN_VOICE_TRANSFER
void app_demo_udp_deinit(void)
{
    GLOBAL_INT_DECLARATION();

    APP_DEMO_UDP_PRT("app_demo_udp_deinit\r\n");
    if(app_demo_udp_run == 0)
        return;

    GLOBAL_INT_DISABLE();
    app_demo_udp_run = 0;
    GLOBAL_INT_RESTORE();

    while(app_demo_udp_hdl)
        rtos_delay_milliseconds(10);
}

void udp_video_stop(void)
{
         APP_DEMO_UDP_PRT("udp close\r\n");

         #if (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF)  
            //video_transfer_deinit();               
            #endif

          //  GLOBAL_INT_DISABLE();
           // 	app_demo_udp_romote_connected = 0;
           // 	GLOBAL_INT_RESTORE();

            	#if (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF)  
            	// init camera first, for it may opened by other application
            	app_video_intf_open();               
            	#endif							
}
#endif  // (CFG_USE_APP_DEMO_VIDEO_TRANSFER && APP_DEMO_CFG_USE_UDP)

// EOF


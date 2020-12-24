#include "include.h"

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
#include "net_param_pub.h"			
#include "wlan_ui_pub.h"			
#include "rw_pub.h"			       
#include "common.h"			       

#include "camera_intf_pub.h"
#include "motor_main.h"			        
#include "app_motor.h"			       
#include "video_transfer.h"
#include "udp_cmd.h"
#include "app_sd.h"

static uint8_t udpcmd_index;

extern UINT8 cmdbuf[8];

extern SD_HANDLE_RESULT sd_action_handle(void);
extern void set_sd_action(SD_ACTION act);
extern void put_drone_cmd(UINT8 head,UINT8 index,UINT8 cmd1,UINT8 cmd2);
extern int app_lwip_udp_cmd_send_packet (UINT8 *data, UINT32 len);	

void init_udphandle_cmd(void)
{
	udpcmd_index=0xFF;
}

void udp_handle_cmd_data(UINT8 *data, UINT16 len)
{    uint8_t crc_cal;
	uint8_t cmd,para;
    	int i;
    	uint32_t m,n;
SD_HANDLE_RESULT result;
	
	if((len<6)||(len>32))return;
	if((data[0] != 0x55)|| (data[len-1]!= 0x99))return;
      crc_cal=0;	
        for ( i=1; i<len-3; i++)
        {
            crc_cal = crc_cal^data[i];
        }
#if 1
			
    for (int i=0; i<len; i++)
    {
    
	os_printf("%02X ",data[i]);
    }
    	os_printf("\r\n");
       os_printf("==================\r\n");
	
#endif
	cmd=data[3];	 
	para=data[4];
	if(udpcmd_index==data[2])return;
	udpcmd_index=data[2];
    	os_printf("udp cmd=%d\r\n",cmd);

	switch(cmd)
		{
			case MSG_PIX_SW:
							if(para)
								{
									os_printf("CAMERA F-HID SET!!\r\n");
									m=VGA_640_480;
									n=TYPE_10FPS;
								}
							else
								{
									os_printf("CAMERA HID SET!!/r/n");
									m=QVGA_320_240;
									n=TYPE_10FPS;
								}
								video_transfer_set_video_param(m,n);								
							
			break;
			
			case MSG_VOICE_SW:
			break;
			
			case MSG_SD_SNAPSHOT:
			break;
			
			case MSG_SD_RECORD:
			break;
						
#ifdef CONFIG_IR_LED				
			case MSG_INFRADE_SW:
							if(para)
								{				
								IR_led_ctl(1);
								camera_intfer_set_BW(COLOUR_BW);
								os_printf("IR led on!!\r\n");

								}
							else
								{
								IR_led_ctl(0);
								camera_intfer_set_BW(COLOUR_NORMAL);
								os_printf("IR led off!!\r\n");
								
								}
								
			break;
#endif			
			case MSG_INSTRUCT_SW:
							if(para)
								{
								app_led_send_msg(MONITOR_MODE);
								os_printf("LED ON!!\r\n");
								}
							else
								{
								app_led_send_msg(LED_OFF);		
								os_printf("LED off!!\r\n");
							
								}							
				
			break;
			
			case MSG_LANDSCAPE_SW:
							os_printf("flip H!!\r\n");
							camera_flip(FLIP_MIRROR);
			break;
			
			case MSG_PORTRAIT_SW:
							os_printf("flip V!!\r\n");
							camera_flip(FLIP_UPDN);
			break;
			
			case MSG_TF_CHECK:
							os_printf("TF check!!\r\n");							
							extern 	int dfs_unmount(const char *specialfile);
							dfs_unmount("/sd");

							set_sd_action(SD_ACTION_ATTACH);
							result=sd_action_handle();
							os_printf("TF result=%d!!\r\n",result);
							
							if(result==1)os_printf("=====TF OK!!====\r\n");
							else if(result==2)os_printf("=====TF none!!====\r\n");
							else if(result==3)os_printf("=====TF ERROR!!====\r\n");

        						put_drone_cmd(0x55,0,MSG_TF_CHECK,result);	//result=1:ok,2=none TF 3=error
        						for (int i=0; i<5; i++)
        						{
       	 					app_lwip_udp_cmd_send_packet(cmdbuf,8);		
        						}	

			break;
				
			default:
			break;	
			
		}
	
}


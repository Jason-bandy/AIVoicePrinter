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
#include "motor_main.h"
#include "app_demo_did.h"			       

#define APP_DEMO_DID_DEBUG        1
#if APP_DEMO_DID_DEBUG
#define APP_DEMO_DID_PRT      os_printf
#define APP_DEMO_DID_WARN     warning_prf
#define APP_DEMO_DID_FATAL    fatal_prf
#else
#define APP_DEMO_DID_PRT      null_prf
#define APP_DEMO_DID_WARN     null_prf
#define APP_DEMO_DID_FATAL    null_prf
#endif
PPCSCFMTypedef PPCScfm;

void app_get_didstr(void)
{
    if(app_drone_get_did(&PPCScfm) != 0)
    {
        strcpy(PPCScfm.didStr, DEFAULT_DID_STR);
        strcpy(PPCScfm.didApilics, DEFAULT_DID_APILICENSE);

      //  save_info_item(DID_STR_ITEM, (UINT8 *)&PPCScfm.didStr, (UINT8 *)&PPCScfm.didApilics, NULL);

        os_printf("PPCScfm set default value\r\n");
    }
    
    os_printf("PPCScfm DID=%s\r\n",PPCScfm.didStr);
    os_printf("PPCScfm DID_APILICENSE=%s\r\n",PPCScfm.didApilics);
}

#if (CFG_SUPPORT_RTT) && (CFG_SUPPORT_TIANZHIHENG_DRONE)
void didcmd(int argc, char **argv)
{
    UINT16 len1,len2;

    if(0 == os_strcmp(argv[1], "write"))
    {
        os_memset(PPCScfm.didStr, 0, sizeof(PPCScfm.didStr));
        os_memset(PPCScfm.didApilics, 0, sizeof(PPCScfm.didApilics));

        len1=os_strlen(argv[2]);	
        len2=os_strlen(argv[3]);
        
        if((len1>32)||(len2>32))
            return;

        os_memcpy(PPCScfm.didStr, argv[2], len1);
        os_memcpy(PPCScfm.didApilics, argv[3], len2);
        if(app_drone_save_did(&PPCScfm) != 0)
        {
            APP_DEMO_DID_PRT("DID_failed=%s,%s\r\n",PPCScfm.didStr,PPCScfm.didApilics);
            return;
        }
        
        if(app_drone_get_did(&PPCScfm) == 0)
        {
            APP_DEMO_DID_PRT("DID_OK=%s,%s\r\n",PPCScfm.didStr,PPCScfm.didApilics);
        }

    }
    else if(0 == os_strcmp(argv[1], "read"))
    {
        if(app_drone_get_did(&PPCScfm) != 0)
        {
            APP_DEMO_DID_PRT("DID NULL!!\r\n");
        }
        APP_DEMO_DID_PRT("PPCScfm DID=%s\r\n",PPCScfm.didStr);
        APP_DEMO_DID_PRT("PPCScfm DID_APILICENSE=%s\r\n",PPCScfm.didApilics);
    }
    else
    {
        APP_DEMO_DID_PRT("didcmd write/read\r\n");
    }
}
FINSH_FUNCTION_EXPORT_ALIAS(didcmd, __cmd_did, didcmd);

#endif

USERPSWTypedef UserPswcfm;

void Init_userpsw(void)
{
       strcpy(UserPswcfm.CamName,DEFAULT_CAMERA_NAME);
        strcpy(UserPswcfm.UserName,DEFAULT_USER_NAME);
        strcpy(UserPswcfm.UserPsw,DEFAULT_USER_PSW);

        save_info_item(USER_PSW_ITEM, (UINT8 *)&UserPswcfm.CamName, (UINT8 *)&UserPswcfm.UserName,  (UINT8 *)&UserPswcfm.UserPsw);

        os_printf("UserPswcfm set default value\r\n");
}
void app_get_userpsw(void)
{
    if(!get_info_item(USER_PSW_ITEM, (UINT8 *)&UserPswcfm.CamName, (UINT8 *)&UserPswcfm.UserName,  (UINT8 *)&UserPswcfm.UserPsw))
    {
        strcpy(UserPswcfm.CamName,DEFAULT_CAMERA_NAME);
        strcpy(UserPswcfm.UserName,DEFAULT_USER_NAME);
        strcpy(UserPswcfm.UserPsw,DEFAULT_USER_PSW);

        save_info_item(USER_PSW_ITEM, (UINT8 *)&UserPswcfm.CamName, (UINT8 *)&UserPswcfm.UserName,  (UINT8 *)&UserPswcfm.UserPsw);

        os_printf("UserPswcfm set default value\r\n");
    }
    
    os_printf("Camera name=%s\r\n",UserPswcfm.CamName);
    os_printf("User name=%s\r\n",UserPswcfm.UserName);
    os_printf("User password=%s\r\n",UserPswcfm.UserPsw);
	
}

void app_save_userpsw(void)
{
        save_info_item(USER_PSW_ITEM, (UINT8 *)&UserPswcfm.CamName, (UINT8 *)&UserPswcfm.UserName,  (UINT8 *)&UserPswcfm.UserPsw);
    os_printf("save Camera name=%s\r\n",UserPswcfm.CamName);
    os_printf("save User name=%s\r\n",UserPswcfm.UserName);
    os_printf("save User password=%s\r\n",UserPswcfm.UserPsw);

}

/*
 * check_passwd - Check the user name and passwd against configuration.
 *
 * returns:
 *      0: Authentication failed.
 *      1: Authentication succeeded.
 */
int check_passwd(char *auser, int userlen, char *apasswd, int passwdlen) 
{
  int cfm_userlen;
  int cfm_passwdlen;

    //os_printf("APP_&user=%s,len=%d\r",auser,userlen);
    //os_printf("APP_&pwd=%s,len=%d\r\n",apasswd,passwdlen);

    	cfm_userlen = (int)strlen(UserPswcfm.UserName);
    	cfm_passwdlen = (int)strlen(UserPswcfm.UserPsw);
		
    //os_printf("old User name=%s,len=%d\r",UserPswcfm.UserName,cfm_userlen);
    //os_printf("old User password=%s,len=%d\r\n",UserPswcfm.UserPsw,cfm_passwdlen);
		
    if (cfm_userlen == userlen
        && cfm_passwdlen == passwdlen
        && !memcmp(auser, UserPswcfm.UserName, userlen)
        && !memcmp(apasswd, UserPswcfm.UserPsw, passwdlen) )
        {	os_printf("compare succeed!!!\r\n");
      		return 1;
  	 }
	
	os_printf("compare faild!!!\r\n");
  	return 0;
}



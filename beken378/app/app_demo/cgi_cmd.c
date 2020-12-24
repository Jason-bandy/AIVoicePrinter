#include <stdio.h>
#include "include.h"
#include "netdb.h"
#include "PPCS_API.h"
#include "task.h"
#include "error.h"
#include "rtos_pub.h"

#include "uart_pub.h"
#include "mem_pub.h"
#include "str_pub.h"
#include "app_demo_did.h"			       

#include <tzh_ppcs_api.h>
#include <cgi_cmd.h>
#include <motor_main.h>	
#include <app_motor.h>		       

extern  char* http_ota_get_fw_vision(void);
int CGI_GetStatus(char *output,char *input,unsigned char auth);
int CGI_GetRecordFile(char *output,char *input,unsigned char auth);
int CGI_Reboot(char *output,char *input,unsigned char auth);
int CGI_DecoderControl(char *output,char *input,unsigned char auth);
int CGI_RestoreFactory(char *output,char *input,unsigned char auth);
int CGI_SetFormatSd(char *output,char *input,unsigned char auth);
int CGI_UpgradeFirmWare(char *output,char *input,unsigned char auth);
int CGI_SetApConfig(char *output,char *input,unsigned char auth);
int CGI_Set_Alam(char *output,char *input,unsigned char auth);	//TEST!!!
int CGI_Audiostream(char *output,char *input,unsigned char auth);
int CGI_ReName(char *output,char *input,unsigned char auth);
int CGI_ReSetPsw(char *output,char *input,unsigned char auth);

CGIWEBCMD gCgiWebCmd[]={
	//get cgi
	{0x0001,"get_status.cgi",					CGI_GetStatus,				CGI_GET_STATUS},
	{0x0002,"get_record_file.cgi",				CGI_GetRecordFile,			CGI_GET_RECORD_FILE},
	{0x0003,"reboot.cgi",						CGI_Reboot,					CGI_REBOOT},
	{0x0005,"decoder_control.cgi",				CGI_DecoderControl,			CGI_DECODER_CONTROL},
	{0x0006,"restore_factory.cgi",				CGI_RestoreFactory,			CGI_RESTORE_FACTORY},
	{0x0008,"set_formatsd.cgi",					CGI_SetFormatSd,			CGI_FORMAT_SD},
	{0x0009,"set_ap_config.cgi",					CGI_SetApConfig,			CGI_SET_AP_CONFIG},
    {0x0010,"upgrade_firmware.cgi",				CGI_UpgradeFirmWare,		CGI_UPGRADE_FIRMWARE},
    {0x0011,"set_alarm_config.cgi",				CGI_Set_Alam,				CGI_SET_ALAM},
    {0x0012,"rename.cgi",						CGI_ReName,				    CGI_SET_RENAME},
    {0x0013,"reset_passwd.cgi",					CGI_ReSetPsw,				CGI_SET_RESETPSW},
    //cgi end
    {0xffff,"",NULL}
};

int CGI_GetStatus(char *output,char *input,unsigned char auth)
{
	int 			ret = 0;
	char 			factorystatus = 0;
	int				status;
	unsigned int 	sdtotal = 0;
	unsigned int 	sdfree = 0;

	ret += UTILS_WriteStr(output+ret,"deviceid",TZH_PPCS_GetDIDString());
    /* get sdtotal size and sdfree size here */
	ret += UTILS_WriteInt(output+ret,"sdtotal",sdtotal);
	ret += UTILS_WriteInt(output+ret,"sdfree",sdfree);
	return ret;
}


int CGI_GetRecordFile(char *output,char *input,unsigned char auth)
{
	int 		ret;
	int 		startdate = 0;
	int 		enddate = 0;
	int			value = 0;
	int			querybydateflag=0;
    int         pagesize=0;
    int         pageindex=0;
    int         recordcount=0;
    int         pagecount=0;
    int         recordnum = 0;
	static char listflag = 0;
	printf("input %s\n",input);

	ret = UTILS_GetKeyInt(input,"startdate",&value); 
	if (ret == 0x00)
    {
		startdate = value;		
		querybydateflag |= 0x01;
		//printf("start_date=%d\n",startdate);
	}
	ret = UTILS_GetKeyInt(input,"enddate",&value);	
	if (ret == 0x00)
    {
		enddate = value;		
		querybydateflag |= 0x02;
		//printf("end_date=%d\n",enddate);
	}
    ret = UTILS_GetKeyInt(input,"PageIndex",&value); 
	if (ret == 0x00)
    {
		pageindex = value;		
		//printf("PageIndex=%d\n",pageindex);
	}
	ret = UTILS_GetKeyInt(input,"PageSize",&value);	
	if (ret == 0x00)
    {
		pagesize = value;		
		//printf("PageSize=%d\n",pagesize);
	}
	ret = 0;
	ret += sprintf( output + ret, "var record_name0=new Array();\r\n" );
	ret += sprintf( output + ret, "var record_size0=new Array();\r\n" );
    /* XXX: do get record info, recordcount, pagecount,recordnum HERE */
    ret += UTILS_WriteInt(output+ret,"record_num0",recordnum);
    ret += UTILS_WriteInt(output+ret,"RecordCount",recordcount);
    ret += UTILS_WriteInt(output+ret,"PageCount",pagecount);
    
	if (listflag == 0x00)
    {
		listflag++;
		if (querybydateflag == 3)
		{
            /* do find record file by startdate and enddate HERE*/
			//ret += Record_GetFileByDate(output+ret,startdate,enddate,pageindex,pagesize);
		}
		listflag = 0x00;
	}	
    //printf("output :\n%s\n",output);
	return ret;
}


int CGI_Reboot(char *output,char *input,unsigned char auth)
{
    int ret = 0;
    
    printf("input:%s\n",input);
    /* do reboot HERE */	
    return ret;
}

int CGI_DecoderControl(char *output,char *input,unsigned char auth)
{
	int	ret = 0,ret1;
	int	cmd = 0;
	int	value = 0;
	int	onestep = 0;
	
	int 	namelen=0;
	int 	pswlen=0;
	char	namebuf[64];
	char	pswbuf[32];

	os_printf("input:%s\n",input);
	ret =  UTILS_GetKeyInt(input,"command",&cmd);
	ret += UTILS_GetKeyInt(input,"onestep",&onestep);
	
	ret1 = UTILS_GetKeyInt(input,"sit",&value);
	ret1+=UTILS_GetKeyStr(input,"user",namebuf,MAX_USERNAME_LEN);
	ret1+=UTILS_GetKeyStr(input,"pwd",pswbuf,MAX_USERPSW_LEN);

	namelen= (int)strlen(namebuf);	
	pswlen= (int)strlen(pswbuf);	
	
   	 os_printf("namebuf:%s,len=%d\r\n",namebuf,namelen);
    	os_printf("pswbuf:%s,len=%d\r\n",pswbuf,pswlen);
		
	check_passwd(namebuf,namelen,pswbuf,pswlen);

	if(ret  == 0x00 )
	{
		/* Do Moto run here  */
		ret=App_Motor_Ctl(output,cmd, onestep);
	}
	return ret;
}

int CGI_SetFormatSd(char *output,char *input,unsigned char auth)
{
	int	ret=0;
	printf("input:%s\n",input);
    /* do format sd here */
	rtos_thread_sleep(2);
	return ret;
}

int CGI_RestoreFactory(char *output,char *input,unsigned char auth)
{
	int ret = 0;
	printf("input:%s\n",input);
    /* do restore factory here */

	return ret;
}

int CGI_UpgradeFirmWare(char *output,char *input,unsigned char auth)
{
	int ret = 0;

	printf("input:%s\n",input);
    /* do upgrade firmware here */
	return ret;
}

int CGI_SetApConfig(char *output,char *input,unsigned char auth)
{

	int	ret = 0;
	int	value = 0;
	char	keyvalue[CGI_MAX_LEN];
	int  bSaveFlag = 0;
	printf("######input:%s\n",input);
	memset(keyvalue, 0x0, CGI_MAX_LEN);
	ret = UTILS_GetKeyStr(input,"ssid",keyvalue,32);
    if(ret == 0)
    {
        /* compare ssid, if different then update it */
    #if 0
    	if(strcmp(keyvalue,pNetDev->szSSID) != 0)
    	{
    		memset(pNetDev->szSSID,0, sizeof(pNetDev->szSSID));
			strncpy(pNetDev->szSSID,keyvalue,32);
			bSaveFlag++;
    	}
    #endif    
    }
	ret = UTILS_GetKeyStr(input,"wpa_psk",keyvalue,32);
    if(ret == 0x00)
    {
        /* compare wpa_psk, if different then update it */
    #if 0
        if(strcmp(keyvalue,pNetDev->szKey) != 0)
        {
        	memset(pNetDev->szKey,0,sizeof(pNetDev->szKey));
            strncpy(pNetDev->szKey,keyvalue,32);
			bSaveFlag++;
        } 
   #endif
    }
	ret = UTILS_GetKeyInt(input,"now",&value);
	if(ret == 0x00)
	{
		printf("LOC time now:%d",value);
	    /* Set LOCAL time HERE, not UTC */
	}
	if(bSaveFlag)
	{
		/* SAVE PARAMS HERE */
	}
	
	ret = UTILS_WriteInt(output,"result",0);
    printf("output :\n%s\n",output);
    return ret;
}

int CGI_Set_Alam(char *output,char *input,unsigned char auth)
{
    int ret = 0;
    printf("input:%s\n",input);
    /* do reboot HERE */	
    printf("SET ALAM Test!!\r\n");//chen

	return ret;
}

int CGI_ReName(char *output,char *input,unsigned char auth)
{
    	int ret = 0;
	int value = 0;
	int  bSaveFlag = 0;	
	char	namebuf[32];
	char	pswbuf[32];
	
    	printf("input:%s\n",input);

	memset(namebuf, 0x00, 32);
	memset(pswbuf, 0x00, 32);
		
	ret = UTILS_GetKeyStr(input,"newuser",namebuf,32);
	ret +=UTILS_GetKeyStr(input,"newpas",pswbuf,32);

    	if(ret == 0)
    	{
    		bSaveFlag++;
         	memset(UserPswcfm.UserName,0,sizeof(UserPswcfm.UserName));
            	strncpy(UserPswcfm.UserName,namebuf,32);		
				
             	memset(UserPswcfm.UserPsw,0,sizeof(UserPswcfm.UserPsw));
            	strncpy(UserPswcfm.UserPsw,pswbuf,32);
	
    	}

	if(bSaveFlag)
		{
		app_save_userpsw();
		}
	
	ret = UTILS_WriteInt(output,"result",0);
    	printf("output :\n%s\n",output);
	return ret;
}

int CGI_ReSetPsw(char *output,char *input,unsigned char auth)
{
    	int ret = 0;
	int value = 0;
	int  bSaveFlag = 0;	
	char	namebuf[32];
	char	pswbuf[32];
	
    	printf("input:%s\n",input);

	memset(pswbuf, 0x00, 32);
		
	
	ret = UTILS_GetKeyStr(input,"newpas",pswbuf,8);
    	if(ret == 0)
    	{
    		bSaveFlag++;
          	memset(UserPswcfm.UserPsw,0,sizeof(UserPswcfm.UserPsw));
            	strncpy(UserPswcfm.UserPsw,pswbuf,8);
  
    	}	
	if(bSaveFlag)
		{
		app_save_userpsw();
		}
	
	ret = UTILS_WriteInt(output,"result",0);
    	printf("output :\n%s\n",output);
	return ret;
}

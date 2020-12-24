#ifndef __CGI_CMD_H__
#define  __CGI_CMD_H__

#define CGI_GET_STATUS              0x6001
#define CGI_GET_RECORD_FILE         0x6007
#define CGI_DECODER_CONTROL		    0x6019

#define CGI_CAM_CONTROL             0x6012
#define CGI_REBOOT                  0x6027
#define CGI_FORMAT_SD               0x6028
#define CGI_SET_WIFISCAN            0x602a
#define CGI_RESTORE_FACTORY         0x602b

#define CGI_UPGRADE_FIRMWARE        0x6030
#define CGI_SET_AP_CONFIG           0x6036

#define CGI_SET_ALAM                0x6040	//TEST!!
#define	CGI_AUDIOT_STREAM           0x6050

#define	CGI_SET_RENAME		        0x6100
#define	CGI_SET_RESETPSW		    0x6101

typedef struct _CGIWEBCMD{
	int		cmdindex;												//cgi command index
	char		key[32];													//cgi command string
	int		(*callback)(char *output,char *input,unsigned char auth);		//cgi calllback
	short	p2pcmd;
	short	other;
}CGIWEBCMD,*PCGIWEBCMD;

#endif

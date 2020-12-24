#ifndef __APP_DID_H__
#define __APP_DID_H__

//#define	DEFAULT_DID_STR	 		 "THPC-000026-CCSDR" 
//#define	DEFAULT_DID_APILICENSE	 "LILRUH"	

#define	DEFAULT_DID_STR	 		 "THPC-000057-JFGKM" 
#define	DEFAULT_DID_APILICENSE	 "TTKICC"	

#define	DEFAULT_CAMERA_NAME	 	"Camera" 
#define	DEFAULT_USER_NAME	 	"admin"	
#if (CUS_SERVER==CUS_SERV_CY)
#define	DEFAULT_USER_PSW	 		"6666"	
#else
#define	DEFAULT_USER_PSW	 		"admin"	
#endif
#define	MAX_CAMNAME_LEN	32
#define	MAX_USERNAME_LEN	32
#define	MAX_USERPSW_LEN	32

typedef  struct  				
{
    char didStr[32];
    char didApilics[32];
} PPCSCFMTypedef;

typedef  struct  				
{
    char CamName[MAX_CAMNAME_LEN];			//Camera alias
    char UserName[MAX_USERNAME_LEN];
    char UserPsw[MAX_USERPSW_LEN];
} USERPSWTypedef;

extern PPCSCFMTypedef PPCScfm;
extern USERPSWTypedef UserPswcfm;

void app_get_didstr(void);
void app_get_userpsw(void);
void app_save_userpsw(void);
int check_passwd(char *auser, int userlen, char *apasswd, int passwdlen);
void Init_userpsw(void);

#endif


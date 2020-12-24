#include "include.h"

#include "FreeRTOS.h"
#include "task.h"
#include "rtos_pub.h"
#include "error.h"

#include "uart_pub.h"
#include "mem_pub.h"
#include "str_pub.h"
#include <sys/time.h>
#include "board.h"
#include "app_video_intf.h"

#include "app_demo_config.h"
#include "app_audio_intf.h"

#include "app_jpeg2avi.h"
#include "video_transfer.h"
#include "tzh_ppcs_api.h"
#include "app_demo_did.h"
#include <string.h>
#include "app_sd.h"
#include "motor_main.h"

#define FILE_PIC_NUM (10*60*2)/*4 minute pictures per file ,frame rate :10f/s*/
#define FILE_WRITE_ONGOING 1
#define FILE_WRITE_END 2

#define FULL_FILENAME_PAHT_LEN 24
#define NTP_TIMEOUT_MAX 	600/*600 frame pictures*/

#define	SD_STATUS_UNKNOWN 	0
#define	SD_STATUS_OK 			1
#define	SD_STATUS_BAD 		2
static uint8_t production_item_sd_check=SD_STATUS_UNKNOWN;

extern void app_dac_play_write_data(unsigned char * buffer,int buffer_bytes);
extern int app_dac_play_init(int sample_rate,int n_channel);

#if(0)
#define RTOS_PPCS_DID_STRING            "RTOS-000615-VVYSC"
#define RTOS_PPCS_DID_APILICENSE        "SWBCUL"
#endif

#if(1)
#define RTOS_PPCS_DID_STRING           "RTOS-000616-PKNGJ" 
#define RTOS_PPCS_DID_APILICENSE        "LFNLUX"
#endif

#if(0)
#define RTOS_PPCS_DID_STRING           "RTOS-000665-MSDNR" 
#define RTOS_PPCS_DID_APILICENSE        "SKNXZE"
#endif

#if(0)
#define RTOS_PPCS_DID_STRING           "RTOS-000666-RBGYG" 
#define RTOS_PPCS_DID_APILICENSE        "TVFDSV"
#endif

#if(0)
#define RTOS_PPCS_DID_STRING           "RTOS-000667-MHZYD" 
#define RTOS_PPCS_DID_APILICENSE        "FGUFQG"
#endif

#if(0)
#define RTOS_PPCS_DID_STRING           "RTOS-000668-RVZDJ" 
#define RTOS_PPCS_DID_APILICENSE        "VHYAGK"
#endif

#if(0)
#define RTOS_PPCS_DID_STRING           "RTOS-000669-KYXHK" 
#define RTOS_PPCS_DID_APILICENSE        "VUIBHF"
#endif

#if(0)
#define RTOS_PPCS_DID_STRING           "RTOS-000670-BTPPP" 
#define RTOS_PPCS_DID_APILICENSE        "SAYNCF"
#endif

#if(0)
#define RTOS_PPCS_DID_STRING           "RTOS-000671-RGUMF" 
#define RTOS_PPCS_DID_APILICENSE        "FYXFDE"
#endif

#if 0
#define RTOS_PPCS_SERVER_INIT_STRING    "EFGBFFBJKEJKGGJJEEGFFHELHHNNHONHGLFNBHCCAEJDLNLPDDAGCIOFGDLGJMLAAOMOKCDLOONOBICJJIMM"
#define RTOS_PPCS_DID_CRCKEY            "EasyView"
#endif

#if (CUS_SERVER==CUS_SERV_TZH)
//PPCS_SERVER_INIT_STRING+P2PKEY
#define RTOS_PPCS_SERVER_INIT_STRING    "EBGJFNBCKJJLGHJAEBHLFGEIGAJFGPJCHCBAAFGBAHIPOFLHGFFDDJODCKKFNGPIBDICKJGEPDICFICOMCMKJOEIJIPJECCLAN:hyWHzyhtzH" 
#define RTOS_PPCS_DID_CRCKEY            "tzhHYZb"
#elif (CUS_SERVER==CUS_SERV_JXL)
//PPCS_SERVER_INIT_STRING+P2PKEY
#define RTOS_PPCS_SERVER_INIT_STRING    "EBGDEJBJKGJLGHJIEJGMFOEGHFNPDINDHDABAEGJAFNJOJLKHLAKDMKNHCLMMILCFIMIPAHEPPICADCN:@@@@...." 
#define RTOS_PPCS_DID_CRCKEY            "SZSJXL"
#elif (CUS_SERVER==CUS_SERV_CY)
#define RTOS_PPCS_SERVER_INIT_STRING    "EDHNFGBILOINGGIGFDHHFKFICMJIHKNAHAAABJGPAOMOLJOACDEKCAKGGDKNNALNFGIOLMCELGJLEBCLMLMEMLALJMPNEICBFLCLHOFMMFKGFNGB:camera" 
#define RTOS_PPCS_DID_CRCKEY            "camera"

#elif (CUS_SERVER==CUS_SERV_XSJ)
#define RTOS_PPCS_SERVER_INIT_STRING    "EEGDFHBIKBJJGFJKEGGBFDEMHJMOHAIHGBBAACCNFNIIPMLCHFBCGAPHDCPMIBOMAHNPONCGLGMBEGGKJGJNJBEDNILJAFGCAHCLHCBBIGKG:xA@15WS92-sj" 
#define RTOS_PPCS_DID_CRCKEY            "XA-368a@"

#endif

#define VI_BUF_LEN                      (50*1024)
#define VI_MALLOC                       sdram_malloc
#define VI_FREE                         sdram_free

#if 1
typedef struct _JPEG_BUF_INFO_
{
    beken_thread_t pthread;
    char *pJpegBuf;
    int run;
    int p2p_need;
    int record_need;
    int serialno;
}JPEG_BUF_INFO;

static JPEG_BUF_INFO jbuf_info = {0};

static void app_video_add_p2p_header(char * pBuf,int nBufSize)
{
    struct timeval tv;
    gettimeofday( &tv, NULL );
    STREAMHEAD * phead      = (STREAMHEAD *)pBuf;
    phead->startcode        = htolong(STREAM_START_CODE);
    phead->type             = 3;
    phead->type1            = 0;
    phead->len              = htolong(nBufSize);
    phead->streamid         = 0x00;
    phead->frameno          = htolong(jbuf_info.serialno++);
    phead->militime         = htonet(tv.tv_usec / 1000);
    phead->sectime          = htolong(tv.tv_sec);
    phead->resolution       = 0;//0-VIDEO_HDVGA; 1-QVGA 2-HD720P
    phead->currsit          = 0;
    phead->byzone           = 0;
}


uint8_t app_video_production_sd_is_ok(void)
{
	return production_item_sd_check;
}

void app_video_intf_main( void* arg )
{
	int write_pic_num = 0;
	int file_opera_phase = FILE_WRITE_END;
	int ret;
	SD_HANDLE_RESULT result;
	char path_filename[FULL_FILENAME_PAHT_LEN];
	int ntp_timeout_cnt = 0;
    uint8_t bp2p_flag = 0;
	
    os_printf("video_intf_main\r\n");

    jbuf_info.pJpegBuf = VI_MALLOC(VI_BUF_LEN *sizeof(char));
    if(jbuf_info.pJpegBuf == NULL)
    {
        os_printf("JPEG malloc error\r\n");
        goto main_exit;
    }
    
#if (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF) 
    if(video_buffer_open() == 0)
    {
        os_printf("video_buffer_open error\r\n");
        goto main_exit;
    }
#endif

	if(1 != jpeg2avi_init())
		goto main_exit;
	
	if(-1 == sd_check_init())
		goto main_exit;	
	
    jbuf_info.run = 1;
	jbuf_info.record_need = 0;
	jbuf_info.p2p_need = 0;
	
	while(jbuf_info.run)
	{
    	write_pic_num = 0;
		if(	(FILE_WRITE_END == file_opera_phase)&&(1 == jbuf_info.record_need)&&(0 == jbuf_info.p2p_need) )
		{
			ret = createfile_pre_handle(jbuf_info.pJpegBuf);
			if(0 == ret)
			{
				ret = get_filename_handle(path_filename,FULL_FILENAME_PAHT_LEN,ntp_timeout_cnt,NTP_TIMEOUT_MAX);
				if(0 == ret)
				{
					ret = app_video_intf_open_record(path_filename);
					if(0 == ret)
					{
						file_opera_phase = FILE_WRITE_ONGOING;
						os_printf("==================create file ok========================\r\n");
					}
					else
					{
						os_printf("==create file error!!==");
						jbuf_info.record_need = 0;//stop record
					}
				}
			}

			if(-1 == ret)
			{
				jbuf_info.record_need = 0;//stop record
			}
		}
		
	    while(jbuf_info.run)
	    {
            jbuf_info.p2p_need = TZH_PPCS_GetLiveFlag();

            /*end cuurent record if currently recording and switch to p2p*/
            if(jbuf_info.record_need && (FILE_WRITE_ONGOING == file_opera_phase) && jbuf_info.p2p_need)
            {
                jpeg2avi_stop_record();
                file_opera_phase = FILE_WRITE_END;
                write_pic_num = 0;
                os_printf("==========end current local record===================\r\n");
                break;
            }

            /*end p2p by jbuf_info.p2p_need == 0 if current p2p online watch*/
            if(jbuf_info.p2p_need == 0 && bp2p_flag == 1)
            {
                bp2p_flag = 0;
		        video_transfer_set_video_param(QVGA_320_240,TYPE_10FPS);	//for avi record!!		
		
                os_printf("===========end online p2p==============\r\n");
				
                break;
            }
	        UINT8 *buf = jbuf_info.pJpegBuf + sizeof(STREAMHEAD);
	        UINT32 read_len;
	        
	        read_len = video_buffer_read_frame(buf, VI_BUF_LEN - sizeof(STREAMHEAD));
			if(ntp_timeout_cnt < NTP_TIMEOUT_MAX)
			{
				ntp_timeout_cnt ++;
			}
			
			
	        // for p2p 
	        if(jbuf_info.p2p_need)
	        {
	            app_video_add_p2p_header(jbuf_info.pJpegBuf, read_len);
	            PPCS_SendVideoData(jbuf_info.pJpegBuf, read_len + sizeof(STREAMHEAD));
	            bp2p_flag = 1;
	        }
			else
			{
				result = sd_action_handle();
				if(SD_NO_CHANGE != result)
				{
					if(SD_MOUNT_OK == result)
					{
						jbuf_info.record_need = 1;
						production_item_sd_check=SD_STATUS_OK;
					}
					else
					{
						jbuf_info.record_need = 0;
						if(FILE_WRITE_ONGOING == file_opera_phase)
						{
							jpeg2avi_stop_record();
							file_opera_phase = FILE_WRITE_END;
						}
						production_item_sd_check=SD_STATUS_BAD;

					}
					break;
				}
		        // for record
		        if(jbuf_info.record_need)
		        {
		        	if(FILE_WRITE_ONGOING == file_opera_phase)
			        {
			            read_len = jpeg2avi_input_data(buf, read_len, eTypeVideo);
						if((-1 != read_len)&&(++write_pic_num < FILE_PIC_NUM))
						{
							continue;
						}
						
						jpeg2avi_stop_record();
						file_opera_phase = FILE_WRITE_END;
						if(-1 == read_len)
						{
							jbuf_info.record_need = 0;
							os_printf("video write sd err, close record\r\n");
							msh_exec("free",strlen("free"));
							os_printf("\r\n");
						}
						else
						{
							os_printf("write file end\r\n");
						}
			        }
					break;
		        }
			}
	    }
	}
	if(FILE_WRITE_ONGOING == file_opera_phase)
	{
		jpeg2avi_stop_record();
	}
#if (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF)  
    video_buffer_close(); 
#endif

    app_video_intf_close_p2p();
    app_video_intf_close_record();
    
main_exit:

    os_printf("video_intf_main exit\r\n");
    
    if(jbuf_info.pJpegBuf)
    {
        VI_FREE(jbuf_info.pJpegBuf);
        jbuf_info.pJpegBuf = NULL;
    }

    jbuf_info.pthread = NULL;
    rtos_delete_thread(NULL);
}


void app_video_intf_open (void)
{
    int ret;
    
    os_printf("video open\r\n");

    if(jbuf_info.pthread) 
    {
        os_printf("voide open aready DONE\r\n");
        return;
    }

    os_memset(&jbuf_info, 0, sizeof(JPEG_BUF_INFO));

    ret = rtos_create_thread(&jbuf_info.pthread,
                                      5,
                                      "v_intf",
                                      (beken_thread_function_t)app_video_intf_main,
                                      1024,
                                      (beken_thread_arg_t)0);
    if (ret != kNoErr)
    {
        os_printf("Error: video_intf_open: %d\r\n", ret);
        return;
    }
    
    //os_printf("voide open DONE\r\n");
}

void app_video_intf_close(void)
{
    GLOBAL_INT_DECLARATION();
    
    os_printf("video close\r\n");

    if(jbuf_info.pthread == NULL) 
        return;

    GLOBAL_INT_DISABLE();
    jbuf_info.run = 0;
    GLOBAL_INT_RESTORE();

    while(jbuf_info.pthread)
        rtos_delay_milliseconds(10);

    os_printf("voide close DONE\r\n");
}

int app_video_intf_open_record(char *path_filename)
{
    int ret;
 //   GLOBAL_INT_DECLARATION();
    
    if(jbuf_info.pthread == NULL) 
        return -1;
    
    os_printf("vi open_record\r\n");

	
    ret = jpeg2avi_start_record(path_filename);
    if(ret == 0)
    {
        os_printf("can't mk file\r\n");
        jpeg2avi_deinit();
        return -1;
    }

//    GLOBAL_INT_DISABLE();
//    jbuf_info.record_need = 1;
//    GLOBAL_INT_RESTORE();   

    os_printf("vi open_record done\r\n");
    return 0;
}

int app_video_intf_close_record(void)
{
    GLOBAL_INT_DECLARATION();
    
    if(jbuf_info.pthread == NULL) 
        return 0;

    if(jbuf_info.record_need == 0)
        return 1;

    os_printf("vi close_record\r\n");
	extern 	int dfs_unmount(const char *specialfile);
	dfs_unmount("/sd");

    GLOBAL_INT_DISABLE();
    jbuf_info.record_need = 0;
    GLOBAL_INT_RESTORE();  
    
    jpeg2avi_stop_record(); 
    jpeg2avi_deinit(); 

    return 1;
}

int app_video_intf_open_p2p(void)
{
    GLOBAL_INT_DECLARATION();
    
    if(jbuf_info.pthread == NULL) 
        return 0;

    if(jbuf_info.p2p_need)
        return 1;
    
    os_printf("vi open_p2p\r\n");
    
if(0)
{
    TZH_PPCS_SetDIDString(RTOS_PPCS_DID_STRING);
    TZH_PPCS_SetDIDLicense(RTOS_PPCS_DID_APILICENSE);
}	
else
{
    TZH_PPCS_SetDIDString(PPCScfm.didStr);
    TZH_PPCS_SetDIDLicense(PPCScfm.didApilics);

}
    TZH_PPCS_SetServerInitString(RTOS_PPCS_SERVER_INIT_STRING);
    TZH_PPCS_SetDIDCRCKey(RTOS_PPCS_DID_CRCKEY);
    TZH_StartPPCS();

//    GLOBAL_INT_DISABLE();
//    jbuf_info.p2p_need = 1;
//    GLOBAL_INT_RESTORE();   

    os_printf("vi open_p2p done\r\n");
    return 1;
}

int app_video_intf_close_p2p(void)  
{
    GLOBAL_INT_DECLARATION();
    
    if(jbuf_info.pthread == NULL) 
        return 0;

    if(jbuf_info.p2p_need == 0)
        return 1;

    os_printf("vi close_p2p\r\n");

    GLOBAL_INT_DISABLE();
    jbuf_info.p2p_need = 0;
    GLOBAL_INT_RESTORE(); 

    TZH_StopPPCS();
    
    os_printf("vi close_p2p done\r\n");
    
    return 1;
}
#else

typedef struct jpeg_hdr_st
{
    UINT8 id;
    UINT8 is_eof;
    UINT8 pkt_cnt; 
    UINT8 size;
    char hd_dt0;
    char hd_dt1;
    char hd_dt2;
    UINT8 hd_dt3; 	
}JPEG_HDR_ST, *JPEG_HDR_PTR;

typedef struct _JPEG_BUF_INFO_
{
	char  * pJpegBuf;
	int     curJpegSize;
	int     curJpegPktCnt;
	int     curJpegID;
}JPEG_BUF_INFO;


static char  * g_pJpegBuf = NULL;
static int   gMaxJpegLen = 50*1024;
static int   gCurJpegSize = sizeof(STREAMHEAD);
static int   gCurJpegID = -1;
static int   gCurJpegPktCnt = 0;
static int   gSerialNo = 0;
static int   gDropId = -1;
static int   gDropFlag = 0;

static int   gWriteUseJpegBufIdx = -1; // 0 or 1, -1 no used
static int   gReadUseJpegBufIdx = -1;  // 0 or 1, -1 no used

static volatile int   gvideo_record_open = 0;

void app_video_add_pkt_hdr(TV_HDR_PARAM_PTR param)
{
    JPEG_HDR_PTR elem_tvhdr = (JPEG_HDR_PTR)param->ptk_ptr;

    elem_tvhdr->id = (UINT8)param->frame_id;
    elem_tvhdr->is_eof = param->is_eof;
    elem_tvhdr->pkt_cnt = param->frame_len;
    elem_tvhdr->size = 0;
}
void app_video_add_p2p_header(char * pBuf,int nBufSize)
{
	struct timeval          tv;
	gettimeofday( &tv, NULL );
	STREAMHEAD * phead = (STREAMHEAD *)pBuf;
	phead->startcode		= htolong(STREAM_START_CODE);
	phead->type          = 3;
	phead->type1			= 0;
	phead->len           = htolong(nBufSize);
	phead->streamid      = 0x00;
	phead->frameno      	= htolong(gSerialNo++);
	phead->militime      = htonet(tv.tv_usec / 1000);
	phead->sectime       = htolong(tv.tv_sec);
	phead->resolution	= 0;//0-VIDEO_HDVGA; 1-QVGA 2-HD720P
	phead->currsit		= 0;
	phead->byzone		= 0;
}

int app_video_intf_send_packet (UINT8 *data, UINT32 len)
{
	JPEG_HDR_PTR pHdr = (JPEG_HDR_PTR)data;

	if((gDropFlag == 1) && (gDropId == pHdr->id))
	{
		//os_printf("drop id %d\r\n",pHdr->id);
		if(pHdr->is_eof == 1)
		{
			gDropFlag = 0;
			gDropId = -1;
		}
		return len;
	}
	//os_printf("HDR:ID=%d EOF=%d pkgCnt=%d size=%d len=%d\r\n",pHdr->id,pHdr->is_eof,pHdr->pkt_cnt,pHdr->size,len);
    //os_printf("voide send:%p, %p\r\n", data, len);
    if(gCurJpegPktCnt==0)
	{
		if(gCurJpegID != pHdr->id)
		{
			gCurJpegID = pHdr->id;
		}
		//int i;
		//os_printf("JPEG Header:");
		//for(i=0;i<16;i++)
		//	os_printf("%02X ",data[i]);
		//os_printf("\r\n");
    }
    if(gMaxJpegLen < (gCurJpegSize+len))
    {
		os_printf("cur jpeg size %d too large, drop it\r\n",gCurJpegSize+len);
		gCurJpegID = -1;
		gCurJpegPktCnt = 0;
		gCurJpegSize = sizeof(STREAMHEAD);
		gDropFlag = 1;
		gDropId = pHdr->id;
		return len;
    }
	gCurJpegPktCnt++;
	if(pHdr->is_eof != 1)
	{
		os_memcpy(g_pJpegBuf+gCurJpegSize,data+sizeof(JPEG_HDR_ST),len-sizeof(JPEG_HDR_ST));		
		gCurJpegSize+=len-sizeof(JPEG_HDR_ST);
	}
	else
	{
		if(gCurJpegPktCnt == pHdr->pkt_cnt)
		{
			UINT8 * plen = data+len-4;
		#if 0
			int i;
			UINT8 * plen = data+len-4;
			os_printf("JPEG TAIL:");
			for(i=len-8;i<len;i++)
				os_printf("%02X ",data[i]);
		    os_printf("\r\n");
			os_printf("Total len=%d:%d\r\n",gCurJpegSize-sizeof(STREAMHEAD),*((UINT32*)plen));
		#endif
            if(len-sizeof(JPEG_HDR_ST) > 5)
            {
                os_memcpy(g_pJpegBuf+gCurJpegSize, data+sizeof(JPEG_HDR_ST), len-sizeof(JPEG_HDR_ST)-5);
                gCurJpegSize += len-sizeof(JPEG_HDR_ST) - 5;
            }
            else
            {
                // the ff d9 has aready copyed in previous pkt, 
                // so no need copy data, 
                // but recalc jpeg length.
                gCurJpegSize -= 5 - (len-sizeof(JPEG_HDR_ST));
            }

			if((gCurJpegSize-sizeof(STREAMHEAD)) == *((UINT32*)plen))
			{
                GLOBAL_INT_DECLARATION();
				app_video_add_p2p_header(g_pJpegBuf,gCurJpegSize-sizeof(STREAMHEAD));
				PPCS_SendVideoData(g_pJpegBuf,gCurJpegSize);

                if(gvideo_record_open == 1)
                {
                    int write_ret = 0;
                    void *pData = g_pJpegBuf+sizeof(STREAMHEAD);
                    const int nLength = gCurJpegSize-sizeof(STREAMHEAD);
                    
                    write_ret = jpeg2avi_input_data(pData, nLength, eTypeVideo);
                    if(write_ret <= 0)
                    {
                        GLOBAL_INT_DISABLE();
                        gvideo_record_open = 2;
                        GLOBAL_INT_RESTORE();
                        os_printf("vi record write err\r\n");
                    }
                }
                else if(gvideo_record_open == 2)
                {
                    jpeg2avi_stop_record(); 
                    jpeg2avi_deinit(); 

                    GLOBAL_INT_DISABLE();
                    gvideo_record_open = 0;
                    GLOBAL_INT_RESTORE();

                    os_printf("vi stop_record done\r\n");
                    
                }
			}
		}		
		gCurJpegPktCnt = 0;
		gCurJpegID = -1;
		gCurJpegSize = sizeof(STREAMHEAD);		
	}
	return len;
}

int app_set_ppcs_server_init_string(char * didString)
{   
    char did_head[5];
    int did_len;
    did_len = os_strlen(didString);

    if((didString == NULL) || (did_len < 4))
        return 0;
    
    os_memset(did_head,0,sizeof(did_head));
    os_memcpy( did_head, didString, 4);

    if(os_strcmp(did_head,"RTOS")==0)
    {
        TZH_PPCS_SetServerInitString(RTOS_PPCS_SERVER_INIT_STRING);
        TZH_PPCS_SetDIDCRCKey(RTOS_PPCS_DID_CRCKEY);
        os_printf("RTOS_PPCS_SERVER_INIT_STRING\r\n");	
        return 1;
    }
    else 
        return 0;
}

void app_video_intf_open (void)
{
    os_printf("voide open\r\n");
	g_pJpegBuf = sdram_realloc(g_pJpegBuf, gMaxJpegLen);
	if(g_pJpegBuf == NULL)
		os_printf("JPEG malloc error\r\n");
    #if (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF) 
    TVIDEO_SETUP_DESC_ST setup;
    
    setup.send_type = TVIDEO_SND_INTF;
    setup.send_func = app_video_intf_send_packet;
    setup.start_cb = NULL;
    setup.end_cb = NULL;

    setup.pkt_header_size = sizeof(JPEG_HDR_ST);
    setup.add_pkt_header = app_video_add_pkt_hdr;
   
    video_transfer_init(&setup);
	app_audio_intf_open();

    //app_video_intf_open_record();
    if(app_dac_play_init(8000, 2) == 0)
    {
        TZH_PPCS_RegisterPlayTalkCB(app_dac_play_write_data,2048);
    }

if(0)
{
    TZH_PPCS_SetDIDString(RTOS_PPCS_DID_STRING);
    TZH_PPCS_SetDIDLicense(RTOS_PPCS_DID_APILICENSE);
    TZH_PPCS_SetServerInitString(RTOS_PPCS_SERVER_INIT_STRING);
    TZH_PPCS_SetDIDCRCKey(RTOS_PPCS_DID_CRCKEY);
}	
else
{
    TZH_PPCS_SetDIDString(PPCScfm.didStr);
    TZH_PPCS_SetDIDLicense(PPCScfm.didApilics);
  	app_set_ppcs_server_init_string(PPCScfm.didStr);
}

	TZH_StartPPCS();
    #endif
}

void app_video_intf_close (void)
{
    os_printf("voideo close\r\n");
    #if (CFG_USE_SPIDMA || CFG_USE_CAMERA_INTF)  
    video_transfer_deinit();  
    #endif

    app_video_intf_close_record();
    
}

int app_video_intf_open_record(void)
{
    if(jpeg2avi_init() == 1)
    {
        os_printf("vi open_record\r\n");
        
        jpeg2avi_set_video_param(320, 240, 10);
        jpeg2avi_set_audio_param(1, 8000, 16);

        jpeg2avi_start_record("sd/jpeg2avi_000.avi");

        GLOBAL_INT_DECLARATION();
        GLOBAL_INT_DISABLE();
        gvideo_record_open = 1;
        GLOBAL_INT_RESTORE(); 
        
        os_printf("vi open_record done\r\n");
    }
    
    return 0;
}

int app_video_intf_close_record(void)
{
    if(gvideo_record_open)
    {
        os_printf("vi close_record\r\n");

        GLOBAL_INT_DECLARATION();
        GLOBAL_INT_DISABLE();
        gvideo_record_open = 2;
        GLOBAL_INT_RESTORE(); 
    }
    
    return 0;
}

#endif


#if CFG_SUPPORT_RTT
void video(int argc, char **argv)
{
#if 0
    if(0 == os_strcmp(argv[1], "open"))
    {
        app_video_intf_open();
    }
    else if(0 == os_strcmp(argv[1], "close"))
    {
        app_video_intf_close();
    }
    else if(0 == os_strcmp(argv[1], "record_open"))
    {
        app_video_intf_open_record();
    }
    else if(0 == os_strcmp(argv[1], "record_close"))
    {
        app_video_intf_close_record();
    }
    else if(0 == os_strcmp(argv[1], "p2p_open"))
    {
        //app_video_intf_open_p2p();
    }
    else if(0 == os_strcmp(argv[1], "p2p_close"))
    {
        //app_video_intf_close_p2p();
    }
    else
    {
        os_printf("video open/close/record_open/record_closep2p_open/p2p_close\r\n");
    }
#endif
}
FINSH_FUNCTION_EXPORT_ALIAS(video, __cmd_video, video);

int app_get_ssid_from_ppcs_id( char* ssid_buf, int ssid_len)
{
    char* ppcs_id;
    int ppcs_len;
    
    if((ssid_buf == NULL) || (ssid_len < 15))
        return 0;
    
    if(0)
    {
        //ppcs_id = RTOS_PPCS_DID_STRING;
        //ppcs_len = os_strlen(RTOS_PPCS_DID_STRING);
    }
    else
    {
        ppcs_id = PPCScfm.didStr;
        ppcs_len = os_strlen(PPCScfm.didStr);
    }

    if(ssid_len < ppcs_len)
        return 0;

    for(int i=0, j=0; i<ppcs_len; i++)
    {
        if(ppcs_id[i] == '-')
            continue;
        
        ssid_buf[j++] = ppcs_id[i];
    }

    return 1;
}

#endif  //  CFG_SUPPORT_RTT


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
#include "app_audio_intf.h"
#include "voice_transfer.h"
#include "audio_adpcm.h"
#include "app_demo_config.h"
#include "tzh_ppcs_api.h"
#include "app_jpeg2avi.h"


static char * g_pAudioAdpcmBuf = NULL;
static int   gAudioSerialNo = 0;

extern int PPCS_SendAudioData(UINT8 *data, UINT32 len);

void app_audio_add_p2p_header(char * pBuf,int nBufSize)
{
    struct timeval          tv;
    gettimeofday( &tv, NULL );
    STREAMHEAD * phead = (STREAMHEAD *)pBuf;
    phead->startcode		= htolong(STREAM_START_CODE);
    phead->type          = 6;
    phead->type1			= 0;
    phead->len           = htolong(nBufSize);
    phead->streamid      = 0x01;
    phead->frameno      	= htolong(gAudioSerialNo++);
    phead->militime      = htonet(tv.tv_usec / 1000);
    phead->sectime       = htolong(tv.tv_sec);
    phead->resolution	= 0;//0-VIDEO_HDVGA; 1-QVGA 2-HD720P
    phead->currsit		= 0;
    phead->byzone		= 0;
}

int app_audio_intf_send_packet (UINT8 *data, UINT32 len)
{
//#define TEST_PCM

	if(TZH_PPCS_GetLiveFlag())//online    
	{
		UINT32 tx_len = 0;              
	#ifdef TEST_PCM         
		if(len > 2048)        
		{            
			len = 2048;//avoid g_pAudioAdpcmBuf overflow        
		}        
		app_audio_add_p2p_header(g_pAudioAdpcmBuf,len);        
		memcpy(g_pAudioAdpcmBuf+sizeof(STREAMHEAD),data,len);        
		tx_len = len + sizeof(STREAMHEAD);        
		//os_printf("pcm:tx_len:%d,len:%d\r\n",tx_len,len);  
	#else        
      	int adpcmlen = 0;                
		adpcmlen = len/4;        
		ADPCM_EncodeData(0, data, len, g_pAudioAdpcmBuf+sizeof(STREAMHEAD)); 
		app_audio_add_p2p_header(g_pAudioAdpcmBuf,adpcmlen);        
		tx_len = adpcmlen + sizeof(STREAMHEAD);        
		//os_printf("DVI_ADPCM:tx_len:%d,len:%d\r\n",tx_len,adpcmlen); 
	#endif
	
		PPCS_SendAudioData(g_pAudioAdpcmBuf, tx_len);    
	}    
	else //offline    
	{        ADPCM_DecoderClear(0);
		if(jpeg2avi_audio_format_get() == WAVE_FORMAT_PCM)         
		{            
			jpeg2avi_input_data(data,len,eTypeAudio);            
			//os_printf("save data to local storage\r\n");        
		}        
		else        
		{            
			os_printf("audio format error! 0x%04x@@@\r\n",jpeg2avi_audio_format_get());            
			return -1;        
		}    
	}        
	return len;
}

#if 0
void app_audio_process_talk_packet(UINT8 *data, int len)
{
    os_printf("get talk packet len=%d\r\n",len);
    /* Must Decode talk packet first, then play buffer to speaker */
}
#endif

void app_audio_intf_open (void)
{
    os_printf("voice open\r\n"); 
    if(g_pAudioAdpcmBuf != NULL)
    {
        os_printf("voice aready opened\r\n"); 
        return;
    }

    g_pAudioAdpcmBuf = sdram_realloc(g_pAudioAdpcmBuf, 2112);
    if(g_pAudioAdpcmBuf == NULL)
    {
        os_printf("Audio malloc error\r\n");
        return;
    }
    
    tvoice_transfer_init(app_audio_intf_send_packet);
    /* Register process talk buffer callback function and set the talk packet max size */
    //TZH_PPCS_RegisterPlayTalkCB(app_audio_process_talk_packet, 256);
}

void app_audio_intf_close (void)
{
    os_printf("voice close\r\n");
    if(g_pAudioAdpcmBuf == NULL)
    {
        os_printf("voice aready closed\r\n"); 
        return;
    }

    tvoice_transfer_deinit();  
    
    sdram_free(g_pAudioAdpcmBuf);
    g_pAudioAdpcmBuf = NULL;
}

#if CFG_SUPPORT_RTT
void audio(int argc, char **argv)
{
    if(0 == os_strcmp(argv[1], "open"))
    {
        app_audio_intf_open();
    }
    else if(0 == os_strcmp(argv[1], "close"))
    {
        app_audio_intf_close();
    }
    else
    {
        os_printf("audio open/close\r\n");
    }
}
FINSH_FUNCTION_EXPORT_ALIAS(audio, __cmd_audio, audio);
#endif  //  CFG_SUPPORT_RTT





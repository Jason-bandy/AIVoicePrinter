#ifndef _APP_AUDIO_INTF_H__
#define _APP_AUDIO_INTF_H__

void app_audio_intf_open (void);
void app_audio_intf_close (void);
int app_audio_intf_send_packet (UINT8 *data, UINT32 len);
    
#endif // _APP_VIDEO_INTF_H__

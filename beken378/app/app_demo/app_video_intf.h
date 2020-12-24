#ifndef _APP_VIDEO_INTF_H__
#define _APP_VIDEO_INTF_H__

void app_video_intf_open (void);
void app_video_intf_close (void);
int app_video_intf_send_packet (UINT8 *data, UINT32 len);
int app_video_intf_open_record(char *path_filename);
int app_video_intf_close_record(void);
int app_video_intf_open_p2p(void);
int app_video_intf_close_p2p(void);

#endif // _APP_VIDEO_INTF_H__

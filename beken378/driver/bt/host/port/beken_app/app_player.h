#ifndef _APP_PLAYER_H_
#define APP_PLAYER_H_

#include "ff.h"
#include "mp3common.h"
#if (CONFIG_APP_MP3PLAYER == 1)
enum
    {
        APP_PLAYER_WORK_MODE_PLAY_ONE = 0,
        APP_PLAYER_WORK_MODE_PLAY_CONT = 1,
        APP_PLAYER_WORK_MODE_PLAY_ONE_CYCLE = 2,
        APP_PLAYER_WORK_MODE_PLAY_CONT_CYCLE = 3
    };

enum
    {
        BUTTON_PLAYER_NONE = 0,
        BUTTON_PLAYER_PLAY_PAUSE,
        BUTTON_PLAYER_NEXT,
        BUTTON_PLAYER_PREV,
        BUTTON_PLAYER_VOL_P,
        BUTTON_PLAYER_VOL_M,
        BUTTON_PLAYER_VOL_MUTE,

#if (defined(CONFIG_BLUETOOTH_HFP))
        BUTTON_MP3_HFP_ACK,
        BUTTON_MP3_HFP_REJECT,
#endif

        BUTTON_PLAYER_PLAY_MODE_SET,
        BUTTON_PLAYER_MODE_CHANGE,
        BUTTON_PLAYER_END
    };

#define MAX_DIR_DEPTH             6  // no large than 7, otherwise the break point can not restroe correctly.(see driver_flash.c for detail)

void app_player_button_setting(void);
void app_player_hw_init( int mode );
void app_player_init( int mode );
void app_player_uninit( int mode );
void app_player_file_info_init( void );
void app_player_print_file_info( void );
int app_player_state_pause( void );
void app_player_play_func( void );
void player_start_first_running(void);

void wav_mem_init(void);
void wav_mem_uninit(void);
void mp3_mem_init(void);
void mp3_mem_uninit(void);
uint32 app_check_mp3_music_type(void);

void app_player_play_pause_caller( int play_pause);//add by zjw for more memory
int player_get_play_status( void );
void app_player_save_breakpoint( void );
int app_player_button_play_pause( void );
void app_backup_volume(flash_info_t vol_type);
void app_restore_volume(flash_info_t vol_type);
void app_playwav_resumeMp3(uint32 fieldId);
void playwav_resumeMp3(uint32 fieldId);
uint8 get_fat_ok_flag(void);
void app_backup_breakpoint(void);
FIL *Get_File_From_Number(uint16 number);
uint32 app_player_automatic_play_at_first_time(void);
void app_restore_breakpoint(void);
void MP3_CALL Convert_Mono(short *buffer, int outputSamps);
void ClearMP3(MP3DecInfo *mp3DecInfo);
int app_player_unload( void );
#endif

#endif

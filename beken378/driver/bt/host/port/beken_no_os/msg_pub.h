#ifndef _MSG_PUB_H_
#define _MSG_PUB_H_

#define DELAY_MSG_PUT                (0)
#define OS_TICK_MSEC                 (10)

#define MSG_SUCCESS_RET              (0)
#define MSG_FAILURE_RET              (-1)

typedef struct
{
    uint32 id;

    void *hdl;
    uint32 arg;
} MSG_T, *MSG_PTR;

/* public api*/
extern void msg_init(void);
extern void msg_dump(void);
extern void msg_uninit(void);
extern void msg_put(uint32 msg);
extern void msg_clear_all(void);
extern int msg_priority_put(MSG_T *msg);

#if DELAY_MSG_PUT
extern int msg_delay_put(uint32 os_tick, MSG_T *msg);
#endif

/* message table*/
#define KEY_MSG_GP                      (0x0000FF00)         /* Attention: special msg*/
#define BLUETOOTH_MSG_GP                (0x10000000)
#define SDMUSIC_MSG_GP                  (0x20000000)
#define UDISK_MSG_GP                    (0x30000000)
#define FM_MSG_GP                       (0x40000000)
#define LINEIN_MSG_GP                   (0x50000000)
#define OTHER_MSG_GP                    (0x60000000)
#define ENV_MSG_GP                      (0x70000000)

/* Name format: MSG_module_messageInformation
   assume: message number is less than 65535 at one module
*/
enum
{
    MSG_NULL                         = 0,

    /* Attention: special msg for key press, from 0x0000ff00--0x0000ffff*/
    MSG_KEYPRESS                     = KEY_MSG_GP       + 0x00,
        
    /*BLK0: bluetooth msg*/
    MSG_BT_INIT                      = BLUETOOTH_MSG_GP + 0x0000,
    
    /*BLK1: sd music msg*/
    MSG_SD_INIT                      = SDMUSIC_MSG_GP   + 0x0000,
    MSG_SD_NOTIFY,
    
    /*BLK2: usb disk msg*/
    MSG_UDISK_INIT                   = UDISK_MSG_GP     + 0x0000,
    
    /*BLK3: fm msg*/
    MSG_FM_INIT                      = FM_MSG_GP        + 0x0000,
    MSG_FM_SEEK_CONTIUE,
    MSG_FM_MEMERIZE,
    MSG_FM_RESTORE,
    MSG_FM_INSTALL_START,
    MSG_FM_INSTALL_HW,
    MSG_FM_INSTALL_CHANNEL,
    MSG_FM_UNINSTALL,
    MSG_FM_SEEK_PREV_CHANNEL,
    MSG_FM_SEEK_NEXT_CHANNEL,
    MSG_FM_AUTO_SEEK_START,
    MSG_FM_CHANNEL_SEEK_START,
    MSG_FM_CHANNEL_TUNE_CONTINUE,                  /* 0xc*/
    MSG_FM_CHANNEL_SEEK_CONFIG,
    MSG_FM_CHANNEL_TUNE_OVER,
    MSG_FM_TUNE_SUCCESS_CONTINUE,
    MSG_FM_TUNE_FAILURE_CONTINUE,
    MSG_FM_AUTO_SEEK_OVER,
    MSG_FM_DISABLE_MUTE,
    MSG_FM_ENABLE_MUTE,
    
    
    /*BLK4: linein msg*/
    MSG_LINEIN_INIT                  = LINEIN_MSG_GP  + 0x0000,
    
    /*BLK5: other msg*/   
    MSG_LED_INIT                     = OTHER_MSG_GP   + 0x0000,
    MSG_SD_ATTACH_CHANGE,            /* sd attach or detach*/
    MSG_SD_READ_ERR,                 /* mp3-mode,SD read Err */
    MSG_SDADC,                       /* sdadc*/
    MSG_POWER_DOWN,
    MSG_POWER_UP,
    MSG_FLASH_WRITE,
    MSG_IRDA_RX,
    MSG_LOWPOWER_DETECT,

    MSG_ENV_WRITE_ACTION            = ENV_MSG_GP + 0x0000,
    
    #ifdef MAIN_BOARD_EVB_V1
    MSG_LINEIN_ATTACH,
    MSG_LINEIN_DETACH,
    #endif

    MSG_TIMER_PWM1_PT2_HDL,
    MSG_SAVE_VOLUME,
    
#if(POWERKEY_5S_PARING == 1)
    MSG_ENTER_MATCH_STATE,
    MSG_POWER_ON_START_CONN
#endif

/*-----------le_wav_play, DualMode use only----------*/
#ifdef BT_DUALMODE
    MSG_LE_CONN_WAV_PLAY,
    MSG_LE_DISCONN_WAV_PLAY,
#endif
/*-----------le_wav_play, DualMode use only----------*/
};

#endif
// EOF

#ifndef _DRIVER_BEKEN_INCLUDES_H_
#define _DRIVER_BEKEN_INCLUDES_H_

#include "host/port/include/types.h"
#include "Common/include/config.h"
#include "excutil.h"
#include "host/port/beken_driver/board.h"
#include "spr_defs.h"
#include "bautil.h"
#include "msg_pub.h"
#include "msg.h"
#include "bt_sys_comp.h"
#include "driver_audio.h"
#include "target.h"
#include "timer.h"

#include "co_list.h"
#include "sys_ctrl_pub.h"
#include "mailbox_pub.h"
#include "audio_pub.h"
#include "arm_mcu_pub.h"
#include "ring_buffer.h"
#include "gpio_bk7271.h"
#include "arm_arch.h"

#ifdef BT_ONE_TO_MULTIPLE
#define A2DP_STREAM_ON        0x100
#define A2DP_STREAM_OFF       0x200
#define HF_INCOMING_CALL      0x400
#define HF_CALL_OVER          0x800
#define HF_OUTGOING_CALL      0x1000
#define A2DP_CONNECTED        0x2000
#endif


/****software interrupt flag**********/
extern volatile uint32 sleep_tick;
extern volatile uint32 pwdown_tick;
extern volatile uint16 sniffmode_wakeup_dly;
extern volatile uint8 flag_power_charge;
extern volatile uint16 adcmute_cnt;
#define CLEAR_SLEEP_TICK     do{sleep_tick = 0;}while(0)
#define INC_SLEEP_TICK       do{sleep_tick++;}while(0)
#define SLEEP_TICK_CHECK     1000
#define CLEAR_PWDOWN_TICK    do{pwdown_tick = 0;}while(0)
#define INC_PWDOWN_TICK(step)do{pwdown_tick += step;}while(0)
#define POWER_DOWN_CHECK     -1

#define ATOMIC_OR(variable, bit) do{\
        uint32 info;    \
        VICMR_disable_interrupts(&info);  \
        variable |= (bit);                                  \
        VICMR_restore_interrupts(info);       \
                                    }while(0)

#define ATOMIC_AND(variable, bit) do{                     \
        uint32 info;    \
        VICMR_disable_interrupts(&info);  \
        variable &= (bit);                                  \
        VICMR_restore_interrupts(info);       \
                                    }while(0)
#if 0
void global_event_set(uint32 flag);
void global_event_clear(uint32 flag);
#endif

#endif

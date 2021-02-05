#ifndef _APP_BEKEN_INCLUDES_H_
#define _APP_BEKEN_INCLUDES_H_

#include "host/port/include/types.h"
//#include <config/config.h>
#include "Common/include/config.h"
#include "jos.h"
#include "bluetooth.h"
#include "hci_internal.h"
#include "bt_a2dp_sink.h"
#include "bt_sco_backend_utils.h"
#include "app_sbc.h"

#include "app_hfp.h"

#include "app_protocol.h"
#include "app_debug.h"
#if 0
#include "driver_flash.h"
#include "app_equ.h"
#include "driver_audio.h"
#include "app_linein.h"


#include "app_aec.h"
#include "app_udisk.h"
#include "app_button.h"
#include "app_exception.h"
#include "app_customer.h"

#endif


#include "app_env.h"
#include "app_msg.h"
#include "app_prompt_wav.h"
#include "audio_out_interface.h"
#include "app_bt.h"
#include "bt_app_internal.h"

extern volatile uint32 BK3000_AUD_AUDIO_CONF_BAK;
#ifdef CONFIG_TWS
//extern uint32 audio_sync[3];

#endif

/* current transaction id. */
#define TID_A2DP_PROTOCOL_SSA_REQUEST   0x7000
#define TID_HFP_PROTOCOL_SSA_REQUEST    0x7001

#endif

//EOF

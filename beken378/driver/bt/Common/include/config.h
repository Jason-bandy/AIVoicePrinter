/* Jungo Confidential, Copyright (c) 2012 Jungo Ltd.  http://www.jungo.com */
#ifndef __CONFIG_H__
#define __CONFIG_H__
//#define CHIP_3231S          1
//#define CHIP_7211           2
//#define CHIP_3262N           3
//#define CHIP_3433           4
//#define CHIP_3435           5
//#define CHIP_7231           6
//#define CHIP_3435_v2        7
#define CHIP_3266             8

#define CHIP_ID             CHIP_3266

#if (CHIP_ID == CHIP_3266)       

#define ADC_3262N_SYTLE
#define DMA_3262N_SYTLE
#define I2C_FM_7211_SYTLE
#define I2C_3231_SYTLE
#define I2S_3252_SYTLE
#define SPI_3231_SYTLE
#define PWM0_3262N_SYTLE
#define PWM1_3262N_SYTLE
#define PWM2_3262N_SYTLE
#define ICU_3262N_SYTLE
#define WDT_3231_SYTLE
#define FFT_7211_SYTLE
#define XVR_3262N_SYTLE
#define IRDA_7211_SYTLE
#define UART0_3231_SYTLE
#define UART1_3231_SYTLE
#define GPIO_3254_STYLE
#define AUDIO_3251_SYTLE
#define FLASH_3231_SYTLE
#define USB_3435_v2_STYLE
#define SPI_DMA_7211_SYTLE
#define MAILBOX_7211_SYTLE


#define MCU_CLK_32KHz       32000
#define MCU_CLK_16MHz       16000000
#define MCU_CLK_26MHz       26000000
#define MCU_CLK_48MHz       48000000
#define MCU_CLK_96MHz       96000000
#define MCU_CLK_120MHz      120000000

#define MCU_CLK             MCU_CLK_26MHz


#define PER_CLK_32KHz       32000
#define PER_CLK_16MHz       16000000
#define PER_CLK_26MHz       26000000
#define PER_CLK_48MHz       48000000
#define PER_CLK_96MHz       96000000
#define PER_CLK_120MHz      120000000

#define PER_CLK             PER_CLK_26MHz

typedef enum {JUNGO_HOST = 0, CEVA_HOST = 1, NONE_CONTROLLER = 2} HOST_MODE;
#define ADDR_ALIGNED(addr, align) ((addr)+(align)-1)/(align)*(align)
#define FLASH_LINE_1    0
#define FLASH_LINE_2	1
#define FLASH_LINE_4 	2


#define FLASH_CLK_26mHz 8
#define FLASH_CLK_39mHz 5
#define FLASH_CLK_78mHz 4

#define CPU_DCO_CLK     96  /* CPU clk = 96MHz */
#define CPU_CLK_SEL     1

#ifdef BT_ONE_TO_MULTIPLE
#define CPU_CLK_DIV     0
#else
#define CPU_CLK_DIV     1
#endif
#define SNIFF_CPU_CLK_SEL 1
#define SNIFF_CPU_CLK_DIV 1
//#define BT_HOST_MODE    CEVA_HOST  /* for Charles BQB */
#define BT_HOST_MODE    JUNGO_HOST
#ifdef CONFIG_TWS
#define FLASH_CLK_SEL   FLASH_CLK_39mHz //FLASH_CLK_39mHz//FLASH_CLK_78mHz
#else
#define FLASH_CLK_SEL   FLASH_CLK_39mHz //FLASH_CLK_39mHz//FLASH_CLK_78mHz
#endif

#ifdef BT_ONE_TO_MULTIPLE
#define DEFAULT_LINE_MODE FLASH_LINE_2
#else
	#ifdef CONFIG_TWS
#define DEFAULT_LINE_MODE FLASH_LINE_2 //FLASH_LINE_2 // FLASH_LINE_2
	#else
#define DEFAULT_LINE_MODE FLASH_LINE_2 //FLASH_LINE_2 // FLASH_LINE_2
	#endif
#endif


#define DEBUG_PORT_UART0        1
#define DEBUG_PORT_UART1        2
#define DEBUG_PORT_SPI          3
#define DEBUG_PORT_I2C0         4
#define DEBUG_PORT_I2C1         5
#define DEBUG_PORT_IO_SIM_UART  10
#define DEBUG_PORT              DEBUG_PORT_UART0
#define PRINT_PORT              DEBUG_PORT_UART0
#endif

#define CONFIG_ARM_COMPILER                     1
#define CONFIG_BYTE_ORDER                       CPU_LITTLE_ENDIAN
#define CONFIG_PORT                             beken_no_os
#define CONFIG_SINGLE_THREADED                  1
#define CONFIG_NATIVE_UINT64                    1
#define CONFIG_JOS_MBUF                         1
#define CONFIG_JOS_BUS                          1
#define CONFIG_JOS_UTILS                        1
#define CONFIG_JOS_SECURE_PTR                   1
#define CONFIG_COMMON_STR_UTILS                 1
#define CONFIG_MEMPOOL                          1
//#define CONFIG_MEMPOOL_SIZE                    (38 * 1024)//(33 * 1024)
#define CONFIG_MEMPOOL_DMABLE                   1
#define CONFIG_BLUETOOTH                        1
#define CONFIG_BLUETOOTH_HCI_UART               1
#define CONFIG_BLUETOOTH_SDP_SERVER             1
#define CONFIG_BLUETOOTH_A2DP                   1
#define CONFIG_BLUETOOTH_A2DP_SINK              1
#define CONFIG_BLUETOOTH_AVRCP                  1
#define CONFIG_BLUETOOTH_AVRCP_CT               1
#define CONFIG_BLUETOOTH_AVRCP_TG              1
#define CONFIG_BLUETOOTH_AVDTP                  1
#define CONFIG_BLUETOOTH_AVDTP_SCMS_T     1
#define CONFIG_BLUETOOTH_AVCTP                  1
#define CONFIG_BLUETOOTH_HFP                    1
#define CONFIG_BLUETOOTH_SDP_HFP                1
#define CONFIG_BLUETOOTH_HSP                    1
#define CONFIG_BLUETOOTH_SDP_HSP                1
#define CONFIG_BLUETOOTH_SCO                    1
#define CONFIG_BLUETOOTH_APP                    1
#define CONFIG_BLUETOOTH_AUDIO_APP_STANDALONE   1
#define CONFIG_AUDIO_OUT_INTERFACE              1
#define CONFIG_PKG                              1
#define CONFIG_PKG_SBC                          1
#define CONFIG_FILE_LIST                        file_list_beken
#define UWVER_STR                               "4.0.33.5"
#define CONFIG_APP_MP3PLAYER                    0
#define CONFIG_AUDIO_TRANSFER_DMA               1
#define LOWBAT_DET_ENABLE						0

#define CONFIG_DEBUG_PCM_TO_UART                0
#define CONFIG_CPU_CLK_OPTIMIZATION            0

#define CONFIG_PROMPT_WAVE_AS_ALAW              0

#define CONFIG_BLUETOOTH_SSP
#define CONFIG_BLUETOOTH_HID
#define CONFIG_BLUETOOTH_SPP

/* #define CONFIG_DRIVER_I2S */
#define CONFIG_DRIVER_DAC
#define CONFIG_DRIVER_ADC
#define CONFIG_CHARGE_EN						0

//#define CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE      1
#define BUTTON_DETECT_IN_SNIFF                  1
#define CONFIG_UART_IN_SNIFF					0

//注意： 音箱方案配置时请将以下宏CONFIG_AUDIO_MIC_DIFF_EN/CONFIG_EXT_AUDIO_PA_EN打开， SYS_CFG_BUCK_ON关闭

#define CONFIG_TX_CALIBRATION_EN                0

#define CHIP_PACKAGE_TSSOP_28                   0
#if     CHIP_PACKAGE_TSSOP_28
#define CONFIG_AUDIO_DAC_ALWAYSOPEN				1
#define CONFIG_AUDIO_DAC_RAMP_EN				0
#define CONFIG_AUDIO_MIC_DIFF_EN				1
#define CONFIG_EXT_AUDIO_PA_EN   				1
#define CONFIG_EXT_PA_DIFF_EN                   1
#define SYS_CFG_BUCK_ON                         0
#define BT_ONE2MULTIPLE_AS_SCATTERNET		    1
#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
    #undef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
#endif
#else
#define BT_ONE2MULTIPLE_AS_SCATTERNET		    0
#define CONFIG_AUDIO_DAC_ALWAYSOPEN				0
#define CONFIG_AUDIO_DAC_RAMP_EN				1
#define CONFIG_AUDIO_MIC_DIFF_EN				0
#define CONFIG_EXT_AUDIO_PA_EN   				0
#define CONFIG_EXT_PA_DIFF_EN                   0
#define SYS_CFG_BUCK_ON                         1
#endif//CHIP_PACKAGE_TSSOP_28

#if ((CONFIG_CHARGE_EN == 1))
#define CHARGE_HARDWARE         0
#define CHARGE_SOFTWARE         1
#define CHARGE_EXTERNAL         2
#define CONFIG_CHARGE_MODE   CHARGE_HARDWARE
#endif

/* #define CONFIG_APP_HALFDUPLEX */
//#define CONFIG_APP_AEC
//#define CONFIG_APP_EQUANLIZER

#define CONFIG_DAC_DELAY_OPERATION              0

#if (CONFIG_APP_MP3PLAYER == 1)
	#ifdef CONFIG_APP_SDCARD
    	#undef CONFIG_APP_SDCARD
    #endif
    #ifdef CONFIG_APP_SDCARD_4_LINE
    	#undef CONFIG_APP_SDCARD_4_LINE
	#endif
    #define CONFIG_APP_SDCARD
	#define CONFIG_APP_SDCARD_4_LINE
#endif

/* #define CONFIG_IRDA */
/* #define CONFIG_APP_USB_DISK */
/* #define WROK_AROUND_DCACHE_BUG */

/*Board Type Select*/
#define MAIN_BOARD_EVB_V1

// PWM not sleep when no connection
//#define CONFIG_PWM_NOT_SLEEP

/*Feature Select*/
/* #define BT_SD_MUSIC_COEXIST */

//#define CONFIG_ACTIVE_SSP
#define CONFIG_CTRL_BQB_TEST_SUPPORT    1
#define BT_MODE_1V1 (1<<0)
#define BT_MODE_1V2 (1<<1)
#define BT_MODE_TWS (1<<2)
#define BT_MODE_BLE (1<<3)
#define BT_MODE_DM_1V1 (1<<4)
#define BT_MODE_DM_TWS (1<<5)

#define BT_MODE		BT_MODE_DM_1V1

#if (CHIP_PACKAGE_TSSOP_28 == 1)
#if (BT_MODE != BT_MODE_1V1)
    #undef BT_MODE
    #define BT_MODE BT_MODE_1V1
#endif
#endif


#if( BT_MODE==BT_MODE_1V1)
	#ifndef BT_ONE_TO_MULTIPLE
	#define BT_ONE_TO_MULTIPLE
	#endif
	#ifndef BT_ONLY_ONE_BT
	#define BT_ONLY_ONE_BT
	#endif
	#ifdef CONFIG_TWS
	#undef CONFIG_TWS
	#endif
#elif( BT_MODE==BT_MODE_1V2)
	#ifndef BT_ONE_TO_MULTIPLE
	#define BT_ONE_TO_MULTIPLE
	#endif
	#ifdef BT_ONLY_ONE_BT
	#undef BT_ONLY_ONE_BT
	#endif
	#ifdef CONFIG_TWS
	#undef CONFIG_TWS
	#endif
    #define CONFIG_AUTO_CONN_AB  // 上电回连 A ,B 手机交叉循环连接
#elif( BT_MODE==BT_MODE_TWS)
	#ifdef BT_ONE_TO_MULTIPLE
	#undef BT_ONE_TO_MULTIPLE
	#endif
	#ifdef BT_ONLY_ONE_BT
	#undef BT_ONLY_ONE_BT
	#endif
	#ifndef CONFIG_TWS
	#define CONFIG_TWS
	#endif
#elif((BT_MODE==BT_MODE_DM_1V1) || (BT_MODE==BT_MODE_BLE))
	#ifndef BT_ONE_TO_MULTIPLE
	#define BT_ONE_TO_MULTIPLE
	#endif
	#ifndef BT_ONLY_ONE_BT
	#define BT_ONLY_ONE_BT
	#endif
	#ifdef CONFIG_TWS
	#undef CONFIG_TWS
	#endif
	#ifndef BT_DUALMODE
	#define BT_DUALMODE
	#endif
#elif( BT_MODE==BT_MODE_DM_TWS)
	#ifdef BT_ONE_TO_MULTIPLE
	#undef BT_ONE_TO_MULTIPLE
	#endif
	#ifdef BT_ONLY_ONE_BT
	#undef BT_ONLY_ONE_BT
	#endif
	#ifndef CONFIG_TWS
	#define CONFIG_TWS
	#endif
	#ifndef BT_DUALMODE
	#define BT_DUALMODE
	#endif
#endif


#ifdef CONFIG_TWS
//	#ifdef CONFIG_BLUETOOTH_AVRCP_TG
//	#undef CONFIG_BLUETOOTH_AVRCP_TG
//	#endif
//	#ifdef CONFIG_APP_AEC
//	#undef CONFIG_APP_AEC
//	#endif
	#ifdef BT_ONE_TO_MULTIPLE
	#undef BT_ONE_TO_MULTIPLE
	#endif
	#ifdef BT_ONLY_ONE_BT
	#undef BT_ONLY_ONE_BT
	#endif
	#ifdef BQB_TEST
	#undef BQB_TEST
	#define BQB_TEST 0
	#endif
	#ifdef CONFIG_DAC_DELAY_OPERATION
	#undef CONFIG_DAC_DELAY_OPERATION
	#define CONFIG_DAC_DELAY_OPERATION 1
	#endif
	#ifdef INQUIRY_ALWAYS
	#undef INQUIRY_ALWAYS
	#endif
	#ifdef CONFIG_AUDIO_TRANSFER_DMA
	#undef CONFIG_AUDIO_TRANSFER_DMA
	#define CONFIG_AUDIO_TRANSFER_DMA 0
	#endif
	#ifdef CONFIG_CPU_CLK_OPTIMIZATION
	#undef CONFIG_CPU_CLK_OPTIMIZATION
	#define CONFIG_CPU_CLK_OPTIMIZATION 0
	#endif

    #define TWS_CONFIG_LINEIN_BT_A2DP_SOURCE
    #define LINE_TX_DEBUG

#endif

//#define CONFIG_PRODUCT_TEST_INF

#ifdef BT_DUALMODE
#define CONFIG_MEMPOOL_SIZE                    (36 * 1024)
#else
#define CONFIG_MEMPOOL_SIZE                    (40 * 1024)
#endif

#ifdef BT_ONE_TO_MULTIPLE
    #ifdef CONFIG_MEMPOOL_SIZE
        #undef CONFIG_MEMPOOL_SIZE
       #ifdef CONFIG_APP_AEC
    	   #ifdef BT_DUALMODE
           #define CONFIG_MEMPOOL_SIZE                    (36 * 1024)
    	   #else
    	   #define CONFIG_MEMPOOL_SIZE                    (40 * 1024)
    	   #endif
       #else
           #define CONFIG_MEMPOOL_SIZE                    (27 * 1024)
       #endif
    #endif
//    #ifdef CONFIG_CPU_CLK_OPTIMIZATION
//        #undef CONFIG_CPU_CLK_OPTIMIZATION
//        #define CONFIG_CPU_CLK_OPTIMIZATION 		0
//    #endif
    #define A2DP_PATCH_FOR_AVRCP
    #define OTT_STRETIGG_LINK_COEXIST
#ifdef BT_ONLY_ONE_BT
    #define BT_MAX_AG_COUNT                     1
    #define CONFIG_SELECT_PREV_A2DP_PLAY		  0
    #define CONFIG_A2DP_PLAYING_SET_AG_FLOW		  0
    #define INQUIRY_ALWAYS
    //#define CONN_WITH_MUSIC
    #define NEED_SNIFF_DEVICE_COUNT 1
    //#define IPHONE_MUSIC_STATE_COME_LATE_REPAIR     //for iphone PLAYING come before MA having comed,we need stream_start manually on avrcp notify coming
#else
    #define BT_MAX_AG_COUNT                        2
    #define CONFIG_A2DP_PLAYING_SET_AG_FLOW		  1
    #define CONFIG_SELECT_PREV_A2DP_PLAY		  1
    //#define INQUIRY_ALWAYS
    #define CONN_WITH_MUSIC
    #define NEED_SNIFF_DEVICE_COUNT 2
    #if(CONFIG_SELECT_PREV_A2DP_PLAY == 0)
    	#define IPHONE_MUSIC_STATE_COME_LATE_REPAIR     //for iphone PLAYING come before MA having comed,we need stream_start manually on avrcp notify coming
    #endif
#endif
//    #define NO_SCAN_WHEN_WORKING
	//#define APP_ENV_RESTART_MUSIC_RECOVER               //music recover after exception retart(only recover the current_a2dp_ptr's music)
#else
    #define A2DP_PATCH_FOR_AVRCP
    #define BT_MAX_AG_COUNT                        1
	#define CONFIG_A2DP_PLAYING_SET_AG_FLOW		   0
	#define CONFIG_SELECT_PREV_A2DP_PLAY		   0
	#ifdef CONFIG_TWS
		#define NEED_SNIFF_DEVICE_COUNT 2
	#else
		#define NEED_SNIFF_DEVICE_COUNT 1
	#endif
    #define INQUIRY_ALWAYS
#endif
#if (CONFIG_DEBUG_PCM_TO_UART == 1)
    #ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
        #undef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
    #endif
#endif

#define CONFIG_HFP17_MSBC_SUPPORTED   1

/* #define INCOMINGCALL_HF_TRANSFER_SCO */
//#define INCOMING_CALL_RING //play remote number wave first, and then internal ring whatever remote support inband ring


/*Memorize System Information into Flash, The Address based on Flash Size
    --8,   If 4Mbit Flash  4*1024*1024/8/0x10000
    --32,  If 8Mbit Flash
*/
#define FLASH_LAST_BLOCK_NUM		          (8)

#ifdef MAIN_BOARD_EVB_V1
    #undef ARN
    #define ARP

    #undef ALN
    #define  ALP
#endif

//#define IPHONE_BAT_DISPLAY
#ifdef CONFIG_APP_AEC
#define ADJUST_AEC_DELAY 1
#define CONFIG_APP_DATA_CAPTURE 1
#endif

#ifdef CONFIG_TWS
	#define JUDGE_PIN_STEREO
	#ifdef JUDGE_PIN_STEREO
	#define PIN_STEREO_ROLE		14
	#endif
#endif

//#define CONFIG_OPTIMIZING_SBC 		                     1  // 20161222    应用层可以直接屏蔽

//#define UPDATE_LOCAL_AFH_ENGINE_AFTER_ACK
//#define LMP_LINK_L2CAL_TIMEOUT_ENABLE

#define CONFIG_SBC_DECODE_BITS_EXTEND		0
#define CONFIG_SBC_PROMPT      				1
#define A2DP_SBC_DUMP_SHOW
#define CONFIG_SW_SWITCH_KEY_POWER			0
//芯片常供电拨动开关控制开关机

/*Dual Mode config,added by yangyang*/
#ifdef BT_DUALMODE
	//#define LE_DEBUG_ENABLE                                               //le_debug message print enable or not
	//#define LE_WAVE_PLAY_COVER_MUSIC_POP_ENABLE                         //play prompt wave to give refuge to music "pop"
	#define LE_FIRST_FULL_RX_TIMEOUT_ENBALE                               // first_full_rx_timeout avoid BT disconn among mini LE_CONNECTION_INTERVAL
	#define LE_SLEEP_ENABLE                                               //DM sleep enable
    #define LE_CLOSE_WITH_SCO_ENBALE                                      //close BLE when seco connected
    #define LE_PROTECT_MUSIC_FLUENCY_ENABLE                               //discrad BLE anchor point to ensure music palying fluency(Andriod 6.0 particularly)
#endif


/*************************************************************************
* control version upgraded.
*************************************************************************/
#define UPGRADED_VERSION 0

/*************************************************************************
* just only for pts testing, it should be closed in release version.
*************************************************************************/
#define PTS_TESTING 0

/*************************************************************************
* for a2dp-SRC <-> a2dp-SNK role switch, A2DP_ROLE_DEFAULT may be following value:

* A2DP_ROLE_AS_SRC:
    1. as a2dp source role;
    2. there is no HFP;
    3. we shall only can connect to other bluetooth earphone or voice box as a2dp-SRC role.

* A2DP_ROLE_AS_SNK:
    1. as a2dp sink role.

* A2DP_ROLE_SOURCE_CODE shall be defined as [ 1 ] when using [ A2DP_ROLE_AS_SRC & A2DP_ROLE_AS_SNK_SRC ].
*************************************************************************/
#define A2DP_ROLE_SOURCE_CODE   0

#define A2DP_ROLE_AS_SNK        1
#define A2DP_ROLE_AS_SRC        2
#define A2DP_ROLE_AS_SNK_SRC    3

#define A2DP_ROLE_DEFAULT       A2DP_ROLE_AS_SRC

#if A2DP_ROLE_SOURCE_CODE

    #ifdef CONFIG_BLUETOOTH_HFP
    #undef CONFIG_BLUETOOTH_HFP
    #endif

    #ifdef CONFIG_BLUETOOTH_HSP
    #undef CONFIG_BLUETOOTH_HSP
    #endif

    #ifndef CONFIG_BLUETOOTH_A2DP_SOURCE
    #define CONFIG_BLUETOOTH_A2DP_SOURCE
    #endif

    #ifndef CONFIG_BLUETOOTH_A2DP_SINK
    #define CONFIG_BLUETOOTH_A2DP_SINK
    #endif

    #ifdef CONFIG_APP_AEC
    #undef CONFIG_APP_AEC
    #endif
    #ifdef ADJUST_AEC_DELAY
    #undef ADJUST_AEC_DELAY
    #endif
    #ifdef CONFIG_APP_DATA_CAPTURE
    #undef CONFIG_APP_DATA_CAPTURE
    #endif

#endif
/************************************************
* END FOR A2DP_ROLE_SOURCE_CODE
*************************************************/
#define CONFIG_APP_TOOLKIT_5 1
#define use_4M_flash 0

#if (use_4M_flash == 1)
	#ifdef CONFIG_BLUETOOTH_SPP
	#undef CONFIG_BLUETOOTH_SPP
	#endif
#endif
#if (CONFIG_APP_TOOLKIT_5 == 1)
#define POWERKEY_5S_PARING		0
	#ifdef BT_MAX_AG_COUNT
	#undef BT_MAX_AG_COUNT
	#define BT_MAX_AG_COUNT (app_check_bt_mode(BT_MODE_1V2)?2:1)
	#endif

#endif

/***********************************************************************************
 * Trace32 debug use for only, while "-flto" must be unselected and the target shoud
 * download this Trace32 debug bin firstly by other download tool
 */
#define TRACE32_DEBUG
#ifdef TRACE32_DEBUG
	#ifdef	CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
		#undef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
	#endif
	#ifdef CONFIG_CPU_CLK_OPTIMIZATION
		#undef CONFIG_CPU_CLK_OPTIMIZATION
		#define CONFIG_CPU_CLK_OPTIMIZATION             0
	#endif
#endif
/**********************************************************************************/
#endif

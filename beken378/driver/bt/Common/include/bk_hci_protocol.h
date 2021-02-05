/*************************************************************
 * @file        BK_HCI_Protocol.h
 * @brief       Header file of BK_HCI_Protocol.c
 * @author      GuWenFu
 * @version     V1.0
 * @date        2016-09-29
 * @par         
 * @attention   
 *
 * @history     2016-09-29 gwf    create this file
 */

#ifndef __BK_HCI_PROTOCOL_H__

#define __BK_HCI_PROTOCOL_H__


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */


//#include "types.h"

/*
 * HCI transport type bytes
 */
enum {
    TRA_HCIT_COMMAND = 1,
    TRA_HCIT_ACLDATA = 2,
    TRA_HCIT_SCODATA = 3,
    TRA_HCIT_EVENT   = 4
};


//#define HCI_EVENT_HEAD_LENGTH       0x03
//#define HCI_COMMAND_HEAD_LENGTH     0x04
#define VENDOR_SPECIFIC_DEBUG_OGF   0x3F
//#define BEKEN_OCF                   0xE0
#define HCI_COMMAND_COMPLETE_EVENT  0x0E

enum
{
    BEKEN_UART_LINK_CHECK                       =0x00,  /*return 0x04 0x0e 0x04 0x01 0xe0 0xfc 0x00*/
    BEKEN_UART_REGISTER_WRITE_CMD               =0x01,  /*Write certain reg*/
    BEKEN_UART_REGISTER_CONTINUOUS_WRITE_CMD    =0x02,  /*write reg continuous*/
    BEKEN_UART_REGISTER_READ_CMD                =0x03,  /*read certain reg value*/
    
    BEKEN_UART_BT_START_CMD                     =0x04,  /*useless*/
    BEKEN_UART_BT_STOP_CMD                      =0x05,  /*useless*/
    BEKEN_UART_PATCH_CMD                        =0x06,  /*useless*/
    
    BEKEN_UART_SET_UART_PROTOCOL                =0x07,  /*set uart protocol, H4:00  BCSP:01*/
    
    /*
    01 e0 fc 09 08 aa aa aa aa bb cc dd ee
    aa: baudrate  
    bb: byte size 5,6,7,8
    cc: stop len  1 or 2
    dd: par_en    00:no par 01:par
    ee: par       00:odd 01:even
    */
    BEKEN_UART_SET_UART_CONFIG                  =0x08,  /*set uart config*/
    BEKEN_ENABLE_AFC                            =0x09,  /*enable afc*/
    
    /*
    01 e0 fc 06 0A aa bb cc dd ee
    aa: data_len        0:16bit, N(8,13,14,15): N bit
    bb: speed;          0:200K,1: 1M,2:64K,3:external,4:256K,5:512K,6:128K
    cc: is_msb          0:msb,1:lsb
    dd: role;           0:slave 1:master
    ee: sync_type;      0:short_sync, N(1-7): long_sync with length N
    */
    BEKEN_CONFIG_PCM                            =0x0A,  /*config PCM*/

    //0:normal mode; 1:only stop CPU when acl number = 0; 2:not stop CPU
    BEKEN_CPU_HALT_MODE                         =0x0B,  /*set CPU sleep mode*/
    BEKEN_ENABLE_32K_SLEEP                      =0x0C,  /*enable cpu enter 32K sleep*/
    BEKEN_ENABLE_ANALOG_POWERDOWN               =0x0D,  /*enable close CEVA clock*/
    #if 0 // for UART DOWNLOAD
    BEKEN_ENABLE_GPIO_EINT_WAKEUP               =0x0E,  /*enable GPIO wake up CPU*/
    BEKEN_ENABLE_UART_RX_WAKEUP                 =0x0F,  /*enable uart RX wake up BT chip*/
    BEKEN_SET_UART_RX_WAKEUP_COUNT              =0x10,  /*set UART RX wake up count*/
    #endif
    BEKEN_ENABLE_UART_TX_WAKEUP                 =0x11,  /*enable uart TX wake up HOST*/
    BEKEN_SET_UART_AFTER_WAKEUP_SIG_WAIT_COUNT  =0x12,  /*set UART TX wake up HOST count*/

    BEKEN_MAX_ACL_BUFF_SIZE                     =0x13,  /*useless*/
    BEKEN_ACL_ACTIVE_CHECK_WHEN_SLEEP           =0x14,
    BEKEN_DISABLE_SNIFFER_WHEN_OTHER_LINK_ACTIVE=0x15,
    BEKEN_LM_CONFIG_SLEEP_IN_STANDBY_MONITOR_PERIOD =0x16,
    BEKEN_LM_CONFIG_AWAKE_IN_STANDBY_MONITOR_PERIOD =0x17,
    BEKEN_DISABLE_ACL_ACCEPT_WHEN_ACL_EXIST     =0x18,
    BEKEN_ENABLE_TX_POWER_CONTROL               =0x19,    
    BEKEN_LM_CONFIG_AWAKE_KEEP_WHEN_UART_WAKEUP =0x1A,
    BEKEN_DISABLE_INQUIRY_WHEN_ACL_EXIST        =0x1B,
    BEKEN_DELAY_BETWEEN_EVERY_PACKET_UART_TX    =0x1C,
    BEKEN_SCATTER_LENGTH_FOR_PACKET_UART_TX     =0x1D,
    BEKEN_ENABLE_ACK_SEQ_CHECK                  =0x1E,
    BEKEN_RESET_ACK_SEQ_AFTER_TX                =0x1F,
    BEKEN_GET_FW_VERSION                        =0x20,
    BEKEN_UART_CLOCK_CONFIG_BEFORE_TX_LOW_LEVEL =0x21,
    BEKEN_UART_BAUD_RATE_FOR_TX_LOW_LEVEL       =0x22,
    BEKEN_DELAY1_AFTER_TX_LOW_LEVEL             =0x23,
    BEKEN_DELAY2_AFTER_TX_LOW_LEVEL             =0x24,
    BEKEN_DELAY_FOR_OBEX_PACKET_FINAL           =0x25,
    BEKEN_DISABLE_SPREADTRUM_HCI                =0x26,
    BEKEN_MAX_ACL_BUFF_NUMBER                   =0x27,
    BEKEN_WRITE_ADDR_AFTER_RESET                =0x28,
    
    BEKEN_ENABLE_MASTER_AFC                     =0x2C,
    BEKEN_ENABLE_VIMicro_ENCRYPTION_ISSUE       =0x2D,
    BEKEN_ENABLE_CPU_SPEED_FOR_ECC              =0x2E,
    BEKEN_CHANGE_CPU_CLK                        =0x2F,
    BEKEN_ENABLE_CSR_TX_CRC                     =0x30,
    
    SLEEP_FOR_ATE_POWER_TEST                    =0x32,
    BEKEN_DISALBE_HAREWARE_ERROR_LOG            =0x33,
    BEKEN_ENABLE_ROLE_SWITCH                    =0x34,
    BEKEN_SET_LMP_FEATURES                      =0x35,
    BEKEN_SET_LMP_EXT_FEATURES                  =0x36,
    BEKEN_DISALBE_EDR3                          =0x37,
    BEKEN_DISALBE_2DH1_WHEN_AUTORATE            =0x38,    
    BEKEN_FORCE_DM1_WHEN_LITTLE_PACKET          =0x39,
    BEKEN_ENABLE_QOS                            =0x3A,
//    BEKEN_DISABLE_ESCO                          =0x3A,    
    BEKEN_DELAY_PTT_SET                         =0x3B,    
    BEKEN_SET_32K_WAKUP_TIME                    =0x3C,
    BEKEN_CFG_MIN_SLOTS_FOR_SLEEP_PROCEDURE     =0x3D,
    BEKEN_SET_HOST_WAKEUP_TIME                  =0x3E,
    BEKEN_BT_ACTIVE_PIN_SEL                     =0x3F,
    BEKEN_BT_PRIORITY_PIN_SEL                   =0x40,
    BEKEN_WLAN_ACTIVE_PIN_SEL                   =0x41,
    BEKEN_WLAN_ACTIVE_PIN_POL                   =0x42,
    BEKEN_ENABLE_SOFTIRQ_FOR_UART               =0x43,
    BEKEN_SET_T_SNIFF_MIN                       =0x44,
    BEKEN_SET_T_SNIFF_MAX                       =0x45,
    BEKEN_CHANGE_SNIFF_ATTEMP_WHEN_SNIFF_MIN_IS_SMALL   =0x46,
    BEKEN_CHANGE_SNIFF_TIMEOUT_WHEN_SNIFF_MIN_IS_SMALL  =0x47,
    BEKEN_PTA_TX_DELAY_AFTER_PRIORITY           =0x48,
    BEKEN_PTA_TX_DELAY_AFTER_FREQ_OVERLAP       =0x49,
    BEKEN_PTA_RX_DELAY_AFTER_PRIORITY           =0x4A,
    BEKEN_ENABLE_PTA                            =0x4B,
    BEKEN_ENABLE_CPU_SPEED_FOR_ECC2             =0x4C,
    BEKEN_SPEED_UART_CRC                        =0x4D,
    BEKEN_UART_MODULE_TEST_CMD                  =0x50,
    BEKEN_UART_MODULE_SUB_TEST_CMD              =0x51,
    BEKEN_UART_MODULE_GENERAL_CMD               =0x52,
    BEKEN_ENABLE_AUTO_CHANGE_CPU_CLK            =0x53,

    BEKEN_SHOW_SYSTEM_INFO                      =0x60,
    
    BEKEN_SET_SCO_USE_HCI                       =0x82,
#if (CONFIG_A2DP_PLAYING_SET_AG_FLOW == 1)
   	BEKEN_SET_L2CAP_FLOW						=0x83,
#endif
	BEKEN_EXCHANGE_ACTIVE_ESCO					=0x84,
	
#if (DEBUG_BASEBAND_MONITORS == 1)
    BEKEN_READ_BASEBAND_MONITORS                =0x90,
    BEKEN_RESET_BASEBAND_MONITORS               =0x91,
#endif
#if (DEBUG_AGC_MODE_CHANNEL_ASSESSMENT == 1)
    BEKEN_AGC_MODE_PARAM                        =0x92,
#endif
#if (DEBUG_SCATTERNET_MONITORS == 1)
    BEKEN_READ_SCATTERNET_MONITORS              =0x94,
#endif
	BEKEN_CLEAR_LINKKEY_CMD						=0x95,

    BEKEN_FLASH_READ_CMD                        =0xA0,
    BEKEN_FLASH_WRITE_CMD                       =0xA1,
    BEKEN_FLASH_ERASE_CMD                       =0xA2,
    BEKEN_SHOW_STACK_CMD                        =0xA3, // 0xAA for uart download
    BEKEN_DUMP_ENV_CMD                          =0xAB,
    BEKEN_SHOW_BT_STATUS                        =0xAC,
    BEKEN_SHOW_BT_DEBUG                         =0xAD,
    BEKEN_PRINT_LINK_KEY                        =0XAE,
    BEKEN_ENTRY_DUT_MODE                        =0XAF,
#ifdef CONFIG_TWS
	BEKEN_TWS									=0xB1,
#endif
	BEKEN_LED_EQ_BUTTON							=0xB2,
	BEKEN_SET_AEC_PARA							=0xB3,
	BEKEN_ENTRY_FCC_TESTMODE					=0xFC,
    LOOP_MODE_CMD                               =0XCC,
    BEKEN_TEMP_CMD                              =0XDD,

#ifdef CONFIG_PRODUCT_TEST_INF
    BEKEN_RSSI_CMD                              =0XE0,
#endif

    /* begin: the following cmds just for testing. */
    BEKEN_CMD_SDP_CONNECT                       =0XE1,
    BEKEN_CMD_SERVICE_SEARCH_REQUEST            =0XE2,
    BEKEN_CMD_SERVICE_ATTRIBUTE_REQUEST         =0XE3,
    BEKEN_CMD_A2DP_SSA_REQUEST                  =0XE4,
    BEKEN_CMD_HFP_SSA_REQUEST                   =0XE5,
#if A2DP_ROLE_SOURCE_CODE
    BEKEN_CMD_A2DP_SRC_CONN_REMOTE_DEVICE       =0XE6,
#endif
#if defined(BT_ONE_TO_MULTIPLE) && !defined(CONN_WITH_MUSIC)
	BEKEN_CMD_1V2_POLL_PKT_TYPE     			=0XE8,
#endif	
	BEKEN_TRANSPARENT_HCI_CMD                   =0xF0,
    BEKEN_CMD_SYS_RESET                         =0xFE
    /* end: the cmds just for testing. */

};

#if 0
#ifndef __PACKED_POST__
#define __PACKED_POST__  __attribute__((packed))
#endif

typedef struct {
    u8 code;             /**< 0x01: HCI Command Packet
                              0x02: HCI ACL Data Packet
                              0x03: HCI Synchronous Data Packet
                              0x04: HCI Event Packet */
    struct {
        u16 ocf : 10;    /**< OpCode Command Field */
        u16 ogf : 6;     /**< OpCode Group Field */
    } __PACKED_POST__ opcode;
    u8 total;
    u8 cmd;              /**< private command */
    u8 param[];
} __PACKED_POST__ HCI_COMMAND_PACKET;

typedef struct {
    u8 code;             /**< 0x01: HCI Command Packet
                              0x02: HCI ACL Data Packet
                              0x03: HCI Synchronous Data Packet
                              0x04: HCI Event Packet */
    u8 event;            /**< 0x00-0xFF: Each event is assigned a 1-Octet event code used to uniquely identify different types of events*/
    u8 total;            /**< Parameter Total Length */
    u8 param[];
} __PACKED_POST__ HCI_EVENT_PACKET;

typedef struct {
    u32 addr;
    u32 value;
} __PACKED_POST__ REGISTER_PARAM;
#endif

extern void TRAhcit_UART_Rx(void);


#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif      /* #ifndef __BK_HCI_PROTOCOL_H__ */

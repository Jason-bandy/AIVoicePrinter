// Copyright 2015-2024 Beken
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <rtthread.h>
#include <finsh.h>
#include "common.h"
#include "param_config.h"
#include "ble_pub.h"

#if ((CFG_SUPPORT_BLE) && (CFG_BLE_USE_CLI))
#include "ble.h"

#if (CFG_BLE_VERSION == BLE_VERSION_4_2)
#include "application.h"
#include "ble_api.h"

#define BUILD_UINT16(loByte, hiByte) \
          ((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#define BK_ATT_DECL_PRIMARY_SERVICE_128     {0x00,0x28,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define BK_ATT_DECL_CHARACTERISTIC_128      {0x03,0x28,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define BK_ATT_DESC_CLIENT_CHAR_CFG_128     {0x02,0x29,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

#define WRITE_REQ_CHARACTERISTIC_128        {0x01,0xFF,0,0,0x34,0x56,0,0,0,0,0x28,0x37,0,0,0,0}
#define INDICATE_CHARACTERISTIC_128         {0x02,0xFF,0,0,0x34,0x56,0,0,0,0,0x28,0x37,0,0,0,0}
#define NOTIFY_CHARACTERISTIC_128           {0x03,0xFF,0,0,0x34,0x56,0,0,0,0,0x28,0x37,0,0,0,0}


static const uint8_t test_svc_uuid[16] = {0xFF,0xFF,0,0,0x34,0x56,0,0,0,0,0x28,0x37,0,0,0,0};

enum
{
    TEST_IDX_SVC,
    TEST_IDX_FF01_VAL_CHAR,
    TEST_IDX_FF01_VAL_VALUE,
    TEST_IDX_FF02_VAL_CHAR,
    TEST_IDX_FF02_VAL_VALUE,
    TEST_IDX_FF02_VAL_IND_CFG,
    TEST_IDX_FF03_VAL_CHAR,
    TEST_IDX_FF03_VAL_VALUE,
    TEST_IDX_FF03_VAL_NTF_CFG,
    TEST_IDX_NB,
};

bk_attm_desc_t test_att_db[TEST_IDX_NB] =
{
    //  Service Declaration
    [TEST_IDX_SVC]              = {BK_ATT_DECL_PRIMARY_SERVICE_128, BK_PERM_SET(RD, ENABLE), 0, 0},

    //  Level Characteristic Declaration
    [TEST_IDX_FF01_VAL_CHAR]    = {BK_ATT_DECL_CHARACTERISTIC_128,  BK_PERM_SET(RD, ENABLE), 0, 0},
    //  Level Characteristic Value
    [TEST_IDX_FF01_VAL_VALUE]   = {WRITE_REQ_CHARACTERISTIC_128,    BK_PERM_SET(WRITE_REQ, ENABLE), BK_PERM_SET(RI, ENABLE), 128},

    [TEST_IDX_FF02_VAL_CHAR]    = {BK_ATT_DECL_CHARACTERISTIC_128,  BK_PERM_SET(RD, ENABLE), 0, 0},
    //  Level Characteristic Value
    [TEST_IDX_FF02_VAL_VALUE]   = {INDICATE_CHARACTERISTIC_128,     BK_PERM_SET(IND, ENABLE), BK_PERM_SET(RI, ENABLE), 128},

    //  Level Characteristic - Client Characteristic Configuration Descriptor

    [TEST_IDX_FF02_VAL_IND_CFG] = {BK_ATT_DESC_CLIENT_CHAR_CFG_128, BK_PERM_SET(RD, ENABLE)|BK_PERM_SET(WRITE_REQ, ENABLE), 0, 0},
    [TEST_IDX_FF03_VAL_CHAR] = {BK_ATT_DECL_CHARACTERISTIC_128,	BK_PERM_SET(RD, ENABLE), 0, 0},
    //  Level Characteristic Value
    [TEST_IDX_FF03_VAL_VALUE] = {NOTIFY_CHARACTERISTIC_128,	   BK_PERM_SET(NTF, ENABLE), BK_PERM_SET(RI, ENABLE)|BK_PERM_SET(UUID_LEN, UUID_16), 128},
    //  Level Characteristic - Client Characteristic Configuration Descriptor
    [TEST_IDX_FF03_VAL_NTF_CFG] = {BK_ATT_DESC_CLIENT_CHAR_CFG_128, BK_PERM_SET(RD, ENABLE)|BK_PERM_SET(WRITE_REQ, ENABLE), 0, 0},

};

ble_err_t bk_ble_init(void)
{
    ble_err_t status;
    struct bk_ble_db_cfg ble_db_cfg;

    ble_db_cfg.att_db = test_att_db;
    ble_db_cfg.att_db_nb = TEST_IDX_NB;
    ble_db_cfg.prf_task_id = 0;
    ble_db_cfg.start_hdl = 0;
    ble_db_cfg.svc_perm = 0;
    memcpy(&(ble_db_cfg.uuid[0]), &test_svc_uuid[0], 16);

    status = bk_ble_create_db(&ble_db_cfg);

    return status;
}

void appm_adv_data_decode(uint8_t len, const uint8_t *data)
{
    uint8_t index;
    uint8_t i;
    for(index = 0; index < len;)
    {
        switch(data[index + 1])
        {
        case 0x01:
        {
            bk_printf("AD_TYPE : ");
            for(i = 0; i < data[index] - 1; i++)
            {
                bk_printf("%02x ",data[index + 2 + i]);
            }
            bk_printf("\r\n");
            index +=(data[index] + 1);
        }
        break;
        case 0x08:
        case 0x09:
        {
            bk_printf("ADV_NAME : ");
            for(i = 0; i < data[index] - 1; i++)
            {
                bk_printf("%c",data[index + 2 + i]);
            }
            bk_printf("\r\n");
            index +=(data[index] + 1);
        }
        break;
        case 0x02:
        {
            bk_printf("UUID : ");
            for(i = 0; i < data[index] - 1;)
            {
                bk_printf("%02x%02x  ",data[index + 2 + i],data[index + 3 + i]);
                i+=2;
            }
            bk_printf("\r\n");
            index +=(data[index] + 1);
        }
        break;
        default:
        {
            index +=(data[index] + 1);
        }
        break;
        }
    }
    return ;
}

void ble_write_callback(write_req_t *write_req)
{
    bk_printf("write_cb[prf_id:%d, att_idx:%d, len:%d]\r\n", write_req->prf_id, write_req->att_idx, write_req->len);
}

uint8_t ble_read_callback(read_req_t *read_req)
{
    bk_printf("read_cb[prf_id:%d, att_idx:%d]\r\n", read_req->prf_id, read_req->att_idx);
    read_req->value[0] = 0x10;
    read_req->value[1] = 0x20;
    read_req->value[2] = 0x30;
    return 3;
}

void ble_event_callback(ble_event_t event, void *param)
{
    switch(event)
    {
    case BLE_STACK_OK:
    {
        bk_printf("STACK INIT OK\r\n");
        bk_ble_init();
    }
    break;
    case BLE_STACK_FAIL:
    {
        bk_printf("STACK INIT FAIL\r\n");
    }
    break;
    case BLE_CONNECT:
    {
        bk_printf("BLE CONNECT\r\n");
    }
    break;
    case BLE_DISCONNECT:
    {
        bk_printf("BLE DISCONNECT\r\n");
    }
    break;
    case BLE_MTU_CHANGE:
    {
        bk_printf("BLE_MTU_CHANGE:%d\r\n", *(uint16_t *)param);
    }
    break;
    case BLE_TX_DONE:
    {
        bk_printf("BLE_TX_DONE\r\n");
    }
    break;
    case BLE_GEN_DH_KEY:
    {
        bk_printf("BLE_GEN_DH_KEY\r\n");
    }
    break;
    case BLE_GET_KEY:
    {
        bk_printf("BLE_GET_KEY\r\n");
    }
    break;
    case BLE_CREATE_DB_OK:
    {
        bk_printf("CREATE DB SUCCESS\r\n");
    }
    break;
    default:
        bk_printf("UNKNOW EVENT\r\n");
        break;
    }
}

static void ble_command_usage(void)
{
    bk_printf("ble help           - Help information\n");
    bk_printf("ble active         - Active ble to with BK7231BTxxx\n");
    bk_printf("ble start_adv      - Start advertising as a slave device\n");
    bk_printf("ble stop_adv       - Stop advertising as a slave device\n");
    bk_printf("ble notify prf_id att_id value\n");
    bk_printf("                   - Send ntf value to master\n");
    bk_printf("ble indicate prf_id att_id value\n");
    bk_printf("                   - Send ind value to master\n");

    bk_printf("ble disc           - Disconnect\n");
    bk_printf("ble dut            - dut test\n");
}

__maybe_unused static void ble_get_info_handler(void);
static void ble_get_info_handler(void)
{
    UINT8 *ble_mac;
    bk_printf("\r\n****** ble information ************\r\n");

    if (ble_is_start() == 0) {
        bk_printf("no ble startup          \r\n");
        return;
    }
    ble_mac = ble_get_mac_addr();
    bk_printf("* name: %s             \r\n", ble_get_name());
    bk_printf("* mac:%02x-%02x-%02x-%02x-%02x-%02x\r\n", ble_mac[0], ble_mac[1],ble_mac[2],ble_mac[3],ble_mac[4],ble_mac[5]);
    bk_printf("***********  end  *****************\r\n");
}

typedef adv_info_t ble_adv_param_t;

static void ble_advertise(void)
{
    UINT8 mac[6];
    char ble_name[20];
    UINT8 adv_idx, adv_name_len;

    wifi_get_mac_address((char *)mac, CONFIG_ROLE_STA);
    adv_name_len = snprintf(ble_name, sizeof(ble_name), "bk72xx-%02x%02x", mac[4], mac[5]);

    memset(&adv_info, 0x00, sizeof(adv_info));

    adv_info.channel_map = 7;
    adv_info.interval_min = 160;
    adv_info.interval_max = 160;

    adv_idx = 0;
    adv_info.advData[adv_idx] = 0x02;
    adv_idx++;
    adv_info.advData[adv_idx] = 0x01;
    adv_idx++;
    adv_info.advData[adv_idx] = 0x06;
    adv_idx++;

    adv_info.advData[adv_idx] = adv_name_len + 1;
    adv_idx +=1;
    adv_info.advData[adv_idx] = 0x09;
    adv_idx +=1; //name
    memcpy(&adv_info.advData[adv_idx], ble_name, adv_name_len);
    adv_idx +=adv_name_len;

    adv_info.advDataLen = adv_idx;

    adv_idx = 0;

    adv_info.respData[adv_idx] = adv_name_len + 1;
    adv_idx +=1;
    adv_info.respData[adv_idx] = 0x08;
    adv_idx +=1; //name
    memcpy(&adv_info.respData[adv_idx], ble_name, adv_name_len);
    adv_idx +=adv_name_len;
    adv_info.respDataLen = adv_idx;

    if (ERR_SUCCESS != appm_start_advertising())
    {
        bk_printf("ERROR\r\n");
    }
}

static void ble(int argc, char **argv)
{
    if ((argc < 2) || (os_strcmp(argv[1], "help") == 0))
    {
        ble_command_usage();
        return ;
    }

    if (os_strcmp(argv[1], "active") == 0)
    {
        ble_set_write_cb(ble_write_callback);
        ble_set_read_cb(ble_read_callback);
        ble_set_event_cb(ble_event_callback);
        ble_activate(NULL);
    }
    else if(os_strcmp(argv[1], "start_adv") == 0)
    {
        ble_advertise();
    }
    else if(os_strcmp(argv[1], "stop_adv") == 0)
    {
        if(ERR_SUCCESS != appm_stop_advertising())
        {
            bk_printf("ERROR\r\n");
        }
    }
    else if(os_strcmp(argv[1], "notify") == 0)
    {
        uint8 len;
        uint16 prf_id;
        uint16 att_id;
        uint8 write_buffer[20];

        if(argc != 5)
        {
            ble_command_usage();
            return ;
        }

        len = os_strlen(argv[4]);
        if((len % 2 != 0) || (len > 40))
        {
            bk_printf("notify buffer len error\r\n");
            return ;
        }
        hexstr2bin(argv[4], write_buffer, len / 2);

        prf_id = atoi(argv[2]);
        att_id = atoi(argv[3]);

        if(ERR_SUCCESS != bk_ble_send_ntf_value(len / 2, write_buffer, prf_id, att_id))
        {
            bk_printf("ERROR\r\n");
        }
    }
    else if(os_strcmp(argv[1], "indicate") == 0)
    {
        uint8 len;
        uint16 prf_id;
        uint16 att_id;
        uint8 write_buffer[20];

        if(argc != 5)
        {
            ble_command_usage();
            return ;
        }

        len = os_strlen(argv[4]);
        if((len % 2 != 0) || (len > 40))
        {
            bk_printf("indicate buffer len error\r\n");
            return ;
        }
        hexstr2bin(argv[4], write_buffer, len / 2);

        prf_id = atoi(argv[2]);
        att_id = atoi(argv[3]);

        if(ERR_SUCCESS != bk_ble_send_ind_value(len / 2, write_buffer, prf_id, att_id))
        {
            bk_printf("ERROR\r\n");
        }
    }
    else if(os_strcmp(argv[1], "disc") == 0)
    {
        appm_disconnect();
    }
    else if(os_strcmp(argv[1], "dut") == 0)
    {
        ble_dut_start();
    }
}

MSH_CMD_EXPORT(ble, ble command);
#endif

#if (CFG_BLE_VERSION == BLE_VERSION_5_1) || (CFG_BLE_VERSION == BLE_VERSION_5_2)
#include "ble_api_5_x.h"
#include "rwprf_config.h"
#include "app_ble.h"
#include "app_sdp.h"
#include "app_ble_init.h"
#if (BLE_APP_SEC)
#include "app_sec.h"
#endif
#include "kernel_mem.h"
#include "cmd_evm.h"


#define BUILD_UINT16(loByte, hiByte) \
          ((uint16_t)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#define BK_ATT_DECL_PRIMARY_SERVICE_128     {0x00,0x28,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define BK_ATT_DECL_CHARACTERISTIC_128      {0x03,0x28,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define BK_ATT_DESC_CLIENT_CHAR_CFG_128     {0x02,0x29,0,0,0,0,0,0,0,0,0,0,0,0,0,0}

#define WRITE_REQ_CHARACTERISTIC_128        {0x01,0xFF,0,0,0x34,0x56,0,0,0,0,0x28,0x37,0,0,0,0}
#define INDICATE_CHARACTERISTIC_128         {0x02,0xFF,0,0,0x34,0x56,0,0,0,0,0x28,0x37,0,0,0,0}
#define NOTIFY_CHARACTERISTIC_128           {0x03,0xFF,0,0,0x34,0x56,0,0,0,0,0x28,0x37,0,0,0,0}

static const uint8_t test_svc_uuid[16] = {0xFF,0xFF,0,0,0x34,0x56,0,0,0,0,0x28,0x37,0,0,0,0};

enum
{
    TEST_IDX_SVC,
    TEST_IDX_FF01_VAL_CHAR,
    TEST_IDX_FF01_VAL_VALUE,
    TEST_IDX_FF02_VAL_CHAR,
    TEST_IDX_FF02_VAL_VALUE,
    TEST_IDX_FF02_VAL_IND_CFG,
    TEST_IDX_FF03_VAL_CHAR,
    TEST_IDX_FF03_VAL_VALUE,
    TEST_IDX_FF03_VAL_NTF_CFG,
    TEST_IDX_NB,
};

#if (CFG_BLE_VERSION == BLE_VERSION_5_1)
bk_attm_desc_t test_att_db[TEST_IDX_NB] =
{
    //  Service Declaration
    [TEST_IDX_SVC]              = {BK_ATT_DECL_PRIMARY_SERVICE_128, BK_PERM_SET(RD, ENABLE), 0, 0},

    //  Level Characteristic Declaration
    [TEST_IDX_FF01_VAL_CHAR]    = {BK_ATT_DECL_CHARACTERISTIC_128,  BK_PERM_SET(RD, ENABLE), 0, 0},
    //  Level Characteristic Value
    [TEST_IDX_FF01_VAL_VALUE]   = {WRITE_REQ_CHARACTERISTIC_128,    BK_PERM_SET(WRITE_REQ, ENABLE), BK_PERM_SET(RI, ENABLE)|BK_PERM_SET(UUID_LEN, UUID_16), 128},

    [TEST_IDX_FF02_VAL_CHAR]    = {BK_ATT_DECL_CHARACTERISTIC_128,  BK_PERM_SET(RD, ENABLE), 0, 0},
    //  Level Characteristic Value
    [TEST_IDX_FF02_VAL_VALUE]   = {INDICATE_CHARACTERISTIC_128,     BK_PERM_SET(IND, ENABLE), BK_PERM_SET(RI, ENABLE)|BK_PERM_SET(UUID_LEN, UUID_16), 128},

    //  Level Characteristic - Client Characteristic Configuration Descriptor

    [TEST_IDX_FF02_VAL_IND_CFG] = {BK_ATT_DESC_CLIENT_CHAR_CFG_128, BK_PERM_SET(RD, ENABLE)|BK_PERM_SET(WRITE_REQ, ENABLE), 0, 0},

    [TEST_IDX_FF03_VAL_CHAR]    = {BK_ATT_DECL_CHARACTERISTIC_128,  BK_PERM_SET(RD, ENABLE), 0, 0},
    //  Level Characteristic Value
    [TEST_IDX_FF03_VAL_VALUE]   = {NOTIFY_CHARACTERISTIC_128,       BK_PERM_SET(NTF, ENABLE), BK_PERM_SET(RI, ENABLE)|BK_PERM_SET(UUID_LEN, UUID_16), 128},

    //  Level Characteristic - Client Characteristic Configuration Descriptor

    [TEST_IDX_FF03_VAL_NTF_CFG] = {BK_ATT_DESC_CLIENT_CHAR_CFG_128, BK_PERM_SET(RD, ENABLE)|BK_PERM_SET(WRITE_REQ, ENABLE), 0, 0},
};
#elif (CFG_BLE_VERSION == BLE_VERSION_5_2)
bk_attm_desc_t test_att_db[TEST_IDX_NB] =
{
    //  Service Declaration
    [TEST_IDX_SVC]              = {BK_ATT_DECL_PRIMARY_SERVICE_128, PROP(RD), 0},

    //  Level Characteristic Declaration
    [TEST_IDX_FF01_VAL_CHAR]    = {BK_ATT_DECL_CHARACTERISTIC_128,  PROP(RD), 0},
    //  Level Characteristic Value
    [TEST_IDX_FF01_VAL_VALUE]   = {WRITE_REQ_CHARACTERISTIC_128,    PROP(WR), 128|OPT(NO_OFFSET)},

    [TEST_IDX_FF02_VAL_CHAR]    = {BK_ATT_DECL_CHARACTERISTIC_128,  PROP(RD), 0},
    //  Level Characteristic Value
    [TEST_IDX_FF02_VAL_VALUE]   = {INDICATE_CHARACTERISTIC_128,     PROP(I), 128|OPT(NO_OFFSET)},

    //  Level Characteristic - Client Characteristic Configuration Descriptor

    [TEST_IDX_FF02_VAL_IND_CFG] = {BK_ATT_DESC_CLIENT_CHAR_CFG_128, PROP(RD)|PROP(WR),OPT(NO_OFFSET)},

    [TEST_IDX_FF03_VAL_CHAR]    = {BK_ATT_DECL_CHARACTERISTIC_128,  PROP(RD), 0},
    //  Level Characteristic Value
    [TEST_IDX_FF03_VAL_VALUE]   = {NOTIFY_CHARACTERISTIC_128,       PROP(N), 128|OPT(NO_OFFSET)},

    //  Level Characteristic - Client Characteristic Configuration Descriptor

    [TEST_IDX_FF03_VAL_NTF_CFG] = {BK_ATT_DESC_CLIENT_CHAR_CFG_128, PROP(RD)|PROP(WR), OPT(NO_OFFSET)},
};
#endif

ble_err_t bk_ble_init(void)
{
    ble_err_t status;
    struct bk_ble_db_cfg ble_db_cfg;

    ble_db_cfg.att_db = test_att_db;
    ble_db_cfg.att_db_nb = TEST_IDX_NB;
    ble_db_cfg.prf_task_id = 0;
    ble_db_cfg.start_hdl = 0;
    ble_db_cfg.svc_perm = 0;
    memcpy(&(ble_db_cfg.uuid[0]), &test_svc_uuid[0], 16);

    status = bk_ble_create_db(&ble_db_cfg);

    return status;
}

void ble_cmd_cb(ble_cmd_t cmd, ble_cmd_param_t *param)
{
    bk_printf("cmd:%d idx:%d status:%d\r\n", cmd, param->cmd_idx, param->status);
}

void ble_notice_cb(ble_notice_t notice, void *param)
{
    switch (notice) {
    case BLE_5_STACK_OK:
        bk_printf("ble stack ok");
        break;
    case BLE_5_WRITE_EVENT:
    {
        write_req_t *w_req = (write_req_t *)param;
        bk_printf("write_cb:conn_idx:%d, prf_id:%d, add_id:%d, len:%d, data[0]:%02x\r\n",
                  w_req->conn_idx, w_req->prf_id, w_req->att_idx, w_req->len, w_req->value[0]);
        break;
    }
    case BLE_5_READ_EVENT:
    {
        read_req_t *r_req = (read_req_t *)param;
        bk_printf("read_cb:conn_idx:%d, prf_id:%d, add_id:%d\r\n",
                  r_req->conn_idx, r_req->prf_id, r_req->att_idx);

        #if (CFG_BLE_VERSION == BLE_VERSION_5_2)
        uint16_t length = 3;
        r_req->value = kernel_malloc(length, KERNEL_MEM_KERNEL_MSG);
        r_req->value[0] = 0x12;
        r_req->value[1] = 0x34;
        r_req->value[2] = 0x56;

        app_gatts_rsp_t rsp;
        rsp.token = r_req->token;
        rsp.con_idx = r_req->conn_idx;
        rsp.attr_handle = r_req->hdl;
        rsp.status = GAP_ERR_NO_ERROR;
        rsp.att_length = length;
        rsp.value_length = length;
        rsp.value = r_req->value;

        app_ble_gatts_set_attr_value(rsp.attr_handle, rsp.value_length, rsp.value);
        bk_ble_gatts_read_response(&rsp);
        kernel_free(r_req->value);
        #else
        r_req->value[0] = 0x12;
        r_req->value[1] = 0x34;
        r_req->value[2] = 0x56;
        r_req->length = 3;
        #endif

        break;
    }
    case BLE_5_REPORT_ADV:
    {
        recv_adv_t *r_ind = (recv_adv_t *)param;

        bk_printf("[%s]r_ind:actv_idx:%d,evt_type:%d adv_addr:%02x:%02x:%02x:%02x:%02x:%02x,rssi:%d\r\n",
                  ((r_ind->evt_type&0x7) == 3)?"scan-rsp":((r_ind->evt_type&0x7) == 1)?"adv":"unknow",
                  r_ind->actv_idx,r_ind->evt_type, r_ind->adv_addr[0], r_ind->adv_addr[1], r_ind->adv_addr[2],
                  r_ind->adv_addr[3], r_ind->adv_addr[4], r_ind->adv_addr[5],r_ind->rssi);
        break;
    }
    case BLE_5_REPORT_PER_ADV:
    {
        recv_adv_t *r_ind = (recv_adv_t *)param;

        bk_printf("[%s]r_ind:actv_idx:%d,evt_type:%d rssi:%d data_len:%d data[0]:0x%x\r\n","per-adv",
                  r_ind->actv_idx,r_ind->evt_type, r_ind->rssi, r_ind->data_len, r_ind->data[0]);
        break;
    }
    case BLE_5_MTU_CHANGE:
    {
        mtu_change_t *m_ind = (mtu_change_t *)param;
        bk_printf("BLE_5_MTU_CHANGE:conn_idx:%d, mtu_size:%d\r\n", m_ind->conn_idx, m_ind->mtu_size);
        break;
    }
    case BLE_5_PHY_IND_EVENT:
    {
        conn_phy_ind_t *set_phy = (conn_phy_ind_t *)param;
        bk_printf("BLE_5_PHY_IND_EVENT:conn_idx:%d, tx_phy:0x%x, rx_phy:0x%x\r\n", set_phy->conn_idx, set_phy->tx_phy, set_phy->rx_phy);
        break;
    }
    case BLE_5_CONNECT_EVENT:
    {
        conn_ind_t *c_ind = (conn_ind_t *)param;
        bk_printf("c_ind:conn_idx:%d, addr_type:%d, peer_addr:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
                  c_ind->conn_idx, c_ind->peer_addr_type, c_ind->peer_addr[0], c_ind->peer_addr[1],
                  c_ind->peer_addr[2], c_ind->peer_addr[3], c_ind->peer_addr[4], c_ind->peer_addr[5]);
        break;
    }
    case BLE_5_DISCONNECT_EVENT:
    {
        discon_ind_t *d_ind = (discon_ind_t *)param;
        bk_printf("d_ind:conn_idx:%d,reason:%d\r\n", d_ind->conn_idx,d_ind->reason);
        break;
    }
    case BLE_5_ATT_INFO_REQ:
    {
        att_info_req_t *a_ind = (att_info_req_t *)param;
        bk_printf("a_ind:conn_idx:%d\r\n", a_ind->conn_idx);
        a_ind->length = 128;
        a_ind->status = ERR_SUCCESS;
        break;
    }
    case BLE_5_CREATE_DB:
    {
        create_db_t *cd_ind = (create_db_t *)param;
        bk_printf("cd_ind:prf_id:%d, status:%d\r\n", cd_ind->prf_id, cd_ind->status);
        break;
    }
    #if (BLE_CENTRAL)
    case BLE_5_INIT_CONNECT_EVENT:
    {
        conn_ind_t *c_ind = (conn_ind_t *)param;
        #if (CFG_BLE_VERSION == BLE_VERSION_5_2)
        app_ble_get_peer_feature(c_ind->conn_idx);
        app_ble_set_le_pkt_size(c_ind->conn_idx,LE_MAX_OCTETS);
        app_ble_mtu_exchange(c_ind->conn_idx);
        sdp_discover_all_service(c_ind->conn_idx);
        #endif
        bk_printf("BLE_5_INIT_CONNECT_EVENT:conn_idx:%d, addr_type:%d, peer_addr:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
                  c_ind->conn_idx, c_ind->peer_addr_type, c_ind->peer_addr[0], c_ind->peer_addr[1],
                  c_ind->peer_addr[2], c_ind->peer_addr[3], c_ind->peer_addr[4], c_ind->peer_addr[5]);
        break;
    }
    case BLE_5_INIT_DISCONNECT_EVENT:
    {
        discon_ind_t *d_ind = (discon_ind_t *)param;
        bk_printf("BLE_5_INIT_DISCONNECT_EVENT:conn_idx:%d,reason:0x%x\r\n", d_ind->conn_idx,d_ind->reason);
        break;
    }
    #endif
    case BLE_5_INIT_CONN_PARAM_UPDATE_REQ_EVENT:
    {
        conn_param_req_t *d_ind = (conn_param_req_t *)param;
        bk_printf("BLE_5_INIT_CONN_PARAM_UPDATE_REQ_EVENT:conn_idx:%d,intv_min:%d,intv_max:%d,time_out:%d\r\n",d_ind->conn_idx,
                  d_ind->intv_min,d_ind->intv_max,d_ind->time_out);
    }
    break;
    case BLE_5_INIT_CONN_PARAM_UPDATE_IND_EVENT:
    {
        conn_update_ind_t *d_ind = (conn_update_ind_t *)param;
        bk_printf("BLE_5_INIT_CONN_PARAM_UPDATE_IND_EVENT:conn_idx:%d,interval:%d,time_out:%d,latency\r\n",d_ind->conn_idx,
                  d_ind->interval,d_ind->time_out,d_ind->latency);
    }
    break;
    case BLE_5_SDP_REGISTER_FAILED:
        bk_printf("BLE_5_SDP_REGISTER_FAILED\r\n");
        break;
    case BLE_5_GAP_CMD_CMP_EVENT:
    {
        ble_cmd_cmp_evt_t *evt = (ble_cmd_cmp_evt_t *)param;
        bk_printf("BLE_5_GAP_CMD_CMP_EVENT cmd:0x%x,conn_idx:%d,status:0x%x\r\n",evt->cmd,evt->conn_idx,evt->status);
        break;
    }
    case BLE_5_TX_DONE:
    {
        atts_tx_t *evt = (atts_tx_t *)param;
        bk_printf("BLE_5_TX_DONE conn_idx:%d,prf_id:%d,att_idx:%d,status:%d\r\n",
                  evt->conn_idx,evt->prf_id,evt->att_idx,evt->status);
    }
    break;
    default:
        break;
    }
}

#if BLE_CENTRAL && (CFG_SOC_NAME == SOC_BK7231N)
static void ble_app_sdp_characteristic_cb(unsigned char conidx,uint16_t chars_val_hdl,unsigned char uuid_len,unsigned char *uuid)
{
    bk_printf("[APP]characteristic conidx:%d,handle:0x%02x(%d),UUID:0x",conidx,chars_val_hdl,chars_val_hdl);
    for(int i = 0; i< uuid_len; i++)
    {
        bk_printf("%02x ",uuid[i]);
    }
    bk_printf("\r\n");
}

void app_sdp_charac_cb(CHAR_TYPE type,uint8 conidx,uint16_t hdl,uint16_t len,uint8 *data)
{
    bk_printf("[APP]type:%x conidx:%d,handle:0x%02x(%d),len:%d,0x",type,conidx,hdl,hdl,len);
    for(int i = 0; i< len; i++)
    {
        bk_printf("%02x ",data[i]);
    }
    bk_printf("\r\n");
}
static const app_sdp_service_uuid service_tab[] = {
    {
        .uuid_len = 0x02,
        .uuid[0] = 0x00,
        .uuid[1] = 0x18,
    },
    {
        .uuid_len = 0x02,
        .uuid[0] = 0x01,
        .uuid[1] = 0x18,
    },
};

#elif (CFG_BLE_VERSION == BLE_VERSION_5_2) && (BLE_GATT_CLI)
void sdp_event_cb(sdp_notice_t notice, void *param)
{
    switch (notice) {
    case SDP_CHARAC_NOTIFY_EVENT:
    {
        sdp_event_t *g_sdp = (sdp_event_t *)param;
        bk_printf("[SDP_CHARAC_NOTIFY_EVENT]con_idx:%d,hdl:0x%x,value_length:%d\r\n",g_sdp->con_idx,g_sdp->hdl,g_sdp->value_length);
    }
    break;
    case SDP_CHARAC_INDICATE_EVENT:
    {
        sdp_event_t *g_sdp = (sdp_event_t *)param;
        bk_printf("[SDP_CHARAC_INDICATE_EVENT]con_idx:%d,hdl:0x%x,value_length:%d\r\n",g_sdp->con_idx,g_sdp->hdl,g_sdp->value_length);
    }
    break;
    case SDP_CHARAC_READ:
    {
        sdp_event_t *g_sdp = (sdp_event_t *)param;
        bk_printf("[SDP_CHARAC_READ]con_idx:%d,hdl:0x%x,value_length:%d\r\n",g_sdp->con_idx,g_sdp->hdl,g_sdp->value_length);
    }
    break;
    case SDP_DISCOVER_SVR_DONE:
    {
        bk_printf("[SDP_DISCOVER_SVR_DONE]\r\n");
    }
    break;
    case SDP_CHARAC_WRITE_DONE:
    {
        bk_printf("[SDP_CHARAC_WRITE_DONE]\r\n");
    }
    break;
    default:
        bk_printf("[%s]Event:%d\r\n",__func__,notice);
        break;
    }
}
#endif

#define BLE_VSN5_DEFAULT_MASTER_IDX      0
#if BLE_BATT_SERVER
#include "app_bass.h"
#elif BLE_HID_DEVICE
#include "app_hogpd.h"
#elif BLE_FINDME_TARGET
#include "app_findt.h"
#elif BLE_DIS_SERVER
#include "app_diss.h"
#endif

#if (BLE_BATT_SERVER | BLE_HID_DEVICE | BLE_FINDME_TARGET | BLE_DIS_SERVER)
void profile_notice_cb(ble_notice_t notice, void *param)
{
    switch (notice) {
    case BLE_5_STACK_OK:
    {
        bk_printf("ble stack ok");
        break;
    }
    case BLE_5_CONNECT_EVENT:
    {
        conn_ind_t *c_ind = (conn_ind_t *)param;
        bk_printf("c_ind:conn_idx:%d, addr_type:%d, peer_addr:%02x:%02x:%02x:%02x:%02x:%02x\r\n",
                  c_ind->conn_idx, c_ind->peer_addr_type, c_ind->peer_addr[0], c_ind->peer_addr[1],
                  c_ind->peer_addr[2], c_ind->peer_addr[3], c_ind->peer_addr[4], c_ind->peer_addr[5]);
        break;
    }
    case BLE_5_DISCONNECT_EVENT:
    {
        discon_ind_t *d_ind = (discon_ind_t *)param;
        bk_printf("d_ind:conn_idx:%d,reason:%d\r\n", d_ind->conn_idx,d_ind->reason);
        break;
    }
    default:
        break;
    }
}
#endif

#if BLE_APP_SEC
void security_notice_cb(sec_notice_t notice, void *param)
{
    switch (notice) {
        #if (CFG_BLE_VERSION == BLE_VERSION_5_2)
    case APP_SEC_SECURITY_REQ_IND:
    {
        uint8_t *conn_idx = (uint8_t *)param;
        bk_ble_gap_security_rsp(*conn_idx, true);
    }
    break;
    case APP_SEC_PAIRING_REQ_IND:
    {
        uint8_t *conn_idx = (uint8_t *)param;
        bk_ble_gap_pairing_rsp(*conn_idx, true);
    }
    break;
    case APP_SEC_PASSKEY_REPLY:
    {
        uint8_t *conn_idx = (uint8_t *)param;
        uint32_t tk = 123456;
        bk_ble_passkey_reply(*conn_idx, true, tk);
        bk_printf("tk: %d\r\n", tk);
    }
    break;
    case APP_SEC_CONFIRM_REPLY:
    {
        numeric_cmp_t *num_par = (numeric_cmp_t *)param;
        bk_printf("Exchange of Numeric Value: %d", num_par->num_value);
        bk_ble_confirm_reply(num_par->conn_idx, true);
    }
    break;
    #endif
    case APP_SEC_PAIRING_SUCCEED:
    {
        bk_printf("BLE PAIRING SUCCEED, bonded status = 0x%x\r\n", app_sec_env.bonded);
    }
    break;
    case APP_SEC_PAIRING_FAILED:
    {
        bk_printf("[WARNING]BLE PAIRING FAILED, bonded status = 0x%x\r\n", app_sec_env.bonded);
    }
    break;
    case APP_SEC_ENCRYPT_SUCCEED:
    {
        bk_printf("BLE ENCRYPTION SUCCEED\r\n");
    }
    break;
    default:
        break;
    }
}
#endif

static void ble(int argc, char **argv)
{
    uint8_t adv_data[31];
    uint8_t actv_idx;

    if(os_strcmp(argv[1],"pta")==0) {
        uint32_t enable = os_strtoul(argv[2], NULL, 10);
        ble_coex_set_pta(enable ? true : false);
    }

    if(os_strcmp(argv[1],"exit")==0) {
        ble_thread_exit();
    }
    if(os_strcmp(argv[1],"notify")==0) {
        uint32_t len;
        uint16 prf_id;
        uint8 write_buffer[128]= {0};
        len=os_strtoul(argv[3], NULL, 10);
        if(argc!=4) {
            bk_printf("notify arg %d error\r\n",argc);
            return;
        } else {
            if(len>128 || len<4) {
                bk_printf("The length of the notify should be between 4 and 128 \r\n");
                if(len<4) {
                    bk_printf("Output 4 bytes\r\n");
                    len=4;
                } else {
                    bk_printf("Output the first 128 bytes\r\n");
                    len=128;
                }
            }
            for(int i=0; i<4; i++) {
                write_buffer[i]=rand()%257;
            }
        }
        prf_id=atoi(argv[2]);
        if(ERR_SUCCESS!=bk_ble_send_ntf_value(len, write_buffer,prf_id,TEST_IDX_FF03_VAL_VALUE)) {
            bk_printf("ERROR\r\n");
        }
    }
    if(os_strcmp(argv[1],"indicate")==0) {
        uint16 prf_id;
        uint32_t len;
        uint8 write_buffer[128]= {0};
        len=os_strtoul(argv[3], NULL, 10);
        if(argc!=4) {
            bk_printf("indicate arg %d error\r\n",argc);
            return;
        } else {
            if(len>128 || len<4) {
                bk_printf("The length of the indicate should be between 4 and 128 \r\n");
                if(len<4) {
                    bk_printf("Output 4 bytes\r\n");
                    len=4;
                } else {
                    bk_printf("Output the first 128 bytes\r\n");
                    len=128;
                }
            }
            for(int i=0; i<4; i++) {
                write_buffer[i]=rand()%257;
            }
        }
        prf_id=atoi(argv[2]);
        if(ERR_SUCCESS!=bk_ble_send_ind_value(len, write_buffer,prf_id,TEST_IDX_FF02_VAL_VALUE)) {
            bk_printf("ERROR\r\n");
        }
    }

    if (os_strcmp(argv[1], "dut") == 0) {
        char *const txevm_exit[3] = {"txevm", "-e", "0"};
        do_evm(NULL, 0, 3, txevm_exit);
        ble_dut_start();
    }

    if (os_strcmp(argv[1], "active") == 0) {
        ble_set_notice_cb(ble_notice_cb);
        ble_entry();
        bk_ble_init();
    }

    #if (BLE_BATT_SERVER) && (CFG_BLE_VERSION == BLE_VERSION_5_2)
    if(os_strcmp(argv[1], "bass_init") == 0) {
        ble_set_notice_cb(profile_notice_cb);
        bk_bass_init();
    }
    if(os_strcmp(argv[1], "bass_ntf") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        bk_bass_ntf_send(os_strtoul(argv[2], NULL, 10));
    }
    if(os_strcmp(argv[1], "bass_enable") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        bk_bass_enable(os_strtoul(argv[2], NULL, 10));
    }
    #endif

    #if (BLE_HID_DEVICE) && (CFG_BLE_VERSION == BLE_VERSION_5_2)
    if (os_strcmp(argv[1], "hogpd_init") == 0) {
        ble_set_notice_cb(profile_notice_cb);
        bk_hogpd_init();
    }
    if (os_strcmp(argv[1], "hogpd_enable") == 0) {
        bk_hogpd_enable();
    }
    #endif

    #if (BLE_FINDME_TARGET) && (CFG_BLE_VERSION == BLE_VERSION_5_2)
    if (os_strcmp(argv[1], "findt_init") == 0) {
        ble_set_notice_cb(profile_notice_cb);
        bk_findt_init();
    }
    #endif

    #if (BLE_DIS_SERVER) && (CFG_BLE_VERSION == BLE_VERSION_5_2)
    if (os_strcmp(argv[1], "diss_init") == 0) {
        ble_set_notice_cb(profile_notice_cb);
        bk_diss_init();
    }
    if (os_strcmp(argv[1], "diss_set") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        if(os_strtoul(argv[2], NULL, 10) == 0) {
            uint8_t set_data[5];
            set_data[0]=0x62;//b
            set_data[1]=0x65;//e
            set_data[2]=0x6b;//k
            set_data[3]=0x65;//e
            set_data[4]=0x6e;//n
            bk_diss_set(os_strtoul(argv[2], NULL, 10), 5, set_data);
        } else if(os_strtoul(argv[2], NULL, 10) == 1) {
            uint8_t set_data[6];
            set_data[0]=0x62;//b
            set_data[1]=0x6b;//k
            set_data[2]=0x37;//7
            set_data[3]=0x32;//2
            set_data[4]=0x33;//3
            set_data[5]=0x38;//8
            bk_diss_set(os_strtoul(argv[2], NULL, 10), 6, set_data);
        } else if(os_strtoul(argv[2], NULL, 10) == 6) {
            uint8_t set_data[8];
            set_data[0]=0xdd;//mac
            set_data[1]=0xa3;
            set_data[2]=0xca;
            set_data[3]=0x05;//beken
            set_data[4]=0xf0;
            set_data[5]=0x8c;
            set_data[6]=0x47;
            set_data[7]=0xc7;
            bk_diss_set(os_strtoul(argv[2], NULL, 10),8, set_data);
        } else if(os_strtoul(argv[2], NULL, 10) == 7) {
            uint8_t set_data[6];
            set_data[0]=0x01;
            set_data[1]=0x02;
            set_data[2]=0x03;
            set_data[3]=0x04;
            set_data[4]=0x05;
            set_data[5]=0x06;
            bk_diss_set(os_strtoul(argv[2], NULL, 10), 6, set_data);
        } else if(os_strtoul(argv[2], NULL, 10) == 8) {
            uint8_t set_data[7];
            set_data[0]=0x01;//Blutooth SIG company
            set_data[1]=0xf0;//beken
            set_data[2]=0x05;
            set_data[3]=0x01;//product id
            set_data[4]=0x00;
            set_data[5]=0x02;//product version
            set_data[6]=0x00;
            bk_diss_set(os_strtoul(argv[2], NULL, 10), 7, set_data);
        } else {
            uint8_t set_data[3];
            set_data[0]=0x31;
            set_data[1]=0x32;
            set_data[2]=0x33;
            bk_diss_set(os_strtoul(argv[2], NULL, 10), 3, set_data);
        }
    }
    #endif

    if (os_strcmp(argv[1], "create_adv") == 0) {
        actv_idx = app_ble_get_idle_actv_idx_handle(ADV_ACTV);
        bk_ble_create_advertising(actv_idx, 7, 160, 160, ble_cmd_cb);
    }
    if (os_strcmp(argv[1], "create_ext_adv") == 0) {
        if (argc < 4) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        actv_idx = app_ble_get_idle_actv_idx_handle(ADV_ACTV);
        bk_ble_create_extended_advertising(actv_idx, 7, 160, 160, /*scannable*/os_strtoul(argv[2], NULL, 10), /*connectable*/os_strtoul(argv[3], NULL, 10), ble_cmd_cb);
    }
    #if (CFG_SOC_NAME == SOC_BK7231N)
    if (os_strcmp(argv[1], "set_adv_data") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        adv_data[0] = 0x02;
        adv_data[1] = 0x01;
        adv_data[2] = 0x06;
        adv_data[3] = 0x0B;
        adv_data[4] = 0x09;
        memcpy(&adv_data[5], "7231N_BLE", 10);
        bk_ble_set_adv_data(os_strtoul(argv[2], NULL, 10), adv_data, 0xF, ble_cmd_cb);
    }
    if (os_strcmp(argv[1], "set_rsp_data") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        adv_data[0] = 0x07;
        adv_data[1] = 0x08;
        memcpy(&adv_data[2], "7231N", 6);
        bk_ble_set_scan_rsp_data(os_strtoul(argv[2], NULL, 10), adv_data, 0x8, ble_cmd_cb);
    }
    if (os_strcmp(argv[1], "set_ext_adv_data") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        uint16_t data_len;
        #if (CFG_BLE_AUX_CHAIN)
        data_len = 140;
        #else
        data_len = 22;
        #endif
        //data_len = os_strtoul(argv[3], NULL, 10);
        uint8_t ext_adv_data[data_len];
        uint16_t i;

        ext_adv_data[0] = 0x02;
        ext_adv_data[1] = 0x01;
        ext_adv_data[2] = 0x06;
        ext_adv_data[3] = 0x0B;
        ext_adv_data[4] = 0x09;
        memcpy(&ext_adv_data[5], "7231N_EXT", 10);
        ext_adv_data[15] = data_len - 16;
        ext_adv_data[16] = 0xFF;
        ext_adv_data[17] = 0xF0;
        ext_adv_data[18] = 0x05;
        for (i = 0; i < data_len - 19; i++) {
            ext_adv_data[19 + i] = i;
        }
        bk_ble_set_ext_adv_data(os_strtoul(argv[2], NULL, 10), ext_adv_data, data_len, ble_cmd_cb);
    }
    if (os_strcmp(argv[1], "set_ext_rsp_data") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        uint16_t data_len;
        #if (CFG_BLE_AUX_CHAIN)
        data_len = 260;
        #else
        data_len = 22;
        #endif
        //data_len = os_strtoul(argv[3], NULL, 10);
        uint8_t ext_adv_data[data_len];
        uint16_t i;

        ext_adv_data[0] = 0x0B;
        ext_adv_data[1] = 0x09;
        memcpy(&ext_adv_data[2], "7231N_EXT", 10);
        ext_adv_data[12] = data_len - 13;
        ext_adv_data[13] = 0xFF;
        ext_adv_data[14] = 0xF0;
        ext_adv_data[15] = 0x05;
        for (i = 0; i < data_len - 16; i++) {
            ext_adv_data[16 + i] = i;
        }
        bk_ble_set_ext_scan_rsp_data(os_strtoul(argv[2], NULL, 10), ext_adv_data, data_len, ble_cmd_cb);
    }
    #elif (CFG_BLE_VERSION == BLE_VERSION_5_2)
    /*note:AD type flags already added to adv data,not be set by application*/
    if (os_strcmp(argv[1], "set_adv_data") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        adv_data[0] = 0x0A;
        adv_data[1] = 0x09;
        #if (CFG_SOC_NAME == SOC_BK7238)
        memcpy(&adv_data[2], "7238_BLE", 9);
        #else
        memcpy(&adv_data[2], "7252n_BLE", 9);
        #endif
        adv_data[11] = 0x03;
        adv_data[12] = 0x19;
        adv_data[13] = app_ble_env.dev_appearance & 0xFF;
        adv_data[14] = (app_ble_env.dev_appearance >> 8) & 0xFF;
        bk_ble_set_adv_data(os_strtoul(argv[2], NULL, 10), adv_data, 0xF, ble_cmd_cb);
    }
    if (os_strcmp(argv[1], "set_rsp_data") == 0) {
        uint8_t adv_data_len;
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        adv_data[0] = 0x06;
        adv_data[1] = 0x08;
        #if (CFG_SOC_NAME == SOC_BK7238)
        memcpy(&adv_data[2], "7238", 5);
        #else
        memcpy(&adv_data[2], "7252n", 5);
        #endif
        #if (BLE_HID_DEVICE)
        adv_data[7] = 0x03;
        adv_data[8] = 0x03;
        adv_data[9] = 0x12;
        adv_data[10] = 0x18;
        adv_data_len = 0xB;
        #else
        adv_data_len = 0x7;
        #endif
        bk_ble_set_scan_rsp_data(os_strtoul(argv[2], NULL, 10), adv_data, adv_data_len, ble_cmd_cb);
    }
    if (os_strcmp(argv[1], "set_ext_adv_data") == 0) {
        uint16_t data_len;
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        #if (CFG_BLE_AUX_CHAIN)
        data_len = 140;		//189frag
        #else
        data_len = 18;
        #endif

        //data_len = os_strtoul(argv[3], NULL, 10);
        uint8_t ext_adv_data[data_len];
        uint16_t i;

        ext_adv_data[0] = 0x0B;
        ext_adv_data[1] = 0x09;
        #if (CFG_SOC_NAME == SOC_BK7238)
        memcpy(&ext_adv_data[2], "BK7238EXT", 10);
        #else
        memcpy(&ext_adv_data[2], "BK7252nEXT", 10);
        #endif
        ext_adv_data[12] = data_len - 13;
        ext_adv_data[13] = 0xFF;
        ext_adv_data[14] = 0xF0;
        ext_adv_data[15] = 0x05;
        for (i = 0; i < data_len - 16; i++) {
            ext_adv_data[16 + i] = i;
        }
        bk_ble_set_ext_adv_data(os_strtoul(argv[2], NULL, 10), ext_adv_data, data_len, ble_cmd_cb);
    }
    if (os_strcmp(argv[1], "set_ext_rsp_data") == 0) {
        uint16_t data_len;
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        #if (CFG_BLE_AUX_CHAIN)
        data_len = 260;
        #else
        data_len = 25;
        #endif

        //data_len = os_strtoul(argv[3], NULL, 10);
        uint8_t ext_adv_data[data_len];
        uint16_t i;

        ext_adv_data[0] = 0x11;
        ext_adv_data[1] = 0x09;
        #if (CFG_SOC_NAME == SOC_BK7238)
        memcpy(&ext_adv_data[2], "BK7238-SCAN-EXT", 16);
        #else
        memcpy(&ext_adv_data[2], "BK7252n-SCAN-EXT", 16);
        #endif
        ext_adv_data[18] = data_len - 19;
        ext_adv_data[19] = 0xFF;
        ext_adv_data[20] = 0xF0;
        ext_adv_data[21] = 0x05;
        for (i = 0; i < data_len - 22; i++) {
            ext_adv_data[22 + i] = i;
        }
        bk_ble_set_ext_scan_rsp_data(os_strtoul(argv[2], NULL, 10), ext_adv_data, data_len, ble_cmd_cb);
    }

    #if (CFG_BLE_PER_ADV)
    if (os_strcmp(argv[1], "create_per_adv") == 0) {
        if (argc < 8) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        struct per_adv_param per_adv_param;

        per_adv_param.adv_intv_min = os_strtoul(argv[2], NULL, 10);
        per_adv_param.adv_intv_max = os_strtoul(argv[3], NULL, 10);
        if ((per_adv_param.adv_intv_min > ADV_INTERVAL_MAX || per_adv_param.adv_intv_min < ADV_INTERVAL_MIN)
                || (per_adv_param.adv_intv_max > ADV_INTERVAL_MAX || per_adv_param.adv_intv_max < ADV_INTERVAL_MIN)
                || (per_adv_param.adv_intv_min > per_adv_param.adv_intv_max)) {
            bk_printf("input param interval is error\n");
            return;
        }

        per_adv_param.chnl_map = os_strtoul(argv[4], NULL, 10);
        if (per_adv_param.chnl_map > 7) {
            bk_printf("input param chnl_map is error\n");
            return;
        }

        per_adv_param.adv_prop = (0 << ADV_PROP_CONNECTABLE_POS) | (0 << ADV_PROP_SCANNABLE_POS);;
        per_adv_param.prim_phy = os_strtoul(argv[5], NULL, 10);
        if(!(per_adv_param.prim_phy == 1 || per_adv_param.prim_phy == 3)) {
            bk_printf("input param prim_phy is error\n");
            return;
        }

        per_adv_param.second_phy = os_strtoul(argv[6], NULL, 10);
        if(per_adv_param.second_phy < 1 || per_adv_param.second_phy > 3) {
            bk_printf("input param second_phy is error\n");
            return;
        }

        per_adv_param.own_addr_type = os_strtoul(argv[7], NULL, 10);
        switch (per_adv_param.own_addr_type) {
        case 0:
        case 1:
            per_adv_param.own_addr_type = GAPM_STATIC_ADDR;
            break;
        case 2:
            per_adv_param.own_addr_type = GAPM_GEN_RSLV_ADDR;
            break;
        case 3:
            per_adv_param.own_addr_type = GAPM_GEN_NON_RSLV_ADDR;
            break;
        default:
            bk_printf("input param own_addr_type is error\n");
            break;
        }

        actv_idx = app_ble_get_idle_actv_idx_handle(ADV_ACTV);

        bk_ble_create_periodic_advertising(actv_idx, &per_adv_param, ble_cmd_cb);
    }

    if (os_strcmp(argv[1], "set_per_adv_data") == 0) {
        if (argc < 4) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        uint8_t adv_data[255];
        uint8_t adv_len = 0;

        sscanf(argv[3],"\"%[^\"]\"",argv[3]);

        adv_len=strlen(argv[3])/2;
        memset(adv_data, 0, sizeof(adv_data));
        hexstr2bin(argv[3], adv_data, adv_len);

        bk_ble_set_periodic_adv_data(os_strtoul(argv[2], NULL, 10), adv_data, adv_len, ble_cmd_cb);
    }

    if (os_strcmp(argv[1], "start_per_adv") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        bk_ble_start_periodic_advertising(os_strtoul(argv[2], NULL, 10), 0, ble_cmd_cb);
    }

    if (os_strcmp(argv[1], "stop_per_adv") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        bk_ble_stop_periodic_advertising(os_strtoul(argv[2], NULL, 10), ble_cmd_cb);
    }
    #endif

    #if (CFG_BLE_PER_SYNC)
    if (os_strcmp(argv[1], "create_per_sync") == 0) {
        actv_idx = app_ble_get_idle_actv_idx_handle(PER_SYNC_ACTV);
        bk_ble_create_periodic_sync(actv_idx, ble_cmd_cb);
    }

    if (os_strcmp(argv[1], "start_per_sync") == 0) {
        if (argc < 11) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        ble_periodic_sync_param_t periodic_param;
        os_memset(&periodic_param, 0, sizeof(periodic_param));

        periodic_param.report_en_bf = os_strtoul(argv[3], NULL, 10);
        periodic_param.adv_sid = os_strtoul(argv[4], NULL, 10);
        hexstr2bin(argv[5], periodic_param.adv_addr.addr, GAP_BD_ADDR_LEN);
        periodic_param.adv_addr_type = os_strtoul(argv[6], NULL, 10);
        periodic_param.skip = os_strtoul(argv[7], NULL, 10);
        periodic_param.sync_to = os_strtoul(argv[8], NULL, 10);
        periodic_param.cte_type = os_strtoul(argv[9], NULL, 10);
        periodic_param.per_sync_type = os_strtoul(argv[10], NULL, 10);

        switch (periodic_param.per_sync_type) {
        case 0:
            periodic_param.per_sync_type = GAPM_PER_SYNC_TYPE_GENERAL;
            break;
        case 1:
            periodic_param.per_sync_type = GAPM_PER_SYNC_TYPE_SELECTIVE;
            break;
        case 2:
            periodic_param.per_sync_type = GAPM_PER_SYNC_TYPE_PAST;
            break;
        default:
            bk_printf("input param per_sync_type is error\n");
            return;
            break;
        }

        bk_ble_start_periodic_sync(os_strtoul(argv[2], NULL, 10), &periodic_param, ble_cmd_cb);
    }

    if (os_strcmp(argv[1], "stop_per_sync") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        bk_ble_stop_periodic_sync(os_strtoul(argv[2], NULL, 10), ble_cmd_cb);
    }
    #endif

    #if (CFG_BLE_PER_ADV) | (CFG_BLE_PER_SYNC)
    if (os_strcmp(argv[1], "per_adv_sync_transf") == 0) {
        if (argc < 4) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        bk_ble_periodic_adv_sync_transf(os_strtoul(argv[2], NULL, 10), os_strtoul(argv[3], NULL, 10));
    }
    #endif

    if (os_strcmp(argv[1], "get_phy") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        ble_read_phy_t phy;
        phy.rx_phy = 0;
        phy.tx_phy = 0;

        if (!bk_ble_gap_read_phy(os_strtoul(argv[2], NULL, 10), &phy)) {
            bk_printf("tx_phy = 0x%x, rx_phy = 0x%x\r\n", phy.tx_phy, phy.rx_phy);
        }
    }

    if (os_strcmp(argv[1], "set_phy") == 0) {
        if (argc < 6) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        ble_set_phy_t set_phy;

        set_phy.tx_phy = os_strtoul(argv[3], NULL, 10);
        set_phy.rx_phy = os_strtoul(argv[4], NULL, 10);
        set_phy.phy_opt = os_strtoul(argv[5], NULL, 10);

        switch (set_phy.phy_opt) {
        case 0:
            set_phy.phy_opt = CODED_NO_PREFERRED;
            break;
        case 1:
            set_phy.phy_opt = CODED_500K_RATE;
            break;
        case 2:
            set_phy.phy_opt = CODED_125K_RATE;
            break;
        default:
            bk_printf("input phy_opt param error\r\n");
            break;
        }

        bk_ble_gap_set_phy(os_strtoul(argv[2], NULL, 10), &set_phy);
    }

    if (os_strcmp(argv[1], "gatts_app_unreg") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        bk_ble_gatts_app_unregister(os_strtoul(argv[2], NULL, 16));
    }

    if (os_strcmp(argv[1], "del_srv") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        bk_ble_gatts_remove_service(os_strtoul(argv[2], NULL, 16));
    }

    if (os_strcmp(argv[1], "set_attr_val") == 0) {
        if (argc < 5) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        uint8_t value[128] = {0};
        uint16_t length = os_strtoul(argv[3], NULL, 10);
        hexstr2bin(argv[4], value, length);

        bk_ble_gatts_set_attr_value(os_strtoul(argv[2], NULL, 16), length, value);
    }

    if (os_strcmp(argv[1], "get_attr_val") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        uint16_t length = 0;
        uint8_t *value;

        if(bk_ble_gatts_get_attr_value(os_strtoul(argv[2], NULL, 16), &length, &value) == 0) {
            bk_printf("att_value:");
            for (uint16_t len = 0; len < length; len++) {
                bk_printf("0x%x ", value[len]);
            }
            bk_printf("\r\n");
        }
    }

    if (os_strcmp(argv[1], "srv_change_ind") == 0) {
        if (argc < 4) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        uint16_t start_handle = os_strtoul(argv[2], NULL, 16);
        uint16_t end_handle = os_strtoul(argv[3], NULL, 16);

        if (start_handle <= end_handle) {
            bk_ble_gatts_send_service_change_indication(start_handle, end_handle);
        }
    }

    if (os_strcmp(argv[1], "rssi") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        app_ble_get_con_rssi(os_strtoul(argv[2], NULL, 10));
    }

    if (os_strcmp(argv[1], "set_icon") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        uint16_t appearance;
        appearance = os_strtoul(argv[2], NULL, 16);
        bk_printf("appearance=0x%x\r\n", appearance);
        bk_ble_gap_config_local_icon(appearance);
    }
    if (os_strcmp(argv[1], "set_host_chnlmap") == 0) {
        if (argc < 3 || (strlen(argv[2]) != BLE_CHANNELS_LEN * 2)) {
            bk_printf("ERROR\r\n");
            return;
        }

        bk_ble_channels_t chnl_map;
        hexstr2bin(argv[2], chnl_map.channels, BLE_CHANNELS_LEN);
        bk_ble_gap_set_channels(&chnl_map);
    }
    if (os_strcmp(argv[1], "get_wl_size") == 0) {
        uint8_t num;
        bk_ble_gap_get_whitelist_size(&num);
        bk_printf("BLE WhiteList Size:%d\r\n", num);
    }
    if (os_strcmp(argv[1], "clear_wl") == 0) {
        bk_ble_gap_clear_whitelist();
    }
    if (os_strcmp(argv[1], "update_wl") == 0) {
        //argv[2] 0:remove, 1:add;
        //argv[4] 0:public, 1:random;
        struct bd_addr addr;
        if (argc < 5) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        hexstr2bin(argv[3], addr.addr, BD_ADDR_LEN);
        bk_ble_gap_update_whitelist(atoi(argv[2]), &addr, atoi(argv[4]));
    }
    if (os_strcmp(argv[1], "update_pal")== 0) {
        //argv[2] 0:remove, 1:add;
        //argv[4] 0:public, 1:random;
        struct bd_addr bdaddr;

        if (argc < 6) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        hexstr2bin(argv[3], bdaddr.addr, GAP_BD_ADDR_LEN);
        bk_ble_gap_update_per_adv_list(atoi(argv[2]), &bdaddr, atoi(argv[4]), atoi(argv[5]));
    }
    if (os_strcmp(argv[1], "clear_pal")== 0) {
        bk_ble_gap_clear_per_adv_list();
    }
    #endif // (CFG_BLE_VERSION == BLE_VERSION_5_2)

    if (os_strcmp(argv[1], "start_adv") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        bk_ble_start_advertising(os_strtoul(argv[2], NULL, 10), 0, ble_cmd_cb);
    }
    if (os_strcmp(argv[1], "stop_adv") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        bk_ble_stop_advertising(os_strtoul(argv[2], NULL, 10), ble_cmd_cb);
    }
    if (os_strcmp(argv[1], "delete_adv") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        bk_ble_delete_advertising(os_strtoul(argv[2], NULL, 10), ble_cmd_cb);
    }
    if (os_strcmp(argv[1], "create_scan") == 0) {
        actv_idx = app_ble_get_idle_actv_idx_handle(SCAN_ACTV);
        bk_ble_create_scaning(actv_idx, ble_cmd_cb);
    }
    if (os_strcmp(argv[1], "start_scan") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        bk_ble_start_scaning(os_strtoul(argv[2], NULL, 10), 100, 30, ble_cmd_cb);
    }
    if (os_strcmp(argv[1], "stop_scan") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        bk_ble_stop_scaning(os_strtoul(argv[2], NULL, 10), ble_cmd_cb);
    }
    if (os_strcmp(argv[1], "delete_scan") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        bk_ble_delete_scaning(os_strtoul(argv[2], NULL, 10), ble_cmd_cb);
    }
    if (os_strcmp(argv[1], "update_conn") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        bk_ble_update_param(os_strtoul(argv[2], NULL, 10), 50, 50, 0, 800);
    }
    if (os_strcmp(argv[1], "dis_conn") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        bk_ble_disconnect(os_strtoul(argv[2], NULL, 10));
    }
    if (os_strcmp(argv[1], "mtu_change") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        bk_ble_gatt_mtu_change(os_strtoul(argv[2], NULL, 10));
    }
    if (os_strcmp(argv[1], "init_adv") == 0) {
        #if (CFG_SOC_NAME == SOC_BK7231N)
        struct adv_param adv_info;
        adv_info.channel_map = 7;
        adv_info.duration = 0;
        adv_info.prop = (1 << ADV_PROP_CONNECTABLE_POS) | (1 << ADV_PROP_SCANNABLE_POS);
        adv_info.interval_min = 160;
        adv_info.interval_max = 160;
        adv_info.advData[0] = 0x02;
        adv_info.advData[1] = 0x01;
        adv_info.advData[2] = 0x06;
        adv_info.advData[3] = 0x0B;
        adv_info.advData[4] = 0x09;
        memcpy(&adv_info.advData[5], "7231N_BLE", 10);
        adv_info.advDataLen = 0xF;
        adv_info.respData[0] = 0x07;
        adv_info.respData[1] = 0x08;
        memcpy(&adv_info.respData[2], "7231N", 6);
        adv_info.respDataLen = 0x8;
        #elif (CFG_BLE_VERSION == BLE_VERSION_5_2)
        struct adv_param adv_info;
        adv_info.channel_map = 7;
        adv_info.duration = 0;
        adv_info.prop = (1 << ADV_PROP_CONNECTABLE_POS) | (1 << ADV_PROP_SCANNABLE_POS);
        adv_info.interval_min = 160;
        adv_info.interval_max = 160;
        adv_info.advData[0] = 0x09;
        adv_info.advData[1] = 0x09;
        #if (CFG_SOC_NAME == SOC_BK7238)
        memcpy(&adv_info.advData[2], "7238_BLE", 8);
        #else
        memcpy(&adv_info.advData[2], "7252nBLE", 8);
        #endif
        adv_info.advDataLen = 10;

        #if (CFG_SOC_NAME == SOC_BK7238)
        adv_info.respData[0] = 0x05;
        adv_info.respData[1] = 0x08;
        memcpy(&adv_info.respData[2], "7238", 4);
        adv_info.respDataLen = 6;
        #else
        adv_info.respData[0] = 0x06;
        adv_info.respData[1] = 0x08;
        memcpy(&adv_info.respData[2], "7252n", 5);
        adv_info.respDataLen = 7;
        #endif
        #endif
        actv_idx = app_ble_get_idle_actv_idx_handle(ADV_ACTV);
        bk_ble_adv_start(actv_idx, &adv_info, ble_cmd_cb);
    }
    if (os_strcmp(argv[1], "deinit_adv") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        bk_ble_adv_stop(os_strtoul(argv[2], NULL, 10), ble_cmd_cb);
    }
    if (os_strcmp(argv[1], "init_scan") == 0) {
        struct scan_param scan_info;
        scan_info.channel_map = 7;
        scan_info.interval = 100;
        scan_info.window = 30;
        actv_idx = app_ble_get_idle_actv_idx_handle(SCAN_ACTV);
        bk_ble_scan_start(actv_idx, &scan_info, ble_cmd_cb);
    }
    if (os_strcmp(argv[1], "deinit_scan") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        bk_ble_scan_stop(os_strtoul(argv[2], NULL, 10), ble_cmd_cb);
    }
    #if BLE_APP_SEC
    if (os_strcmp(argv[1], "get_bond_status") == 0) {
        bk_printf("bond status: 0x%x\r\n", app_sec_get_bond_status());
    }
    if (os_strcmp(argv[1], "smp_init") == 0) {
        struct app_pairing_cfg par;
        par.iocap = GAP_IO_CAP_DISPLAY_ONLY;

        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        if (os_strtoul(argv[2], NULL, 10) == 0) {
            bk_printf("BLE use Legacy Pairing\r\n");
            par.sec_req   = GAP_SEC1_AUTH_PAIR_ENC;
            par.auth      = GAP_AUTH_REQ_MITM_BOND;
            par.ikey_dist = GAP_KDIST_ENCKEY | GAP_KDIST_LINKKEY | GAP_KDIST_IDKEY;
            par.rkey_dist = GAP_KDIST_ENCKEY | GAP_KDIST_LINKKEY;
            #if BLE_APP_SEC_CON
        } else if (os_strtoul(argv[2], NULL, 10) == 1) {
            // 0: PK, 1: NC, 2: JW
            uint8_t pk_meth = os_strtoul(argv[3], NULL, 10);

            bk_printf("BLE use Secure Connection Pairiing\r\n");
            par.sec_req   = GAP_SEC1_SEC_CON_PAIR_ENC;
            par.auth      = GAP_AUTH_REQ_SEC_CON_BOND;
            par.ikey_dist = GAP_KDIST_IDKEY;
            par.rkey_dist = GAP_KDIST_NONE;

            if (pk_meth == 1) {
                par.iocap = GAP_IO_CAP_DISPLAY_YES_NO;
            } else if (pk_meth == 2) {
                par.iocap   = GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
                par.sec_req = GAP_SEC1_NOAUTH_PAIR_ENC;
            }
            #endif
        }
        app_sec_config(&par, security_notice_cb);
    }
    if (os_strcmp(argv[1], "sec_req") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        app_sec_send_security_req(os_strtoul(argv[2], NULL, 10));
    }
    if (os_strcmp(argv[1], "remove_bond")==0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }
        app_sec_remove_bond(os_strtoul(argv[2], NULL, 10));
    }
    #if (CFG_BLE_VERSION == BLE_VERSION_5_2)
    if (os_strcmp(argv[1], "get_bond_dev_num")==0) {
        uint8_t num;
        bk_ble_get_bond_device_num(&num);
        if (num < 0xFF) {
            bk_printf("bond num: %d\r\n", num);
        } else {
            bk_printf("ERROR\r\n");
        }
    }
    if (os_strcmp(argv[1], "get_bond_dev_list")==0) {
        uint8_t exp_num, dev_num;

        bk_ble_get_bond_device_num(&exp_num);
        if (exp_num == 0xFF) {
            return;
        }

        dev_num = exp_num;
        device_addr_t dev_list[exp_num];
        bk_ble_get_bonded_device_list(&dev_num, dev_list);

        if (dev_num != exp_num) {
            bk_printf("[WARNING] exp_num = %d, act_num = %d\r\n", exp_num, dev_num);
        }
        for (int i = 0; i < dev_num; i++) {
            bk_printf("[%d] addr_type:%d, addr", i, dev_list[i].addr_type);
            for (int j = 0; j <6; j++) {
                bk_printf(":%x", dev_list[i].addr[j]);
            }
            bk_printf("\r\n");
        }
    }
    #endif // (CFG_BLE_VERSION == BLE_VERSION_5_2)
    #endif // if BLE_APP_SEC

    #if ((CFG_BLE_VERSION == BLE_VERSION_5_2) && BLE_GATT_CLI)
    if (os_strcmp(argv[1], "read_by_type") == 0) {
        if (argc < 6) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        uint16_t start_handle = os_strtoul(argv[3], NULL, 16);
        uint16_t end_handle = os_strtoul(argv[4], NULL, 16);
        uint8_t uuid_type = os_strtoul(argv[5], NULL, 10);
        uint8_t uuid_length = 0;
        uint8_t uuid[GATT_UUID_128_LEN];

        if (uuid_type == GATT_UUID_16) {
            uuid_length = GATT_UUID_16_LEN;
        } else if (uuid_type == GATT_UUID_32) {
            uuid_length = GATT_UUID_32_LEN;
        } else if (uuid_type == GATT_UUID_128) {
            uuid_length = GATT_UUID_128_LEN;
        } else {
            bk_printf("uuid_type error!\r\n");
            return;
        }

        if (start_handle <= end_handle) {
            hexstr2bin(argv[6], uuid, uuid_length);
            bk_ble_gattc_read_by_type(os_strtoul(argv[2], NULL, 10), start_handle, end_handle, uuid_type, uuid);
        } else {
            bk_printf("param error!\r\n");
        }
    }

    if (os_strcmp(argv[1], "read_mult") == 0) {
        if (argc < 3) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        app_gattc_multi_t read_multi;
        gatt_att_t atts[2];
        read_multi.nb_att = 2;
        atts[0].length = 3;
        atts[0].hdl = 0x1b;
        atts[1].length = 3;
        atts[1].hdl = 0x1e;
        read_multi.p_atts = atts;

        bk_ble_gattc_read_multiple(os_strtoul(argv[2], NULL, 10), &read_multi);
    }

    if (os_strcmp(argv[1], "reg_notify") == 0) {
        if (argc < 4) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        bk_ble_gattc_register_for_notify(os_strtoul(argv[2], NULL, 10), os_strtoul(argv[3], NULL, 16));
    }

    if (os_strcmp(argv[1], "reg_indicate") == 0) {
        if (argc < 4) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        bk_ble_gattc_register_for_indicate(os_strtoul(argv[2], NULL, 10), os_strtoul(argv[3], NULL, 16));
    }

    if (os_strcmp(argv[1], "unreg") == 0) {
        if (argc < 4) {
            bk_printf("\nThe number of param is wrong!\n");
            return;
        }

        bk_ble_gattc_unregister_for_notify_or_indicate(os_strtoul(argv[2], NULL, 10), os_strtoul(argv[3], NULL, 16));
    }
    #endif

    #if CFG_BLE_INIT_NUM
    #if (CFG_BLE_VERSION == BLE_VERSION_5_2)
    if (os_strcmp(argv[1], "set_ext_conn_par") == 0) {
        uint8_t phy_mask;
        struct appm_create_conn_param init_par;

        if (argc < 9) {
            bk_printf("ERROR\r\n");
            return;
        }

        phy_mask = atoi(argv[2]);
        init_par.conn_intv_max = atoi(argv[3]);
        init_par.conn_intv_min = atoi(argv[3]);
        init_par.conn_latency = atoi(argv[4]);
        init_par.supervision_to = atoi(argv[5]);
        init_par.scan_interval = atoi(argv[6]);
        init_par.scan_window = atoi(argv[7]);
        init_par.ce_len_min = atoi(argv[8]);
        init_par.ce_len_max = atoi(argv[8]);
        bk_ble_gap_prefer_ext_connect_params_set(phy_mask, &init_par, &init_par, &init_par);
    }
    #endif
    uint8_t conn_idx;
    if (os_strcmp(argv[1], "con_create") == 0)
    {
        ble_set_notice_cb(ble_notice_cb);
        conn_idx = app_ble_get_idle_conn_idx_handle(INIT_ACTV);
        bk_printf("------------->conn_idx:%d\r\n",conn_idx);

        #if BLE_SDP_CLIENT && (CFG_SOC_NAME == SOC_BK7231N)
        register_app_sdp_service_tab(sizeof(service_tab)/sizeof(app_sdp_service_uuid),(app_sdp_service_uuid *)service_tab);
        app_sdp_service_filtration(0);
        register_app_sdp_characteristic_callback(ble_app_sdp_characteristic_cb);
        register_app_sdp_charac_callback(app_sdp_charac_cb);
        bk_ble_create_init(conn_idx, 0, 0, 0,ble_cmd_cb);
        #elif(CFG_BLE_VERSION == BLE_VERSION_5_2)
        sdp_set_notice_cb(sdp_event_cb);
        bk_ble_create_init(conn_idx, ble_cmd_cb);
        #endif
    }
    else if ((os_strcmp(argv[1], "con_start") == 0) && (argc >= 5))
    {
        struct bd_addr bdaddr;
        unsigned char addr_type = ADDR_PUBLIC;
        int addr_type_str = atoi(argv[3]);
        int actv_idx_str = atoi(argv[4]);
        bk_printf("idx:%d,addr_type:%d\r\n",actv_idx_str,addr_type_str);
        if((addr_type_str > ADDR_RPA_OR_RAND)||(actv_idx_str >= 0xFF)) {
            return;
        }
        conn_idx = actv_idx_str;
        hexstr2bin(argv[2], bdaddr.addr, GAP_BD_ADDR_LEN);
        addr_type = addr_type_str;
        bk_ble_init_set_connect_dev_addr(conn_idx,&bdaddr,addr_type);
        #if (CFG_BLE_VERSION == BLE_VERSION_5_2)
        bk_ble_init_start_conn(conn_idx,10000,ble_cmd_cb);
        #else
        bk_ble_init_start_conn(conn_idx,ble_cmd_cb);
        #endif
    }
    else if ((os_strcmp(argv[1], "con_stop") == 0) && (argc >= 3))
    {
        int actv_idx_str = atoi(argv[2]);
        bk_printf("idx:%d\r\n",actv_idx_str);
        if(actv_idx_str >= 0xFF) {
            return;
        }
        conn_idx = actv_idx_str;
        bk_ble_init_stop_conn(conn_idx,ble_cmd_cb);
    }
    else if ((os_strcmp(argv[1], "con_dis") == 0) && (argc >= 3))
    {
        int actv_idx_str = atoi(argv[2]);
        bk_printf("idx:%d\r\n",actv_idx_str);
        if(actv_idx_str >= 0xFF) {
            return;
        }
        conn_idx = actv_idx_str;
        app_ble_master_appm_disconnect(conn_idx);
    } else if (os_strcmp(argv[1], "del_init") == 0)
    {
        bk_ble_delete_init(os_strtoul(argv[2], NULL, 10),ble_cmd_cb);
    }
    #if BLE_CENTRAL
    else if (os_strcmp(argv[1], "con_read") == 0)
    {
        if(argc < 4) {
            bk_printf("param error\r\n");
            return;
        }
        int actv_idx_str = atoi(argv[3]);
        bk_printf("idx:%d\r\n",actv_idx_str);
        if(actv_idx_str >= 0xFF) {
            return;
        }
        conn_idx = actv_idx_str;
        int handle = atoi(argv[2]);
        if(handle >=0 && handle <= 0xFFFF) {
            bk_ble_read_service_data_by_handle_req(conn_idx,handle);
        }
        else {
            bk_printf("handle(%x) error\r\n",handle);
        }
    }
    else if (os_strcmp(argv[1], "con_write") == 0)
    {
        //cmd:ble con_write 24 0
        if(argc < 4) {
            bk_printf("param error\r\n");
            return;
        }
        int handle = atoi(argv[2]);
        int actv_idx_str = atoi(argv[3]);
        bk_printf("idx:%d\r\n",actv_idx_str);
        if(actv_idx_str >= 0xFF) {
            return;
        }
        conn_idx = actv_idx_str;
        unsigned char test_buf[4] = {0x01,0x02,0x22,0x32};
        if(handle >=0 && handle <= 0xFFFF) {
            bk_ble_write_service_data_req(conn_idx,handle,4,test_buf);
        } else {
            bk_printf("handle(%x) error\r\n",handle);
        }
    }
    #if (CFG_SOC_NAME == SOC_BK7231N)
    else if (os_strcmp(argv[1], "con_rd_sv_ntf_int_cfg") == 0)
    {
        if(argc < 4) {
            bk_printf("param error\r\n");
            return;
        }
        int actv_idx_str = atoi(argv[3]);
        bk_printf("idx:%d\r\n",actv_idx_str);
        if(actv_idx_str >= 0xFF) {
            return;
        }
        conn_idx = actv_idx_str;
        int handle = atoi(argv[2]);
        if(handle >=0 && handle <= 0xFFFF) {
            appm_read_service_ntf_ind_cfg_by_handle_req(conn_idx,handle);
        } else {
            bk_printf("handle(%x) error\r\n",handle);
        }
    }
    else if (os_strcmp(argv[1], "con_rd_sv_ud_cfg") == 0)
    {
        if(argc < 4) {
            bk_printf("param error\r\n");
            return;
        }
        int actv_idx_str = atoi(argv[3]);
        bk_printf("idx:%d\r\n",actv_idx_str);
        if(actv_idx_str >= 0xFF) {
            return;
        }
        conn_idx = actv_idx_str;
        int handle = atoi(argv[2]);
        if(handle >=0 && handle <= 0xFFFF) {
            appm_read_service_userDesc_by_handle_req(conn_idx,handle);
        } else {
            bk_printf("handle(%x) error\r\n",handle);
        }
    }
    #endif //(CFG_SOC_NAME == SOC_BK7231N)
    else if(os_strcmp(argv[1], "svc_filt") == 0)
    {
        if(argc < 3) {
            bk_printf("param error\r\n");
            return;
        }
        int en = atoi(argv[2]);
        bk_printf("svc_filt en:%d\r\n",en);
        app_sdp_service_filtration(en);
    }
    #endif ///#if BLE_CENTRAL
    #endif ///CFG_BLE_INIT_NUM
}

MSH_CMD_EXPORT(ble, ble command);
#endif //#if (CFG_BLE_VERSION == BLE_VERSION_5_1) || (CFG_BLE_VERSION == BLE_VERSION_5_2)
#endif //((CFG_SUPPORT_BLE) && (CFG_BLE_USE_CLI))


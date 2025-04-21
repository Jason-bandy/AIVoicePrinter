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
#include "samples_config.h"

#ifdef BLE_CONFIG_SAMPLE
#include <finsh.h>
#include "ble_api.h"
#include "ble_pub.h"
#include "param_config.h"
#include "ble_netconfig.h"
#include "common.h"
#include "rwapp_config.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../test/direct_connect.h"
#include "sys_config.h"
#if (CFG_SOC_NAME != SOC_BK7252N)
#include "cJSON.h"
#ifdef XIAOYA_OS
#include "parm_cache.h"
#include "player_manager.h"
#endif

//#ifdef BLE_CONFIG_SAMPLE
#define NETCONFIG_TIMEOUT    (90*1000)

#define str_begin_with(s, prefix)       (strstr(s, prefix) == s)
#define str_end_with(buf, len, ending)  (strstr(buf+len-1, ending) == buf+len-1)
static struct ble_session _ble_session = {0}, *ble_session = &_ble_session;
static rt_uint8_t ble_create_prf_ok = 0;
static rt_uint8_t ble_netconfig_state = 0;
static rt_thread_t tid = RT_NULL;


#define BK_ATT_DECL_PRIMARY_SERVICE_128     {0x00,0x28,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define BK_ATT_DECL_CHARACTERISTIC_128      {0x03,0x28,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define BK_ATT_DESC_CLIENT_CHAR_CFG_128     {0x02,0x29,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define WRITE_REQ_CHARACTERISTIC_128        {0xB0,0xBB,0,0,0x34,0x56,0,0,0,0,0x28,0x37,0,0,0,0}
#define NOTIFY_CHARACTERISTIC_128           {0xB1,0xBB,0,0,0x34,0x56,0,0,0,0,0x28,0x37,0,0,0,0}

static const uint8_t test_svc_uuid[16] = {0xAA,0xAA,0,0,0x34,0x56,0,0,0,0,0x28,0x37,0,0,0,0};

static void station_connect(const char *ssid, const char *passwd)
{
    char argv[64];

    memset(argv, 0, sizeof(argv));
    sprintf(argv, "wifi %s join %s %s", "w0", ssid, passwd);
    msh_exec(argv, strlen(argv));
}

static int result_cb(char *ssid, char *password,char *ble_get_openid, void *user_data, void *userdata_len)
{

    #ifdef XIAOYA_OS
    xiaoya_player_tips(TIP_FIND_AP_INFO,0);
    parm_set_wechat_openid_str((uint8_t *)ble_get_openid);
    sta_cfg_t sta_cfg;
    memcpy(sta_cfg.ssid_str,ssid,strlen(ssid)+1);
    memcpy(sta_cfg.pwd_str,password,strlen(password)+1);
    parm_set_sta_cfg(&sta_cfg);
    #endif

    #ifdef AP_DIERCT_CONNECT_TEST
    extern direct_ap_info_t direct_ap_info;
    memcpy(direct_ap_info.direct_ssid,ssid,strlen(ssid)+1);
    memcpy(direct_ap_info.direct_pwd,password,strlen(password)+1);
    #endif

    rt_kprintf("ssid:%s, password:%s,openid:%s\n", ssid, password,ble_get_openid);
    ble_netconfig_state=BLE_NETCONFIG_RECIVE_INFO;
    station_connect(ssid,password);
    return RT_EOK;
}

const bk_attm_desc_t netconfig_att_db[AAAAS_IDX_NB] =
{
    //  Service Declaration
    [AAAAS_IDX_SVC]                     =   {BK_ATT_DECL_PRIMARY_SERVICE_128,   BK_PERM_SET(RD, ENABLE), 0, 0},

    //  Level Characteristic Declaration
    [AAAAS_IDX_BBB0_VAL_CHAR]           =   {BK_ATT_DECL_CHARACTERISTIC_128,   BK_PERM_SET(RD, ENABLE), 0, 0},
    //  Level Characteristic Value
    [AAAAS_IDX_BBB0_VAL_VALUE]          =   {WRITE_REQ_CHARACTERISTIC_128,   BK_PERM_SET(WRITE_REQ, ENABLE),BK_PERM_SET(RI, ENABLE)|BK_PERM_SET(UUID_LEN, UUID_16),AAAA_CHAR_DATA_LEN},

    [AAAAS_IDX_BBB1_VAL_CHAR]           =   {BK_ATT_DECL_CHARACTERISTIC_128,   BK_PERM_SET(RD, ENABLE), 0, 0},
    //  Level Characteristic Value
    [AAAAS_IDX_BBB1_VAL_VALUE]          =   {NOTIFY_CHARACTERISTIC_128,   BK_PERM_SET(NTF, ENABLE)|BK_PERM_SET(RD, ENABLE),BK_PERM_SET(RI, ENABLE)|BK_PERM_SET(UUID_LEN, UUID_16),AAAA_CHAR_DATA_LEN},
    //  Level Characteristic - Client Characteristic Configuration Descriptor
    [AAAAS_IDX_BBB1_VAL_NTF_CFG]        =   {BK_ATT_DESC_CLIENT_CHAR_CFG_128,  BK_PERM_SET(RD, ENABLE)|BK_PERM_SET(WRITE_REQ, ENABLE),0,0},
};

static ble_err_t ble_create_db(void)
{
    ble_err_t status;
    struct bk_ble_db_cfg ble_db_cfg;

    ble_db_cfg.att_db = netconfig_att_db;
    ble_db_cfg.att_db_nb = AAAAS_IDX_NB;
    ble_db_cfg.prf_task_id = 0;
    ble_db_cfg.start_hdl = 0;
    ble_db_cfg.svc_perm = BK_PERM_SET(SVC_UUID_LEN, UUID_16);
    memcpy(&(ble_db_cfg.uuid[0]), &test_svc_uuid[0], 16);

    status = bk_ble_create_db(&ble_db_cfg);

    return status;
}

static void ble_session_dump(void)
{
    rt_kprintf("ble session len:%d\n", ble_session->len);
    rt_kprintf("ble session buf:%s\n",ble_session->response_buf);
    rt_kprintf("ble status:%d\n", ble_session->status);
}


static rt_err_t ble_push_data(uint8_t* buf, uint8_t len)
{
    //rt_kprintf("len:%d,buf:%s\r\n",len,buf);

    for(int i = 0; i< len; i++)
    {
        rt_kprintf("%c",buf[i]);
    }
    rt_kprintf("\r\n");
    if(ble_session->len + len > BLE_RESPONSE_LEN)
    {
        rt_kprintf("data len is too small\n");
        return -RT_ERROR;
    }
    memcpy(ble_session->response_buf + ble_session->len, buf, len);
    ble_session->len += len;

    return RT_EOK;
}

static void ble_clean_data(void)
{
    memset(ble_session->response_buf, 0x0, BLE_RESPONSE_LEN);
    ble_session->len = 0;
}

static void ble_write_callback(write_req_t *param)
{
    cJSON *root = RT_NULL;
    cJSON *ssid = RT_NULL;
    cJSON *password = RT_NULL;
    cJSON *openid= RT_NULL;
    if(AAAAS_IDX_BBB1_VAL_NTF_CFG==param->att_idx)
    {
        uint16_t ntf_cfg = (param->value[0]) | (param->value[1] << 8);
        if(ble_event_cb != NULL)
            ble_event_cb(BLE_CFG_NOTIFY, (void *)(&(ntf_cfg)));
        return;
    }

    if(ble_session->status == START)
    {
        ble_session->status = RECVING;
        ble_push_data(param->value, param->len);
        ble_session->tick = rt_tick_get();
        return;
    }
    else if(ble_session->status == RECVING)
    {
        /* timeout */
        if((rt_tick_get() - ble_session->tick) > BLE_TIMEOUT)
        {
            rt_kprintf("ble recv timeout\n");
            ble_clean_data();
            if(str_begin_with(param->value, "{") != RT_NULL)
            {
                ble_push_data(param->value, param->len);
                ble_session->tick = rt_tick_get();

            }
            else
            {
                ble_session->status = START;
                goto __restart;
            }
        }
        else
        {
            /* not timeout */
            ble_push_data(param->value, param->len);
            ble_session->tick = rt_tick_get();
        }
    }

    if(str_end_with(param->value, param->len, "}"))
    {
        root = cJSON_Parse(ble_session->response_buf);
        if(root == RT_NULL)
        {
            rt_kprintf("ble data parse failed\n");
            ble_session->status = START;
            goto __restart;
        }
        else
        {
            ssid = cJSON_GetObjectItem(root, "ssid");
            password = cJSON_GetObjectItem(root, "password");
            openid=cJSON_GetObjectItem(root, "openid");
            result_cb(ssid->valuestring, password->valuestring,openid->valuestring,RT_NULL, RT_NULL);
            ble_session->status = START;
            goto __restart;
        }
    }
    else
    {
        /* continue receive */
        ble_session->status = RECVING;
        goto __exit;
    }

__restart:
    if(root != RT_NULL)
    {
        cJSON_Delete(root);
        root = RT_NULL;
    }
    ble_clean_data();
__exit:
    return;
}

static uint8_t ble_read_callback(read_req_t *param)
{
    rt_kprintf("prf_id:%d, att_idx:%d\r\n", param->prf_id, param->att_idx);
    param->value[0] = 0x31;
    param->value[1] = 0x32;
    param->value[2] = 0x33;
    return 3;
}
static void ble_event_callback(ble_event_t event, void *param)
{
    switch(event)
    {
    case BLE_STACK_OK:
    {
        rt_kprintf("STACK INIT OK\r\n");
        ble_create_db();
    }
    break;
    case BLE_STACK_FAIL:
        rt_kprintf("STACK INIT FAIL\r\n");
        break;
    case BLE_CREATE_DB_OK:
    {
        rt_kprintf("BLE_CREATE_DB_OK\r\n");
        ble_create_prf_ok=1;
    }
    break;
    case BLE_CREATE_DB_FAIL:
        bk_printf("BLE_CREATE_DB_FAIL\r\n");
        break;
    case BLE_CONNECT:
        rt_kprintf("BLE CONNECT\r\n");
        break;
    case BLE_DISCONNECT:
    {
        rt_kprintf("BLE DISCONNECT\r\n");
    }
    break;
    case BLE_MTU_CHANGE:
        rt_kprintf("BLE_MTU_CHANGE:%d\r\n", *(uint16_t *)param);
        break;
    case BLE_CFG_NOTIFY:
        rt_kprintf("BLE_CFG_NOTIFY:%d\r\n", *(uint16_t *)param);
        break;
    case BLE_CFG_INDICATE:
        rt_kprintf("BLE_CFG_INDICATE:%d\r\n", *(uint16_t *)param);
        break;
    case BLE_TX_DONE:
        rt_kprintf("BLE_TX_DONE\r\n");
        break;

    default:
        rt_kprintf("UNKNOW EVENT\r\n");
        break;
    }
}

static void ble_start(void)
{
    uint8_t adv_idx, adv_name_len;
    uint8_t mac[6];
    char ble_name[20];

    wifi_get_mac_address((char *)mac, CONFIG_ROLE_STA);
    adv_name_len = rt_snprintf(ble_name, sizeof(ble_name), "bk-%02x%02x", mac[4], mac[5]);

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
        rt_kprintf("ERROR\r\n");
    }
}

static rt_err_t ble_netconfig_stop(void)
{
    appm_disconnect();
    rt_thread_delay(500);
    if(ERR_SUCCESS==appm_stop_advertising())
        rt_kprintf("appm_stop_advertising success\r\n");
    if(NULL!=ble_session->response_buf)
    {
        rt_free(ble_session->response_buf);
        ble_session->response_buf=NULL;
    }
    ble_stop();

    return RT_EOK;
}

rt_err_t bk_ble_netconfig_thread()
{
    extern rt_sem_t direct_conn_done_sem;
    if(direct_conn_done_sem != RT_NULL)
    {
        rt_kprintf(" rt_sem_release direct_conn_done_sem =%d \n",rt_sem_release(direct_conn_done_sem));
    }
    int time=rt_tick_get();
    if(BLE_NETCONFIG_START==ble_netconfig_state)
    {
        rt_kprintf("ble is already init\n");
        return RT_EOK;
    }
    ble_netconfig_state=BLE_NETCONFIG_START;
    ble_session->response_buf = rt_malloc(1024);
    if(ble_session->response_buf == RT_NULL)
    {
        rt_kprintf("malloc failed\n");
        return -RT_ENOMEM;
    }
    ble_clean_data();
    ble_session->len = 0;
    ble_session->tick = 0;
    ble_session->status = START;

    //ble activate

    ble_set_write_cb(ble_write_callback);
    ble_set_read_cb(ble_read_callback);
    ble_set_event_cb(ble_event_callback);
    ble_activate(NULL);
    gapm_set_max_mtu(AAAA_CHAR_DATA_LEN);
    while(!ble_create_prf_ok)
    {
        rt_thread_delay(10);
    }
    //ble start advertise
    ble_start();
    while(1)
    {
        if(BLE_NETCONFIG_STOP==ble_netconfig_state)
            break;
        #ifdef XIAOYA_OS
        if(rt_tick_get()-time>=NETCONFIG_TIMEOUT)
        {
            rt_kprintf("[PLAYER] netconfig timeout\r\n");
            xiaoya_player_tips(TIP_NET_CONFIG_TIMEOUT,0);
            break;
        }
        #endif
        rt_thread_delay(10);
    }
_exit:
    tid=NULL;
    ble_netconfig_stop();
    return RT_EOK;
}


int bk_ble_netconfig_start()
{
    if (tid)
    {
        rt_kprintf("bk_ble_netconfig_start already init.\n");
        return -1;
    }

    tid = rt_thread_create("ble_netconfig",
                           bk_ble_netconfig_thread,
                           RT_NULL,
                           1024 * 6,
                           20,
                           10);

    if (tid != RT_NULL)
    {
        rt_thread_startup(tid);
    }

    return 0;
}
int bk_ble_netconfig_stop()
{
    ble_netconfig_state=BLE_NETCONFIG_STOP;
}
void ble_send_wifi_connected_to_master()
{
    if(BLE_NETCONFIG_RECIVE_INFO==get_ble_netconfig_state())
    {
        uint8_t write_buffer[20]="wifi connected";
        bk_ble_send_ntf_value(strlen(write_buffer), write_buffer, 0, 4);
        rt_thread_delay(500);
    }
}

int get_ble_netconfig_state(void)
{
    return ble_netconfig_state;
}
static void wifi_got_ip_cb()
{
    ble_send_wifi_connected_to_master();
    bk_ble_netconfig_stop();
}

static void ble_netconfig_sample()
{
    // net_set_sta_ipup_callback(wifi_got_ip_cb);
    bk_ble_netconfig_stop();
    bk_ble_netconfig_start();
}
MSH_CMD_EXPORT(ble_netconfig_sample,ble_netconfig_sample);
MSH_CMD_EXPORT(bk_ble_netconfig_start,bk_ble_netconfig_start);
MSH_CMD_EXPORT(bk_ble_netconfig_stop,bk_ble_netconfig_stop);

#else // (CFG_SOC_NAME != SOC_BK7252N)

#include <string.h>
#include <stdint.h>
#include "rtos_pub.h"
#include "ble_api_5_x.h"
#include "app_ble.h"
#include "param_config.h"
#include "wlan_ui_pub.h"
#include "mem_pub.h"
#include "str_pub.h"

#define BK_ATT_DECL_PRIMARY_SERVICE_128     {0x00,0x28,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define BK_ATT_DECL_CHARACTERISTIC_128      {0x03,0x28,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define BK_ATT_DESC_CLIENT_CHAR_CFG_128     {0x02,0x29,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
#define NOTIFY_CHARACTERISTIC_AF6C_128      {0xfb,0x34, 0x9b,0x5f, 0x80 , 0x00,0x00, 0x80,0x00, 0x10 , 0x00 , 0x00  ,0xf7, 0xff, 0x00, 0x00}       /**/
#define WRITE_REQ_CHARACTERISTIC_AF6C_128   {0xfb,0x34, 0x9b,0x5f, 0x80 , 0x00,0x00, 0x80,0x00, 0x10 , 0x00 , 0x00  ,0xf8, 0xff, 0x00, 0x00}       /*写特征*/
static const uint8_t test_svc_uuid[16]   =  {0xfb,0x34, 0x9b,0x5f, 0x80, 0x00,0x00, 0x80,0x00, 0x10, 0x00, 0x00,0xf9, 0xff, 0x00, 0x00};           /*UUID*/

typedef void (*g_write_cb)(write_req_t *write_req);

static uint8_t g_adv_act = 0xFF;
static uint8_t g_con_idx = 0xFF;
static g_write_cb bt_write_cb = NULL;

enum
{
    TEST_IDX_SVC,
    //TEST_IDX_AF6A_VAL_CHAR,
    //TEST_IDX_AF6A_VAL_VALUE,
    //TEST_IDX_AF6B_VAL_CHAR,
    //TEST_IDX_AF6B_VAL_VALUE,
    TEST_IDX_AF6C_VAL_CHAR,
    TEST_IDX_AF6C_VAL_VALUE,
    TEST_IDX_AF6C_VAL_NTF_CFG,
    TEST_IDX_AF6D_VAL_CHAR,
    TEST_IDX_AF6D_VAL_VALUE,
    //TEST_IDX_AF6E_VAL_CHAR,
    //TEST_IDX_AF6E_VAL_VALUE,
    //TEST_IDX_AF6E_VAL_NTF_CFG,
    TEST_IDX_NB,
};

static bk_attm_desc_t test_att_db[TEST_IDX_NB] =
{
    [TEST_IDX_SVC           ]   = {BK_ATT_DECL_PRIMARY_SERVICE_128,   PROP(RD), 0},
    //[TEST_IDX_AF6A_VAL_CHAR]    = {BK_ATT_DECL_CHARACTERISTIC_128,    PROP(RD), 0},
    //[TEST_IDX_AF6B_VAL_CHAR]    = {BK_ATT_DECL_CHARACTERISTIC_128,    PROP(RD), 0},
    [TEST_IDX_AF6C_VAL_CHAR]    = {BK_ATT_DECL_CHARACTERISTIC_128,    PROP(RD), 0},
    [TEST_IDX_AF6C_VAL_VALUE]   = {NOTIFY_CHARACTERISTIC_AF6C_128,    PROP(N) | ATT_UUID(128), 512|OPT(NO_OFFSET)},
    [TEST_IDX_AF6C_VAL_NTF_CFG] = {BK_ATT_DESC_CLIENT_CHAR_CFG_128,   PROP(RD) | PROP(WR), OPT(NO_OFFSET)},

    [TEST_IDX_AF6D_VAL_CHAR]    = {BK_ATT_DECL_CHARACTERISTIC_128,    PROP(RD), 0},
    [TEST_IDX_AF6D_VAL_VALUE]   = {WRITE_REQ_CHARACTERISTIC_AF6C_128, PROP(WR) | ATT_UUID(128), 512|OPT(NO_OFFSET)},

    //[TEST_IDX_AF6E_VAL_CHAR]    = {BK_ATT_DECL_CHARACTERISTIC_128,    PROP(RD), 0},
    //[TEST_IDX_AF6E_VAL_NTF_CFG] = {BK_ATT_DESC_CLIENT_CHAR_CFG_128,   PROP(RD) | PROP(WR), OPT(NO_OFFSET)},
};

static void ble_notice_cb(ble_notice_t notice, void *param)
{
    switch (notice) {
    case BLE_5_STACK_OK:
        break;
    case BLE_5_WRITE_EVENT:
    {
        write_req_t *w_req = (write_req_t *)param;

        if(w_req == NULL)
        {
            break;
        }
        if (bt_write_cb && (w_req->att_idx == TEST_IDX_AF6D_VAL_VALUE)
                && (g_con_idx == w_req->conn_idx)) {
            bt_write_cb(w_req);
        }
        break;
    }
    case BLE_5_READ_EVENT:
    {
        read_req_t *r_req = (read_req_t *)param;
        bk_printf("read_cb:conn_idx:%d, prf_id:%d, add_id:%d\r\n",
                  r_req->conn_idx, r_req->prf_id, r_req->att_idx);
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
        if(g_con_idx == 0xFF)
        {
            bk_printf("BLE_5_CONNECT_EVENT:%d\r\n", c_ind->conn_idx);
            g_con_idx = c_ind->conn_idx;
        }
        break;
    }
    case BLE_5_DISCONNECT_EVENT:
    {
        discon_ind_t *d_ind = (discon_ind_t *)param;
        bk_printf("d_ind:conn_idx:%d,reason:%d\r\n", d_ind->conn_idx,d_ind->reason);
        if(g_con_idx == d_ind->conn_idx)
        {
            g_con_idx = 0xFF;
            rtos_delay_milliseconds(1000);
            //sys_wdtReboot();
        }
        break;
    }
    case BLE_5_CREATE_DB:
    {
        bk_printf("BLE_5_CREATE_DB ok\r\n");
        break;
    }
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
    case BLE_5_GAP_CMD_CMP_EVENT:
    {
        ble_cmd_cmp_evt_t *evt = (ble_cmd_cmp_evt_t *)param;
        bk_printf("BLE_5_GAP_CMD_CMP_EVENT cmd:0x%x,conn_idx:%d,status:0x%x\r\n",evt->cmd,evt->conn_idx,evt->status);
        break;
    }
    default:
        break;
    }
}

static void ble_netconfig_init(void)
{
    ble_set_notice_cb(ble_notice_cb);

    struct bk_ble_db_cfg ble_db_cfg;
    ble_db_cfg.att_db      = test_att_db;
    ble_db_cfg.att_db_nb   = TEST_IDX_NB;
    ble_db_cfg.prf_task_id = 0;
    ble_db_cfg.start_hdl   = 0;
    ble_db_cfg.svc_perm    = BK_PERM_SET(SVC_UUID_LEN, UUID_128);
    memcpy(&(ble_db_cfg.uuid[0]), &test_svc_uuid[0], 16);

    bk_ble_create_db(&ble_db_cfg);
}

static void ble_netconfig_advertise(void)
{
    if (g_adv_act == 0xFF) {
        struct adv_param adv_info;
        adv_info.channel_map = 7;
        adv_info.duration = 0;
        adv_info.prop = (1 << ADV_PROP_CONNECTABLE_POS) | (1 << ADV_PROP_SCANNABLE_POS);
        adv_info.interval_min = 160;
        adv_info.interval_max = 160;

        uint8_t mac[6];
        char ble_name[20];

        wifi_get_mac_address((char *)mac, CONFIG_ROLE_STA);
        uint8_t adv_name_len = rt_snprintf(ble_name, sizeof(ble_name), "bk7252n-%02x%02x", mac[4], mac[5]);

        adv_info.advData[0] = adv_name_len + 1;
        adv_info.advData[1] = 0x09;
        memcpy(&adv_info.advData[2], ble_name, adv_name_len);
        adv_info.advDataLen = adv_name_len + 2;

        adv_info.respData[0] = adv_name_len + 1;
        adv_info.respData[1] = 0x08;
        memcpy(&adv_info.respData[2], ble_name, adv_name_len);
        adv_info.respDataLen = adv_name_len + 2;
        g_adv_act = app_ble_get_idle_actv_idx_handle(ADV_ACTV);
        bk_ble_adv_start(g_adv_act, &adv_info, NULL);
    } else {
        app_ble_start_advertising(g_adv_act,0);
    }
}

void ble_netconfig_send_back(int success)
{
    char write_buffer[20];
    if(success)
        os_strcpy(write_buffer, "wifi connected") ;
    else
        os_strcpy(write_buffer, "wifi failed") ;

    bk_printf("send0 %d\r\n", g_con_idx);
    if (g_con_idx != 0xFF)
    {
        bk_printf("send 1\r\n");
        if(ERR_SUCCESS != bk_ble_send_ntf_value(os_strlen(write_buffer),(uint8_t*) write_buffer, 0, TEST_IDX_AF6C_VAL_VALUE))
        {
            bk_printf("ERROR\r\n");
        }
        rt_thread_delay(500);
    }
}

volatile int ble_netconfig_flag = 0;
void ble_netconfig_sta_rw_event_func(void *new_evt)
{
    rw_evt_type evt_type = *((rw_evt_type *)new_evt);

    if (evt_type == RW_EVT_STA_GOT_IP)
    {
        bk_printf("**************** RW_EVT_STA_GOT_IP\r\n");
        ble_netconfig_send_back(1);
        ble_netconfig_flag = 0;
    }
    else if (evt_type < RW_EVT_STA_CONNECTED &&
             evt_type > RW_EVT_STA_CONNECTING)
    {
        bk_printf("**************** sta_rw_event_func others:%d \r\n", evt_type);
        ble_netconfig_flag = 0;
        ble_netconfig_send_back(0);
    }
}

int ble_netconfig_sta_setup(char *ssid, char *wifi_key)
{
    network_InitTypeDef_st wNetConfig;
    int len;
    os_memset(&wNetConfig, 0x0, sizeof(network_InitTypeDef_st));

    len = os_strlen(ssid);
    if (32 < len)
    {
        bk_printf("ssid name more than 32 Bytes\r\n");
        return 0;
    }
    os_strcpy((char *)wNetConfig.wifi_ssid, ssid);

    if(wifi_key)
    {
        len = os_strlen(wifi_key);
        if (64 < len)
        {
            bk_printf("key more than 64 Bytes\r\n");
            return 0;
        }
        os_strcpy((char *)wNetConfig.wifi_key, wifi_key);
    }

    wNetConfig.wifi_mode = BK_STATION;
    wNetConfig.dhcp_mode = DHCP_CLIENT;
    wNetConfig.wifi_retry_interval = 100;

    bk_printf("ssid:%s key:%s\r\n", wNetConfig.wifi_ssid, wNetConfig.wifi_key);
    bk_wlan_start(&wNetConfig);

    return 1;
}

void ble_netconfig_cb( write_req_t *write_req )
{
    if(write_req) {
        char *buf = (char *)write_req->value;
        int len = (int)write_req->len;
        // repace ' ' to '\0',  to use srtlen
        for(int i=0; i<len; i++) {
            if(buf[i] == ' ') {
                buf[i] = 0;
            }
        }
        bk_printf("value:%p, %s, len:%d\r\n", write_req, write_req->value, os_strlen(buf));
#define BLE_NCFG     "blencfg"
#define BLE_NCFG_LEN    (os_strlen(BLE_NCFG))
        if(os_strcmp(buf, BLE_NCFG) == 0) {
            buf += BLE_NCFG_LEN + 1;
            len -= BLE_NCFG_LEN + 1;
            char *ssid = NULL, *key = NULL;

            if(len > 0) {
                while(len > 0) {
                    if(buf[0] == 0) {
                        buf++;
                        len--;
                    } else {
                        ssid = buf;
                        break;
                    }
                }

                if(ssid) {
                    buf += os_strlen(ssid) + 1;
                    len -= os_strlen(ssid) + 1;
                }

                while(len > 0) {
                    if(buf[0] == 0) {
                        buf++;
                        len--;
                    } else {
                        key = buf;
                        break;
                    }
                }
            }

            bk_printf("ssid %s, kety %s, conflag: %d\r\n", ssid, key, ble_netconfig_flag);
            if((ssid) && (ble_netconfig_flag == 0)) {
                ble_netconfig_flag = 1;
                bk_wlan_status_register_cb(ble_netconfig_sta_rw_event_func);
                ble_netconfig_sta_setup(ssid, key);
            }
        }
    }
}

void ble_netconfig_stop_adv( void )
{
    if (g_adv_act == 0xFF) {
        bk_printf("%s g_adv_act no init\r\n",__func__);
        return;
    }

    app_ble_stop_advertising(g_adv_act);
}

void ble_netconfig_disconnectlink(void)
{
    if (g_con_idx == 0xFF) {
        bk_printf("%s no connect\r\n",__func__);
        return;
    }

    bk_ble_disconnect(g_con_idx);
}

void ble_netconfig_start_adv( void )
{
    if (g_adv_act == 0xFF) {
        bk_printf("%s g_adv_act no init\r\n",__func__);
        return;
    }

    app_ble_start_advertising(g_adv_act,0);
}

void ble_netconfig_sample(void)
{
    bt_write_cb = ble_netconfig_cb;
    ble_netconfig_init();
    ble_netconfig_advertise();
}
MSH_CMD_EXPORT(ble_netconfig_sample, ble_netconfig_sample);

#endif
#endif


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

#ifndef __BLE_CONFIG_H_
#define __BLE_CONFIG_H_

#define BLE_TIMEOUT         500         /* ble timeout */
#define BLE_RESPONSE_LEN    1024
#define AAAA_CHAR_DATA_LEN  128
typedef int (*rt_ble_netconfig_result_cb)(char *ssid, char *password, char*ble_get_openid,void *user_data, void *userdata_len);

enum
{
    AAAAS_IDX_SVC,
    AAAAS_IDX_BBB0_VAL_CHAR,
    AAAAS_IDX_BBB0_VAL_VALUE,
    AAAAS_IDX_BBB1_VAL_CHAR,
    AAAAS_IDX_BBB1_VAL_VALUE,
    AAAAS_IDX_BBB1_VAL_NTF_CFG,
    AAAAS_IDX_NB,
};


enum ble_status
{
    START = 0,
    RECVING,
};

enum ble_netconfig_state
{
    BLE_NETCONFIG_STOP=0,
    BLE_NETCONFIG_START,
    BLE_NETCONFIG_RECIVE_INFO,

};


struct ble_session
{
    uint16_t len;
    char* response_buf;
    rt_tick_t tick;
    rt_ble_netconfig_result_cb result_cb;
    rt_uint8_t status;
};

int bk_ble_netconfig_start(void);
int bk_ble_netconfig_stop(void);
void ble_send_wifi_connected_to_master(void);
int get_ble_netconfig_state(void);
void ble_send_wifi_connected_to_master();

#endif

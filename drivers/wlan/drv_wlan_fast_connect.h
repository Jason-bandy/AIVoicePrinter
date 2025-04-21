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

#ifndef __DRV_WLAN_FAST_CONNECT_H__
#define __DRV_WLAN_FAST_CONNECT_H__
#if 0
#define FLASH_FAST_DATA_ADDR        (0x1FF000)

struct wlan_fast_connect
{
    unsigned char ssid[32];
    unsigned char bssid[6];
    unsigned char channel;
    unsigned char security;
    unsigned char psk[32];
};
typedef struct wlan_fast_connect wlan_fast_connect_t;

int wlan_fast_connect_info_write(wlan_fast_connect_t *data_info);
int wlan_fast_connect_info_read(wlan_fast_connect_t *data_info);
int wlan_fast_connect_info_erase(void);
#endif
#endif

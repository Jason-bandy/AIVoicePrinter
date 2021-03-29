#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "bk_log.h"

#define BLE_TAG       "ble"
#define BLE_LOGI(...) BK_LOGI(BLE_TAG, ##__VA_ARGS__)
#define BLE_LOGW(...) BK_LOGW(BLE_TAG, ##__VA_ARGS__)
#define BLE_LOGE(...) BK_LOGE(BLE_TAG, ##__VA_ARGS__)
#define BLE_LOGD(...) BK_LOGD(BLE_TAG, ##__VA_ARGS__)
#define BLE_LOG_RAW   BK_LOG_RAW

#ifdef __cplusplus
}
#endif

#pragma once

#include "bk_log.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IP_TAG "wifi"
#define IP_LOGI(...) BK_LOGI(IP_TAG, ##__VA_ARGS__)
#define IP_LOGW(...) BK_LOGW(IP_TAG, ##__VA_ARGS__)
#define IP_LOGE(...) BK_LOGE(IP_TAG, ##__VA_ARGS__)
#define IP_LOGD(...) BK_LOGD(IP_TAG, ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif


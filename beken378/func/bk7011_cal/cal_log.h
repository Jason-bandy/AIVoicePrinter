#pragma once

#include "bk_log.h"

#define CAL_TAG "cal"
#define CAL_LOGI(...) BK_LOGI(CAL_TAG, ##__VA_ARGS__)
#define CAL_LOGW(...) BK_LOGW(CAL_TAG, ##__VA_ARGS__)
#define CAL_LOGE(...) BK_LOGE(CAL_TAG, ##__VA_ARGS__)
#define CAL_LOGD(...) BK_LOGD(CAL_TAG, ##__VA_ARGS__)

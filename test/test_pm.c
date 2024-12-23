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

#include <rthw.h>
#include <rtthread.h>
#include "wlan_ui_pub.h"
#include <stdint.h>
#include <stdlib.h>
#include "test_config.h"
#include "wlan_dev.h"
#include "rtdevice.h"

#ifdef PM_TEST

int bk_wlan_enter_powersave(struct rt_wlan_device *device, int level)
{
    int result = 0;

    if (device == RT_NULL) return -RT_EIO;

    result = rt_device_control(RT_DEVICE(device), WIFI_ENTER_POWERSAVE, (void *)&level);

    return result;
}

int bk_wlan_set_listen_int(struct rt_wlan_device *device, int intv)
{
    int result = 0;

    if (device == RT_NULL) return -RT_EIO;

    result = rt_device_control(RT_DEVICE(device), WIFI_SET_LISTEN_INT, (void *)&intv);

    return result;
}

int bk_wlan_set_gpio_wakeup(struct rt_wlan_device *device, int gpio_id, int gpio_type)
{
    int result = 0;
    int params[2] = {gpio_id, gpio_type};

    if (device == RT_NULL) return -RT_EIO;

    result = rt_device_control(RT_DEVICE(device), WIFI_SET_GPIO_WAKEUP_CONFIG, (void *)&params);

    return result;
}

static int pm_level(int argc, char **argv)
{
    int level, intv;

    if (argc < 2)
    {
        rt_kprintf("input argc is err!\n");
        return -1;
    }

    level = atoi(argv[1]);
    if (level > 4)
    {
        rt_kprintf("nonsupport level %d\n", level);
        return -1;
    }

    intv = atoi(argv[2]);
    if (intv > 30)
    {
        rt_kprintf("nonsupport listen intval %d\n", intv);
        return -1;
    }

    {
        struct rt_wlan_device *sta_device = (struct rt_wlan_device *)rt_device_find(WIFI_DEVICE_STA_NAME);
        if (NULL != sta_device)
        {
            if (intv != 0)
            {
                bk_wlan_set_listen_int(sta_device, intv);
                rt_kprintf("bk_wlan_set_listen_int to %d\n", intv);
            }
            bk_wlan_enter_powersave(sta_device, level);
            rt_kprintf("bk_wlan_enter_powersave switch to %d\n", level);
        }
    }
    return 0;
}

static int pm_gpio(int argc, char **argv)
{
    int gpio_id, gpio_type;

    if (argc < 2)
    {
        rt_kprintf("input argc is err!\n");
        return -1;
    }

    gpio_id = atoi(argv[1]);
    gpio_type = atoi(argv[2]);

    if (gpio_type > 3)
    {
        rt_kprintf("nonsupport gpio wakeup type %d\n", gpio_type);
        return -1;
    }

    struct rt_wlan_device *sta_device = (struct rt_wlan_device *)rt_device_find(WIFI_DEVICE_STA_NAME);
    if (NULL != sta_device)
    {
        bk_wlan_set_gpio_wakeup(sta_device, gpio_id, gpio_type);
        rt_kprintf("set default gpio wakeup: %d type: %d\n", gpio_id, gpio_type);
    }

    return 0;
}


#ifdef RT_USING_FINSH
#include <finsh.h>

MSH_CMD_EXPORT(pm_level, pm_level 1);
MSH_CMD_EXPORT(pm_gpio, pm_gpio id type);

#endif /* RT_USING_FINSH */

#endif


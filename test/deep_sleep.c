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

#include "error.h"
#include "include.h"

#include <rthw.h>
#include <rtthread.h>
#include <rtdevice.h>

#include <stdint.h>
#include <stdlib.h>
#include <finsh.h>
#include <rtdef.h>

#include "manual_ps_pub.h"
#include "rtos_pub.h"
#include "test_config.h"
#include "gpio_pub.h"

#ifdef DEEP_SLEEP_TEST
static int htoi(char s[])
{
    int i;
    int n = 0;
    if (s[0] == '0' && (s[1]=='x' || s[1]=='X'))
    {
        i = 2;
    }
    else
    {
        i = 0;
    }
    for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z'); ++i)
    {
        if (tolower(s[i]) > '9')
        {
            n = 16 * n + (10 + tolower(s[i]) - 'a');
        }
        else
        {
            n = 16 * n + (tolower(s[i]) - '0');
        }
    }
    return n;
}

static void enter_deep_sleep_test(int argc,char *argv[])
{
    rtos_delay_milliseconds(10);
    PS_DEEP_CTRL_PARAM deep_sleep_param;

    deep_sleep_param.wake_up_way			= 0;

    deep_sleep_param.gpio_index_map      	= htoi(argv[1]);
    deep_sleep_param.gpio_edge_map       	= htoi(argv[2]);
    deep_sleep_param.gpio_last_index_map 	= htoi(argv[3]);
    deep_sleep_param.gpio_last_edge_map  	= htoi(argv[4]);
    deep_sleep_param.sleep_time     		= htoi(argv[5]);
    deep_sleep_param.wake_up_way     		= htoi(argv[6]);
    deep_sleep_param.lpo_32k_src			= htoi(argv[7]);

    if(argc == 8)
    {
        rt_kprintf("---deep sleep test param : 0x%0X 0x%0X 0x%0X 0x%0X %d %d %d\r\n",
                   deep_sleep_param.gpio_index_map,
                   deep_sleep_param.gpio_edge_map,
                   deep_sleep_param.gpio_last_index_map,
                   deep_sleep_param.gpio_last_edge_map,
                   deep_sleep_param.sleep_time,
                   deep_sleep_param.wake_up_way,
                   deep_sleep_param.lpo_32k_src);

        bk_enter_deep_sleep_mode(&deep_sleep_param);
    }
    else
    {
        rt_kprintf("---argc error!!! \r\n");
    }
}

MSH_CMD_EXPORT(enter_deep_sleep_test,test sleep mode);

#endif


#ifdef FORCE_LV_SLEEP_TEST
#if (1 == CFG_USE_FORCE_LOWVOL_PS)
#include "force_ps_pub.h"
#include "str_pub.h"
extern UINT32 bk_wlan_instant_lowvol_sleep( PS_DEEP_CTRL_PARAM *lowvol_param );
extern int bk_get_lv_sleep_wakeup_gpio_status(void);
extern int bk_misc_wakeup_get_gpio_num(void);
void lowvol_sleep_test(int argc,char *argv[])
{
    PS_DEEP_CTRL_PARAM sleep_param;

    sleep_param.gpio_index_map = os_strtoul(argv[1], NULL, 16);
    sleep_param.gpio_edge_map = os_strtoul(argv[2], NULL, 16);
    sleep_param.gpio_last_index_map = 0;
    sleep_param.gpio_last_edge_map = 0;
    sleep_param.sleep_time = os_strtoul(argv[3], NULL, 16);
    sleep_param.wake_up_way = os_strtoul(argv[4], NULL, 16);
    sleep_param.sleep_mode = os_strtoul(argv[5], NULL, 16);

    if(argc == 6)
    {
        if (MCU_NORMAL_SLEEP == sleep_param.sleep_mode)
        {
            os_printf("---normal sleep test param : 0x%0X 0x%0X %d s  %x\r\n",
                      sleep_param.gpio_index_map,
                      sleep_param.gpio_edge_map,
                      sleep_param.sleep_time,
                      sleep_param.wake_up_way);

            // use return value to indicate wakeup source
            os_printf("wakeup by %x\r\n", bk_wlan_instant_lowvol_sleep(&sleep_param));
        }
        else if (MCU_LOW_VOLTAGE_SLEEP == sleep_param.sleep_mode)
        {
            os_printf("---lowvol sleep test param : 0x%0X 0x%0X %d s  %d\r\n",
                      sleep_param.gpio_index_map,
                      sleep_param.gpio_edge_map,
                      sleep_param.sleep_time,
                      sleep_param.wake_up_way);

            bk_wlan_instant_lowvol_sleep(&sleep_param);

            if(PS_DEEP_WAKEUP_RTC == sleep_param.wake_up_way)
            {
                os_printf("wakeup by RTC %ds\r\n",sleep_param.sleep_time);
            }
            else if(PS_DEEP_WAKEUP_GPIO == sleep_param.wake_up_way)
            {
                bk_get_lv_sleep_wakeup_gpio_status();
                os_printf("wakeup by GPIO%d\r\n",bk_misc_wakeup_get_gpio_num());
            }
        }
        else
        {
            os_printf("---unsupported sleep mode:%d! \r\n", sleep_param.sleep_mode);
            return;
        }
    }
    else
    {
        os_printf("---argc error: %d!!! \r\n", argc);
    }
}
MSH_CMD_EXPORT(lowvol_sleep_test, force_lowvol_sleep_test);
#endif // CFG_USE_FORCE_LOWVOL_PS
#endif // FORCE_LV_SLEEP_TEST

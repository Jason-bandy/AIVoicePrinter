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

#include <stdint.h>

#include "sys_rtos.h"
#include "rtos_pub.h"
#include "power_save_pub.h"
#include "low_voltage_ps.h"

extern void WFI(void);
extern UINT32 mcu_power_save(UINT32 sleep_tick);
extern void rt_user_idle_hook(void);


/*
0: normal
1: cpu sleep. -4mA
2: RF sleep. 27mA
3: RF+CPU sleep. 7mA
4: standby to reset. 7uA
*/
static char log_print = 0;

static void idle_hook(void)
{
    rt_tick_t timeout_tick, delta_tick=0;

    rt_enter_critical();

    GLOBAL_INT_DECLARATION();
    GLOBAL_INT_DISABLE();
    /* get next os tick */
    timeout_tick = rt_timer_next_timeout_tick();
    if (timeout_tick != RT_TICK_MAX)
    {
        timeout_tick -= rt_tick_get();
    }

    #if (1 == CFG_LOW_VOLTAGE_PS)
    if (LV_PS_ENABLED)
    {
        UINT32 sleep_ms = BK_TICKS_TO_MS ( timeout_tick );
        if(sleep_ms > MCU_SLEEP_DURATION_MIN)
            lv_ps_sleep_check( timeout_tick );
        GLOBAL_INT_RESTORE();
    }
    else
    #endif
    {
        #if CFG_USE_MCU_PS
        /* sleep cpu */
        delta_tick = mcu_power_save(timeout_tick);
        if(log_print)
            rt_kprintf("s:%d, d:%d\n", timeout_tick, delta_tick);
        #else
        WFI();
        #endif

        if (delta_tick)
        {
            /* adjust OS tick */
            rt_tick_set(rt_tick_get() + delta_tick);
            GLOBAL_INT_RESTORE();
            /* check system timer */
            rt_timer_check();
        }
        else
        {
            GLOBAL_INT_RESTORE();
        }
    }

    rt_exit_critical();

    rt_user_idle_hook();
}

static int drv_pm_init(void)
{
    rt_kprintf("%s\n", __FUNCTION__);
    rt_thread_idle_sethook(idle_hook);

    return 0;
}

int set_log(int argc, char *argv[])
{
    char val;

    val = atoi(argv[1]);

    if(val == 1) {
        log_print = 1;
    } else {
        log_print = 0;
    }

    return 0;
}

MSH_CMD_EXPORT(set_log, set_log on or off);
INIT_DEVICE_EXPORT(drv_pm_init);

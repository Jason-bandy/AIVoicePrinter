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

#include "rtos_pub.h"
#include "BkDriverPwm.h"
#include "pwm_pub.h"
#include "error.h"
#include <stdint.h>
#include <stdlib.h>
#include <finsh.h>
#include "test_config.h"


#ifdef	PWM_TEST


static void pwm_test(int argc,char *argv[])
{
    UINT32 channel,duty_cycle,cycle;

    if(argc != 4)
        return;

    channel		= atoi(argv[1]);
    duty_cycle	= atoi(argv[2]);
    cycle		= atoi(argv[3]);

    if(cycle < duty_cycle)
    {
        rt_kprintf("pwm param error: end < duty\r\n");
        return;
    }

    rt_kprintf("---pwm %d test--- \r\n",channel);

    #if (CFG_SOC_NAME == SOC_BK7231N)
    bk_pwm_initialize(channel, cycle, duty_cycle,0,0);    /*pwm 模块初始化，设置对应通道的占空比*/
    #else
    bk_pwm_initialize(channel, cycle, duty_cycle);   /*pwm 模块初始化，设置对应通道的占空比*/
    #endif
    bk_pwm_start(channel);						/*启动pwm */

    rt_thread_delay(2000);

    bk_pwm_stop(channel);							/*关闭pwm */
    rt_kprintf("---pwm test stop---\r\n");
}

static void pwm_cap_test(int argc,char *argv[])
{
    UINT32 channel,PwmMode;

    if(argc != 3)
        return;

    channel		= atoi(argv[1]);
    PwmMode		= atoi(argv[2]);
    //duty_cycle	= atoi(argv[2]);
    //cycle		= atoi(argv[3]);

    rt_kprintf("---pwm %d test--- \r\n",channel);

    bk_pwm_capture_initialize(channel,PwmMode);		/*pwm 模块初始化，PwmMode: 1 :pos 2:neg*/

    rt_thread_delay(100);

}



MSH_CMD_EXPORT(pwm_test,pwm test);
MSH_CMD_EXPORT(pwm_cap_test,pwm cap test);

#endif

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
#include <rtdevice.h>

#include "drivers/rt_drv_pwm.h"

#include "typedef.h"
#include "drv_pwm.h"
#include "pwm.h"
#include "generic.h"

#include "drv_model_pub.h"
#include "BkDriverPwm.h"

#define MAX_PERIOD              (0xFFFFFFFFU / 26)
#define MIN_PERIOD              (2)

#define PWM_MIN_CHANNEL         (0)
#define PWM_MAX_CHANNEL         (5)

#ifdef RT_USING_PWM
static rt_err_t rt_pwm_set_channel(pwm_param_t *param, struct rt_pwm_configuration *configuration);
static rt_err_t drv_pwm_control(struct rt_device_pwm *device, int cmd, void *arg);
static struct rt_pwm_ops drv_ops =
{
    drv_pwm_control
};

static rt_err_t drv_pwm_enable(pwm_param_t *param, struct rt_pwm_configuration *configuration, rt_bool_t enable)
{
    rt_err_t result = RT_EOK;
    UINT32 ret = DRV_SUCCESS;

    result = rt_pwm_set_channel(param, configuration);
    if (result != RT_EOK)
    {
        return result;
    }

    if (!enable)
    {
        param->cfg.bits.en     = PWM_DISABLE;
        ret = bk_pwm_stop(param->channel);
    }
    else
    {
        param->cfg.bits.en     = PWM_ENABLE;
        ret = bk_pwm_start(param->channel);
    }

    if (ret != DRV_SUCCESS)
    {
        result = -RT_EIO;
    }

    return result;
}

static rt_err_t rt_pwm_set_channel(pwm_param_t *param, struct rt_pwm_configuration *configuration)
{
    if (configuration->channel < PWM_MIN_CHANNEL || configuration->channel > PWM_MAX_CHANNEL)
    {
        rt_kprintf("pwm channel invalid\n");
        return -RT_EINVAL;
    }
    param->channel = configuration->channel;

    return RT_EOK;
}

static rt_err_t drv_pwm_set(pwm_param_t *param, struct rt_pwm_configuration *configuration)
{
    UINT32 ret = RT_EOK;
    rt_uint32_t pulse;
    if (configuration->period < 0 || configuration->pulse < 0 || (configuration->period <= configuration->pulse))
    {
        rt_kprintf("invalid param\n");
        return -RT_ERROR;
    }

    configuration->period = configuration->period / 1000;
    pulse = configuration->pulse;
    configuration->pulse = configuration->pulse / 1000;
    if(configuration->period < MIN_PERIOD || configuration->period > MAX_PERIOD)
    {
        rt_kprintf("invalid param, period should be 2000ns ~ %dns\n", MAX_PERIOD*1000);
        return -RT_ERROR;
    }
    if(configuration->pulse == 0)
    {
        rt_kprintf("invalid param, pulse should not be 0. %d/1000=0\n", pulse);
        return -RT_ERROR;
    }
    ret = rt_pwm_set_channel(param, configuration);
    if (ret == -RT_EINVAL)
    {
        return ret;
    }

    param->duty_cycle = configuration->pulse * 26;
    param->end_value = configuration->period * 26;
    if(bk_pwm_check_is_used(param->channel) == 0)
    {
        ret = bk_pwm_initialize(param->channel, param->end_value, param->duty_cycle);
    }
    else
    {
        ret = bk_pwm_update_param(param->channel, param->end_value, param->duty_cycle);
    }

    return ret;
}

static rt_err_t drv_pwm_control(struct rt_device_pwm *device, int cmd, void *arg)
{
    struct rt_pwm_configuration *configuration = (struct rt_pwm_configuration *)arg;
    pwm_param_t *param = (pwm_param_t *)device->parent.user_data;

    switch (cmd)
    {
    case PWM_CMD_ENABLE:
        return drv_pwm_enable(param, configuration, RT_TRUE);
    case PWM_CMD_DISABLE:
        return drv_pwm_enable(param, configuration, RT_FALSE);
    case PWM_CMD_SET:
        return drv_pwm_set(param, configuration);
    // case PWM_CMD_GET:
    //     return drv_pwm_get(param, configuration);
    default:
        return -RT_EINVAL;
    }
}

pwm_param_t pwm_param;

static void rt_pwm_init(void)
{
    pwm_param.channel         = PWM0;
    pwm_param.cfg.bits.en     = PWM_DISABLE;
    pwm_param.cfg.bits.int_en = PWM_INT_DIS;
    pwm_param.cfg.bits.mode   = PWM_PWM_MODE;
    pwm_param.cfg.bits.clk    = PWM_CLK_26M;
    pwm_param.p_Int_Handler   = 0;
    pwm_param.duty_cycle      = 0;
    pwm_param.end_value       = 0x00;
}
#endif /* RT_USING_PWM */

int drv_pwm_init(void)
{
    #ifdef RT_USING_PWM
    rt_pwm_init();
    rt_device_pwm_register(rt_calloc(1, sizeof(struct rt_device_pwm)), "pwm", &drv_ops, &pwm_param);
    #endif /* RT_USING_PWM */
    return RT_EOK;
}
INIT_DEVICE_EXPORT(drv_pwm_init);

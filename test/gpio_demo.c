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

/*
* 程序清单： 这是一个PIN 设备使用例程
* 例程导出了pin_led_sample 命令到控制终端
* 命令调用格式： pin_led_sample
* 程序功能： 通过按键控制led 对应引脚的电平状态控制led
*/
#include <rtthread.h>
#include <rtdevice.h>
#include "test_config.h"

#ifdef GPIO_DEMO
#define LED_PIN_NUM 24
#define LED1_PIN_NUM 26
#define KEY4_PIN_NUM 2
#define KEY3_PIN_NUM 3

void led_on(void *args) {
    rt_kprintf("turn on led!\n");
    rt_pin_write(LED_PIN_NUM, PIN_HIGH);
}

void led_off(void *args) {
    rt_kprintf("turn off led!\n");
    rt_pin_write(LED_PIN_NUM, PIN_LOW);
}

static void pin_led_sample(void) {
    /* led 引脚为输出模式*/
    rt_pin_mode(LED_PIN_NUM, PIN_MODE_OUTPUT);
    /* 默认低电平*/
    rt_pin_write(LED_PIN_NUM, PIN_HIGH);
    /* 按键0引脚为输入模式*/
    rt_pin_mode(KEY4_PIN_NUM, PIN_MODE_INPUT_PULLUP);
    /* 绑定中断， 下降沿模式， 回调函数名为beep_on */
    rt_pin_attach_irq(KEY4_PIN_NUM, PIN_IRQ_MODE_FALLING, led_on, RT_NULL);
    /* 使能中断*/
    rt_pin_irq_enable(KEY4_PIN_NUM, PIN_IRQ_ENABLE);
    /* 按键1引脚为输入模式*/
    rt_pin_mode(KEY3_PIN_NUM, PIN_MODE_INPUT_PULLUP);
    /* 绑定中断， 下降沿模式， 回调函数名为led_off */
    rt_pin_attach_irq(KEY3_PIN_NUM, PIN_IRQ_MODE_FALLING, led_off, RT_NULL);
    /* 使能中断*/
    rt_pin_irq_enable(KEY3_PIN_NUM, PIN_IRQ_ENABLE);

    /* led 引脚为输出模式*/
    rt_pin_mode(LED1_PIN_NUM, PIN_MODE_OUTPUT);
    /* 默认低电平*/
    rt_pin_write(LED1_PIN_NUM, PIN_HIGH);
}
/* 导出到msh 命令列表中*/
MSH_CMD_EXPORT(pin_led_sample, pin led sample);
#endif


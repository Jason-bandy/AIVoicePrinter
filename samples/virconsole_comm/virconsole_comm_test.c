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

#include <rtdevice.h>
#include <rthw.h>
#include <rtthread.h>
#include <string.h>
#include "comm_uart.h"
#include "virtual_console.h"


static void comm_uart_parse_thread_entry(void *param)
{
    uint8_t ch = 0;
    uint8_t index = 0;
    uint8_t bexit = 0;
    uint8_t strbuf[128] = {0};


    while (!bexit)
    {
        ch = comm_uart_getchar();
        if (ch == '\n')
        {
            if(memcmp(strbuf,"test",strlen("test")) == 0)
            {
                comm_uart_sendstr("test ok\n",strlen("test ok\n"));
            }
            else
            {
                comm_uart_sendstr("exit ok\n",strlen("exit ok\n"));
                bexit = 1;
            }

            memset(strbuf, 0, 128);
            index = 0;
        }
        else
        {
            strbuf[index] = ch;
            index++;
        }
    }

    comm_uart_deinit();
    virtual_console_stop();//resume to normal mode
    virtual_console_deinit();
    rt_kprintf("comm_uart_parse_thread_entry exit!\r\n");
}

static int comm_uart_thread_start(void)
{
    rt_thread_t tid = rt_thread_create(RT_CONSOLE_DEVICE_NAME"thread", comm_uart_parse_thread_entry, RT_NULL, 2048 * 1, 6, 10);
    if (tid == RT_NULL)
    {
        return -1;
    }
    rt_thread_startup(tid);

    return RT_EOK;
}

int virconsole_comm_init(void)
{
    comm_uart_init();
    comm_uart_thread_start();

    return 0;
}

/*from debug mode switch to virtual console mode and do uart communication*/
static void virconsole_comm_test(int argc,char *argv[])
{
    rt_kprintf("virconsole_comm_test...\r\n");

    /*switch to virtual console*/
    virtual_console_init();
    virtual_console_start();

    virconsole_comm_init();//do uart communication
}
MSH_CMD_EXPORT(virconsole_comm_test, test);

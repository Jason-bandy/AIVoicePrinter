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

#include "includes.h"
#include "rtos_pub.h"
#include "mem_pub.h"
#include "uart_pub.h"
#include "video_transfer.h"
#include "audio_device.h"
//#include "ff.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dfs_fs.h>
#include <dfs_file.h>
#include "dfs_private.h"
#include <dfs_posix.h>
#include "sys_ctrl_pub.h"
#include "audio.h"
#include "test_config.h"

#ifdef MIC_SDCARD_TEST

#define AUD_DAC_SINGLE_PORT         1
#define AUD_DAC_DIFF_PORT           2
#define AUD_DAC_USE_PORT_SET        AUD_DAC_SINGLE_PORT

static beken_thread_t aud_thread_handle = NULL;

static void aud_task( void *para )
{
    int fd = -1;
    uint32_t len;
    uint8_t *pdata = malloc(320);

    /* mount sd card fat partition 1 as root directory */
    saradc_config_vddram_voltage(PSRAM_VDD_3_3V);
    if(dfs_mount("sd0", "/sd", "elm", 0, 0) == 0)
        rt_kprintf("SD File System initialized!\n");
    else
        rt_kprintf("SD File System initialzation failed!\n");

    #if(AUD_DAC_USE_PORT_SET == AUD_DAC_SINGLE_PORT)
    audio_dac_volume_use_single_port();
    #else
    audio_dac_volume_diff_port();
    #endif

    audio_device_init();
    audio_device_mic_set_channel(1);
    audio_device_mic_set_rate(8000);
    audio_device_mic_set_volume(60);
    audio_device_mic_open();

    fd = open("/sd/test.pcm", O_WRONLY | O_CREAT | O_BINARY | O_TRUNC, 0);
    bk_printf("fd = %d\n", fd);
    if (fd != 0) {
        for (int i = 0; i < 100; i++)
        {
            len = audio_device_mic_read(pdata,320);
            bk_printf("len = %d\n", len);
            write(fd, pdata, len);
        }
    }

    bk_printf("aud_task exit\r\n");
    close(fd);
    audio_device_mic_close();
    aud_thread_handle = NULL;
    rtos_delete_thread(NULL);

}

static void aud_task_init(void)
{
    if(aud_thread_handle)
        return;

    rtos_create_thread(&aud_thread_handle,
                       5,
                       "aud",
                       (beken_thread_function_t)aud_task,
                       4096,
                       0);
}

static void mic_test(int argc, char **argv)
{
    aud_task_init();
}

MSH_CMD_EXPORT(mic_test, mic_test);
#endif
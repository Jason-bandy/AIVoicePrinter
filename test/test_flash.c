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

#include "test_config.h"
#include <string.h>
#include <stdlib.h>
#include <rtthread.h>
#include <rthw.h>
#include <rtdevice.h>
#include <finsh.h>
#include "BkDriverFlash.h"

#ifdef RBL_HEADER_COPY_TEST

static int flash_protect(int argc, char **argv)
{
    PROTECT_TYPE protect_val = FLASH_PROTECT_NONE;

    if(argc == 2)
    {
        protect_val = atoi(argv[1]);

        if((uint32_t)protect_val > FLASH_UNPROTECT_LAST_BLOCK)
        {
            protect_val = 0;
        }
    }

    rt_kprintf("flash_protect  ==> %d\n", protect_val);

    bk_flash_enable_security(protect_val);

    return 0;
}
MSH_CMD_EXPORT(flash_protect, flash_protect);

static int flash_erase(int argc, char **argv)
{
    if(argc == 2)
    {
        uint32_t address = atoi(argv[1]);

        rt_kprintf("flash_erase %d 0x%08X\n", address, address);

        flash_ctrl(CMD_FLASH_ERASE_SECTOR, &address);
    }
    else
    {
        rt_kprintf("ex: flash_erase 2093056\n");
    }

    return 0;
}
MSH_CMD_EXPORT(flash_erase, flash_erase 2093056);

#define __is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')
static void dump_hex(const rt_uint8_t *ptr, rt_size_t buflen)
{
    unsigned char *buf = (unsigned char*)ptr;
    int i, j;

    for (i=0; i<buflen; i+=16)
    {
        rt_kprintf("%08X: ", i);

        for (j=0; j<16; j++)
            if (i+j < buflen)
                rt_kprintf("%02X ", buf[i+j]);
            else
                rt_kprintf("   ");
        rt_kprintf(" ");

        for (j=0; j<16; j++)
            if (i+j < buflen)
                rt_kprintf("%c", __is_print(buf[i+j]) ? buf[i+j] : '.');
        rt_kprintf("\n");
    }
}

static int flash_dump(int argc, char **argv)
{
    uint32_t address, len;
    void *data;

    if(argc != 3)
    {
        rt_kprintf("ex: flash_dump 2093056\n");
        return -1;
    }

    address = atoi(argv[1]);
    len = atoi(argv[2]);

    rt_kprintf("flash_dump 0x%08X %d\n", address, len);

    if( (address > (1024*1024*4)) || (len > 256) )
    {
        rt_kprintf("out of range\n");
        return -1;
    }

    data = rt_malloc(len);
    if(!data)
    {
        rt_kprintf("no memory for data\n");
        return -1;
    }

    flash_read(data, len, address);

    dump_hex(data, len);

    rt_free(data);

    return 0;
}
MSH_CMD_EXPORT(flash_dump, flash_dump 2093056 32);

static int flash_copy(int argc, char **argv)
{
    uint32_t address_src, address_dest, len;
    void *data;

    if(argc != 4)
    {
        rt_kprintf("ex: flash_copy 0 4096 256\n");
        return -1;
    }

    address_src = atoi(argv[1]);
    address_dest = atoi(argv[2]);
    len = atoi(argv[3]);
    rt_kprintf("address src 0x%08X ==> dest 0x%08X len:%d\n", address_src, address_dest, len);

    data = rt_malloc(len);
    if(!data)
    {
        rt_kprintf("no memory for data\n");
        return -1;
    }

    flash_read(data, len, address_src);
    flash_write(data, len, address_dest);

    rt_free(data);

    return 0;
}
MSH_CMD_EXPORT(flash_copy, flash_dump 2093056 32);
#define PRINT_RBL_INFO
#include <dfs_posix.h>
/**
 *指令格式 rbl_header_copy 2084864 /sd/rtthread.rbl
 *2084864为内部flash倒数第三个分区的起始地址,如对应2M bytes为0x1fd000<-->2084864
*/
static int rbl_header_copy(int argc, char **argv)
{
    uint32_t address = atoi(argv[1]);
    const char *fn = argv[2];
    int fd = open(fn, O_RDONLY, 0);

    if(fd >= 0)
    {
        uint8_t data[96];
        int len = 96;

        read(fd, data, 96);
        #if defined (PRINT_RBL_INFO)
        int i;
        for(i = 0; i<96; i++)
        {
            rt_kprintf("%02x ",data[i]);
            if((i+1)%16 == 0)
                rt_kprintf("\r\n");
        }
        #endif

        bk_flash_enable_security(/*FLASH_PROTECT_HALF*/FLASH_PROTECT_NONE); // half or custom

        rt_kprintf("flash_erase %d 0x%08X\n", address, address);
        flash_ctrl(CMD_FLASH_ERASE_SECTOR, &address);
        flash_write((char *)data, len, address);

        #if defined (PRINT_RBL_INFO)
        memset(data,0,len);
        flash_read((char *)data, len, address);
        for(i = 0; i<96; i++)
        {
            rt_kprintf("%02x ",data[i]);
            if((i+1)%16 == 0)
                rt_kprintf("\r\n");
        }
        #endif

        close(fd);
    }
    else
    {
        rt_kprintf("[%s] fd:%d\n",__func__,fd);
    }

    return 0;
}
MSH_CMD_EXPORT(rbl_header_copy, copy rbl.header to flash);

static void flash_verify_thread_entry(void *parameter)
{
    rt_kprintf("flash_verify start\n");
    bk_logic_partition_t *pt = bk_flash_get_info(BK_PARTITION_OTA);

    uint32_t src_addr = pt->partition_start_addr;
    uint32_t test_len = pt->partition_length;

    uint32_t len = 0;

    uint32_t address = src_addr;

    uint32_t loop_times = 0;

    char *data = rt_malloc(4096);
    if(!data)
    {
        rt_kprintf("no memory for data\n");
        rt_free(data);
        return;
    }

    while(1)
    {
        rt_memset(data, 0, 4096);

        bk_flash_enable_security(FLASH_PROTECT_NONE);
        flash_ctrl(CMD_FLASH_ERASE_SECTOR, &address);
        rt_thread_mdelay(1000);
        bk_flash_enable_security(FLASH_PROTECT_ALL);

        flash_read(data, 4096, address);

        for(uint32_t i = 0; i < 4096; i++)
        {
            if(data[i] != 0xFF)
            {
                rt_kprintf("erase error\n");
                break;
            }
        }

        rt_kprintf("earse 4K pass 0x%06x - 0x%06x\n", address, address + 4096);

        bk_flash_enable_security(FLASH_PROTECT_NONE);
        rt_memset(data, 0xA5, 4096);
        flash_write(data, 4096, address);
        rt_thread_mdelay(100);
        bk_flash_enable_security(FLASH_PROTECT_ALL);

        flash_read(data, 4096, address);
        rt_thread_mdelay(100);

        for(uint32_t i = 0; i < 4096; i++)
        {
            if(data[i] != 0xA5)
            {
                rt_kprintf("write 0xA5 error\n");
                break;
            }
        }

        rt_kprintf("write 4K 0xA5 pass 0x%06x - 0x%06x\n", address, address + 4096);

        bk_flash_enable_security(FLASH_PROTECT_NONE);
        rt_memset(data, 0x5A, 4096);
        flash_write(data, 4096, address);
        rt_thread_mdelay(100);
        bk_flash_enable_security(FLASH_PROTECT_ALL);

        flash_read(data, 4096, address);
        rt_thread_mdelay(100);

        for(uint32_t i = 0; i < 4096; i++)
        {
            if(data[i] != 0x00)
            {
                rt_kprintf("write 0x5A error\n");
                break;
            }
        }

        rt_kprintf("write 4K 0x5A pass 0x%06x - 0x%06x\n", address, address + 4096);

        len += 4096;
        address += 4096;
        if(address == (src_addr + test_len))
        {
            loop_times += 1;
            rt_kprintf("test loop finish %d\n", loop_times);
            address = src_addr;
        }
    }
}

static int flash_verify(int argc, char **argv)
{
    rt_thread_t tid = RT_NULL;

    /* create record thread */
    tid = rt_thread_create("flash_verify",
                            flash_verify_thread_entry,
                            RT_NULL,
                            1024 * 4,
                            27,
                            10);
    if (tid != RT_NULL)
        rt_thread_startup(tid);

    return 0;
}
MSH_CMD_EXPORT(flash_verify, flash_verify);
#endif

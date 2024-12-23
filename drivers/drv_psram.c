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

#include <rtthread.h>
#include <board.h>
#include "drv_psram.h"
#include "qspi_pub.h"
#include "mem_pub.h"
#include "rtos_pub.h"

#if (CFG_SOC_NAME != SOC_BK7231N && CFG_SOC_NAME != SOC_BK7252N)
#define RT_HW_PSRAM_BEGIN (void*)(0x3000000)
#define RT_HW_PSRAM_END   (void*)(0x3000000 + 8 *1024 * 1024)

extern OSStatus bk_qspi_dcache_initialize(qspi_dcache_drv_desc *qspi_config);
extern OSStatus bk_qspi_start(void);

struct rt_memheap _psram_heap;
static struct rt_memheap *psram_heap = &_psram_heap;

void rt_psram_heap_init(void)
{
    rt_kprintf("rt_psram_heap_init complete.\n");
}

void *psram_malloc(unsigned long size)
{
    return rt_memheap_alloc(psram_heap, size);
}

void psram_free(void *ptr)
{
    rt_memheap_free(ptr);
}

void *psram_calloc(unsigned int n, unsigned int size)
{
    void* ptr = NULL;

    ptr = psram_malloc(n * size);
    if (ptr) {
        os_memset(ptr, 0, n * size);
    }

    return ptr;
}

void *psram_realloc(void *ptr, unsigned long size)
{
    return rt_memheap_realloc(psram_heap, ptr, size);
}

/*
 * line_mode      0: 1 line; 1: 4 line
 * voltage_level  0: 1.8v; 1: 2.5v; 2: 3.3v
 */
void psram_init(uint8_t line_mode,uint8_t voltage_level)
{
    #if CFG_USE_DCACHE
    bk_qspi_psram_reset_enable();
    bk_qspi_psram_reset();
    #endif

    qspi_dcache_drv_desc qspi_cfg;
    uint8_t SetLineMode = 0;

    if(line_mode == 0)
    {
        SetLineMode = 0;
    }
    else if(line_mode == 1)
    {
        SetLineMode = 3;
    }
    else
    {
        printf("line_mode err!\r\n");
        return;
    }

    if(voltage_level > 3)
    {
        printf("voltage_level err!\r\n");
        return;
    }

    bk_qspi_psram_quad_mode_switch(line_mode);
    qspi_cfg.mode = 0x00 | SetLineMode; 	// 0: 1 line mode	 3: 4 line mode
    qspi_cfg.clk_set = 0x10;
    qspi_cfg.wr_command = SetLineMode ? 0x38 : 0x02;		//write
    qspi_cfg.rd_command = SetLineMode ? 0xEB : 0x03;		//read
    qspi_cfg.wr_dummy_size = 0;
    qspi_cfg.rd_dummy_size = SetLineMode ? 0x06 : 0x00;
    qspi_cfg.voltage_level = voltage_level;

    bk_qspi_dcache_initialize(&qspi_cfg);
    bk_qspi_start();

    rt_memheap_init(&_psram_heap, "PSRAM", RT_HW_PSRAM_BEGIN, RT_HW_PSRAM_END - RT_HW_PSRAM_BEGIN);

    printf("rt_memheap_init ok!\r\n");
}
#elif (CFG_SOC_NAME == SOC_BK7252N)
#include "BkDriverQspi.h"
#define RT_HW_PSRAM_BEGIN (void*)(0x40000000)
#define RT_HW_PSRAM_END   (void*)(RT_HW_PSRAM_BEGIN + 8 *1024 * 1024)

//extern OSStatus bk_qspi_dcache_initialize(qspi_dcache_drv_desc *qspi_config);
//extern OSStatus bk_qspi_start(void);

struct rt_memheap _psram_heap;
static struct rt_memheap *psram_heap = &_psram_heap;
volatile uint32_t rt_psram_heap_init_flag = 0;

void rt_psram_heap_init(void)
{
    if(rt_psram_heap_init_flag == 0)
    {
        psram_init(1, 0);
        rt_kprintf("rt_psram_heap_init complete.\n");
    }
}

void *psram_malloc(unsigned long size)
{
    if(rt_psram_heap_init_flag == 2)
        return rt_memheap_alloc(psram_heap, size);
    else
        return NULL;
}

void psram_free(void *ptr)
{
    if(rt_psram_heap_init_flag == 2)
        rt_memheap_free(ptr);
}

void *psram_calloc(unsigned int n, unsigned int size)
{
    void* ptr = NULL;

    ptr = psram_malloc(n * size);
    if (ptr) {
        os_memset(ptr, 0, n * size);
    }

    return ptr;
}

void *psram_realloc(void *ptr, unsigned long size)
{
    if(rt_psram_heap_init_flag == 2)
        return rt_memheap_realloc(psram_heap, ptr, size);
    else
        return NULL;
}

/*
 * line_mode      0: 1 line; 1: 4 line
 * voltage_level  0: 1.8v; 1: 2.5v; 2: 3.3v
 */
void psram_init(uint8_t line_mode, uint8_t voltage_level)
{
    OSStatus ret;
    (void) line_mode;
    (void) voltage_level;

    if(rt_psram_heap_init_flag != 0)
    {
        rt_kprintf("psram_inited\r\n");
        return;
    }

    rt_psram_heap_init_flag = 1;
    // config qspi psram
    if(line_mode == 0)
        line_mode = QSPI_1WIRE;
    else if(line_mode == 1)
        line_mode = QSPI_4WIRE;
    else
    {
        rt_kprintf("line mode %d not supported\r\n",line_mode);
        return;
    }

    ret = bk_qspi_psram_initialize(line_mode);
    if(ret != kNoErr)
    {
        rt_kprintf("qspi init failed\r\n");
        return;
    }

    // enter_quad_mode
    ret = bk_qspi_psram_enter_quad_mode();
    if(ret != kNoErr)
    {
        rt_kprintf("enter_quad_mode failed\r\n");
        return;
    }

    ret = bk_qspi_psram_quad_write();
    if(ret != kNoErr)
    {
        rt_kprintf("enable quad write failed\r\n");
        return;
    }

    ret = bk_qspi_psaram_quad_read();
    if(ret != kNoErr)
    {
        rt_kprintf("enable quad read failed\r\n");
        return;
    }

    // enable cpu mode, and clean count
    ret = bk_qspi_psram_switch_mcu_mode();
    if(ret != kNoErr)
    {
        rt_kprintf("enable cpu mode failed\r\n");
        return;
    }

    rt_kprintf("rt_memheap_init %08x - %08x, %08x\r\n", RT_HW_PSRAM_BEGIN, RT_HW_PSRAM_END, RT_HW_PSRAM_END - RT_HW_PSRAM_BEGIN);
    rt_memheap_init(&_psram_heap, "PSRAM", RT_HW_PSRAM_BEGIN, RT_HW_PSRAM_END - RT_HW_PSRAM_BEGIN);
    rt_kprintf("rt_memheap_init ok!\r\n");

    rt_psram_heap_init_flag = 2;
}
#endif

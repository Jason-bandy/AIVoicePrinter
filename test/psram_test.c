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
#include "sys_config.h"

#ifdef PSRAM_TEST
#if (CFG_SOC_NAME != SOC_BK7231N)
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <rtthread.h>
#include <rtdevice.h>
#include "drv_psram.h"
#include "drv_wdt.h"

static void psram_mem_rw_test(uint32_t start_address, uint32_t size)
{
    uint32_t i;
    uint8_t *p_uint8_t = NULL;

    extern void rt_hw_wdg_stop(void);
    rt_hw_wdg_stop();

    p_uint8_t = (uint8_t *)start_address;
    for (i = 0; i < size / sizeof(uint8_t); i++)
    {
        *p_uint8_t++ = (uint8_t)i;
    }

    p_uint8_t = (uint8_t *)start_address;
    for (i = 0; i < size / sizeof(uint8_t); i++)
    {
        if (*p_uint8_t != (uint8_t)i)
        {
            printf("err i=%d, rd:%x,wd:%x\r\n", i, (uint8_t)*p_uint8_t, (uint8_t)i);
            break;
        }
        p_uint8_t++;
    }

    rt_hw_wdg_start(0,NULL);
}

/*
 * NOTE:MUST INIT PSRAM FIRST BEFORE OPERATE PSRAM MEMORY
 * eg. psram_mem_api_test malloc/calloc/realloc 1024
 */
static void psram_mem_api_test(int argc, char * argv [])
{
    static uint8_t bpsram_init = 0;
    uint8_t *malloc_ptr,*calloc_ptr,*realloc_ptr;


    printf("psram_mem_api_test start!\r\n");

    malloc_ptr = calloc_ptr = realloc_ptr = NULL;

    if(bpsram_init == 0)
    {
        psram_init(1,2);//NOTE:should set according to the actual situation! here default set as 1 line;3.3v
        bpsram_init = 1;
    }

    uint32_t  nksize = atoi(argv[2]);
    if(strcmp(argv[1],"malloc") == 0)
    {
        malloc_ptr = (uint8_t *)psram_malloc(nksize * 1024);
        if(!malloc_ptr)
        {
            printf("psram_malloc fail!\r\n");
        }
        printf("%p\r\n",malloc_ptr);
        psram_mem_rw_test((uint32_t)malloc_ptr,nksize * 1024);
    }
    else if(strcmp(argv[1],"calloc") == 0)
    {
        calloc_ptr = (uint8_t *)psram_calloc(2,nksize * 1024);
        if(!calloc_ptr)
        {
            printf("psram_calloc fail!\r\n");
        }
        printf("%p\r\n",calloc_ptr);
        psram_mem_rw_test((uint32_t)calloc_ptr,2 * nksize * 1024);
    }
    else if(strcmp(argv[1],"realloc") == 0)
    {
        uint8_t *p = (uint8_t *)psram_malloc(nksize * 1024);
        realloc_ptr = psram_realloc(p,nksize * 1024 + 1024);
        if(!realloc_ptr)
        {
            printf("psram_realloc fail!\r\n");
        }
        printf("%p\r\n",realloc_ptr);
        psram_mem_rw_test((uint32_t)realloc_ptr,nksize * 1024 + 1024);
    }
    else
    {
        printf("param err!\r\n");
    }

    if(malloc_ptr)
    {
        printf("free %p\r\n",malloc_ptr);
        psram_free(malloc_ptr);
        malloc_ptr = NULL;
    }

    if(calloc_ptr)
    {
        printf("free %p\r\n",calloc_ptr);
        psram_free(calloc_ptr);
        calloc_ptr = NULL;
    }

    if(realloc_ptr)
    {
        printf("free %p\r\n",realloc_ptr);
        psram_free(realloc_ptr);
        realloc_ptr = NULL;
    }

    printf("psram_mem_api_test end!\r\n\r\n");
}

MSH_CMD_EXPORT(psram_mem_api_test, psram_mem_api_test);
#endif  // (CFG_SOC_NAME != SOC_BK7231N)
#endif  // ifdef(PSRAM_TEST)

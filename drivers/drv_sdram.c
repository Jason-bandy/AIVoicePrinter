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
#include "string.h"

extern struct rt_memheap _heap;
static struct rt_memheap *sdram_heap = &_heap;

void rt_sdram_heap_init(void)
{
    rt_kprintf("rt_sdram_heap_init complete.\n");
}

void *sdram_malloc(unsigned long size)
{
    return rt_memheap_alloc(sdram_heap, size);
}

void sdram_free(void *ptr)
{
    rt_memheap_free(ptr);
}

void *sdram_calloc(unsigned int n, unsigned int size)
{
    void* ptr = NULL;

    ptr = sdram_malloc(n * size);
    if (ptr)
    {
        memset(ptr, 0, n * size);
    }

    return ptr;
}

void *sdram_realloc(void *ptr, unsigned long size)
{
    return rt_memheap_realloc(sdram_heap, ptr, size);
}

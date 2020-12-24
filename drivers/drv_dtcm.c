#include <rtthread.h>
#include <board.h>
#include "drv_dtcm.h"
#include "sys_config.h"

#if(CFG_SOC_NAME == SOC_BK7221U)
/*dtcm memory management*/
static struct rt_memheap dtcm_heap;

void rt_dtcm_heap_init(void)
{
	rt_kprintf("rt_dtcm_heap_init.\n");
	rt_memheap_init(&dtcm_heap, "DTCM", RT_DTCM_ADDR_START, RT_DTCM_ADDR_END - RT_DTCM_ADDR_START);
}

void *dtcm_malloc(unsigned long size)
{
	return rt_memheap_alloc(&dtcm_heap, size);
}

void dtcm_free(void *ptr)
{
	rt_memheap_free(ptr);
}

void *dtcm_calloc(unsigned int n, unsigned int size)
{
	void *ptr = NULL;

	ptr = dtcm_malloc(n * size);
	if (ptr)
		memset(ptr, 0, n * size);

	return ptr;
}

void *dtcm_realloc(void *ptr, unsigned long size)
{
	return rt_memheap_realloc(&dtcm_heap, ptr,  size);
}
#else
void rt_dtcm_heap_init(void)
{
}

void *dtcm_malloc(unsigned long size)
{
	return rt_malloc(size);
}

void dtcm_free(void *ptr)
{
	rt_free(ptr);
}

void *dtcm_calloc(unsigned int n, unsigned int size)
{
	return rt_calloc(n, size);
}

void *dtcm_realloc(void *ptr, unsigned long size)
{
	return rt_realloc(ptr,  size);
}
#endif
// eof



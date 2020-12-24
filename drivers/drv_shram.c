#include <rtthread.h>
#include <board.h>
#include "drv_shram.h"

/*shared memory management*/
static struct rt_memheap shared_heap;

void rt_shared_heap_init(void)
{
	rt_kprintf("rt_shared_heap_init.\n");
	rt_memheap_init(&shared_heap, "shared_heap", RT_SHARED_MEM_BEGIN,  RT_SHARED_MEM_END - RT_SHARED_MEM_BEGIN);
}

void *shared_malloc(unsigned long size)
{
	return rt_memheap_alloc(&shared_heap, size);
}

void shared_free(void *ptr)
{
	rt_memheap_free(ptr);
}

void *shared_calloc(unsigned int n, unsigned int size)
{
	void *ptr = NULL;

	ptr = shared_malloc(n * size);
	if (ptr)
		memset(ptr, 0, n * size);

	return ptr;
}

void *shared_realloc(void *ptr, unsigned long size)
{
	return rt_memheap_realloc(&shared_heap, ptr, size);
}

// eof



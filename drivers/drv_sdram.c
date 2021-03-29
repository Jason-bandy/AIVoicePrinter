#include <rtthread.h>
#include "board.h"
#include "sys_config.h"
#include "typedef.h"
#include "mem_pub.h"

#define SHRAM_WORK_AROUND       0
#if SHRAM_WORK_AROUND
/* work around for shared memory issue */
typedef struct memory_info {
	uint32_t address;
	uint32_t using;
} memory_info;

memory_info mem_bank_2K[] = {
	{0x04001000, 0},
	{0x04001800, 0},
	{0x04002000, 0},
	{0x04002800, 0},
	{0x04003000, 0},
	{0x04003800, 0},
	{0x04004000, 0},
	{0x04004800, 0},
	{0x04005000, 0},
	{0x04005800, 0},
	{0x04006000, 0},
	{0x04006800, 0},
};

memory_info mem_bank_4K[] = {
	{0x04007000, 0},
	{0x04008000, 0},
	{0x04009000, 0},
	{0x0400A000, 0},
	{0x0400B000, 0},
	{0x0400C000, 0},
	{0x0400D000, 0},
};

memory_info mem_bank_8K[] = {
	{0x04014000, 0},
	{0x04016000, 0},
	{0x04018000, 0},
	{0x0401A000, 0},
	{0x0401C000, 0},
	{0x0401E000, 0},
};

memory_info mem_bank_16K[] = {
	{0x04030000, 0},
	{0x04034000, 0},
	{0x04038000, 0},
	{0x0403C000, 0},
};

memory_info mem_bank_128K[] = {
	{0x04040000, 0},
};

struct rt_mutex shram_lock;
#define SHRAM_LOCK() //rt_mutex_take(&shram_lock, RT_WAITING_FOREVER)
#define SHRAM_UNLOCK() //rt_mutex_release(&shram_lock)

void *mem_bank_malloc(memory_info *mem_bank, int mem_count, int mem_size)
{
	int index;
	void *ptr = NULL;

	SHRAM_LOCK();
	for (index = 0; index < mem_count; index++) {
		if (mem_bank[index].using == 0) {
			mem_bank[index].using = 1;
			ptr = mem_bank[index].address;
			break;
		}
	}

	SHRAM_UNLOCK();
	rt_kprintf("%s:0x%x,size=%d\n", __FUNCTION__, ptr, mem_size);
	return ptr;
}

void mem_bank_free(void *ptr)
{
	int index;

	rt_kprintf("%s:0x%x\n", __FUNCTION__, ptr);
	SHRAM_LOCK();
	for (index = 0; index < sizeof(mem_bank_2K) / sizeof(mem_bank_2K[0]); index++) {
		if ((void *)mem_bank_2K[index].address == ptr) {
			mem_bank_2K[index].using = 0;

			SHRAM_UNLOCK();
			return;
		}
	}

	for (index = 0; index < sizeof(mem_bank_4K) / sizeof(mem_bank_4K[0]); index++) {
		if ((void *)mem_bank_4K[index].address == ptr) {
			mem_bank_4K[index].using = 0;

			SHRAM_UNLOCK();
			return;
		}
	}

	for (index = 0; index < sizeof(mem_bank_8K) / sizeof(mem_bank_8K[0]); index++) {
		if ((void *)mem_bank_8K[index].address == ptr) {
			mem_bank_8K[index].using = 0;

			SHRAM_UNLOCK();
			return;
		}
	}

	for (index = 0; index < sizeof(mem_bank_16K) / sizeof(mem_bank_16K[0]); index++) {
		if ((void *)mem_bank_16K[index].address == ptr) {
			mem_bank_16K[index].using = 0;

			SHRAM_UNLOCK();
			return;
		}
	}

	for (index = 0; index < sizeof(mem_bank_128K) / sizeof(mem_bank_128K[0]); index++) {
		if ((void *)mem_bank_128K[index].address == ptr) {
			mem_bank_128K[index].using = 0;

			SHRAM_UNLOCK();
			return;
		}
	}

	SHRAM_UNLOCK();
	return;
}


int mem_bank_size(void *ptr)
{
	int index;

	SHRAM_LOCK();
	for (index = 0; index < sizeof(mem_bank_2K) / sizeof(mem_bank_2K[0]); index++) {
		if ((void *)mem_bank_2K[index].address == ptr) {
			SHRAM_UNLOCK();
			return 2048;
		}
	}

	for (index = 0; index < sizeof(mem_bank_4K) / sizeof(mem_bank_4K[0]); index++) {
		if ((void *)mem_bank_4K[index].address == ptr) {
			SHRAM_UNLOCK();
			return 4096;
		}
	}

	for (index = 0; index < sizeof(mem_bank_8K) / sizeof(mem_bank_8K[0]); index++) {
		if ((void *)mem_bank_8K[index].address == ptr) {
			SHRAM_UNLOCK();
			return 8192;
		}
	}

	for (index = 0; index < sizeof(mem_bank_16K) / sizeof(mem_bank_16K[0]); index++) {
		if ((void *)mem_bank_16K[index].address == ptr) {
			SHRAM_UNLOCK();
			return 16384;
		}
	}

	for (index = 0; index < sizeof(mem_bank_128K) / sizeof(mem_bank_128K[0]); index++) {
		if ((void *)mem_bank_128K[index].address == ptr) {
			SHRAM_UNLOCK();
			return 128 * 1024;
		}
	}

	SHRAM_UNLOCK();
	return 0;
}

#else
struct rt_memheap shram_heap;
#endif

extern struct rt_memheap _heap;

void rt_shram_heap_init(void)
{
#if SHRAM_WORK_AROUND
	rt_mutex_init(&shram_lock, "shram", RT_IPC_FLAG_FIFO);
#else
	rt_memheap_init(&shram_heap,
					"shram",
					RT_HW_SHRAM_BEGIN,
					(rt_uint32_t)RT_HW_SHRAM_END - (rt_uint32_t)RT_HW_SHRAM_BEGIN);
	rt_kprintf("rt_shram_heap_init complete.\n");
#endif
}

void *shram_malloc(unsigned long size)
{
#if SHRAM_WORK_AROUND
	memory_info *mem_bank = NULL;
	int mem_count;
	if (size <= 1024)
		return sdram_malloc(size);
	else if (size <= 2048) {
		mem_bank = mem_bank_2K;
		mem_count = sizeof(mem_bank_2K) / sizeof(mem_bank_2K[0]);
	} else if (size <= 4096) {
		mem_bank = mem_bank_4K;
		mem_count = sizeof(mem_bank_4K) / sizeof(mem_bank_4K[0]);
	} else if (size <= 8192) {
		mem_bank = mem_bank_8K;
		mem_count = sizeof(mem_bank_8K) / sizeof(mem_bank_8K[0]);
	} else if (size <= 16384) {
		mem_bank = mem_bank_16K;
		mem_count = sizeof(mem_bank_16K) / sizeof(mem_bank_16K[0]);
	} else if (size < 96 * 1024) {
		rt_kprintf("%s:%d\n", __FUNCTION__, size);
		rt_hw_stack_print(NULL);
	} else {
		mem_bank = mem_bank_128K;
		mem_count = sizeof(mem_bank_128K) / sizeof(mem_bank_128K[0]);
	}

	if (NULL != mem_bank) {
		void *ptr = mem_bank_malloc(mem_bank, mem_count, size);
		if (NULL != ptr) return ptr;
	}
	return sdram_malloc(size);
#else
	return rt_memheap_alloc(&shram_heap, size);
#endif
}

void shram_free(void *ptr)
{
#if SHRAM_WORK_AROUND
	if (((void *)0x04001000 <= ptr) && (ptr <= (void *)0x0405E000)) {
		mem_bank_free(ptr);
		return;
	}
#endif
	rt_memheap_free(ptr);
}

void *shram_calloc(unsigned int n, unsigned int size)
{
	void *ptr = NULL;

	ptr = shram_malloc(n * size);
	if (ptr)
		os_memset(ptr, 0, n * size);

	return ptr;
}

void *shram_realloc(void *ptr, unsigned long size)
{
#if SHRAM_WORK_AROUND
	if (((void *)0x04001000 <= ptr) && (ptr <= (void *)0x0405E000)) {
		void *new_ptr = shram_malloc(size);
		int old_size = mem_bank_size(ptr);
		rt_kprintf("shram_realloc: bad policy\n");
		if (NULL == new_ptr)
			return NULL;
		memcpy(new_ptr, ptr, (old_size > size) ? size : old_size);
		mem_bank_free(ptr);
		return new_ptr;
	}
	return sdram_realloc(ptr, size);
#else
	return rt_memheap_realloc(&shram_heap, ptr, size);
#endif
}

void rt_sdram_heap_init(void)
{
	rt_kprintf("rt_sdram_heap_init TODO.\n");
}

void *sdram_malloc(unsigned long size)
{
	return rt_malloc(size);
}

void sdram_free(void *ptr)
{
	rt_free(ptr);
}

void *sdram_calloc(unsigned int n, unsigned int size)
{
	void *ptr = NULL;

	ptr = rt_malloc(n * size);
	if (ptr)
		os_memset(ptr, 0, n * size);

	return ptr;
}

void *sdram_realloc(void *ptr, unsigned long size)
{
	return rt_realloc(ptr, size);
}


// eof


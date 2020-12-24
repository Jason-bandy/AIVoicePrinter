#ifndef __DRV_SHRAM_H__
#define __DRV_SHRAM_H__

#define RT_SHARED_MEM_BEGIN   (void*)(0x00900000)
#define RT_SHARED_MEM_END     (void*)(0x00900000 + 256 * 1024)

void rt_shared_heap_init(void);

#endif // __DRV_SHRAM_H__



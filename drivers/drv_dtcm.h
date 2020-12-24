#ifndef __DRV_DTCM_H__
#define __DRV_DTCM_H__
#include "sys_config.h"

extern unsigned char _empty_ram;

/* _empty_ram, link script file*/
#define RT_DTCM_ADDR_START     (void*)&_empty_ram
#if (CFG_SOC_NAME == SOC_BK7231N)
#define RT_DTCM_ADDR_END       (void*)(0x00400000 + 192 * 1024)
#else
#define RT_DTCM_ADDR_END       (void*)(0x00400000 + 256 * 1024)
#endif

void rt_dtcm_heap_init(void);

#endif // __DRV_DTCM_H__



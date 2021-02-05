/* resamplesubs_port.c - port setting*/
// Altered version
#include "include.h"
#include "pcm_resampler_port.h"

#if CFG_SUPPORT_RTT
#include "rtthread.h"
#else
#include "mem_pub.h"
#endif

void *pcm_malloc(unsigned long size)
{
	#if CFG_SUPPORT_RTT
	return dtcm_malloc(size);
	#else
	return os_malloc(size);
	#endif
}

// eof


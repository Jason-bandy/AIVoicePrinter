/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _COMPILER_ARM_H_
#define _COMPILER_ARM_H_

/* Global mandatory definitions */
#define __RESTRICT__
#define __VOLATILE__ volatile

#ifdef __INLINE__
#undef __INLINE__
#endif

#define __INLINE__ static inline

#ifndef __PACKED_POST__
#define __PACKED_POST__  __attribute__((packed))
#endif

#define EMBEDDED_ASM_ENABLE 0

#endif

// EOF

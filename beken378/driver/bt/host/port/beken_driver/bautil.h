/* -------------------------------------------------------------------------- */
/* We have provided the software delivered to you for evaluation purposes     */
/* only. It is authorized for use by participants of the specified evaluation */
/* for up to 60 days, unless further use is officially granted by CAST upon   */
/* request.  If you are not an authorized user, please destroy this source    */
/* code file and notify CAST immediately that you inadvertently received an   */
/* unauthorized copy.                                                         */
/* -------------------------------------------------------------------------- */


/**
 * BA22 utility routines.
 * SPR access, enable & disable interrupts.
 */

#ifndef BAUTIL_H_
#define BAUTIL_H_

#include "spr_defs.h"
#include "compiler_arm.h"

//#undef __INLINE__
// #define __INLINE__ __inline 
 //   #define __INLINE__ inline
    //#define __INLINE__  __forceinline static

/* Peripheral register (memory) access macros */
#define REG8(addr) *((volatile unsigned char *)(addr))
#define REG16(addr) *((volatile unsigned short *)(addr))
#define REG32(addr) *((volatile unsigned long *)(addr))

/**
 * Returns the value of special purpose register
 * with specified number. See spr_defs.h for declarations
 * of common BA22 SPRs. Also see architecture manual.
 */
__INLINE__ unsigned int get_spr(unsigned int sprnum)
{
    unsigned int result = 0;
    #if EMBEDDED_ASM_ENABLE
    __asm(
        "b.mfspr %0,%1,0;"
        :"=r" (result)
        :"r" (sprnum)
        :
    );
    #endif
    return result;
}

__INLINE__ unsigned int get_lr(void)
{
    unsigned int val = 0;
    #if EMBEDDED_ASM_ENABLE
    __asm(
        "b.mov %0,r9"
        :"=r" (val)
        :
    );
    #endif
    return val;
}

/**
 * Sets the address of vic.
 */
__INLINE__ void set_vic_handler(unsigned int vicnum, void (*handler)(void))
{
#if EMBEDDED_ASM_ENABLE
    unsigned int value = (unsigned int)handler;    
    __asm volatile (
        "b.mtspr %0,%1,0;"
        :
        :"r" (vicnum), "r" (value)
        :"cc"
    );
#endif
}


/**
 * Sets the value of special purpose register.
 */
__INLINE__ void set_spr(unsigned int sprnum, unsigned int value)
{
    #if EMBEDDED_ASM_ENABLE
    __asm volatile (
        "b.mtspr %0,%1,0;"
        :
        :"r" (sprnum), "r" (value)
        :"cc"
    );
    #endif
}

/**
 * Enables (newValue != 0) or disables (newValue == 0)
 * external interrupts on CPU.
 * Returns previous setting (0 if it was disabled).
 */
__INLINE__ int cpu_set_interrupts_enabled(int newValue)
{
 #if EMBEDDED_ASM_ENABLE
    unsigned int sr = get_spr(SPR_SR);
    if (newValue) {
        set_spr(SPR_SR, sr | SPR_SR_IEE);
    } else {
        set_spr(SPR_SR, sr & ~SPR_SR_IEE);
    }
    return (sr & SPR_SR_IEE);
#else
    return 0;
#endif
}

/**
 * Enables (newValue != 0) or disables (newValue == 0)
 * tick timer interrupts on CPU.
 * Returns previous setting (0 if it was disabled).
 */
__INLINE__ int cpu_set_timer_enabled(int newValue)
{
 #if EMBEDDED_ASM_ENABLE
    unsigned int sr = get_spr(SPR_SR);
    if (newValue) {
        set_spr(SPR_SR, sr | SPR_SR_TEE);
    } else {
        set_spr(SPR_SR, sr & ~SPR_SR_TEE);
    }
    return (sr & SPR_SR_TEE);
#else
    return 0;
#endif
}

/**
 * Enables or disables specified interrupt line on PIC.
 * @param line
 *      interrupt line (bitmask of line to enable)
 * @param newvalue
 *      0: disable that interrupt line
 *      otherwise: enable interrupt line
 * @return
 *      state of interrupt line before this call (0: disabled)
 */
__INLINE__ int pic_enable_interrupt_line(unsigned int line, int newValue)
{
    //disable UART interrupts on PIC
    unsigned int picmr = get_spr(SPR_PICMR);
    if (newValue)
        set_spr(SPR_PICMR, picmr | line);
    else
        set_spr(SPR_PICMR, picmr & ~line);
    return (picmr & line);
}

#endif /* BAUTIL_H_ */

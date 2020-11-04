/*
 * File      : trap.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2013, RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 */

#include <rtthread.h>
#include <rthw.h>
#include "sys_config.h"
#include "start_type_pub.h"

#define INT_IRQ     0x00
#define INT_FIQ     0x01

extern struct rt_thread *rt_current_thread;
#ifdef RT_USING_FINSH
extern long list_thread(void);
extern void rt_hw_stack_print(rt_thread_t thread);
#endif

struct rt_hw_register
{
    rt_uint32_t r0;
    rt_uint32_t r1;
    rt_uint32_t r2;
    rt_uint32_t r3;
    rt_uint32_t r4;
    rt_uint32_t r5;
    rt_uint32_t r6;
    rt_uint32_t r7;
    rt_uint32_t r8;
    rt_uint32_t r9;
    rt_uint32_t r10;
    rt_uint32_t fp;
    rt_uint32_t ip;
    rt_uint32_t sp;
    rt_uint32_t lr;
    rt_uint32_t pc;
    rt_uint32_t spsr;
    rt_uint32_t cpsr;
};

/**
 * this function will show registers of CPU
 *
 * @param regs the registers point
 */

void rt_hw_show_register (struct rt_hw_register *regs)
{
    rt_kprintf("Current regs:\n");
    rt_kprintf("r00:0x%08x r01:0x%08x r02:0x%08x r03:0x%08x\n",
               regs->r0, regs->r1, regs->r2, regs->r3);
    rt_kprintf("r04:0x%08x r05:0x%08x r06:0x%08x r07:0x%08x\n",
               regs->r4, regs->r5, regs->r6, regs->r7);
    rt_kprintf("r08:0x%08x r09:0x%08x r10:0x%08x\n",
               regs->r8, regs->r9, regs->r10);
    rt_kprintf("fp :0x%08x ip :0x%08x\n",
               regs->fp, regs->ip);
    rt_kprintf("sp :0x%08x lr :0x%08x pc :0x%08x\n",
               regs->sp, regs->lr, regs->pc);
    rt_kprintf("SPSR:0x%08x\n", regs->spsr);
    rt_kprintf("CPSR:0x%08x\n", regs->cpsr);

    int i;
    unsigned int *reg1;

    rt_kprintf("\nseparate regs:\n");

    reg1 = (const unsigned int *)0x400024;
    rt_kprintf("SYS:cpsr r8-r14\n");
    for(i=0;i<0x20>>2;i++)
    {
        rt_kprintf("0x%08x\n",*(reg1 + i));
    }

    rt_kprintf("IRQ:cpsr spsr r8-r14\n");
    reg1 = (const unsigned int *)0x400044;
    for(i=0;i<0x24>>2;i++)
    {
        rt_kprintf("0x%08x\n",*(reg1 + i));
    }

    rt_kprintf("FIR:cpsr spsr r8-r14\n");
    reg1 = (const unsigned int *)0x400068;
    for(i=0;i<0x24>>2;i++)
    {
        rt_kprintf("0x%08x\n",*(reg1 + i));
    }

    rt_kprintf("ABT:cpsr spsr r8-r14\n");
    reg1 = (const unsigned int *)0x40008c;
    for(i=0;i<0x24>>2;i++)
    {
        rt_kprintf("0x%08x\n",*(reg1 + i));
    }

    rt_kprintf("UND:cpsr spsr r8-r14\n");
    reg1 = (const unsigned int *)0x4000b0;
    for(i=0;i<0x24>>2;i++)
    {
        rt_kprintf("0x%08x\n",*(reg1 + i));
    }

    rt_kprintf("SVC:cpsr spsr r8-r14\n");
    reg1 = (const unsigned int *)0x4000d4;
    for(i=0;i<0x24>>2;i++)
    {
        rt_kprintf("0x%08x\n",*(reg1 + i));
    }
    
    rt_kprintf("\r\n");

}

/**
 * When ARM7TDMI comes across an instruction which it cannot handle,
 * it takes the undefined instruction trap.
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
void rt_hw_trap_udef(struct rt_hw_register *regs)
{
#if (CFG_SOC_NAME == SOC_BK7231N)
    *((volatile uint32_t *)START_TYPE_ADDR) = (uint32_t)(CRASH_UNDEFINED_VALUE & 0xffff);
#else
    *((volatile uint32_t *)START_TYPE_ADDR) = (uint32_t)CRASH_UNDEFINED_VALUE;
#endif

    rt_kprintf("undefined instruction\n");
    rt_hw_show_register(regs);

    rt_kprintf("thread - %s stack:\n", rt_current_thread->name);

#ifdef RT_USING_FINSH
    list_thread();
    rt_hw_stack_print(rt_current_thread);
#endif
    rt_hw_cpu_shutdown();
}

/**
 * The software interrupt instruction (SWI) is used for entering
 * Supervisor mode, usually to request a particular supervisor
 * function.
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
void rt_hw_trap_swi(struct rt_hw_register *regs)
{
    rt_hw_show_register(regs);

    rt_kprintf("software interrupt\n");
    rt_hw_cpu_shutdown();
}

/**
 * An abort indicates that the current memory access cannot be completed,
 * which occurs during an instruction prefetch.
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
void rt_hw_trap_pabt(struct rt_hw_register *regs)
{
#if (CFG_SOC_NAME == SOC_BK7231N)
    *((volatile uint32_t *)START_TYPE_ADDR) = (uint32_t)(CRASH_PREFETCH_ABORT_VALUE & 0xffff);
#else
    *((volatile uint32_t *)START_TYPE_ADDR) = (uint32_t)CRASH_PREFETCH_ABORT_VALUE;
#endif

    rt_kprintf("prefetch abort\n");
    rt_hw_show_register(regs);

    rt_kprintf("thread - %.*s stack:\n", RT_NAME_MAX, rt_current_thread->name);

#ifdef RT_USING_FINSH
    list_thread();
    rt_hw_stack_print(rt_current_thread);
#endif
    rt_hw_cpu_shutdown();
}

/**
 * An abort indicates that the current memory access cannot be completed,
 * which occurs during a data access.
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
void rt_hw_trap_dabt(struct rt_hw_register *regs)
{
#if (CFG_SOC_NAME == SOC_BK7231N)
    *((volatile uint32_t *)START_TYPE_ADDR) = (uint32_t)(CRASH_DATA_ABORT_VALUE & 0xffff);
#else
    *((volatile uint32_t *)START_TYPE_ADDR) = (uint32_t)CRASH_DATA_ABORT_VALUE;
#endif

    rt_kprintf("data abort\n");
    rt_hw_show_register(regs);

    rt_kprintf("thread - %.*s stack:\n", RT_NAME_MAX, rt_current_thread->name);

#ifdef RT_USING_FINSH
    list_thread();
    rt_hw_stack_print(rt_current_thread);
#endif
    rt_hw_cpu_shutdown();
}

/**
 * Normally, system will never reach here
 *
 * @param regs system registers
 *
 * @note never invoke this function in application
 */
void rt_hw_trap_resv(struct rt_hw_register *regs)
{
#if (CFG_SOC_NAME == SOC_BK7231N)
    *((volatile uint32_t *)START_TYPE_ADDR) = (uint32_t)(CRASH_UNUSED_VALUE & 0xffff);
#else
    *((volatile uint32_t *)START_TYPE_ADDR) = (uint32_t)CRASH_UNUSED_VALUE;
#endif

    rt_kprintf("not used\n");
    rt_hw_show_register(regs);
    rt_hw_cpu_shutdown();
}

extern void rt_interrupt_dispatch(void);

void rt_hw_trap_irq(void)
{   
    rt_interrupt_dispatch();
}

void rt_hw_trap_fiq(void)
{
    rt_interrupt_dispatch();
}

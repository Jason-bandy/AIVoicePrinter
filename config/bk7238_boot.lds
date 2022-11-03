/* 
 * Script for GNU linker.
 * Describes layout of sections, location of stack.
 *
 * In this case vectors are at location 0 (reset @ 0x08)
 *
 * +------------+  0x00400020
 * data         |
 * end
 * |(heap)      |
 * .            .
 * .            .
 * |(heap limit)|
 *
 * |- - - - - - |
 * stack bottom    256k
 * +------------+  
 *
 * +------------+  0x0000000
 * |vectors     |
 * |            |
 * |------------+
 * |text        |  
 * |data        |
 * |            |  1024k
 * +------------+
 */


/* Split memory into area for vectors and ram */
MEMORY
{
	flash  (rx) : ORIGIN = 0x00010000, LENGTH = 1M-68K
	itcm   (rwx): ORIGIN = 0x003F0000, LENGTH = 16k
	ram    (rw!x): ORIGIN = 0x00400100, LENGTH = 256k - 0x100
}

OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_vector_start);
_vector_start = ORIGIN(flash);

SECTIONS
{
/* vectors go to vectors region */
	. = ORIGIN(flash);
	.vectors : 
	{ 
		KEEP(*(*.vectors))
		KEEP( *(*.rom1))
	} > flash

/* instructions go to the text region*/

    . = ORIGIN(itcm);
    .itcm.code ALIGN(8) :
    {
		/* itcm 4KB code */
		*(.text.intc_hdl_entry)
		*(.text.intc_irq)
		*(.text.intc_fiq)
		*(.text.bk_timer_isr)
		*(.text.power_save_wakeup_isr)
		*(.text.bmsg_rx_sender)
		*(.text.bmsg_null_sender)
		*(.text.fclk_get_tick)
		*(.text.flash_read_sr)
		*(.text.flash_write_sr)
		*(.text.flash_clr_qwfr)
		*(.text.set_flash_protect)
		*(.text.flash_read)
		*(.text.flash_read_data)
		*(.text.flash_set_qe)
		*(.text.flash_set_qwfr)
		*(.text.flash_set_line_mode*)
		*(.text.flash_get_line_mode)
		*(.text.flash_write)
		*(.text.flash_ctrl)
		*(.text.power_save_dtim_wake)
		*(.text.sctrl_fix_dpll_div)
		 *(.text.enter_sleep)
		*(.text.rt_irq_dispatch)
		*(.text.rt_fiq_dispatch)
		*(.text.fclk_hdl)
		*(.text.rt_tick_increase)
		*(.text.rt_thread_yield)
		*(.text.rt_thread_suspend)
		*(.text.platform_is_in_irq_context)
		*(.text.platform_is_in_fiq_context)
		*(.text.platform_is_in_interrupt_context)
		*(.text.portENABLE_IRQ)
		*(.text.portENABLE_FIQ)
		*(.text.portDISABLE_FIQ)
		*(.text.portDISABLE_IRQ)
		*(.text.rt_schedule)
		*(.text.rt_thread_sleep)
		*(.text.rt_exit_critical)
		*(.text.rt_enter_critical)
		*(.text.sctrl_mcu_wakeup)
		*(.text.sctrl_hw_wakeup)
		*(.text.sctrl_cali_dpll)
		*(.text.rt_hw_interrupt_disable)
		*(.text.rt_hw_interrupt_enable)
		*(.text.rt_thread_self)
		*(.text.rt_schedule_remove_thread)
		*(.text.rt_timer_stop)
		*(.text.rt_thread_resume)
		*port_asm.o(.text .text.*)
		*context_gcc.o(.text .text.*)
    } > itcm AT>flash
    _itcmcode_flash_begin = LOADADDR(.itcm.code);
    _itcmcode_ram_begin = ADDR(.itcm.code);
    _itcmcode_ram_end = _itcmcode_ram_begin + SIZEOF(.itcm.code);

    . = ALIGN(0x8);
    /* code, instructions.for example: i=i+1; */
    .text :
    {
        . = ALIGN(4);
        *(.text)
        *(.text.*)
        *(.stub)
        /* .gnu.warning sections are handled specially by elf32.em.  */
        *(.gnu.warning)
        *(.gnu.linkonce.t*)
        *(.glue_7t) *(.glue_7)

        KEEP(*(.fini))

        /* section information for finsh shell */
        . = ALIGN(4);
        __fsymtab_start = .;
        KEEP(*(FSymTab))
        __fsymtab_end = .;
        . = ALIGN(4);
        __vsymtab_start = .;
        KEEP(*(VSymTab))
        __vsymtab_end = .;
        . = ALIGN(4);

        /* section information for modules */
        . = ALIGN(4);
        __rtmsymtab_start = .;
        KEEP(*(RTMSymTab))
        __rtmsymtab_end = .;

        /* section information for initialization */
        . = ALIGN(4);
        __rt_init_start = .;
        KEEP(*(SORT(.rti_fn*)))
        __rt_init_end = .;
    } > flash

    /* read only data.for example: const int rom_data[3]={1,2,3}; */
    .rodata ALIGN(8) :
    {
        *(.rodata)
        *(.rodata.*)
        *(.gnu.linkonce.r*)
        *(.eh_frame)
    } > flash

    . = ALIGN(4);
    .ctors :
    {
        PROVIDE(__ctors_start__ = .);
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)
        PROVIDE(__ctors_end__ = .);
    }

    .ARM.extab : 
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    }

    /* The .ARM.exidx section is used for C++ exception handling. */
    /* .ARM.exidx is sorted, so has to go in its own output section.  */
    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx*)
        *(.gnu.linkonce.armexidx.*)
    } > flash
    __exidx_end = .;
    
    .dtors :
    {
        PROVIDE(__dtors_start__ = .);
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)
        PROVIDE(__dtors_end__ = .);
    } > flash

    . = ORIGIN(ram);
/* globals.for example: int ram_data[3]={4,5,6}; */		/* VMA in RAM, but keep LMA in flash */
    _begin_data = .;
    .data :
    {
        *(.data .data.*)
        *(.sdata)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))
        . = ALIGN(4);
        /* All data end */

        *(.gnu.linkonce.d*)
        SORT(CONSTRUCTORS)
        } >ram AT>flash
        _end_data = .;

    /* Loader will copy data from _flash_begin to _ram_begin..ram_end */
    _data_flash_begin = LOADADDR(.data);
    _data_ram_begin = ADDR(.data);
    _data_ram_end = .;

    /* uninitialized data section - global   int i; */
    .bss ALIGN(8):
    {
        _bss_start = .;
        *boot_handlers.o(.bss .bss.* .scommon .sbss .dynbss COMMON)
        *(.bss .bss.*)
        *(.scommon)
        *(.sbss)
        *(.dynbss)
        *(COMMON)
        /* Align here to ensure that the .bss section occupies space up to
           _end.  Align after .bss to ensure correct alignment even if the
           .bss section disappears because there are no input sections.  */
        . = ALIGN(32 / 8);
        _bss_end = .;
    } > ram

    . = ALIGN (8);
    _empty_ram = .;

    /* This symbol defines end of code/data sections. Heap starts here. */
    PROVIDE(end = .);
}
 GROUP(
   libgcc.a
   libg.a
   libc.a
   libm.a
   libnosys.a
 )
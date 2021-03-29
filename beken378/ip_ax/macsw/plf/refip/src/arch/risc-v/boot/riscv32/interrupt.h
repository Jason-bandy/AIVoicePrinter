/* Define an interrupt handler to deal with interrupt number x.
   This should be preceded by void and followed by the body of the
   handler between { }
   This interrupt handler will save and restore the registers which
   are used in the interrupt handler body. */
#define interrupt_handler(x) \
    _concat (interrupt,x,_handler (void) __attribute__((interrupt));)\
    _concat (void interrupt,x,_handler (void))

/* Define an interrupt handler to deal with interrupt number x.
   This should be preceded by void and followed by the body of the
   handler between { }
   This interrupt handler will not save and restore any registers
   at all.
   This is used with certain RTOS'es such as FreeRTOS */
#define naked_interrupt_handler(x) \
    _concat (interrupt,x,_handler (void) __attribute__((naked));)\
    _concat (void interrupt,x,_handler (void))

#define _concat(x,y,z) x ## y ## z

/// Debug trap interrupt index
/// WiFi interrupt index
#define IC_WIFI_IRQ        0
#ifdef CFG_RTOS
/// Tick interrupt index
#define IC_TICK_IRQ        1
/// RTOS yield interrupt index
#define IC_YIELD_IRQ       2
#endif

;/**
; ****************************************************************************************
; *
; * @file boot_vectors.s
; *
; * @brief ARM Exception Vectors table.
; *
; * Copyright (C) RivieraWaves 2011-2019
; *
; ****************************************************************************************
; */
    ;Import pointers to the actual vector handlers.
    IMPORT   boot_reset
    IMPORT   boot_undefined
    IMPORT   boot_swi
    IMPORT   boot_pabort
    IMPORT   boot_dabort
    IMPORT   boot_reserved
    IMPORT   intc_irq
    IMPORT   intc_fiq

    AREA  ||.boot_vectors||, CODE, READONLY

    ;
    ; This is the entry point for the system and this vector table must be
    ; physically located or mapped to address 0.
    ;
    ENTRY
    EXPORT  boot_vectors
boot_vectors
    B boot_reset
    B boot_undefined
    B boot_swi
    B boot_pabort
    B boot_dabort
    B boot_reserved

    IF {CPU} == "ARM926EJ-S"
    ; described in the documentation of the AIC in AT91SAM9261:
    ;  - IRQ
    LDR PC, [PC,# -&F20]
    ;  - FIQ
    LDR PC, [PC,# -&F20]
    ELSE
    B intc_irq
    B intc_fiq
    ENDIF

    EXPORT boot_breakpoint
boot_breakpoint
    ; If set to 0 by host before getting out of reset, the cpu
    ; will loop and light the leds indefinitely
    DCD 1

    END

;/**
; ****************************************************************************************
; *
; * @file boot_handlers.s
; *
; * @brief ARM Exception Vector handler functions.
; *
; * Copyright (C) RivieraWaves 2011-2019
; *
; ****************************************************************************************
; */

    ;Export pointers to the vector handlers.
    EXPORT   boot_reset
    EXPORT   boot_undefined
    EXPORT   boot_swi
    EXPORT   boot_pabort
    EXPORT   boot_dabort
    EXPORT   boot_reserved

    IMPORT   wifi_main

;/* ========================================================================
; *                                Constants
; * ======================================================================== */

BOOT_MODE_MASK EQU 0x1F

BOOT_MODE_USR  EQU 0x10
BOOT_MODE_FIQ  EQU 0x11
BOOT_MODE_IRQ  EQU 0x12
BOOT_MODE_SVC  EQU 0x13
BOOT_MODE_ABT  EQU 0x17
BOOT_MODE_UND  EQU 0x1B
BOOT_MODE_SYS  EQU 0x1F

BOOT_COLOR_UNUSED  EQU 0xAAAAAAAA      ; Pattern to fill UNUSED stack
BOOT_COLOR_SVC     EQU 0xBBBBBBBB      ; Pattern to fill SVC stack
BOOT_COLOR_IRQ     EQU 0xCCCCCCCC      ; Pattern to fill IRQ stack
BOOT_COLOR_FIQ     EQU 0xDDDDDDDD      ; Pattern to fill FIQ stack

;/* ========================================================================
; *                                Macros
; * ======================================================================== */

;/* ========================================================================
;/**
; * Macro for switching ARM mode
; */
    MACRO
$x  BOOT_CHANGE_MODE $newMode
        MRS   R0, CPSR
        BIC   R0, R0, #BOOT_MODE_MASK
        ORR   R0, R0, #BOOT_MODE_$newMode
        MSR   CPSR_c, R0
    MEND


;/* ========================================================================
;/**
; * Macro for setting the stack
; */
    MACRO
$x  BOOT_SET_STACK $stackName
        LDR   R0, boot_stack_base_$stackName
        LDR   R2, boot_stack_len_$stackName
        ADD   R1, R0, R2
        MOV   SP, R1        ; Set stack pointer

        LDR   R2, =BOOT_COLOR_$stackName

90      CMP   R0, R1        ; End of stack?
        STRLT R2, [r0]      ; Colorize stack word
        ADDLT R0, R0, #4
        BLT   %B90          ; branch to previous local label

    MEND


   PRESERVE8
   AREA  ||.text||, CODE, READONLY
;/* ========================================================================
; *                                Globals
; * ======================================================================== */

;/* ========================================================================
;/**
; * CP15 DTCM Control Reg settings
; */
boot_Cp15DtcmReg
    DCD 0x01010001


;/* ========================================================================
;/**
; * CP15 ITCM Control Reg settings
; */
boot_Cp15ItcmReg
    DCD 0x01000001

;/* ========================================================================
;/**
; * RAM_BSS
; */
    IMPORT   |Image$$RAM_BSS$$ZI$$Base|
ram_bss_base
    DCD |Image$$RAM_BSS$$ZI$$Base|

    IMPORT   |Image$$RAM_BSS$$ZI$$Length|
ram_bss_length
    DCD |Image$$RAM_BSS$$ZI$$Length|

;/* ========================================================================
;/**
; * Unused (ABT, UNDEFINED, SYSUSR) Mode
; */
    IMPORT   |Image$$RAM_STACK_UNUSED$$Base|
boot_stack_base_UNUSED
    DCD |Image$$RAM_STACK_UNUSED$$Base|

    IMPORT   |Image$$RAM_STACK_UNUSED$$ZI$$Length|
boot_stack_len_UNUSED
    DCD |Image$$RAM_STACK_UNUSED$$ZI$$Length|


;/* ========================================================================
;/**
; * IRQ Mode
; */
    IMPORT   |Image$$RAM_STACK_IRQ$$Base|
boot_stack_base_IRQ
    DCD |Image$$RAM_STACK_IRQ$$Base|

    IMPORT   |Image$$RAM_STACK_IRQ$$ZI$$Length|
boot_stack_len_IRQ
    DCD |Image$$RAM_STACK_IRQ$$ZI$$Length|


;/* ========================================================================
;/**
; * Supervisor Mode
; */
    IMPORT   |Image$$RAM_STACK_SVC$$Base|
boot_stack_base_SVC
    DCD |Image$$RAM_STACK_SVC$$Base|

    IMPORT   |Image$$RAM_STACK_SVC$$ZI$$Length|
boot_stack_len_SVC
    DCD |Image$$RAM_STACK_SVC$$ZI$$Length|


;/* ========================================================================
;/**
; * FIQ Mode
; */
    IMPORT   |Image$$RAM_STACK_FIQ$$Base|
boot_stack_base_FIQ
    DCD |Image$$RAM_STACK_FIQ$$Base|

    IMPORT   |Image$$RAM_STACK_FIQ$$ZI$$Length|
boot_stack_len_FIQ
    DCD |Image$$RAM_STACK_FIQ$$ZI$$Length|


;/* ========================================================================
; *                                Functions
; * ======================================================================== */

;/* ========================================================================
;/**
; * Function to handle reset vector
; */
boot_reset

    ; Disable IRQ and FIQ before starting anything
    MRS   R0, CPSR
    ORR   R0, R0, #0xC0
    MSR   CPSR_c, R0

    ; Skip MMU/cache inits on arm7
    IF {CPU} == "ARM926EJ-S"
    ; * ==================
    ; Initialize ARM CP 15 registers
    ; Control Register
    ;        [18] : SBO (Should Be One)
    ;        [17] : SBZ (Should Be Zero)
    ;        [16] : SBO (Should Be One)
    ;  L4    [15] : Loads to PC also sets T bit (0=Enabled, 1=Disabled)
    ;  RR    [14] : Cache replacement algo (0=random, 1=round-robin)
    ;   V    [13] : Exception Vectors location (0=0x00000000, 1=0xFFFF0000)
    ;   I    [12] : Instruction Cache (1=Enabled, 0=Disabled)
    ; SBZ [11-10] : SBZ
    ;   R     [9] : Rom protection (used in Domain access control)
    ;   S     [8] : System Protection (used in Domain access control)
    ;   B     [7] : Endianness (1=Big, 0=Little)
    ; SBO   [6-3] : SBO
    ;   C     [2] : Data Cache (1=Enabled, 0=Disabled)
    ;   A     [1] : Alignment faults (1=Enabled, 0=Disabled)
    ;   M     [0] : MMU (1=Enabled, 0=Disabled)
    ; at init reset value is 0x0005.0078.
    ; configure our default behaviour: R=1, A=1, others=0
    LDR   R0, =0x0005027A
    MCR   p15, 0, R0, c1, c0, 0

    ; Flush Caches and TLB.
    MOV   R0, #0
    MCR   p15, 0, R0, c7, c7, 0 ; Caches
    MCR   p15, 0, R0, c8, c7, 0 ; TLB

    ; Clear Fault Registers
    MOV   R0, #0
    MCR   p15, 0, R0, c5, c0, 0 ; Data Fault Status
    MCR   p15, 0, R0, c5, c0, 1 ; Instruction Fault Status
    MCR   p15, 0, R0, c6, c0, 0 ; Fault Address

    ; Configure FCSE for flat mapping
    MOV   R0, #0
    MCR   p15, 0, R0, c13, c0, 0

    ; Enable Instruction Cache. Note: Physical addresses
    ; are cached until MMU is enabled. Before enabling MMU
    ; the instruction cache should be flushed and disabled.
    ; After enabling the MMU the cache can be re-enabled.
    MRC   p15, 0, R0, c1, c0, 0
    ORR   R0, R0, #0x1000
    MCR   p15, 0, R0, c1, c0, 0

    ; Configure DTCM to be at address 0x01010000 and enable it
    LDR   R0, boot_Cp15DtcmReg
    MCR   p15, 0, R0, c9, c1, 0

    ; Configure ITCM to be at address 0x01000000 and enable it
    LDR   R0, boot_Cp15ItcmReg
    MCR   p15, 0, R0, c9, c1, 1

    ENDIF ; end of MMU/cache inits

    ; * ==================
    ; Setup all stacks

    ; Note: Sys and Usr mode are not used
    BOOT_CHANGE_MODE SYS
    BOOT_SET_STACK   UNUSED
    BOOT_CHANGE_MODE ABT
    BOOT_SET_STACK   UNUSED
    BOOT_CHANGE_MODE UND
    BOOT_SET_STACK   UNUSED
    BOOT_CHANGE_MODE IRQ
    BOOT_SET_STACK   IRQ
    BOOT_CHANGE_MODE FIQ
    BOOT_SET_STACK   FIQ

    ; Clear FIQ banked registers while in FIQ mode
    MOV     R8, #0
    MOV     R9, #0
    MOV     R10, #0
    MOV     R11, #0
    MOV     R12, #0

    BOOT_CHANGE_MODE SVC
    BOOT_SET_STACK   SVC

    ; Stay in Supervisor Mode

    ; Init the BSS section
    LDR     R0, ram_bss_base
    LDR     R1, ram_bss_length
    MOV     R2, #0
    MOV     R3, #0
    MOV     R4, #0
    MOV     R5, #0
init_bss_loop
    SUBS    R1, R1, #16
    STMCSIA R0!, {R2, R3, R4, R5}
    BHI     init_bss_loop
    LSLS    R1, R1, #29
    STMCSIA R0!, {R4, R5}
    STRMI   R3, [R0]

    ; * ==================
    ; Clear Registers
    MOV R0, #0
    MOV R1, #0
    MOV R2, #0
    MOV R3, #0
    MOV R4, #0
    MOV R5, #0
    MOV R6, #0
    MOV R7, #0
    MOV R8, #0
    MOV R9, #0
    MOV R10, #0
    MOV R11, #0
    MOV R12, #0

    BL wifi_main


    ; If For some reason main returns (which it shouldn't)
    ; Just loop here and wait for the watchdog to reset
_boot_reset_loop
    B _boot_reset_loop


;/* ========================================================================
;/**
; * Function to handle undefined vector
; */
boot_undefined

    B boot_undefined

;/* ========================================================================
;/**
; * Function to handle software interrupt vector
; */
boot_swi

    B boot_swi


;/* ========================================================================
;/**
; * Function to handle Prefetch Abort vector
; */
boot_pabort

    B boot_pabort

;/* ========================================================================
;/**
; * Function to handle Data Abort vector
; */
boot_dabort

    B boot_dabort

;/* ========================================================================
;/**
; * Function to handle Reserved vector
; */
boot_reserved

    B boot_reserved
    SUBS PC, LR, #4

    END

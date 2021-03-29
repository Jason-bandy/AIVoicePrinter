// Copyright 2017 ETH Zurich and University of Bologna.
// Copyright and related rights are licensed under the Solderpad Hardware
// License, Version 0.51 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://solderpad.org/licenses/SHL-0.51. Unless required by applicable law
// or agreed to in writing, software, hardware and materials distributed under
// this License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#define EXCEPTION_STACK_SIZE 96

.data
  STACK_INIT_PATTERN: .word   0xF3F3F3F3

/* ========================================================= [ entry ] === */
.section .text
.option rvc;

reset_handler:
  /* set 0 in mtvec (base for IVT) */
  csrrw x0, mtvec, x0

  /* set all registers to zero */
  mv  x1, x0
  mv  x2, x1
  mv  x3, x1
  mv  x4, x1
  mv  x5, x1
  mv  x6, x1
  mv  x7, x1
  mv  x8, x1
  mv  x9, x1
  mv x10, x1
  mv x11, x1
  mv x12, x1
  mv x13, x1
  mv x14, x1
  mv x15, x1
  mv x16, x1
  mv x17, x1
  mv x18, x1
  mv x19, x1
  mv x20, x1
  mv x21, x1
  mv x22, x1
  mv x23, x1
  mv x24, x1
  mv x25, x1
  mv x26, x1
  mv x27, x1
  mv x28, x1
  mv x29, x1
  mv x30, x1
  mv x31, x1

  /* stack initilization */
  la   x2, __stack_top

_start:
  .global _start

  /* clear BSS */
  la x26, _ssbss
  la x27, _esbss

  bge x26, x27, zero_loop_end

zero_loop:
  sw x0, 0(x26)
  addi x26, x26, 4
  blt x26, x27, zero_loop
zero_loop_end:


  /* clear BSS */
  la x26, _sbss
  la x27, _ebss

  bge x26, x27, zero_loop_end2

zero_loop2:
  sw x0, 0(x26)
  addi x26, x26, 4
  blt x26, x27, zero_loop2
zero_loop_end2:

  /* clear heap */
  la x26, __heap_bottom
  la x27, __heap_top

  bge x26, x27, zero_heap_loop_end

zero_heap_loop:
  sw x0, 0(x26)
  addi x26, x26, 4
  blt x26, x27, zero_heap_loop
zero_heap_loop_end:

  /* initialize stack */
  lw x25, STACK_INIT_PATTERN
  la x26, __stack_bottom
  la x27, __stack_top

  bge x26, x27, stack_init_end

stack_init:
  sw x25, 0(x26)
  addi x26, x26, 4
  blt x26, x27, stack_init
stack_init_end:

main_entry:
  /* jump to main program entry point (argc = argv = 0) */
  addi x10, x0, 0
  addi x11, x0, 0
  jal x1, main

  .global _init
  .global _fini
_init:
_fini:
  # These don't have to do anything since we use init_array/fini_array.
  ret

/* =================================================== [ exceptions ] === */
  .section .vectors, "ax"
  .option norvc;

  // external interrupts are handled by the same callback
  // until compiler supports IRQ routines
  .org 0x00
  jal x0, interrupt0_handler
  jal x0, interrupt1_handler
  jal x0, interrupt2_handler
  jal x0, interrupt3_handler
  jal x0, interrupt4_handler
  jal x0, interrupt5_handler
  jal x0, interrupt6_handler
  jal x0, interrupt7_handler
  jal x0, interrupt8_handler
  jal x0, interrupt9_handler
  jal x0, interrupt10_handler
  jal x0, interrupt11_handler
  jal x0, interrupt12_handler
  jal x0, interrupt13_handler
  jal x0, interrupt14_handler
  jal x0, interrupt15_handler
  jal x0, interrupt16_handler
  jal x0, interrupt17_handler
  jal x0, interrupt18_handler
  jal x0, interrupt19_handler
  jal x0, interrupt20_handler
  jal x0, interrupt21_handler
  jal x0, interrupt22_handler
  jal x0, interrupt23_handler
  jal x0, interrupt24_handler
  jal x0, interrupt25_handler
  jal x0, interrupt26_handler
  jal x0, interrupt27_handler
  jal x0, interrupt28_handler
  jal x0, interrupt29_handler
  jal x0, interrupt30_handler
  jal x0, interrupt31_handler

  // reset vector
  .org 0x80
  jal x0, reset_handler

  // illegal instruction exception
  .org 0x84
  jal x0, illegal_insn_handler

  // ecall instruction handler
  .org 0x88
  jal x0, ecall_insn_handler

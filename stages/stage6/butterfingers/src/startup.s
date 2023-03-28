@ constants that define various operating modes
.equ  PSR_MASK,    0x0000001F       @ CSPR bits that define operating mode

.equ  MODE_USR,    0x00000010       @ "User" Mode
.equ  MODE_FIQ,    0x00000011       @ "FIQ" Mode
.equ  MODE_IRQ,    0x00000012       @ "IRQ" Mode
.equ  MODE_SVC,    0x00000013       @ "Supervisor" Mode
.equ  MODE_ABT,    0x00000017       @ "Abort" Mode
.equ  MODE_UND,    0x0000001B       @ "Undefined" Mode
.equ  MODE_SYS,    0x0000001F       @ "System" Mode

.equ FIQ_BIT,      0x00000040       @ FIQ exception enable/disable bit
.equ IRQ_BIT,      0x00000080       @ IRQ exception enable/disable bit

.equ VECTORS_TABLE_ADDR, 0x00000000

.section .init
.code 32

.global vectors_start
vectors_start:
    LDR PC, reset_handler_addr
    LDR PC, undef_handler_addr
    LDR PC, swi_handler_addr
    LDR PC, prefetch_abort_handler_addr
    LDR PC, data_abort_handler_addr
    B .
    LDR PC, irq_handler_addr
    LDR PC, fiq_handler_addr

reset_handler_addr:
    .word reset_handler
undef_handler_addr:
    .word unhandled
swi_handler_addr:
    .word unhandled
prefetch_abort_handler_addr:
    .word unhandled
data_abort_handler_addr:
    .word unhandled
irq_handler_addr:
    .word irq_handler
fiq_handler_addr:
    .word unhandled

.global vectors_end
vectors_end:

.section .text
.code 32

reset_handler:
    @ set "Supervisor" Mode stack
    LDR sp, =stack_top

    @ clear the whole BSS section to zeros
    LDR r0, __bss_begin_addr
    LDR r1, __bss_end_addr
    MOV r2, #0
bss_clear_loop:
    CMP r0, r1
    STRLTB r2, [r0], #1
    BLT bss_clear_loop

    @ get "Program Status Register" (CPSR)
    MRS r0, cpsr

    @ disable IRQ and FIQ interrupts for the "Supervisor" Mode
    ORR r1, r0, #IRQ_BIT|FIQ_BIT
    MSR cpsr, r1

    @ switch into "IRQ" Mode
    BIC r1, r0, #PSR_MASK
    ORR r1, r1, #MODE_IRQ
    MSR cpsr, r1

    @ disable IRQ and FIQ interrupts for the "IRQ" Mode
    ORR r1, r1, #IRQ_BIT|FIQ_BIT
    MSR cpsr, r1

    @ set "IRQ" Mode stack
    LDR sp, =irq_stack_top

    @ switch back into "Supervisor" mode
    BIC r1, r0, #PSR_MASK
    ORR r1, r1, #MODE_SVC
    MSR cpsr, r1

    @ jump to init
    BL init

    @ jump to main
    B main

unhandled:
    B .

__bss_begin_addr:
    .word __bss_begin
__bss_end_addr:
    .word __bss_end

.end

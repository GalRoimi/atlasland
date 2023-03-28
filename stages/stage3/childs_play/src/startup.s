.org 0

.section .init
.code 32

.global entry_point
entry_point:
    LDR sp, =stack_top
    BL main
    B .

.section .secret
.word 0xaabbccdd

.end

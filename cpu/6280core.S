;===== register aliasing =====
; scratchpad
#define scratch0    r0
#define scratch1    r1

; cpu registers
#define flags   r2
#define PC      r3
#define A       r4
#define X       r5
#define Y       r6
#define S       r7

#define return  r14

; cpu timer

; (10 bits) continuously counts down from and then resets to 1024
#define cycle_counter   r8

; (1 bit + 7 bits) 1 bit enable/disable timer at 0x0C01 ; 7 bit counter at 0x0C00 ;
#define timer_counter   r9

; (7 bits) memory copied when timer is disabled (0x0C01 = 0)
#define timer_latch     r10


;===== bit flag constants =====
.set timer_enabled, 0x80

.set IQIRQ1_bit  , 0x0100
.set IQIRQ1_mask , 0x0200

.set IQIRQ2_bit  , 0x0400
.set IQIRQ2_mask , 0x0800

.set IQTIMER_bit , 0x1000
.set IQTIMER_mask, 0x2000

.set flag_N, 0x80 ; negative
.set flag_V, 0x40 ; overflow
.set flag_T, 0x20 ; block transfer in progress
.set flag_B, 0x10 ; break
.set flag_D, 0x08 ; decimal mode
.set flag_I, 0x04 ; interupt ignore
.set flag_Z, 0x02 ; zero result
.set flag_C, 0x01 ; carry

;===== helper macros =====

.macro jump reg
    jr \reg
.endm

.macro goto address
    mov scratch0, \address
    jump scratch0
.endm

.macro call address
    mov scratch0, \address
    jalr scratch0
.endm

.macro if_true reg
    beq 2
    jump \reg
.endm

.macro if_false reg
    bne 2
    jump \reg
.endm

.macro if_true_imm address
    beq \address
.endm

.macro if_false_imm address
    bne \address
.endm

.macro test_equality reg0, reg1
    sub scratch0, \reg0, \reg1
.endm

.macro set_flag  flag_id
    mov scratch0, \flag_id
    orr flags, flags, scratch0
.endm

.macro clear_flag  flag_id
    set_flag \flag_id
    eor flags, flags, scratch0
.endm

.macro set_bit  reg, bitnum
    mov scratch0, 1
    lsl scratch0, scratch0, \bitnum
    orr \reg, \reg, scratch0
.endm

;===== timer =====
.global _start
.text

reset_timer:
    mov cycle_counter, 0x400
    mov timer_counter, 0x80
    mov timer_latch  , 0x80
    rts


decrement_timer:
    sub cycle_counter, cycle_counter, 1
    if_false return
    mov cycle_counter, 0x400
    sub timer_counter, timer_counter, 1
    if_false return
    mov timer_counter, 0x80
    rts

.macro test_timer_zero
    mov scratch0, 0x400
    test_equality cycle_counter, scratch0
    if_false_imm 4 ; skip next test
    test_equality timer_counter, 0x80
.endm



_start:
    mov X, #0
    call _reset_timer
loop:
    test_timer_zero
    if_false_imm 4 ; if false, skip increment
      add X, X, 1 ; increment
    call decrement_timer
    goto loop
.end ; end of text section

#include "6280dynarec.h"
#if 0
#include <stdint.h>

using namespace HuC6280;



enum : uint8_t   // status flags for status register (P)
{
  flag_N = 0x80, // negative
  flag_V = 0x40, // overflow
  flag_T = 0x20, // block transfer in progress
  flag_B = 0x10, // break
  flag_D = 0x08, // decimal mode
  flag_I = 0x04, // interupt ignore
  flag_Z = 0x02, // zero result
  flag_C = 0x01, // carry
};

enum register_e  : uint16_t
{
/*
  // epiphany registers
  R0=0, R1,  R2,  R3,  R4,  R5,  R6,  R7,  R8,  R9,
  R10, R11, R12, R13, R14, R15, R16, R17, R18, R19,
  R20, R21, R22, R23, R24, R25, R26, R27, R28, R29,
  R30, R31, R32, R33, R34, R35, R36, R37, R38, R39,
  R40, R41, R42, R43, R44, R45, R46, R47, R48, R49,
  R50, R51, R52, R53, R54, R55, R56, R57, R58, R59,
  R60, R61, R62, R63,

  // argument/result registers
  A1=0,
  A2,
  A3,
  A4,

  // variable registers
  V1, V2, V3, V4, V5, V6, V7, V8,

  SB = R9, // static base
  SL, // stack limit
  FP, // frame pointer
  SP = R13, // stack pointer
  LR, // link register
*/
  // PC Engine registers and flags

  // two scratch registers to play with. :)
  V0 = 0,
  V1,

  PC, // Program Counter

  A, // Accumulator
  X, // X Index register
  Y, // Y Index register
  S, // Stack Pointer
  P, // Processor Flags
/*
  // Processor Flags
  C, // carry
  Z, // zero result
  I, // ignore interrupts
  D, // decimal mode
  B, // break
  T, // block transfer in progress
  V, // overflow
  N, // negative
*/
};

enum op_id_e : uint16_t
{
  branch      = 0,
  move        = 1,
  integer     = 5,
};

enum condition_e : uint8_t
{
  equal         = 0x00, // branch equal
  not_equal     = 0x01, // branch not equal
  greater_than  = 0x03, // branch greater than (unsigned)
  less_than     = 0x05, // branch less than (unsigned)
  unconditional = 0x0E, // unconditional branch
};

enum op_sub_id_e : uint16_t
{
  add = 1,
  sub = 3,

};

struct integer_op_t
{
  uint8_t     op_id        : 7;
  register_e  destination  : 3;
  register_e  source1      : 3;
  register_e  source2      : 3;
};


enum op_type_e : uint16_t
{
  move_op = 0x03,

  lsr  = 0x06,
  lsl  = 0x07,
  asr  = 0x0E,
  bitr = 0x1E,

};

constexpr uint16_t shift(op_type_e shift_type,
                         uint8_t immediate,
                         register_e source,
                         register_e destination)
{
//  return
}



constexpr uint16_t move(uint8_t immediate,
                        register_e destination)
{
  return move_op
      | (immediate   << 5)
      | (destination << 8);
}

constexpr uint16_t branch(condition_e condition,
                          uint8_t immediate)
{
  return (condition << 4)
       | (immediate << 8);
}



DynarecCore::DynarecCore(void)
{
}


void DynarecCore::ExecutionLoop(void)
{
switch(op)
{
  case 0x00: /* brk */ brk()                          ; break;
  case 0x01: /* ora */ LogicOp<OR>(IndirectX())       ; break;
  case 0x02: /* sxy */ swap(m_regs.X, m_regs.Y)       ; break;
  case 0x03: /* st0 */ st0()                          ; break;
  case 0x04: /* tsb */ tsb(ZeroPage())                ; break;
  case 0x05: /* ora */ LogicOp<OR>(ZeroPage())        ; break;
  case 0x06: /* asl */ lshift(ZeroPage())             ; break;
  case 0x07: /* rmb */ rmb<0x01>()                    ; break;
  case 0x08: /* php */ m_regs.flags.T = 0;
                       push(m_regs.P | flag_B)        ; break;
  case 0x09: /* ora */ ImmediateLogicOp<OR>()         ; break;
  case 0x0a: /* asl */ asl_a()                        ; break;
  case 0x0b: /* BAD */ bad_opcode<0x0B>()             ; break;
  case 0x0c: /* tsb */ tsb(Absolute())                ; break;
  case 0x0d: /* ora */ LogicOp<OR>(Absolute())        ; break;
  case 0x0e: /* asl */ lshift(Absolute())             ; break;
  case 0x0f: /* bbr */ bbr<0x01>()                    ; break;
  case 0x10: /* bpl */ BranchIfNotSet<flag_N>()       ; break;
  case 0x11: /* ora */ LogicOp<OR>(IndirectY())       ; break;
  case 0x12: /* ora */ LogicOp<OR>(Indirect())        ; break;
  case 0x13: /* st1 */ st1()                          ; break;
  case 0x14: /* trb */ trb(ZeroPage())                ; break;
  case 0x15: /* ora */ LogicOp<OR>(ZeroPageX())       ; break;
  case 0x16: /* asl */ lshift(ZeroPageX())            ; break;
  case 0x17: /* rmb */ rmb<0x02>()                    ; break;
  case 0x18: /* clc */ clear_flag<flag_C>()           ; break;
  case 0x19: /* ora */ LogicOp<OR>(AbsoluteY())       ; break;
  case 0x1a: /* inc */ increment(m_regs.A)            ; break;
  case 0x1b: /* BAD */ bad_opcode<0x1B>()             ; break;
  case 0x1c: /* trb */ trb(Absolute())                ; break;
  case 0x1d: /* ora */ LogicOp<OR>(AbsoluteX())       ; break;
  case 0x1e: /* asl */ lshift(AbsoluteX())            ; break;
  case 0x1f: /* bbr */ bbr<0x02>()                    ; break;
  case 0x20: /* jsr */ jsr()                          ; break;
  case 0x21: /* and */ LogicOp<AND>(IndirectX())      ; break;
  case 0x22: /* sax */ swap(m_regs.A, m_regs.X)       ; break;
  case 0x23: /* st2 */ st2()                          ; break;
  case 0x24: /* bit */ bit(ZeroPage())                ; break;
  case 0x25: /* and */ LogicOp<AND>(ZeroPage())       ; break;
  case 0x26: /* rol */ lrotate(ZeroPage())            ; break;
  case 0x27: /* rmb */ rmb<0x04>()                    ; break;
  case 0x28: /* plp */ plp()                          ; continue; // PuLl P register
  case 0x29: /* and */ ImmediateLogicOp<AND>()        ; break;
  case 0x2a: /* rol */ rol_a()                        ; break;
  case 0x2b: /* BAD */ bad_opcode<0x2B>()             ; break;
  case 0x2c: /* bit */ bit(Absolute())                ; break;
  case 0x2d: /* and */ LogicOp<AND>(Absolute())       ; break;
  case 0x2e: /* rol */ lrotate(Absolute())            ; break;
  case 0x2f: /* bbr */ bbr<0x04>()                    ; break;
  case 0x30: /* bmi */ BranchIfSet<flag_N>()          ; break;
  case 0x31: /* and */ LogicOp<AND>(IndirectY())      ; break;
  case 0x32: /* and */ LogicOp<AND>(Indirect())       ; break;
  case 0x33: /* BAD */ bad_opcode<0x3B>()             ; break;
  case 0x34: /* bit */ bit(ZeroPageX())               ; break;
  case 0x35: /* and */ LogicOp<AND>(ZeroPageX())      ; break;
  case 0x36: /* rol */ lrotate(ZeroPageX())           ; break;
  case 0x37: /* rmb */ rmb<0x08>()                    ; break;
  case 0x38: /* sec */ set_flag<flag_C>()             ; break;
  case 0x39: /* and */ LogicOp<AND>(AbsoluteY())      ; break;
  case 0x3a: /* dec */ decrement(m_regs.A)            ; break;
  case 0x3b: /* BAD */ bad_opcode<0x3B>()             ; break;
  case 0x3c: /* bit */ bit(AbsoluteX())               ; break;
  case 0x3d: /* and */ LogicOp<AND>(AbsoluteX())      ; break;
  case 0x3e: /* rol */ lrotate(AbsoluteX())           ; break;
  case 0x3f: /* bbr */ bbr<0x08>()                    ; break;
  case 0x40: /* rti */ rti()                          ; continue; // ReTurn from Interrupt
  case 0x41: /* eor */ LogicOp<XOR>(IndirectX())      ; break;
  case 0x42: /* say */ swap(m_regs.A, m_regs.Y)       ; break;
  case 0x43: /* tma */ tma()                          ; break;
  case 0x44: /* bsr */ bsr()                          ; break;
  case 0x45: /* eor */ LogicOp<XOR>(ZeroPage())       ; break;
  case 0x46: /* lsr */ rshift(ZeroPage())             ; break;
  case 0x47: /* rmb */ rmb<0x10>()                    ; break;
  case 0x48: /* pha */ push(m_regs.A)                 ; break;
  case 0x49: /* eor */ ImmediateLogicOp<XOR>()        ; break;
  case 0x4a: /* lsr */ lsr_a()                        ; break;
  case 0x4b: /* BAD */ bad_opcode<0x4B>()             ; break;
  case 0x4c: /* jmp */ jump(Immediate())              ; break;
  case 0x4d: /* eor */ LogicOp<XOR>(Absolute())       ; break;
  case 0x4e: /* lsr */ rshift(Absolute())             ; break;
  case 0x4f: /* bbr */ bbr<0x10>()                    ; break;
  case 0x50: /* bvc */ BranchIfNotSet<flag_V>()       ; break;
  case 0x51: /* eor */ LogicOp<XOR>(IndirectY())      ; break;
  case 0x52: /* eor */ LogicOp<XOR>(Indirect())       ; break;
  case 0x53: /* tam */ tam()                          ; break;
  case 0x54: /* csl */ csl()                          ; break;
  case 0x55: /* eor */ LogicOp<XOR>(ZeroPageX())      ; break;
  case 0x56: /* lsr */ rshift(ZeroPageX())            ; break;
  case 0x57: /* rmb */ rmb<0x20>()                    ; break;
  case 0x58: /* cli */ clear_flag<flag_I>()           ; break;
  case 0x59: /* eor */ LogicOp<XOR>(AbsoluteY())      ; break;
  case 0x5a: /* phy */ push(m_regs.Y)                 ; break;
  case 0x5b: /* BAD */ bad_opcode<0x5B>()             ; break;
  case 0x5c: /* BAD */ bad_opcode<0x5C>()             ; break;
  case 0x5d: /* eor */ LogicOp<XOR>(AbsoluteX())      ; break;
  case 0x5e: /* lsr */ rshift(AbsoluteX())            ; break;
  case 0x5f: /* bbr */ bbr<0x20>()                    ; break;
  case 0x60: /* rts */ rts()                          ; break;
  case 0x61: /* adc */ add(IndirectX())               ; break;
  case 0x62: /* cla */ cla()                          ; break;
  case 0x63: /* BAD */ bad_opcode<0x63>()             ; break;
  case 0x64: /* stz */ store(ZeroPage(), 0)           ; break;
  case 0x65: /* adc */ add(ZeroPage())                ; break;
  case 0x66: /* ror */ rrotate(ZeroPage())            ; break;
  case 0x67: /* rmb */ rmb<0x40>()                    ; break;
  case 0x68: /* pla */ pull(m_regs.A)                 ; break;
  case 0x69: /* adc */ add(Immediate())               ; break;
  case 0x6a: /* ror */ ror_a()                        ; break;
  case 0x6b: /* BAD */ bad_opcode<0x6B>()             ; break;
  case 0x6c: /* jmp */ jump(Absolute())               ; break;
  case 0x6d: /* adc */ add(Absolute())                ; break;
  case 0x6e: /* ror */ rrotate(Absolute())            ; break;
  case 0x6f: /* bbr */ bbr<0x40>()                    ; break;
  case 0x70: /* bvs */ BranchIfSet<flag_V>()          ; break;
  case 0x71: /* adc */ add(IndirectY())               ; break;
  case 0x72: /* adc */ add(Indirect())                ; break;
  case 0x73: /* tii */ BlockTransfer(tii_loop)        ; break;
  case 0x74: /* stz */ store(ZeroPageX(), 0)          ; break;
  case 0x75: /* adc */ add(ZeroPageX())               ; break;
  case 0x76: /* ror */ rrotate(ZeroPageX())           ; break;
  case 0x77: /* rmb */ rmb<0x80>()                    ; break;
  case 0x78: /* sei */ set_flag<flag_I>()             ; break;
  case 0x79: /* adc */ add(AbsoluteY())               ; break;
  case 0x7a: /* ply */ pull(m_regs.Y)                 ; break;
  case 0x7b: /* BAD */ bad_opcode<0x7B>()             ; break;
  case 0x7c: /* jmp */ jump(AbsoluteX())              ; break;
  case 0x7d: /* adc */ add(AbsoluteX())               ; break;
  case 0x7e: /* ror */ rrotate(AbsoluteX())           ; break;
  case 0x7f: /* bbr */ bbr<0x80>()                    ; break;
  case 0x80: /* bra */ bra()                          ; break;
  case 0x81: /* sta */ store(IndirectX(), m_regs.A)   ; break;
  case 0x82: /* clx */ clx()                          ; break;
  case 0x83: /* tst */ tst(ZeroPage(), GetPC())       ; break;
  case 0x84: /* sty */ store(ZeroPage(), m_regs.Y)    ; break;
  case 0x85: /* sta */ store(ZeroPage(), m_regs.A)    ; break;
  case 0x86: /* stx */ store(ZeroPage(), m_regs.X)    ; break;
  case 0x87: /* smb */ smb<0x01>()                    ; break;
  case 0x88: /* dey */ decrement(m_regs.Y)            ; break;
  case 0x89: /* bit */ bit(Immediate())               ; break;
  case 0x8a: /* txa */ transfer(m_regs.X, m_regs.A)   ; break;
  case 0x8b: /* BAD */ bad_opcode<0x8B>()             ; break;
  case 0x8c: /* sty */ store(Absolute(), m_regs.Y)    ; break;
  case 0x8d: /* sta */ store(Absolute(), m_regs.A)    ; break;
  case 0x8e: /* stx */ store(Absolute(), m_regs.X)    ; break;
  case 0x8f: /* bbs */ bbs<0x01>()                    ; break;
  case 0x90: /* bcc */ BranchIfNotSet<flag_C>()       ; break;
  case 0x91: /* sta */ store(IndirectY(), m_regs.A)   ; break;
  case 0x92: /* sta */ store(Indirect(), m_regs.A)    ; break;
  case 0x93: /* tst */ tst(Absolute(), GetPC())       ; break;
  case 0x94: /* sty */ store(ZeroPageX(), m_regs.Y)   ; break;
  case 0x95: /* sta */ store(ZeroPageX(), m_regs.A)   ; break;
  case 0x96: /* stx */ store(ZeroPageY(), m_regs.X)   ; break;
  case 0x97: /* smb */ smb<0x02>()                    ; break;
  case 0x98: /* tya */ transfer(m_regs.Y, m_regs.A)   ; break;
  case 0x99: /* sta */ store(AbsoluteY(), m_regs.A)   ; break;
  case 0x9a: /* txs */ txs()                          ; break;
  case 0x9b: /* BAD */ bad_opcode<0x9B>()             ; break;
  case 0x9c: /* stz */ store(Absolute(), 0)           ; break;
  case 0x9d: /* sta */ store(AbsoluteX(), m_regs.A)   ; break;
  case 0x9e: /* stz */ store(AbsoluteX(), 0)          ; break;
  case 0x9f: /* bbs */ bbs<0x02>()                    ; break;
  case 0xa0: /* ldy */ load(Immediate(), m_regs.Y)    ; break;
  case 0xa1: /* lda */ load(IndirectX(), m_regs.A)    ; break;
  case 0xa2: /* ldx */ load(Immediate(), m_regs.X)    ; break;
  case 0xa3: /* tst */ tst(ZeroPageX(), GetPC())      ; break;
  case 0xa4: /* ldy */ load(ZeroPage(), m_regs.Y)     ; break;
  case 0xa5: /* lda */ load(ZeroPage(), m_regs.A)     ; break;
  case 0xa6: /* ldx */ load(ZeroPage(), m_regs.X)     ; break;
  case 0xa7: /* smb */ smb<0x04>()                    ; break;
  case 0xa8: /* tay */ transfer(m_regs.A, m_regs.Y)   ; break;
  case 0xa9: /* lda */ load(Immediate(), m_regs.A)    ; break;
  case 0xaa: /* tax */ transfer(m_regs.A, m_regs.X)   ; break;
  case 0xab: /* BAD */ bad_opcode<0xAB>()             ; break;
  case 0xac: /* ldy */ load(Absolute(), m_regs.Y)     ; break;
  case 0xad: /* lda */ load(Absolute(), m_regs.A)     ; break;
  case 0xae: /* ldx */ load(Absolute(), m_regs.X)     ; break;
  case 0xaf: /* bbs */ bbs<0x04>()                    ; break;
  case 0xb0: /* bcs */ BranchIfSet<flag_C>()          ; break;
  case 0xb1: /* lda */ load(IndirectY(), m_regs.A)    ; break;
  case 0xb2: /* lda */ load(Indirect(), m_regs.A)     ; break;
  case 0xb3: /* tst */ tst(AbsoluteX(), GetPC())      ; break;
  case 0xb4: /* ldy */ load(ZeroPageX(), m_regs.Y)    ; break;
  case 0xb5: /* lda */ load(ZeroPageX(), m_regs.A)    ; break;
  case 0xb6: /* ldx */ load(ZeroPageY(), m_regs.X)    ; break;
  case 0xb7: /* smb */ smb<0x08>()                    ; break;
  case 0xb8: /* clv */ clear_flag<flag_V>()           ; break;
  case 0xb9: /* lda */ load(AbsoluteY(), m_regs.A)    ; break;
  case 0xba: /* tsx */ transfer(m_regs.S, m_regs.X)   ; break;
  case 0xbb: /* BAD */ bad_opcode<0xBB>()             ; break;
  case 0xbc: /* ldy */ load(AbsoluteX(), m_regs.Y)    ; break;
  case 0xbd: /* lda */ load(AbsoluteX(), m_regs.A)    ; break;
  case 0xbe: /* ldx */ load(AbsoluteY(), m_regs.X)    ; break;
  case 0xbf: /* bbs */ bbs<0x08>()                    ; break;
  case 0xc0: /* cpy */ compare(Immediate(), m_regs.Y) ; break;
  case 0xc1: /* cmp */ compare(IndirectX(), m_regs.A) ; break;
  case 0xc2: /* cly */ cly()                          ; break;
  case 0xc3: /* tdd */ BlockTransfer(tdd_loop)        ; break;
  case 0xc4: /* cpy */ compare(ZeroPage(), m_regs.Y)  ; break;
  case 0xc5: /* cmp */ compare(ZeroPage(), m_regs.A)  ; break;
  case 0xc6: /* dec */ decrement(ZeroPage())          ; break;
  case 0xc7: /* smb */ smb<0x10>()                    ; break;
  case 0xc8: /* iny */ increment(m_regs.Y)            ; break;
  case 0xc9: /* cmp */ compare(Immediate(), m_regs.A) ; break;
  case 0xca: /* dex */ decrement(m_regs.X)            ; break;
  case 0xcb: /* BAD */ bad_opcode<0xCB>()             ; break;
  case 0xcc: /* cpy */ compare(Absolute(), m_regs.Y)  ; break;
  case 0xcd: /* cmp */ compare(Absolute(), m_regs.A)  ; break;
  case 0xce: /* dec */ decrement(Absolute())          ; break;
  case 0xcf: /* bbs */ bbs<0x10>()                    ; break;
  case 0xd0: /* bne */ BranchIfNotSet<flag_Z>()       ; break;
  case 0xd1: /* cmp */ compare(IndirectY(), m_regs.A) ; break;
  case 0xd2: /* cmp */ compare(Indirect(), m_regs.A)  ; break;
  case 0xd3: /* tin */ BlockTransfer(tin_loop)        ; break;
  case 0xd4: /* csh */ csh()                          ; break;
  case 0xd5: /* cmp */ compare(ZeroPageX(), m_regs.A) ; break;
  case 0xd6: /* dec */ decrement(ZeroPageX())         ; break;
  case 0xd7: /* smb */ smb<0x20>()                    ; break;
  case 0xd8: /* cld */ clear_flag<flag_D>()           ; break;
  case 0xd9: /* cmp */ compare(AbsoluteY(), m_regs.A) ; break;
  case 0xda: /* phx */ push(m_regs.X)                 ; break;
  case 0xdb: /* BAD */ bad_opcode<0xDB>()             ; break;
  case 0xdc: /* BAD */ bad_opcode<0xDC>()             ; break;
  case 0xdd: /* cmp */ compare(AbsoluteX(), m_regs.A) ; break;
  case 0xde: /* dec */ decrement(AbsoluteX())         ; break;
  case 0xdf: /* bbs */ bbs<0x20>()                    ; break;
  case 0xe0: /* cpx */ compare(Immediate(), m_regs.X) ; break;
  case 0xe1: /* sbc */ subtract(IndirectX())          ; break;
  case 0xe2: /* BAD */ bad_opcode<0xE2>()             ; break;
  case 0xe3: /* tia */ BlockTransfer(tia_loop)        ; break;
  case 0xe4: /* cpx */ compare(ZeroPage(), m_regs.X)  ; break;
  case 0xe5: /* sbc */ subtract(ZeroPage())           ; break;
  case 0xe6: /* inc */ increment(ZeroPage())          ; break;
  case 0xe7: /* smb */ smb<0x40>()                    ; break;
  case 0xe8: /* inx */ increment(m_regs.X)            ; break;
  case 0xe9: /* sbc */ subtract(Immediate())          ; break;
  case 0xea: /* nop */ nop()                          ; break;
  case 0xeb: /* BAD */ bad_opcode<0xEB>()             ; break;
  case 0xec: /* cpx */ compare(Absolute(), m_regs.X)  ; break;
  case 0xed: /* sbc */ subtract(Absolute())           ; break;
  case 0xee: /* inc */ increment(Absolute())          ; break;
  case 0xef: /* bbs */ bbs<0x40>()                    ; break;
  case 0xf0: /* beq */ BranchIfSet<flag_Z>()          ; break;
  case 0xf1: /* sbc */ subtract(IndirectY())          ; break;
  case 0xf2: /* sbc */ subtract(Indirect())           ; break;
  case 0xf3: /* tai */ BlockTransfer(tai_loop)        ; break;
  case 0xf4: /* set */ set_flag<flag_T>()             ; continue; // SEt flag T
  case 0xf5: /* sbc */ subtract(ZeroPageX())          ; break;
  case 0xf6: /* inc */ increment(ZeroPageX())         ; break;
  case 0xf7: /* smb */ smb<0x80>()                    ; break;
  case 0xf8: /* sed */ set_flag<flag_D>()             ; break;
  case 0xf9: /* sbc */ subtract(AbsoluteY())          ; break;
  case 0xfa: /* plx */ pull(m_regs.X)                 ; break;
  case 0xfb: /* BAD */ bad_opcode<0xFB>()             ; break;
  case 0xfc: /* BAD */ bad_opcode<0xFC>()             ; break;
  case 0xfd: /* sbc */ subtract(AbsoluteX())          ; break;
  case 0xfe: /* inc */ increment(AbsoluteX())         ; break;
  case 0xff: /* bbs */ bbs<0x80>()                    ; break;
}


void DynarecCore::brk(void)
{

  m_regs.flags.T = 0;
  ++m_regs.PC16;

  raw_push(m_regs.PC.high); // 1
  AddCycle(); // 1
  raw_push(m_regs.PC.low ); // 1
  AddCycle(); // 1
  raw_push(m_regs.P | flag_B); // 1
  AddCycle(); // 1

  m_regs.flags.D = 0;
  m_regs.PC.low  = ReadMem(0xFFF6); // 1
  m_regs.PC.high = ReadMem(0xFFF7); // 1
  m_regs.flags.I = 1;
}
#endif

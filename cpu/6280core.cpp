#include "6280core.h"

namespace HuC6280
{
  Core::Core(void)
  {
    Reset();
  }

  void Core::Read (IO::common_busses_t& busses)
  {
  }

  void Core::Write(const IO::common_busses_t& busses)
  {
  }

  // 8 cycles
  inline void Core::Reset(void) // hard reset
  {
    m_regs.flags.T = 0;
    ++m_regs.PC16;
    raw_push(m_regs.PC.low    ); // 1
    raw_push(m_regs.PC.high   ); // 1
    raw_push(m_regs.P | flag_B); // 1

    m_regs.PC.low  = ReadMem(RESET_VECTOR    ); // 1
    m_regs.PC.high = ReadMem(RESET_VECTOR + 1); // 1
    AddCycle(); // 1
    AddCycle(); // 1
    AddCycle(); // 1

    m_reset = 1;
    m_speed = 2;
  }

  void Core::ProcessPorts(void)
  {
    IO::Interface& interface = *fromend<IO::Interface>(0);
    if(interface.RESET)
    {
      Reset();
      interface.RESET = 0;
    }
    else if(interface.CS)
    {
      if(interface.RD)
        Read(*interface.busses);
      else if(interface.WR)
        Write(*interface.busses);
    }
  }


  void Core::Execute(void)
  {
    for(;;)
    {
      // update ports
      ProcessPorts();

      if(m_reset) // soft reset
      {
        m_reset = 0;
        m_speed = 2; // low speed
        m_irqs.enabled.irq1  = 1;
        m_irqs.enabled.irq2  = 1;
        m_irqs.enabled.timer = 1;
        m_mem_mappers[7] = 0;

        m_regs.PC.low  = Read(RESET_VECTOR    ); // reset "vector" low byte
        m_regs.PC.high = Read(RESET_VECTOR + 1); // reset "vector" high byte
        m_regs.flags.I = 1;

        continue; // still possible for irq sample or low to be set
      }

      register uint16_t active_irqs = IRQsActive();
      if(active_irqs)
      {
        // Total: 8 cycles(2 read, 3 write, 2 read, 1 for irq stuff)
        ReadMem(m_regs.PC16    );      // Dummy read | cycle: 1
        ReadMem(m_regs.PC16 + 1);      // Dummy read | cycle: 2

        raw_push(m_regs.PC.high);      // Push PCH   | cycle: 3
        raw_push(m_regs.PC.low );      // Push PCL   | cycle: 4
        raw_push(m_regs.P & ~flag_B);  // Push P     | cycle: 5

        m_regs.flags.I = 1;
        m_regs.flags.T = 0;
        m_regs.flags.D = 0;

        switch(active_irqs)
        {
          case IQTIMER: active_irqs = TIMER_VECTOR; break;
          case IQIRQ1 : active_irqs = IRQ1_VECTOR ; break;
          case IQIRQ2 : active_irqs = IRQ2_VECTOR ; break;
          default:
            assert(false);
        }

        m_regs.PC.low  = ReadMem(active_irqs    );      // Fetch vector PCL | cycle 6
        m_regs.PC.high = ReadMem(active_irqs + 1);      // Fetch vector PCH | cycle 7

        // cycle 8
        continue;
      }

      //VarDump();
      register uint8_t op = Read(m_regs.PC16++);

      //========================== MASTER OPCODE TABLE =============================
      // Note: tst() requires GetPC() to execute _before_ the addressing mode function
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

      m_regs.flags.T = 0;
    }
  }


//====================================================================================

//========================== MEMORY IO ============================

  // 0 cycles
  uint8_t Core::Read (uint16_t address)
  {
    mapper_t& base = to<mapper_t>(address);
    IO::common_busses_t busses{base.page_address, m_mem_mappers[base.mapper]};
    Read(busses);
    return busses.data;
  }

  // 0 cycles
  void Core::Write (uint16_t address, uint8_t value) // 1
  {
    mapper_t& base = to<mapper_t>(address);
    Write(IO::common_busses_t {base.page_address, m_mem_mappers[base.mapper], value});
  }

  // 1 cycle
  uint8_t Core::ReadMem (uint16_t address)
  {
    AddCycle();
    return Read(address);
  }

  // 1 cycle
  void Core::WriteMem (uint16_t address, uint8_t value)
  {
    AddCycle();
    Write(address, value);
  }

  // 1 cycle
  uint8_t Core::GetPC(void)
  {
    return ReadMem(m_regs.PC16++);
  }


//========================== COMMON HELPERS ============================


  // 2 cycles
  template<uint8_t bit_flag>
  void Core::bad_opcode(void)
  {
#if defined(DEBUG_ENABLED) && !defined(DEBUG_SILENT)
    printf("\nopcode %02x is not valid!", bit_flag);
    fflush(stdout);
#endif
    AddCycle();
    AddCycle();
  }

  // 1 cycle
  void Core::SetZN(register uint8_t value)
  {
    m_regs.flags.Z = !value;
    m_regs.flags.N = value & flag_N ? 1 : 0;

    AddCycle();
  }


  // 0 cycles
  inline uint8_t Core::IRQsActive(void)
  {
    return m_regs.flags.I ? 0 : (to_u8(m_irqs.active) & to_u8(m_irqs.enabled));
  }

//========================== ADDRESSING MODES ============================

/*/
 * NOTE: Addressing modes execute each step explicitly in an effort to
 *       prevent compiler [de]optimization issues.
/*/

  // 3 cycles
  uint16_t Core::Absolute(void)
  {
    register uint16_t r1_16;
    register uint16_t r2_16;

    r1_16 = GetPC(); // low
    r2_16 = GetPC(); // high
    r2_16 <<= 8;
    r2_16 |= r1_16;

    AddCycle();
    return r2_16;
  }

  // 3 cycles
  uint16_t Core::AbsoluteX(void)
  {
    register uint16_t r1_16;
    register uint16_t r2_16;

    r1_16 = GetPC(); // low
    r2_16 = GetPC(); // high
    r2_16 <<= 8;
    r2_16 |= r1_16;
    r2_16 += m_regs.X;

    AddCycle();
    return r2_16;
  }

  // 3 cycles
  uint16_t Core::AbsoluteY(void)
  {
    register uint16_t r1_16;
    register uint16_t r2_16;

    r1_16 = GetPC(); // low
    r2_16 = GetPC(); // high
    r2_16 <<= 8;
    r2_16 |= r1_16;
    r2_16 += m_regs.Y;

    AddCycle();
    return r2_16;
  }


  // 2 cycles
  uint16_t Core::ZeroPageX(void)
  {
    register uint16_t r_16;

    r_16 = GetPC();
    r_16 += m_regs.X;
    r_16 &= 0x00FF;
    r_16 |= 0x2000;

    AddCycle();
    return r_16;
  }

  // 2 cycles
  uint16_t Core::ZeroPageY(void)
  {
    register uint16_t r_16;

    r_16 = GetPC();
    r_16 += m_regs.Y;
    r_16 &= 0x00FF;
    r_16 |= 0x2000;

    AddCycle();
    return r_16;
  }

  // 2 cycles
  uint16_t Core::ZeroPage(void)
  {
    AddCycle();
    return 0x2000 | GetPC();
  }

  // 5 cycles
  uint16_t Core::Indirect(void)
  {
    register uint8_t  r1_8;
    register uint16_t r2_16;
    register uint16_t r3_16;

    r1_8 = GetPC();
    r2_16 = r1_8;
    r2_16 |= 0x2000;
    r3_16 = ReadMem(r2_16);

    ++r1_8;
    r2_16 = r1_8;
    r2_16 |= 0x2000;
    r2_16 = ReadMem(r2_16);

    r2_16 <<= 8;
    r2_16 |= r3_16;

    AddCycle();
    AddCycle();
    return r2_16;
  }

  // 5 cycles
  uint16_t Core::IndirectX(void)
  {
    register uint8_t  r1_8;
    register uint16_t r2_16;
    register uint16_t r3_16;

    r1_8 = GetPC();
    r1_8 += m_regs.X;
    r2_16 = r1_8;
    r2_16 |= 0x2000;
    r3_16 = ReadMem(r2_16);

    ++r1_8;
    r2_16 = r1_8;
    r2_16 |= 0x2000;
    r2_16 = ReadMem(r2_16);

    r2_16 <<= 8;
    r2_16 |= r3_16;

    AddCycle();
    AddCycle();
    return r2_16;
  }

  // 5 cycles
  uint16_t Core::IndirectY(void)
  {
    register uint8_t  r1_8;
    register uint16_t r2_16;
    register uint16_t r3_16;

    r1_8 = GetPC();
    r2_16 = r1_8;
    r2_16 |= 0x2000;

    r3_16 = ReadMem(r2_16);
    ++r1_8;
    r2_16 = r1_8;
    r2_16 |= 0x2000;

    r2_16 = ReadMem(r2_16);
    r2_16 <<= 8;

    r3_16 |= r2_16;
    r3_16 += m_regs.Y;

    AddCycle();
    AddCycle();

    return r3_16;
  }

  //========================== END OF ADDRESSING MODES ============================

  // 3 cycles
  inline void Core::st0(void) { AddCycle(); AddCycle(); AddCycle(); Write({0x0000, 0xFF, GetPC()}); } // VDC Register select
  inline void Core::st1(void) { AddCycle(); AddCycle(); AddCycle(); Write({0x0002, 0xFF, GetPC()}); } // Low Data register
  inline void Core::st2(void) { AddCycle(); AddCycle(); AddCycle(); Write({0x0003, 0xFF, GetPC()}); } // High Data register


  //=============== BRANCHING =====================

  // conditionally 2 or 4 cycles
  template<uint8_t bit_flag>
  inline void Core::BranchIfSet(void)
  {
    if(m_regs.P & bit_flag) { bra(); } // 4
    else { AddCycle(); ++m_regs.PC16; AddCycle(); } // 2
  }

  // conditionally 2 or 4 cycles
  template<uint8_t bit_flag>
  inline void Core::BranchIfNotSet(void)
  {
    if(!(m_regs.P & bit_flag)) { bra(); } // 4
    else { AddCycle(); ++m_regs.PC16; AddCycle(); } // 2
  }

  // conditionally 2 or 4 cycles
  inline void Core::BranchIf(const bool cond)
  {
    if(cond) { bra(); } // 4
    else { AddCycle(); ++m_regs.PC16; AddCycle(); } // 2
  }

  // 4 cycles
  inline void Core::bra(void) // branch opcode (0x80)
  {
    m_regs.PC16 += static_cast<int8_t>(GetPC()); // 1
    AddCycle(); // 1
    AddCycle(); // 1
    //++m_regs.PC16;
    AddCycle(); // 1
  }

  //=============== JUMP AND RETURN =====================

  // address mode + 4 cycles
  void Core::jump(register uint16_t address)
  {
    AddCycle(); m_regs.PC.low = ReadMem(address); // 2
    ++address;
    m_regs.PC.high = ReadMem(address); // 2

    AddCycle();
  }

  // 7 cycles
  inline void Core::rti(void) // return from interupt
  {
    raw_pull(m_regs.P); // 1
    // restore location
    AddCycle(); // 1
    AddCycle(); // 1

    raw_pull(m_regs.PC.low); // 1
    AddCycle(); // 1

    raw_pull(m_regs.PC.high); // 1

    AddCycle(); // 1
  }

  //=============== SUBROUTINE ACTIONS =====================

  // 8 cycles
  inline void Core::bsr(void) // branch to subroutine (8 bit address)
  {
    // store current location + 1
    raw_push(m_regs.PC.high); // 1
    AddCycle(); // 1

    raw_push(m_regs.PC.low); // 1
    AddCycle(); // 1

    bra();  // bra (4)
  }

  // 7 cycles
  inline void Core::jsr(void) // jump to subrouting (16 bit address)
  {
    register uint8_t low_addr = GetPC(); // 1
    // store current location
    raw_push(m_regs.PC.high); // 1
    raw_push(m_regs.PC.low ); // 1
    AddCycle(); // 1

    m_regs.PC.high = GetPC(); // 1
    AddCycle(); // 1

    m_regs.PC.low = low_addr;
    AddCycle(); // 1
  }

  // 7 cycles
  inline void Core::rts(void) // return from subroutine (16 bit)
  {
    // restore location
    AddCycle();
    AddCycle();

    raw_pull(m_regs.PC.low ); // 1

    AddCycle(); // 1
    AddCycle(); // 1
    raw_pull(m_regs.PC.high); // 1

    ++m_regs.PC16;
    AddCycle(); // 1
  }

  //====================================

  // 8 cycles
  void Core::brk(void)
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
    m_regs.PC.low  = ReadMem(IRQ2_VECTOR    ); // 1
    m_regs.PC.high = ReadMem(IRQ2_VECTOR + 1); // 1
    m_regs.flags.I = 1;
  }

  //====================================

  // address mode + 2 cycles

  void Core::bit(register uint16_t address)
  {
    register uint8_t r_8;
    r_8 = ReadMem(address); // 1

    m_regs.flags.Z = !(r_8 & m_regs.A);
    m_regs.flags.N = r_8 & flag_N ? 1 : 0;
    m_regs.flags.V = r_8 & flag_V ? 1 : 0;

    AddCycle(); // 1
  }


  //====================================

  // address mode + 2 cycles
  inline void Core::compare(register uint16_t address, uint8_t& destination)
  {
    address = destination - ReadMem(address); // 1
    m_regs.flags.C = (address & 0x0100) ? 0 : 1;

    SetZN(address & 0x00FF); // 1
  }

  //================== SET CPU SPEED ==================

  // m_speed = 0 is 7.16 MHz
  // m_speed = 2 is 1.79 MHz

  // 3 cycles
  inline void Core::csh(void) { AddCycle(); AddCycle(); m_speed = 0; AddCycle(); } // 3
  inline void Core::csl(void) { AddCycle(); AddCycle(); m_speed = 2; AddCycle(); } // 3

  //============= BASIC MATH FUNCTIONS ================

  // address mode + 2 cycles
  // add 1 cycle if flag_D is set
  void Core::subtract(register uint16_t address)
  {
    register uint32_t l;
    register uint8_t r_8 = ReadMem(address); // 1

    if(m_regs.P & flag_D)
    {
      uint32_t c = (m_regs.P & 1) ^ 1;
      l = m_regs.A - r_8 - c;
      uint32_t low  = (m_regs.A & 0x0f) - (r_8 & 0x0f) - c;
      uint32_t high = (m_regs.A & 0xf0) - (r_8 & 0xf0);

      if(low & 0xf0) { low -= 0x06; }
      if(low & 0x80) { high -= 0x10; }
      if(high & 0x0f00) high -= 0x60;

      m_regs.P &= ~flag_C;
      m_regs.P |= ((l >> 8) & flag_C) ^ flag_C;
      m_regs.A = (low & 0x0f) | (high & 0xf0);
    }
    else
    {
      l = m_regs.A - r_8 - ((m_regs.P & 1) ^ 1);
      m_regs.P &= ~(flag_C | flag_V);
      m_regs.P |= ((m_regs.A ^ l) & (m_regs.A ^ r_8) & 0x80) >> 1;
      m_regs.P |= ((l >> 8) & flag_C) ^ flag_C;
      m_regs.A = l;
    }

    SetZN(m_regs.A);
  }

  // address mode + 2 cycles
  // add 1 cycle if flag_D is set
  // add 3 cycles if flag_T is set
  void Core::add(register uint16_t address)
  {
    register uint8_t old_A = 0;
    register uint8_t r_8 = ReadMem(address); // 1
    if(m_regs.P & flag_T)
    {
      old_A = m_regs.A;
      m_regs.A = ReadMem(0x2000 + m_regs.X); // 1
    }

    if(m_regs.P & flag_D)
    {
      register uint32_t low  = (m_regs.A & 0x0f) + (r_8 & 0x0f) + m_regs.flags.C;
      register uint32_t high = (m_regs.A & 0xf0) + (r_8 & 0xf0);
      if(low  > 0x09) { high += 0x10; low += 0x06; }
      if(high > 0x90) { high += 0x60; }

      m_regs.P &= ~flag_C;
      m_regs.P |= (high >> 8) & flag_C;

      m_regs.A = (low & 0x0F) | (high & 0xF0);
      AddCycle(); // 1
    }
    else
    {
      register uint32_t l = m_regs.A + r_8 + (m_regs.P & flag_C);
      m_regs.P &= ~(flag_Z | flag_C | flag_N | flag_V); // clear these flags
      m_regs.P |= ((((m_regs.A ^ r_8) & flag_N) ^ flag_N) & ((m_regs.A ^ l) & flag_N)) >> 1;
      m_regs.P |= (l >> 8) & flag_C;
      m_regs.A = l;
    }


    SetZN(m_regs.A); // 1

    if(m_regs.P & flag_T)
    {
      WriteMem(0x2000 + m_regs.X, m_regs.A); // 1
      m_regs.A = old_A;
      AddCycle(); // 1
    }
  }

  //====================================

  // 5 cycles
  inline void Core::tam(void)
  {
    register uint8_t r_8 = GetPC(); // 1

    for(register uint8_t i = 0; i < 8; ++i)
      if(r_8 & (1 << i))
        m_mem_mappers[i] = m_regs.A;

    AddCycle(); // 1
    AddCycle(); // 1
    AddCycle(); // 1

    AddCycle(); // 1
  }

  // 4 cycles
  void Core::tma(void)
  {
    register uint8_t r_8 = GetPC(); // 1
    register uint8_t i = 0;

    for(; i < 4; ++i)
      if(r_8 & (1 << i))
        m_regs.A = static_cast<uint8_t>(m_mem_mappers[i]);
    AddCycle(); // 1
    AddCycle(); // 1

    for(; i < 8; ++i)
      if(r_8 & (1 << i))
        m_regs.A = static_cast<uint8_t>(m_mem_mappers[i]);

    AddCycle(); // 1
  }


  // ========================== block transfer operations ==========================

  // variables used exclusively for block transfer operations
  static uint16_t m_bmt_source;
  static uint16_t m_bmt_destination;
  static uint16_t m_bmt_alternate;

  void Core::tai_loop(Core* instance) // 2 cycles
  {
    instance->WriteMem(m_bmt_destination, instance->ReadMem(m_bmt_source + m_bmt_alternate)); // 2
    ++m_bmt_destination;
    m_bmt_alternate ^= 1;
  }

  void Core::tdd_loop(Core* instance) // 2 cycles
  {
    instance->WriteMem(m_bmt_destination, instance->ReadMem(m_bmt_source)); // 2
    --m_bmt_source;
    --m_bmt_destination;
  }

  void Core::tia_loop(Core* instance) // 2 cycles
  {
    instance->WriteMem(m_bmt_destination + m_bmt_alternate, instance->ReadMem(m_bmt_source)); // 2
    ++m_bmt_source;
    m_bmt_alternate ^= 1;
  }

  void Core::tii_loop(Core* instance) // 2 cycles
  {
    instance->WriteMem(m_bmt_destination, instance->ReadMem(m_bmt_source)); // 2
    ++m_bmt_source;
    ++m_bmt_destination;
  }

  void Core::tin_loop(Core* instance) // 2 cycles
  {
    instance->WriteMem(m_bmt_destination, instance->ReadMem(m_bmt_source)); // 2
    ++m_bmt_source;
  }

  // 17 cycles + 6 cycles per loop
  void Core::BlockTransfer(instance_function transfer_func)
  {
    register uint16_t transfer_length;

    // store state

    raw_push(m_regs.Y); // 1
    raw_push(m_regs.A); // 1
    raw_push(m_regs.X); // 1

    m_bmt_source = Absolute();      // 3
    m_bmt_destination = Absolute(); // 3
    transfer_length = Absolute();   // 3
    m_bmt_alternate = 0;

    AddCycle(); // 1
    do
    {
      AddCycle();
      AddCycle();
      transfer_func(this); // 2
      AddCycle();
      AddCycle();
    } while(--transfer_length); // if transfer_length is 0, it will loop 0xFFFF times

    // restore state
    raw_pull(m_regs.X); // 1
    raw_pull(m_regs.A); // 1
    raw_pull(m_regs.Y); // 1

    AddCycle(); // 1
  }

  //====================================

  // address mode + 4 cycles
  void Core::trb(register uint16_t address)
  {
    register uint8_t r_8 = ReadMem(address); // 1

    m_regs.P &= ~(flag_Z | flag_N | flag_V);
    AddCycle(); // 1
    m_regs.P |= (r_8 & ~m_regs.A) ? 0 : flag_Z;
    m_regs.P |= r_8 & (flag_N | flag_V);

    r_8  &= ~m_regs.A;
    AddCycle(); // 1

    WriteMem(address, r_8); //1
  }

  // address mode + 4 cycles

  void Core::tsb(register uint16_t address)
  {
    register uint8_t r_8 = ReadMem(address); // 1
    m_regs.P &= ~(flag_Z | flag_N | flag_V);
    AddCycle(); // 1

    if(!(r_8 | m_regs.A)) { m_regs.P |= flag_Z; }
    m_regs.P |= r_8 & (flag_N | flag_V);
    r_8  |= m_regs.A;
    AddCycle(); // 1

    WriteMem(address, r_8); // 1
  }

  // address mode + 5 cycles
  void Core::tst(register uint16_t address, register uint8_t value)
  {
    register uint8_t r_8 = ReadMem(address); // 1

    m_regs.P &= ~(flag_Z | flag_V | flag_N);
    AddCycle(); // 1

    m_regs.P |= (r_8 & value) ? 0 : flag_Z;
    AddCycle(); // 1

    m_regs.P |= r_8 & (flag_V | flag_N);
    AddCycle(); // 1
  }



  // ========================== bitwise operations ==========================

  // 1 cycle
  template<uint8_t bit_flag>
  void Core::conditional_set_flag(register bool condition)
  {
    if(condition)
      { m_regs.P |= bit_flag; }
    else
      { m_regs.P &= ~bit_flag; }

    AddCycle(); // 1
  }

  // ============= bit shift =============

  // address mode + 4 cycles
  void Core::lshift(register uint16_t address) // fixed
  {
    register uint8_t r_8 = ReadMem(address); // 1
    WriteMem(address, r_8 << 1); // 1

    conditional_set_flag<flag_C>(r_8 & 0x80); // 1

    SetZN(r_8 << 1); // 1
  }

  // 2 cycles
  inline void Core::asl_a(void)
  {
    conditional_set_flag<flag_C>(m_regs.A & 0x80); // 1
    m_regs.A <<= 1;

    SetZN(m_regs.A); // 1
  }

  // address mode + 4 cycles
  void Core::rshift(register uint16_t address)
  {
    register uint8_t r_8 = ReadMem(address); // 1
    m_regs.P &= ~(flag_C | flag_N | flag_Z);
    m_regs.P |= r_8 & flag_C;
    AddCycle(); // 1

    r_8 >>= 1;
    SetZN(r_8); // 1

    WriteMem(address, r_8); // 1
  }

  // 2 cycles
  inline void Core::lsr_a(void)
  {
    conditional_set_flag<flag_C>(m_regs.A & 0x01); // 1
    m_regs.A >>= 1;

    SetZN(m_regs.A); // 1
  }


  // ============= bit rotate =============

  // address mode + 4 cycles
  void Core::lrotate(register uint16_t address)
  {
    register uint8_t r_8 = ReadMem(address); // 1
    register uint8_t carry = r_8 & 0x80;

    r_8 <<= 1;
    r_8 |= (m_regs.P & flag_C);
    m_regs.P &= ~(flag_Z | flag_N | flag_C);        // clear these flags
    AddCycle(); // 1

    if(carry) { m_regs.flags.C = 1; }
    WriteMem(address, r_8); // 1

    SetZN(r_8); // 1
  }

  // 2 cycles
  inline void Core::rol_a(void) // fixed
  {
    register uint8_t r_8;
    r_8 = m_regs.A;
    m_regs.A <<= 1;
    m_regs.A |= m_regs.P & flag_C;
    m_regs.P &= ~(flag_Z | flag_N | flag_C);        // clear these flags
    AddCycle(); // 1

    if(r_8 & 0x80) { m_regs.P |= flag_C; }

    SetZN(m_regs.A); // 1
  }

  // address mode + 4 cycles
  void Core::rrotate(register uint16_t address)
  {
    register uint8_t r_8 = ReadMem(address); // 1
    register uint8_t carry = r_8 & 0x01;
    r_8 >>= 1;
    if(m_regs.flags.C) { r_8 |= 0x80; }
    m_regs.P &= ~(flag_Z | flag_N | flag_C); // clear flags
    if(carry) { m_regs.flags.C = 1; }
    AddCycle(); // 1

    SetZN(r_8); // 1

    WriteMem(address, r_8); // 1
  }

  // 2 cycles
  inline void Core::ror_a(void)
  {
    register uint8_t old_A = m_regs.A;
    m_regs.A >>= 1;
    m_regs.A |= m_regs.flags.C << 7;
    conditional_set_flag<flag_C>(old_A & 0x01); // 1

    SetZN(m_regs.A); // 1
  }

  // ============= AND/OR/XOR logic (result in accumulator) =============

  // address mode + 4 cycles if T is set, 2 cycles if not
  template<logic_op_function operation>
  void Core::LogicOp(register uint16_t address)
  {
    register uint8_t r_8;
    if(m_regs.P & flag_T)
    {
      r_8 = ReadMem(0x2000 + m_regs.X); // 1
      r_8 = operation(r_8, ReadMem(address)); // 1
      SetZN(r_8); // 1

      WriteMem(0x2000 + m_regs.X, r_8); // 1
    }
    else
    {
      register uint16_t r_16 = address;
      r_8 = ReadMem(r_16); // 1
      m_regs.A = operation(m_regs.A, r_8);

      SetZN(m_regs.A); // 1
    }
  }

  // 4 cycles if T is set, 2 cycles if not
  template<logic_op_function operation>
  void Core::ImmediateLogicOp(void)
  {
    if(m_regs.flags.T)
    {
      register uint8_t r_8 = operation(ReadMem(0x2000 + m_regs.X), GetPC()); // 2
      SetZN(r_8);                       // 1
      WriteMem(0x2000 + m_regs.X, r_8); // 1
    }
    else
    {
      m_regs.A = operation(m_regs.A, GetPC()); // 1
      SetZN(m_regs.A);                         // 1
    }
  }


  // ============= increment/decrement =============

  // address mode + 4 cycles
  void Core::increment(register uint16_t address)
  {
    register uint8_t r_8;
    r_8 = ReadMem(address);   // 1
    WriteMem(address, ++r_8); // 1
    SetZN(r_8);               // 1

    AddCycle();               // 1
  }

  // 2 cycles
  void Core::increment(uint8_t& source)
  {
    ++source;
    SetZN(source); // 1

    AddCycle(); // 1
  }

  // address mode + 4 cycles
  void Core::decrement(register uint16_t address)
  {
    register uint8_t r_8 = ReadMem(address); // 1
    WriteMem(address, --r_8); // 1
    SetZN(r_8); // 1

    AddCycle(); // 1
  }

  // 2 cycles
  void Core::decrement(uint8_t& source)
  {
    --source;
    SetZN(source); // 1

    AddCycle(); // 1
  }

  // ========================== flag operations ==========================

  // 2 cycles
  template<uint8_t bit_flag>
  void Core::set_flag(void)
  {
    AddCycle(); // 1
    m_regs.P |= bit_flag;

    AddCycle(); // 1
  }

  // 2 cycles
  template<uint8_t bit_flag>
  void Core::clear_flag(void)
  {
    AddCycle(); // 1
    m_regs.P &= ~bit_flag;

    AddCycle(); // 1
  }

  // ========================== bit operations ==========================

  // ============= (re)set bit branch =============

  // 6 or 8 cycles (depending on branch result)
  template<uint8_t bit_flag>
  void Core::bbs(void)
  {
    AddCycle(); // 1
    BranchIf(ReadMem(ZeroPage()) & bit_flag); // 2 + 1 + (2 or 4)
  }

  // 6 or 8 cycles (depending on branch result)
  template<uint8_t bit_flag>
  void Core::bbr(void)
  {
    AddCycle(); // 1
    BranchIf(!(ReadMem(ZeroPage()) & bit_flag)); // 1 + (2 or 4)
  }

  // ============= (re)set bit =============

  // 7 cycles
  template<uint8_t bit_flag>
  void Core::smb(void)
  {
    register uint16_t address = ZeroPage();
    AddCycle();
    AddCycle();
    WriteMem(address, ReadMem(address) | bit_flag); // 2

    AddCycle(); // 1
  }

  // 7 cycles
  template<uint8_t bit_flag>
  void Core::rmb(void)
  {
    uint16_t address = ZeroPage();
    AddCycle();
    AddCycle();
    WriteMem(address, ReadMem(address) & ~bit_flag); // 2

    AddCycle(); // 1
  }

  // ========================== basic register manipulation ==========================


  // ============= push register to stack =============

  // 3 cycles
  void Core::push(register uint8_t source) // copy register to stack
  {
    AddCycle(); // 1
    raw_push(source); // 1

    AddCycle(); // 1
  }

  // ============= pull/pop register from stack =============

  // 4 cycles
  void Core::pull(uint8_t& destination) // load register from stack
  {
    raw_pull(destination); // 1
    AddCycle(); // 1
    SetZN(destination); // 1

    AddCycle(); // 1
  }

  // 1 cycle
  void Core::raw_push(register uint8_t source) // copy to stack
  {
    WriteMem(0x2100 | m_regs.S--, source); // 1
  }

  // 1 cycles
  void Core::raw_pull(uint8_t& destination) // load from stack
  {
    destination = ReadMem(0x2100 | ++m_regs.S); // 1
  }

  // 4 cycles
  inline void Core::plp(void) // load register P (flags) from stack
  {
    AddCycle(); // 1
    AddCycle(); // 1
    raw_pull(m_regs.P); // 1
    AddCycle(); // 1
  }

  // ============= store/load/clear/copy/swap register values =============

  // address mode + 2 cycles
  void Core::store(register uint16_t address, register uint8_t source) // store register value in stack
  {
    WriteMem(address, source); // 1
    AddCycle(); // 1
  }


  // address mode + 2 cycles
  void Core::load(/*register*/ uint16_t address, uint8_t& destination) // load register value from stack
  {
    destination = ReadMem(address); // 1
    SetZN(destination); // 1
  }


  // 2 cycles
  inline void Core::cla(void) { AddCycle(); m_regs.A = 0; AddCycle(); } // clear acumulator
  inline void Core::clx(void) { AddCycle(); m_regs.X = 0; AddCycle(); } // clear X
  inline void Core::cly(void) { AddCycle(); m_regs.Y = 0; AddCycle(); } // clear Y


  inline void Core::txs(void)
  {
    AddCycle(); // 1
    m_regs.S = m_regs.X;
    AddCycle(); // 1
  }


  // 2 cycles
  inline void Core::transfer(register uint8_t source, uint8_t& destination) // copy register value to another
  {
    AddCycle(); // 1
    destination = source;
    SetZN(source); // 1
  }


  // 3 cycles
  inline void Core::swap(uint8_t& first, uint8_t& second) // swap values between registers
  {
    register uint8_t r_8;
    AddCycle(); r_8 = first;     // 1
    AddCycle(); first = second;  // 1
    AddCycle(); second = r_8;    // 1
  }



  //========================= TIMER/IRQs ============================


  void Core::AddCycle(void)
  {
    EndOfClockCycle();
    EndOfClockCycle();
    EndOfClockCycle();

    if(m_speed) // low speed - 1.79 MHz
    {
      EndOfClockCycle();
      EndOfClockCycle();
      EndOfClockCycle();

      EndOfClockCycle();
      EndOfClockCycle();
      EndOfClockCycle();

      EndOfClockCycle();
      EndOfClockCycle();
      EndOfClockCycle();
    }

    if(m_timer.isEnabled() && !--m_timer)
      m_irqs.active.timer = 1; // activate timer IRQ
  }
}

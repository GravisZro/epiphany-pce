#ifndef HUC6280_CORE_H
#define HUC6280_CORE_H

//======================================================
//=============== HuC6280 Processor Core ===============
//======================================================

#include <global.h>
#include <businterface.h>

#include "6280timer.h"
#include "6280registers.h"
#include "6280irqs.h"

namespace HuC6280
{
  class Core;
  typedef void (*instance_function)(void);
  typedef uint8_t (*logic_op_function)(uint8_t a, uint8_t b);

  class Core
  {
  public:
    Core(void);

    static void Execute(void);

  #ifdef DEBUG_ENABLED
  protected:
    virtual void VarDump(void) = 0;
  #endif

    static void ProcessPorts(void);
    static void Reset(void);
    static void Read (      IO::common_busses_t& busses);
    static void Write(const IO::common_busses_t& busses);

  private:
    static inline addr_range_t AddressRange(void) { return {IO::GameSaveMemory, IO::WorkRAM8K}; }

  /* =========== INSTRUCTION SET IMPLEMENTION FUNCTIONS =========== */

  // unique functions
    static void asl_a(void); // op code: 0x0A | addressing mode: Accumulator
    static void bra  (void); // op code: 0x80 | addressing mode: Relative
    static void brk  (void); // op code: 0x00 | addressing mode: Implied
    static void bsr  (void); // op code: 0x44 | addressing mode: Relative
    static void cla  (void); // op code: 0x62 | addressing mode: Implied
    static void clx  (void); // op code: 0x82 | addressing mode: Implied
    static void cly  (void); // op code: 0xC2 | addressing mode: Implied
    static void csh  (void); // op code: 0xD4 | addressing mode: Implied
    static void csl  (void); // op code: 0x54 | addressing mode: Implied
    static void jsr  (void); // op code: 0x20 | addressing mode: Absolute
    static void lsr_a(void); // op code: 0x4A | addressing mode: Accumulator
    static void plp  (void); // op code: 0x28 | addressing mode: Implied
    static void rol_a(void); // op code: 0x2A | addressing mode: Accumulator
    static void ror_a(void); // op code: 0x6A | addressing mode: Accumulator
    static void rti  (void); // op code: 0x40 | addressing mode: Implied
    static void rts  (void); // op code: 0x60 | addressing mode: Implied
    static void st0  (void); // op code: 0x03 | addressing mode: Immediate
    static void st1  (void); // op code: 0x13 | addressing mode: Immediate
    static void st2  (void); // op code: 0x23 | addressing mode: Immediate
    static void tam  (void); // op code: 0x53 | addressing mode: Immediate
    static void tma  (void); // op code: 0x43 | addressing mode: Immediate
    static void txs  (void); // op code: 0x9A | addressing mode: Implied

    template<uint8_t bit_flag> static void bad_opcode(void); // used to catch calls to bad opcodes
    static void nop(void) { AddCycle(); AddCycle(); } // 2  --  do nothing but waste cycles


  // === ADDRESSING MODES ===

    typedef uint16_t (*address_mode)(void);

  // 3 cycles
    static uint16_t Absolute (void);
    static uint16_t AbsoluteX(void);
    static uint16_t AbsoluteY(void);

  // 2 cycles
    static uint16_t ZeroPage (void);
    static uint16_t ZeroPageX(void);
    static uint16_t ZeroPageY(void);

  // 5 cycles
    static uint16_t Indirect (void);
    static uint16_t IndirectX(void);
    static uint16_t IndirectY(void);

    static uint16_t Immediate(void) { return m_regs.PC16++; }

  // === LOGIC OPERATIONS AND PROCESSORS ===
  // (types: logic_op_function)
    static constexpr uint8_t AND(uint8_t a, uint8_t b) { return a & b; } // 0
    static constexpr uint8_t OR (uint8_t a, uint8_t b) { return a | b; } // 0
    static constexpr uint8_t XOR(uint8_t a, uint8_t b) { return a ^ b; } // 0

    template<logic_op_function operation> static void LogicOp(register uint16_t address);
    template<logic_op_function operation> static void ImmediateLogicOp(void);

  // === BRANCHING ===
    template<uint8_t bit_flag> static void bbs(void);
    template<uint8_t bit_flag> static void bbr(void);
    template<uint8_t bit_flag> static void smb(void);
    template<uint8_t bit_flag> static void rmb(void);

  // === REGISTER MANIPULATORS ===
    static void decrement(uint8_t& source);
    static void increment(uint8_t& source);

    static void jump     (register uint16_t address);
    static void decrement(register uint16_t address);
    static void increment(register uint16_t address);
    static void rrotate  (register uint16_t address);
    static void lrotate  (register uint16_t address);
    static void rshift   (register uint16_t address);
    static void lshift   (register uint16_t address);

    static void subtract (register uint16_t address);
    static void add      (register uint16_t address);

    static void push (register uint8_t source);
    static void pull (uint8_t& destination);
    static void raw_push (register uint8_t source);
    static void raw_pull (uint8_t& destination);

    static void transfer(register uint8_t source, uint8_t& destination);
    static void swap    (uint8_t& first, uint8_t& second);

    static void store  (register uint16_t address, register uint8_t source);
    static void compare(register uint16_t address, uint8_t& destination);
    static void load   (/*register*/ uint16_t address, uint8_t& destination);

    static void trb(register uint16_t address);
    static void tsb(register uint16_t address);
    static void tst(register uint16_t address, register uint8_t value);
    static void bit(register uint16_t address);

  // =========== BLOCK TRANSFER FUNCTIONS ===========

    static void BlockTransfer(instance_function transfer_func);

    static void tii_loop(void);
    static void tdd_loop(void);
    static void tin_loop(void);
    static void tia_loop(void);
    static void tai_loop(void);

  // =========== FLAG MANIPULATION FUNCTIONS (not opcodes) ===========

    template<uint8_t bit_flag> static void set_flag(void);
    template<uint8_t bit_flag> static void clear_flag(void);
    template<uint8_t bit_flag> static void conditional_set_flag(register bool condition);

    static void SetZN(register uint8_t value);

  // =========== MEMORY ACCESS FUNCTIONS ===========

  // === DIRECT ROM/RAM ACCESS ===

    template<uint8_t bit_flag> static void BranchIfSet   (void);
    template<uint8_t bit_flag> static void BranchIfNotSet(void);
    static void BranchIf(const bool cond);

  // === MISC ===
    static void AddCycle(void);

    static uint8_t GetPC(void) { return ReadMem(m_regs.PC16++); } // 1

  protected:
    struct mapper_t
    {
      uint16_t page_address : 13;
      uint16_t mapper       :  3;
    };
/*
    static uint8_t Read (uint16_t address);
    static void Write (uint16_t address, uint8_t value);
    static uint8_t ReadMem (uint16_t address);
    static void WriteMem (uint16_t address, uint8_t value);
*/
    static uint8_t Read (uint16_t address) // 1
    {
      mapper_t& base = to<mapper_t>(address);
      IO::common_busses_t busses{base.page_address, m_mem_mappers[base.mapper]};
      Read(busses);
      return busses.data;
    }

    static void Write (uint16_t address, uint8_t value) // 1
    {
      mapper_t& base = to<mapper_t>(address);
      Write(IO::common_busses_t {base.page_address, m_mem_mappers[base.mapper], value});
    }

    static uint8_t ReadMem (uint16_t address) // 1
      { AddCycle(); return Read(address); }

    static void WriteMem (uint16_t address, uint8_t value) // 1
      { AddCycle(); Write(address, value); }

  // =========== CPU REGISTERS ===========
  public:

    enum : uint16_t
    {
      // page 0x00
      RESET_VECTOR  = 0xFFFE,
      NMI_VECTOR    = 0xFFFC,

      TIMER_VECTOR  = 0xFFFA,
      IRQ1_VECTOR   = 0xFFF8,
      IRQ2_VECTOR   = 0xFFF6,
    };

    static Registers m_regs;
    static Timer m_timer;
    static IRQs  m_irqs;

    static uint8_t m_mem_mappers[8];
    static uint8_t m_reset;
    static uint8_t m_speed;

    static uint8_t IRQsActive(void)
      { return m_regs.flags.I ? 0 : (to_u8(m_irqs.active) & to_u8(m_irqs.enabled)); } // 0

  };
}
#endif

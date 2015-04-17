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
  typedef void (*instance_function)(Core*);
  typedef uint8_t (*logic_op_function)(uint8_t a, uint8_t b);

  class Core : protected BusInterface
  {
    /*
  private:
    Core(void);
    Core(const Core&);
    Core& operator=(const Core&);
  public:
    static Core& Instance()
    {
      static Core singleton;
      return singleton;
    }
    */
  public:
    Core(void);

    void Execute(void);

  #ifdef DEBUG_ENABLED
  protected:
    virtual void VarDump(void) = 0;
  #endif

  private:
    addr_range_t AddressRange(void) const { return {IO::GameSaveMemory, IO::WorkRAM8K}; }
    void ProcessPorts(void);
    void Reset(void);
    void Read (      IO::common_busses_t& busses);
    void Write(const IO::common_busses_t& busses);

  /* =========== INSTRUCTION SET IMPLEMENTION FUNCTIONS =========== */

  // unique functions
    void asl_a(void); // op code: 0x0A | addressing mode: Accumulator
    void bra  (void); // op code: 0x80 | addressing mode: Relative
    void brk  (void); // op code: 0x00 | addressing mode: Implied
    void bsr  (void); // op code: 0x44 | addressing mode: Relative
    void cla  (void); // op code: 0x62 | addressing mode: Implied
    void clx  (void); // op code: 0x82 | addressing mode: Implied
    void cly  (void); // op code: 0xC2 | addressing mode: Implied
    void csh  (void); // op code: 0xD4 | addressing mode: Implied
    void csl  (void); // op code: 0x54 | addressing mode: Implied
    void jsr  (void); // op code: 0x20 | addressing mode: Absolute
    void lsr_a(void); // op code: 0x4A | addressing mode: Accumulator
    void plp  (void); // op code: 0x28 | addressing mode: Implied
    void rol_a(void); // op code: 0x2A | addressing mode: Accumulator
    void ror_a(void); // op code: 0x6A | addressing mode: Accumulator
    void rti  (void); // op code: 0x40 | addressing mode: Implied
    void rts  (void); // op code: 0x60 | addressing mode: Implied
    void st0  (void); // op code: 0x03 | addressing mode: Immediate
    void st1  (void); // op code: 0x13 | addressing mode: Immediate
    void st2  (void); // op code: 0x23 | addressing mode: Immediate
    void tam  (void); // op code: 0x53 | addressing mode: Immediate
    void tma  (void); // op code: 0x43 | addressing mode: Immediate
    void txs  (void); // op code: 0x9A | addressing mode: Implied

    template<uint8_t bit_flag> void bad_opcode(void); // used to catch calls to bad opcodes
    inline void nop(void) { AddCycle(); AddCycle(); } // 2  --  do nothing but waste cycles


  // === ADDRESSING MODES ===

    typedef uint16_t (*address_mode)(void);

  // 3 cycles
    uint16_t Absolute (void);
    uint16_t AbsoluteX(void);
    uint16_t AbsoluteY(void);

  // 2 cycles
    uint16_t ZeroPage (void);
    uint16_t ZeroPageX(void);
    uint16_t ZeroPageY(void);

  // 5 cycles
    uint16_t Indirect (void);
    uint16_t IndirectX(void);
    uint16_t IndirectY(void);

    uint16_t Immediate(void) { return m_regs.PC16++; }

  // === LOGIC OPERATIONS AND PROCESSORS ===
  // (types: logic_op_function)
    static constexpr uint8_t AND(uint8_t a, uint8_t b) { return a & b; } // 0
    static constexpr uint8_t OR (uint8_t a, uint8_t b) { return a | b; } // 0
    static constexpr uint8_t XOR(uint8_t a, uint8_t b) { return a ^ b; } // 0

    template<logic_op_function operation> void LogicOp(register uint16_t address);
    template<logic_op_function operation> void ImmediateLogicOp(void);

  // === BRANCHING ===
    template<uint8_t bit_flag> void bbs(void);
    template<uint8_t bit_flag> void bbr(void);
    template<uint8_t bit_flag> void smb(void);
    template<uint8_t bit_flag> void rmb(void);

  // === REGISTER MANIPULATORS ===
    void decrement(uint8_t& source);
    void increment(uint8_t& source);

    void jump     (register uint16_t address);
    void decrement(register uint16_t address);
    void increment(register uint16_t address);
    void rrotate  (register uint16_t address);
    void lrotate  (register uint16_t address);
    void rshift   (register uint16_t address);
    void lshift   (register uint16_t address);

    void subtract (register uint16_t address);
    void add      (register uint16_t address);

    void push (register uint8_t source);
    void pull (uint8_t& destination);
    void raw_push (register uint8_t source);
    void raw_pull (uint8_t& destination);

    void transfer(register uint8_t source, uint8_t& destination);
    void swap    (uint8_t& first, uint8_t& second);

    void store  (register uint16_t address, register uint8_t source);
    void compare(register uint16_t address, uint8_t& destination);
    void load   (/*register*/ uint16_t address, uint8_t& destination);

    void trb(register uint16_t address);
    void tsb(register uint16_t address);
    void tst(register uint16_t address, register uint8_t value);
    void bit(register uint16_t address);

  // =========== BLOCK TRANSFER FUNCTIONS ===========

    void BlockTransfer(instance_function transfer_func);

    static void tii_loop(Core* instance);
    static void tdd_loop(Core* instance);
    static void tin_loop(Core* instance);
    static void tia_loop(Core* instance);
    static void tai_loop(Core* instance);

  // =========== FLAG MANIPULATION FUNCTIONS (not opcodes) ===========

    template<uint8_t bit_flag> void set_flag(void);
    template<uint8_t bit_flag> void clear_flag(void);
    template<uint8_t bit_flag> void conditional_set_flag(register bool condition);

    void SetZN(register uint8_t value);

  // =========== MEMORY ACCESS FUNCTIONS ===========

  // === DIRECT ROM/RAM ACCESS ===

    template<uint8_t bit_flag> void BranchIfSet   (void);
    template<uint8_t bit_flag> void BranchIfNotSet(void);
    void BranchIf(const bool cond);

  // === MISC ===
    void AddCycle(void);

    uint8_t GetPC(void);// { return ReadMem(m_regs.PC16++); } // 1

  protected:
    struct mapper_t
    {
      uint16_t page_address : 13;
      uint16_t mapper       :  3;
    };

    uint8_t Read (uint16_t address);
    void Write (uint16_t address, uint8_t value);
    uint8_t ReadMem (uint16_t address);
    void WriteMem (uint16_t address, uint8_t value);
/*
    uint8_t Read (uint16_t address) // 1
    {
      mapper_t& base = to<mapper_t>(address);
      IO::common_busses_t busses{base.page_address, m_mem_mappers[base.mapper]};
      Instance().Read(busses);
      return busses.data;
    }

    void Write (uint16_t address, uint8_t value) // 1
    {
      mapper_t& base = to<mapper_t>(address);
      Instance().Write(IO::common_busses_t {base.page_address, m_mem_mappers[base.mapper], value});
    }

    uint8_t ReadMem (uint16_t address) // 1
      { AddCycle(); return Read(address); }

    void WriteMem (uint16_t address, uint8_t value) // 1
      { AddCycle(); Write(address, value); }
*/

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

    regs_t m_regs;
    Timer m_timer;
    IRQs  m_irqs;

    uint8_t m_mem_mappers[8];
    uint8_t m_reset;
    uint8_t m_speed;

    uint8_t IRQsActive(void);
      //{ return m_regs.flags.I ? 0 : (to_u8(m_irqs.active) & to_u8(m_irqs.enabled)); } // 0

  };
}

#endif

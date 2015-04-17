#ifndef HUC6280_REGISTERS_H
#define HUC6280_REGISTERS_H

#include <global.h>

namespace HuC6280
{
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

  struct regs_t
  {
    union
    {
      highlow_t PC;    // Program Counter
      uint16_t  PC16;  // Also Program Counter
    };

    uint8_t A; // Accumulator
    uint8_t X; // X Index register
    uint8_t Y; // Y Index register
    uint8_t S; // Stack Pointer

    union
    {
      uint8_t P; // Processor Flags/Status Register
      struct
      {
        uint8_t C : 1; // carry
        uint8_t Z : 1; // zero result
        uint8_t I : 1; // ignore interrupts
        uint8_t D : 1; // decimal mode
        uint8_t B : 1; // break
        uint8_t T : 1; // block transfer in progress
        uint8_t V : 1; // overflow
        uint8_t N : 1; // negative
      } pack flags; // Direct Access to Processor Flags/Status Register
    };

    uint8_t : 8; // makes regs_t exactly 64 bits
  };
  static_assert(sizeof(regs_t) == sizeof(uint64_t), "bad size of CPU registers struct"); // check that it is packed
}

#endif

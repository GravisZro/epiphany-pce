#ifndef HUC6260_REGISTERS_H
#define HUC6260_REGISTERS_H

#include <global.h>

namespace HuC6260
{
  struct rgb9_t
  {
    uint16_t blue   : 3;
    uint16_t red    : 3;
    uint16_t green  : 3;
    uint16_t        : 7;
  } pack;

  union color_t
  {
    uint16_t  value;
    highlow_t highlow;
    rgb9_t    rgb9;
  } pack;

  struct regs_t
  { //           [Name]        [ID]    [Description]
    union
    {
      highlow_t CR;         // 0400 - Control Register
      struct                //        Write Only
      {
        uint16_t DCC    : 2;  // Dot Clock Control
                              //  0 = 5.37 MHz
                              //  1 = 7.16 MHz
                              //  2 = 10.7 MHz
                              //  3 = 10.7 MHz
        uint16_t config : 1;	// Frame/Field Configuration
                              //  00 = 262-line frame
                              //  01 = 263-line frame
        uint16_t        : 4;
        uint16_t strip  : 1;  // Strip Colorburst
                              //  0 = Colorburst intact
                              //  1 = Strip colorburst
        uint16_t        : 8;
      } pack CR_bits;
    };

    union
    {
      uint16_t  CTA16;      // 0402 - Color Table Address
      highlow_t CTA;
    };

    color_t CTRW;           // 0404 - Color Table Write / Color Table Read
                            //        Write Only        / Read Only
  } pack;

  static_assert(sizeof(regs_t) == 3 * sizeof(uint16_t), "bad size of VCE registers struct");
}

#endif

#include "6280irqs.h"

#include <cassert>
#include <global.h>

namespace HuC6280
{
  IRQs::IRQs(void)
  {
    Reset();
  }

  void IRQs::Reset(void)
  {
    enabled = { 0, 0, 0 };
    active  = { 0, 0, 0 };
  }


  void IRQs::ProcessPorts(void)
  {
  }

  void IRQs::Read (IO::common_busses_t& busses)
  {
    if(ChipSelect(busses))
      switch(busses.address & 1)
      {
        case 0: // disable timer on read
          active.timer = 0;
          break;
        case 1: // return which IRQs are disabled (inverse of which are enabled)
          busses.data = to_u8(enabled) ^ 0x07;
      }
  }

  void IRQs::Write(const IO::common_busses_t& busses)
  {
    if(ChipSelect(busses))
      switch(busses.address & 1)
      {
        case 0:
          active.timer = 0; // disable timer
          break;
        case 1:
          to_u8(enabled) = (busses.data & 0x07) ^ 0x07; // set which IRQs are enabled (input is which IRQs are disabled)
      }
  }
}

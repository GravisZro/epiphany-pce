#include "6260core.h"


namespace HuC6260
{
  Core::Core(void)
  {
  }

  void Core::ExecutionLoop(void)
  {
    IO::Interface& interface = *fromend<IO::Interface>(0);
    for(;;)
    {
      // update ports
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
  }

  void Core::Reset(void)
  {
  }


  void Core::ProcessPorts(void)
  {

  }

  void Core::Read(IO::common_busses_t& busses)
  {
    if(ChipSelect(busses))
      switch(busses.data & 0x0007) // pins A0, A1, A2
      {
        case Addresses::CTR_LSB:
          busses.data = color_table_RAM[m_regs.CTA16].highlow.low;
          break;
        case Addresses::CTR_MSB:
          busses.data = color_table_RAM[m_regs.CTA16].highlow.high;
          ++m_regs.CTA16;
          break;
        default:
          busses.data = 0xFF; // reading most registers returns 0xFF
      }
  }

  void Core::Write(const IO::common_busses_t& busses)
  {
    if(ChipSelect(busses))
      switch(busses.data & 0x0007) // pins A0, A1, A2
      {
        case Addresses::CR:
          m_regs.CR.low = busses.data;
          break;
        case Addresses::CTA_LSB:
          m_regs.CTA.low = busses.data;
          break;
        case Addresses::CTA_MSB:
          m_regs.CTA.high = busses.data;
          break;
        case Addresses::CTW_LSB:
          color_table_RAM[m_regs.CTA16].highlow.low = busses.data;
          break;
        case Addresses::CTW_MSB:
          color_table_RAM[m_regs.CTA16].highlow.high = busses.data;
          ++m_regs.CTA16;
      }
  }
}

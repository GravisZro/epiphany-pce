#include "6270core.h"

namespace HuC6270
{
  static const uint16_t increment_widths[] = { 0x01, 0x20, 0x40, 0x80 };

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
      else
      {
        if(interface.CS)
        {
          if(interface.RD)
            Read(*interface.busses);
          else if(interface.WR)
            Write(*interface.busses);
        }
        //if(interface.HSYNC)

      }



    }
  }


/*/
 * NOTES:
 *    - address is HuC6270 input pins A0 and A1
/*/



  void Core::Reset(void)
  {
  }

  void Core::ProcessPorts(void)
  {

  }


  void Core::Read(IO::common_busses_t& busses)
  {
    if(ChipSelect(busses))
      switch(busses.address & 0x03) // only read A0 and A1
      {
        case Addresses::SR: // return status
          busses.data = m_SR;
          //CMemoryMap::SystemWrite(0xFF, 0x1400, 0x00); // clear HuC6280's IRQs
          break;

        case Addresses::LSB: // Least Significant Bit
          switch(m_AR)
          {
            // registers that get cleared when you read them
            case Registers::MAWR: m_regs.MAWR = 0; break;
            case Registers::MARR: m_regs.MARR = 0; break;

              // VRAM data is already in VRR
            case Registers::VRR: busses.data = m_regs.VRR.low;
          }
          break;

        case Addresses::MSB: // Most Significant Bit
          switch(m_AR)
          {
            // registers that get cleared when you read them
            case Registers::MAWR: m_regs.MAWR = 0; break;
            case Registers::MARR: m_regs.MARR = 0; break;

              // read VRAM at address MARR into VRR
            case Registers::VRR:
              m_regs.VRR16 = m_VRAM[m_regs.MARR];
              busses.data = m_regs.VRR.high;
              m_regs.MARR += increment_widths[m_regs.IW]; // increment by "increment width"
          }
          break;

        default:
          busses.data = 0x00; // default action; trying to reading most registers returns 0
          break;
      }
  }


  void Core::Write(const IO::common_busses_t& busses)
  {
    if(ChipSelect(busses))
      switch(busses.address & 0x03) // only read A0 and A1
      {
        case Addresses::AR: m_AR = busses.data; break; // write register number to use
        case Addresses::LSB: // Least Significant Bit
          switch(m_AR)
          {
            default: m_regnumber[m_AR].low = busses.data;
            case Registers::bad1:
            case Registers::bad2:
              break;
          }
          break;

        case Addresses::MSB: // Most Significant Bit
          switch(m_AR)
          {
            default: m_regnumber[m_AR].high = busses.data;
            case Registers::bad1:
            case Registers::bad2:
              break;

              // write data into VWR MSB
              // write VWR into VRAM address MAWR
            case Registers::MAWR:
              m_regs.VRR.high = busses.data;
              m_VRAM[m_regs.MAWR] = m_regs.VWR16;
              m_regs.MAWR += increment_widths[m_regs.IW]; // increment by "increment width"
          }
      }
  }
}

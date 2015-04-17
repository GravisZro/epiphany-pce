#include "businterface.h"

#include <e_lib.h>

//void e_irq_attach(e_irq_type_t irq, sighandler_t handler);

static BusInterface* s_handle = nullptr;

static void irq_handler(int) { s_handle->ProcessPorts(); }

BusInterface::BusInterface(void)
{
  s_handle = this;
  e_irq_attach(E_USER_INT, irq_handler);
  e_coreid_t id = e_get_coreid();

}


namespace IO
{
  ESelect chip_select(common_busses_t bus_interface)
  {

    uint32_t addr = bus_interface.physical_address;

    if(CDROM)
    {
      if(addr < ESelect::BIOS)
        return ESelect::BIOS;
      if(addr < ESelect::ArcadeIOPages)
        return ESelect::ArcadeIOPages;

      if(addr < 0x0D0000) // gap
        return ESelect::Error;

      if(addr < ESelect::WorkRAM192K)
        return ESelect::WorkRAM192K;
      if(addr < ESelect::WorkRAM64K)
        return ESelect::WorkRAM64K;
    }

    if(addr < ESelect::HuCard)
      return ESelect::HuCard;

    if(addr < 0x1EE000) // gap
      return ESelect::Error;

    if(addr < ESelect::GameSaveMemory)
      return ESelect::GameSaveMemory;

    if(addr < 0x1F0000) // gap
      return ESelect::Error;

    if(addr < ESelect::WorkRAM8K)
      return ESelect::WorkRAM8K; // 32KB if mirrored

    if(addr < 0x1FE000) // gap
      return ESelect::Error;

    if(addr < ESelect::VDC)
    {
      if(SuperGrafx)
      {
        if(addr & 0x001F < 0x0008)
          return ESelect::VDC1;
        if(addr & 0x001F < 0x0010)
          return ESelect::VPC;
        if(addr & 0x001F < 0x0018)
          return ESelect::VDC2;
        return ESelect::Error;
      }
      return ESelect::VDC;
    }

    if(addr < ESelect::VCE)
      return ESelect::VCE;
    if(addr < ESelect::PSG)
      return ESelect::PSG;
    if(addr < ESelect::Timer)
      return ESelect::Timer;
    if(addr < ESelect::GamepadPort)
      return ESelect::GamepadPort;
    if(addr < ESelect::IRQs)
      return ESelect::IRQs;
    if(addr < ESelect::IDLocations)
      return ESelect::IDLocations;

    if(addr < ESelect::MultiController)
      return ESelect::MultiController;

    return ESelect::Error;
  }
}

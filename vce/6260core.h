#ifndef HUC6260_CORE_H
#define HUC6260_CORE_H

#include <businterface.h>
#include "6260registers.h"

namespace HuC6260
{
  class Core : protected BusInterface
  {
  public:
    Core(void);

    void ExecutionLoop(void);

  private:
    inline addr_range_t AddressRange(void) const { return {IO::VDC, IO::VCE}; }
    void ProcessPorts(void);
    void Reset(void);
    void Read (      IO::common_busses_t& busses);
    void Write(const IO::common_busses_t& busses);

  protected:
    regs_t m_regs;
    color_t color_table_RAM[512];
  };

  namespace Addresses
  {
    enum : uint16_t
    {
      CR      = 0, // Write

      CTA_LSB = 2, // Write
      CTA_MSB = 3, // Write

      CTW_LSB = 4, // Write
      CTW_MSB = 5, // Write

      CTR_LSB = 4, // Read
      CTR_MSB = 5, // Read
    };
  }
}

#endif

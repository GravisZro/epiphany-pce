#ifndef HUC6280_IRQS_H
#define HUC6280_IRQS_H

#include <businterface.h>

// located in page 0xFF at 1FF400 - 1FF7FF

namespace HuC6280
{
  struct IRQSelector_t
  {
    uint8_t irq2  : 1;
    uint8_t irq1  : 1;
    uint8_t timer : 1;
    uint8_t       : 5;
  };

  enum : uint8_t
  {
    IQIRQ2  = 0x01, // bit 1
    IQIRQ1  = 0x02, // bit 2
    IQTIMER = 0x04, // bit 3
  };

  class IRQs : protected BusInterface
  {
  public:
    IRQs(void);
  private:
    void ExecutionLoop(void){}
    inline addr_range_t AddressRange(void) const { return {IO::GamepadPort, IO::IRQs}; }
    void ProcessPorts(void);
    void Reset(void);
    void Read (      IO::common_busses_t& busses);
    void Write(const IO::common_busses_t& busses);

    public:
    // interrupt registers
    IRQSelector_t enabled; // "disable" register
    IRQSelector_t active ; // "request" register (read only)
  };
}

#endif

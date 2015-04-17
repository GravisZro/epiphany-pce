#ifndef HUC6280_TIMER_H
#define HUC6280_TIMER_H

#include <businterface.h>

namespace HuC6280
{
  class Timer : protected BusInterface
  {
  public:
    void ExecutionLoop(void);

  private:
    void ProcessPorts(void);
    void Reset(void);
    void Read (      IO::common_busses_t& busses);
    void Write(const IO::common_busses_t& busses);

  public:
    Timer& operator --(void);
    bool   operator  !(void);

    bool isEnabled(void) const { return m_enabled; }

  private:
    uint32_t m_enabled       : 1 ; // enable/disable timer at 0x0C01 (disabled = 0, enabled = 1)
    uint32_t m_timer_counter : 7 ; // 7 bit counter at 0x0C00
    uint32_t m_cycle_counter : 10; // continuously counts down from and then resets to 1024
    uint32_t m_timer_latch   : 7 ; // memory copied when timer is disabled (0x0C01 = 0)
  };
}

#endif

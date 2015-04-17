#include "6280timer.h"

#include <cassert>

namespace HuC6280
{
  Timer::Timer(void)
  {
    static bool initialized = false;
    if(!initialized)
    {
      initialized = true;
      Reset();
    }
  }

  void Timer::Reset(void)
  {
    m_enabled       = 0; // enable/disable timer at 0x0C01 (disabled = 0, enabled = 1)
    m_timer_counter = 0;
    m_cycle_counter = 0;
    m_timer_latch   = 0;

    m_timer_counter -= 1; // set max value
    m_cycle_counter -= 1; // set max value
    m_timer_latch   -= 1; // set max value
  }

  Timer& Timer::operator --(void)
  {
    if(!--m_cycle_counter)
      --m_timer_counter;
    return *this;
  }

  bool Timer::operator !(void)
  {
    return !m_cycle_counter &&
           !m_timer_counter;
  }

  void Timer::ProcessPorts(void)
  {

  }

  void Timer::Read (IO::common_busses_t& busses)
  {
    if(ChipSelect(busses))
      busses.data = m_timer_counter;
  }


  void Timer::Write(const IO::common_busses_t& busses)
  {
    if(ChipSelect(busses))
    {
      if(!m_enabled && (busses.data & 0x01)) // if enabling
      {
        m_cycle_counter = 0;
        m_cycle_counter -= 1; // set max value
        m_timer_counter = m_timer_latch;
      }
      m_enabled = busses.data & 0x01;
    }
  }
}

#ifndef DEBUG_H
#define DEBUG_H
#if 0
#include <stdint.h>
#include "6280core.h"

namespace HuC6280
{
  struct State
  {
    regs_t regs;
    IRQs irqs;
    //Timer::IRQSelector_t irqs_enabled;
    //Timer::IRQSelector_t irqs_set;
  };

  class Debug : public Core
  {
    public:
      Debug(void);

      void VarDump(void);

      void LoadState(const State& s);
  };
}

#endif

#endif

#include "debug.h"
#if 0

#include <cassert>
#include <stdio.h>

namespace HuC6280
{
  static const uint8_t instruction_size_table[256] = // number of bytes per instruction
  {
    1, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 0, 3, 3, 3, 3, // 00-0F
    2, 2, 3, 2, 2, 2, 2, 2, 1, 3, 1, 0, 3, 3, 3, 3, // 10-1F
    3, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 0, 3, 3, 3, 3, // 20-2F
    2, 2, 3, 0, 2, 2, 2, 2, 1, 3, 0, 0, 3, 3, 3, 3, // 30-3F
    1, 2, 1, 2, 2, 2, 2, 2, 1, 2, 1, 0, 0, 3, 3, 3, // 40-4F
    2, 2, 3, 2, 1, 2, 2, 2, 1, 3, 1, 0, 3, 3, 3, 3, // 50-5F
    1, 2, 1, 0, 2, 2, 2, 2, 1, 2, 1, 0, 3, 3, 3, 3, // 60-6F
    2, 2, 3, 7, 2, 2, 2, 2, 1, 3, 1, 0, 3, 3, 3, 3, // 70-7F
    2, 2, 1, 3, 2, 2, 2, 2, 1, 2, 1, 0, 3, 3, 3, 3, // 80-8F
    2, 2, 3, 4, 2, 2, 2, 2, 1, 3, 1, 0, 3, 3, 3, 3, // 90-9F
    2, 2, 2, 3, 2, 2, 2, 2, 1, 2, 1, 0, 3, 3, 3, 3, // A0-AF
    2, 2, 3, 4, 2, 2, 2, 2, 1, 3, 1, 0, 3, 3, 3, 3, // B0-BF
    2, 2, 1, 7, 2, 2, 2, 2, 1, 2, 1, 0, 3, 3, 3, 3, // C0-CF
    2, 2, 3, 7, 1, 2, 2, 2, 1, 3, 1, 0, 0, 3, 3, 3, // D0-DF
    2, 2, 0, 7, 2, 2, 2, 2, 1, 2, 1, 0, 3, 3, 3, 3, // E0-EF
    2, 2, 3, 7, 1, 2, 2, 2, 1, 3, 1, 0, 0, 3, 3, 3  // F0-FF
  };

  Debug::Debug(void) : Core()
  {
    // safety first!
    m_mem_mappers[0] = 0;
    m_mem_mappers[1] = 0;
    m_mem_mappers[2] = 0;
    m_mem_mappers[3] = 0;
    m_mem_mappers[4] = 0;
    m_mem_mappers[5] = 0;
    m_mem_mappers[6] = 0;
  }

  void Debug::VarDump(void)
  {
    int op = SystemRead(m_regs.PC16);
    uint8_t buffer[8];

    int i;
    for(i = 0; i < instruction_size_table[op] - 1; ++i)
      buffer[i] = SystemRead(m_regs.PC16 + i + 1);

    printf("\n     Mine  ");
    printf("opcode: %02x  params:", op);

    for(i = 0; i < instruction_size_table[op] - 1; ++i)
      printf(" %02x", buffer[i]);

    while(i++ < 2) { printf("   "); }

    printf("  PC: %08x ", m_regs.PC16 + 1);
    printf(" X:%02x ", m_regs.X);
    printf(" A:%02x ", m_regs.A);
    printf(" Y:%02x ", m_regs.Y);
    printf(" S:%02x ", m_regs.S);

    printf(" P:%02x ", m_regs.P);
    printf(" P: ");
    printf("%c", m_regs.flags.N ? 'N' : '_');
    printf("%c", m_regs.flags.V ? 'V' : '_');
    printf("%c", m_regs.flags.T ? 'T' : '_');
    printf("%c", m_regs.flags.B ? 'B' : '_');
    printf("%c", m_regs.flags.D ? 'D' : '_');
    printf("%c", m_regs.flags.I ? 'I' : '_');
    printf("%c", m_regs.flags.Z ? 'Z' : '_');
    printf("%c", m_regs.flags.C ? 'C' : '_');

    printf(" | banks:");for(i = 0; i < 8; ++i) { printf(" %i:%02x", i, static_cast<uint8_t>(m_mem_mappers[i])); }

  //  /*
    //printf(" | timestamp: %04x ", Timestamp());

    printf(" | IRQs Enabled: ");
    printf("%c", m_reset ? 'R' : '_');
  //  printf("%c", m_irqs.enabled.reset ? 'R' : '_');
    printf("%c", m_irqs.enabled.timer ? 'T' : '_');
    printf("%c", m_irqs.enabled.irq1  ? '1' : '_');
    printf("%c", m_irqs.enabled.irq2  ? '2' : '_');

    printf(" | IRQs Set: ");
    printf("%c", m_reset ? 'R' : '_');
  //  printf("%c", m_irqs.set.reset ? 'R' : '_');
    printf("%c", m_irqs.active.timer ? 'T' : '_');
    printf("%c", m_irqs.active.irq1  ? '1' : '_');
    printf("%c", m_irqs.active.irq2  ? '2' : '_');

    //printf(" | IODataBuffer: %02x ", *m_IOBuffer);
    printf(" | speed: %c ", m_speed == 2 ? 'L' : 'H');
  /*
    printf(" | next_event: %06i ", m_timer_next_event);
    printf(" next_user_event: %06i ", m_timer_next_user_event);

    printf(" | timer  ");
    printf(" div: %06i ", v_0x0C00);
    printf(" value: %06i ", m_timer_value);
    printf(" load: %06i ", m_timer_load);
    printf(" status: %04x ", v_0x0C01);
    printf(" lastts: %06i ", m_timer_last_timestamp);
    printf(" in_reload: %s ", m_timer_reloading ? "yes" : "no");
   */
    fflush(stdout);
  }

  void Debug::LoadState(const State& s)
  {
    m_regs = s.regs;
    m_irqs = s.irqs;
  }
}

#endif

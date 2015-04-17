#ifndef HUC6270_CORE_H
#define HUC6270_CORE_H

#include <businterface.h>

#include "6270registers.h"

namespace HuC6270
{
  class Core : protected BusInterface
  {
  public:
    Core(void);

    void ExecutionLoop(void);

  private:
    inline addr_range_t AddressRange(void) const { return {0x1FEC00, IO::VDC}; }
    void ProcessPorts(void);
    void Reset(void);
    void Read (      IO::common_busses_t& busses);
    void Write(const IO::common_busses_t& busses);

  private:
    // NOTE: VRAM is isolated and not mapped to the system memory.
    uint16_t m_VRAM         [0xFFFF]; // 64KB VRAM (external ram)
    uint16_t m_SpriteBuffer [0x00FF]; // "Sprite Shift Register" (buffers the current sprite)
    uint16_t m_TileBuffer   [0x003F]; // "Background Shift Register" (buffers the current tile)

    uint8_t RN; // raster number
    uint16_t raster_settings;

    uint8_t m_AR : 5; // [Figure 3A] (5 bit) Address Register

    union
    {
      uint8_t m_SR;
      struct {
        uint8_t CR  : 1; // Collision Register (sprite collision)
        uint8_t OR  : 1; // Overflow Register (sprite overflow)
        uint8_t RR  : 1; // Raster Register (raster detect)
        uint8_t DS  : 1; // VRAM->VRAM DMA transfer status
        uint8_t DV  : 1; // VRAM->SATB DMA transfer status
        uint8_t VD  : 1; // Vertical retrace period
        uint8_t BSY : 1; // Busy
        uint8_t     : 1;
      } pack; // [Figure 3B] (7 bit) Status Register
    };

    union
    {
      highlow_t m_regnumber[20];
      registers_t m_regs;
    };

    // BAT = Background Attribute Table
    struct bat_entry_s // [Figure 5A]
    {
      struct // [Figure 5B]
      {
        uint16_t address    : 12; // bit 0-B: character code
        uint16_t pal_number :  4; // bit C-F: palette number }
      } pack data[32]; // 32 subentries (words) per BAT entry
    } pack;

    // SAT = Sprite Attribute Table
    struct sat_entry_s // Figure 6B (64 bits)
    {
      uint64_t x          : 10; // (10 bit) x coordinate
      uint64_t            :  6;
      uint64_t y          : 10; // (10 bit) y coordinate
      uint64_t            :  6;
      uint64_t address    : 11; // (11 bit) address of the sprite data (aka pattern)
      uint64_t            :  5;
      uint64_t pal_number :  4; // bit 0-3: palette number (0 to F)
      uint64_t            :  3;
      uint64_t priority   :  1; // bit 7: priority (above/below bg tiles)
      uint64_t width      :  1; // bit 8: width (16/32 pixels)
      uint64_t            :  2;
      uint64_t mirror_x   :  1; // bit B: flip x (horizonatally)
      uint64_t height     :  2; // bit C-D: height (16/32/NAN/64 pixels)
      uint64_t            :  1;
      uint64_t mirror_y   :  1; // bit F: flip y (vertically)
    } pack;

    bat_entry_s m_BAT[64];
    sat_entry_s m_SAT[64];
    uint16_t m_SG[0x1000]; // sprite generator (memory for 64 sprites)
  };


  namespace Addresses
  {
    enum : uint16_t       // HuC6270 input pins A0 and A1
    {
      SR  = 0, // Status Register  (Read)
      AR  = 0, // Address Register (Write)
      LSB = 2, // Least Significant bit (latch memory, no action taken when written to)
      MSB = 3, // Most Significant bit  (upon write, LSB and MSB are copied to register number stored at AR
    };
  }

  namespace Registers
  {
    enum : uint8_t
    {
      MAWR = 0, // Memory Address Write Register (VRAM Write Address)
      MARR = 1, // Memory Address Read Register (VRAM Read Address)
      VRR  = 2, // VRAM Data Read Register
      VWR  = 2, // VRAM Data Write Register
      bad1, // invalid register number
      bad2, // invalid register number
      CR  , // Control Register
      RCR , // Raster Compare Register
      BXR , // Background X-Scroll Register
      BYR , // Background Y-Scroll Register
      MWR , // Memory Width Register
      HPR , // Horizontal Period Register
      HDR , // Horizontal Display Register
      VSR , // Vertical Sync Register
      VDR , // Vertical Display Register
      VCR , // Vertical Display Position End Register
      DCR , // DMA Control Register
      SOUR, // DMA Source Register
      DESR, // DMA Destination Register
      LENR, // DMA Length Register
      SATB, // Sprite Attribute Table Address Register
    };
  }
}

#endif

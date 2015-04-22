#ifndef BUSINTERFACE_H
#define BUSINTERFACE_H

#include <stdint.h>
#include <global.h>

const bool CDROM = false;
const bool SuperGrafx = false;

namespace IO
{
  enum class ESignal : uint8_t
  {
    Reset,
    Read ,
    Write,
  };

  enum ESelect : const uint32_t
  {
    BIOS            = 0x080000, // System Card
    ArcadeIOPages   = 0x088000, // Arcade Card
    HuCard          = 0x100000, // HuCard games
    WorkRAM192K     = 0x100000, // System Card 3.x / Arcade Card
    WorkRAM64K      = 0x110000, // CDROM addon
    GameSaveMemory  = 0x1EE800, // CDROM/Booster+
    WorkRAM8K       = 0x1F8000, // all systems
    WorkRAM32K      = 0x1FE000, // SuperGrafx
    //gap           = 0x1FE000,
    VDC             = 0x1FE400, // normal system
    VDC1            = 0x1FE008, // SuperGrafx
    VPC             = 0x1FE010, // SuperGrafx
    VDC2            = 0x1FE018, // SuperGrafx
    VCE             = 0x1FE800, // all systems
    PSG             = 0x1FEC00, // all systems
    Timer           = 0x1FF000, // all systems
    GamepadPort     = 0x1FF400, // all systems
    IRQs            = 0x1FF800, // all systems
    IDLocations     = 0x1FFA00, // System Card 3.x / Arcade Card
    MultiController = 0x1FFC00, // CDROM/Booster+
    ArcadeIOPorts   = 0x1FFC00, // Arcade Card
    Error           = 0x200000,
  };

  struct common_busses_t
  {
    //select_t chip        ; // chip select
    union
    {
      struct
      {
        uint32_t address : 13; // addressing for 8KB page
        uint32_t page    : 8 ; // page select
      };
      struct
      {
        uint32_t physical_address : 21;
      };
    };
    uint8_t data        ; // data from the data bus
  };

  struct Interface
  {
    // Common
    uint8_t CS    : 1; // Chip Select (enables Address Bus and Data Bus)
    uint8_t RESET : 1; // Chip reset
    uint8_t RD    : 1; // Read Active
    uint8_t WR    : 1; // Write Active
//  uint8_t CK    : 1; // Clock

    IO::common_busses_t* busses; // CPU <-> Everything

    // VDC/VCE only
    uint8_t* VRAMbus  ; // VDC <-> VCE
    uint8_t  HSYNC : 1; // Horizontal (display) Sync
    uint8_t  VSYNC : 1; // Vertical   (display) Sync
    uint8_t  SPBG  : 1; // 0 = background  1 = sprite

    // VDC only
    uint8_t  BSY   : 1; // busy signal
  }; // do not pack!

  ESelect chip_select(common_busses_t bus_interface);
}

struct addr_range_t
{
  uint32_t minimum : 21;
  uint32_t maximum : 21;
  bool operator ==(const uint32_t& other) const { return other >= minimum && other < maximum; }
};

class BusInterface
{
#if 0
public:
  virtual void Reset       (void) = 0;

  virtual addr_range_t AddressRange(void) const = 0;
  virtual void Read       (      IO::common_busses_t& busses) = 0;
  virtual void Write      (const IO::common_busses_t& busses) = 0;
#endif
};

#endif // BUSINTERFACE_H

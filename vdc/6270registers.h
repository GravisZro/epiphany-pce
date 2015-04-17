#ifndef HUC6270_REGISTERS_H
#define HUC6270_REGISTERS_H

#include <global.h>

namespace HuC6270
{
  struct registers_t
  {
    //       [Name]        [ID][Figure ID] [Description]
    uint16_t MAWR;      // 00 [Figure 3C] Memory Address Write Register (VRAM address)
    uint16_t MARR;      // 01 [Figure 3D] Memory Address Read Register  (VRAM address)

    union
    {
      uint16_t  VWR16;  // 02 [Figure 3E] VRAM Data Write Register / VRAM Write Latch (VRAM data)
      highlow_t VWR;

      uint16_t  VRR16;  // 02 [Figure 3F] VRAM Data Read Register  / VRAM Read Buffer (VRAM data)
      highlow_t VRR;
    };

    uint16_t : 16;      // inaccessible
    uint16_t : 16;      // inaccessible

    union
    {
      uint16_t CR;      // 05 [Figure 3G] (13 bit) Control Register
                        //                (0-3 IE) (4-5 EX) (6 SB) (7 BB) (8-9 TE) (A DR) (B-C IW)
      struct
      {
        uint16_t IE : 4;  // interrupt enable (0x0001, 0x0002, 0x0004, 0x0008)
                          //   (1.1) bit 0 - collision detection of sprites
                          //   (1.2) bit 1 - excess number detection of sprites
                          //   (1.3) bit 2 - raster detection
                          //   (1.4) bit 3 - detection of vertical retrace period

        uint16_t EX : 2;  // external sync (0x0010, 0x0020)
                          //               bit4 : Horizontal
                          //               bit5 : Vertical
                          // bits 5 and 4 (EX)
                          //      0     0      - vsync and hsync are inputs and synchronous to external signals
                          //      0     1      - vsync is input and synchronous to external signals while hsync is an output
                          //      1     0      - unused
                          //      1     1      - vsync and hsync are outputs

        uint16_t SB : 1;  //
                          // bit 6 (SB) - sprite blanking (show/hide sprites) (0x0040)
                          //  It is decided whether a sprite should be displayed on a screen or not.
                          //  The control of the bit is effective in the following horizontal display period.
                          //  (3.1) "0" - do not display sprites
                          //  (3.2) "1" - display sprites

        uint16_t BB : 1;  // bit 7 (BB) - background blanking (show/hide background) (0x0080)
                          //  It is decided whether background should be displayed on a screen or not.
                          //  The control of the bit is effective in the following horizontal display period.
                          //  (4.1) "0" - do not display background
                          //  (4.2) "1" - display background

        uint16_t TE : 2;  // bits 9 and 8 (TE) - selection of DISP terminal outputs (0x0100, 0x0200)
                          //      0     0      - DISP     - output "H" during display
                          //      0     1      - BURST    - color burst inserting position is indicated by output "L"
                          //      1     0      - INTHSYNC - internal horizontal sync signal
                          //      1     1      - unused

        uint16_t DR : 1;  // bit 10 (DR) - dynamic RAM refresh (0x0400)
                          //  Refresh address is supplied from the terminals MA0 to MA15 upon the setting of the
                          //  bit in a case where VRAM dot with is of 2 dots or 4 dots for background in a memory
                          //  width register as showing in Figure 3K.

        uint16_t IW : 2;  // bits 12 and 11 (IW) - increment width selection of Memory Address Write Register (MAWR) or Memory Address Read Register (MARR) (0x0400, 0x0800)
                          //       0      0      - 0x01
                          //       0      1      - 0x20
                          //       1      0      - 0x40
                          //       1      1      - 0x80
                          // In a case of 8 bit access, an address is incremented upon the upper byte.

        uint16_t    : 3;

      } pack;   // 05 [Figure 3G] (13 bit) Control Register
    };

    uint16_t RCR : 10;  // 06 [Figure 3H] (10 bit) Raster Counter Register aka Raster Detecting Register
    uint16_t     :  6;
    uint16_t BGX : 10;  // 07 [Figure 3I] (BXR) (10 bit) Background X-Scroll Register
    uint16_t     :  6;
    uint16_t BGY : 10;  // 08 [Figure 3J] (BYR) (10 bit) Background Y-Scroll Register
    uint16_t     :  6;

    union
    {
      uint16_t MWR ;    // 09 [Figure 3K] (8 bit) Memory Width Register
                        //                (0-1 VM) (2-3 SM) (4-6 SCREEN) (7 CM)
      struct
      {
        uint16_t VM     : 2; // VRAM dot width
        uint16_t SM     : 2; // sprite dot width
        uint16_t SCREEN : 3; // sprite size
        uint16_t CM     : 1; // character generator mode
        uint16_t        : 8;
      } pack;  // 09 [Figure 3K] (8 bit) Memory Width Register
    };

// Video/Sync registers
    union
    {
      uint16_t HSR ;    // 0A [Figure 3L] (15 bit) Horizontal Synchronous Register
                        //                (0-4 HSW) (5-7 blank?) (A-E HDS)
      struct
      {
        uint8_t HSW : 5;  // horizontal sync pulse width
        uint8_t     : 3;
        uint8_t HDS : 7;  // horizontal display starting position
        uint8_t     : 1;
      } pack;  // 0A [Figure 3L] (HSR) (15 bit) Horizontal Synchronous Register
    };

    union
    {
      uint16_t HDR ;    // 0B [Figure 3M] (14 bit) Horizontal Display Register
                        //                (0-6 HDW) (8-E HDE)
      struct
      {
        uint8_t HDW : 7;  // horizontal display width
        uint8_t     : 1;
        uint8_t HDE : 7;  // horizontal display ending position
        uint8_t     : 1;
      } pack;  // 0B [Figure 3M] (14 bit) Horizontal Display Register
    };

    union
    {
      uint16_t VSR;     // 0C [Figure 3N] (13 bit) Vertical Sync Register
                        //                (0-4 VSW) (8-F VDS)
      struct
      {
        uint16_t VSW : 5; // vertical sync pulse width
        uint16_t     : 3;
        uint16_t VDS : 8; // vertical display starting position
      } pack;  // 0C [Figure 3N] (13 bit) Vertical Sync Register
    };

    uint16_t VDW : 9;   // 0D [Figure 3O] (VDW / VDR) (9 bit) Vertical Display Register / Vertical Display Width
    uint16_t     : 7;
    uint16_t VCR : 8;   // 0E [Figure 3P] (VCR / VDE) (8 bit) Vertical Display End Position Register
    uint16_t     : 8;

// DMA registers

    union
    {
      uint16_t DCR ;    // 0F [Figure 3Q] (5 bit) DMA Control Register
                        //                (0 DSC) (1 DVC) (2 SI/D) (3 DI/D) (4 DSR)
      struct
      { // 0 = disabled  1 = enabled
        uint16_t DSC : 1; // bit 0 (DSC) - Enable an interruption at the finishing of transfer between the VRAM (7) and Sprite Attribute Table buffer (23).
                          //               It is decided whether or not an interruption is enabled at the finishing time of the transfer.
                          //  between VRAM and SATB

        uint16_t DVC : 1; // bit 1 (DVC) - Enable of interruption at the finishing of transfer between two regions of the VRAM (7).
                          //               It is decided whether or not an interruption is enabled finishing time of the transfer.
                          // between VRAM regions

        uint16_t SI  : 1; // bit 2 (SI/D) - Increment/decrement of a source address
                          //                One of automatical increment and decrement of a source address is selected in a transfer between two regions of VRAM (7).
                          // source address increment/decrement

        uint16_t DI  : 1; // bit 3 (DI/D) - Increment/decrement of a destination address
                          //                One of automatical increment and decrement of a destination address is selected in a transfer between two regions of VRAM (7).
                          // destination address increment/decrement

        uint16_t DSR : 1; // bit 4 (DSR) - Repetition of a transfer between the VRAM (7) and the sprite attribute table buffer (23) is enabled.
                          // destination address increment/decrement
                          // transfer repetition

        uint16_t     : 11;
      } pack;  // 0F [Figure 3Q] (5 bit) DMA Control Register
    };

    uint16_t SOUR;      // 10 [Figure 3R] DMA Source Address Register
    uint16_t DESR;      // 11 [Figure 3S] DMA Destination Address Register
    uint16_t LENR;      // 12 [Figure 3T] DMA Block Length Register
    uint16_t DVSSR;     // 13 [Figure 3U] DMA VRAM-SAT Source Address Register
  } pack;

  static_assert(sizeof(registers_t) == 20 * sizeof(uint16_t), "bad size of VDC registers struct");
}

#endif

#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>


static const char* const framebuffer_device = "/dev/fb0";
static const uint32_t buffer_offset = 0x01000000;

struct coords_t
{
  uint32_t x;
  uint32_t y;
} __attribute__((packed));

struct fbinfo_t
{
  uint32_t memory_address;
  uint32_t memory_size;
  uint32_t bits_per_pixel;
  uint32_t bytes_per_line;

  coords_t resolution;
  coords_t virt_resolution;
  coords_t virt_offset;
} __attribute__((packed, aligned(8)));

struct rgb_t
{
  uint8_t red;
  uint8_t green;
  uint8_t blue;
} __attribute__((packed));



class HFrameBuffer
{
public:
  HFrameBuffer(void);
 ~HFrameBuffer(void);

private:
  static fbinfo_t m_specifications;

private:
  HFrameBuffer(void* a);
  static HFrameBuffer s_instance;

  friend class HCommunication;
};

#endif // FRAMEBUFFER_H

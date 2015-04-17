#include "framebuffer.h"

// linux includes
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <linux/fb.h>

// low level functionality
#include <unistd.h>
#include <fcntl.h>

// legacy libs
#include <cstdlib>
#include <cassert>


HFrameBuffer HFrameBuffer::s_instance(nullptr);
fbinfo_t HFrameBuffer::m_specifications;

HFrameBuffer::HFrameBuffer(void* a) // private contructor for singleton
{
  return;
  fb_fix_screeninfo fbfsi;
  fb_var_screeninfo fbvsi;

  int device_id = open(framebuffer_device, O_RDWR);
  assert(device_id > 0);
  assert(!ioctl(device_id, FBIOGET_FSCREENINFO, &fbfsi));
  assert(!ioctl(device_id, FBIOGET_VSCREENINFO, &fbvsi));
  close(device_id);

  assert(fbvsi.bits_per_pixel == 24);

  m_specifications.memory_address = fbfsi.smem_start;
  m_specifications.memory_size = fbfsi.smem_len;
  m_specifications.bits_per_pixel = fbvsi.bits_per_pixel;
  m_specifications.bytes_per_line = fbfsi.line_length;

  m_specifications.resolution      = { fbvsi.xres        , fbvsi.yres         };
  m_specifications.virt_resolution = { fbvsi.xres_virtual, fbvsi.yres_virtual };
  m_specifications.virt_offset     = { fbvsi.xoffset     , fbvsi.yoffset      };
}

HFrameBuffer::HFrameBuffer(void)
{
}

HFrameBuffer::~HFrameBuffer(void)
{

}


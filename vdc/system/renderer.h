#ifndef RENDER_H
#define RENDER_H

#include <global.h>
#include "framebuffer.h"


class Renderer
{
public:
  Renderer(void);

  void redraw(void);

private:
  rgb_t* m_FrameBuffer;
  rgb_t* m_DoubleBuffer;
};

#endif // RENDER_H

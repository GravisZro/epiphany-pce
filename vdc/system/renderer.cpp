#include "renderer.h"

Renderer::Renderer(void)
{
  //static_assert(sizeof(uint8_t*) == sizeof(uint32_t), "incorrect pointer size.  needs to be 32 bit");
}

void Renderer::redraw(void)
{
/*
    if(frame_count % frame_skip == 0)
    {
        if(bitmap.viewport.changed)
        {
            static int old_width = 320;

            if((old_width <= 320) && (option.autores) && (bitmap.viewport.w > 320))
            {
                old_width = bitmap.viewport.w;
                change_display(option.autores_w, option.autores_h);
            }

            center_x = (SCREEN_W - bitmap.viewport.w) / 2;
            center_y = (SCREEN_H / (option.scanlines ? 4 : 2)) - (bitmap.viewport.h / 2);
            bitmap.viewport.changed = 0;
            clear(screen);
        }

                blit(bmp, screen,
                    bitmap.viewport.x, bitmap.viewport.y,
                    center_x, center_y,
                    bitmap.viewport.w, bitmap.viewport.h);
    }
*/
}

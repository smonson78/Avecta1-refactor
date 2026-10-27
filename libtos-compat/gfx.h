#ifndef __GFX_H
#define __GFX_H

#include <stdint.h>

void plotline(int x0, int y0, int x1, int y1, uint32_t colour);

void filled_rect(int x, int y, int w, int h, int colour);
void pattern_rect(int x, int y, int w, int h, uint16_t *bitmap, int colour);
void copy_rect(int x, int y, int w, int h, int dx, int dy);
void render_text_char(int fg_colour, int bg_colour, uint16_t *base, int srcx, int srcy, int dstx, int dsty, 
  int width, int height, int fontwidth, int wrt_mode);

#endif
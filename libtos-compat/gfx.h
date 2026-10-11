#ifndef __GFX_H
#define __GFX_H

#include <stdint.h>

void filled_rect(int x, int y, int w, int h, int colour);
void pattern_rect(int x, int y, int w, int h, uint16_t *bitmap);
void render_text_char(int fg_colour, int bg_colour, uint16_t *base, int srcx, int srcy, int dstx, int dsty, 
  int width, int height, int fontwidth, int wrt_mode);

#endif
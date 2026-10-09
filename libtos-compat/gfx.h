#ifndef __GFX_H
#define __GFX_H

void filled_rect(int x, int y, int w, int h, int colour);
void pattern_rect(int x, int y, int w, int h, uint16_t *bitmap);
void render_text_char(int fg_colour, int bg_colour, char c, int x, int y, int width, int height, TTF_Font *font);

#endif
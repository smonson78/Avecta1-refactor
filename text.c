#include <stdint.h>
#include "gemdefs.h"

/****************************************************************************
* textsix(FLAG,X,Y,LENGTH,STRING) is a call to output the string whose 
* starting address is (LONG) STRING, and whose length is (INT) LENGTH at the
* screen location (INT) X,Y.  If (INT) FLAG is set then the text will be output
* in inversed mode, background color 1 (black), foreground color 0 (white).
*****************************************************************************/

void textsix(int16_t flag, int16_t x, int16_t y, int16_t length, const char *text)
{
  LINEA *parameter_block; //a0
  FONT_HDR **sysfont_pointers; //a1

  linea_init(&parameter_block, &sysfont_pointers);

  const char *a5 = text;

  FONT_HDR *font = sysfont_pointers[0];

  parameter_block->fbase = font->dat_table;
  parameter_block->fwidth = font->form_width;
  parameter_block->delx = font->max_cell_width;
  parameter_block->dely = font->form_height;
  parameter_block->desty = y;
  parameter_block->scale = 0;
  parameter_block->t_sclsts = 1;
  parameter_block->xacc_dda = 0x8000;
  // Always use colour 1, i.e. red
  parameter_block->text_fg = 1;

  if (flag == 1) {
    // Set XOR mode if flag = 1
    parameter_block->wrt_mode = 2;
  } else {
    // Set REPLACE mode if flag = 0
    parameter_block->wrt_mode = 0;
  }

  while (length != 0) {
    parameter_block->destx = x;
    x += 6;
    uint32_t d6 = *a5 * 6;
    a5++;
    length--;
    parameter_block->sourcex = d6;

    linea_textblock_transfer();
  }
}
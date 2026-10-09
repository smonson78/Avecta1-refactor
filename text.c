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

  //   move.l 16(R14),a5    * a5 now holds the address of the string
  const char *a5 = text;

  //   movea.l (a1),a3       * a3 holds first fontheader address
  FONT_HDR *font = sysfont_pointers[0];

  //   move.l 76(a3),84(a4)  * move font data address into line A 
  parameter_block->fbase = font->dat_table;

  //   move.w 80(a3),88(a4)  * move font width value
  parameter_block->fwidth = font->form_width;

  //   move.w 52(a3),80(a4)  
  parameter_block->delx = font->max_cell_width;

  //   move.w 82(a3),82(a4)
  parameter_block->dely = font->form_height;

  //   move.w 12(R14),78(a4)  * select screen y-loc 8
  parameter_block->desty = y;

  //   move.w #1,102(a4)
  parameter_block->scale = 1;

  //   move.w #1,68(a4)       * set yet another scaling flag
  parameter_block->t_sclsts = 1;

  //   move.w $8000,64(a4)    * must be set for a textblt ?
  parameter_block->xacc_dda = 0x8000;

  //   move.w #1,106(a4)      * When running this is black
  parameter_block->text_fg = 1;

  if (flag == 1) {
    // Set XOR mode if flag = 1
    parameter_block->wrt_mode = 2;
  } else {
    // Set REPLACE mode if flag = 0
    parameter_block->wrt_mode = 0;
  }

  while (length != 0) {
    //   move.w 10(R14),76(a4)  * select screen x-loc 8
    parameter_block->destx = x;
    //   addq #6,10(R14)
    x += 6;
    //   clr.l d6
    uint32_t d6 = 0;
    //   move.b (a5),d6
    d6 = *a5;
    //   mulu #6,d6
    d6 *= 6;
    //   addq #1,a5
    a5++;
    //   subq #1,d3
    length--;
    //   move.w d6,72(a4)
    parameter_block->sourcex = d6;

    linea_textblock_transfer();
  }
}
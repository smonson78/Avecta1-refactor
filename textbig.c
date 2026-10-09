#include <stdint.h>
#include "gemdefs.h"
// Initalise LINE-A aka linea_init()

// These line-a structures should be in libtos

// On stack:
// --- 0:  return address
// --- 4:  reserved 4 bytes from link
// --- 8:  s
// --- 10: x
// --- 12: y
void textbig(const char s, int x, int y)
{
  LINEA *parameter_block;
  FONT_HDR **sysfont_pointers;

  linea_init(&parameter_block, &sysfont_pointers);

  // movea.l 8(a1),a3      * a3 holds third fontheader address
  FONT_HDR *font = sysfont_pointers[2];

  // move.l 76(a3),84(a0)  * move font data address into line A 
  parameter_block->fbase = font->dat_table;
  
  // move.w 80(a3),88(a0)  * move font width value
  parameter_block->fwidth = font->form_width;

  // move.w 52(a3),80(a0)  
  parameter_block->delx = font->max_cell_width;

  // move.w 82(a3),82(a0)  
  parameter_block->dely = font->form_height;

  // move.w d0,72(a0)      * select ascii value in d0 (times 8 pixels)
  parameter_block->sourcex = s * 8;

  // move.w 10(R14),76(a0) * select screen x-loc 8
  parameter_block->destx = x;
  // move.w 12(R14),78(a0) * select screen y-loc 8
  parameter_block->desty = y;

  // move.w #1,102(a0)
  parameter_block->scale = 1;

  // move.w #1,68(a0)      * set yet another scaling flag
  parameter_block->t_sclsts = 1;

  // move.w $8000,64(a0)   * must be set for a textblt ?
  parameter_block->xacc_dda = 0x8000;

  // move.w #254,d3
  // mulu #256,d3
  // move.w d3,66(a0)
  parameter_block->dda_inc = (0xfe << 8); // "scaling increment"

  // move.w #1,90(a0)      * Thickened text
  parameter_block->style = 1;

  // move.w #1,106(a0)     * Set text color to be red
  parameter_block->text_fg = 1;

  // move.w #0,36(a0)      * Set replace mode
  parameter_block->wrt_mode = 0;

  // LINE-A text block transfer
  // __asm__ __volatile__
  // (
  //   ".short 0xa008\n\t"      // Call Line-A initialisation
  // : /* outputs */
  // : /* inputs */
  // : "d0", "d1", "d2", "a0", "a1", "a2" /* clobbered regs */
  // );
  linea_textblock_transfer();

}
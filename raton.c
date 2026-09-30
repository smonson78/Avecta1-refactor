#include "gemdefs.h"

/*
.globl _raton
.text
_raton:    link R14,#-4
           .dc.w $a000
           move.l 4(a0),a3
           move.l 8(a0),a4
           move.w #1,6(a3)
           move.w #0,2(a3)
           move.w #1,(a4)
           .dc.w $a009
           unlk R14
           rts
*/

// Enable mouse, get it, rat on?
void raton() {
  LINEA *parameter_block; //a0
  FONT_HDR **sysfont_pointers; //a1

  linea_init(&parameter_block, &sysfont_pointers);

  // These two may not do anything for all I know
  // parameter_block->contrl[3] = 1;
  // parameter_block->contrl[1] = 0;

  parameter_block->intin[0] = 1; // "flag"

  linea_showmouse();
  
}
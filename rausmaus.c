#include "gemdefs.h"

/*

.globl _rausmaus
.text
_rausmaus: link R14,#-4
           .dc.w $a000
           .dc.w $a00a
           unlk R14
           rts
*/

// "Out mouse" in German?
// disables the mouse
void rausmaus() {

  LINEA *parameter_block; //a0
  FONT_HDR **sysfont_pointers; //a1

  linea_init(&parameter_block, &sysfont_pointers);
  linea_showmouse();
}
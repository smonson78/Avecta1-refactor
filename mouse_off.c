#include "gemdefs.h"

/*

.globl _mouse_off
.text
_mouse_off: link R14,#-4
           .dc.w $a000
           .dc.w $a00a
           unlk R14
           rts
*/

// "Out mouse" in German?
// disables the mouse
void mouse_off() {

  LINEA *parameter_block;
  FONT_HDR **sysfont_pointers;

  linea_init(&parameter_block, &sysfont_pointers);
  linea_hidemouse();
}
#include "gemdefs.h"

// disables the mouse
void mouse_off() {

  LINEA *parameter_block;
  FONT_HDR **sysfont_pointers;

  linea_init(&parameter_block, &sysfont_pointers);
  linea_hidemouse();
}
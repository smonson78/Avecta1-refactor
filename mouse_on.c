#include "gemdefs.h"

// Enable mouse
void mouse_on() {
  LINEA *parameter_block; //a0
  FONT_HDR **sysfont_pointers; //a1

  linea_init(&parameter_block, &sysfont_pointers);

  parameter_block->intin[0] = 1;
  linea_showmouse();
}
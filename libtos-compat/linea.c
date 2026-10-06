#include "linea.h"

LINEA linea_parameter_block;
FONT_HDR system_fonts[3];
FONT_HDR *system_font_pointers[] = {
  &system_fonts[0],
  &system_fonts[1],
  &system_fonts[2],
  0,
};

// LINE-A initialisation (get parameter blocks)
void linea_init(LINEA **parameter_block, FONT_HDR ***sysfont_pointers) {
  *parameter_block = &linea_parameter_block;
  *sysfont_pointers = system_font_pointers;
}

void linea_textblock_transfer() {
}

void linea_showmouse() {
}

void linea_hidemouse() {
}
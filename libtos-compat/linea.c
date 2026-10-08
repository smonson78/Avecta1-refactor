#include <stdio.h>

#include "linea.h"

int16_t linea_intin[128];
int16_t linea_intout[128];
int16_t linea_contrl[12];
int16_t linea_ptsin[128];
int16_t linea_ptsout[12];

LINEA linea_parameter_block;

FONT_HDR system_fonts[3];

FONT_HDR *system_font_pointers[] = {
  &system_fonts[0],
  &system_fonts[1],
  &system_fonts[2],
  0,
};

void linea_setup() {
  linea_parameter_block.intin = linea_intin;
  linea_parameter_block.intout = linea_intout;
  linea_parameter_block.contrl = linea_contrl;
  linea_parameter_block.ptsin = linea_ptsin;
  linea_parameter_block.ptsout = linea_ptsout;
}

// LINE-A initialisation (get parameter blocks)
void linea_init(LINEA **parameter_block, FONT_HDR ***sysfont_pointers) {
  // printf("linea_init()\n");
  *parameter_block = &linea_parameter_block;
  *sysfont_pointers = system_font_pointers;
}

void linea_textblock_transfer() {
}

void linea_showmouse() {
  // These are mostly used to avoid screen corruption so we probably don't need to even bother
  //printf("linea_showmouse()\n");
}

void linea_hidemouse() {
  //printf("linea_hidemouse()\n");
}
#include <stdio.h>
#include "st_globals.h"
#include "linea.h"
#include "gfx.h"
#include "compat.h"

int16_t linea_intin[128];
int16_t linea_intout[128];
int16_t linea_contrl[12];
int16_t linea_ptsin[128];
int16_t linea_ptsout[12];

extern FONT_HDR_ROM _binary_font0_bin_start;
extern FONT_HDR_ROM _binary_font1_bin_start;
extern FONT_HDR_ROM _binary_font2_bin_start;

FONT_HDR sysfonts[3];

LINEA linea_parameter_block;
FONT_HDR *system_font_pointers[4];

void convert_font_hdr(FONT_HDR_ROM *src, FONT_HDR *dst) {
  memcpy(dst, src, 70); // Everything up to the pointers
  dst->form_width = src->form_width;
  dst->form_height = src->form_height;
  dst->next_font = 0;

  endianness_fix(&dst->font_id, 1, 16);
  endianness_fix(&dst->point, 1, 16);
  endianness_fix(&dst->first_ade, 1, 16);
  endianness_fix(&dst->last_ade, 1, 16);
  endianness_fix(&dst->top, 1, 16);
  endianness_fix(&dst->ascent, 1, 16);
  endianness_fix(&dst->half, 1, 16);
  endianness_fix(&dst->descent, 1, 16);
  endianness_fix(&dst->bottom, 1, 16);
  endianness_fix(&dst->max_char_width, 1, 16);
  endianness_fix(&dst->max_cell_width, 1, 16);
  endianness_fix(&dst->left_offset, 1, 16);
  endianness_fix(&dst->right_offset, 1, 16);
  endianness_fix(&dst->thicken, 1, 16);
  endianness_fix(&dst->ul_size, 1, 16);
  endianness_fix(&dst->lighten, 1, 16);
  endianness_fix(&dst->skew, 1, 16);
  endianness_fix(&dst->flags, 1, 16);
  endianness_fix(&dst->form_width, 1, 16);
  endianness_fix(&dst->form_height, 1, 16);

  int codepoints = dst->last_ade - dst->first_ade;
  
  uint8_t *next = (uint8_t *)src + sizeof(FONT_HDR);

  if (src->hor_table) {
    dst->hor_table = next;
    next += codepoints;
  } else {
    dst->hor_table = 0;
  }

  if (src->off_table) {
    dst->off_table = (uint16_t *)next;
    next += 2 * codepoints;
  } else {
    dst->off_table = 0;
  }

  dst->dat_table = (uint16_t *)next;
}

void linea_setup() {
  linea_parameter_block.intin = linea_intin;
  linea_parameter_block.intout = linea_intout;
  linea_parameter_block.contrl = linea_contrl;
  linea_parameter_block.ptsin = linea_ptsin;
  linea_parameter_block.ptsout = linea_ptsout;

  system_font_pointers[0] = &sysfonts[0];
  system_font_pointers[1] = &sysfonts[1];
  system_font_pointers[2] = &sysfonts[2];
  system_font_pointers[3] = 0;

  convert_font_hdr(&_binary_font0_bin_start, &sysfonts[0]);
  convert_font_hdr(&_binary_font1_bin_start, &sysfonts[1]);
  convert_font_hdr(&_binary_font2_bin_start, &sysfonts[2]);

  for (int i = 0; i < 3; i++) {
    FONT_HDR *p = system_font_pointers[i];

    printf("Font %d:\n", i);
    printf("  Name: %32s\n", &p->name);
    printf("  Codepoints: %d-%d\n", p->first_ade, p->last_ade);
    printf("  Horizontal offset table: %p\n", p->hor_table);
    printf("  Character offset table: %p\n", p->hor_table);
    printf("  Form size: %dx%d\n", p->form_width, p->form_height);
    printf("  Data: %p (base + %d)\n", p->dat_table, (uint8_t *)(p->dat_table) - (uint8_t *)p);
  }
}

// LINE-A initialisation (get parameter blocks)
void linea_init(LINEA **parameter_block, FONT_HDR ***sysfont_pointers) {
  *parameter_block = &linea_parameter_block;
  *sysfont_pointers = system_font_pointers;
}

void linea_textblock_transfer() {
  printf("linea_textblock_transfer()\n");

  render_text_char(
    linea_parameter_block.text_fg,
    linea_parameter_block.text_bg,
    linea_parameter_block.sourcex / linea_parameter_block.delx,
    linea_parameter_block.destx, 
    linea_parameter_block.desty, 
    linea_parameter_block.delx, 
    linea_parameter_block.dely, 
    globals.video.small_font);
}

void linea_showmouse() {
  // These are mostly used to avoid screen corruption so we probably don't need to even bother
  //printf("linea_showmouse()\n");
}

void linea_hidemouse() {
  //printf("linea_hidemouse()\n");
}
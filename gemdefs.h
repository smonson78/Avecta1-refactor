#ifndef __GEMDEFS_H
#define __GEMDEFS_H

#include <stdint.h>

#include <libc.h>
#include <gemdos.h>
#include <xbios.h>
#include <aes.h>
#include <linea.h>

extern FONT_HDR *binary_font0_bin_start;
extern FONT_HDR *binary_font1_bin_start;
extern FONT_HDR *binary_font2_bin_start;

// Not in my library yet, but needed:
void vbl_animation_off();
void vbl_animation_on();
void save_palette();

void linea_init(LINEA **parameter_block, FONT_HDR ***sysfont_pointers);
void linea_textblock_transfer();
void linea_showmouse();
void linea_hidemouse();

#endif
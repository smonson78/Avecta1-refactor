#ifndef __GEMDEFS_H
#define __GEMDEFS_H

#include <stdint.h>

#include <gemdos.h>
#include <xbios.h>
#include <aes.h>
#include <linea.h>

// Not in my library yet, but needed:
void vbl_animation_off();
void vbl_animation_on();
void save_palette();

void linea_init(LINEA **parameter_block, FONT_HDR ***sysfont_pointers);
void linea_textblock_transfer();
void linea_showmouse();
void linea_hidemouse();


#define VBL_LIST ((volatile int16_t (**)())0x4ce)

#endif
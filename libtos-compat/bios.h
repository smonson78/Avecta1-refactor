#ifndef __BIOS_H
#define __BIOS_H

#include <stdint.h>

extern int16_t (*VBL_LIST[8])(void);

extern uint16_t *compat_palette;

#endif
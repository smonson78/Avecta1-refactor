#include <stdint.h>
#include "st_globals.h"

int16_t (*VBL_LIST[8])(void);

uint16_t *compat_palette = &globals.video.palette[0];
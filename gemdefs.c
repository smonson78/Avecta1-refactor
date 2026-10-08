#include <stdint.h>

#include "tos_compat.h"

#include <gemdefs.h>

#include "off.h"
#include "vbl.h"
#include "startaux.h"


void vbl_animation_off() {
  xbios_supexec(off);
}

void vbl_animation_on() {
  xbios_supexec(vbl);
}

void save_palette() {
  xbios_supexec(savpal);
}


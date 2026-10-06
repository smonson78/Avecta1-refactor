#include <stdint.h>
#include "off.h"
#include "vbl.h"
#include "startaux.h"

#include <gemdefs.h>

void vbl_animation_off() {
  xbios_supexec(off);
}

void vbl_animation_on() {
  xbios_supexec(vbl);
}

void save_palette() {
  xbios_supexec(savpal);
}


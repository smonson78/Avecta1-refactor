#include <stdint.h>
#include "off.h"
#include "vbl.h"
#include "startaux.h"

#include <gemdefs.h>

void xbios_38_off() {
  xbios_supexec(off);
}

void xbios_38_vbl() {
  xbios_supexec(vbl);
}

void xbios_38_savpal() {
  xbios_supexec(savpal);
}
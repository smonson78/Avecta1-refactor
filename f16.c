#include "globals.h"
#include "vbl.h"
#include "off.h"
#include "raton.h"
#include "rausmaus.h"
#include "gemdefs.h"

int i16(int pc)
{
  xbios_37();
  xbios_38_off();
  raton();
  form_alert(1, "[1][ZZZ... not implemented][OK]");
  rausmaus();
  xbios_37();
  xbios_38_vbl();
  return 0;
}

int o16(int pc)
{
  return 1;
}

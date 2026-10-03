#include "globals.h"
#include "vbl.h"
#include "off.h"
#include "mouse_on.h"
#include "mouse_off.h"
#include "gemdefs.h"

int i16(int pc)
{
  Vsync();
  vbl_animation_off();
  mouse_on();
  form_alert(1, "[1][ZZZ... not implemented][OK]");
  mouse_off();
  Vsync();
  vbl_animation_on();
  return 0;
}

int o16(int pc)
{
  return 1;
}

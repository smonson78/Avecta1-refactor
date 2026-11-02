#include "globals.h"
#include "tos_compat.h"

int i8(int pc)
{
  debug_printf("> i8()\n");
  uint8_t *c = curmon[pc];
  curmon2_t *ch = &new_curmon2[pc];

  c[16] = 4;
  c[15] = 8;
  c[7] = c[8] = c[9] = 0;      

  ch->player.unknown15[0] = 8;
  ch->player.unknown15[1] = 4;
  ch->player.unknown7 = 0;
  ch->player.unknown8 = 0;
  ch->player.unknown9 = 0;

  debug_printf("< i8()\n");

  return 1;
}

int o8(int pc)
{
  debug_printf("> o8()\n");
  uint8_t *c = curmon[pc];
  curmon_t *new_c = &new_curmon[pc];

  // Players only
  if (pc < 4) {
    c[18] = 8;
    new_c->unknown15[3] = 8;
  }

  debug_printf("< o8()\n");
  return 1;
}

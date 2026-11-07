#include "globals.h"
#include "tos_compat.h"

int wait_in(int pc)
{
  debug_printf("> wait_in()\n");
  uint8_t *c = curmon[pc];
  curmon2_t *ch = &new_curmon2[pc];

  c[16] = 4;
  c[15] = 8;
  c[7] = c[8] = c[9] = 0;      

  ch->player.command = 8;
  ch->player.unknown16[0] = 4;
  ch->player.unknown7 = 0;
  ch->player.unknown8 = 0;
  ch->player.unknown9 = 0;

  debug_printf("< wait_in()\n");

  return 1;
}

int wait_out(int pc)
{
  debug_printf("> wait_out()\n");
  uint8_t *c = curmon[pc];
  curmon_t *new_c = &new_curmon[pc];

  // Players only
  if (pc < 4) {
    c[18] = 8;
    new_c->unknown16[2] = 8;
  }

  debug_printf("< wait_out()\n");
  return 1;
}

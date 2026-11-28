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

  ch->player.command = ACTION_WAIT;
  ch->player.time_until_next_move = 4;
  ch->player.target = 0;
  ch->player.target_inventory = 0;
  ch->player.target_object = 0;

  debug_printf("< wait_in()\n");

  return ERR_SUCCESS;
}

int wait_out(int pc)
{
  debug_printf("> wait_out()\n");
  uint8_t *c = curmon[pc];
  curmon_t *new_c = &new_curmon[pc];

  // Players only
  if (IS_CHARACTER(pc)) {
    c[18] = 8;
    new_c->unknown18 = ACTION_WAIT;
  }

  debug_printf("< wait_out()\n");
  return ERR_SUCCESS;
}

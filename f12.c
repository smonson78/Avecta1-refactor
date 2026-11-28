#include "globals.h"
#include "start.h"
#include "startaux.h"
#include "torches.h"
#include "caux.h"

/*****************************************************************************/
/* It's torch time!!                                                         */
/*****************************************************************************/

int burn_snuff_in(int pc)
{
  uint8_t *c = curmon[pc];
  curmon2_t *ch = &new_curmon2[pc];

  // No need for torch outside
  if (outside) {
    return ERR_FAILURE;
  }

  // if (c[41] > 60) {
  if (ch->player.lit_torch_turns > 60) {
    c[10] = 0;
    ch->player.spell_cast = 0;
  }

  if (c[41] <= 60) {
    if (c[42] == 0) {
      error(ERROR_NOTHING_IN_HAND);
      return ERR_FAILURE;
    }
    c[10] = 1;
    ch->player.spell_cast = 1;
  }

  c[7] = c[8] = c[9] = 0;
  ch->player.target = 0;
  ch->player.target_inventory = 0;
  ch->player.target_object = 0;

  c[15] = 12;
  c[16] = 6;
  ch->player.command = ACTION_BURN_SNUFF;
  ch->player.time_until_next_move = 6;

  return ERR_SUCCESS;
}

int burn_snuff_out(int pc)
{
  uint8_t *c = curmon[pc];
  curmon2_t *new_c = &new_curmon2[pc];
  char *w = pc == 0 ? pname : name[c[3]];

  // if (c[10] == 0) {
  if (new_c->player.spell_cast == 0) {
    c[41] = 0;
    c[42] += 1;
    c[46] = 0;
    new_c->player.lit_torch_turns = 0;
    new_c->player.unlit_torches++;
    new_c->player.other_hand = 0;

    litetrol(c[24], c[25],0);
    prnt("-> %s puts out and stores the torch.", w);
    return ACTION_RESULT_COMPLETED;
  }

  c[42] -= 1;
  new_c->player.unlit_torches--;

  prnt("-> %s lights a torch.", w);

  // if (c[41] > 0) {
  if (new_c->player.lit_torch_turns > 0) {
    c[41] = 100;
    c[42] -= 1;
    new_c->player.lit_torch_turns = 100;
    new_c->player.unlit_torches--;

    return 1;
  } else {
    if (c[46] != 81 && c[46] != 0) {
      if (!putaway(pc, c[46])) {
        error(ERROR_INVENTORY_TOO_FULL);
        return ACTION_RESULT_COMPLETED;
      }
    }

    c[46] = 81;
    c[41] = 100;
    new_c->player.other_hand = OBJECT_TORCH;
    new_c->player.lit_torch_turns = 100;

    // litetrol(c[24], c[25], 1);
    litetrol(new_c->player.x, new_c->player.y, 1);
  }

  return ACTION_RESULT_COMPLETED;
}

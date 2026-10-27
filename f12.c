#include "globals.h"
#include "start.h"
#include "startaux.h"
#include "torches.h"
#include "caux.h"

/*****************************************************************************/
/* It's torch time!!                                                         */
/*****************************************************************************/

int i12(int pc)
{
  uint8_t *c = curmon[pc];
  curmon_t *new_c = &new_curmon[pc];

  // No need for torch outside
  if (outside) {
    return 0;
  }

  if (c[41] > 60) {
    c[10] = 0;
    new_c->unknown5[5] = 0;
  }

  if (c[41] <= 60) {
    if (c[42] == 0) {
      error(ERROR_NOTHING_IN_HAND);
      return 0;
    }
    c[10] = 1;
    new_c->unknown5[5] = 1;
  }

  c[7] = c[8] = c[9] = 0;
  new_c->unknown5[2] = 0;
  new_c->unknown5[3] = 0;
  new_c->unknown5[4] = 0;

  c[15] = 12;
  c[16] = 6;
  new_c->unknown15[0] = 12;
  new_c->unknown15[1] = 6;

  return 1;
}

int o12(int pc)
{
  uint8_t *c = curmon[pc];
  curmon_t *new_c = &new_curmon[pc];
  char *w = (pc == 0 ? pname : name[c[3]]);

  if (c[10] == 0) {
    c[41] = 0;
    c[42] += 1;
    c[46] = 0;
    new_c->base_chance_1_attack = 0;
    new_c->base_chance_2_attack++;
    new_c->magic_mode_1 = 0;

    litetrol(c[24], c[25],0);
    prnt("-> %s puts out and stores the torch.", w, NULL, NULL, NULL, NULL, NULL);
    return 1;
  }

  c[42] -= 1;
  new_c->base_chance_2_attack--;

  prnt("-> %s lights a torch.", w, NULL, NULL, NULL, NULL, NULL);

  if (c[41] > 0) {
    c[41] = 100;
    c[42] -= 1;
    new_c->base_chance_1_attack = 100;
    new_c->base_chance_2_attack--;

    return 1;
  } else {
    if (c[46] != 81 && c[46] != 0) {
      if (!putaway(pc, c[46])) {
        error(ERROR_INVENTORY_TOO_FULL);
        return 1;
      }
    }

    c[46] = 81;
    c[41] = 100;
    new_c->magic_mode_1 = 81;
    new_c->base_chance_1_attack = 100;

    litetrol(c[24], c[25], 1);
  }

  return 1;
}

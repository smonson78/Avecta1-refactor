#include "globals.h"
#include "damage.h"
#include "start.h"
#include "startaux.h"
#include "explode.h"
#include "fireball.h"
#include "gemdefs.h"
#include "caux.h"

#include "f5.h"

#include "tos_compat.h"

// Fire arrow

int i11(int pc)
{
  debug_printf("> i11()\n");
  uint8_t *c = curmon[pc];
  curmon2_t *ch = &new_curmon2[pc];
  char *w = (pc == 0 ? pname : name[c[3]]);
  int tar,ret;
  int16_t x, y;

  if (outside) {
    return 0;
  }

  // if (c[45] != 45 || c[44] == 0) {
  if (ch->player.good_hand != OBJECT_BOW || ch->player.arrows == 0) {
    error(ERROR_NO_BOW_OR_ARROWS);
    return 0;
  }

  // if (c[46] != 0) {
  if (ch->player.other_hand != 0) {
    error(ERROR_BOW_TWO_FREE_HANDS);
    return 0;
  }

  domsg(3);
  header(w);
  winker = pc + 1;
  sgetxy(&x, &y, 0, 0, 0, &ret);
  winker = 0;
  undomsg();
  tar = zline[x][y].monster;

  if (pc < 4 && tar < 4) {
    error(ERROR_NO_CHARACTER);
    return 0;
  }

  if (!los(pc, x, y)) {
    error(ERROR_NO_LINE_OF_SIGHT);
    return 0;
  }

  c[7] = tar;
  ch->player.target = tar;

  if (pc > 0) {
    c[39] = tar;
    ch->player.unknown39 = tar;
  }

  c[8] = c[9] = 0;
  ch->player.unknown8 = 0;
  ch->player.unknown9 = 0;

  c[15] = 11;
  ch->player.command = ACTION_FIRE_ARROW;
  c[16] = 2 + c[11];
  ch->player.time_until_next_move = 2 + ch->player.unknown11;

  debug_printf("< i11()\n");
  return 1;
}

int o11(int pc)
{
  debug_printf("> o11()\n");
  uint8_t *c = curmon[pc];
  curmon2_t *ch = &new_curmon2[pc];
  curmon2_t *new_t;
  uint8_t *t;
  char *w = (pc == 0 ? pname : name[c[3]]), *wt;
  int x, y, miss, tar, dir = 0, i, j, k = 0;

  // Target, I assume
  tar = (pc == 0 ? c[7] : c[39] );

  t = curmon[tar];
  new_t = &new_curmon2[tar];

  wt = (tar == 0 ? pname : name[t[3]]);

  c[7] = pc > 0 ? c[39] : c[7];
  ch->player.target = pc > 0 ? ch->player.unknown39 : ch->player.target;

  if (t[30] != crum || !los(pc, t[24], t[25])) {
    prnt("-> The %s is out of %s's sight!", wt, w, NULL, NULL, NULL, NULL);
    return 1;
  }

  if (rnd(100) < 10 * (t[11]) + 5 * (c[50] - 10)) { 
    miss = 0;
    i = 0;
    j = 0;
  } else {
    miss = 1;
    do {
      k++;
      i = rnd(2);
      i = 2 * i - 1;
      j = rnd(2);
      j = 2 * j - 1;
    } while (k < 10 && zline[t[24] + i][t[25] + j].unknown0 == 0);
  }
  nextxy(c[24], c[25], t[24] + i, t[25] + j, c[24], c[25], &x, &y);

  switch (y - c[25]) {
    case 1:
      switch (x - c[24]) {
        case 1:
          dir = 3;
          break;
        case 0:
          dir = 4;
          break;
        case -1:
          dir = 5;
        }
        break;
    case 0:
      switch (x - c[24]) {
        case 1:
          dir = 2;
          break;
        case 0:
          dir = 8;
          break;
        case -1:
          dir = 6;
      }
      break;
    case -1:
      switch (x - c[24]) {
        case 1:
          dir = 1;
          break;
        case 0:
          dir = 0;
          break;
        case -1:
          dir = 7;
      }
      break;
  }

  dir /= 2;

  Vsync();
  vbl_animation_off();
  // fireball(c[24], c[25], t[24] + i, t[25] + j, 3 + dir);
  fireball(ch->player.x, ch->player.y, new_t->monster.x + i, new_t->monster.y + j, 3 + dir);
  // explode(t[24] + i, t[25] + j, 3 + dir);
  explode(new_t->monster.x + i, new_t->monster.y + j, 3 + dir);
  
  if (!miss && (c[43] || (tar > 3 && !t[46]) || tar < 3) ) {
    prnt("-> %s's arrow hits the %s!", w, wt, NULL, NULL, NULL, NULL);
    miss = 5 + rnd(c[50]);
    damage(tar, miss);
    return 1;
  }
  prnt("-> %s's arrow misses the %s!", w, wt, NULL, NULL, NULL, NULL);

  debug_printf("< o11()\n");
  return 1;
}

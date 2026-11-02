#include "globals.h"
#include "caux.h"
#include "start.h"
#include "startaux.h"
#include "attack.h"
#include "damage.h"

int i1(int pc)
{
  uint8_t *c = curmon[pc];
  curmon_t *new_c = &new_curmon[pc];
  uint8_t *t = curmon[c[39]];
  // curmon_t *new_t = &new_curmon[new_c->unknown38[1]];

  int x = t[24], y = t[25];

  if (outside) {
    return 0;
  }

  combat = 0;
  if (!adjac(pc,x,y)) {
    error(ERROR_TARGET_NOT_ADJACENT);
    return 0;
  }

  // Characters (including the player) only:
  if (pc < 4 && (c[45] > 45 || c[45] < 41) ) {
    error(ERROR_NO_WEAPON_IN_HAND);
    return 0;
  }

  // The player only:
  if (pc == 0 && c[45] == 45 && c[46] != 0) {
    error(ERROR_BOW_TWO_FREE_HANDS);
    return 0;
  }

  if (pc < 4 && pc > 0) {
    c[7] = c[39];
    new_c->spells[2] = new_c->unknown39;
  }

  c[15] = 1;
  new_c->unknown15[0] = 1;

  c[16] = c[11];
  new_c->unknown15[1] = new_c->unknown11;

  c[9] = 0;
  new_c->spells[4] = 0;

  return 1;
}

int o1(int pc) 
{
  uint8_t *a = curmon[pc];
  uint8_t *t = curmon[a[39]];

  curmon_t *new_a = &new_curmon[pc];
  player_t *p = &players[pc];

  curmon_t *new_t = &new_curmon[new_a->unknown39];

  char *w;
  int base, roll, dam, wepchanz;

  if (new_t->in_use == 0 || new_t->current_room != crum) {
    return 1;
  }

  if (!adjac(pc, new_t->x, new_t->y)) {
    return 1;
  }

  attack(pc, new_t->x, new_t->y);

  //if (pc < 4 && t[46] && !a[43]) {
  if (pc < 4 && new_t->magic_mode_1 && !p->base_chance_3_attack) {
    a[34] = -1;
    p->unknown33[1] = -1;
    
    return 1;
  }

  if (pc < 4) {
    wepchanz = weapon[a[45] - 40][0] + 20 * (a[43]);
  }

  if (pc < 4 && a[51] > 5) {
    wepchanz /= a[51] - 4;
  } else {
    wepchanz = *(a + 40 + a[40]);
  }
  
  if (t[39] == pc) {
    wepchanz += wepmatx[a[40]][t[40]];
  } else {
    wepchanz += 40;   /* big bonus for blind side shots */         
  }

  roll = rnd(100);
  base = wepchanz + 3*( a[50] - 10 + a[11] - t[11] );

  if (pc < 4) {
    // dam = weapon[a[45] - 40][1];
    dam = weapon[p->good_hand - 40][1];
  } else {
    dam = a[3 + 2 * (a[40])];
  }

  if (pc > 3) {
    dam = 1 + rnd(dam);
  } else {
    dam += rnd(dam / 2);
  }

  if (pc < 4 && t[31] > 3) {
    base += 200;
    dam += 10;
  }

  if (pc < 4) {
    a[8] = a[45];
    p->unknown8 = p->good_hand;
  }

  if (base > roll && dam > 0) {
    if (pc > 3 && a[40] == a[45] && t[47] == 0) {
      w = (a[39] == 0 ? pname : name[curmon[a[39]][3]]);
      if (a[39] < 4) {
        prnt("-> %s is envenomed!", w, NULL, NULL, NULL, NULL, NULL);
      } else {
        prnt("-> The %s is envenomed!", w, NULL, NULL, NULL, NULL, NULL);
      }

      t[47] = 1;
      new_t->poisoned = 1;
    }

    if (pc < 4) {
      a[34] = 1;
      new_a->unknown33[1] = 1;
    }

    return damage(a[39], dam);
  } else {
    if (pc < 4) {
      a[34] = -1;
      new_a->unknown33[1] = -1;
    }
  }

  return 1;
}

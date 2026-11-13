#include "globals.h"
#include "caux.h"
#include "start.h"
#include "startaux.h"
#include "attack.h"
#include "damage.h"
#include "tos_compat.h"

int attack_in(int pc)
{
  debug_printf("> attack_in()\n");
  uint8_t *c = curmon[pc];
  curmon2_t *new_c = &new_curmon2[pc];
  // uint8_t *t = curmon[c[39]];
  curmon2_t *new_t = &new_curmon2[new_c->monster.unknown39];

  // int x = t[24], y = t[25];
  int x = new_t->monster.x, y = new_t->monster.y;

  if (outside) {
    return 0;
  }

  combat = 0;
  if (!adjac(pc, x, y)) {
    error(ERROR_TARGET_NOT_ADJACENT);
    return 0;
  }

  // Characters (including the player) only:
  // if (pc < 4 && (c[45] > 45 || c[45] < 41) ) {
  if (pc < 4 && (new_c->player.good_hand > OBJECT_BOW || new_c->player.good_hand < OBJECT_BROADSWORD) ) {
    error(ERROR_NO_WEAPON_IN_HAND);
    return 0;
  }

  // The player only:
  // if (pc == 0 && c[45] == 45 && c[46] != 0) {
  if (pc == 0 && new_c->player.good_hand == OBJECT_BOW && new_c->player.other_hand != 0) {
    error(ERROR_BOW_TWO_FREE_HANDS);
    return 0;
  }

  // Other player characters
  if (pc < 4 && pc > 0) {
    c[7] = c[39];
    new_c->player.target = new_c->player.unknown39;
  }

  c[15] = 1;
  new_c->monster.command = ACTION_ATTACK;

  c[16] = c[11];
  new_c->monster.time_until_next_move = new_c->monster.unknown11;

  c[9] = 0;
  new_c->monster.spells[4] = 0;

  return 1;
}

int attack_out(int pc) 
{
  debug_printf("> attack_out()\n");
  uint8_t *a = curmon[pc];
  uint8_t *t = curmon[a[39]];
  curmon2_t *new_a = &new_curmon2[pc];
  curmon2_t *new_t = &new_curmon2[new_a->monster.unknown39];

  char *w;
  int base, roll, dam, weapon_chance;

  if (new_t->monster.in_use == 0 || new_t->monster.current_room != crum) {
    return 1;
  }

  if (!adjac(pc, new_t->monster.x, new_t->monster.y)) {
    return 1;
  }

  attack(pc, new_t->monster.x, new_t->monster.y);

  //if (pc < 4 && t[46] && !a[43]) {
  if (pc < 4 && new_t->player.other_hand && !new_a->player.base_chance_3_attack) {
    a[34] = -1;
    new_a->player.unknown33[1] = -1;
    
    return 1;
  }

  if (pc < 4) {
    // weapon_chance = weapon[a[45] - 40].chance + 20 * (a[43]);
    weapon_chance = weapon[new_a->player.good_hand - 40].chance + (20 * new_a->player.base_chance_3_attack);
  }

  // if (pc < 4 && a[51] > 5) {
  if (pc < 4 && new_a->player.magic_level > 5) {
    // weapon_chance /= a[51] - 4;
    weapon_chance /= new_a->player.magic_level - 4;
  } else {
    // weapon_chance = *(a + 40 + a[40]);
    // FIXME need to know what that value is
    weapon_chance = a[40 + new_a->player.unknown40];
  }
  
  // if (t[39] == pc) {
  if (new_t->monster.unknown39 == pc) {
    // weapon_chance += wepmatx[a[40]][t[40]];
    weapon_chance += wepmatx[new_a->monster.unknown40][new_t->monster.unknown40];
  } else {
    weapon_chance += 40;   /* big bonus for blind side shots */         
  }

  roll = rnd(100);
  // base = weapon_chance + 3 * (a[50] - 10 + a[11] - t[11]);
  base = weapon_chance + 3 * (new_a->monster.physical_strength - 
    10 + new_a->monster.unknown11 - new_t->monster.unknown11);

  if (pc < 4) {
    // dam = weapon[a[45] - 40][1];
    dam = weapon[new_a->player.good_hand - 40].damage;
  } else {
    // dam = a[3 + 2 * (a[40])];
    // FIXME
    dam = a[3 + 2 * (a[40])];
  }

  if (pc > 3) {
    dam = 1 + rnd(dam);
  } else {
    dam += rnd(dam / 2);
  }

  // if (pc < 4 && t[31] > 3) {
  if (pc < 4 && new_t->monster.intelligence_routine > 3) {
    base += 200;
    dam += 10;
  }

  if (pc < 4) {
    a[8] = a[45];
    new_a->player.unknown8 = new_a->player.good_hand;
  }

  if (base > roll && dam > 0) {
    // if (pc > 3 && a[40] == a[45] && t[47] == 0) {
    if (pc > 3 && new_a->monster.unknown40 == new_a->monster.poison_mode_2 
        && new_t->monster.poisoned == 0) {
      
      // w = (a[39] == 0 ? pname : name[curmon[a[39]][3]]);
      w = (new_a->monster.unknown39 == 0 ? 
        pname : name[new_curmon2[new_a->monster.unknown39].monster.monster_name_id]);

      // if (a[39] < 4) {
      if (new_a->monster.unknown39 < 4) {
        prnt("-> %s is envenomed!", w, NULL, NULL, NULL, NULL, NULL);
      } else {
        prnt("-> The %s is envenomed!", w, NULL, NULL, NULL, NULL, NULL);
      }

      t[47] = 1;
      new_t->monster.poisoned = 1;
    }

    if (pc < 4) {
      a[34] = 1;
      new_a->player.unknown33[1] = 1;
    }

    // return damage(a[39], dam);
    return damage(new_a->monster.unknown39, dam);
  } else {
    if (pc < 4) {
      a[34] = -1;
      new_a->player.unknown33[1] = -1;
    }
  }

  return 1;
}

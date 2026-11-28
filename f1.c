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
  curmon2_t *new_t = &new_curmon2[new_c->monster.attack_target];

  // int x = t[24], y = t[25];
  int x = new_t->monster.x;
  int y = new_t->monster.y;

  if (outside) {
    return ERR_FAILURE;
  }

  combat = 0;
  if (!adjac(pc, x, y)) {
    error(ERROR_TARGET_NOT_ADJACENT);
    return ERR_FAILURE;
  }

  // Characters (including the player) only:
  // if (pc < 4 && (c[45] > 45 || c[45] < 41) ) {
  if (IS_CHARACTER(pc) && (new_c->player.good_hand > OBJECT_BOW || new_c->player.good_hand < OBJECT_BROADSWORD) ) {
    error(ERROR_NO_WEAPON_IN_HAND);
    return ERR_FAILURE;
  }

  // The player only:
  // if (pc == 0 && c[45] == 45 && c[46] != 0) {
  if (IS_PLAYER(pc) && new_c->player.good_hand == OBJECT_BOW && new_c->player.other_hand != 0) {
    error(ERROR_BOW_TWO_FREE_HANDS);
    return ERR_FAILURE;
  }

  // Other player characters
  // if (pc < 4 && pc > 0) {
  if (IS_CHARACTER(pc) && !IS_PLAYER(pc)) {
    c[7] = c[39];
    new_c->player.target = new_c->player.unknown39;
  }

  c[15] = 1;
  new_c->monster.command = ACTION_ATTACK;

  c[16] = c[11];
  new_c->monster.time_until_next_move = new_c->monster.speed;

  c[9] = 0;
  new_c->monster.target_object = 0;

  return ERR_SUCCESS;
}

// FIXME: At no point does the armour stat do anything.
int attack_out(int pc) 
{
  debug_printf("> attack_out()\n");
  uint8_t *a = curmon[pc];
  uint8_t *t = curmon[a[39]];
  curmon2_t *new_a = &new_curmon2[pc];
  curmon2_t *new_t = &new_curmon2[new_a->monster.attack_target];

  char *w;
  int base, roll, dam, weapon_chance;

  if (new_t->monster.in_use == 0 || new_t->monster.current_room != crum) {
    return ERR_SUCCESS;
  }

  if (!adjac(pc, new_t->monster.x, new_t->monster.y)) {
    return ERR_SUCCESS;
  }

  attack(pc, new_t->monster.x, new_t->monster.y);

  //if (pc < 4 && t[46] && !a[43]) {
  if (IS_CHARACTER(pc) && new_t->player.other_hand && !new_a->player.unknown43) {
    a[34] = -1;
    new_a->player.unknown34 = -1;
    
    return ERR_SUCCESS;
  }

  // if (pc < 4) {
  if (IS_CHARACTER(pc)) {
    // weapon_chance = weapon[a[45] - 40].chance + 20 * (a[43]);
    weapon_chance = weapon[new_a->player.good_hand - 40].chance + (20 * new_a->player.unknown43);
  }

  // if (pc < 4 && a[51] > 5) {
  if (IS_CHARACTER(pc) && new_a->player.magic_level > 5) {
    // weapon_chance /= a[51] - 4;
    weapon_chance /= new_a->player.magic_level - 4;
  } else {
    // weapon_chance = *(a + 40 + a[40]);
    // This must somehow involve the base_X_attack values where X is 1-3
    weapon_chance = a[40 + new_a->player.combat_stance];
    debug_printf(">>> attack weapon chance was %d from unknown40=%d!\n", weapon_chance, new_a->player.combat_stance);
  }
  
  // This is testing whether the monster is also fighting the character attacking them
  // if (t[39] == pc) {
  if (new_t->monster.attack_target == pc) {
    // If two creatures are fighting each other, the weapon chance of the attacker is affected by the
    // combat stance of both individuals.
    // weapon_chance += wepmatx[a[40]][t[40]];
    weapon_chance += wepmatx[new_a->monster.combat_stance][new_t->monster.combat_stance];
    debug_printf(">>> in combat, attacker had stance %d and defender had stance %d, giving a chance of %d!\n",
      new_a->monster.combat_stance,
      new_t->monster.combat_stance,
      wepmatx[new_a->monster.combat_stance][new_t->monster.combat_stance]);
  } else {
    weapon_chance += 40;   /* big bonus for blind side shots */         
  }

  roll = rnd(100);
  // base = weapon_chance + 3 * (a[50] - 10 + a[11] - t[11]);
  base = weapon_chance + 3 * (new_a->monster.physical_strength - 
    10 + new_a->monster.speed - new_t->monster.speed);

  // if (pc < 4) {
  if (IS_CHARACTER(pc)) {
    // dam = weapon[a[45] - 40][1];
    dam = weapon[new_a->player.good_hand - 40].damage;
  } else {
    // dam = a[3 + 2 * (a[40])];
    // FIXME
    dam = a[3 + 2 * (a[40])];
  }

  // if (pc > 3) {
  if (IS_MONSTER(pc)) {
    dam = 1 + rnd(dam);
  } else {
    dam += rnd(dam / 2);
  }

  // if (pc < 4 && t[31] > 3) {
  if (IS_CHARACTER(pc) && new_t->monster.intelligence_routine > 3) {
    base += 200;
    dam += 10;
  }

  if (IS_CHARACTER(pc)) {
    a[8] = a[45];
    new_a->player.target_inventory = new_a->player.good_hand;
  }

  if (base > roll && dam > 0) {
    // if (pc > 3 && a[40] == a[45] && t[47] == 0) {
    if (IS_MONSTER(pc) && new_a->monster.combat_stance == new_a->monster.poison_mode_2 
        && new_t->monster.poisoned == 0) {
      
      // w = (a[39] == 0 ? pname : name[curmon[a[39]][3]]);
      w = (new_a->monster.attack_target == 0 ? 
        pname : name[new_curmon2[new_a->monster.attack_target].monster.name_id]);

      // if (a[39] < 4) {
      if (IS_CHARACTER(new_a->monster.attack_target)) {
        prnt("-> %s is envenomed!", w);
      } else {
        prnt("-> The %s is envenomed!", w);
      }

      t[47] = 1;
      new_t->monster.poisoned = 1;
    }

    if (pc < 4) {
      a[34] = 1;
      new_a->player.unknown34 = 1;
    }

    // return damage(a[39], dam);
    return damage(new_a->monster.attack_target, dam);
  } else {
    if (IS_CHARACTER(pc)) {
      a[34] = -1;
      new_a->player.unknown34 = -1;
    }
  }

  return ERR_SUCCESS;
}

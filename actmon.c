#include <stdint.h>
#include <libc.h>

#include "globals.h"
#include "dorep.h"
#include "rumdraw.h"
#include "torches.h"
#include "caux.h"
#include "curmon.h"

#include "debug.h"

#include "tos_compat.h"

/****************************************************************************/
/* ACTMON(TYPE,NUMBER,X,Y) activates the number of type monsters around and */
/* including the position x,y.                                              */
/****************************************************************************/

// Negative TYPE means ressurect this monster from a dead body

// Type can be 1-8 (random), taken from rumdata.room_monster_type, or -3, -2, -1, 2, 5, 11, 19, 25 

int actmon(int type, int nm, int x, int y)
{
  uint8_t *c, *z;
  curmon_t *new_c;
  player_t *pc;
  permon_t *p;
  int adder, i, j = 0, k, l;

  debug_printf("> actmon(%d, %d, %d, %d)\n", type, nm, x, y);

  check_curmon("actmon start");

  // Negative numbers seen in the code: -1, -2, -3
  // ...if "type" is negative it will resurrect a player character.
  if (type < 0) {
    type = abs(type);

    // Zero out the whole curmon structure:
    c = curmon[type];
    for (i = 0; i < 60; i++) {
      c[i] = 0;
    }
    // The new way:
    pc = &players[type];
    memset(pc, 0, sizeof(curmon_t));

    // The range of type here is -3 to -1 therefore the result will be permon[4] to permon[6]
    // 4 - dog
    // 5 - goblin
    // 6 - bat
    p = &permon[type + 7];

    c[0] = p->in_use;
    c[1] = p->damage;
    c[2] = p->max_health;
    c[3] = p->monster_name_id;
    c[4] = p->sprite_id;
    c[5] = p->unknown5;
    c[6] = p->unknown6;
    c[7] = p->unknown7;
    c[8] = p->unknown8;
    c[9] = p->unknown9;
    c[10] = p->unknown10;
    c[11] = p->unknown11;
    c[12] = p->unknown12;
    c[13] = p->unknown13;
    c[14] = p->spell_units;

    c[50] = p->physical_strength;
    c[42] = 10;
    c[32] = p->poison_mode;
    c[45] = p->poison_mode;

    // These offsets don't agree with the later ones (41, 42, 43)
    c[46] = p->base_chance_1_attack;
    c[37] = p->base_chance_2_attack;
    c[51] = p->base_chance_3_attack;

    c[24] = x;
    c[25] = y;
    c[15] = 8;
    c[16] = 1;
    c[30] = crum;
    c[39] = i; // seems like this will always be 60

    pc->in_use = p->in_use;
    pc->damage = p->damage;
    pc->max_health = p->max_health;
    pc->monster_name_id = p->monster_name_id;
    pc->sprite_id = p->sprite_id;
    pc->unknown5 = p->unknown5;
    pc->unknown6 = p->unknown6;
    pc->unknown7 = p->unknown7;
    pc->unknown8 = p->unknown8;
    pc->unknown9 = p->unknown9;
    pc->spell_cast = p->unknown10;
    pc->unknown11 = p->unknown11;
    pc->unknown12 = p->unknown12;
    pc->unknown13 = p->unknown13;
    pc->spell_units = p->spell_units;

    pc->command = ACTION_WAIT;
    pc->unknown16[0] = 1;

    pc->x = x;
    pc->y = y;
    pc->current_room = crum;
    pc->poison_mode_1 = p->poison_mode;
    pc->unknown39 = i;
    pc->unlit_torches = 10;
    pc->good_hand = p->poison_mode;
    pc->other_hand = p->base_chance_1_attack;
    pc->perception = p->base_chance_2_attack;
    pc->magic_level = p->base_chance_3_attack;

    pc->physical_strength = p->physical_strength;

    check_curmon("actmon 1");

    zline[x][y][2] = type;
    drawman(type, x, y);

    return 1;
  }

  p = &permon[type];

  // Do this loop for 8 monsters with index 4 to 11
  debug_printf("-- Spawning new monsters [0..8] in room of type %d\n", type);
  for (i = 0; i < 8; i++) {
    c = curmon[i + 4];
    new_c = &monsters[i];

    adder = 0;

    // if (c[0] == 0 || c[30] != crum) {
    if (new_c->in_use == 0 || new_c->current_room != crum) {
      z = zline[x][y];
      if (z[0] == 1 && (z[1] == 0 || crumobj[z[1]].object_id > OBJECT_DEAD_EIRIK) && z[2] == 0 && 
        // (curmon[0][24] != x || curmon[0][25] != y) ) { 
        (player->x != x || player->y != y) ) { 
        zline[x][y][2] = i + 4;   /* then square is available */

        c[24] = x;
        c[25] = y;
        new_c->x = x;
        new_c->y = y;
        
        adder = 1;
      } else { /* must root for adjacent square to dump monster into */
        k = -2;
        l = -1;

        do {
          k++;

          if (k == 2) {
            k = -2;
            l++;
          }

          z = zline[x+k][y+l];

          if (k == -2 || *z != 1 || z[2] != 0 || (player->x == (x + k) && 
            player->y == (y + l)) || 
            (z[1] > 0 && crumobj[z[1]].object_id < OBJECT_BROADSWORD) ) {
            /* if it doesn't continue then square available */
            continue;  
          } else {
            z[2] = i + 4;
            
            c[24] = x + k;
            c[25] = y + l;
            new_c->x = x + k;
            new_c->y = y + l;

            adder = 1;
            break;
          }
        } while (l != 1 || k != 1);
      }   
    }
    
    // Do we have somewhere to place this monster, or not
    if (adder == 0) {
      // Apparently we don't.
      continue;
    }

    j++;
    
    // Clear out monster instance apart from 24 and 25 (x and y)
    for (k = 0; k < 60; k++) { 
      if (k == 24 || k == 25) {
        continue;
      }

      c[k] = 0;
    }

    // Same for new_curmon:
    int temp_x = new_c->x;
    int temp_y = new_c->y;
    memset(new_c, 0, sizeof(curmon_t));
    new_c->x = temp_x;
    new_c->y = temp_y;

    // Now copy monster type to monster instance
    c[0] = p->in_use;
    c[1] = p->damage;
    c[2] = p->max_health;
    c[3] = p->monster_name_id;
    c[4] = p->sprite_id;
    c[5] = p->unknown5;
    c[6] = p->unknown6;
    c[7] = p->unknown7;
    c[8] = p->unknown8;
    c[9] = p->unknown9;
    c[10] = p->unknown10;
    c[11] = p->unknown11;
    c[12] = p->unknown12;
    c[13] = p->unknown13;
    c[14] = p->spell_units;    
    c[50] = p->physical_strength;   /* physical strength */
    c[45] = p->poison_mode;   /* poison mode */
    c[41] = p->base_chance_1_attack;   /* base chance #1 attack */
    c[42] = p->base_chance_2_attack;   /* base chance #2 */
    c[43] = p->base_chance_3_attack;   /* base chance #3 */
    c[31] = p->intell_routine;   /* intell routine */
    c[44] = p->bane;   /* bane */
    
    c[46] = p->magic_mode;   /* magic mode, can be hit by magic? */
    c[54] = p->magic_mode;

    c[58] = type;    /* record type of monster */
    c[51] = c[14] / 3;  /* magic level is one-third of spelunits */
    
    if (c[51] > 20) {
      c[54] = 1;     /* then he is immune to magic as in p.magic_mode */
    }

    c[15] = 8;       /* installed with a wait */
    c[16] = 1;
    c[30] = crum;
    c[39] = i + 4;
    int randomness = rnd(3);
    if (type == 18) {
      c[4] = 116 + randomness;
    }

    // ...same again, but for new_curmon
    new_c->in_use = p->in_use;
    new_c->damage = p->damage;
    new_c->max_health = p->max_health;
    new_c->monster_name_id = p->monster_name_id;
    new_c->sprite_id = p->sprite_id;
    new_c->spells[0] = p->unknown5;
    new_c->spells[1] = p->unknown6;
    new_c->spells[2] = p->unknown7;
    new_c->spells[3] = p->unknown8;
    new_c->spells[4] = p->unknown9;
    new_c->spell_cast = p->unknown10;
    new_c->unknown11 = p->unknown11;
    new_c->unknown12 = p->unknown12;
    new_c->unknown13 = p->unknown13;
    new_c->spell_units = p->spell_units;    

    new_c->intelligence_routine = p->intell_routine;   /* intell routine */

    new_c->physical_strength = p->physical_strength;   /* physical strength */
    new_c->poison_mode_2 = p->poison_mode;   /* poison mode */
    new_c->base_chance_1_attack = p->base_chance_1_attack;   /* base chance #1 attack */
    new_c->base_chance_2_attack = p->base_chance_2_attack;   /* base chance #2 */
    new_c->base_chance_3_attack = p->base_chance_3_attack;   /* base chance #3 */
    new_c->bane = p->bane;   /* bane */
    
    new_c->magic_mode_1 = p->magic_mode;   /* magic mode, can be hit by magic? */
    new_c->magic_mode_2 = p->magic_mode;

    new_c->monster_type = type;    /* record type of monster */
    new_c->magic_level = new_c->spell_units / 3;  /* magic level is one-third of spelunits */
    
    if (new_c->magic_level > 20) {
      new_c->magic_mode_2 = 1;     /* then he is immune to magic as in p.magic_mode */
    }

    new_c->command = ACTION_WAIT;       /* installed with a wait */
    new_c->unknown16[0] = 1;
    new_c->current_room = crum;
    new_c->unknown39 = i + 4;

    // Pedestrian?
    if (type == 18) {
      new_c->sprite_id = 116 + randomness;
    }    

    // drawman(i + 4, c[24], c[25]);
    drawman(i + 4, new_c->x, new_c->y);

    // Old curmon
    c[26] = curmon[0][24];
    c[27] = curmon[0][25];

    // new_curmon
    new_c->old_x = player->x;
    new_c->old_y = player->y;
    
    if (j == nm) {
      break;
    }
  }

  // c[31] is intelligence_routine
  if (j > 0 && c[31] < 4) {
    mode = 1;
    dorep();
  } else {
    mode = 0;
  }

  check_curmon("actmon 2");

  // If only one monster/player spawned:
  if (nm == 1) {
    if (i + 4 > 0) {
      // I don't think it's possible for i to be less than 4 here (or i + 4 in the new version)
      debug_printf("< actmon() returning %d\n", i + 4);
      return i + 4;
    } else {
      debug_printf("< actmon() returning -1\n");
      return -1;
    }
  }
  
  return j;
}
       

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
  permon_t *p;
  int adder, i, j = 0, k, l;

  // Negative numbers seen in the code: -1, -2, -3
  if (type < 0) {
    type = abs(type);

    // Zero out the whole curmon structure:
    c = curmon[type];
    for (i = 0; i < 60; i++) {
      c[i] = 0;
    }
    // The new way:
    new_c = &new_curmon[type];
    memset(&new_curmon[type], 0, sizeof(curmon_t));

    // The range of type here is -3 to -1 therefore the result will be permon[4] to permon[6]
    // 4 - dog
    // 5 - goblin
    // 6 - bat
    p = &permon[type + 7];

    c[0] = p->in_use;
    c[1] = p->unknown1;
    c[2] = p->unknown2;
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
    c[14] = p->unknown14;

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

    new_c->in_use = p->in_use;
    new_c->unknown1[0] = p->unknown1;
    new_c->unknown1[1] = p->unknown2;
    new_c->monster_name_id = p->monster_name_id;
    new_c->sprite_id = p->sprite_id;
    new_c->unknown5[0] = p->unknown5;
    new_c->unknown5[1] = p->unknown6;
    new_c->unknown5[2] = p->unknown7;
    new_c->unknown5[3] = p->unknown8;
    new_c->unknown5[4] = p->unknown9;
    new_c->unknown5[5] = p->unknown10;
    new_c->unknown5[6] = p->unknown11;
    new_c->unknown5[7] = p->unknown12;
    new_c->unknown5[8] = p->unknown13;
    new_c->unknown5[9] = p->unknown14;

    new_c->unknown15[0] = 8;
    new_c->unknown15[1] = 1;

    new_c->x = x;
    new_c->y = y;
    new_c->current_room = crum;
    new_c->poison_mode_1 = p->poison_mode;
    new_c->unknown38[1] = i;
    new_c->unknown38[4] = 10;
    new_c->poison_mode_2 = p->poison_mode;
    
    // These 3 seem like bugs. I'm not even sure you can resurrect monsters in this game
    new_c->magic_mode_1 = p->base_chance_1_attack;
    new_c->perception = p->base_chance_2_attack;
    new_c->magic_level = p->base_chance_3_attack;

    new_c->physical_strength = p->physical_strength;

    check_curmon("actmon 1");

    zline[x][y][2] = type;
    drawman(type, x, y);

    return 1;
  }

  p = &permon[type];

  // Do this loop for 8 monsters with index 4 to 11
  debug_printf("-- Spawning new monsters [4..11] in room of type %d\n", type);
  for (i = 4; i < 12; i++) {
    c = curmon[i];
    new_c = &new_curmon[i];

    adder = 0;

    if (*c == 0 || c[30] != crum) {
      z = zline[x][y];
      if (*z == 1 && (z[1] == 0 || crumobj[z[1]][0] > 40) && z[2] == 0 && 
        (curmon[0][24] != x || curmon[0][25] != y) ) { 
        zline[x][y][2] = i;   /* then square is available */

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

          if (k == -2 || *z != 1 || z[2] != 0 || (curmon[0][24] == (x + k) && 
            curmon[0][25] == (y + l)) || 
            (z[1] > 0 && crumobj[z[1]][0] < 41) ) {
            /* if it doesn't continue then square available */
            continue;  
          } else {
            z[2] = i;
            
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
    c[1] = p->unknown1;
    c[2] = p->unknown2;
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
    c[14] = p->unknown14;    
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
    c[39] = i;
    if (type == 18) {
      c[4] = 116 + rnd(3);
    }

    // ...same again, but for new_curmon
    new_c->in_use = p->in_use;
    new_c->unknown1[0] = p->unknown1;
    new_c->unknown1[1] = p->unknown2;
    new_c->monster_name_id = p->monster_name_id;
    new_c->sprite_id = p->sprite_id;
    new_c->unknown5[0] = p->unknown5;
    new_c->unknown5[1] = p->unknown6;
    new_c->unknown5[2] = p->unknown7;
    new_c->unknown5[3] = p->unknown8;
    new_c->unknown5[4] = p->unknown9;
    new_c->unknown5[5] = p->unknown10;
    new_c->unknown5[6] = p->unknown11;
    new_c->unknown5[7] = p->unknown12;
    new_c->unknown5[8] = p->unknown13;
    new_c->unknown5[9] = p->unknown14;    

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
    
    if (new_c->unknown1[51] > 20) {
      new_c->unknown1[54] = 1;     /* then he is immune to magic as in p.magic_mode */
    }

    new_c->unknown15[0] = 8;       /* installed with a wait */
    new_c->unknown15[1] = 1;
    new_c->current_room = crum;
    new_c->unknown38[1] = i;

    // Pedestrian?
    if (type == 18) {
      new_c->sprite_id = 116 + rnd(3);
    }    

    drawman(i, c[24], c[25]);
    // drawman(i, new_c->x, new_c->y);

    // Old curmon
    c[26] = curmon[0][24];
    c[27] = curmon[0][25];

    // new_curmon
    new_c->old_x = new_curmon[0].x;
    new_c->old_y = new_curmon[0].y;
    
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

  if (nm == 1) {
    if (i > 0) {
      return i;
    } else {
      return -1;
    }
  }
  
  return j;
}
       

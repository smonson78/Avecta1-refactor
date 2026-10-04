#include <stdint.h>

#include "globals.h"
#include "dorep.h"
#include "rumdraw.h"
#include "torches.h"
#include "caux.h"

/****************************************************************************/
/* ACTMON(TYPE,NUMBER,X,Y) activates the number of type monsters around and */
/* including the position x,y.                                              */
/****************************************************************************/

int actmon(int type, int nm, int x, int y)
{
  uint8_t *c, *z;
  permon_t *p;
  
  int adder, i, j = 0, k, l;

  if (type < 0) {
    type = abs(type);
    c = curmon[type];
    for (i = 0; i < 60; i++) {
      c[i] = 0;
    }

    p = &permon[type + 7];

    c[0] = p->in_use;
    c[1] = p->unknown1;
    c[2] = p->unknown2;
    c[3] = p->monster_name_id;
    c[4] = p->unknown4;
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
    c[46] = p->base_chance_1_attack;
    c[37] = p->base_chance_2_attack;
    c[51] = p->base_chance_3_attack;
    c[24] = x;
    c[25] = y;
    c[15] = 8;
    c[16] = 1;
    c[30] = crum;
    c[39] = i;

    zline[x][y][2] = type;
    drawman(type, x, y);
    return 1;
  }

  p = &permon[type];

  for (i = 4; i < 12; i++) {
    c = curmon[i];
    adder = 0;

    if(*c == 0 || c[30] != crum) {
      z = zline[x][y];
      if(*z == 1 && (z[1] == 0 || crumobj[z[1]][0] > 40) && z[2] == 0 && 
        (curmon[0][24] != x || curmon[0][25] != y) ) { 
        zline[x][y][2] = i;   /* then square is available */
        c[24] = x;
        c[25] = y;
        adder = 1;
        }
      else { /* must root for adjacent square to dump monster into */
        k = -2;
        l = -1;
        do {
            k++;
            if(k == 2) {
                k = -2;
                l++;
                }
            z = zline[x+k][y+l];
            if(k == -2 || *z != 1 || z[2] != 0 || (curmon[0][24] == (x+k) && 
                    curmon[0][25] == (y+l)) || 
                    (z[1] > 0 && crumobj[z[1]][0] < 41) ) 
                continue;  /* if it doesn't continue then square available */
            else {
                z[2] = i;
                c[24] = x + k;
                c[25] = y + l;
                adder = 1;
                break;
            }
        } while ( l != 1 || k != 1); 
      }   
    }
    
    if (adder == 0) {
      continue;
    }
    j++;
    
    // Clear out monster instance apart from 24 and 25
    for (k = 0; k < 60; k++) { 
      if (k == 24 || k == 25) {
        continue;
      }

      c[k] = 0;
    }

    // Now copy monster type to monster instance
    c[0] = p->in_use;
    c[1] = p->unknown1;
    c[2] = p->unknown2;
    c[3] = p->monster_name_id;
    c[4] = p->unknown4;
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

    drawman(i, c[24], c[25]);
    c[26] = curmon[0][24];
    c[27] = curmon[0][25];
    
    if (j == nm) {
      break;
    }
  }
  if (j > 0 && c[31] < 4) {
    mode = 1;
    dorep();
  } else {
    mode = 0;
  }

  if (nm == 1) {
    if (i > 0) {
      return i;
    } else {
      return -1;
    }
  }
  
  return j;
}
       

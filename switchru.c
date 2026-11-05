#include "globals.h"
#include "gemdefs.h"
#include "trigtrol.h"
#include "words.h"
#include "caux.h"
#include "dist.h"
#include "rumdraw.h"
#include "torches.h"
#include "dorep.h"
#include "start.h"
#include "blt.h"

#include "debug.h"

#include "tos_compat.h"

void stormon();
int getmon(int room);

// Switch room, and the associated housekeeping
int switchrum() {
  int d, i, j, x = 0, y = 0, xn, yn, oldrum;

  uint8_t *p = curmon[0];
  curmon_t *new_p = &new_curmon[0];

  debug_printf("> switchrum()\n");
  check_curmon("switchrum start");

  // uint8_t *r = rumdata[crum];
  room_data_t *new_r = &new_rumdata[crum];

  uint8_t *c;
  room_object_t *object;

  char pan[12];

  if (crum != p[30] && crum != -1 && !outside) {
    for (i = 0; i < 13; i++) {
      
      object = &crumobj[i + 1];

      // Each of these iterations is for one object inside the current room.
      // Copy 9 bytes for each
      // for (j = 0; j < 9; j++) {
      //   // Copy the objects data from the current room array data back into storage in rumdata[]
      //   // Maybe because we're leaving the room which suggests p[30] is a room number
      //   // r[31 + (9 * i) + j] = c[j];
      //   new_r->object[i].unknown[j] = c[j];
      // }
      memcpy(&new_r->object[i], object, sizeof(room_object_t));
    }

    // Init pursuit[3] to all zeroes
    // for(i=0;i<3;pursuit[i++] = 0)
    //   ;
    pursuit[0] = 10;
    pursuit[1] = 0;
    pursuit[2] = 0;

    for (i = 1; i < 12; i++) {
      
      c = curmon[i];
      c[35] = 0;

      if (c[0] == 1 && c[30] == crum && (i < 4 || c[38] == 0) && 
        (i < 4 || c[39] < 4) && c[58] != 16 &&
        c[31] < 4 && (dungeon != 2 || c[58] == 17) &&
        (i < 4 || (c[36] == 0 && c[13] > rnd(100)) ) ) {

        pursuit[2] = crum;
        xn = c[24];
        yn = c[25];
        d = dist(x, y, 0, xn, yn, 0);
        pursuit[0] = (pursuit[0] > d ? d : pursuit[0]);
        pursuit[1]++;
      } 
    }
  }

  if (p[30] == 127) {
    stormon();
    pursuit[0] = pursuit[1] = pursuit[2] = 0;
    return 2;
  }

  if (fromout == 1 && p[30] != crum) {
    // The big bird-looking thing
    p[4] = 125;
    new_p->sprite_id = 125;

    outside = 1;
    fromout = 0;
  }

  if (fromout == 2) {
    fromout = 1;
  }

  p[15] = 8;
  p[16] = 1;
  new_p->unknown15[0] = 8;
  new_p->unknown15[1] = 1;

  x = p[24];
  y = p[25];

  p[35] = 0;
  new_p->stink_lines = 0;

  mode = 0;

  // Init pan[12] to all zeroes
  for (i = 0; i < 12; i++) {
    pan[i] = 0;
  }

  for (i = 4; i < 12; i++) {
    if (curmon[i][30] != crum && (curmon[i][36] == 1 || curmon[i][31] == 6)) {
      pan[i - 1] = 1;
    }
  }

  if (crum != p[30] && crum != -1) {
    specbuf[35] = specbuf[36] = 0;
    stormon();
  }

  if (pursuit[0] > 1) {
    pursuit[0] /= 2;
  }

  if (pursuit[1] == 0) {
    pursuit[0] = 0;
  }
  
  // Move to a new room?
  oldrum = crum;
  crum = p[30];

  // Crum 70 might be the endgame room
  if (crum == 70) 
    return(1);

  // ROOM DRAW hopefully
  rumdraw(pan);

  // Seems like this just draws room furniture
  i = 0;
  // ...if we changed rooms
  if (oldrum != crum) {
    // 1..13
    for (i = 1; i < 14; i++) {
      // This array is 19x9 or possibly 18x9
      object = &crumobj[i];

      // if (c[3] == oldrum) {
      if (object->target == oldrum) {
        
        // x = c[6];
        // y = c[7];
        x = object->x;
        y = object->y;

        p[24] = x;
        p[25] = y;
        new_p->x = x;
        new_p->y = y;

        /* must reveal hidden door */
        // if (c[8] == 0) { 
        if (object->visible == 0) { 
          // c[8] = 1;
          object->visible = 1;
          Vsync(); 

          blt(&bitmap[object->object_id], 16 * x, 16 * y);

          zline[x][y][1] = i;
        }
        break;
      }
    }
  }

  /* got into the room through a trap */
  if (oldrum == crum || i == 14) {
    x = p[24];
    y = p[25];

    if (i == 14) {
      pursuit[0] = 0;
      pursuit[1] = 0;
      pursuit[2] = 0;
    }
  }

  drawman(0, x, y);

  if (oldrum == crum) {
    for (i = 1; i < 12; i++) { 
      if (pan[i - 1] == 1) {
        curmon[i][36] = 1;
        new_curmon[i].flee = 1;
      }
    }
  }

  getmon(crum);

  if (mode) {
    dorep();
  }

  if (!outside) {
    if (fromout != 0) {  
      prnt(msg[crum], NULL, NULL, NULL, NULL, NULL, NULL);
    } else {
      prnt(rummsg[crum + 80 * (dungeon - 1)], NULL, NULL, NULL, NULL, NULL, NULL);
    }

    trigtrol(0);
  }

  // If the new room the player has moved into is lit
  if (new_rumdata[crum].room_has_bg) {
    
    // Search for a torch in curmon 0 to 3 (player characters)
    i = 0;
    for (j = 0; j < 4; j++) {
      c = curmon[j];
      curmon_t *new_c = &new_curmon[j];
      
      // Some odd calculations in here
      if (c[41] > 0) { 
        // This means it was a lit torch
        i = 1;

        c[42] += 1; // Maybe this is the number of unlit torches
        c[41] = 0; // Now it's not a lit torch
        c[46] = 0;

        new_c->base_chance_2_attack++;
        new_c->base_chance_1_attack = 0;
        new_c->magic_mode_1 = 0;
      }
    }

    // Player was holding a lit torch when moving into a lit room
    if (i) {
      prnt("-> The lit torch is snuffed and put away.", NULL, NULL, NULL, NULL, NULL, NULL);
    }
  }

  debug_printf("< switchrum()\n");

  return 0;
}

void stormon()
{
  uint8_t *c;
  curmon_t *new_c;
  char *r = monbuf;
  int i, j = 1;
  
  // Do all non-player monsters
  for (i = 4; i < 12; i++) {
    c = curmon[i];
    new_c = &new_curmon[i];

    if (c[0] == 0 || c[30] != crum) {
      continue;
    }

    while (r[j] != 0 && j < 320) {
      j += 4;
    }

    if (j >= 320) {
      break;
    }

    if (c[4] > 115 && c[4] < 119) {
      c[4] = 110;
      new_c->sprite_id = 110;
    }

    r[j] = c[4];
    *(r+j+1) = crum;
    *(r+j+2) = c[1];
    *(r+j+3) = 16*( c[25] ) + c[24];
  }
}

int getmon(int room)
{
  uint8_t *c;
  permon_t *p;
  char *r = monbuf;
  int i,j,k,type,count=0,x,y;
  check_curmon("getmon start");

  for (i = 1; i < 320; i += 4) {
    if (r[i + 1] == room) {
      for (j = 1; j < 26; j++) {
        if (permon[j].sprite_id == r[i]) {
          break;
        }
      }

      if (j == 26) {
        continue;
      }

      type = j;
      p = &permon[type];
      x = r[i+3] % 16;
      y = r[i+3] / 16;

      if (zline[x][y][2] != 0 || (x == curmon[0][24] && y == curmon[0][25])) {
        continue;
      }

      // Do all non-player monsters
      for (j = 0; j < 8; j++) {
        c = curmon[j + 4];
        // curmon_t *new_c = &new_curmon[j + 4];
        curmon_t *mon = &monsters[j];

        if (c[0] == 0 || c[30] != room) {

          // Clear all curmon data
          for (k = 0; k < 60; k++) {
            c[k] = 0;
          }
          // New way:
          memset(mon, 0, sizeof(curmon_t));
          
          // Copy curmon (monster instance) data from permon (monster type)
          // for (k = 0; k < 15; k++) {
          //   *(c+k) = *(p+k);
          // }
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
          
          mon->in_use = p->in_use;
          mon->damage = p->damage;
          mon->max_health = p->max_health;
          mon->monster_name_id = p->monster_name_id;
          mon->sprite_id = p->sprite_id;
          mon->spells[0] = p->unknown5;
          mon->spells[1] = p->unknown6;
          mon->spells[2] = p->unknown7;
          mon->spells[3] = p->unknown8;
          mon->spells[4] = p->unknown9;
          mon->spell_cast = p->unknown10;
          mon->unknown11 = p->unknown11;
          mon->unknown12 = p->unknown12;
          mon->unknown13 = p->unknown13;
          mon->spell_units = p->spell_units;

          c[50] = p->physical_strength;
          c[45] = p->poison_mode;
          c[41] = p->base_chance_1_attack;
          c[42] = p->base_chance_2_attack;
          c[43] = p->base_chance_3_attack;
          c[31] = p->intell_routine;
          c[44] = p->bane;
          c[46] = p->magic_mode;
          c[37] = p->magic_mode;
          c[58] = type;
          c[51] = (c[14]) / 3;

          mon->physical_strength = p->physical_strength;
          mon->poison_mode_2 = p->poison_mode;
          mon->base_chance_1_attack = p->base_chance_1_attack;
          mon->base_chance_2_attack = p->base_chance_2_attack;
          mon->base_chance_3_attack = p->base_chance_3_attack;
          mon->intelligence_routine = p->intell_routine;
          mon->bane = p->bane;
          mon->magic_mode_1 = p->magic_mode;
          mon->perception = p->magic_mode;
          mon->monster_type = type;
          mon->magic_level = mon->spell_units / 3;

          if (c[51] > 20) {
            c[54] = 1;
            mon->magic_mode_2 = 1;
          }

          c[24] = x;
          c[25] = y;
          c[30] = room;
          c[39] = j + 4;
          c[1] = r[i + 2];
          c[15] = 8;
          c[16] = 1;

          mon->x = x;
          mon->y = y;
          mon->current_room = room;
          mon->unknown39 = j + 4; // monster index in array? what's the good of that
          mon->damage = r[i + 2];
          mon->unknown15[0] = 8;
          mon->unknown15[1] = 1;

          zline[x][y][2] = j + 4;
          count++;

          if (r[i] == 110) {
            // Vampire, dog, or zombie?
            int mod = rnd(3);
            c[4] = 116 + mod; 
            mon->sprite_id = 116 + mod;
          }

          if (c[31] < 4) {
            mode = 1;
          }

          drawman(j + 4, x, y);

          // Clear out the monbuf[] entry
          r[i] = *(r+i+1) = *(r+i+2) = *(r+i+3) = 0;
          break;
        }
      }
    } 
  }
  
  return count;
}

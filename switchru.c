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

void stormon();
int getmon(int room);

// Switch room, and the associated housekeeping
int switchrum() {
  int d, i, j, x = 0, y = 0, xn, yn, oldrum;
  uint8_t *p = curmon[0];

  // It must be room data. There are 80 of these and crum might be "current room"
  uint8_t *r = rumdata[crum];
  room_data_t *new_r = &new_rumdata[crum];

  uint8_t *c;

  char pan[12];

  if (crum != p[30] && crum != -1 && !outside) {
    for (i = 0; i < 13; i++) {
      
      c = crumobj[i+1];

      // Each of these iterations is for one object inside the current room.
      // Copy 9 bytes for each
      for (j = 0; j < 9; j++) {
        // Copy the objects data from the current room array data back into storage in rumdata[]
        // Maybe because we're leaving the room which suggests p[30] is a room number
        r[31 + (9 * i) + j] = c[j];
        new_r->object[i].unknown[j] = c[j];
      }
    }
    check_rumdata("switchrum 1");

    // Init pursuit[3] to all zeroes
    // for(i=0;i<3;pursuit[i++] = 0)
    //   ;
    pursuit[0] = 10;
    pursuit[1] = 0;
    pursuit[2] = 0;

    for (i = 1; i < 12; i++) {
      
      c = curmon[i];
      c[35] = 0;

      if (*c == 1 && c[30] == crum && (i < 4 || c[38] == 0) && 
        (i < 4 || c[39] < 4) && c[58] != 16 &&
        c[31] < 4 && (dungeon != 2 || c[58] == 17) &&
        (i < 4 || (c[36] == 0 && c[13] > rnd(100)) ) ) {

        pursuit[2] = crum;
        xn = c[24];
        yn = c[25];
        d = dist(x,y,0,xn,yn,0);
        pursuit[0] = (pursuit[0] > d ? d : pursuit[0]);
        pursuit[1]++;
      } 
    }
  }

  if (p[30] == 127) {
    stormon();
    pursuit[0] = pursuit[1] = pursuit[2] = 0;
    return(2);
  }

  if (fromout == 1 && p[30] != crum) {
    p[4] = 125;
    outside = 1;
    fromout = 0;
  }

  if(fromout == 2)
    fromout = 1;

  p[15] = 8;
  p[16] = 1;
  x = p[24];
  y = p[25];
  p[35] = 0;
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
      c = crumobj[i];

      if (c[3] == oldrum) {
        
        x = c[6];
        y = c[7];
        p[24] = x;
        p[25] = y;

        /* must reveal hidden door */
        if (c[8] == 0) { 
          c[8] = 1;
          Vsync(); 

          blt(bitmap[c[0]], 16 * x, 16 * y, addr);

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
    for(i=1;i<12;i++) { 
      if(pan[i-1] == 1)
          curmon[i][36] = 1;
      }
  }

  getmon(crum);

  if (mode) {
    dorep();
  }

  // At the beginning of the game, we are not outside
  if (!outside) {
    if(fromout != 0) {  
      prnt(msg[crum], NULL, NULL, NULL, NULL, NULL, NULL);
    } else {
      prnt(rummsg[crum + 80 * (dungeon - 1)], NULL, NULL, NULL, NULL, NULL, NULL);
    }

    trigtrol(0);
  }

  // If the new room the player has moved into is lit
  if (new_rumdata[crum].room_has_bg) {
    
    // Search for a torch in curmon 0 to 3
    i = 0;
    for (j = 0; j < 4; j++) {
      c = curmon[j];
      
      // Some odd calculations in here
      if (c[41] > 0) { 
        // This means it was a lit torch
        i = 1;
        c[42] += 1; // Maybe this is the number of unlit torches
        c[41] = 0; // Now it's not a lit torch
        c[46] = 0;
      }
    }

    // Player was holding a lit torch when moving into a lit room
    if (i) {
      prnt("-> The lit torch is snuffed and put away.", NULL, NULL, NULL, NULL, NULL, NULL);
    }
  }

  return 0;
}

void stormon()
{
  uint8_t *c;
  char *r = monbuf;
  int i, j = 1;
  for (i = 4; i < 12; i++) {
    c = curmon[i];
    if(*c == 0 || c[30] != crum)
      continue;
    while( *(r+j) != 0 && j < 320) {
        j += 4;
        }
    if(j >= 320)
        break;
    if(c[4] > 115 && c[4] < 119)
        c[4] = 110;
    *(r+j) = c[4];
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

  for (i=1;i<320;i += 4) {
    if (*(r+i+1) == room) {
      for (j=1;j<26;j++) {
        if (permon[j].unknown4 == r[i]) {
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

      for (j = 4; j < 12; j++) {
        c = curmon[j];
        if (c[0] == 0 || c[30] != room) {

          // Clear curmon data
          for (k = 0; k < 60; k++) {
            c[k] = 0;
          }
          
          // Copy curmon (monster instance) data from permon (monster type)
          // for (k = 0; k < 15; k++) {
          //   *(c+k) = *(p+k);
          // }
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
          if (c[51] > 20) {
            c[54] = 1;
          }
          c[24] = x;
          c[25] = y;
          c[30] = room;
          c[39] = j;
          c[1] = r[i + 2];
          c[15] = 8;
          c[16] = 1;
          zline[x][y][2] = j;
          count++;
          if (r[i] == 110) {
            c[4] = 116 + rnd(3);
          }
          if (c[31] < 4) {
            mode = 1;
          }
          drawman(j, x, y);
          *(r+i) = *(r+i+1) = *(r+i+2) = *(r+i+3) = 0;
          break;
        }
      }
    } 
  }
  return count;
}

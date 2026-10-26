
#include "globals.h"
#include "startaux.h"
#include "dorep.h"
#include "gemdefs.h"
#include "storsc.h"
#include "caux.h"
#include "cinput.h"
#include "start.h"

#include "f5.h"

int i9(int pc)
{
  uint8_t *c = curmon[pc];
  curmon_t *new_c = &new_curmon[pc];
  uint8_t *z;
  char *w = (pc == 0 ? pname : name[c[3]]);
  int i,j,ret,quit = 0;
  int16_t x, y;

  if (pc > 0 && pc < 4 && curmon[0][15] != 9) {
    prnt("-> %s says 'I cannot abandon you!  I will stay with you!'", w, NULL, NULL, NULL, NULL, NULL);
    return 0;
  }

  // Anyone other than the player is doing this
  if (pc > 0) {
    c[15] = 8;
    c[16] = curmon[0][16] + 1;

    new_c->unknown15[0] = 8;
    new_c->unknown15[1] = new_curmon[0].unknown15[1];

    return 1;
  }

  if (outside) {
    x = c[24];
    y = c[25];

    // -1, 0, 1
    for (i = -1; i < 2; i++) {

      // -1, 0, 1
      for (j = -1; j < 2; j++) {
       
        if (zline[x+i][y+j][0] == 2 && i * j == 0 && (i != 0 || j != 0)) { 
          if (i == -1) {
            c[24] = 5;
            c[30] -= 1;

            // That's odd. These must be repurposed while outside.
            new_c->x = 5;
            new_c->current_room--;

            prnt("-> %s goes west...", w, NULL, NULL, NULL, NULL, NULL);
            quit = 1;
          }

          if (i == 1) {
            c[24] = 1; 
            c[30] += 1;

            new_c->x = 1;
            new_c->current_room++;


            prnt("-> %s goes east...", w, NULL, NULL, NULL, NULL, NULL);
            quit = 1;
          }

          if (j == -1) {
            c[25] = 5;
            c[30] -= 5;

            new_c->y = 5;
            new_c->current_room -= 5;

            prnt("-> %s goes north...", w, NULL, NULL, NULL, NULL, NULL);
            quit = 1;
          }

          if (j == 1) {
            c[25] = 1;
            c[30] += 5;

            new_c->y = 1;
            new_c->current_room += 5;

            prnt("-> %s goes south...", w, NULL, NULL, NULL, NULL, NULL);
            quit = 1;
          }

          if (quit) {
            clrinp();
            c[15] = 8;
            c[16] = 1;
            new_c->unknown15[0] = 8;
            new_c->unknown15[1] = 1;

            return 1;
          }
        }
      }
    }
  }

  // Quit should always be false here I think, since it would have returned from the 
  // function otherwise.
  if (outside && !quit) {
    error(ERROR_NO_EXIT);
    return 0;
  }

  domsg(5);
  header(w);
  winker = pc+1;
  sgetxy(&x,&y,0,0,0,&ret);
  winker = 0;
  undomsg();
  clrinp();
  
  if (!adjac(0,x,y)) {
    error(ERROR_LOCATION_NOT_ADJACENT);
    return 0;
  }

  z = zline[x][y];
  
  if (crumobj[z[1]][3] == 0) {
    error(ERROR_NO_EXIT);
    return 0;
  }

  c[7] = c[8] = 0;      
  c[9] = z[1];
  c[10] = crumobj[z[1]][3];
  c[15] = 9;
  c[16] = 6;

  new_c->unknown5[2] = 0;
  new_c->unknown5[3] = 0;
  new_c->unknown5[4] = z[1];
  new_c->unknown5[5] = crumobj[z[1]][3];
  new_c->unknown15[0] = 9;
  new_c->unknown15[1] = 6;

  return 1;
}

int o9(int pc)
{
  uint8_t *c = curmon[pc];
  curmon_t *new_c = &new_curmon[pc];
  int x = c[24], y = c[25];
  uint8_t *z = zline[x][y];
  
  // Player
  if (pc == 0 && crumobj[c[9]][5] == -2) {
    prnt("-> %s finds the exit is locked!", pname, NULL, NULL, NULL, NULL, NULL);
    c[30] = crum;
    new_c->current_room = crum;
    return 1;
  }

  z[2] = 0;
  if (z[5] > 0 || new_rumdata[c[30]].room_has_bg) {
    Vsync();
    storsc(storbuf[pc],16*x,16*y,1,addr);
  }

  c[30] = c[10];
  new_c->current_room = new_c->unknown5[5];

  if (pc != 0) {
    z = zline[c[26]][c[27]];
    c[30] = crumobj[z[1]][3];
    new_c->current_room = crumobj[z[1]][3];
    
    if (mode) {
      dorep();
    }
    return -1;
  }

  // Player
  if (pc == 0) {
    c[34] = 0;
    new_c->unknown33[1] = 0;

    if (mode) {
      prnt("-> %s flees!", &pname[0], NULL, NULL, NULL, NULL, NULL);
    }

    c[18] = 8;
    new_c->unknown15[3] = 8;
  }

  return -1;
}


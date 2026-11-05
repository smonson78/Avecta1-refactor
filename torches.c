
#include "globals.h"
#include "gemdefs.h"
#include "rumdraw.h"
#include "storsc.h"
#include "blt.h"
#include "curmon.h"
#include "tos_compat.h"

void drawsq(int x, int y);
int litetrol(int x, int y, int s);

int setlite(int pc, int dx, int dy)
{
  player_t *p = &players[pc];

  if (new_rumdata[crum].room_has_bg) {
    return 1;
  }

  int x = p->x;
  int y = p->y;

  Vsync();
  vbl_animation_off();
  drawsq(x, y);

  if (p->lit_torches == 0) {
    drawsq(x - dx, y - dy);
    return 0;
  }

  litetrol(x, y, 1);
  litetrol(x - dx, y - dy, 0);
  
  Vsync();
  vbl_animation_on();

  return 0;
}

// Light a torch
int litetrol(int x, int y, int s)
{
  int i,j,k,a,flag,l;

  if (new_rumdata[crum].room_has_bg) { 
    /* if room is naturally lit */
    /* don't sweat torches  */
    return 1;
  }

  for (i = -2; i <= 2; i++) { /* do that funny shaped pattern around square */
    switch (abs(i)) {
      case 2:
        j = 0;
        break;
      case 1:
        j = 1;
        break;
      default:
        j = 2;
    }

    for (k = -j; k <= j; k++) {
      if (x + i > 15 || x + i < 0 || y + k > 7 || y + k < 0 || zline[x + i][y + k][0] == 0) {
        continue; /* if off display surface or square is void */
      }

      a = zline[x + i][y + k][5]; /* fetch torch overlay number */

      switch(s) { /* s=0 ->  turn lights out, s=1 -> turn 'em on */
        case 0: /* turn out the lights, the party's over */
          if (a > 1) { /* decrement torch overlays */
            a--;
          } else { /* there is zero torch overlays */
            a = 0;
          }

          zline[x + i][y + k][5] = a; /*set zline*/

          if (a > 0) { /* another torch is carrying the lighting in the square */
            continue;
          }

          flag = 0; /* shut down flag */

          for (l = 0; l <= 3; l++) { /* don't shut down square if 1-4 charac there */
            player_t *p = &players[l];

            if (p->in_use && p->current_room == crum && p->x == x + i && p->y == y + k) {
              flag = 1;
              break;
            }
          }

          if (!flag) { /* shut down square */
            setfill(2); /* blank it */
            fillsq(x + i, y + k); /* paint it black */
          }
          break;

        case 1: /* turn on the lights, the party's started */
          flag = 0; /* is the square a blit? */
          if (a == 0) { /* then the square was black to begin with */
            flag = 1; /* so set up blit flag */
          }
          a++; /* increment the torch overlay number */
          zline[x + i][y + k][5] = a;/*set zline*/
          if (flag) {  /* blit flag is set so blit it, buster! */
            drawsq(x + i, y + k);
          }
          break;                  
      }
    }
  }

  return 0;
}

void drawsq(int x, int y)
{
  uint8_t *z = zline[x][y];
  // uint8_t *c;
  int i, pc;

  if (z[0] == 1) {
    i = 1;
  }

  if (z[0] == 2) {
    i = 0;
  }

  if (z[0] == 0) {
    i = 2;
  }

  setfill(i);
  fillsq(x,y);

  if (i == 2) {
    return;
  }

  pc = z[2];

  if (!pc) {
    //pc = (curmon[0][24] == x && curmon[0][25] == y ? 0 : -1);
    pc = (player->x == x && player->y == y ? 0 : -1);
  }

  x *= 16;
  y *= 16;
  
  // if (z[1] != 0 && crumobj[z[1]][8] == 1) {
  if (z[1] != 0 && crumobj[z[1]].visible == 1) {
    // blt(&bitmap[crumobj[z[1]][0]], x, y);
    blt(&bitmap[crumobj[z[1]].object_id], x, y);
  }
  
  // This could be a character or a monster
  if (pc >= 0) { 

    // Seems useless
    // c[0] = 0;
    // new_c->in_use = 0;

    Vsync();

    curmon[pc][0] = 1;

    storsc(storbuf[pc], x, y, 0, addr);

    if (pc < 4) {
      player_t *p = &players[pc];
      p->in_use = 1;
      blt(&bitmap[p->sprite_id], x, y);
      
    } else {
      curmon_t *p = &monsters[pc - 4];
      p->in_use = 1;
      blt(&bitmap[p->sprite_id], x, y);
    }

    // storsc(storbuf[pc], x, y, 0, addr);
    // blt(&bitmap[c[4]], x, y);
  }
  
  if (z[6] != 0) {
    blt(&bitmap[130 + 2 * (z[6])], x, y);
  }
}

void drawman(int pc, int x, int y)
{
  debug_printf("> drawman(%d, %d, %d)\n", pc, x, y);
  if (pc < 4) {
    if (players[pc].lit_torches > 0) {
      litetrol(x, y, 1);
    }
  }
 
  if (pc < 4 || zline[x][y][5] > 0 || new_rumdata[crum].room_has_bg) {
    drawsq(x, y);
  }
}

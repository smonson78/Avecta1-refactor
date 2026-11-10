#include "globals.h"
#include "gemdefs.h"
#include "rumdraw.h"
#include "torches.h"
#include "start.h"
#include "blt.h"
#include "storsc.h"
#include "caux.h"

#include "debug.h"
#include "tos_compat.h"

// I think this is "move"

int move_in(int pc)
{
  debug_printf("> move_in()\n");
  check_curmon("pre-i4");
  
  uint8_t *c = curmon[pc];
  curmon_t *new_c = &new_curmon[pc];
  int x, y;

  c[22] = c[24];
  c[23] = c[25];
  c[15] = 4;
  c[16] = c[11] > 0 ? c[11] : 1;
  new_c->unknown16[6] = new_c->x;
  new_c->unknown16[7] = new_c->y;
  new_c->command = ACTION_MOVE;
  new_c->unknown16[0] = new_c->unknown11 > 0 ? new_c->unknown11 : 1;

  nextxy(c[24], c[25], c[26], c[27], c[24], c[25], &x, &y);

  if (x - c[24] != 0 && y - c[25] != 0) {
    c[16] += c[16] / 2;
    new_c->unknown16[0] += new_c->unknown16[0] / 2;
  }

  c[28] = x;
  c[29] = y;
  c[7] = c[8] = c[9] = 0;

  new_c->unknown28[0] = x;
  new_c->unknown28[1] = y;
  
  if (pc < 4) {
    player_t *p = &players[pc];
    p->target = 0;
    p->unknown8 = 0;
    p->unknown9 = 0;
  } else {
    // Maybe this is limited to characters only
    new_c->spells[2] = 0;
    new_c->spells[3] = 0;
    new_c->spells[4] = 0;
  }

  check_curmon("i4");
  debug_printf("< i4()\n");

  return 1;
}

int move_out(int pc)
{
  debug_printf("> move_out()\n");
  uint8_t *c = curmon[pc];
  curmon_t *new_c = &new_curmon[pc];
  zline_t *z;
  char id, *w;
  int flag, xroll, yroll, i, xnow = c[24], ynow = c[25], x = c[28], y = c[29];
  int j;
  int light = new_rumdata[crum].room_has_bg;

  z = &zline[x][y];
  id = crumobj[z->visible].object_id;
  w = (pc == 0 ? &pname[0] : name[curmon[pc][3]]);

  if (z->unknown0 != 1 || (id < 41 && id > 0) || z->monster != 0) {
    i = curmon[z->monster][3];

    if (pc < 4 && z->monster != 0 && pc != z->monster) {
      if (z->monster > 3 && i != 10) {
        prnt("-> The %s blocks %s!", name[i], w, NULL, NULL, NULL, NULL);
      } else {
        prnt("-> %s blocks %s!", name[i], w, NULL, NULL, NULL, NULL);
      }
    }

    return 1;
  }

  // if (pc > 0 && z->monster == 0 && x == curmon[0][24] && y == curmon[0][25]) {
  if (pc > 0 && z->monster == 0 && x == player->x && y == player->y) {
    return 1;
  }

  if (pc == 0) {
    if (c[18] != 4) {
      prnt("-> %s moves...", &pname[0], NULL, NULL, NULL, NULL, NULL);
    }

    c[18] = 4;
    new_c->unknown16[2] = 4;
  }

  xroll = 2 * (x - xnow) / (1 + outside);
  yroll = 2 * (y - ynow) / (1 + outside);
  z = &zline[xnow][ynow];
  Vsync();
  vbl_animation_off();
  x = 16 * xnow;
  y = 16 * ynow;

  if (!light && z->unknown5 == 0) {
    flag = 1;
  } else {
    flag = 0;
  }

  if (!light && zline[c[28]][c[29]].unknown5 == 0) {
    if (pc < 4) {
      drawsq(c[28], c[29]);
    } else {
      z->monster = 0;
      
      if (z->unknown5 > 0) {
        drawsq(xnow, ynow);
      }

      c[24] = c[28];
      c[25] = c[29];
      new_c->x = new_c->unknown28[0];
      new_c->y = new_c->unknown28[1];

      zline[c[24]][c[25]].monster = pc;

      return 1;
    }
  }

  for (i = 1; i < 9 + 8 * outside; i++) {
    x += xroll;
    y += yroll;

    if (outside) {
      for (j = 0; j < 2; j++) {
        Vsync();
      }
    }
    Vsync();

    if (i > 1 || pc < 4 || !flag) {
      storsc(storbuf[pc], x - xroll, y - yroll, 1, addr); /* restores the screen */
    }

    storsc(storbuf[pc], x, y, 0, addr);

    blt(&bitmap[c[4]], x, y);
  }

  c[34] = 0;
  c[35] = 0;
  c[24] = c[28];
  c[25] = c[29];
  new_c->unknown33[1] = 0;
  new_c->stink_lines = 0;
  new_c->x = new_c->unknown28[0];
  new_c->y = new_c->unknown28[1];

  z->monster = 0; /* reset the zline array to reflect move */

  zline[c[24]][c[25]].monster = pc;

  if (pc < 4) {
    setlite(pc, c[24] - xnow, c[25] - ynow);
  }

  if (!light && z->unknown5 == 0) {
    setfill(2);
    fillsq(xnow, ynow);
  }

  if (z->unknown6) {
    blt(&bitmap[132], 16 * xnow, 16 * ynow);
  }

  Vsync();
  vbl_animation_on();

  if (c[28] == c[26] && c[29] == c[27]) {
    return 1; /* since the move is complete */
  }

  nextxy(c[22], c[23], c[26], c[27], c[24], c[25], &x, &y);

  c[16] = c[11] > 0 ? c[11] : 1;
  new_c->unknown16[0] = new_c->unknown11 > 0 ? new_c->unknown11 : 1;

  if (x - c[24] != 0 && y - c[25] != 0) {
    c[16] += c[16] / 2;
    new_c->unknown16[0] += new_c->unknown16[0] / 2;
  }

  c[28] = x;
  c[29] = y;
  new_c->unknown28[0] = x;
  new_c->unknown28[1] = y;

  check_curmon("o4");

  return 0; /* the move is not complete otherwise */
}

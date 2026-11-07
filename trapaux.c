#include "tos_compat.h"

#include "globals.h"
#include "caux.h"
#include "gemdefs.h"
#include "instring.h"
#include "start.h"
#include "explode.h"
#include "torches.h"
#include "damage.h"

#include "debug.h"

int rumslot();

int destroy(int type, int num)
{
  int x,y;
  // int i;
  uint8_t *z;
  uint8_t *c;
  room_object_t *object;

  if (type == 1) {
    object = &crumobj[num];
    // x = c[6];
    // y = c[7];
    x = object->x;
    y = object->y;
  } else {
    c = curmon[num];
    x = c[24];
    y = c[25];
  }

  z = zline[x][y];
  
  if (type == 1) {
    // for (i = 0; i < 9; i++) {
    //   c[i] = 0;
    // }
    memset(object, 0, sizeof(room_object_t));

    z[1] = 0;

    if (z[5] > 0 || new_rumdata[crum].room_has_bg) {
      drawsq(x, y);
    }
  }

  if (type == 2) {
    damage(num, c[2] + c[12]);
  }

  return 1;
}

int empty(int num)
{
  // crumobj[num][5] = 0;
  crumobj[num].hide = 0;
  return 1;
}

int make(int id, int take, int trap, int con, int nmsg, int hide, int x, int y, int vis)
{
  int i;
  room_object_t *c;

  i = zline[x][y][1];

  if (i != 0) {
    destroy(1, i);
  }

  i = rumslot();

  if (i > 13) {
    destroy(1,13);
    i = 13;
  }

  c = &crumobj[i];

  // c[0] = id;
  // c[1] = take;
  // c[2] = trap;
  // c[3] = con;
  // c[4] = nmsg;
  // c[5] = hide;
  // c[6] = x;
  // c[7] = y;
  // c[8] = vis;
  c->object_id = id;
  c->movable = take;
  c->trap = trap;
  c->target = con;
  c->nmsg = nmsg;
  c->hide = hide;
  c->x = x;
  c->y = y;
  c->visible = vis;

  zline[x][y][1] = i;

  // if (c[8] == 1 && (new_rumdata[crum].room_has_bg == 1 || zline[x][y][5] == 1)) {
  if (c->visible == 1 && (new_rumdata[crum].room_has_bg == 1 || zline[x][y][5] == 1)) {    
    drawsq(x, y);
  }
  return i;
}

// Never called
// void settrap(int num, int trap)
// {
//   crumobj[num][2] = trap;
//   triglist[0]++;
//   triglist[num] = trap;
// }

int invtrap(char obj)
{
  int i, j, t=0;

  for (i = 0; i < 20; i++) {
    if (invtrig[i] == obj || obj == 69 || obj == 65 || obj == 63) {
      t=1;
    }
  }

  if (t == 0) {
    return 0;
  }

  for (j = 14; j < 25; j++) {
    if (triglist[j] == obj) {
      return 1;
    }
  }

  j = 14;
  while (triglist[j] != 0 && j < 25) {
    j++;
  }

  if (j == 25) {
    return 0;
  }

  triglist[0]++;
  triglist[j] = obj;  /* REMEMBER! the object #s (40-80) are reserved traps!!! */
  return 1;
}

int untrap(int num)
{
  // crumobj[num][2] = 0;
  crumobj[num].trap = 0;
  triglist[num] = 0;
  triglist[0]--;
  return 1;
}

// These are never called
// // Connect door object to room
// int conn(int robj, int room)
// {
//   // crumobj[robj][3] = room;
//   crumobj[robj].target = room;
//   return 1;
// }

// // Disconnect door object from room
// int discon(int robj)
// {
//   // crumobj[robj][3] = 0;
//   crumobj[robj].target = 0;
//   return 1;
// }

int blowup(int x, int y)
{
  uint8_t *z = zline[x][y];
  room_object_t *c;
  int i;

  for (i = 0; i < 5; i++) {
    explode(x, y, 0);
  }

  // if (z[2] > 0 || (curmon[0][24] == x && curmon[0][25] ==y) ) {
  if (z[2] > 0 || (player->x == x && player->y == y)) {
    destroy(2, z[2]);
  }

  // if (z[1] > 0 && crumobj[z[1]][0] != 49) {
  if (z[1] > 0 && crumobj[z[1]].object_id != OBJECT_RUNE) {
    c = &crumobj[z[1]];
    // if (c[3] != 0 || c[2] != 0) {
    if (c->target != 0 || c->trap != 0) {
      return 1;
    }
    destroy(1, z[1]);
  }

  return 1;
}

int getword(char *comp)
{
  prnt("-> [Type answer]: ", NULL, NULL, NULL, NULL, NULL, NULL);
  instring(row, col, junk, 18, 1);
  hold = 0;
  usedline = 0;

  if (strcmp(junk, comp) != 0) {
    return 0;
  } else {
    return 1;
  }
}

int rumslot()
{
  // Search through crumobj[18][9] looking for one with nothing in it.
  // If no spaces found, we'll "destroy" some fixed item and zero out part of the current room data.
  // Note bug here, it should really stop at 17 but continues to 18 which is the 19th one.
  for (int i = 1; i < 19; i++) {
    // if (crumobj[i][0] == 0) {
    if (crumobj[i].object_id == 0) {      
      return i;
    }
  }

  destroy(1, 18);
  // rumdata[crum][152] = 0;
  new_rumdata[crum].room_unknown_3[4] = 0;

  // check_rumdata("rumslot");

  // I guess we're saying "use slot 18". Did we destroy item 18 in the array to make space?
  // Or does 18 mean "not found" since the array actually ends at 17?
  return 18;
}

int slotnum()
{
  int i, j = 0;
  for (i = 1; i < 19; i++) {
    // if (crumobj[i][0] == 0) {
    if (crumobj[i].object_id == 0) {
      j++;
    }
  }

  return j;
} 

// Never called
// int thief(int pc)
// {
//   // Monster
//   if (pc > 3) {
//     return 0;
//   }

//   // Player character: skill check
//   if (curmon[pc][37] > rnd(100)) {
//     return 1;
//   }

//   return 0;
// }


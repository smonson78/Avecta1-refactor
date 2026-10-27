#include "globals.h"
#include "start.h"
#include "startaux.h"
#include "caux.h"
#include "cinput.h"
#include "tos_compat.h"

#include "f5.h"

/******************************************************************************/
/* ROB/LISTEN is function 10.  This attempts to open locked objects, and is   */
/* highly dependent on thieving abilities.  It only works with room objects   */
/* and therefore parses intent from the nature of the object.  If it is locked*/
/* the object is assumed to be jimmied.                                       */
/******************************************************************************/

int i10(int pc)
{
  debug_printf("> i10()\n");
  int16_t x, y;
  int obj, ret;
  uint8_t *c = curmon[pc];
  curmon_t *new_c = &new_curmon[pc];
  char *w = (pc == 0 ? pname : name[c[3]]);

  if (outside) {
    return 0;
  }

  if (c[45] > 0 && c[46] > 0) {
    error(ERROR_HANDS_FULL);
    return 0;
  }

  header(w);
  winker = pc + 1;
  domsg(2);
  sgetxy(&x, &y, 0, 0, 0, &ret);
  undomsg();
  winker = 0;
  clrinp();
  obj = zline[x][y][1];
  if (obj == 0 || (crumobj[obj][5] != -2 && crumobj[obj][3] < 1) ) {
    error(ERROR_NOTHING_TO_ROOK_SNOOP);
    return 0;
  }

  if (!adjac(pc, x, y)) {
    error(ERROR_LOCATION_NOT_ADJACENT);
    return 0;
  }

  c[7] = c[8] = c[9] = 0;
  c[5] = x;
  c[6] = y;
  c[9] = obj;
  c[15] = 10;
  c[16] = 6;
  c[34] = 0;

  new_c->unknown5[2] = 0;
  new_c->unknown5[3] = 0;
  //new_c->unknown5[4] = 0;

  new_c->unknown5[0] = x;
  new_c->unknown5[1] = y;
  new_c->unknown5[4] = obj;
  new_c->unknown15[0] = 10;
  new_c->unknown15[1] = 6;
  new_c->unknown33[1] = 0;

  debug_printf("< i10()\n");
  return 1;
}

int o10(int pc)
{
  debug_printf("> o10()\n");
  int x, y, object, flag=0;
  uint8_t *c = curmon[pc], *o;
  curmon_t *new_c = &new_curmon[pc];
  //uint8_t *r;
  room_data_t *new_r;
  char *w = (pc == 0 ? pname : name[c[3]]);

  x = c[5];
  y = c[6];

  object = c[9];
  o = crumobj[object];
  // r = rumdata[o[3]];
  new_r = &new_rumdata[o[3]];

  if (object != zline[x][y][1]) {
    return 1;
  }

  if (rnd(100) < c[37] && o[5] == -2) {
    if (o[3] == 0) {
      o[5] = 1;
    } else {
      if (o[2] == 0) {
        o[5] = -1;
      } else {
        return 1;
      }
    }
    prnt("-> %s unlocks the %s!", w, obj[o[0]], NULL, NULL, NULL, NULL);
    flag = 1;
  }

  //if (o[3] != 0 && r[22] != 0 && rnd(100) < r[26] && rnd(100) < c[37]) {
  if (o[3] != 0 && new_r->room_monster_type != 0 && (rnd(100) < new_r->room_monsters_chance) && rnd(100) < c[37]) {
    flag = 1; 
    prnt("-> %s hears one or more %ss through the %s!", w,
        //name[permon[r[22]].unknown3],
        name[permon[new_r->room_monster_type].monster_name_id],
        obj[o[0]],
        NULL, NULL, NULL);
  }

  if (flag == 0 && c[18] != 10) {
    prnt("-> %s has no success with the %s!", w, obj[o[0]], NULL, NULL, NULL, NULL);
  }

  c[18] = 10;
  new_c->unknown15[3] = 10;

  debug_printf("< o10()\n");
  return 1;
}

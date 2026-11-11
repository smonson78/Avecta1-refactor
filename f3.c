#include "tos_compat.h"

#include "globals.h"
#include "caux.h"
#include "startaux.h"
#include "words.h"
#include "start.h"
#include "cinput.h"

#include "f5.h"

#include "tos_compat.h"

/******************************************************************************/
/* I3(NPC) initializes an EXAMINE action.  It simply determines if the charac */
/* can see the object.                                                        */
/******************************************************************************/

int examine_in(int pc)
{
  debug_printf("> i3() (examine)\n");
  int16_t x, y;
  int i, bot = 0, top1 = 0, type, ret;
  uint8_t *c = curmon[pc];
  player_t *new_c = &players[pc];
  char z1, z2, scratch[15];
  char *w = (pc == 0 ? pname : name[*(c + 3)]);

  if (invnpc[pc][0] == 0 && c[45] == 0 && (c[46] == 0 || c[46] == 81)) {
    type = 0;
  } else {
    top(1);
    clrinp();
    bot = 2 + listinv(pc, scratch);
    type = 1;
    top1 = 3;
  }

  if (type == 0) {
    header(w);
    domsg(6);
  } else {
    header("Examine");
  }

  winker = pc + 1;
  sgetxy(&x, &y, type, top1, bot, &ret);
  if (!type) {
    undomsg();
  }

  winker = 0;
  top(2);
  clrinp();

  if (ret == 0) {
    z1 = zline[x][y].visible;
    z2 = zline[x][y].monster;

    if (!los(pc, x, y)) {
      error(ERROR_NO_LINE_OF_SIGHT);
      return 0;
    }
    
    if (z2 == 0 && z1 != 0 && !adjac(pc, x, y)) {
      error(ERROR_LOCATION_NOT_ADJACENT);
      return 0;
    }

    if (z1 != 0 && z2 != 0) {
      error(ERROR_OBJECT_IS_CLOSED);
      return 0;
    }

    if (!z1 && !z2 && (c[24] != x || c[25] != y)) {
      error(ERROR_NOTHING_TO_EXAMINE);
      return 0;
    }

    if (c[24] == x && c[25] == y && !c[45] && (!c[46] || c[46] == 81)) {
      error(ERROR_NOTHING_TO_EXAMINE);
      return 0;
    }

    *(c + 7) = *(c + 8) = *(c + 9) = 0;
    *(c + 10) = (!z1 ? 2 : 1);
    *(c + 7) = z2;
    *(c + 9) = z1;
    //new_c->unknown7 = 0;
    new_c->unknown8 = 0;
    //new_c->unknown9 = 0;
    new_c->spell_cast = (!z1 ? 2 : 1);
    new_c->target = z2;
    new_c->unknown9 = z1;

    if (c[24] == x && c[25] == y) {
      c[10] = 3;
      c[8] = (c[45] == 0 ? c[46] : c[45]);
      new_c->spell_cast = 3;
      new_c->unknown8 = (new_c->good_hand == 0 ? new_c->other_hand : new_c->good_hand);
      
      if (c[45] != 0 && c[46] != 0 && c[46] != 81) {
        // Get the number of the button clicked by the user on modal 31, which is
        // "Which hand's object? 1=Good, 2=Other"
        i = error(ERROR_WHICH_HANDS_OBJECT);
        c[8] = c[44 + i];
        new_c->unknown8 = i == 1 ? new_c->good_hand : new_c->other_hand;
      }

      c[7] = c[9] = 0;
      new_c->target = 0;
      new_c->unknown9 = 0;
    }
  } else {
    c[10] = 3;
    c[8] = scratch[ret];
    new_c->spell_cast = 3;
    new_c->unknown8 = scratch[ret];
  }

  c[15] = 3;
  c[16] = 6;
  c[34] = 0;
  new_c->command = ACTION_EXAMINE;
  new_c->time_until_next_move = 6;

  return 1;
}

/*****************************************************************************/
/* O3(pc) is a call to output an EXAMINE function.                           */
/*****************************************************************************/

int examine_out(int pc)
{
  uint8_t *c = curmon[pc];
  player_t *new_c = &players[pc];
  char *w, *w1, scratch[10];
  uint8_t *t;
  curmon_t *new_t;
  int flag, type, x = c[24], y = c[25], object = 0, mess = 0;
  
  // This will be 1 for a crumobj, or anything else for a curmon
  type = c[10];

  // Player name or monster name
  w1 = (pc == 0 ? &pname[0] : name[new_c->monster_name_id]);

  if ((type == 3 && (object = c[8]) > 40) ||
      (!outside && type == 1 && (object = c[9]) > 40)) {
    if (type == 3 && c[8] != c[45] && c[8] != c[46]) {
      if (!handman(pc, c[8], 0, 1)) {
        return 1;
      }

      if (c[46] == 0) {
        c[46] = c[8];
        new_c->other_hand = new_c->unknown8;
      } else {
        c[45] = c[8];
        new_c->good_hand = new_c->unknown8;
      }

      takeout(pc, c[8]);
      object = c[8];
      prnt("-> %s takes out the %s to examine.", w1, obj[object], NULL, NULL, NULL, NULL);
    }

    prnt("-> %s examines the %s.", w1, obj[object], NULL, NULL, NULL, NULL);

    if (!new_rumdata[crum].room_has_bg && !zline[x][y].unknown5) {
      prnt("-> It is too dark for %s to see anything!", w1, NULL, NULL, NULL, NULL, NULL);
      return 1;
    }

    prnt(msg[object], NULL, NULL, NULL, NULL, NULL, NULL);
    return 1;
  }

  if (type == 2) {
    object = c[7];
  }

  if (type == 2 && !los(object, c[24], c[25])) {
    prnt("-> The object which %s was examining has moved out of sight.",
         w1, NULL, NULL, NULL, NULL, NULL);
    return 1;
  }

  if (type == 1) {
    object = c[9];
  }

  // This isn't great for structures...
  // t = (type == 1 ? crumobj[object] : curmon[object]);
  room_object_t *obj_t;
  t = curmon[object];

  if (type != 1) {
    new_t = &new_curmon[object];
  } else {
    obj_t = &crumobj[object];
  }

  if (type == 2) {
    // Get character name
    // w = name[t[3]];
    w = name[new_t->monster_name_id];
  } else {
    // Get object name
    // w = obj[t[0]];
    w = obj[obj_t->object_id];

    // if (t[4] > 0 && t[0] < 41) {
    if (obj_t->nmsg > 0 && obj_t->object_id < OBJECT_BROADSWORD) {
      // mess = t[4] + (1 - outside) * 80 * dungeon;
      mess = obj_t->nmsg + (1 - outside) * 80 * dungeon;
      if (fromout > 0) {
        mess -= 80;
      }
    }

    // if (t[4] > 0 && t[0] > 40) {
    if (obj_t->nmsg > 0 && obj_t->object_id > OBJECT_DEAD_EIRIK) {
      // mess = t[4];
      mess = obj_t->nmsg;
    }

    // if (t[4] == 0) {
    if (obj_t->nmsg == 0) {
      // mess = 0;
      mess = 0;
    }
  }

  if (!outside && !fromout && (type != 2 || c[7])) {
    prnt("-> %s examines the %s.", w1, w, NULL, NULL, NULL, NULL);
  }

  // Curmons:
  if (type == 2) {
    if (t[39] != pc) {
      if (t[39] == 0) {
        w1 = pname;
      }

      if (t[39] > 0 && t[39] < 4) {
        w1 = name[curmon[t[39]][3]];
      }

      if ((t[39] > 3 || t[38] == 1) && t[31] < 3) {
        if (mode) {
          sprintf(scratch, "%s", "its enemy");
        } else {
          if (t[38]) {
            sprintf(scratch, "%s", "a friend");
          }
        }

        w1 = scratch;
      }
    }

    prnt(monmsg[t[3]], w1, NULL, NULL, NULL, NULL, NULL);

    return 1;
  }

  flag = 0;

  // Crumobjs:
  // if (type == 1 && mess == 0 && t[5] == -1 && t[3] == 0 && (t[2] == 0 || rnd(100) > c[37])) {
  if (type == 1 && mess == 0 && obj_t->hide == -1 && obj_t->target == 0 
      && (obj_t->trap == 0 || rnd(100) > c[37])) {
    prnt("-> %s notices nothing unusual.", w1, NULL, NULL, NULL, NULL, NULL);
    return 1;
  }

  // Crumobjs:
  // if (!outside && type == 1 && t[2] != 0 && c[37] > rnd(100)) {
  if (!outside && type == 1 && obj_t->trap != 0 && c[37] > rnd(100)) {
    flag = 1;
    prnt("-> %s senses something strange about the %s.", w1, w, NULL, NULL, NULL, NULL);
  }

  if (mess != 0) {
    prnt(msg[mess], NULL, NULL, NULL, NULL, NULL, NULL);
    return 1;
  }

  // Crumobjs:
  // if (!outside && type == 1 && mess == 0 && t[3] != 0) {
  // FIXME mess is always 0 here as per above
  if (!outside && type == 1 && mess == 0 && obj_t->target != 0) {
    flag = 1;
    prnt("-> %s notices an exit through the %s.", w1, w, NULL, NULL, NULL, NULL);
  }

  // Crumobjs:
  // if (type == 1 && t[5] == -2) {
  if (type == 1 && obj_t->hide == -2) {
    flag = 1;
    prnt("-> %s notices the %s is locked.", w1, w, NULL, NULL, NULL, NULL);
  }

  // Crumobjs:
  // FIXME mess is always 0 here as per above
  if ((type == 1 || mess == 0) && !flag) {
    prnt("-> %s notices nothing unusual.", w1, NULL, NULL, NULL, NULL, NULL);
  }

  c[18] = 3;
  new_c->unknown18 = 3;
  
  return 1;
}

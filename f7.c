#include "globals.h"
#include "attack.h"
#include "caux.h"
#include "trapaux.h"
#include "trapres.h"
#include "torches.h"
#include "start.h"
#include "startaux.h"
#include "cinput.h"
#include "tos_compat.h"

#include "f5.h"


/******************************************************************************/
/* I7() is a  call to initialize a TAKE/DROP.      It must be adjacent and    */
/* takeable, which means its type must be greater than 40.                    */
/******************************************************************************/

int take_drop_in(int pc)
{
  uint8_t *c = curmon[pc];
  player_t *new_c = &players[pc];

  uint8_t *z;
  char scratch[15],*w = (pc == 0 ? pname : name[c[3]]);
  int object,k,l,self,bot=0,top1=3,type,ret=0;
  int16_t x, y;

  if (outside) {
    return 0;
  }

  if (invnpc[pc][0] == 0 && c[45] == 0 && (c[46] == 0 || c[46] == 81) ) {
    type = 0;
  } else {
    bot = 2 + listinv(pc, scratch);
    type = 1;
  }

  if (type == 0) {
    domsg(2);
  }

  if (!type) {
    header(w);
  } else {
    header("Drop");
    prhand(pc);
  }

  winker = pc + 1;
  sgetxy(&x, &y, type, top1, bot, &ret);
  if (type == 0) {
    undomsg();
  }
  winker = 0;
  top(1);
  clrinp();
  
  //self = (x == c[24] && y == c[25] ? 1 : 0);
  self = x == new_c->x && y == new_c->y ? 1 : 0;

  if (ret == 0 && !self) {
    if ((object = zline[x][y][1]) == 0) {
      error(ERROR_NOTHING_TO_TAKE);
      return 0;
    }
    
    // if (crumobj[object][1] != 1 || crumobj[object][0] < 41 ) {
    if (crumobj[object].movable != 1 || crumobj[object].object_id < OBJECT_BROADSWORD ) {
      error(ERROR_NOTHING_TO_TAKE);
      return 0;
    }
    
    if (!adjac(pc, x, y)) {
      error(ERROR_NOTHING_TO_TAKE);
      return 0;
    }
    
    if (invnpc[pc][0] == 13) {
      error(ERROR_NOTHING_TO_TAKE);
      return 0;
    }
  }

  c[7] = c[8] = c[9] = 0;      
  new_c->unknown7 = 0;
  new_c->unknown8 = 0;
  new_c->unknown9 = 0;
  
  if (ret == 0 && !self) {
    c[9] = zline[x][y][1];
    new_c->unknown9 = zline[x][y][1];
    c[10] = 0;
    c[15] = 7;  
    c[16] = 6;
    new_c->spell_cast = 0;
    new_c->command = ACTION_TAKE_DROP;
    new_c->unknown16[0] = 6;
    return 1;
  } else {
    /* this is a drop out of inventory */
    if (self) {
      if (c[45] == 0 && (c[46] == 0 || c[46] == 81)) {
        error(ERROR_NOTHING_IN_HAND);
        return 0;
      }
      
      if (c[45] > 0 && c[46] > 0 && c[46] != 81) {
        l = error(ERROR_WHICH_HANDS_OBJECT);
      } else {
        l = ( c[46] == 0 || c[46] == 81 ? 1 : 2);
      }
      ret = l;
    }

    header(w);
    domsg(4);
    winker = pc + 1;
    sgetxy(&x, &y, 0, 0, 0, &k);
    winker = 0;
    undomsg();

    if (!adjac(pc,x,y)) {
      error(ERROR_LOCATION_NOT_ADJACENT);
      return 0;
    } 

    z = zline[x][y];
    // if (z[0] == 0 || ( (k = z[1] ) != 0 && crumobj[k][5] < 0) ) {
    if (z[0] == 0 || ( (k = z[1] ) != 0 && crumobj[k].hide < 0) ) {
      error(ERROR_OBJECT_IS_CLOSED);
      return 0;
    }

    c[8] = scratch[ret];
    new_c->unknown8 = scratch[ret];
    c[7] = c[9] = 0;
    new_c->unknown7 = 0;
    new_c->unknown9 = 0;

    if (k != 0) {
      c[9] = k;
      new_c->unknown9 = k;
    }

    c[5] = x;
    new_c->unknown5 = x;
    c[6] = y;
    new_c->unknown6 = y;

    c[10] = 1;
    new_c->spell_cast = 1;
    c[15] = 7;
    new_c->command = ACTION_TAKE_DROP;
    c[16] = 2;
    new_c->unknown16[0] = 2;

    //if (c[8] != c[45]) {
    if (new_c->unknown8 != new_c->good_hand) {
      c[16] += 4;
      new_c->unknown16[0] += 4;
    }

    return 1;
  }
}

/****************************************************************************/
/* TAKE/DROP output function.                                               */
/****************************************************************************/

// pc = player (0) or character (1 or more)
int take_drop_out(int pc)
{
  uint8_t *c = curmon[pc];
  curmon_t *new_c = &new_curmon[pc];
  curmon2_t *ch = &new_curmon2[pc];

  player_t *p = &players[pc];

  uint8_t *t;
  room_object_t *z;
  char *w, *w1 = (pc == 0 ? pname : name[c[3]]);

  // Interesting these seem to apply only to player characters. But further down,
  // there a conditional to check whether pc < 4.
  //int i, k, l, inv = c[8], r = c[9],x = c[5],y = c[6],j = c[10], id;
  int i, k, l, inv = p->unknown8, r = p->unknown9;
  int x = p->unknown5, y = p->unknown6, j = p->spell_cast, id;

  debug_printf("> o7\n");

  c[18] = 7;
  ch->monster.unknown16[2] = 7;

  if (j > 0) {
    r = 0;
  }
  
  if (!handman(pc, inv, r, j)) {
    return 1;
  }

  if (j > 0 && (k = zline[x][y][1]) == 0 && slotnum() == 0) {
    prnt("-> Supernatural forces prevent %s from dropping the %s!", w1,
        obj[c[8]], NULL, NULL, NULL, NULL);
    return 1;
  }

  // FIXME this hangs the game when putting the scimitar into the door. In the original 
  // executable, you get the error "The door is closed" instead
  if (j > 0 && k != 0) {
    // prnt("-> %s puts the %s in the %s.", w1, obj[c[8]], obj[crumobj[k][0]], NULL, NULL, NULL);
    prnt("-> %s puts the %s in the %s.", w1, obj[c[8]], obj[crumobj[k].object_id], NULL, NULL, NULL);
    // crumobj[k][5] = 1;
    crumobj[k].hide = 1;
    takeout(pc, c[8]);

    if (c[45] == c[8]) {
      c[45] = 0;
      ch->player.good_hand = 0;
    } else {
      //if (c[46] != OBJECT_TORCH && c[45] != 0 && c[46] == c[8]) {
      if (p->other_hand != OBJECT_TORCH && p->good_hand != 0 && p->other_hand == p->unknown8) {
        c[46] = 0;
        ch->player.other_hand = 0;
      }
    }

    putinto(k, c[8]);  
  }

  if (j > 0 && k == 0) {
    c[9] = 0;
    if (pc < 4) {
      ch->player.unknown9 = 0;
    } else {
      ch->monster.spells[4] = 0;
    }

    i = rumslot();
    // prnt("-> %s puts the %s down.", w1, obj[c[8]], NULL, NULL, NULL, NULL);
    prnt("-> %s puts the %s down.", w1, obj[ch->player.unknown8], NULL, NULL, NULL, NULL);
    // z = crumobj[i];
    z = &crumobj[i];
    
    if (i > 13) { 
      storobj(c[8], x, y);
      // Old
      // rumdata[crum][134+i] = c[8];
      // New
      new_rumdata[crum].room_unknown_3[i - 14] = c[8];

      // check_rumdata("o7");
    }

    //takeout(pc, c[8]);
    takeout(pc, new_c->spells[3]);

    //if (c[45] == c[8]) {
    if (ch->player.good_hand == ch->player.unknown8) {
      c[45] = 0;
      ch->player.good_hand = 0;
    }

    //if (c[46] == c[8]) {
    if (ch->player.other_hand == ch->player.unknown8) {
      c[46] = 0;
      ch->player.other_hand = 0;
    }

    if (pc < 4 && c[32] == c[8]) {
      c[32] = 0;
      new_c->poison_mode_1 = 0;
      for (j = 1; j < 14; j++) {
        if (invnpc[pc][j] < 46 && invnpc[pc][j] > 40) {
          c[32] = invnpc[pc][j];
          new_c->poison_mode_1 = invnpc[pc][j];
          break;
        }
      }
    }

    // z[0] = c[8];
    // z[0] = ch->player.unknown8;
    // z[1] = 1;
    // z[6] = x;
    // z[7] = y;
    // z[4] = ch->player.unknown8;
    // z[5] = -1;
    // z[8] = 1;
    // z[2] = z[3] = 0;

    z->object_id = ch->player.unknown8;
    z->movable = 1;
    z->x = x;
    z->y = y;
    z->nmsg = ch->player.unknown8;
    z->hide = -1;
    z->visible = 1;
    z->trap = 0;
    z->target = 0;

    zline[x][y][1] = i;

    if (zline[x][y][5] > 0 || new_rumdata[crum].room_has_bg) {
      drawsq(x,y);
    }
  }

  if (j > 0) {
    // This clearly overflows invtrig[20]! Let's reduce it.
    // for (k = 0; k < 40; k++) {
    for (k = 0; k < 20; k++) {
      // if (c[8] == 0) {
      if (ch->player.unknown8 == 0) {
        break;
      }
      
      if (invtrig[k] == c[8]) {
        for (l = 1; l < 14; l++) {
          if (triglist[l] == c[8] ) {
            t = trigval[triglist[l]];
            if ((t[0] & 16) && t[3] == 7) {
              // trapres(pc, t[5], c[24], c[25]);
              trapres(pc, t[5], ch->player.x, ch->player.y);
            }
            triglist[l] = 0;
            triglist[0]--;
            break;
          }
        }
      }
    }
  }

  if (j > 0 && c[8] == 80) {
    c[52] = c[30];
    c[53] = x + (16 * y);
    new_c->seekshard_imprinted_room = new_c->current_room;
    new_c->seekshard_imprinted_xy = x + (16 * y);

    prnt("-> The seeking shard imprints the room!", NULL, NULL, NULL, NULL, NULL, NULL);
  }

  if (j == 0) {
    // x = crumobj[c[9]][6];
    // y = crumobj[c[9]][7];
    x = crumobj[c[9]].x;
    y = crumobj[c[9]].y;
    if (!adjac(pc,x,y)) {
      return 1;
    }

    id = zline[x][y][1];

    // k = crumobj[id][0];
    k = crumobj[id].object_id;
    if (k < 41) {
      return 1;
    }

    l = (c[45] == 0 ? 0 : 1);
    c[45 + l] = k;
    // Ugh
    if (l == 0) {
      new_c->poison_mode_2 = k;
    } else {
      new_c->magic_mode_1 = k;
    }

    invtrap(k);
    w = obj[k];

    prnt("-> %s takes the %s into hand.", w1, w, NULL, NULL, NULL, NULL); 
    // for (j = 0; j < 9; j++) {
    //   // Zero out all of crumobj[id] because the object is no longer in the room
    //   crumobj[id][j] = 0;
    // }
    memset(&crumobj[id], 0, sizeof(room_object_t));

    if (id > 13) {
      // Delete object from room
      // I would think crumobj[id][0] is always 0 here given the last block.
      // xobj(crumobj[id][0], crum);
      xobj(crumobj[id].object_id, crum);

      // Old
      // rumdata[crum][134 + id] = 0;
      // New
      new_rumdata[crum].room_unknown_3[id - 14] = 0;
      // check_rumdata("o7 position 2");
    }

    zline[x][y][1] = 0;

    c[5] = x;
    c[6] = y;
    c[8] = k;
    new_c->spells[0] = x;
    new_c->spells[1] = y;
    new_c->spells[3] = k;

    if (k < 46 && k > 40 && c[32] == 0) {
      c[32] = k;
      new_c->poison_mode_1 = k;
    }

    attack(pc, x, y);

    if (new_rumdata[crum].room_has_bg || zline[x][y][5] > 0) {
      drawsq(x,y);
    }
  }

  debug_printf("< o7\n");
  return 1;
}


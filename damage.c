#include <libc.h>

#include "globals.h"
#include "gemdefs.h"
#include "switchru.h"
#include "dorep.h"
#include "torches.h"
#include "caux.h"
#include "trapaux.h"
#include "storsc.h"
#include "text.h"
#include "start.h"
#include "blt.h"
#include "toggle.h"

#include "tos_compat.h"
#include "debug.h"

// pc = monster/character index
// level = 
int damage(int pc, int level)
{
  uint8_t *c = curmon[pc];
  curmon_t *new_c = &new_curmon[pc];
  room_object_t *z;
  zline_t *z1;

  check_curmon("damage 0");

  char *w = (pc == 0 ? pname : name[c[3]]);
  int newpolice = 1, old, i, j, k, l;
  int x = c[24], y = c[25];
  //int x = new_c->x, y = new_c->y;

  if (police) {
    newpolice = 0;
  }

  if (c[30] != crum) {
  //if (new_c->current_room != crum) {
    return 1;
  }
    
  int light = new_rumdata[crum].room_has_bg;

  level -= c[12];
  //level -= new_c->unknown12;
  level = level < 1 ? 0 : level;

  old = c[1];
  //old = new_c->damage;
  
  c[1] += level;
  new_c->damage += level;
  
  // Keep the smaller of the two values
  c[1] = (c[1] > c[2] ? c[2] : c[1]);
  new_c->damage = new_c->damage > new_c->max_health ?
    new_c->max_health : new_c->damage;

  if (pc > 3 && c[31] < 4 && mode) {
    i = (c[1] * 4) / (c[2]);
    if (i > 3) {
      c[0] = 0;
      new_c->in_use = 0;

      i = 4;
    }

    if (c[36] == 1 && c[1] < c[2]) {
      i = 5;
    }

    if (c[38] && c[0] == 1) {
      i = 6;
    }

    Vsync();
    textsix(0, 313, 1 + 8 * (c[33]), 1, " ");
    textsix(1, 313, 1 + 8 * (c[33]), 1, mod[i]);
  }

  if (level == 0) {
    return 1;
  }

  if (pc > 3 && (c[31] > 3  || (c[38] && curmon[0][39] == pc)) && crum != 31) {
    if (c[31] > 3 && c[52] == 0) {
      
      c[52] = 1;
      new_c->seekshard_imprinted_room = 1;

      prnt("-> The %s shrieks in pain and fear!", w, NULL, NULL, NULL, NULL, NULL);
    }

    // if (curmon[0][39] == pc || (curmon[0][15] == 2 && curmon[0][7] == pc) ) {
    if (player->unknown39 == pc || (player->command == ACTION_CAST && player->target == pc) ) {
      c[38] = 0;
      new_c->controlled = 0;

      dorep();
    }

    if (dungeon == DUNGEON_KILKANEY && c[31] > 3 && crum != 31 && !c[38] ) {
      police = 1;
    }

    if (dungeon == DUNGEON_KILKANEY && c[31] > 3 && specbuf.wallfire_turns != 0) {
      police = 1;
    }
  }

  c[35] = 2;
  new_c->stink_lines = 2;

  Vsync();
  vbl_animation_off();

  if (pc < 4 || light || zline[x][y].unknown5 > 0) {
    // Magic "splat" mark
    blt(&bitmap[128], 16 * x, 16 * y);
  }

  if (pc > 3 && mode) {
    for (i = 0; i < 8; i++) {
      Vsync(); 
      toggle(32, c[33], 7);
    }
  }

  Vsync();
  vbl_animation_on();

  // if (c[1] >= c[2] || level > c[2]) {
  if (new_c->damage >= new_c->max_health || level > new_c->max_health) {
    c[1] = c[2];
    new_c->damage = new_c->max_health;
    c[0] = 0;
    new_c->in_use = 0;

    zline[x][y].monster = 0;

    check_curmon("damage 0.1");

    if (pc > 0) {
      if (pc > 3 && c[58] != 16) {
         prnt("-> The %s has been slain!", w, NULL, NULL, NULL, NULL, NULL);
      }

      if (newpolice && police) {
        prnt("-> And an evil deed it was!  %s will pay for this heinous crime!",
          pname, NULL, NULL, NULL, NULL, NULL);
      }

      if (pc > 3 && c[58] == 16) {
        prnt("->  MELKTHROP IS SLAIN!!!", NULL, NULL, NULL, NULL, NULL, NULL);
      }

      // Players:
      if (pc < 4) {
        j = rumslot();
        z = &crumobj[j];

        prnt("-> %s IS DEAD!", w, NULL, NULL, NULL, NULL, NULL);

        if (c[41] > 0) {
          litetrol(x, y, 0);
        }

        i = zline[x][y].visible;

        if (i != 0 && crumobj[i].target != 0) {
          storsc(storbuf[pc], 16 * x, 16 * y, 1, addr);
          do {
            i = 1 - rnd(3);
            k = 1 - rnd(3);
            
            x = c[24] + i;
            y = c[25] + k;

            z1 = &zline[x][y];
            l = z1->visible;
          } while ( x < 0 || x > 15 || y < 0 || y > 7 ||  
            (crumobj[l].object_id < OBJECT_BROADSWORD && crumobj[l].target != 0) || z1->unknown0 != 1);

          c[24] = x;
          c[25] = y;
          new_c->x = x;
          new_c->y = y;

          storsc(storbuf[pc], 16 * x, 16 * y, 0, addr);

          if (l!= 0 && crumobj[l].object_id > 40) {
            putaway(pc, crumobj[l].object_id);
          }
        } 

        zline[x][y].visible = j;

        if (c[45] != 0) {
          putaway(pc, c[45]);
        }

        if (c[46] != 0 && c[41] == 0) {
          putaway(pc, c[46]);
        }
         
        // j came from rumslot() BTW, it could be (1..18)
        // with 134 added to it that becomes (135 .. 152)
        // and with the > 13 limitation that then becomes (148..152) which is room_unknown_3
        if (j > 13) {
          // Old
          //rumdata[crum][134 + j] = 41 - pc;
          // New
          new_rumdata[crum].room_unknown_3[j - 14] = OBJECT_BROADSWORD - pc;

          // This is a really weird number. pc must be 0-3 here, which means:
          // It may be the case that Eirik is the only possible playable character that 
          // matters here, since if the player (0) dies the game ends. PC values 2 and 3 
          // may never come up.
          // 0 - broadsword
          // 1 - Eirik's dead body
          // 2 - nothing (an object in the array without a name)
          // 3 - nothing again
          storobj(OBJECT_BROADSWORD - pc, x, y);
          //check_rumdata("damage");
        }

        z->object_id = OBJECT_BROADSWORD - pc;
        z->nmsg = z->object_id;
        z->movable = 1;
        z->visible = 1;
        z->x = c[24];
        z->y = c[25];

        if (invnpc[pc][0] > 0) {
          for (i = 1; i <= 13; i++) {
            if (invnpc[pc][i] != 0) {
              putinto(j, invnpc[pc][i]);
              invnpc[pc][i] = 0;
            }
          }

          invnpc[pc][0] = 0;
          z->hide = 1;
        } else {
          z->hide = 0;
        }

        z->trap = 0;
        z->target = 0;
      }
      
      if (light || zline[x][y].unknown5 > 0) {
        drawsq(x, y);
      }

      check_curmon("damage 0.9");

      // Clear out curmon[i]
      for (i = 0; i < 60; i++) {
        c[i] = 0;
      }
      memset(new_c, 0, sizeof(curmon_t));
         
      check_curmon("damage 1");
      return 1;
    } else {
      undorep();
      prnt("   -- ! %s IS PASSING OUT ! --", w, NULL, NULL, NULL, NULL, NULL);
      stormon();
      combat = 0;
      check_curmon("damage 1.1");
      return -1;
    }
  }

  if (pc < 4 && (old * 4) / c[2] < (i = (c[1] * 4) / c[2])) {
    prnt("-> %s's damage is now %s!", w, wordmod[i], NULL, NULL, NULL, NULL);
  }
  
  check_curmon("damage 2");
  return 1;
}


#include "globals.h"
#include "startaux.h"
#include "blt.h"
#include "trapaux.h"
#include "start.h"
#include "caux.h"
#include "tos_compat.h"

#include "f5.h"

// Search

int search_in(int pc)
{
  uint8_t *c = curmon[pc];
  player_t *new_c = &players[pc];

  if (pc >= 4) {
    debug_printf(">>> ERROR: non-character calling search_in() pc=%d\n", pc);
    appl_exit();
  }

  if (outside) {
    return 0;
  }

  c[16] = 6;
  c[15] = 6;
  c[7] = c[8] = c[9] = 0;      

  new_c->command = 6;
  new_c->unknown16[0] = 6;
  new_c->unknown7 = 0;
  new_c->unknown8 = 0;
  new_c->unknown9 = 0;

  return 1;
}

int search_out(int pc)
{
  uint8_t *c = curmon[pc], *z;
  // uint8_t *o;
  room_object_t *object;
  player_t *new_c = &players[pc];
  char *w = pc == 0 ? &pname[0] : name[c[3]];
  uint8_t *r = putbuf;
  //int x = c[24], y = c[25];
  int x = new_c->x, y = new_c->y;
  int i,j,k,l,m = 0,num = 0,flag;

  c[7] = c[8] = c[9] = 0;
  new_c->unknown7 = 0;
  new_c->unknown8 = 0;
  new_c->unknown9 = 0;

  if (pc == 0 && c[18] != 6) {
    prnt("-> %s searches nearby...", w, NULL, NULL, NULL, NULL, NULL);
  }

  for (i = -1; i < 2; i++) {
    for (j = -1; j < 2; j++) {
      z = zline[x+i][y+j];

      // Don't search squares outside the map, or under the player
      // if (x + i < 0 || x + i > 15 || y + j < 0 || y + j > 7 || z[0] == 0 || (x + i == c[24]
      //     && y + j == c[25])) {
      if (x + i < 0 || x + i > 15 || y + j < 0 || y + j > 7 || z[0] == 0 || (i == 0 && j == 0)) {            
        continue;
      }

      // if (z[1] != 0 && crumobj[z[1]][5] > 0) {
      if (z[1] != 0 && crumobj[z[1]].hide > 0) {
        // o = crumobj[z[1]];
        object = &crumobj[z[1]];
        flag = 0;

        while (m < 320) {
          if (r[m + 3] != crum || r[m + 2] != z[1]) {
            m += 4;
            continue;
          }

          flag++;          
          k = r[m + 1];
          
          // Who is 3?
          // if (pc != 3 || c[38] == 1) {
          if (pc != 3 || new_c->controlled == 1) {
            // prnt("-> %s discovers a %s in the %s!", w, obj[k], obj[o[0]], NULL, NULL, NULL);
            prnt("-> %s discovers a %s in the %s!", w, obj[k], obj[object->object_id], NULL, NULL, NULL);
          }
        
          c[18] = 0;
          new_c->unknown16[2] = 0;
          
          num++;

          // if (pc == 3 || ((pc != 3 || c[38] == 1) && error(ERROR_TAKE_OBJECT) == 1)) {
          if (pc == 3 || ((pc != 3 || new_c->controlled == 1) && error(ERROR_TAKE_OBJECT) == 1)) {
            flag--;
            if (!handman(pc, 0, k, 0)) {
              return 1;
            }

            // l = c[45] == 0 ? 0 : 1;
            l = new_c->good_hand == 0 ? 0 : 1;
            c[45 + l] = k;

            if (l == 0) {
              new_c->good_hand = k;
            } else {
              new_c->other_hand = k;
            }

            invtrap(k);
            remove_object(z[1], k);
            
            c[15] = 7;
            c[8] = k;
            c[10] = 0;
            new_c->command = 7;
            new_c->unknown8 = k;
            new_c->spell_cast = 0;
            
            prnt("-> %s breaks off the search to take the %s.", w, obj[k], NULL, NULL, NULL, NULL);

            return 1;
          }

          m += 4;
        }

        if (flag == 0) {
          // crumobj[z[1]][5] = 0;
          crumobj[z[1]].hide = 0;
        }
      }

      // if (z[1] != 0 && crumobj[z[1]][5] == -2 && c[18] != 6) {
      if (z[1] != 0 && crumobj[z[1]].hide == -2 && c[18] != 6) {
        // prnt("-> %s notices the %s is locked!", w, obj[crumobj[z[1]][0]], NULL, NULL, NULL, NULL);
        prnt("-> %s notices the %s is locked!", w, obj[crumobj[z[1]].object_id], NULL, NULL, NULL, NULL);
      }

      for (k = 1; k < 17; k++) {
        // o = crumobj[k];
        object = &crumobj[k];
        // if (o[0] != 0 && c[37] > rnd(100) && o[8] == 0 && o[6] == x + i && o[7] == y + j) {
        if (object->object_id != 0 && new_c->perception > rnd(100) && object->visible == 0 
            && object->x == x + i && object->y == y + j) {
          // o[8] = 1;
          object->visible = 1;
          // prnt("-> %s discovers a hidden %s!", w, obj[*o], NULL, NULL, NULL, NULL);
          prnt("-> %s discovers a hidden %s!", w, obj[object->object_id], NULL, NULL, NULL, NULL);
          
          c[18] = 0;
          new_c->unknown16[2] = 0;

          if (new_rumdata[crum].room_has_bg || zline[x + i][y + j][5] > 0) {
            // blt(&bitmap[o[0]],16 * (x + i),16 * (y + j));
            blt(&bitmap[object->object_id], 16 * (x + i), 16 * (y + j));
          }

          z[1] = k;
          
          // if (o[3] != 0) {
          if (object->target != 0) {
            z[0] = 2;
          }
          num++;
        }
      }
    }
  }    

  if (num == 0 && c[18] != 6 && (pc != 3 || c[38] == 1)) {
    prnt("-> %s discovers nothing.", w, NULL, NULL, NULL, NULL, NULL);
    c[18] = 6;
    new_c->unknown16[2] = 6;
  }

  return 1;
}


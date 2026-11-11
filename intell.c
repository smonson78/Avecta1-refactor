
#include "globals.h"
#include "caux.h"
#include "dist.h"
#include "dorep.h"
#include "damage.h"
#include "trapaux.h"
#include "sell.h"
#include "start.h"
#include "debug.h"
#include "tos_compat.h"

#include "f4.h"
#include "f6.h"

void sillymsg(int npc);
void alert(int npc);
int matt(int npc);
int friend(int npc);
int setatt(int npc);
int target(int npc);
int settarg(int npc);
int setmove(int npc, int flag);
int pass(int npc);
int setflee(int npc);
int morale(int npc);

int intell(int npc)
{
  uint8_t *c = curmon[npc];
  curmon_t *new_c = &new_curmon[npc];
  player_t *p = &players[npc];

  uint8_t *t;
  int done = 0; /* this will give the ith step in the intelligence routine */
  char type = c[31], step;
  int j, flag;

  debug_printf("> intell()\n");
  check_curmon("intell start");

  // if (!c[0]) {
  if (!new_c->in_use) {
    return 0;
  }

  if (npc < 4 || (npc > 3 && new_c->controlled == 1)) {
    type = INTELLIGENCE_FRIEND;
  }

  step = type;
  c[40] = 0;
  new_c->unknown40 = 0;

  while (!done) {
    debug_printf("intell step %d\n", step);

    switch (step) {
      case 1: /* sets up the standard berserk attack sequence. */
        done = 1; /* always fall through after a berserk */
        if (!morale(npc)) {
          if (!setflee(npc) || !setmove(npc,0)) {
            pass(npc);
          }
          return 1;
        }

        if (!target(npc) && !settarg(npc)) {
          pass(npc);
          return 1;
        }

        switch (setatt(npc)) {
          case 0:                          /* target is in range */
            matt(npc);
            break;
          case 1:                          /* target at wrong x,y */
            if (!setmove(npc, 1)) {        /* option 1 checks target motion */ 
              pass(npc);                   /* and inveigles the rear when */
            }
            break;                         /* possible */
          }
        break;

      case 2:                           /* this sets up  the patrol actions */
          if (target(npc) || settarg(npc)) {
            step = 1;
            if (type != 3) {
              alert(npc);                 /* this changes patrol crew to berserk */
            }
            break;
          }

          if (type != 3) {               /* don't want pickets to leave the room */
            if (!setmove(npc,2)) {
              pass(npc);               /* else exit room */
            }
          } else{
            pass(npc);
          }
          done = 1;
          break;

      case 3:                  /* this is a picket, i.e., stationary creatures */
        step = 2;
        break;

      case INTELLIGENCE_FRIEND:
        /* this is the accompanying friendly NPC's */
        done = 1;
        // if (npc < 4 && c[38] == 1) { /* under control of player */
        if (npc < 4 && p->controlled == 1) { /* under control of player */          
          break;
        }

        friend(npc);
        break;

      case 5:  /* intell for merchants */
        done = 1;
        sell(npc);
        break;

      case 6:  /* bystanders headed for nearest or some exit */
        wander(npc);
        done = 1;
        break;

      default:
        flag = 0;
        for (j = 0;j < 12; j++) {
          if (j == npc) {
              continue;
          }
          t = curmon[j];
          //curmon_t *new_t = &new_curmon[j];

          if (t[0] == 0 || t[30] != crum) {
            continue;
          }

          if ((t[39] == npc && t[15] == 1) || c[47] > 0 || c[1] > 0) { 
            flag = 1;
            break;
          }
        }

        if (flag) {
          // Monsters only
          for (j = 4; j < 12; j++) {
            c = curmon[j];
            new_c = &new_curmon[j];
            if (c[31] > 6 && c[30] == crum && c[0] == 1) {
              c[31] = 1;
              new_c->intelligence_routine = 1;
            }
          }

          // Now we gotta change it back because we used it for something else :|
          c = curmon[npc];
          new_c = &new_curmon[npc];

          if (crum == 31) {
            prnt("-> The forlorn creature sobs `All is lost. I die now.'", NULL, NULL, NULL, NULL, NULL, NULL);
            damage(npc, c[2] + c[12]);
            
            c[0] = 0;
            new_c->in_use = 0;

            untrap(3);
            done = 1;
            break;
          }

          step = 1;
          c[31] = 1;
          new_c->intelligence_routine = 1;

          prnt("-> A general clamor can be heard `Get the guards! There's a lunatic about!'", 
            NULL, NULL, NULL, NULL, NULL, NULL);
          police = 1;
          dorep();
          break;
        }

        pass(npc);
        done = 1;
    }
  }

  check_curmon("intell end");
  debug_printf("< intell()\n");

  return 1;
}

/*****************************************************************************/
/* MORALE(NPC) checks the NPC for a failure of nerve.                        */
/*****************************************************************************/

// 0 = panic
// 1 = no panic
int morale(int npc)
{
  uint8_t *c = curmon[npc];
  curmon2_t *new_c = &new_curmon2[npc];
  // uint8_t *d;
  int level,num,i,dam,top,bot;
  
  if (c[36] == 1 || c[0] == 0) {
    c[38] = 0;
    new_c->monster.controlled = 0;
    return 0;
  }

  // Only monsters
  if (npc > 3 && c[44] > 0) {
    // Check against the 4 player characters
    for (i = 0; i < 4; i++) {
      // d = curmon[i];
      player_t *new_d = &players[i];

      // if (!d[0] || d[30] != crum) {
      if (!new_d->in_use || new_d->current_room != crum) {
        // They're not here, skip
        continue;
      }

      // if (d[45] == c[44] || d[46] == c[44]) {
      if (new_d->good_hand == new_c->monster.bane || new_d->other_hand == new_c->monster.bane) {
        prnt("-> The %s is gripped by terror at the sight of the %s!",
              name[new_c->monster.monster_name_id], 
              obj[new_c->monster.bane], NULL, NULL, NULL, NULL);
        damage(npc, c[12]);
        return 0;
      }
    }
  }

  // if (c[13] > 100 || c[1] < (c[2])/2 ) { /* not intelligent creature */
  if (new_c->monster.unknown13 > 100 || new_c->monster.damage < (new_c->monster.max_health) / 2) { 
    /* not intelligent creature */
    return 1;
  }

  num = 0;
  // How many friends are there around?
  top = (npc > 3 ? 12 : 4);
  bot = (npc > 3 ? 4 : 0);
  
  for (i = bot; i < top; i++) {
    if (i == npc) {
      continue;
    }
    
    // if (curmon[i][0] == 1 && curmon[i][30] == crum) {
    if (new_curmon2[i].monster.in_use == 1 && new_curmon2[i].monster.current_room == crum) {
      num++;
    }
  }
  
  // 3 or more friends? No panic.
  if (num > 2) {
    return 1;
  }

  // level = 5 * (c[2] / 5) + c[13];
  level = 5 * (new_c->monster.max_health / 5) + new_c->monster.unknown13;
  // dam = c[2] - c[1];
  // Done in two steps in order to force signed maths
  dam = new_c->monster.max_health;
  dam -= new_c->monster.damage;
  
  
  if (dam <= 0) {
    c[0] = 0; /* you're dead but too stupid to know it */
    new_c->monster.in_use = 0;
    return 0;
  }
  
  // level -= 5 * ((c[2]) / dam);
  level -= 5 * (new_c->monster.max_health / dam);

  if (rnd(100) > level) {
    c[36] = 1;
    c[38] = 0;
    new_c->monster.flee = 1;
    new_c->monster.controlled = 0;
    damage(npc, 0);
    return 0;
  }

  return 1;
}

/******************************************************************************/
/* SETFLEE(NPC) is a call to set up the charac to take a bunk.  It hammers in */
/* a flee flag and then picks an exit, as well as initializing the time for   */
/* heading south.                                                             */
/******************************************************************************/

int setflee(int npc)
{
  int i,x,y;
  room_object_t *d;
  uint8_t *c = curmon[npc];
  curmon_t *new_c = &new_curmon[npc];

  c[36] = 1;
  new_c->flee = 1;

  for (i = 1; i < 14; i++) {
    d = &crumobj[i];
    if (d->target != 0 && d->visible == 1 && d->hide != -2) {

      if (adjac(npc, d->x, d->y) ) { /* then we're at exit */
        c[15] = 9;                 /* so we set up a X-it act */
        c[16] = 6;
        c[26] = d->x; // Presumably this is where they're headed
        c[27] = d->y;

        new_c->command = ACTION_EXIT;
        new_c->unknown16[0] = 6;
        new_c->dest_x = d->x;
        new_c->dest_y = d->y;

        return 1;
      }

      c[26] = d->x;
      c[27] = d->y;
      new_c->dest_x = d->x; // FIXME this happens regardless of the conditional
      new_c->dest_y = d->y;

      c[22] = c[24];
      c[23] = c[25];

      new_c->unknown16[6] = new_c->x;
      new_c->unknown16[7] = new_c->y;


      nextxy(c[22], c[23], c[26], c[27], c[24], c[25], &x, &y);

      c[28] = x;
      c[29] = y;
      new_c->unknown28[0] = x;
      new_c->unknown28[1] = y;
      c[15] = 4;
      c[18] = i;
      c[16] = (c[11] < 1 ? 1 : c[11]);
      new_c->command = ACTION_MOVE;
      new_c->unknown16[2] = i;
      new_c->unknown16[0] = (new_c->unknown11 < 1 ? 1 : new_c->unknown11);

      return 1;
    }
  }

  return 0;
}

/******************************************************************************/
/* SETMOVE(NPC,FLAG)  is a call to set a move option for the NPC to location  */
/* chosen by this function.  If the move is impossible a 0 is returned.       */
/******************************************************************************/

int setmove(int npc, int flag)
{
  uint8_t *c = curmon[npc];
  curmon_t *new_c = &new_curmon[npc];
  curmon_t *new_t;

  uint8_t *c1, *t;
  zline_t *z;
  int top, bot, i, j, l, m, n, k = 0, xdes, ydes, x, y, buddy = 0;
  int tar = c[39];

  t = curmon[tar];
  new_t = &new_curmon[tar];

  if (npc < 4 || (npc > 3 && c[38] ) ) {
    top = 4;
    bot = 1;
  } else {
    top = 12;
    bot = 4;
  }

  if (c[36] == 0) {
    if (los(npc, t[24], t[25])) {
      c[26] = t[24];
      c[27] = t[25];
      new_c->dest_x = new_t->x;
      new_c->dest_y = new_t->y;
    } else {
      if (c[26] == c[24] && c[27] == c[25]) {
        for (i = bot; i < top; i++) {
          if (i == npc) {
            continue;
          }

          c1 = curmon[i];
          curmon_t *new_c1 = &new_curmon[i];

          if (los(npc, c1[24], c1[25]) && los(i, t[24], t[25]) ) {
            c[26] = *(c1+24);
            c[27] = *(c1+25);
            new_c->dest_x = new_c1->x;
            new_c->dest_y = new_c1->y;
            buddy = 1;
            break;
          }
        }

        if (!buddy) {
          x = (t[24] - c[24] > 0 ? 1 : -1);
          y = (t[25] - c[25] > 0 ? 1 : -1);
          c[26] = c[24] + x;
          c[27] = c[25] + y;
          new_c->dest_x = new_c->x + x;
          new_c->dest_y = new_c->y + y;          

          if (zline[c[26]][c[27]].unknown0 != 1) {
            // A little random movement!
            int rand_x = rnd(3);
            int rand_y = rnd(3);

            c[26] = c[24] + 1 - rand_x;
            c[27] = c[24] + 1 - rand_y;

            new_c->dest_x = new_c->x + 1 - rand_x;
            new_c->dest_y = new_c->y + 1 - rand_y;          
          }
        }
      }
    }   
  }

  if (c[36] == 1 && adjac(npc, c[26], c[27])) {
    // Characters
    if (npc < 4) {
      c[36] = 0;
      new_c->flee = 0;
      pass(npc);

      return 1;
    }

    // Monsters
    c[15] = 9;
    c[16] = 6;
    c[9] = crumobj[c[18]].target;

    new_c->command = ACTION_EXIT;
    new_c->unknown16[0] = 6;
    // This is a bit of a concern because it doesn't seem to make sense
    new_c->spells[4] = crumobj[new_c->unknown16[2]].target;

    return 1;
  }

  if (!flag) {
    if (c[24] == c[26] && c[25] == c[27]) {
      return 0;
    }

    nextxy(c[22], c[23], c[26], c[27], c[24], c[25], &xdes, &ydes);
    c[28] = xdes;
    c[29] = ydes;
    new_c->unknown28[0] = xdes;
    new_c->unknown28[1] = ydes;
  }

  do {
    switch (flag) {  
      case 0:              /* this case is a simple move */
        i = 1;     /* must check to see if square is open */
        j = 1;
        xdes = c[28];
        ydes = c[29];      
        l = xdes - c[24];
        m = ydes - c[25];
        do {
          x = c[24] + (i * l);
          y = c[25] + (j * m);
          z = &zline[x][y];
          if (z->unknown0 == 1 && z->unknown6 == 0 && z->monster == 0 && 
            (x != c[24] || y != c[25]) ) {
            if (z->visible != 0 && (npc < 4 || !c[37]) && 
              (n = crumobj[z->visible].object_id) < 41) {
              
              if (c[51] > weight[n] && weight[n] != -1) {
                if (!move(z->visible, 1, x, y)) {
                  goto next;
                }
              } else {
                goto next;
              }
            }

            c[28] = c[24]+(i*l);
            c[29] = c[25]+(j*m);
            c[15] = 4;
            c[16] = c[11];

            new_c->unknown28[0] = new_c->x + (i * l);
            new_c->unknown28[1] = new_c->y + (j * m);
            new_c->command = ACTION_MOVE;
            new_c->unknown16[0] = new_c->unknown11;

            if (i * l != 0 && j * m != 0) {
              c[16] += c[16] / 2;
              new_c->unknown16[0] += new_c->unknown16[0] / 2;
            }

            return 1;
          }

        next:
          if (i == 1 && j == 0) {
            i = 0; 
            if (m == 0) {
              m = rnd(2) ? 1 : -1;
            }
          }

          if (i == 0 && j == 1) {
            i = 1;
            if (l == 0) {
              l = rnd(2) ? 1 : -1;
            }
            j = 0;
          }

          if (i == 1 && j == 1) {
            i = 0;
            if (m == 0) {
              m = rnd(2) ? 1 : -1;
            }
          }             
        } while (i != 0 || j != 0);

        return 0;

    case 1: /* chasing a charac */
      flag = 0;
      if (!los(npc,curmon[tar][24], curmon[tar][25])) {
        nextxy(c[24], c[25], c[26], c[27], c[24], c[25], &xdes, &ydes);
        c[28] = xdes;
        c[29] = ydes;
        new_c->unknown28[0] = xdes;
        new_c->unknown28[1] = ydes;
        break;
      }

      for (j = bot; j < top; j++) {
        c = curmon[j];
        new_c = &new_curmon[j];

        if (c[0] == 0 || c[18] == 1 || c[15] != 1 || j == npc || c[30] !=
          crum || c[39] != tar) {
            continue;
        }
        k++;                  /* k counts number of other active monsters */
      }

      // If we didn't re-use c then we wouldn't need to do this all the time
      c = curmon[npc];
      new_c = &new_curmon[npc];

      if (t[15] == 4) {       /* target in motion                        */
        if (k > 0) {          /* somebody's already stompin' his butt    */
          nextxy(c[24], c[25], t[24], t[25], c[24], c[25], &xdes, &ydes);
        } else { /* he's not being attacked */
          nextxy(c[24], c[25], t[28], t[29], c[24], c[25], &xdes, &ydes);
        } 

        c[28] = xdes;
        c[29] = ydes;
        new_c->unknown28[0] = xdes;
        new_c->unknown28[1] = ydes;

        break;
      }

      /* charac not moving but under attack so nail rear */
      if (k > 1) {
        nextxy(t[24], t[25], c[24], c[25], t[24], t[25], &xdes, &ydes);
        c[26] = xdes;
        c[27] = ydes;
        new_c->dest_x = xdes;
        new_c->dest_y = ydes;

        nextxy(c[24], c[25], c[26], c[27], c[24], c[25], &xdes, &ydes);

        c[28] = xdes;
        c[29] = ydes;
        new_c->unknown28[0] = xdes;
        new_c->unknown28[1] = ydes;

        if (zline[xdes][ydes].unknown0 != 0) {
          break;
        }
      }

      nextxy(t[24], t[25], c[24], c[25], t[24], t[25], &xdes, &ydes);
      c[26] = xdes;
      c[27] = ydes;
      new_c->dest_x = xdes;
      new_c->dest_y = ydes;

      nextxy(c[24], c[25], c[26], c[27], c[24], c[25], &xdes, &ydes);
      c[28] = xdes;
      c[29] = ydes;
      new_c->unknown28[0] = xdes;
      new_c->unknown28[1] = ydes;      

      break;

    case 2:
      //  c[26] = rumdata[crum][155];
      //  c[27] = rumdata[crum][156];
      c[26] = new_rumdata[crum].room_unknown_3[7];
      c[27] = new_rumdata[crum].room_unknown_3[8];
      new_c->dest_x = new_rumdata[crum].room_unknown_3[7];
      new_c->dest_y = new_rumdata[crum].room_unknown_3[8];

      flag = 0;

      break;
    }
  } while (flag == 0);

  return 0;
}

int pass(int npc)
{
  uint8_t *c = curmon[npc];
  curmon_t *new_c = &new_curmon[npc];

  c[15] = 8;
  c[16] = 4;
  new_c->command = ACTION_WAIT;
  new_c->unknown16[0] = 4;

  return 1;
}

int target(int npc)
{
  int i, j = curmon[npc][39];
  uint8_t *c;
  //curmon_t *new_c;

  if (((npc > 3 && curmon[npc][38]) || npc < 4) && j == 0) {
    return 0;
  }

  // All characters
  for (i = 0; i < 12; i++) {
    if (i == npc) {
      continue;
    }

    c = curmon[i];
    //new_c = &new_curmon[i];

    if (c[39] == npc && c[0] && c[30] == crum) {
      curmon[npc][39] = i;
      new_curmon[npc].unknown39 = i;
      return 1;
    }
  }

  if (j == npc || curmon[j][0] != 1 || curmon[j][30] != crum) {
    return 0;
  }

  return 1;
}

int settarg(int npc)
{
  uint8_t *c = curmon[npc];
  curmon_t *new_c = &new_curmon[npc];
  uint8_t *t;
  int j,list[12];
  int top,bot;
  list[0] = 0;

  if (npc < 4 || (npc > 3 && c[38])) {
    top = 12;
    bot = 4;
  } else {
    top = 4;
    bot = 0;
  }

  // Players only
  if (npc < 4 && player->unknown39 == npc) {
    c[39] = 0;
    new_c->unknown39 = 0;
    return 1;
  }

  // Monsters only
  if (npc > 3 && c[58] != 17 && c[58] != 24) {
    for (j = 4; j < 12; j++) {
      t = curmon[j];
      if (t[58] == 17 && c[38] == 0) {
        continue;
      }

      if (t[4] != c[4] && *t == 1 && t[30] == crum && (c[38] == 1 || 
        ((c[58] != 20 || t[58] != 4) && t[58] != 24))
        && los(npc, t[24], t[25]) ) {
        list[0]++;
        list[list[0]] = j;
      }
    }
  }

  if (c[38] == 0 || list[0] == 0) {
    for (j = bot; j < top; j++) {
      if (j == npc) {
        continue;
      }

      t = curmon[j];

      if (t[0] != 0 && t[30] == crum && (npc < 4 || los(npc,t[24],t[25])) &&
          (npc > 3 || t[38] == 0) && t[31] < 4) {
        list[0]++;
        list[list[0]] = j;  
      }
    }
  }

  if (!list[0]) {
    return 0;
  }

  j = rnd(list[0] - 1);
  
  c[39] = list[1+j]; 
  c[26] = curmon[c[39]][24];
  c[27] = curmon[c[39]][25];

  new_c->unknown39 = list[1 + j]; 
  new_c->dest_x = new_curmon[new_c->unknown39].x;
  new_c->dest_y = new_curmon[new_c->unknown39].y;

  return 1;
}

/******************************************************************************/
/* ATT(NPC) sets up the attack mode of the monster.  There are only two       */
/* possibilities: assail or magic.                                            */
/******************************************************************************/

int matt(int npc)
{
  uint8_t *a = curmon[npc];
  curmon_t *new_a = &new_curmon[npc];
  int i, j, sp;

  a[26] = curmon[a[39]][24];
  a[27] = curmon[a[39]][25];
  new_a->dest_x = new_curmon[new_a->unknown39].x;
  new_a->dest_y = new_curmon[new_a->unknown39].y;

  if (a[14] != 0) {               /* indicates a magic using monster */
    if (a[47] > 3 && a[14] > 0) {
      a[47] = 0;
      a[14] = (a[14] == 0 ? 0 : a[14] - 1);

      new_a->poisoned = 0;
      new_a->spell_units = new_a->spell_units == 0 ? 0 : new_a->spell_units - 1;
    }

    if (new_a->spell_units < 3) {
      a[36] = 1;                /* set flee since out of spell units */
      new_a->flee = 1;

      pass(npc);
      return 0;
    }

    j = 0;

    // Search from curmon [5] to [9] looking for a zero
    for (i = 5; i < 10; i++) {
      if (a[i] == 0) {
        break;
      }
      j++;
    }

    // j is now 0..5

    // Then get a random number up to that point (0..4) so 5 =< i =< 9
    i = 5 + rnd(j);
    // [5..9] must therefore be possible spells that it knows
    sp = a[i];

    a[14] -= spelunit[sp];
    a[16] = spelinfo[sp-1][2];
    a[40] = 4;
    a[15] = 2;
    a[14] = (a[14] < 5 ? -1 : a[14] );
    a[10] = sp;

    new_a->spell_units -= spelunit[sp];
    new_a->unknown16[0] = spelinfo[sp - 1][2];
    new_a->unknown40 = 4;
    new_a->command = ACTION_CAST;
    new_a->spell_units = new_a->spell_units < 5 ? -1 : new_a->spell_units;
    new_a->spell_cast = sp;

    return 1;
  }

  a[15] = 1;
  new_a->command = ACTION_ATTACK;

  if (a[45] == 45) {
    a[15] = 11;
    new_a->command = ACTION_FIRE_ARROW;
  }

  a[16] = a[11];
  new_a->unknown16[0] = new_a->unknown11;

  if (a[15] == 11) {
    a[16] *= 2;
    new_a->unknown16[0] *= 2;
  }

  // Monsters only
  if (npc > 3) {
    i = rnd(100);
    j = 0;
    
    while (j < 2 && a[6+2*j] < i) {
      j++;
    }
  } else {
    j = rnd(3);
  }

  a[40] = j+1;
  new_a->unknown40 = j + 1;

  return 1;
}

void alert(int npc)
{
  for (int i = 1; i < 7; i++) {
    uint8_t *c = curmon[i];
    curmon_t *new_c = &new_curmon[i];

    //if (c[0] == 1 && c[31] == 2) {
    if (new_c->in_use == 1 && new_c->intelligence_routine == 2) {
      c[31] = 1;
      new_c->intelligence_routine = 1;
    }
  }
}

int setatt(int npc)
{
  uint8_t *a = curmon[npc];
  uint8_t *t = curmon[a[39]];
  int d = dist(a[24], a[25], t[24], t[25]);

  if (!los(npc,t[24],t[25])) {
    return 1;
  }

  if (d == 1 || a[14] > 0 || (npc < 4 && a[45] == 45 && a[39] != 0)) {
    return 0;
  }

  return 1;
}

int friend(int npc)
{
  uint8_t *c = curmon[npc];
  curmon_t *new_c = &new_curmon[npc];
  player_t *p = &players[npc];
  int i, j, x = player->x, y = player->y;

  // Characters only:
  // if (npc < 4 && new_rumdata[crum].room_has_bg == 0 && c[41] < 3 && c[42] > 0) {
  if (npc < 4 && new_rumdata[crum].room_has_bg == 0 && p->lit_torches < 3 && p->unlit_torches > 0) {
    // If room is dark and character has less than three turns of torchlight left, 
    // and still has unlit torches: light a torch

    c[10] = 1;
    c[15] = 12;
    c[16] = 6;

    new_c->spell_cast = 1;
    new_c->command = ACTION_BURN_SNUFF;
    new_c->unknown16[0] = 6;
    return 1;
  }

  if (mode) {
    // Characters only:
    if (npc < 4 && ( c[45] > 45 || c[45] < 41) ) {
      if (c[45] > 0) {
        putaway(npc, c[45]);
      }

      if (c[32] > 40 && c[32] < 46) {
        takeout(npc,c[32]);
        c[45] = c[32];
        new_c->poison_mode_2 = new_c->poison_mode_1;
      } else {
        for (i = 0; i < invnpc[npc][0]; i++) {
          if ((j = invnpc[npc][i+1]) > 40 && j < 46) {
            takeout(npc, j);
            c[45] = j;
            c[32] = j;
            new_c->poison_mode_2 = j;
            new_c->poison_mode_1 = j;
            break;
          }
        }
      }
    }

    // Characters only:
    if (npc < 4 && (c[45] > 45 || c[45] < 41)) {
      if (c[53] == 0) {
        prnt("-> %s yells, 'I don't have a weapon! Give me one quick!'",
            name[c[3]], NULL, NULL, NULL, NULL, NULL);
      }
      pass(npc);
      j = c[53];

      c[53] = (j == 5 ? 0 : ++j);
      new_c->seekshard_imprinted_xy = (j == 5 ? 0 : ++j);

      return 1;
    }

    if (!morale(npc)) {
      if (!adjac(npc,x,y)) {
        c[26] = x;
        c[27] = y;
        new_c->dest_x = x;
        new_c->dest_y = y;

        if (!setmove(npc,0)) {
          pass(npc);
        }

        return 1;
      } else {
        c[36] = 0;
        new_c->flee = 0;
      }
    }

    if (!target(npc) && !settarg(npc)) {
      c[39] = 0;
      new_c->unknown39 = 0;

      if (!setmove(npc, 1)) {
        pass(npc);
      }

      return 1;
    }

    switch (setatt(npc)) {
      case 0:
        matt(npc);
        break;
      case 1:
        if (!setmove(npc,1)) {
          pass(npc);
        }
        break;
      default:
        break;
    }              
  }

  if (!mode) {
    c[36] = 0;
    new_c->flee = 0;

    if (curmon[0][15] == 4 || npc > 3 || !los(npc,x,y) || 
      dist(c[24], c[25], x, y) > 3) {
      c[39] = 0;
      new_c->unknown39 = 0;

      if (dist(c[24], c[25], x, y) < 4 - rnd(2) && los(npc, x, y) ) {
        pass(npc);
        return 1;
      } else {
        if (!los(npc, x, y) && c[24] == c[26] && c[25] == c[27]) {
          c[26] = x;
          c[27] = y;
          new_c->dest_x = x;
          new_c->dest_y = y;
        }

        if (!setmove(npc, 1)) {
          pass(npc);
        }

        return 1;
      }
    } else {
      if (rnd(100) < 4) {
        sillymsg(npc);
        pass(npc);
        return 1;
      }

      if (c[15] != 6) {
        search_in(npc);
        return 1;
      } else {
        do {
          // Teleport around randomly?
          int xrnd = rnd(16);
          int yrnd = rnd(8);

          c[26] = xrnd;
          c[27] = yrnd;
          new_c->dest_x = xrnd;
          new_c->dest_y = yrnd;

        } while (!move_in(npc));
      }
    }
  }

  return 1;
}

void sillymsg(int npc)
{
  uint8_t *c = curmon[npc];
  char *w = name[c[3]];

  if (c[47] > 0) {
    prnt("-> %s says `The room spins.  I am poisoned.'",w, NULL, NULL, NULL, NULL, NULL);
    return;
  }

  if (c[1] > (c[2])/2) {
    prnt("-> %s says `By Zandru, my body aches.  Perhaps a clerical spell...'",w, NULL, NULL, NULL, NULL, NULL);
    return;
  }
  
    // if(rumdata[crum][26] > 0) {
  if (new_rumdata[crum].room_monsters_chance > 0) {
    prnt("-> %s says `I believe dangerous creatures lurk here!'",w, NULL, NULL, NULL, NULL, NULL);
    return;
  }

  switch (rnd(8)) {
    case 0:
      prnt("-> %s growls, `Hurry up, %s!  Let us move on.'",w,pname, NULL, NULL, NULL, NULL);
      break;
    case 1:
      prnt("-> %s says `Why are you wasting time, %s?  This place is dangerous!'",w,pname, NULL, NULL, NULL, NULL);
      break;
    case 2:
      prnt("-> %s says `My stomach growls like an Alarian varg. Something to eat would be good.'",w, NULL, NULL, NULL, NULL, NULL);
      break;
    case 3:
      prnt("-> %s says `I favor a quick exit from here.'",w, NULL, NULL, NULL, NULL, NULL);
      break;
    case 4:
      prnt("-> %s grumbles `When I get my hands on Lord Vindroble... He bears the responsibility for our woe!'",w, NULL, NULL, NULL, NULL, NULL);
      break;
    case 5:
      prnt("-> %s stares around the room in anger and mutters `An abomination such as this would never be tolerated in Nordheim.'",w, NULL, NULL, NULL, NULL, NULL);
      break;
    case 6:
      prnt("-> %s searches his carrying pack and mumbles `By the gods! Those accursed fiends took my elven wafers.'",w, NULL, NULL, NULL, NULL, NULL);
      break;
    case 7:
      prnt("-> %s utters an unrepeatable oath of disgust over the general situation.",w, NULL, NULL, NULL, NULL, NULL);
      break;
    default:
      break;
  }
}


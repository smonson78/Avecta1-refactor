
#include "tos_compat.h"

#include "globals.h"
#include "caux.h"
#include "trapaux.h"
#include "instring.h"
#include "start.h"
#include "intell.h"

#include "f4.h"

#include "debug.h"

int sell(int npc)
{
  int done = 0, num = 1, j, amount;

  // get storekeeper ID from room data
  //int i = rumdata[crum][27];
  int i = new_rumdata[crum].room_shopkeeper_id;

  // The shopkeeper:
  uint8_t *c = curmon[npc];
  curmon2_t *ch = &new_curmon2[npc];

  // The player:
  uint8_t *h = curmon[0];
  char str[6], temp[3];

  // Old
  //rumdata[crum][26] = 0;
  // New
  new_rumdata[crum].room_monsters_chance = 0;
  // check_rumdata("sell");

  c[39] = 0;
  c[32] = 6;
  ch->player.unknown39 = 0;
  ch->player.poison_mode_1 = 6;

  // All monsters
  for (j = 4; j < 12; j++) {
    // if (curmon[j][0] && curmon[j][30] == crum && curmon[j][31] < 4) {
    if (new_curmon2[j].monster.in_use && new_curmon2[j].monster.current_room == crum 
        && new_curmon2[j].monster.intelligence_routine < 4) {
      num++;
    }
  }
  
  // Asking price
  sprintf(temp, "%d", selllist[i].min_price + 5);

  if (num == 1 && (selllist[i].attitude == -1 || police) )  {
    prnt("-> The merchant shrieks `Is there no end to this lunatic! Help! "
      "Guards!'", NULL, NULL, NULL, NULL, NULL, NULL);
    
    police = 1;
    c[15] = 8;
    c[16] = 80;
    c[6] = c[8] = 1;
    c[5] = 2;

    ch->player.command = ACTION_WAIT;
    ch->player.unknown16[0] = 80;
    ch->player.unknown6 = 1;
    ch->player.unknown8 = 1;
    ch->player.unknown5 = 2;

    return 1;
  }

  // if (selllist[i].attitude == 1 && adjac(npc, h[24], h[25])) {
  if (selllist[i].attitude == 1 && adjac(npc, player->x, player->y)) {
    prnt("-> The merchant gestures impatiently and says `Come now, you've "
      "had plenty of time to make up your mind.  What do you say?  Yes or no?'",
      NULL, NULL, NULL, NULL, NULL, NULL);
    c[8] = 0;
    c[6] = 1;
    ch->player.unknown8 = 0;
    ch->player.unknown6 = 1;
  }

  if (num > 1) {
    prnt("-> The merchant shouts `Kill the ruffian!'", NULL, NULL, NULL, NULL, NULL, NULL);
    c[15] = 8;
    c[16] = 75;
    ch->player.command = ACTION_WAIT;
    ch->player.unknown16[0] = 75;

    return 1;
  }

  if (c[7] || !adjac(npc, h[24], h[25])) {
    if (!setmove(npc, 1)) {
      pass(npc);
    }
    return 1;
  }

  if (invnpc[0][0] > 9) {
    prnt("-> The merchant laughs `How can you carry anything more? Your bag is\
  completely full! Why don't you drop something?'", NULL, NULL, NULL, NULL, NULL, NULL);
    pass(npc);
    return 1;
  }

  //if (!c[5] && !c[8] && h[49] < selllist[i].min_price) {
  if (!c[5] && !c[8] && player->gold_amount < selllist[i].min_price) {
    prnt("-> The merchant looks down his long nose at %s and sniffs `I believe "
      "the merchandise I handle is currently outside of your financial purview.  I "
      "suggest you leave and go to a cheaper business.'", pname, NULL, NULL, NULL, NULL, NULL);
    c[6] = 1;
    c[8] = 1;
    ch->player.unknown6 = 1;
    ch->player.unknown8 = 1;

    pass(npc);
    return 1;
  }

  if (selllist[i].attitude == 2) {
    prnt("-> The merchant smiles at %s warmly and coos `Ahhh! My good friend! "
      "Evidently you wish to purchase something more!'", pname, NULL, NULL, NULL, NULL, NULL);
  }
  
  if (c[6] == 0) {
    h[15] = 8;
    player->command = ACTION_WAIT; 
    prnt("-> The merchant says `Are you interested in buying one of my "
      "exceptional %ss?  They are a mere %s goldpieces!  Yes or no?'",
      obj[selllist[i].object], temp, NULL, NULL, NULL, NULL);
  }

  if (c[8] || !getword("yes")) {
    c[5] += 1;
    ch->player.unknown5 += 1;

    if (c[5] > 2) {
      prnt("-> The merchant howls `Guards! Guards! There's a thief in my store!'\
    A general clamor can be heard on the street as the cries for help are \
  passed on by the local residents.", NULL, NULL, NULL, NULL, NULL, NULL);
      selllist[i].attitude = -1;
      c[7] = 1;
      ch->player.unknown7 = 1;

      police = 1;
      return 1;
    }

    if (c[5] > 1) {
      c[6] = 1; 
      ch->player.unknown6 = 1;

      prnt("-> The merchant snarls `Get out of my store, you deadbeat, or I'll call the guards!'"
        "  He stands glowering at %s.", &pname[0], NULL, NULL, NULL, NULL, NULL);
    } else {
      prnt("-> The merchant responds `Very well.  Browse for a while.  But I haven't got all day.'",
        NULL, NULL, NULL, NULL, NULL, NULL);
    }
    selllist[i].attitude = 1;
    c[15] = 8;
    c[16] = 30;

    ch->player.command = ACTION_WAIT;
    ch->player.unknown16[0] = 30;    

  } else {
    prnt("-> The merchant smiles and burbles `What excellent taste you have!  How many gold pieces "
      "do you offer for my %s?'", obj[selllist[i].object], NULL, NULL, NULL, NULL, NULL);

    while (!done) {
      prnt("-> [Input your offer]: ", NULL, NULL, NULL, NULL, NULL, NULL);
      instring(row, col, str, 4, 0);
      hold = usedline = 0;
      amount = atoi(str);

      // if (amount <= 0 || amount > h[49]) {
      if (amount <= 0 || amount > player->gold_amount) {
        if (amount <= 0) {
          prnt("-> The merchant snarls `You are deranged!'", NULL, NULL, NULL, NULL, NULL, NULL);
        } else {
          prnt("-> The merchant growls `From the look of your pouch I don't "
            "think you have that many goldpieces!'", NULL, NULL, NULL, NULL, NULL, NULL);
        }

        c[6] = c[8] = 1;
        c[5] = 1;
        ch->player.unknown6 = 1;
        ch->player.unknown8 = 1;
        ch->player.unknown5 = 1;

        done = 1;
        selllist[i].attitude = 1;
        pass(npc);
        break;
      }

      if (amount < c[9]) {
          prnt("-> The merchant shrugs `I can see you are not interested in "
            "bargaining in good faith!'", NULL, NULL, NULL, NULL, NULL, NULL);
          pass(npc);
          done = 1;
          selllist[i].attitude = 1;

          c[8] = 1;
          c[6] = 1;
          ch->player.unknown8 = 1;
          ch->player.unknown6 = 1;

          continue;
      }

      if (amount < selllist[i].min_price) {
          sprintf(temp, "%d", selllist[i].unknown);
          prnt("-> The merchant shrieks `Such an insult!  How am I to feed my "
            "family with a pitiful pittance such as that!  How about %s goldpieces?'",
            temp, NULL, NULL, NULL, NULL, NULL);
          c[9] = amount;
          ch->player.unknown9 = amount;

          continue;
      }  
      if (amount < selllist[i].unknown - c[10] &&
        amount < selllist[i].min_price + selllist[i].unknown - amount ) {
        prnt("-> The merchant moans woefully ", NULL, NULL, NULL, NULL, NULL, NULL);
        j = rnd(8);
        while (j == c[32]) {
          j = rnd(8);
        }

        c[32] = j;
        ch->player.poison_mode_1 = j;

        switch (j) {
          case 0:
            prnt("`Ahh! By the gods! Such a hard trader!'", NULL, NULL, NULL, NULL, NULL, NULL);
            break;
          case 1:
            prnt("`Are you heartless? I'm old and ill!'", NULL, NULL, NULL, NULL, NULL, NULL);
            break;
          case 2:
            prnt("`Think of my poor wife and children!'", NULL, NULL, NULL, NULL, NULL, NULL);
            break;
          case 3:
            prnt("`Mercy! My family lives in a hovel!'", NULL, NULL, NULL, NULL, NULL, NULL);
            break;
          case 4:
            prnt("'Upon my poor sweet mother's grave!'", NULL, NULL, NULL, NULL, NULL, NULL);
            break;
          case 5:
            prnt("`Aiiee!! For all the love of Zandru!'", NULL, NULL, NULL, NULL, NULL, NULL);
            break;
          case 6:
            prnt("`A demon would be easier to sell to!'", NULL, NULL, NULL, NULL, NULL, NULL);
            break;
          case 7:
            prnt("`Did Fat Oskar send you to ruin me?'", NULL, NULL, NULL, NULL, NULL, NULL);
            break;
        }

        c[9] = amount;
        ch->player.unknown9 = amount;

        j = selllist[i].unknown - amount + selllist[i].min_price;
        sprintf(temp, "%d", j);
        prnt("and keens `%s goldpieces and no lower!'", temp, NULL, NULL, NULL, NULL, NULL);
        
        c[10] = selllist[i].unknown - j;
        ch->player.spell_cast = selllist[i].unknown - j;

        continue;
      }

      prnt("-> The merchant throws up his hands and wails `I must accept this "
        "offer!  It is done!'  He fetches the %s from the locked case and gives "
        "it to %s.",
            obj[selllist[i].object], 
            pname, NULL, NULL, NULL, NULL);
      
      // Player gold
      h[49] -= amount;
      player->gold_amount -= amount;
      selllist[i].attitude = 2;
      done = 1;

      c[15] = 8;
      c[16] = 100;
      c[6] = 0;
      c[7] = 0;
      c[8] = 0;
      c[9] = 0;
      c[10] = 0;
      ch->player.command = ACTION_WAIT;
      ch->player.unknown16[0] = 100;
      ch->player.unknown6 = 0;
      ch->player.unknown7 = 0;
      ch->player.unknown8 = 0;
      ch->player.unknown9 = 0;
      ch->player.spell_cast = 0;

      // If item is a bow, get 30 free arrows
      if (selllist[i].object == OBJECT_BOW) {
        prnt("-> The merchant adds `Please accept these thirty fine arrows "
          "as well.' Of course, %s takes them gladly.",
          pname, NULL, NULL, NULL, NULL, NULL);
        curmon[0][44] += 30;
        player->arrows += 30;
      }

      if (selllist[i].object == OBJECT_POWER_VIAL) {
        invtrap(69);
      }

      if (selllist[i].object != OBJECT_TORCH) {
        putaway(0, selllist[i].object);
      } else {
        for (j = 0; j < 4; j++) {
          c = curmon[i];
          c[42] = (c[42] + 30 > 40 ? 40 : c[42] + 30);

          // Just limit it to 40 torches basically
          // new_curmon2[i].player.unlit_torches = new_curmon2[i].player.unlit_torches + 30 > 40 ? 
          //   40 : new_curmon2[i].player.unlit_torches + 30;
          new_curmon2[i].player.unlit_torches += 30;
          if (new_curmon2[i].player.unlit_torches > 40) {
            new_curmon2[i].player.unlit_torches = 40;
          }
        }
      }
    }
  }

  return 1;
}

// NPCs wandering around. Probably shouldn't be in this file.
int wander(int npc)
{
  uint8_t *c = curmon[npc];
  curmon2_t *new_c = &new_curmon2[npc];
  room_object_t *o;
  int i, j, k, l;
  int x = crumobj[new_c->monster.unknown48].x;
  int y = crumobj[new_c->monster.unknown48].y;

  // This is the room number that a door leads to.
  int ex = crumobj[c[48]].target;

  // if (c[48] != 0 && adjac(npc, x, y) && ex != 0 && ex != 8) {
  if (new_c->monster.unknown48 != 0 && adjac(npc, x, y) && ex != 0 && ex != 8) {
    c[15] = 9;
    c[16] = 4;
    c[10] = ex;
    c[9] = c[48];

    new_c->monster.command = ACTION_EXIT;
    new_c->monster.unknown16[0] = 4;
    new_c->monster.spell_cast = ex;
    new_c->monster.spells[4] = new_c->monster.unknown48;

    return 1;
  }

  // if (c[48] == 0 || !los(npc,x,y) || ex == 8) {
  if (new_c->monster.unknown48 == 0 || !los(npc, x, y) || ex == 8) {
    l = 0;

    do {
      l++;
      if (l == 100) {
        break;
      }

      k = 1 + rnd(13); // 1 to 13
      o = &crumobj[k];
      j = o->target;

      if (o->object_id == 0 || j == 0 || j == 15 || j == 16 || j == 13 || j == 8 
          || j == 20 || j == 21 || j == 19 || j == 23 || j == 25 || j == 4
          || j == 5 || o->hide == -2 || adjac(npc, o->x, o->y)) {
        continue;
      }

      x = o->x;
      y = o->y;

      c[48] = k;
      new_c->monster.unknown48 = k;

      break;

    } while (1);

    if (l == 100 || !lom(npc, x, y)) { /* no exits that are not original exit */
      c[48] = 0;
      new_c->monster.unknown48 = 0;

      i = 1 - rnd(3);
      j = 1 - rnd(3);

      // Set destination to a random new adjacent square
      c[26] = c[24] + i;
      c[27] = c[25] + j;
      new_c->monster.old_x = new_c->monster.x + i;
      new_c->monster.old_y = new_c->monster.y + j;

      // Now move there (if possible)
      if (!move_in(npc)) {
        c[48] = 0;
        new_c->monster.unknown48 = 0;

        pass(npc);
      }

      return 1;
    }
  }

  // Set new destination
  c[26] = x;
  c[27] = y;
  new_c->monster.old_x = x;
  new_c->monster.old_y = y;

  // Try to move there
  if (!move_in(npc)) {
    c[48] = 0;
    new_c->monster.unknown48 = 0;
    pass(npc);
  }

  return 1;
}

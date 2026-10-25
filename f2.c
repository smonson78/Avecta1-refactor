#include "tos_compat.h"

/******************************************************************************/
/* I2() initializes the use of magic.                                         */
/******************************************************************************/

#include <xbios.h>

#include "globals.h"
#include "osbind.h"
#include "gemdefs.h"

#include "words.h"
#include "startaux.h"
#include "caux.h"
#include "trapaux.h"
#include "blt.h"
#include "storsc.h"
#include "torches.h"
#include "actmon.h"
#include "damage.h"
#include "explode.h"
#include "fireball.h"
#include "start.h"
#include "dorep.h"
#include "text.h"
#include "cinput.h"
#include "curmon.h"

#include "f5.h"
#include "f6.h"
#include "f10.h"

int savthrow(int npc, int mag);

int i2(int pc)
{
  int16_t status, x, y;
  int i, j, k, numb, ret;
  uint8_t *c = curmon[pc];
  curmon_t *new_c = &new_curmon[pc];
  uint8_t *z;
  char scratch[3],*w = (pc == 0 ? pname : name[c[3]]);

  debug_printf("> i2\n");

  if (outside) {
    return 0;
  }

  if (c[14] <= 0) {
    error(13);
    return 0;
  }

  top(1);
  clrinp();
  
  do {
    vq_mouse(handle, &status, &x, &y);
  } while (status != 0);

  Vsync();
  vbl_animation_off();
  undorep();

  for (i = 0; i < 25; i++) {
    textsix(1,258,1 + 8*i,strlen(spell[i]),spell[i]);
    sprintf(scratch,"%d",1 + (i/5));
    
    if (i != 24) {
      textsix(1,314,1+8*i,1,scratch);
    }
  }

  Vsync();
  vbl_animation_on();
  sgetxy(&x, &y, 2, 0, 24, &numb);
  top(1);
  Vsync();
  clrinp();
  Vsync();
  undorep();

  if (mode) {
    dorep();
  }

  if (numb == 24) {
    return 0;
  }

  numb++;

  if ((numb-1)/5 > (c[51] + 1)/2) {
    error(28);
    return 0;
  }

  if (1 + (numb-1)/5 > c[14]) {
    error(13);
    return 0;
  }

  if ((i = spelinfo[numb-1][1]) > 0) {
    domsg(i);
    header(w);
    winker = pc + 1;
    sgetxy(&x,&y,0,0,0,&ret);
    undomsg();
    winker = 0;
    if (!los(pc,x,y)) {
      error(3);
      return(0);
    }
    z = zline[x][y];
    j = spelinfo[numb-1][0];

    if (j == 1 && z[2] == 0 ) {
      error(8);
      return 0;
    }

    if (j == 2 && z[1] == 0) {
      error(23);
      return 0;
    }

    if (j == 3 && ( z[2] == 0 && (x != c[24] || y != c[25]) ) ) {
      error(8);
      return 0;
    }

    c[9] =  z[1];
    c[8] = 0;
    c[7] = z[2];
    c[5] = x;
    c[6] = y;

    new_c->unknown5[4] = z[1];
    new_c->unknown5[3] = 0;
    new_c->unknown5[2] = z[2];
    new_c->unknown5[1] = y;
    new_c->unknown5[0] = x;
  }

  if (numb == 24) {
    k = 0;
    // Could be "left hand", "right hand"?
    if (c[45] == 80 || c[46] == 80) {
      k = 1;
    }

    // Search inventory for an 80
    for (j = 1; j < invnpc[pc][0]; j++) {
      if (invnpc[pc][j] == 80) {
        k = 1;
        break;
      }
    }

    if (k == 0) {
      prnt("-> %s does not have a seekshard to trigger the teleport spell!",
            w, NULL, NULL, NULL, NULL, NULL);
      return 0;
    }
  }

  if (numb == 13) { /* asshole wants to blink */
    domsg(1);
    header(w);
    winker = pc + 1;
    sgetxy(&x,&y,0,0,0,&ret);
    winker = 0;
    undomsg();
    
    if (!los(pc,x,y) || zline[x][y][0] != 1) {
      error(ERROR_NO_LINE_OF_SIGHT);
      return 0;
    }

    if (zline[x][y][1] != 0 && crumobj[zline[x][y][1]][0] < 41) {
      error(ERROR_PATH_BLOCKED);
      return 0;
    }

    c[26] = x;
    c[27] = y;
    new_c->x = x;
    new_c->y = y;
  }

  c[10] = numb;
  c[15] = 2;
  c[16] = spelinfo[numb-1][2];
  c[14] -= 1 + (numb-1)/5;

  new_c->unknown5[5] = numb;
  new_c->unknown15[0] = 2;
  new_c->unknown15[1] = spelinfo[numb - 1][2];
  new_c->spell_units -= 1 + (numb - 1) / 5;
  
  debug_printf("< i2\n");
  return 1;
}  

void losespel(int pc)
{
  uint8_t *c = curmon[pc];
  curmon_t *new_c = &new_curmon[pc];
  char *w = (pc == 0 ? pname : name[c[3]]);
  char *n = (pc < 4 ? rummsg[0] : msg[0]);

  if (c[21] != 0) {
    prnt("-> The original spell on %s%s was lost!", n, w, NULL, NULL, NULL, NULL);  
    // WTF
    //*(c + c[19]) = c[20];
    // FIXME This can't be easily ported to new_curmon. First we need to work what it does.
    c[c[19]] = c[20];
    // new_c->...something = new_c->unknown15[5];


    c[21] = 0;
    new_c->unknown15[6] = 0;
  }
}

void flash()
{
  int i, j;
  int16_t funk[3];
  Vsync();
  vbl_animation_off();
  for (i = 0; i < 30; i++) {
    for (j = 2; j < 15; j++) {
      funk[0] = rnd(1000);
      funk[1] = rnd(1000);
      funk[2] = rnd(1000);
      vs_color(handle, j, funk);
    }
    Vsync();
  }
  Setpalette(newpal);
  Vsync();
  vbl_animation_on();
}

int o2(int pc)
{
  uint8_t *c = curmon[pc];
  curmon_t *new_c = &new_curmon[pc];
  uint8_t *p, *z;
  char *w,*wt;
  uint8_t *t;
  curmon_t *new_t;
  int k, m, flag = 0, dam, j, trap, num, target, i, x, y;
  w = (pc == 0 ? pname :  name[c[3]]);

  debug_printf("> o2\n");
  
  // Monsters only
  if (pc > 3) {
    if (c[58] != 16) {
      sprintf(junk, "%s", "The ");
      sprintf(&junk[4], "%s", w);
      w = junk;
    }
    num = c[10];
    target = c[39]; // This version is for monsters
    
    t = curmon[target];
    new_t = &new_curmon[target];

    if (!los(pc,t[24],t[25])) {
      flag = 1; 
    }

    wt = (target == 0 ? pname : name[t[3]]);
  }

  // Players only
  if (pc < 4) {
    flag = 0;
    // This is the spell number that was cast (index into array plus one)
    num = c[10];
    if (spelinfo[num - 1][0] != 2) {
      target = c[7]; // ...While this version is for player characters
      t = curmon[target];
      if (!*t || t[30] != crum || !los(pc,t[24],t[25]))  {
        flag = 1;
      }
      wt = (target == 0 ? pname : name[t[3]]);
    } else {
      target = c[9]; // ...And this version is for objects!
      t = crumobj[target];
      wt = obj[*t];
      if (t[0] == 0) {
        flag = 1;
      }
    }
  }
    
  if (spelinfo[num - 1][0] == 0) {
    flag = 0;
  }

  if (flag) {
    prnt("-> %s's %s Spell has no target!", w, spell[num - 1], NULL, NULL, NULL, NULL);
    return 1;
  }

  if (num != 13) {
    prnt("-> %s casts the %s Spell!", w, spell[num - 1], NULL, NULL, NULL, NULL);
  }
  flash();

  switch (num) {
    case 1:  /* cure light wounds but not poison */
      // t is a curmon object
      int randomness = rnd(20);
      t[1] -= 5 + curmon[pc][51] + randomness;
      new_t->unknown1[0] -= 5 + new_curmon[pc].magic_level + randomness;

      if (t[1] < 0) {
        t[1] = 0;
        new_t->unknown1[0] = 0;
      }

      break;

    case 2: /* heavy duty search */
      num = c[37];

      // Searching chance out of 100, i.e. guaranteed result:
      c[37] = 120;
      new_c->perception = 120;
      
      // Search action
      o6(pc);

      c[37] = num; // put back to original value
      new_c->perception = num;

      break;

    case 3:  /* the infamous freeze spell */
      // t is a curmon object    
      if (savthrow(target, c[51])) {
        break;
      }

      if (t[16] == 0) {
        prnt("-> The spell fails!", NULL, NULL, NULL, NULL, NULL, NULL);
        break;
      }

      losespel(target);
      t[16] = 50 + 5*(c[51]);
      t[19] = 0;
      t[20] = 1;
      t[21] = (t[16])/5;

      new_t->unknown15[1] = 50 + 5 * new_c->magic_level;
      new_t->unknown15[4] = 0;
      new_t->unknown15[5] = 1;
      new_t->unknown15[6] = new_t->unknown15[1] / 5; // Note we just set this value earlier

      break;

    case 4:  /* the unvenom spell */
      // t is a curmon
      t[47] = 0;
      new_t->poisoned = 0;
      break;

    case 5:  /* vorpal blade */
      // t is a curmon (player)
      if (target > 3) {
        break;
      }

      losespel(target);

      t[20] = t[43];
      t[43] = 1;
      t[19] = 43;
      t[21] = 20 + 2*(c[51]);
      new_t->unknown15[5] = new_t->base_chance_3_attack;
      new_t->base_chance_3_attack = 1;
      new_t->unknown15[4] = 43;
      new_t->unknown15[6] = 20 + (2 * new_c->magic_level);

      break;

    case 6:  /* speed spell */
      // t is a curmon
      losespel(target);

      t[20] = t[11];
      t[11] = 1;
      t[19] = 11;
      t[21] = 20 + 2*(c[51]);
      new_t->unknown15[5] = new_t->unknown5[6];
      new_t->unknown5[6] = 1;
      new_t->unknown15[4] = 11;
      new_t->unknown15[6] = 20 + (2 * new_c->magic_level);

      break;

    case 7:  /* armor spell */
      // t is a curmon
      losespel(target);

      t[20] = t[12];
      t[12] += c[51];
      t[19] = 12;
      t[21] = 20 + 2*(c[51]);
      new_t->unknown15[5] = new_t->unknown5[7];
      new_t->unknown5[7] += new_c->magic_level;
      new_t->unknown15[4] = 12;
      new_t->unknown15[6] = 20 + (2 * new_c->magic_level);

      break;

    case 8: /* disarm trap */
      // t is a crumobj    
      trap = t[2];
      if (trap > dismax || trap == 0) {
        return 1;
      }

      t[2] = 0;

      untrap(target);
      break;

    case 9: /*  bolt */
    case 12: /* fireball */
    case 16: /* prism */
      // t is a curmon

      // Bolt
      if (num == 9) {
        dam = 10;
        j = 1;
      }
      // Fireball
      if (num == 12) {
        dam = 20;
        j = 0;
      }
      // Prism
      if (num == 16) {
        dam = 40;
        j = 2;
      }

      if (!los(pc,t[24],t[25])) {
        prnt("-> But the target is out of sight!", NULL, NULL, NULL, NULL, NULL, NULL);
        return 1;
      }

      Vsync();
      vbl_animation_off();
      fireball(c[24], c[25], t[24], t[25], j);
      explode(t[24], t[25], j);
      if (savthrow(target,c[51])) {
        break;
      }
      damage(target, dam + rnd(10));
      break;

    case 10: /* thief spell */
      // t is a crumobj    
      num = c[37];
      c[37] = 120;
      new_c->perception = 120;

      // Rook/snoop probably
      o10(pc);
      
      c[37] = num;
      new_c->perception = num;

      break;

    case 11:  /* fear spell */
      // t is a curmon    
      if (savthrow(target, c[51])) {
        break;
      }
      losespel(target);

      t[20] = t[36];
      t[36] = 1;
      t[19] = 36;
      t[21] = 40 + 2*(c[51]);
      new_t->unknown15[5] = new_t->flee;
      new_t->flee = 1;
      new_t->unknown15[4] = 36;
      new_t->unknown15[6] = 40 + (2 * new_c->magic_level);

      damage(target, 0);
      break;

    case 13:  /* blink spell */
      // t is a curmon    
      if (zline[c[26]][c[27]][2] != 0) {
        prnt("-> %s's blink spell fails!", w, NULL, NULL, NULL, NULL, NULL);
        break;
      }

      x = t[24];
      y = t[25];

      t[35] = 0;
      new_t->stink_lines = 0;

      prnt("-> %s blinks out of reality for an instant!", wt, NULL, NULL, NULL, NULL, NULL);
      explode(x, y, 0);
      Vsync();
      storsc(storbuf[target], 16 * x, 16 * y, 1, addr);
      zline[x][y][2] = 0;

      t[24] = c[26];
      t[25] = c[27];
      new_t->x = new_c->old_x;
      new_t->y = new_c->old_y;

      zline[t[24]][t[25]][2] = target;
      
      if (target < 4 && t[41] > 0) {
        setlite(target, t[24] - x, t[25] - y); 
      }

      x = c[26];
      y = c[27];

      if (zline[t[24]][t[25]][5] > 0 || new_rumdata[crum].room_has_bg) {
        explode(x,y,0);
        drawsq(x,y);
      }

      t[15] = 8;
      new_t->unknown15[0] = 8;

      break;

    case 14: /* reveal */
      // no t for reveal   
      for (i = 1; i < 19; i++) {
        c = crumobj[i];
        if (c[8] == 0) {
          c[8] = 1;
          // c is a crumobj! no need to do new_c

          zline[c[6]][c[7]][1] = i;

          if (new_rumdata[crum].room_has_bg || zline[c[6]][c[7]][5] > 0) {
            blt(bitmap[c[0]],16*(c[6]),16*(c[7]));
          }
        }       
      }
      break;

    case 15: /* heal, you sumbitch */
      // t is a curmon    
      t[1] = 0;
      t[47] = 0;
      new_t->unknown1[0] = 0;
      new_t->poisoned = 0;

      break;

    case 17: /* block head */
      // t is a crumobj    
      // I think this actually blocks doorways and thus turns off your police pursuit flags.
      flag = 0;
      // Player and NPCs only
      for (i = 0; i < 4; i++) {
        if (curmon[i][0] && curmon[i][30] == pursuit[2]) {
           prnt("-> %s holds the %s spell to wait for friends!", w, spell[num - 1], NULL, NULL, NULL, NULL);
           flag = 1;
           break;
        }
      }

      if (flag) {
        t[16] = 6;
        return 0;
      }

      if (pursuit[2] == t[3]) {
        pursuit[0] = pursuit[1] = pursuit[2] = 0;
      }
      break;

    case 18: /* charm old scratch */
      // t is a curmon    
      if (savthrow(target,c[51])) {
        break;
      }
      losespel(target);
      t[38] = 1;
      t[20] = 0;
      t[19] = 38;
      t[21] = 20 + 2*(c[51]);
      new_t->unknown38[0] = 1;
      new_t->unknown15[5] = 0;
      new_t->unknown15[4] = 38;
      new_t->unknown15[6] = 20 + (2 * new_c->magic_level);

      dorep();
      break;

    case 19: /* firewall */
      if (specbuf[35] != 0) {
        prnt("-> The existing wall of fire prevents a second spell!", NULL, NULL, NULL, NULL, NULL, NULL);
        break;
      }

      specbuf[35] = c[51]/3;
      specbuf[36] = c[5] + 16*(c[6]);
      for (i = -1; i < 2; i++) {
        for (j = -1; j < 2; j++) {
          z = zline[c[5] + i][c[6] +j];
          if (*z == 1) {
            z[6] = 1;
            z[5] += 1;
            drawsq(c[5] + i, c[6] + j);
          }
        }
      }
      break;

    case 20:  /* destroy an object */
      // t is a crumobj
      if (t[2] > dismax || t[3] > 0 || t[0] == 1) {
        prnt("-> The spell inexplicably fails!", NULL, NULL, NULL, NULL, NULL, NULL);
        break;
      }

      explode(t[6],t[7],0);
      destroy(1, target);
      break;

    case 21: /* summon some kinda monsters */
      // no target for summon
      j = 1 + rnd(7); // Monster type to summon.
      i = 1 + rnd(6); // Number to summon
      
      if (pc < 4) {
        // XY for players. This might be where you clicked?
        x = c[5];
        y = c[6];
      } else {
        // XY for monsters
        x = t[24];
        y = t[25];
      }

      // Activate monsters
      actmon(j, i, x, y);
      
      // Loop over the non-player monsters
      for (i = 4; i < 12; i++) {
        // If the monster type we're summoning matches the type in curmon[i]...
        if (curmon[i][58] == j) { // 58 = monster type.
          // Then we set something unknown here
          curmon[i][38] = 1;
          new_curmon[i].unknown38[0] = 1;
        }
      }

      dorep();
      break;

    case 22: /* death */
      // t will be a curmon here
      if (savthrow(target, c[51])) {
        break;  
      }

      t[0] = 0;
      new_t->in_use = 0;

      damage(target, 1 + t[2] + t[12]);
      break;

    case 23: /* bringin back the dead */
      // t is a crumobj here (a corpse)
      if (t[0] < 38 || t[0] > 40) {
        prnt("-> The life spell does not work on %ss!", wt, NULL, NULL, NULL, NULL, NULL);
        break;
      }

      k = 41 - t[0];
      
      for (i = 0; i < 14; i++) {
        invnpc[k][i] = 0;
      }

      if (target > 13) {
        xobj(t[0], crum);
      }

      p = putbuf;
      m = 0;
      while (m < 320) {
        if (*(p+m+3) != crum || *(p+m+2) != target) {
          m += 4;
          continue;
        }
        putaway(k, *(p+m+1));
        *(p+m) = *(p+m+1) = *(p+m+2) = *(p+m+3) = 0;
      }

      x = t[6];
      y = t[7];

      destroy(1, target);
      explode(x, y, 0);
      actmon(-k, 1, x, y);         
      prnt("-> Eirik shakes his head and mutters `Crom, my eyes spin! But I still live! "
        "I do not wish to know the ensorcelment used to do this.'",
        NULL, NULL, NULL, NULL, NULL, NULL);
      break;

    case 24:  /* time to teleport */
      // no target t for teleport

      if ((k = c[52]) == 0) {
        prnt("-> The teleport invocator cannot find the inline mana thread!", NULL, NULL, NULL, NULL, NULL, NULL);
        break;
      }

      crum = -1;
      x = (c[53])%16;
      y = (c[53])/16;

      // Players only
      for (j = 0; j < 4; j++) {
        c = curmon[j];
        new_c = &new_curmon[j];

        if (c[0] == 1) {
          c[24] = x + j/2 - (j%2);
          c[25] = y - (j+1)/2 + ((j+1)%2);
          c[30] = k;

          new_c->x = x + (j / 2) - (j % 2);
          new_c->y = y - ((j + 1) / 2) + ((j + 1) % 2);
          new_c->current_room = k;
        }
      }
  }

  debug_printf("< o2\n");
  return 1;
}


int savthrow(int npc, int mag)
{
  uint8_t *c = curmon[npc];
  char *w = (npc == 0 ? pname : name[c[3]]);

  if (rnd(100) < (c[51] - mag)*8 || c[54] > 0) {
    if (npc < 4 || curmon[npc][3] == 10) {
      prnt("-> %s resists the spell!", w, NULL, NULL, NULL, NULL, NULL);
    } else {
      prnt("-> The %s resists the spell!", w, NULL, NULL, NULL, NULL, NULL);
    }

    return 1;
  }

  return 0;
}

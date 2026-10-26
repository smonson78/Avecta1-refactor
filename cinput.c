#include "tos_compat.h"

#include "globals.h"
#include "osbind.h"
#include "gemdefs.h"
#include "caux.h"
#include "text.h"

#include "f5.h"

#include "debug.h"

int comwind(int pc);

// This runs at the start of your movement
void clrinp() {
  int i;
  debug_printf("> clrinp()\n");
  Vsync();
  // Clear to end of line from line 3 to 16, column 33.
  // That's 256,16 --> 319, 120
  for (i = 3; i < 17; i++) {
    vs_curaddress(handle, i, 33);
    v_eeol(handle);
  }
} 

int dotop(int pc)
{
  uint8_t *c = curmon[pc];
  //curmon_t *new_c = &new_curmon[pc];
  char scratch[4], l = ' ';

  vs_curaddress(handle,2,33);
  v_eeol(handle);
  textsix(1,260,9,1,"H");
  sprintf(scratch,"%d",c[2] - c[1]);
  textsix(1,269,9,strlen(scratch),scratch);
  if (c[47] > 0) {
    sprintf(scratch, "%c", 'P');
  } else {
    sprintf(scratch, "%c", 'G');
  }
  textsix(1,281,9,1,scratch);
  textsix(1,290,9,1,"S");
  sprintf(scratch,"%d", c[14]);
  textsix(1,299,9,strlen(scratch),scratch);
  
  if (c[21] == 0) {
    sprintf(scratch, "%c", ' ');
  } else {
    if (c[19] == 12) {
      l = 'A';
    }
    if (c[19] == 11) {
      l = 'S';
    }
    if (c[19] == 43) {
      l = 'V';
    }
    if (c[19] == 54) {
      l = 'I';
    }
  }
  sprintf(scratch, "%c", l);
  textsix(1,315,9,1,scratch);

  return 1;
}

int input(int pc)
{
  int16_t x, y;
  int i,j,ret;
  uint8_t *c = curmon[pc];
  curmon_t *new_c = &new_curmon[pc];
  uint8_t *z = NULL;
  char *w = (pc == 0 ? pname : name[c[3]]);

  debug_printf("> input()\n");
  check_curmon("input()");
  
  if (pc > 3) {
    return 0;
  }

  hold = usedline = 0;
  
com:
  // I hope I got these parentheses in the right groupings
  if (((pc == 0) && (c[15] == 1)) || ((pc < 4) && c[38] && (c[15] == 1)) ) {
    combat = comwind(pc);
  }

  if ((pc == 0 || (pc < 4 && c[38])) && combat) {
    return 1;
  }

top:
  Vsync();
  vbl_animation_off();
  header(w);
  dotop(pc);

  vs_curaddress(handle, 3, 33);
  v_rvon(handle);
  printf("        ");
  v_rvoff(handle);
  textsix(1, 272, 17, 6, "Action");
  for (i = 2; i < 15; i++) {
    textsix(1, 258, 9 + 8 * i, strlen(verblist[i - 2]), verblist[i - 2]);
  }
  vsf_interior(handle, 0);
  vsf_style(handle, 0);
  vsf_color(handle, 0);
  v_bar(handle, vxy);
  Vsync();

  c[31] = 0;
  new_c->intelligence_routine = 0;

  winker = pc + 1;
  vbl_animation_on();

  do {
    i = 0;
    sgetxy(&x, &y, 1, 3, 15, &ret);

    if (ret != 0) {
      ret -= 2;
    }

    if (ret == 0 || ret == 14) {
      if ((c[24] == x && c[25] == y) || ret == 14) {
        i = status(pc); /* if status returns a 0 it fixed the action */

        if (!i) {
          return 8;
        }

        goto top;
      }

      z = zline[x][y];

      if (!lom(pc,x,y)) {
        i = 1;
      }

      j = crumobj[z[1]][0];

      if ((j > 0 && j < 41) || (z[2] < 4 && pc > 0 && z[2] > 0) 
          || (z[0] == 2 && z[2] == 0)) {
        i = 1;
      }
    }
  } while (i == 1); 

  winker = 0;
  clrinp();

  if (pc != 0) {
    header(pname);
    dotop(0);
  }

  if (ret == 0) {
    if (z[2] != 0) {
      c[7] = z[2];
      new_c->unknown5[2] = z[2];

      if (z[2] > 3)  {
        combat = 1;
        if (pc == 0 || (pc < 4 && c[38])) {
          c[15] = 1;
          c[39] = c[7];
          new_c->unknown15[0] = 1;
          new_c->unknown38[1] = new_c->unknown5[2];
          goto com;
        } else {
          return 1;
        }
      } else {
        if (pc == 0) {
          i = status(c[7]);
          if (!i) {
            curmon[c[7]][15] = 8;
            new_curmon[new_c->unknown5[2]].unknown15[0] = 8;
          }
        } else {
          goto top;
        }

      }
    } else {
      c[26] = x;
      c[27] = y;
      new_c->old_x = x;
      new_c->old_y = y;

      return 4;
    }
  }

  debug_printf("< input()\n");

  if (ret == 13) {
    return -1;
  }
  if (ret < 3) {
    return ret + 1;
  }
  if (ret > 2) {
    return ret + 2;
  }

  return 0;
}

// Combat Window
int comwind(int pc)
{
  uint8_t *c = curmon[pc];
  uint8_t *c1 = curmon[c[39]];
  curmon_t *new_c = &new_curmon[pc];
  // curmon_t *new_c1 = &new_curmon[new_c->unknown38[1]];

  char scratch[3];
  char *w = (pc == 0 ? pname : name[c[3]]);
  int16_t x, y;
  int i, j, ret;

  if (!c[39] || c1[0] == 0 || c1[30] != crum || !adjac(pc, c1[24], c1[25])) {
    c[34] = c[39] = c[7] = 0;

    new_c->unknown33[1] = 0;
    new_c->unknown38[1] = 0;
    new_c->unknown5[2] = 0;

    clrinp();
    return 0;
  }

  clrinp();
  Vsync();
  vbl_animation_off();
  v_rvon(handle);

  for (i = 1; i < 5; i++) {
    vs_curaddress(handle,i,33);
    printf("         ");
  }

  vs_curaddress(handle,8,33);
  printf("        ");
  textsix(1,260,57,8,"Options:");
  v_rvoff(handle);
  textsix(1,260,1,strlen(w),w);
  sprintf(scratch,"%d",c[2] - c[1]);
  textsix(1,280,9,strlen(scratch),scratch);
  if (c[47] == 0) {
    sprintf(scratch,"%c",'G');
  } else {
    sprintf(scratch,"%c",'P');
  }
  textsix(1,294,9,1,scratch);
  textsix(1,260,17,strlen(obj[c[45]]),obj[c[45]]);
  if (c[34] < -1 || c[34] > 1) {
    c[34] = 0;
    new_c->unknown33[1] = 0;
  }
  if (c[34] == 1) {
    textsix(1,260,25,3,"Hit");
  }
  if (c[34] == -1) {
    textsix(1,260,25,4,"Miss");
  }
  if (c[34] == 0) {
    textsix(1,260,25,4," -- ");
  }
  textsix(1,260,33,strlen(name[*(c1+3)]),name[*(c1+3)]);
  i = (*(c1+1) * 4)/(*(c1+2));
  if (*(c1+36) == 1 && *(c1+1) < *(c1+2)) {
    i = 5;
  }
  textsix(1,260,41,strlen(wordmod[i]),wordmod[i]);
  i = *(c1+40);
  j = *(c1+58);
  if (*(c1+39) != pc) {
    i = 0;
  }
  if (i < 4) {
    textsix(1,260,49,strlen(att[j][i]),att[j][i]);
  }
  if (i == 4) {
    textsix(1,260,49,10,"Cast spell");
  }
  
  for (i = 0; i < 4; i++) {
    textsix(1,260,65+8*i,strlen(posture[i]),posture[i]);
  }

  winker = 1;
  vbl_animation_on();
  sgetxy(&x,&y,1,8,11,&ret);
  clrinp();

  if(ret == 11 || ret == 12) {
    c[40] = c[39] = c[34] = 0;

    new_c->unknown38[2] = 0;
    new_c->unknown38[1] = 0;
    new_c->unknown33[1] = 0;

    return 0;
  }

  if (ret == 0) {
    if (zline[x][y][2] == c[39]) { 
      c[40] = 2;
      new_c->unknown38[2] = 2;
      return 1;
    } else {
      c[34] = c[39] = c[40] = 0;
      new_c->unknown33[1] = 0;
      new_c->unknown38[1] = 0;
      new_c->unknown38[2] = 0;
      return 0;
    }
  } else {
    ret -= 7;
  }

  c[40] = ret;
  new_c->unknown38[2] = ret;

  check_curmon("comwind");

  return 1;
}


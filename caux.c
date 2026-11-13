#include "tos_compat.h"
#include "globals.h"
#include "gemdefs.h"
#include "caux.h"
#include "cinput.h"
#include "startaux.h"
#include "torches.h"
#include "rumdraw.h"
#include "toggle.h"
#include "mouse_on.h"
#include "mouse_off.h"
#include "text.h"

#include "f5.h"

#include "debug.h"

// Weird to split these out but whatever
// Read character, no echo
int read_char_no_echo() {
   return Cnecin() & 0xff;
}

// Is character waiting on console? 0 = no, non-zero = yes
int check_char_waiting() {
   return Cconis();
}

int sgetxy(int16_t *x, int16_t *y, int type, int top1, int bot, int *ret)
{
  int16_t status;
  int oldline = -1, newline=0, newx, newy, oldx = -1, oldy = 0, i, j, k;
  int light, keybd = 0, keystk = 0, mflag=0, rflag=0, tflag = 0, inrflag, intflag = 0;
  zline_t *z;
  // uint8_t *c = NULL;
  curmon_t *new_c = NULL;
  char *string, letter = 0;

  if (winker != 0) {
    // c = curmon[winker - 1];
    new_c = &new_curmon[winker - 1];
  }

  light = new_rumdata[crum].room_has_bg;

  // Clear keyboard buffer by reading everything
  while (check_char_waiting() != 0) {
    read_char_no_echo();
  }

  // Wait for the mouse buttons to be let go
  do {
    vq_mouse(handle, &status, x, y);
  } while (status != 0);

  if (top1 == 3 && bot == 15) {
    keybd = 13;
  }

  if (top1 == 8 && bot == 11) {
    keybd = 4;
  }

  // keybd could be 0, 4, or 13 now

  Vsync();
  mouse_on();

  do {
    // Read a command from the keyboard
    if (check_char_waiting() == -1) {
      keystk = 1;
      letter = read_char_no_echo();
    }

    if (keybd != 0 && keystk) {
      keystk = 0;

      // This loop is going to run 0 times, 4 times, or 13 times
      for (i = 0; i < keybd; i++) {
        // Get a character from some string
        if (keybd == 13) {
          // These are the commands one can do in the game
          // ...there are 13 of these.
          string = verblist[i];
        } else {
          // There are 4 of these.
          string = posture[i];
        }

        // Check whether the user pressed spacebar, the first letter in the string corresponding to i,
        // or a control character that corresponds to the letter in the string.
        // Not sure why the -32 is there but it allows keyboard selection of the action.
        if (letter == ' ' || string[0] == letter || string[0] == (letter - 32)) {
          
          // We selected a command on the keyboard.

          mouse_off();
          top(1);
          clrinp();

          if (rflag) {
            Vsync();
            zline[oldx][oldy].unknown4 = 0;
            
            for (j = 0; j < 2; j++) {
              for (k = 0; k < 2; k++) {
                toggle(2 * oldx + j, 2 * oldy + k, 0);
              }
            }

            if (mflag > 0 && mode) {
              toggle(32, mflag, 7);
              mflag = 0;
            }
          }

          // Main options
          if (keybd == 13) {
            if (letter == ' ') {
              // Spacebar = change item in hand
              *ret = 16;
            } else {
              // Command number from 0 to 12 --> 3 to 15
              *ret = i + 3;
            }
          } else {
            if (letter == ' ') {
              // Spacebar = not sure
              *ret = 12;
            } else {
              // 0 to 3 --> 8 to 11
              *ret = 8 + i;
            }
          }

          // Final ret values here are 3 to 16.
          debug_printf("sgetxy() returned ret=%d\n", *ret);

          return 1;  
        }
      }
    }

    vq_mouse(handle, &status, x, y);  /* sample mouse state */

    // See where the cursor landed
    newx = (*x) / 16;
    newy = (*y) / 16;
    newline = (*y) / 8;
    
    if (newx < 16 && newy < 8) {
        inrflag = 1;
    } else {
        inrflag = 0;
    }
      
    if (type == 0 && (newx > 15 || newy > 7) ) {
      if (rflag) { 
        Vsync();
        for (j = 0; j < 2; j++) {
          for (k = 0; k < 2; k++) {
            toggle(2 * oldx + j, 2 * oldy + k, 0);
          }
        }

        zline[oldx][oldy].unknown4 = 0;
        if (mflag > 0 && mode) {
          toggle(32, mflag, 7);
          mflag = 0;
        }
        rflag = 0;
      }

      oldx = newx;
      continue;
    }

    if (type == 1) {
      if (newx < 16 && newy < 8) {
        inrflag = 1;
      } else{
        inrflag = 0;
      }

      if (newx > 15 && newline >= top1 && newline <= bot) {
        intflag = 1;
      } else {
        intflag = 0;
      }

      if (tflag && !intflag) {
        Vsync();
        mouse_off();
        tflag = 0;
        toggle(32, oldline, 7);
        mouse_on();
      }

      if (rflag && !inrflag) {
        Vsync();
        for (j = 0; j < 2; j++) {
          for(k = 0; k < 2; k++) {
            toggle(2 * oldx + j, 2 * oldy + k, 0);
          }
        }
        zline[oldx][oldy].unknown4 = 0;
        if (mflag > 0 && mode) {
          toggle(32, mflag, 7);
          mflag = 0;
        }
        rflag = 0;
      }

      if (!intflag) {
        oldline = newline;
      }
      
      if (!inrflag) {  
        if(!intflag) {
          oldx = newx;
        }
        oldy = newy;
      }

      if (!intflag && !inrflag) {
        continue;
      }
    }

    if (type == 2 && (newx < 16 || newline < top1 || newline > bot ) ) {
      if (tflag) {
        tflag = 0;
        Vsync();
        mouse_off();
        toggle(32, oldline, 7);
        mouse_on();
      } 
      oldline = newline;
      oldx = newx;
      continue;
    }

    if (type == 0 || (type == 1 && inrflag)) {
      z = &zline[newx][newy];
      if (tflag) {
        tflag = 0;
        Vsync();
        mouse_off();
        toggle(32, oldline, 7);
        mouse_on();
      } 

      // if (newx < 16 && newy < 8 && (*z == 1 || (*z == 2 && z[1] != 0 ) ) ) {
      if (newx < 16 && newy < 8 && (z->unknown0 == 1 || (z->unknown0 == 2 && z->visible != 0 ) ) ) {
        if(newx != oldx || newy != oldy) {
          Vsync();
          if (tflag) {
            tflag = 0;
            toggle(32, oldline, 7);
          }

          mouse_off();
          if (oldx != -1 && rflag) {
            Vsync();
            for (j = 0; j < 2; j++) {
              for (k = 0; k < 2; k++) {
                toggle(2 * oldx + j, 2 * oldy + k, 0);
              }
            }
            zline[oldx][oldy].unknown4 = 0;
            if (mflag > 0 && mode) {
              toggle(32, mflag, 7);
              mflag = 0;
            }
            rflag = 0;
          }

          if (z->unknown5 > 0 || light || (z->monster > 0 && z->monster < 4) || 
            //(winker > 0 && c[24] == newx && c[25] == newy) ) {
            (winker > 0 && new_c->x == newx && new_c->y == newy) ) {
            // if (z[5] > 0 || light || (z[2] > 0 && z[2] < 4) || 
            //   (winker > 0 && new_c->x == newx && new_c->y == newy) ) {              
            Vsync();
            for (j = 0; j < 2; j++) {
              for (k = 0; k < 2; k++) {
                  toggle(2 * newx + j, 2 * newy + k, 0);
              }
            }
            z->unknown4 = 1;
            rflag = 1;
          }

          oldx = newx;
          oldy = newy;

          // if (mflag == 0 && mode && z[2] > 3 && (z[5] > 0 || light) 
          //   && curmon[z[2]][31] < 4 ) {
          if (mflag == 0 && mode && z->monster > 3 && (z->unknown5 > 0 || light) 
             && new_curmon[z->monster].intelligence_routine < 4 ) {
            // mflag = curmon[z[2]][33];
            mflag = new_curmon[z->monster].unknown33[0];
            Vsync();
            toggle(32, mflag, 7);             
          }
          mouse_on();
        }
      } else {
        if (rflag) {
          mouse_off();
          Vsync();
          for (j = 0; j < 2; j++) {
            for (k = 0; k < 2; k++) {
              toggle(2 * oldx + j, 2 * oldy + k, 0);
            }
          }

          zline[oldx][oldy].unknown4 = 0;
          if (mflag > 0 && mode) {
            toggle(32, mflag, 7);
            mflag = 0;
          }
          mouse_on();
          rflag = 0;
        }
        oldx = newx;
        oldy = newy;
      }
    }

    if (type == 2 || (type == 1 && intflag)) {
      if (rflag) {
        mouse_off();
        Vsync();
        
        for (j = 0; j < 2; j++) {
          for (k = 0; k < 2; k++) {
            toggle(2 * oldx + j, 2 * oldy + k, 0);
          }
        }

        zline[oldx][oldy].unknown4 = 0;
        if (mflag > 0 && mode) {
          toggle(32, mflag, 7);
          mflag = 0;
        }
        mouse_on();
        rflag = 0;
        oldx = newx;
        oldy = newy;
      }
    
      if (newx > 15 && newline >= top1 && newline <= bot) {
        if (newline != oldline || oldx < 16) {
          Vsync();
          mouse_off();
          if(oldline != -1 && tflag && oldx > 15) 
            toggle(32, oldline, 7);
          toggle(32, newline, 7);
          tflag = 1;
          oldline = newline;
          oldx = newx;
          mouse_on();
        }
      } else {
        if (tflag) {
          Vsync();
          mouse_off();
          toggle(32,oldline,7);
          tflag = 0;
          mouse_on();
        }
        oldline = newline;
        oldx = newx;
        oldy = newy;
      }
    }

    if (status != 0 && (rflag || tflag || 
        (inrflag && winker > 0 && lom(winker - 1, oldx, oldy)) ) ) {
      mouse_off();
      Vsync();

      if (rflag) {
        zline[oldx][oldy].unknown4 = 0;
        for (j = 0; j < 2; j++) {
          for (k = 0; k < 2; k++) {
            toggle(2 * oldx + j, 2 * oldy + k, 0);
          }
        }
      }

      if (tflag) {
        toggle(32, oldline, 7);
      }

      if (mflag > 0 && mode) {
        toggle(32, mflag, 7);
      }

      *x = oldx;
      *y = oldy;

      if (tflag) {
        *ret = oldline;
      } else {
        *ret = 0;
      }

      // ret could be 0 or some other value that isn't clear
      debug_printf("sgetxy() returned ret=%d\n", *ret);
      return 1;
    }
  } while (1);
}

int lom(int pc, int x, int y)
{
  int x2, y2, i, j, q;
  long int k, l, m, n;
  char z1;
  zline_t *z;
  // uint8_t *c0 = curmon[pc];
  curmon_t *new_c0 = &new_curmon[pc];

  // x2 = *(c0+24); /* fetch the x-coordinate of the PC */
  // y2 = *(c0+25); /* fetch the y-coordinate of the PC */
  x2 = new_c0->x; /* fetch the x-coordinate of the PC */
  y2 = new_c0->y; /* fetch the y-coordinate of the PC */

  i = (x2 - x);            /* find difference between target and PC */
  j = (y2 - y);            
  k = abs(i);   /* take absolute value of i and j */
  l = abs(j);
  
  if (k < 2 && l < 2) { /* always adjacent will make it through */
    return 1;
  }

  if (!zline[x][y].unknown5 && !new_rumdata[crum].room_has_bg && pc < 4) { /* the square is dark */
    return 0;
  }

  if (i != 0) {
    i = (i / k); /* i is +1 if x2>x and i is -1 if x2<x, else = 0 */
  }
  
  if (j != 0) {
    j = (j / l);  /* j is +1 if y2>y and j is -1 if y2<y, else = 0  */
  }

  m = k;      /* m is the temporary value of x separation,called roll variable */
  n = l;      /* n is the temporary value of y separation, called roll variable */
  q = 0;

  while ( (x != x2) || (y != y2) ) {
    if (q == 1) {
      z = &zline[x][y];
      z1 = crumobj[z->visible].object_id;
      
      if (z->visible != 0) {
        if (z1 < 41 && z1 > 1) {
          return 0;
        }
      }

      if (z->monster != 0 || z->unknown0 == 0 || (z->unknown0 == 2 && z1 == 0)) {
        return 0;    
      }

      q = 0;  /* all this is stored in zline array for each room */
    }

    --m;         /* decrement each roll variable */
    --n;

    if (m < 0) {   /* if x separation variable is 0 then roll y */
      y += j;
      q = 1;    /* alert system to check if los is blocked */
      m = k;
    }

    if (n < 0) {   /* same for y separation variable */
      x += i;
      q = 1;
      n = l;
    }
  }

  return 1;  /* if it makes it all the way through then los is open */         
}

// Line-Of-Sight
int los(int pc, int x, int y)
{
  int x2, y2, i, j, q;
  long int k, l, m, n;
  uint8_t *c0 = curmon[pc];
  //curmon_t *new_c0 = &new_curmon[pc];
  zline_t *z;
  char z1;

  x2 = c0[24]; /* fetch the x-coordinate of the PC */
  y2 = c0[25]; /* fetch the y-coordinate of the PC */
  //  x2 = *(new_c0->x); /* fetch the x-coordinate of the PC */
  //  y2 = *(new_c0->y); /* fetch the y-coordinate of the PC */   

  i = x2 - x;            /* find difference between target and PC */
  j = y2 - y;

  k = abs(i);   /* take absolute value of i and j */
  l = abs(j);

  if (k < 2 && l < 2) {
    /* always adjacent will make it through */
    return 1;
  }

  if (!zline[x][y].unknown5 && !new_rumdata[crum].room_has_bg && pc < 4) {
    /* the square is dark */
    return 0;
  }

  if (i != 0) {
    i = ( i/k ); /* i is +1 if x2>x and i is -1 if x2<x, else = 0 */
  }

  if (j != 0) {
    j = ( j/l );  /* j is +1 if y2>y and j is -1 if y2<y, else = 0  */
  }

  m = k;      /* m is the temporary value of x separation, called roll variable */
  n = l;      /* n is the temporary value of y separation, called roll variable */
  q = 0;

  while ( (x != x2) || (y != y2) ) {
    
    if (q == 1) {
      z = &zline[x][y];
      z1 = crumobj[z->visible].object_id;
      
      if (z->unknown0 != 1 || (z->visible != 0 && z1 == 1)) {
        return 0;
      }
      q = 0;  /* all this is stored in zline array for each room     */
    }

    --m;         /* decrement each roll variable */
    --n;

    if (m < 0) {   /* if x separation variable is 0 then roll y */
        y += j;
        q = 1;    /* alert system to check if los is blocked   */
        m = k;
    }

    if (n < 0) {   /* same for y separation variable            */
        x += i;
        q = 1;
        n = l;
    }
  }

  return 1;  /* if it makes it all the way through then los is open */         
}

/*****************************************************************************/
/* NEXTXY(XLO,YLO,XHI,YHI,XNOW,YNOW,&XNEXT,&YNEXT) is a dandy call to find   */
/* the next coordinates along the line joining the coordinates (xlo,ylo) to  */
/* (xhi,yhi) given that the character or whatever is currently at the (xnext,*/
/* ynext) coordinates.  It passes the coordinate of the                      */
/* square which the PC is going to move to into the addresses &XNEXT,&YNEXT. */
/* In addition, it returns the direction of the move.  The code for that is  */
/* 0=North,1=Northeast,2=East,3=Southeast,4=South,5=Southwest,6=West,7=North-*/
/* west,8=Samesquare.                                              12/12/85  */
/*****************************************************************************/

void nextxy(int xlo, int ylo, int xhi, int yhi, int xnow, int ynow, int *xnext, int *ynext)
{
  int i, j, k = 1, rollx, rolly, signx = 0, signy = 0;

  i = xhi - xlo;
  j = yhi - ylo;

  rollx = abs(i);
  rolly = abs(j);

  if (i != 0) {
    signx = i/rollx;
  }

  if (j != 0) {
    signy = j/rolly;
  }

  do {
    if (line(xlo, ylo, xhi, yhi, xnow + signx, ynow + signy)) {
      *xnext = xnow + signx;
      *ynext = ynow + signy;
      k = 0;
      break;
    }

    if (line(xlo,ylo,xhi,yhi,xnow+signx,ynow)) {
      *xnext = xnow + signx;
      *ynext = ynow;
      signy = 0;
      k = 0;
      break;
    }

    *xnext = xnow;
    *ynext = ynow + signy;
    k = 0;    
  } while (k == 1);
}


int line(int xlo, int ylo, int xhi, int yhi, int xnow, int ynow)
{
  int i, j, q;
  int k, l, m, n;

  i = xhi - xlo;            /* find difference between two coordinates */
  j = yhi - ylo;

  if ((xnow == xlo && ynow == ylo) || (xnow == xhi && ynow == yhi)) {
    return 1;
  }

  k = abs(i);   /* take absolute value of i and j */
  l = abs(j);

  if (i != 0) {
    i = ( i/k ); /* i is +1 if xhi>xlo and i is -1 if xhi<xlo, else = 0 */
  }

  if (j != 0) {
    j = ( j/l );  /* j is +1 if yhi>ylo and j is -1 if yhi<ylo, else = 0  */
  }

  m = k;      /* m is the temporary value of x separation,called roll variable */
  n = l;      /* n is the temporary value of y separation, called roll variable */
  q = 0;

  while ((xlo != xhi) || (ylo != yhi)) {
    if (q == 1) {
      if (xnow == xlo && ynow == ylo) {
        return 1; /* then it's along the line */
      }
      q = 0;  
    }

    --m;         /* decrement each roll variable */
    --n;

    if (m < 0) {   /* if x separation variable is 0 then roll y */
      ylo += j;
      q = 1;    /* alert system to check if los is blocked   */
      m = k;
    }

    if (n < 0) {   /* same for y separation variable            */
      xlo += i;
      q = 1;
      n = l;
    }
  }

  return 0;  /* if it makes it all the way through then it's NOT in line */     
}

int rnd(int max)
{
  if (max == 0 || max == 1) {
    return 0;
  }

  return Random() % max;
}

int adjac(int pc, int x, int y)
{
  int xnow = curmon[pc][24];
  int ynow = curmon[pc][25];
  //  int xnow = new_curmon[pc].x;
  //  int ynow = new_curmon[pc].y;
  x = abs(xnow - x);
  y = abs(ynow - y);

  if (x <= 1 && y <= 1) {
    return 1;
  } else {
    return 0;
  }
}	

/*****************************************************************************/
/* If flag is set this function moves the object randomly to a square around */
/* x,y.  If flag is zero then it moves the object to x,y.                    */
/*****************************************************************************/

int move(int obj, int flag, int x, int y)
{
  room_object_t *c = &crumobj[obj];
  zline_t *z;
  int done, i, j;
  int light = new_rumdata[crum].room_has_bg;

  if (flag) {
    done = 0;
    for (i = -1; i <= 1; i++) {
      for (j = -1;j <= 1;j++) {
        if (i == 0 && j == 0) {
          continue;
        }

        z = &zline[x + i][y + j];
        if (z->unknown0 == 1 && z->visible == 0 && z->monster == 0 &&
          (curmon[0][24] != x + i || curmon[0][25] != y + j)) {
          // if (*z == 1 && z[1] == 0 && z[2] == 0 &&
          //   (new_curmon[0].x != x+i || new_curmon[0].y != y+j)) {
          if (obj > 13) {
            xobj(c->object_id, crum);
          }

          zline[x][y].visible = 0;

          if (light || zline[x][y].unknown5 > 0) {
            drawsq(x, y);
          }

          z->visible = obj;
          c->x = x + i;
          c->y = y + j;

          if (obj > 13) {
            storobj(c->object_id, x + i, y + j);
          }

          if (light || z->unknown5 > 0) {
            drawsq(x + i, y + j);
          }

          done = 1;
          break;
        }
      }

      if (done) {
        break;
      }
    }
  }

  if (!flag) {
    zline[c->x][c->y].visible = 0;
    
    if (light || zline[c->x][c->y].unknown5 > 0) {
      drawsq(c->x, c->y);
    }

    c->x = x;
    c->y = y;
    zline[x][y].visible = obj;

    if (light || zline[x][y].unknown5 > 0) {
      drawsq(x, y);
    }

    if (obj > 13) {
      xobj(c->object_id, crum);
      storobj(c->object_id, x, y);
    }
    done = 1;
  }

  return done;
}

int putinto(int rumobj, int thing)
{
  int i = 0;
  while (putbuf[i] != 0 && i < 320) {
    i += 4;
  }

  if (i >= 320) {
    return 0;
  }

  putbuf[i] = 1;
  putbuf[i+1] = thing;
  putbuf[i+2] = rumobj;
  putbuf[i+3] = crum;

  return 1;
}

int putaway(int pc, int thing)
{
  uint8_t *i = invnpc[pc];
  int j = 1;

  if (thing == 0) {
    return 1;
  }

  if (i[0] > 9 || thing == OBJECT_TORCH) {
    return 0;
  }

  while (i[j] != 0 && j < 11) {
    j++;
  }

  if (j == 11) {
    return 0;
  }

  if (thing < 46 && thing > 40) {
    curmon[pc][32] = 0;
    new_curmon[pc].poison_mode_1 = 0;
  }

  i[j] = thing;
  i[0] += 1;

  return 1;
}

int takeout(int pc, int thing)
{
  uint8_t *i = invnpc[pc];
  int j = 1;

  if (thing == 0) {
    return 0;
  }
  
  while (i[j] != thing && j < 11) {
    j++;
  }

  if (j == 11) {
    return 0;
  }
  
  i[0] -= 1;
  i[j] = 0;

  return 1;
}

int remove_object(int rumobj, int thing)
{
  int i = 0;
  
  while ((putbuf[i + 3] != crum || putbuf[i + 2] != rumobj || putbuf[i + 1] != thing) && i < 320) {
    i += 4;
  }

  if (i >= 320) {
    return 0;
  }

  putbuf[i] = 0;
  putbuf[i + 1] = 0;
  putbuf[i + 2] = 0;
  putbuf[i + 3] = 0;
  
  return 1;
} 

// Put an object into the room_zero array
int storobj(int thing, int x, int y)
{
  // Room Zero
  //uint8_t *r = &room_zero;

  // int i = 1;
  // while (r[i] != 0 && i < 157) {
  //    i += 3;
  // }

  int i;
  for (i = 0; i < 52; i++) {
    if (room_zero.unknown2[i].thing_id == 0) {
      break;
    }
  }

  // if (i >= 157)
  //    return 0;
  if (i >= 52) {
    return 0;
  }

  // Old rumdata[0]
  // r[i * 3 + 1] = thing;
  // r[i * 3 + 2] = 16*y + x;
  // r[i * 3 + 3] = crum;

  // New room_zero
  room_zero.unknown2[i].thing_id = thing;
  room_zero.unknown2[i].thing_xy = (16 * y) + x;
  room_zero.unknown2[i].room_id = crum;   

  // check_rumdata("storobj");

  return 1;
}

int xobj(int thing, int room)
{
  //int i = 1;
  int i = 0;
  // Room zero
  //uint8_t *r = rumdata[0];

  // while( ( r[i] != thing || *(r+i+2) != room) && i < 157) {
  //    i += 3;
  // }
  // Search for a room zero entry which has the thing and room we're looking for
  while (
    (room_zero.unknown2[i].thing_id != thing || room_zero.unknown2[i].room_id != room) 
    && i < 52) {
    i++;
  }

  if (i >= 52) {
    // Didn't find it
    return 0;
  }

  // Now blank it out to zeroes
  //r[i] = *(r+i+1) = *(r+i+2) = 0;
  room_zero.unknown2[i].thing_id = 0;
  room_zero.unknown2[i].thing_xy = 0;
  room_zero.unknown2[i].room_id = 0;
  
  return 1;
}

int status(int pc)
{
  int16_t i, j;
  int ret, flag = 0;
  uint8_t *c = curmon[pc];
  // curmon_t *new_c = &new_curmon[pc];
  char scratch[3];
  char *w = (pc == 0 ? pname : name[c[3]]);
  //char *w = (pc == 0 ? pname : name[new_c->monster_name_id]);

  // Player characters only
  if (pc > 3) {
    return 1;
  }
  
  player_t *p = &players[pc];

  Vsync();
  vbl_animation_off();
  top(1);
  clrinp();
  header(w);

  for (i = 0; i < 2; i++) {
    textsix(1, 260, 9 + 8 * i, strlen(statword[i]), statword[i]);
  }

  v_rvon(handle);
  for (i = 2; i < 5; i++) {
    vs_curaddress(handle, 2 * i, 33);
    printf("        ");
    textsix(1, 260, 16 * i - 7, strlen(statword[i]), statword[i]);
  }
  // textsix(1, 260, 33, strlen(obj[c[45]]), obj[c[45]]);
  // textsix(1, 260, 49, strlen(obj[c[46]]), obj[c[46]]);
  textsix(1, 260, 33, strlen(obj[p->good_hand]), obj[p->good_hand]);
  textsix(1, 260, 49, strlen(obj[p->other_hand]), obj[p->other_hand]);

  // sprintf(scratch, "%d", c[2] - c[1]);
  sprintf(scratch, "%d", p->max_health - p->damage);
  textsix(1, 300, 9, 2, scratch);

  // if (c[47] > 0) {
  if (p->poisoned > 0) {
    sprintf(scratch, "%c", 'P');
  } else {
    // Good health
    sprintf(scratch, "%c", 'G');
  }
  textsix(1, 314, 9, 1, scratch);

  // sprintf(scratch, "%d", c[14]);
  sprintf(scratch, "%d", p->spell_units);
  textsix(1, 300, 17, strlen(scratch), scratch);

  v_rvoff(handle);

  for (i = 5; i < 9; i++) {
    textsix(1, 260, 25 + 8 * i, strlen(statword[i]), statword[i]);
  }

  Vsync();
  vbl_animation_on();
  sgetxy(&i, &j, 2, 8, 11, &ret);
  top(1);
  clrinp();
  
  ret -= 7;
  switch (ret) {
    case 1:
    case 2:
      if (ret == 2) {
        // if (c[41] > 0) {
        if (p->lit_torch_turns > 0) {
          c[46] = 0;
          p->other_hand = 0;

          // litetrol(c[24], c[25], 0);
          litetrol(p->x, p->y, 0);
        }

        // if (c[41] > 64) {
        if (p->lit_torch_turns > 64) {
          c[42] += 1;
          p->unlit_torches++;
        }

        // if (c[41] > 0) {
        if (p->lit_torch_turns > 0) {
          c[41] = 0;
          p->lit_torch_turns = 0;
        }
      }

      j = invent(pc);
      if (j == 0) {
        if (!putaway(pc, c[44 + ret])) {
          error(ERROR_INVENTORY_TOO_FULL);
          break;
        }

        c[44 + ret] = 0;
        if (ret == 1) {
          p->good_hand = 0; // good hand object
        } else {
          p->other_hand = 0; // other hand object
        }
        
        break;
      }

      // ret == 1 represents an object in the good hand
      // ret == 2 represents an object in the other hand
      
      int object_in_hand = ret == 1 ? p->good_hand : p->other_hand;
      // if (c[44 + ret] != 0 ) { 
      if (object_in_hand != 0 ) { 
        if (!putaway(pc, object_in_hand)) {
          error(ERROR_INVENTORY_TOO_FULL);
          break;
        }
      }

      takeout(pc, j);
      c[44 + ret] = j;
      if (ret == 1) {
        p->good_hand = j;
      } else {
        p->other_hand = j;
      }      

      // if (c[45] < 46 && c[45] > 40) {
      if (p->good_hand < 46 && p->other_hand > 40) {
        // objects 41, 42, 43, 44, 45 which are
        // broadsword, scimitar, mace, dagger, bow - i.e. weapons
        c[32] = c[45];
        p->poison_mode_1 = p->good_hand;
      }

      break;

    case 3:
      invent(pc);
      flag = 1;
      break;

    default:
      flag = 1;
      break;
  }

  top(1);
  clrinp();
  dotop(pc);
  
  return flag;
}

/****************************************************************************/
/* INVENT() is a call to update the inventory of the hero.  It updates the  */
/* status as well as which possessions are in the person's inventory.       */
/****************************************************************************/
int invent(int pc)
{
  int16_t i, j;
  int top = 3, ret, bot;
  char scratch[16];

  bot = 2 + listinv(pc, scratch);
  sgetxy(&i, &j, 2, top, bot + 1, &ret);

  if (ret == bot + 1) {
    return 0;
  }

  return scratch[ret];
}

int listinv(int pc, char *scratch)
{
  int j;
  // uint8_t *c = curmon[pc];
  uint8_t *o = invnpc[pc];
  player_t *p = &players[pc];
  char word[3];

  Vsync();
  vbl_animation_off();
  vs_curaddress(handle, 1, 33);
  v_rvon(handle);
  printf("        ");
  v_rvoff(handle);
  textsix(1, 260, 1, 9, "INVENTORY");
  prhand(pc);
  
  //  scratch[1] = c[45];
  //  scratch[2] = c[46];
  scratch[1] = p->good_hand;
  scratch[2] = p->other_hand;

  int bot = 0;
  for (j = 1; j < 14; j++) {
    if (o[j] != 0) {
      bot++;
      textsix(1, 260, 17 + bot * 8, strlen(obj[o[j]]), obj[o[j]]);
      scratch[bot + 2] = o[j];
    }
  }
  textsix(1, 260, 25 + bot * 8, 9, "No Choice");
  
  textsix(1, 260, 33 + 8 * bot, 7, "Torches");
  //  sprintf(word, "%d", c[42]);
  sprintf(word, "%d", p->unlit_torches);
  textsix(1, 308, 33 + 8 * bot, strlen(word), word);

  textsix(1, 260, 41 + 8 * bot, 4, "Gold");
  // sprintf(word, "%d", c[49]);
  sprintf(word, "%d", p->gold_amount);
  textsix(1, 308, 41 + 8 * bot, strlen(word), word);
  
  Vsync();
  vbl_animation_on();

  invnpc[pc][0] = bot;

  return bot;
}

// Clear this many text rows from column 33 to the end
void top(int line)
{
   int i;
   for (i = 2; i >= line; i--) {
      vs_curaddress(handle, i, 33);
      v_eeol(handle);
   }
}

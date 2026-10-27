#include "globals.h"
#include "trapres.h"
#include "caux.h"
#include "debug.h"

// TRIGger TRap
// ...not sure about the OL part of the name

/* pc = character in room which triggered trap */
void trigtrol(int pc)
{
  int j, l, flag, type, i, x1, y1, hold = 0, x, y;
  uint8_t trapvec;
  uint8_t *c = curmon[pc];
  // curmon_t *new_c = &new_curmon[pc];
  uint8_t *t, *inv;

  check_curmon("trigtrol start");

  if (triglist[0] == 0) { /* triglist[0] gives the number of traps in room */
    // No traps here, don't bother checking
    return;
  }

  // Coordinates of the character
  x = c[24];
  y = c[25];

  for (i = 1; i < 25; i++) {
    if (triglist[i] == 0) {
      continue;
    }

    trapvec = triglist[i];
    t = trigval[trapvec];

    // Bit field
    type = t[0];

    x1 = (t[1]) % 16;
    y1 = (t[1]) / 16;

    // Flag indicating consqeuences will happen for triggering the trap
    flag = 1;

    for (j = 1; j < 32; j *= 2) {

      if ((j & type) ) {
        switch(j) { 
          case 1:
            if (c[24] != x1 || c[25] != y1) {
              flag = 0;
            }
            break;

          case 2:
            if (!adjac(pc, x1, y1)) {
              flag = 0;
            }
            break;

          case 4:
          case 8:

            // Monsters:
            if (pc > 3) {
              flag = 0;
              break;
            }

            // Playable characters:
            inv = invnpc[pc];    
            for (l = 1; l < 11; l++) {
              if (inv[l] == t[4]) {
                hold = 1;
                break;
              }
            }

            if (c[45] == t[4] || c[46] == t[4]) {
              hold = 1;
            }
            
            if ((j == 4 && hold == 0) || (j == 8 && hold == 1)) {
              flag = 0;
            }
            break;

          case 16:
            if (c[15] != t[3]) {
              flag = 0;
            }
            
            if (t[2] != 0 && c[9] != t[2]) {
              flag = 0;
            }

            if (t[4] != 0 && c[8] != t[4]) {
              flag = 0;
            }

            break;
        }
      }
    }

    if (flag) {
      trapres(pc, t[5], x, y);
    }
  }

  check_curmon("trigtrol end");
}

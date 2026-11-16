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
  curmon_t *new_c = &new_curmon[pc];
  uint8_t *t, *inv;

  check_curmon("trigtrol start");

  /* triglist[0] gives the number of traps in room */
  if (triglist[0] == 0) { 
    // No traps here, don't bother checking
    return;
  }

  // Coordinates of the character
  // x = c[24];
  // y = c[25];
  x = new_c->x;
  y = new_c->y;

  for (i = 1; i < 25; i++) {
    if (triglist[i] == 0) {
      continue;
    }

    trapvec = triglist[i];
    t = trigval[trapvec];

    // Bit field
    type = t[0];

    x1 = t[1] % 16;
    y1 = t[1] / 16;

    // Flag indicating consqeuences will happen for triggering the trap
    flag = 1;

    // This is a 5-bit bitfield
    for (j = 1; j < 32; j <<= 1) {

      if (j & type) {
        switch (j) {

          // Bit 0 set: trap is only set off if victim is on the same square
          case 1:
            // if (c[24] != x1 || c[25] != y1) {
            if (new_c->x != x1 || new_c->y != y1) {
              flag = 0;
            }
            break;

          // Bit 1 set: trap is only set off if on an adjacent square to the victim 
          case 2:
            if (!adjac(pc, x1, y1)) {
              flag = 0;
            }
            break;

          // Bits 2 or 3 set: monsters don't set the trap off and some other logic here
          case 4:
          case 8:

            // Monsters don't set off traps
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

          // Bit 4 set:
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

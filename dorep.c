
#include "tos_compat.h"

#include "globals.h"
#include "gemdefs.h"
#include "text.h"

void undorep()
{
  Vsync();

  for (int i = 16; i < 26; i++) {
    // Move text cursor
    vs_curaddress(handle, i, 33);
    // Erase to end of line
    v_eeol(handle);
  }
}

void dorep()
{
  int i, j, k, x = 256, y = 127, w = 62, h = 72;
  uint8_t *c;
  char scratch[13];

  // Polyline coords
  int16_t pxy[10];
  pxy[0]=256;
  pxy[1]=127;

  // Across to the right horizontally
  pxy[2]=319;
  pxy[3]=127;

  // Then down to the bottom of the screen
  pxy[4]=319;
  pxy[5]=199;

  // Back to the left to line up with the starting point
  pxy[6]=256;
  pxy[7]=199;

  // Then return to start to form a rectangle
  pxy[8]=256;
  pxy[9]=127;
  k = 0;

  undorep();

  for (i = 4; i < 12; i++) {
    c = curmon[i];
    if (c[0] == 1 && c[30] == crum && c[31] < 4) {
      k = 1;
    }
  }

  if (k == 0) {
    mode = 0;
    return;
  }

  mode = 1;
  Vsync();
  vbl_animation_off();
  form_dial(1,0,0,0,0,x,y,w,h);
  v_pline(handle, 5, pxy);

  v_rvon(handle);
  vs_curaddress(handle,17,33);
  printf("        ");
  v_rvoff(handle);
  textsix(1,259,129,10,"Opponents ");

  j = 17;
  for (k = 4; k < 12; k++) {
    c = curmon[k];
    if (c[0] != 0 && c[30] == crum && c[31] < 4) {
      sprintf(scratch, "%s", name[c[3]]);
      textsix(1, 259, 1 + (8 * j), strlen(scratch), scratch);

      c[33] = j;
      i = (c[1] * 4) / (c[2]);
      if (c[36] == 1) {
        i = 5;
      }
      if (c[38]) {
        i = 6;
      }
      textsix(1, 313, 1 + (8 * j), 1, mod[i]);
      j++;
    }
  }
  
  Vsync();
  vbl_animation_on();
}

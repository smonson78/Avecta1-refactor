#include "globals.h"
#include "osbind.h"
#include "gemdefs.h"
#include "title.h"
#include "mouse_off.h"
#include "caux.h"
#include "startaux.h"
#include "mouse_on.h"
#include "text.h"
#include "cinput.h"
#include "texwin.h"

#include "debug.h"

void domsg(int i) {
  int x = 256, y = 8, w = 64, h = 40;
  char scratch[10];
  top(2);

  int16_t pxy[10];
  // Top corner of rectangle
  pxy[0] = 256;
  pxy[1] = 8;

  // Horizontal line to the top right corner
  pxy[2] = 319;
  pxy[3] = 8;

  // Vertical line down to bottom right
  pxy[4] = 319;
  pxy[5] = 48;

  // Horizontal line back to the bottom left
  pxy[6] = 256;
  pxy[7] = 48;

  // Back up to the original point
  pxy[8] = 256;
  pxy[9] = 8;

  Vsync();
  vbl_animation_off();
  form_dial(0, 0, 0, 0, 0, x, y, w, h);
  form_dial(1, 0, 0, 0, 0, x, y, w, h);
  v_pline(handle, 5, pxy);
  textsix(1, 259, 17, 7, "  Point");
  textsix(1, 259, 25, 8, "  at the");
  sprintf(scratch, "%s", com[i]);
  textsix(1, 259, 33, strlen(scratch), scratch);
  Vsync();
  vbl_animation_on();
}

void undomsg() {
  int x = 255, y = 0, w = 64, h = 40;
  Vsync();
  vbl_animation_off();
  form_dial(2, 0, 0, 0, 0, x, y, w, h);
  top(1);
  clrinp();
  Vsync();
  vbl_animation_on();
}

int init(int flag) {
  int i;
  char val[3];
  uint8_t *c;

  // Move cursor to position (1, 1)
  vs_curaddress(handle, 1, 1);
  // Clear screen to end
  v_eeos(handle);

  row = 17;
  if (flag == 1) {
    texwin();
    return(1);
  }

  dungeon = 2;
  outside = 0;

  for (i = 4; i < 12; i++) {
    // From 4 to 11 ( in a [0 .. 11] block)
    c = curmon[i];

    // Clear the entire line of 60 chars
    // for (j = 0; j < 60; j++) {
    //   *(c+j) = 0;
    // }
    memset(curmon[i], 60, 0);
  }

  // Initialise to zero all of pursuit[3]
  // for (i = 0; i < 3; pursuit[i++] = 0)
  //   ;
  pursuit[0] = 0;
  pursuit[1] = 0;
  pursuit[2] = 0;

  c = curmon[0];
  crum = -1;

  // equivalent to curmon[0][0] = 0
  *c = 1;

  // Maybe starting stats on the player.
  c[46] = c[45] = c[31] = c[32] = c[33] = c[34] = c[35] = c[1] = 0;
  c[59] = 2;
  c[42] = 20;
  c[41] = 0;
  c[32] = 2;
  c[33] = 4;
  c[4] = 81;

  // Clear all of invnpc[4][20]
  // for (j = 0; j < 4; j++) {
  //   for (i = 0; i < 20; i++) {
  //     invnpc[j][i] = 0;
  //   }
  // }
  memset(invnpc, 4 * 20, 0);

  invnpc[0][0] = 1;
  invnpc[0][2] = 42;

  c[44] = 50;
  c[47] = 0;

  // Starting gold
  c[49] = 50;

  if (flag == 2) {
    c[14] = 6*(c[51]);
    texwin();
    return 1;
  }

  val[0] = 10;
  val[1] = 3;
  val[2] = 1;

  c[37] = 5 + 20 * val[2];
  c[50] = 3 + 2 * val[1];
  c[14] = 6 * val[0];
  c[51] = val[0];
  c[2] = 50;
  c[11] = 4;

  texwin();

  usedline = 0;
  hold = 0;

  return 0;
}

int ruminit()
{
  uint8_t *c = curmon[1];

  // Zero out 660 bytes
  //for (i = 0; i < 660; *(c + (i++)) = 0) {
  //}
  memset(c, 660, 0);

  if (!loadnew()) {
    mouse_on();
    Setpalette(oldpal);
    Vsync();
    form_alert(1, "[1][There are data files missing!][OK]");
    mouse_off();
    return(1);
  }

  return 0;
}

int error(int i) {
  Vsync();
  vbl_animation_off();
  mouse_on();
  i = form_alert(1,errmsg[i]);
  mouse_off();
  Vsync();
  vbl_animation_on();
  return i;
}

int32_t savpal() {
  volatile int16_t *j;
  int i;
  j = (volatile int16_t *)0xFF8240; // Must be the address of the palette in TOS
  for (i=0; i<16; i++) {
    oldpal[i] = *(j+i);
  }
  return 0;
}

int console() {
  int i;
  uint8_t *c = curmon[0];
  for (i = 1; i < 17; i++) {
    vs_curaddress(handle, i ,1);
    v_eeol(handle);
  }
  for (i = 17; i < 26; i++) {
    vs_curaddress(handle, i, 31);
    v_eeol(handle);
  }
  for(i = 17; i < 26; i++) {
    vs_curaddress(handle, i, 1);
    printf(" ");
  }
  vsf_interior(handle, 1);
  vsf_color(handle, 2);
  v_contourfill(handle, 300, 10, -1);
  mouse_on();

  if (dungeon == 2 && police) {
    form_alert(1,"[1][The blond man murmurs |`Unworthy soul!    |No resurrection!   "
      "|You violate the law!'][Death!]");
    return 0;
  }

  new = 0;
  loadnew();
  new = 1;
  c[30] = 29;

  form_alert(1,"[1][The blond man murmurs  |`Resurrection again... |My strength fails me..."
    "|Such a great distance...|So little time.' ][ Once more ]");

  specbuf[28] = 1;
  mouse_off();
  Setpalette(newpal);
  Vsync();
  init(2);

  return 3;
}

// Endgame
void congratulate() {
  int i;
  int16_t rgb[3];

  Vsync();
  vbl_animation_off();
  vs_curaddress(handle,1,1);
  v_eeos(handle);
  vs_curaddress(handle,5,1);

  printf("\n%s appears in the peaceful",&pname[0]);
  printf("\ncountryside around the abbey's entrance."); 
  printf("\nBirds sing again, the sun shines, and");
  printf("\nthe air seems so clean and fresh.");
  printf("\nIt's as if the whole world is ");
  printf("\ncelebrating %s's great...",pname);
  printf("\n");
  printf("\n[More...]");
  Bconin(2);

  vs_curaddress(handle,1,1);
  v_eeos(handle);
  rgb[0] = rgb[1] = rgb[2] = 0;
  vs_color(handle,0,rgb);
  Vsync();
  prntbig("VICTORY OVER",4,1);
  prntbig(" MELKTHROP!",4,5);

  // Flash some colours
  do {
    rgb[0] = Random();
    rgb[1] = Random();
    rgb[2] = Random();
    rgb[0] %= 1000;
    rgb[1] %= 1000;
    rgb[2] %= 1000;  
    vs_color(handle, 2, rgb);
    i = Bconstat(2);
  } while (i == 0);
  Vsync();
}


int loadnew() {
  uint8_t *c = curmon[0];
  int fhandle, i, j, old, d1 = 0, d2 = 0, d3 = 0, flag = 0;

  old = dungeon;
  if (new == 0) {
    if(specbuf[32]) {
      flag = 1;
    }

    police = 0;
    c[55] = 53;
    c[56] = 3;
    c[57] = 3;
    c[59] = 2;
    for (i = 0; i < 4; i++) {
      selllist[i][3] = 0;
    }

    for (i = 1; i < 320; i += 4) {
      if (old == 2 && !flag && lev > 0 && monbuf[i] == 112) {
        continue;
      }

      for (j = 0; j < 4; j++) {
        monbuf[i + (j++)] = 0;
      }
    }

    specbuf[31] = specbuf[32] = specbuf[33] = 0;
    specbuf[27] = 2;

    if (flag) {
      lev = 1;
    } else {
      lev = 0;
    }
  } else {
    lev = 1;
  }

  if (dungeon != 0 && new != 0) {
    specbuf[30 + dungeon] = 1;  /* gotta set the dirty flag on that dungeon */

    fhandle = Fopen(savname[dungeon + (3 * specbuf[30 + dungeon])], 0);
    if (fhandle < 0) {
      fhandle = Fcreate(savname[dungeon + (3 * specbuf[30 + dungeon])], 0);
    }

    if (fhandle < 0) {
      return 0;
    }

    if (Fwrite(fhandle,(long)12560,rumdata) < 12560 ||
      Fwrite(fhandle,(long)320,putbuf) < 320 ||
      Fwrite(fhandle,(long)480,trigval) < 480 ||
      Fwrite(fhandle,(long)40,specbuf) < 40 ||
      Fwrite(fhandle,(long)320,monbuf) < 320) {
          Fclose(fhandle);
          return 0;
    }

    Fclose(fhandle);
  }

  dungeon = c[59];

  // Load entirety of GRAFX.DAT into bitmap[]
  // This has 200 sprite and object images in it.
  fhandle = Fopen("GRAFX.DAT", 0);
  if (fhandle < 0) {
    return 0;
  }
  Fread(fhandle, (long)26000, bitmap[0]);
  Fclose(fhandle);

  // Now load the first part of FILL.DAT
  // It's actually 26000 bytes long
  fhandle = Fopen("FILL.DAT", 0);
  if (fhandle < 0) {
    return 0;
  }

  // Just read 41 sprites into "FILLPIC"
  Fread(fhandle, (long)5330, fillpic);

  if (dungeon == 0) {
    c[4] = 125;
    if(c[47] > 0 && c[14] > 0)
      c[47] = 0;
    c[41] = 0;
    Fseek((long)5330, fhandle, 0);
    Fread(fhandle, (long)5200, bitmap[21]);
  } else {
    c[4] = 81;
  }
  Fclose(fhandle);

  if(dungeon != 0)
    fhandle = Fopen(savname[dungeon + 3*specbuf[30 + dungeon]],0);
  else
    fhandle = Fopen(savname[0],0);

  if (fhandle < 0) {
    return(0);
  }

  d1 = specbuf[31];
  d2 = specbuf[32];
  d3 = specbuf[33];

  // Load initial rumdata into both arrays
  Fread(fhandle,(long)12560, new_rumdata);
  memcpy(rumdata, new_rumdata, 12560);
  // check_rumdata(); 

  Fread(fhandle,(long)320,putbuf);
  Fread(fhandle,(long)480,trigval);
  Fread(fhandle,(long)40,specbuf);
  if(dungeon != 0 && specbuf[30 + dungeon])
    Fread(fhandle,(long)320,monbuf);
  else {
    if(new || dungeon == 0)
      for(i=0;i<320;monbuf[i++] = 0);
    }
  if(flag) { /* got killed outside of kilkaney */
    monbuf[1] = 112;
    monbuf[2] = 30;
    monbuf[4] = 38;
    }
  Fclose(fhandle);
  dismax = specbuf[21];
  c[24] = specbuf[22];
  c[25] = specbuf[23];
  winroom = specbuf[24];
  c[30] = specbuf[25];
  police = specbuf[34];
  specbuf[31] = d1;
  specbuf[32] = d2;
  specbuf[33] = d3;
  if(old != 0 && new == 1)
    specbuf[30+old] = 1;  /* inform this dungeon the old one is dirty */
  if(new == 0 && lev != 0 ) {
    c[24] = 6;
    c[25] = 3;
    lev = specbuf[28] = 1;
    }
  for(i=1;i<4;i++) {
    if(curmon[i][0] == 1) 
      curmon[i][30] = -2;
    }
  for(i=4;i<12;i++) {
    c = curmon[i];
    for(j=0;j<60;j++)
      c[j] = 0;
    }
  for(i=0;i<20;i++)
    invtrig[i] = specbuf[i+1];
  crum = -1;
  return 1;
}  

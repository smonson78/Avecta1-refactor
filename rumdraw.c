#include "globals.h"
#include "trapaux.h"
#include "tacmode.h"
#include "dorep.h"
#include "blt.h"
#include "torches.h"
#include "caux.h"

#include "debug.h"

// Transfer data from bit to stor
// bit[] is in bitplanes (4 bitplane words for a group of 16 pixels)
// stor is all the words for first bitplane, then second bitplane, etc
void trans(int16_t *bit, int16_t *stor)
{
  int i, j;
  // 0..3
  for (i = 0; i < 4; i++) {
    // 0..15
    for (j = 0; j < 16; j++) {
      // original: *(stor+j+16*i) = *(bit+4*j+i);
      // *(stor + j + (16 * i)) = *(bit + (4 * j) + i);
      stor[j + (16 * i)] = bit[(4 * j) + i];
      // stor[] gets filled with 64 continuous words (one sprite)
      // but bit[] gets read in bitplane format
    }
  }
}

// First fill calls:
// colour 1, style 5, pattern 255
// colour 1, style 21, pattern 255
// (then the furniture gets drawn)


void setfill(int k)
{
  room_data_t *r = &new_rumdata[crum];

  if (k == 2) {
    // Clear background with pattern 1 (solid fill), colour 0 (background)
    vsf_interior(handle, 1);
    vsf_color(handle, 0);
    return;
  }

  int pattern = r->room_bg[k].room_bg_pattern;
  int style = r->room_bg[k].room_bg_style;
  int colour = r->room_bg[k].room_bg_colour;

  vsf_color(handle, colour);

  if (pattern != 0xff) {
    vsf_interior(handle, pattern);
    vsf_style(handle, style);
  } else {
    trans(fillpic[style], crudbuf);
    
    // Install 4 bitplanes from crudbuf to the AES pattern buffer
    vsf_udpat(handle, crudbuf, 4);

    // User-defined style = 4!
    vsf_interior(handle, 4);
  }
}

// Erase 16 text lines from (1, 1) to (1, 16)
void rumclear()
{
  for (int i = 1; i < 17; i++) {
    vs_curaddress(handle, i, 1);
    v_eeol(handle);
  }
  undorep();
}

// just normal abs()
int abs(int x)
{
  return x < 0 ? -x : x;
}

void click()
{
  int16_t status, x, y;
  do {
      vq_mouse(handle, &status, &x, &y);
  } while (status == 0);

  do {
      vq_mouse(handle, &status, &x, &y);
  } while (status != 0);
}  

void fillsq(int x, int y)
{
  int16_t pxy[4];
  pxy[0] = 16 * x;
  pxy[1] = 16 * y;
  pxy[2] = pxy[0] + 15;
  pxy[3] = pxy[1] + 15;
  
  // VDI filled rectangle
  vr_recfl(handle, pxy);
}

void rumdraw(char *pan)
{
  int top;
  int i, j, k;

  // c must be uint8_t or else we'll crash
  uint8_t *c;

  uint8_t *r = rumdata[crum];
  room_data_t *new_r = &new_rumdata[crum];

  // Set up room data by copying from rundata[crum][] to crumobj[]
  // Do 13 times, for a maximum of 13 objects in each room
  for (i = 0; i < 13; i++) {

    c = crumobj[i + 1]; // 1 to 13

    // Copy 9 bytes from rumdata into crumobj[i + 1] 0..8, which is all of the crumobj object
    for (j = 0; j < 9; j++) {
      //*(c + j) = *(r + 31 + (9 * i) + j);
      //c[j] = r[31 + (9 * i) + j];
      c[j] = new_r->object[i].unknown[j];
    }
  }

  // 5 things, 14..18
  for (i = 14; i < 19; i++) {
    c = crumobj[i];
    // for (j = 0; j < 9; c[(j++]) = 0) {
    // }

    // Blank it out
    for (j = 0; j < 9; j++) {
      c[j] = 0;
    }
  }

  // This is blanking out the entire zline array
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 8; j++) {
        for (k = 0; k < 7; k++) {
          zline[i][j][k] = 0;
        }
    }
  }

  // 25 things, which is all of triglist
  // for(i=0;i<25;triglist[i++] = 0)
  //   ;
  for (i = 0; i < 25; i++) {
    triglist[i] = 0;
  }
    
  // objnum = 0;
  // 13 things, 1 to 13
  for (i = 1; i < 14; i++) {
    c = crumobj[i]; 
    if (c[0] != 0) {
      // objnum++;
      if (c[2] != 0) {
        triglist[0]++;
        triglist[i] = c[2];
      }
    }
  }

  // Something about invisible traps
  for (i = 0; i < 4; i++) {
    c = curmon[i];
    
    if (c[45] > 40 && c[45] < 81) {
      invtrap(c[45]);
    }
    
    if (c[46] > 40 && c[46] < 81) {
      invtrap(c[46]);
    }
    
    c = invnpc[i];
    
    if (c[0] == 0) {
      continue;
    }

    for (j = 1; j < 14; j++) {
      if (c[j] == 0) {
        continue;
      }
      invtrap(c[j]);
    }
  }

  int x, y;

  // Get some coords
  // xold in lower 4 bits
  int xold = rumdata[crum][0] % 16;
  // yold in upper 4 bits
  int yold = rumdata[crum][0] / 16;

  int x1 = xold;
  int y1 = yold;
  i = 1;

  int x2, y2;
  do {
    // Get another set of coords from byte 1 onwards up to a maximum of 16
    x2 = rumdata[crum][i] % 16;
    y2 = rumdata[crum][i] / 16;

    // Get distance from last coord to this one
    int stepx = x2 - x1;
    int stepy = y2 - y1;

    // 0 stays
    // positive number turns into n/n AKA 1
    // negative number turns into n/-n AKA -1
    stepx = (stepx != 0 ? stepx / abs(stepx) : 0);
    stepy = (stepy != 0 ? stepy / abs(stepy) : 0);

    // Move toward the target goal (x2, y2) one square at a time, setting zline for this square to 2
    while (x1 != x2 || y1 != y2) {
      zline[x1][y1][0] = 2;
      x1 += stepx;
      y1 += stepy;
    }

    // Now make this the "previous" coord
    x1 = x2;
    y1 = y2;

    i++;

    // Stop if we return to the original position in r[0].
  } while ((x2 != xold || y2 != yold) && i <= 16);

  top = -1;
  // Iterate over the 8x16 grid
  for (j = 0; j < 8; j++) {
    int open = 0;

    for (i = 0; i < 16; i++) {

      // There is one "zline" entry for each square
      uint8_t *z = zline[i][j];
      if (top == -1 && z[0] != 0) {
        top = j;
      }

      // We're looking at the square above to see if it's 2, but THIS square isn't 2.
      if (i > 0 && zline[i-1][j][0] == 2 && z[0] != 2) {
        open = !open;
        
        k = 0;
        while (j - k > 0) {
          if (j == 0)
            break;
          if (k > 0 && zline[i][j-k][0] != 2)
            break;
          else
            k++;
        }

        if (k == 1) { 
          if (zline[i][j-1][0] != 2)
            open = zline[i][j-1][0];
          else
            open = 1;
        } else {
          if (j == k && zline[i][j-k][0] == 2)
            open = 1;
          else
            open = !zline[i][j-k][0];
        }
      }

      if (j == top || j == 7) {
        open = 0;
      }

      if (z[0] != 2) {
        z[0] = open;
      }
    }   
  }

  // Clear room area maybe
  rumclear();

  // This seems to want to draw in some plain background stuff
  //if (*(r + 30)) {
  if (new_r->room_has_bg) {

    // We're just doing 0 and 1 - the two types of background tiles for this room.
    // Loop 0 only does something indoors
    // Loop 1 does something both indoors and outdoors
    for (k = 0; k < 2; k++) {
      
      // Set up the AES custom fill pattern using the tile defined in rumdata
      setfill(k);

      // Now we're going to go 8 steps vertically and 16 steps horizontally
      for (j = 0; j < 8; j++) {
        for (i = 0; i < 16; i++) {
          // On the first iteration of the outermost loop, where k is 0, this inner loop
          // only does something if we're inside.

          // I assume zline has ended up with the background tiles in the room that you can actually see from where you are
          // Relevant values in [0] here are either 2 or 1
          if (zline[i][j][0] == 2 - k && (!outside || k != 0) ) {
            // Draw the background tile using the AES custom fill pattern
            fillsq(i, j);
          }
        }
      }
    }
  } else {
    // or alternately, to just blank the whole area. Fill pattern 2 here means solid black.
    setfill(2);
    for (j = 0; j < 8; j++) {
      for (i = 0; i < 16; i++)
        fillsq(i, j);
      }
  }

  // Not sure what the point of this here is.
  vsf_interior(handle, 1); // Solid
  vsf_style(handle, 0);    // Blank (probably colour 1)
  vsf_color(handle, 0);    // Black colour - not really used?
  v_bar(handle, vxy);   // (255,0) to (255, 199) - a vertical line between the play area and the menu
  v_bar(handle, lxy);   // (0, 127) to (244, 128) - a horizontal line 2 pixels high probably above the text area

  // Draw room objects
  // 1..13
  for (i = 1; i < 14; i++) {

    // Get object i data
    c = crumobj[i];

    if (c[0] != 0) {
      int obj_x = c[6];
      int obj_y = c[7];

      if (c[8]) {
        // if (r[30]) {
        if (new_r->room_has_bg) {
          // Draw the sprite for room object
          blt(bitmap[c[0]], obj_x * 16, obj_y *16);
        }

        zline[obj_x][obj_y][1] = i; // put the object ID into position 1
      } else {
        zline[obj_x][obj_y][3] = i; // put the object ID into position 3
      }
    }
  }

  // 5 more bytes in rumdata[crum][148...152]
  // for (i = 148; i < 153; i++) {
  for (i = 0; i < 5; i++) {

    // r[i] but with a better name
    int thing = new_r->room_unknown_3[i];
    
    // we're doing 0..4 now
    if (thing == 0) {
      // Nothing there?
      continue;
    }

    // Now we're going to start on 1, and go up 3 each time. There is a 3-byte structure there.
    // For some reason we're going to keep checking right past the end of the structure into 
    // other unrelated data like background colours.
    // Perhaps let's just limit ourselves to the 5 that we know exist.
    // We're ONLY looking for this in rumdata[0] rather than "r".
    // Probably rumdata[0] is a different data structure!

    // j = 1;
    // while (j < 157 && (rumdata[0][j] != thing || rumdata[0][j + 2] != crum)) {
    j = 0;
    while (j < 52 && (room_zero.unknown2[j].thing_id != thing || room_zero.unknown2[j].room_id != crum)) {
      j += 1;
    }

    // Didn't find the thing we wanted - so kill whatever this thing was.
    //if (j >= 157) {
    if (j >= 52) {
      r[148 + i] = 0;
      new_r->room_unknown_3[i] = 0;
      continue;
    }

    // Meaning offset 14...18
    //c = crumobj[i - 134];
    c = crumobj[14 + i];
    // Get some coords
    // x = rumdata[0][j + 1] % 16;
    // y = rumdata[0][j + 1] / 16;
    x = room_zero.unknown2[j].thing_xy % 16;
    y = room_zero.unknown2[j].thing_xy / 16;

    // Now we know the room 0 structure is:
    // 0 - the unknown thing ID
    // 1 - XY coords
    // 2 - room number
    // We may be looking at rumdata[0] as a [52][3] of these. 

    // They've walked off the board apparently
    if (x > 15 || y > 7) {
      continue;
    }

    c[0] = new_r->room_unknown_3[i];
    //zline[x][y][1] = i - 134; // 14..18
    zline[x][y][1] = 14 + i; // 14..18
    invtrap(c[0]);
    c[1] = 1;
    c[2] = 0;
    c[3] = 0;
    c[4] = c[0];
    c[5] = -1;
    c[6] = x;
    c[7] = y;
    c[8] = 1;

    //if (rumdata[crum][30]) {
    if (new_rumdata[crum].room_has_bg) {
      drawsq(x, y);
    }
  }

  if (specbuf[35] != 0) {
    x = specbuf[36] % 16;
    y = specbuf[36] / 16;

    // This is indicating an area of one space around whatever's at (x, y)
    for(i = x - 1; i < x + 2; i++) {
      for (j = y - 1; j < y + 2; j++) {
        uint8_t *z = zline[i][j];

        if (z[0] == 1) {
          z[5] += 1;
          z[6] = 1;
          drawsq(i, j);
        }
      }
    }
  }

  for (i = 1; i < 12; i++) {
    c = curmon[i];

    if (c[0] == 1 && c[30] == crum) {
      x1 = c[24];
      y1 = c[25];
      j = xmon(c[30], c[4], x1, y1);
      
      if (c[4] == 110) {
        c[4] = 116 + rnd(3);
      }
      
      if (c[36] == 0) {
        if (zline[x1][y1][2] != 0) {
          c[0] = 0;
          continue;
        }
      }

      if (*(pan + i - 1) == 1) {
        x1 = rnd(13);
        y1 = rnd(7);

        uint8_t *z = zline[x1][y1];
        k = 0;
        while (k < 100 && (z[0] != 1 || z[1] != 0 || z[2] != 0)) {
          x1 = rnd(13);
          y1 = rnd(7);
          z = zline[x1][y1];
          k++;
        }

        if (k == 100) {
          c[0] = 0;
          continue;
        }

        c[24] = x1;
        c[25] = y1;
        c[36] = 0;
      }

      c[15] = 8;
      c[16] = 1;
      c[18] = 0;
      c[39] = i;

      zline[x1][y1][2] = i;

      drawman(i, c[24], c[25]);

      if (i > 3 && c[31] < 4) {
        mode = 1; 
      }
    }
  }
  
  check_rumdata("rumdraw");
}

#include "tos_compat.h"

#include <stdint.h>
#include <string.h>
#include "globals.h"
#include "mouse_off.h"
#include "mouse_on.h"
#include "gemdefs.h"
#include "startaux.h"

#include "debug.h"

// We're either loading 30 sprites into bitmap[21] to bitmap[50] or 40 sprites into bitmap[21] to bitmap[60]
int load_outside_fill_tiles(int total) {
  int h = Fopen("FILL.DAT", 0);
  if (h < 0) {
    return 1;
  }

  // Skip 41 sprites
  Fseek(41 * sizeof(sprite_t), h, 0);
  
  // Load 30 sprites in bitmap[21 to 50]
  // This overwrites indoor sprite data from the first staircase sprite until the second zany 
  // square sprite.
  Fread(h, total * sizeof(sprite_t), &bitmap[21]);
  Fclose(h);
  endianness_fix(&bitmap[21], total * sizeof(sprite_t) / 2, 16);

  return 0;
}


// FILL.DAT
// It's actually 26000 bytes long
// Breakdown of FILL.DAT:
// 0 -      = empty
// 1 - 23   = floor/wall textures
// 24       = garbage
// 25 - 27  = rivers
// 28 - 33  = empty
// 34       = lake water
// 35 - 40  = empty
// ---------- this is the amount read initially as "fillpic"
// 41 - 47  = rivers
// 48       = lake water
// 49 - 51  = empty
// 52 - 59  = ocean water
// 53       = empty
// 54       = terrain graphic
// 55 - 56  = castles
// 57 - 58  = rivers
// 59       = terrain graphics
// 60       = cave entrance dot
// ---------- up to here is the second set of 40 sprites ready into bitmap[21+]
// 61 - 64  = terrain graphics
// 65       = empty
// 66       = terrain graphics
// 67       = empty
// 68       = terrain graphics
// 69       = empty
// 70       = terrain graphics
// 71 - 198 = empty
// 199      = a plus sign on a black background
int load_inside_fill_tiles(int ) {
  int h = Fopen("FILL.DAT", 0);
  if (h < 0) {
    return 1;
  }
  
  // Read 41 sprites into "FILLPIC"
  Fread(h, sizeof(fillpic), fillpic);
  Fclose(h);
  endianness_fix(fillpic, sizeof(fillpic) / 2, 16);

  return 0;
}

int load_inside_sprites(int total) {
  int h = Fopen("GRAFX.DAT", 0);
  if (h < 0) {
    return 1;
  }

  // We are not always reading all of the 200 images here for some reason
  Fread(h, total * sizeof(sprite_t), bitmap);
  Fclose(h);
  endianness_fix(bitmap, total * sizeof(sprite_t) / 2, 16);

  return 0;
}


// Load a save game
// When flow == 1, save the game
// When flow == 2, restore a saved game
int fileio(int flow) {
  int16_t button;
  int i, j, filehandle;
  
  //uint8_t *r = rumdata[crum];
  room_data_t *r = &new_rumdata[crum];
  
  uint8_t check,*c;
  DTA dta;
  char filename[13];
  
  // Clear the screen
  vs_curaddress(handle, 1, 1);
  v_eeos(handle);

  // Use a GEM file selector to get the save file name
  mouse_on();
  strcpy(filename, "________.___");
  fsel_input(path, filename, &button);
  mouse_off();

  // Clear the screen again
  vs_curaddress(handle, 1, 1);
  v_eeos(handle);

  init(1);

  if (button == 0) {
    return -2;
  }

  debug_printf("fileio %d\n", flow);

  if (flow == 2) {
    // Loading a saved game
    filehandle = Fopen(filename, 0);

    // File not found
    if (filehandle < 0) {
      return -1;
    }

    Fsetdta(&dta);
    Fsfirst(filename, 0);
    if (dta.d_length != 14546) {
      return -1;
    }
  } else if (flow == 1) {
    // Saving the game
    // In case the file already exists:
    filehandle = Fopen(filename, 1);
    if (filehandle < 0) {
      // ..otherwise, try creating it
      filehandle = Fcreate(filename, 0);
      debug_printf("-- filehandle is ultimately %d\n", filehandle);
    }
  } else {
    // there are only 2. This should be split into 2 functions.
    return 0;
  }

  // An error creating the file
  if (filehandle < 0) {
    return -1;
  }

  // SAVE GAME
  if (flow == 1) {
    specbuf[26] = outside;
    specbuf[27] = curmon[0][59];
    specbuf[28] = lev;
    specbuf[29] = new;
    specbuf[34] = police;
    specbuf[37] = fromout;

    // Copy all objects from current room into room data array
    for (i = 0; i < 13; i++) {
      c = crumobj[i + 1];
      for (j = 0; j < 9; j++) {
        //r[31 + (9 * i) + j] = c[j];
        r->object[i].unknown[j] = c[j];
      }
    }

    // Writes 14546 bytes to save the game.
    if (
      // Room zero, then the other rooms
      Fwrite(filehandle, (long)157, &room_zero) < 157 || 
      Fwrite(filehandle, (long)12403, &new_rumdata[1]) < 12403 || 
      Fwrite(filehandle, (long)320, putbuf) < 320 ||
      Fwrite(filehandle, (long)480, trigval) < 480 ||
      Fwrite(filehandle, (long)40, specbuf) < 40 ||
      Fwrite(filehandle, (long)720, curmon) < 720 ||
      Fwrite(filehandle, (long)6, pursuit) < 6  ||
      Fwrite(filehandle, (long)20, pname) < 20 ||
      Fwrite(filehandle, (long)80, invnpc) < 80 ||
      Fwrite(filehandle, (long)320, monbuf) < 320 ) {
      Fclose(filehandle);
      return -1;
    } else {
      Fclose(filehandle);
    }
  }

  // LOAD GAME
  if (flow == 2) {
    Fseek((long)13387, filehandle, 0);
    Fread(filehandle, (long)1, &check);
    if (curmon[0][59] != check && dungeon != 2) {
      Fclose(filehandle);
      return 0;
    } else {
      Fseek((long)0, filehandle, 0);
    }
    
    //if (Fread(filehandle,(long)12560,new_rumdata) < 12560 || 
    if (Fread(filehandle, (long)157, &room_zero) < 157 ||
      Fread(filehandle, (long)12403, &new_rumdata[1]) < 12403 ||
      Fread(filehandle, (long)320, putbuf) < 320 ||
      Fread(filehandle, (long)480, trigval) < 480 ||
      Fread(filehandle, (long)40, specbuf) < 40 ||
      Fread(filehandle, (long)720, curmon) < 720 ||
      Fread(filehandle, (long)6, pursuit) < 6  ||
      Fread(filehandle, (long)20, pname) < 20 ||
      Fread(filehandle, (long)80, invnpc) < 80 ||
      Fread(filehandle, (long)320, monbuf) < 320 ) {
      Fclose(filehandle);
      return -1;
    } else {
      memcpy(new_curmon, curmon, sizeof(curmon));

      lev = specbuf[28] = 1;
      dungeon = specbuf[27];
      fromout = specbuf[37];
      police = specbuf[34];
      outside = specbuf[26];
      crum = curmon[0][30];
      
      for (i = 0; i < 20; i++) {
        invtrig[i] = specbuf[i + 1];
      }

      Fclose(filehandle);

      if (outside || fromout) {
        specbuf[32] = 1;
        if (load_outside_fill_tiles(30)) {
          return 0;
        }
      } else {
        // Just the first 71
        if (load_inside_sprites(71)) {
          return 0;
        }
      }            
    }

    check_curmon("fileio after loading data");

    // Set up room zero and legacy data after loading files
    //memcpy(rumdata, &room_zero, 157);
    // Special room zero data
    //memcpy(rumdata + 1, &new_rumdata[1], 12403);
    //check_rumdata("fileio");
  }

  return 1;
}

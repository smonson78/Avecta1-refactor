#include "tos_compat.h"

#include <stdint.h>
#include <libc.h>
#include "globals.h"
#include "room.h"

#include "debug.h"

// Compare the two rumdata arrays to ensure correctness
void check_rumdata(char *location) {
  // for (int r = 0; r < 80; r++) {
  //   room_data_t *new_room = &new_rumdata[r];
    
  //   for (int b = 0; b < sizeof(room_data_t); b++) {
  //     int old = rumdata[r][b];
  //     int new = ((uint8_t *)new_room)[b];

  //     if (new != old) {
  //       // Clear screen
  //       printf("\eE");
  //       printf("Inconsistency in rumdata!\n");
  //       printf("Location: %s\n", location);
  //       printf("Room %d, byte %d\n", r, b);
  //       printf("rumdata: 0x%02x vs new_rumdata: 0x%02x\n", old, new);
  //       Cconin();
  //     }
  //   }
  // }
}

// Compare the two curmon arrays to ensure correctness
void check_curmon(char *location) {
  for (int c = 0; c < 12; c++) {
     curmon_t *new_monster = &new_curmon[c];
    
     for (int b = 0; b < sizeof(curmon_t); b++) {
      int old = curmon[c][b];
      int new = ((uint8_t *)new_monster)[b];

      
      if (new != old) {
        // Clear screen
        debug_printf("!!!!!!! Inconsistency in curmon!\n");
        debug_printf("Location: %s\n", location);
        debug_printf("curmon number %d, byte %d\n", c, b);
        debug_printf("curmon: 0x%02x vs new_curmon: 0x%02x\n", old, new);
        debug_printf("Press any key to continue\n");
        // Don't actually halt the game yet
        // Cconin();
      }
    }
  }
}
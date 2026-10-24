#include "curmon.h"
#include <libc.h>
#include "tos_compat.h"

// Monster instances
curmon_t new_curmon[12];

// Set up default data for curmon array
void init_curmon() {
  // Set all to zero
  memset(&new_curmon, 0, sizeof(new_curmon));

  // Initialise the player's data
  new_curmon[0].in_use = 1;
  new_curmon[0].unknown1[0] = 4;
  new_curmon[0].unknown1[1] = 50;
  //new_curmon[0].unknown1[2] = 0;
  new_curmon[0].unknown1[3] = 81;
  new_curmon[0].unknown1[4] = 5;
  new_curmon[0].unknown1[5] = 60;
  new_curmon[0].unknown1[6] = 8;
  //new_curmon[0].unknown1[7] = 0;
  //new_curmon[0].unknown1[8] = 0;
  //new_curmon[0].unknown1[9] = 0;
  new_curmon[0].unknown1[10] = 4;
  new_curmon[0].unknown1[11] = 3;
  new_curmon[0].unknown1[12] = 90;
  new_curmon[0].unknown1[13] = 10;
}